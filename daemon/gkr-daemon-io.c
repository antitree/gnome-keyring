/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gnome-keyring-daemon-io.c - handles i/o from the clients

   Copyright (C) 2003 Red Hat, Inc

   Gnome keyring is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
  
   Gnome keyring is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Author: Alexander Larsson <alexl@redhat.com>
*/

#include "config.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#if defined(HAVE_GETPEERUCRED)
#include <ucred.h>
#endif

#include "gkr-daemon.h"
#include "mkdtemp.h"

#include "common/gkr-async.h"
#include "common/gkr-buffer.h"

#include "keyrings/gkr-keyrings.h"

#include "library/gnome-keyring.h"
#include "library/gnome-keyring-memory.h"
#include "library/gnome-keyring-private.h"
#include "library/gnome-keyring-proto.h"

#include "ui/gkr-ask-daemon.h"

#ifndef HAVE_SOCKLEN_T
#define socklen_t int
#endif

typedef enum {
	GNOME_CLIENT_STATE_CREDENTIALS,
	GNOME_CLIENT_STATE_READ_DISPLAYNAME,
	GNOME_CLIENT_STATE_READ_PACKET,
	GNOME_CLIENT_STATE_COLLECT_INFO,
	GNOME_CLIENT_STATE_REQUEST_ACCESS,
	GNOME_CLIENT_STATE_EXECUTE_OP,
	GNOME_CLIENT_STATE_WRITE_REPLY
} GnomeKeyringClientStates;

typedef struct {
	GkrAsyncWorker *worker;
	int sock;

	GnomeKeyringApplicationRef *app_ref;

	GkrBuffer input_buffer;
	GkrBuffer output_buffer;
} GnomeKeyringClient;

static char tmp_dir[1024] = { 0, };
static char socket_path[1024] = { 0, };

#if 0
#define debug_print(x) g_print x
#else
#define debug_print(x)
#endif

static gboolean
set_local_creds (int fd, gboolean on)
{
  gboolean retval = TRUE;

#if defined(LOCAL_CREDS) && !defined(HAVE_CMSGCRED)
  int val = on ? 1 : 0;
  if (setsockopt (fd, 0, LOCAL_CREDS, &val, sizeof (val)) < 0)
    {
      g_warning ("Unable to set LOCAL_CREDS socket option on fd %d\n", fd);
      retval = FALSE;
    }
#endif

  return retval;
}

static GnomeKeyringApplicationRef*
application_ref_new_from_pid (pid_t pid)
{
	GnomeKeyringApplicationRef *app_ref;

	app_ref = g_new0 (GnomeKeyringApplicationRef, 1);

#if defined(__linux__) || defined(__FreeBSD__)
	g_assert (pid > 0);
	{
		char *buffer;
		int len;
		char *path = NULL;
		
#if defined(__linux__)
		path = g_strdup_printf ("/proc/%d/exe", (gint)pid);
#elif defined(__FreeBSD__)
		path = g_strdup_printf ("/proc/%d/file", (gint)pid);
#endif
		buffer = g_file_read_link (path, NULL);
		g_free (path);

		len = (buffer != NULL) ? strlen (buffer) : 0;
		if (len > 0) {
			app_ref->pathname = g_malloc (len + 1);
			memcpy (app_ref->pathname, buffer, len);
			app_ref->pathname[len] = 0;
		}
		g_free (buffer);
	}
#endif

	return app_ref;
}

static gboolean
read_unix_socket_credentials (int fd,
			      pid_t *pid,
			      uid_t *uid)
{
	struct msghdr msg;
	struct iovec iov;
	char buf;
	int ret;
	
#if defined(HAVE_CMSGCRED) || defined(LOCAL_CREDS)
	/* Prefer CMSGCRED over LOCAL_CREDS because the former provides the
	 * remote PID. */
#if defined(HAVE_CMSGCRED)
	struct cmsgcred *cred;
	const size_t cmsglen = CMSG_LEN (sizeof (struct cmsgcred));
	const size_t cmsgspace = CMSG_SPACE (sizeof (struct cmsgcred));
#else /* defined(LOCAL_CREDS) */
	struct sockcred *cred;
	const size_t cmsglen = CMSG_LEN (sizeof (struct sockcred));
	const size_t cmsgspace = CMSG_SPACE (sizeof (struct sockcred));
#endif
	union {
		struct cmsghdr hdr;
		char cred[cmsgspace];
	} cmsg;
#endif
	
	*pid = 0;
	*uid = 0;
	
	/* If LOCAL_CREDS are used in this platform, they have already been
	 * initialized by init_connection prior to sending of the credentials
	 * byte we receive below. */
	
	iov.iov_base = &buf;
	iov.iov_len = 1;
	
	memset (&msg, 0, sizeof (msg));
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	
#if defined(HAVE_CMSGCRED) || defined(LOCAL_CREDS)
	memset (&cmsg, 0, sizeof (cmsg));
	msg.msg_control = (caddr_t) &cmsg;
	msg.msg_controllen = cmsgspace;
#endif

 again:
 	gkr_async_begin_concurrent ();
 	
 		ret = recvmsg (fd, &msg, 0);
 	
 	gkr_async_end_concurrent ();
 	
 	if (ret < 0) {
		if (errno == EINTR) {
			goto again;
		}
		
		g_warning ("Failed to read credentials byte");
		return FALSE;
	}
	
	if (buf != '\0') {
		g_warning ("Credentials byte was not nul");
		return FALSE;
	}

#if defined(HAVE_CMSGCRED) || defined(LOCAL_CREDS)
	if (cmsg.hdr.cmsg_len < cmsglen || cmsg.hdr.cmsg_type != SCM_CREDS) {
		g_warning ("Message from recvmsg() was not SCM_CREDS\n");
		return FALSE;
	}
#endif

	{
#ifdef SO_PEERCRED
		struct ucred cr;   
		socklen_t cr_len = sizeof (cr);
		
		if (getsockopt (fd, SOL_SOCKET, SO_PEERCRED, &cr, &cr_len) == 0 &&
		    cr_len == sizeof (cr)) {
			*pid = cr.pid;
			*uid = cr.uid;
		} else {
			g_warning ("Failed to getsockopt() credentials, returned len %d/%d\n",
				   cr_len, (int) sizeof (cr));
			return FALSE;
		}
#elif defined(HAVE_CMSGCRED)
		cred = (struct cmsgcred *) CMSG_DATA (&cmsg.hdr);
		*pid = cred->cmcred_pid;
		*uid = cred->cmcred_euid;
#elif defined(LOCAL_CREDS)
		cred = (struct sockcred *) CMSG_DATA (&cmsg.hdr);
		*pid = 0;
		*uid = cred->sc_euid;
		set_local_creds(fd, FALSE);
#elif defined(HAVE_GETPEERUCRED)
		ucred_t *uc = NULL;

		if (getpeerucred (fd, &uc) == 0) {
			*pid = ucred_getpid (uc);
			*uid = ucred_geteuid (uc);
			ucred_free (uc);
		} else {
			g_warning ("getpeerucred() failed: %s", strerror (errno));
			return FALSE;
		}
#else /* !SO_PEERCRED && !HAVE_CMSGCRED */
		g_warning ("Socket credentials not supported on this OS\n");
		return FALSE;
#endif
	}

	return TRUE;
}

static gboolean
yield_and_read_all (int fd, guchar *buf, int len)
{
	int all = len;
	int res;
	
	while (len > 0) {
		
		/* Is this worker stopping? */
		if (gkr_async_is_stopping ())
			return -1;
			
		/* Don't block other threads during the read */
		gkr_async_begin_concurrent ();
		
			res = read (fd, buf, len);
			
		gkr_async_end_concurrent ();
		
		if (res <= 0) {
			if (errno == EAGAIN && errno == EINTR)
				continue;

			g_warning ("couldn't read %u bytes from client: %s", all, 
			           res < 0 ? g_strerror (errno) : "");
			return FALSE;
		} else  {
			len -= res;
			buf += res;
		}
	}
	
	return TRUE;
}

static gboolean
yield_and_write_all (int fd, const guchar *buf, int len)
{
	int all = len;
	int res;
	
	while (len > 0) {
		
		/* Is this worker stopping? */
		if (gkr_async_is_stopping ())
			return -1;
			
		/* Don't block other threads during the read */
		gkr_async_begin_concurrent ();

			res = write (fd, buf, len);
			
		gkr_async_end_concurrent ();
		
		if (res <= 0) {
			if (errno == EAGAIN && errno == EINTR)
				continue;

			g_warning ("couldn't write %u bytes to client: %s", all, 
			           res < 0 ? g_strerror (errno) : "");
			return FALSE;
		} else  {
			len -= res;
			buf += res;
		}
	}
	
	return TRUE;
}

static gboolean
read_packet_with_size (GnomeKeyringClient *client)
{
	int fd;
	guint32 packet_size;

	fd = client->sock;
	
	gkr_buffer_resize (&client->input_buffer, 4);
	if (!yield_and_read_all (fd, client->input_buffer.buf, 4))
		return FALSE;

	if (!gkr_proto_decode_packet_size (&client->input_buffer, &packet_size) ||
	    packet_size < 4) {
	    	g_warning ("invalid packet size from client");
		return FALSE;
	}

	gkr_buffer_resize (&client->input_buffer, packet_size + 4);
	if (!yield_and_read_all (fd, client->input_buffer.buf + 4, packet_size - 4))
		return FALSE;

	return TRUE;
}

static void
close_fd (gpointer data)
{
	int *fd = (int*)data;
	g_assert (fd);

	/* If we're waiting anywhere this makes the thread stop */
	shutdown (*fd, SHUT_RDWR);
}

static gpointer
client_worker_main (gpointer user_data)
{
	GnomeKeyringClient *client = (GnomeKeyringClient*)user_data;
	GnomeKeyringOpCode op;
	GkrKeyringRequest req;
	pid_t pid;
	uid_t uid;
	char *str;

	/* This helps any reads wakeup when this worker is stopping */
	gkr_async_register_cancel (close_fd, &client->sock);
	
	/* 1. First we read and verify the client's user credentials */	
	debug_print (("GNOME_CLIENT_STATE_CREDENTIALS %p\n", client));
	if (!read_unix_socket_credentials (client->sock, &pid, &uid))
		return NULL;
	if (getuid() != uid) {
		g_warning ("uid mismatch: %u, should be %u\n", (guint)uid, (guint)getuid());
		return NULL;
	}
	client->app_ref = application_ref_new_from_pid (pid);


	/* 2. Read the connecting application display name */
	debug_print (("GNOME_CLIENT_STATE_READ_DISPLAYNAME %p\n", client));
	if (!read_packet_with_size (client))
		return NULL;
	debug_print (("read packet\n"));
	if (!gkr_proto_get_utf8_string (&client->input_buffer, 4, NULL, &str))
		return NULL;
	if (!str)
		return NULL;
	debug_print (("got name: %s\n", str));
	client->app_ref->display_name = str;


	/* 3. Now read the actual packet of the operation */	
	debug_print (("GNOME_CLIENT_STATE_READ_PACKET %p\n", client));
	if (!read_packet_with_size (client))
		return NULL;
	debug_print (("read packet, size: %d\n", client->input_buffer.len));


	/* 4. Next decode the operation, and execute the operation */	
	debug_print (("GNOME_CLIENT_STATE_EXECUTE_OP %p\n", client));
	if (!gkr_proto_decode_packet_operation (&client->input_buffer, &op))
		return NULL;
	if (op < 0 || op >= GNOME_KEYRING_NUM_OPS)
		return NULL;
	g_assert (keyring_ops[op]);

	/* Make sure keyrings in memory are up to date before doing anything */
	gkr_keyrings_update ();

	gkr_buffer_init_full (&client->output_buffer, 128, g_realloc);
	
	/* Add empty size */
	gkr_buffer_add_uint32 (&client->output_buffer, 0);
		
	memset (&req, 0, sizeof (req));
	req.app_ref = client->app_ref;
		
	if (!(keyring_ops[op])(&client->input_buffer, &client->output_buffer, &req))
		return NULL;
		
	if (!gkr_buffer_set_uint32 (&client->output_buffer, 0,
	                            client->output_buffer.len))
		return NULL;


	/* 5. Write the reply back out */
	debug_print (("GNOME_CLIENT_STATE_WRITE_REPLY %p\n", client));
	debug_print (("writing %d bytes\n", client->output_buffer.len));
	if (!yield_and_write_all (client->sock, client->output_buffer.buf,
                                  client->output_buffer.len))
		return NULL;

	/* All done */
	return NULL;
}

static void
client_worker_done (GkrAsyncWorker *worker, gpointer result, gpointer user_data)
{
	GnomeKeyringClient *client = (GnomeKeyringClient*)user_data;

	gkr_buffer_uninit (&client->input_buffer);
	gkr_buffer_uninit (&client->output_buffer);

	if (client->app_ref != NULL) {
		gnome_keyring_application_ref_free (client->app_ref);
	}

	close (client->sock);
	g_free (client);
}

static void
client_new (int fd)
{
	GnomeKeyringClient *client;

	debug_print (("client_new(fd:%d) -> %p\n", fd, client));
	
	client = g_new0 (GnomeKeyringClient, 1);
	client->sock = fd;
	
	/* 
	 * We really have no idea what operation the client will send, 
	 * so we err on the side of caution and use secure memory in case
	 * passwords or secrets are involved.
	 */  
	gkr_buffer_init_full (&client->input_buffer, 128, gnome_keyring_memory_realloc);

	client->worker = gkr_async_worker_start (client_worker_main, 
	                                         client_worker_done, client);
	g_assert (client->worker);
	
	/* 
	 * The worker thread is tracked in a global list, and is guaranteed to 
	 * be cleaned up, either when it exits, or when the application closes.
	 */
}

static gboolean
accept_client (GIOChannel *channel, GIOCondition cond,
               gpointer callback_data)
{
	int fd;
	int new_fd;
	struct sockaddr_un addr;
	socklen_t addrlen;
  
	fd = g_io_channel_unix_get_fd (channel);
	
	addrlen = sizeof (addr);
	new_fd = accept(fd, (struct sockaddr *) &addr, &addrlen);
	
	if (new_fd >= 0) 
		client_new (new_fd);
	return TRUE;
}

void
gkr_daemon_io_cleanup_socket_dir (void)
{
	if(*socket_path)
		unlink (socket_path);
	if(*tmp_dir)
		rmdir (tmp_dir);
}

gboolean
gkr_daemon_io_create_master_socket (const char **path)
{
	gboolean have_path = FALSE;
	int sock;
	struct sockaddr_un addr;
	GIOChannel *channel;
	gchar *tmp_tmp_dir = NULL;
	
	/* 
	 * When run under control of unit tests, we let the parent process
	 * pass in the socket path that we're going to create our main socket on.
	 */
	
#ifdef WITH_TESTS
	const gchar* env = g_getenv ("GNOME_KEYRING_TEST_PATH");
	if (env && *env) {
		g_strlcpy (tmp_dir, env, sizeof (tmp_dir));
		have_path = TRUE;
	} 
#endif /* WITH_TESTS */	
	
	if (!have_path) {
		/* Create private directory for agent socket */
		tmp_tmp_dir = g_build_filename (g_get_tmp_dir (), "keyring-XXXXXX", NULL);
		strncpy (tmp_dir, tmp_tmp_dir, sizeof (tmp_dir));
		if (mkdtemp (tmp_dir) == NULL) {
			perror ("mkdtemp: socket dir");
			return FALSE;
		}
	}

	snprintf (socket_path, sizeof (socket_path), "%s/socket", tmp_dir);
	if (have_path)
		unlink (socket_path);
	
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		gkr_daemon_io_cleanup_socket_dir ();
		return FALSE;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy (addr.sun_path, socket_path, sizeof (addr.sun_path));
	if (bind (sock, (struct sockaddr *) & addr, sizeof (addr)) < 0) {
		perror ("bind");
		gkr_daemon_io_cleanup_socket_dir ();
		return FALSE;
	}
	
	if (listen (sock, 128) < 0) {
		perror ("listen");
		gkr_daemon_io_cleanup_socket_dir ();
		return FALSE;
	}

        if (!set_local_creds (sock, TRUE)) {
		close (sock);
		gkr_daemon_io_cleanup_socket_dir ();
		return FALSE;
	}

	g_free (tmp_tmp_dir);
	channel = g_io_channel_unix_new (sock);
	g_io_add_watch (channel, G_IO_IN | G_IO_HUP, accept_client, NULL);
	g_io_channel_unref (channel);
	
	*path = socket_path;
	return TRUE;
}

/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* gkr-pk-object.h - A base class for PK objects

   Copyright (C) 2007 Stefan Walter

   The Gnome Keyring Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Keyring Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Author: Stef Walter <stef@memberwebs.com>
*/

#ifndef __GKR_PK_OBJECT_H__
#define __GKR_PK_OBJECT_H__

#include <glib-object.h>
#include <pkcs11/pkcs11.h>

#include "common/gkr-unique.h"

G_BEGIN_DECLS

/* A reason for accessing and/or loading an object */
typedef enum {
	GKR_PK_OBJECT_REASON_UNKNOWN,
	GKR_PK_OBJECT_REASON_IMPORT
} GkrPkObjectReason;

#define GKR_PK_OBJECT_HANDLE_MASK      0x0FFFFFFF
#define GKR_PK_OBJECT_IS_PERMANENT	0x10000000
#define GKR_PK_OBJECT_IS_TEMPORARY	0x00000000

#define GKR_TYPE_PK_OBJECT             (gkr_pk_object_get_type())
#define GKR_PK_OBJECT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GKR_TYPE_PK_OBJECT, GkrPkObject))
#define GKR_PK_OBJECT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GKR_TYPE_PK_OBJECT, GkrPkObjectClass))
#define GKR_IS_PK_OBJECT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), GKR_TYPE_PK_OBJECT))
#define GKR_IS_PK_OBJECT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GKR_TYPE_PK_OBJECT))
#define GKR_PK_OBJECT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), GKR_TYPE_PK_OBJECT, GkrPkObjectClass))

typedef struct _GkrPkObject      GkrPkObject;
typedef struct _GkrPkObjectClass GkrPkObjectClass;

typedef struct _GkrPkObjectManager GkrPkObjectManager;

struct _GkrPkObject {
	GObject parent;
	
	GQuark location;
	gkrunique unique;
	CK_OBJECT_HANDLE handle;
	GkrPkObjectManager *manager;
};

struct _GkrPkObjectClass {
	GObjectClass parent_class;

	CK_RV (*get_bool_attribute) (GkrPkObject* obj, CK_ATTRIBUTE_PTR attr);
	CK_RV (*get_ulong_attribute) (GkrPkObject* obj, CK_ATTRIBUTE_PTR attr);
	CK_RV (*get_data_attribute) (GkrPkObject* obj, CK_ATTRIBUTE_PTR attr);
	CK_RV (*get_date_attribute) (GkrPkObject* obj, CK_ATTRIBUTE_PTR attr);
};

GType               gkr_pk_object_get_type         (void) G_GNUC_CONST;

void                gkr_pk_object_refresh          (GkrPkObject *object);

void                gkr_pk_object_flush            (GkrPkObject *object);

gboolean            gkr_pk_object_match            (GkrPkObject *object,
                                                    GArray *attrs);
                                                    
gboolean            gkr_pk_object_match_one        (GkrPkObject *object,
                                                    CK_ATTRIBUTE_PTR attr);

CK_OBJECT_HANDLE    gkr_pk_object_get_handle       (GkrPkObject *object);

GkrPkObjectManager* gkr_pk_object_get_manager      (GkrPkObject *object);

CK_RV               gkr_pk_object_get_attribute    (GkrPkObject *object,
                                                    CK_ATTRIBUTE_PTR attr);
                                                    
CK_RV               gkr_pk_object_get_attributes   (GkrPkObject *object,
                                                    GArray *attrs);

gchar*              gkr_pk_object_get_label        (GkrPkObject *object);

void                gkr_pk_object_set_label        (GkrPkObject *object, 
                                                    const gchar *label);

G_END_DECLS

#endif /* __GKR_PK_OBJECT_H__ */

/* 
 * gnome-keyring
 * 
 * Copyright (C) 2008 Stefan Walter
 * 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU Lesser General  License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General  License for more details.
 *  
 * You should have received a copy of the GNU Lesser General 
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  
 */

#include "gck-module.h"

#include "pkcs11/pkcs11.h"

static GckModule *pkcs11_module = NULL;
static pid_t pkcs11_module_pid = 0;
static GStaticMutex pkcs11_module_mutex = G_STATIC_MUTEX_INIT;

/* Forward declaration */
CK_RV C_GetFunctionList (CK_FUNCTION_LIST_PTR_PTR list);

static CK_RV
gck_C_Initialize (CK_VOID_PTR init_args)
{
	CK_RV rv = CKR_OK;
	pid_t pid = getpid ();
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL) {
			if (pkcs11_module_pid != pid) 
				rv = CKR_CRYPTOKI_ALREADY_INITIALIZED;
			else 
				pkcs11_module_pid = pid;
		} else {
			pkcs11_module = g_object_new (GCK_MODULE_TYPE, NULL);
			pkcs11_module_pid = pid;
		}

	g_static_mutex_unlock (&pkcs11_module_mutex);

	return CKR_OK;
}

static CK_RV
gck_C_Finalize (CK_VOID_PTR reserved)
{
	CK_RV rv;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module == NULL) {
			rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		} else {
			g_object_unref (pkcs11_module);
			pkcs11_module = NULL;
			pkcs11_module_pid = 0;
		}

	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetInfo (CK_INFO_PTR info)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_GetInfo (pkcs11_module, info);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetFunctionList (CK_FUNCTION_LIST_PTR_PTR list)
{
	xxxxx;
}

static CK_RV
gck_C_GetSlotList (CK_BBOOL token_present, CK_SLOT_ID_PTR slot_list, CK_ULONG_PTR count)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_GetSlotList (pkcs11_module, token_present, slot_list, count);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetSlotInfo (CK_SLOT_ID id, CK_SLOT_INFO_PTR info)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_GetSlotInfo (pkcs11_module, id, info);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetTokenInfo (CK_SLOT_ID id, CK_TOKEN_INFO_PTR info)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_GetTokenInfo (pkcs11_module, id, info);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetMechanismList (CK_SLOT_ID id, CK_MECHANISM_TYPE_PTR mechanism_list, CK_ULONG_PTR count)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_GetMechanismList (pkcs11_module, id, info);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetMechanismInfo (CK_SLOT_ID id, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR info)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_GetMechanismInfo (pkcs11_module, id, type, info);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_InitToken (CK_SLOT_ID id, CK_UTF8CHAR_PTR pin, CK_ULONG pin_len, CK_UTF8CHAR_PTR label)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_InitToken (pkcs11_module, id, pin, pin_len, label);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_WaitForSlotEvent (CK_FLAGS flags, CK_SLOT_ID_PTR slot, CK_VOID_PTR reserved)
{
	xxxxxxx;
}

static CK_RV
gck_C_OpenSession (CK_SLOT_ID id, CK_FLAGS flags, CK_VOID_PTR user_data, CK_NOTIFY callback, CK_SESSION_HANDLE_PTR session)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_OpenSession (pkcs11_module, id, flags, user_data, callback, session);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_CloseSession (CK_SESSION_HANDLE session)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_CloseSession (pkcs11_module, session);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_CloseAllSessions (CK_SLOT_ID id)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_CloseAllSessions (pkcs11_module, id);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetFunctionStatus (CK_SESSION_HANDLE session)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_session_C_GetFunctionStatus (gck_module_lookup_session (session));
			
	g_static_mutex_unlock (&pkcs11_module_mutex);

}

static CK_RV
gck_C_CancelFunction (CK_SESSION_HANDLE session)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_session_C_CancelFunction (gck_module_lookup_session (session));
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetSessionInfo (CK_SESSION_HANDLE session, CK_SESSION_INFO_PTR info)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_session_C_GetSessionInfo (gck_module_lookup_session (session),
			                                   info);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_InitPIN (CK_SESSION_HANDLE session, CK_UTF8CHAR_PTR pin, CK_ULONG pin_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_session_C_InitPIN (gck_module_lookup_session (session),
			                                   pin, pin_len);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SetPIN (CK_SESSION_HANDLE session, CK_UTF8CHAR_PTR old_pin, CK_ULONG old_pin_len, CK_UTF8CHAR_PTR new_pin, CK_ULONG new_pin_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_session_C_SetPIN (gck_module_lookup_session (session),
			                           old_pin, old_pin_len, new_pin, new_pin_len);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetOperationState (CK_SESSION_HANDLE session, CK_BYTE_PTR operation_state, CK_ULONG_PTR operation_state_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_session_C_GetOperationState (gck_module_lookup_session (session),
			                                      operation_state, operation_state_len);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SetOperationState (CK_SESSION_HANDLE session, CK_BYTE_PTR operation_state,
                         CK_ULONG operation_state_len, CK_OBJECT_HANDLE encryption_key,
                         CK_OBJECT_HANDLE authentication_key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_session_C_SetOperationState (gck_module_lookup_session (session),
			                                      operation_state, operation_state_len, encryption_key, authentication_key);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_Login (CK_SESSION_HANDLE session, CK_USER_TYPE user_type,
             CK_UTF8CHAR_PTR pin, CK_ULONG pin_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_Login (session, user_type, pin, pin_len);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);

}

static CK_RV
gck_C_Logout (CK_SESSION_HANDLE session)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
	
	g_static_mutex_lock (&pkcs11_module_mutex);
	
		if (pkcs11_module != NULL)
			rv = gck_module_C_Logout (session);
			
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_CreateObject (CK_SESSION_HANDLE session, CK_ATTRIBUTE_PTR template,
                    CK_ULONG count, CK_OBJECT_HANDLE_PTR new_object)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_CreateObject (gck_module_lookup_session (session),
		                                 template, count, new_object);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_CopyObject (CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object,
                  CK_ATTRIBUTE_PTR template, CK_ULONG count,
                  CK_OBJECT_HANDLE_PTR new_object)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_CopyObject (gck_module_lookup_session (session),
		                               object, template, count, new_object);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}


static CK_RV
gck_C_DestroyObject (CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DestroyObject (gck_module_lookup_session (session), object);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetObjectSize (CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object,
                     CK_ULONG_PTR size)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_object_C_GetObjectSize (gck_module_lookup_object (session, object), 
		                                 size);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GetAttributeValue (CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object,
                         CK_ATTRIBUTE_PTR template, CK_ULONG count)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_object_C_GetAttributeValue (gck_module_lookup_object (session, object), 
		                                     template, count);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SetAttributeValue (CK_SESSION_HANDLE session, CK_OBJECT_HANDLE object,
                         CK_ATTRIBUTE_PTR template, CK_ULONG count)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_object_C_SetAttributeValue (gck_module_lookup_object (session, object), 
		                                     template, count);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_FindObjectsInit (CK_SESSION_HANDLE session, CK_ATTRIBUTE_PTR template,
                       CK_ULONG count)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_FindObjectsInit (gck_module_lookup_session (session), 
		                                    template, count);
	
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_FindObjects (CK_SESSION_HANDLE session, CK_OBJECT_HANDLE_PTR objects,
                   CK_ULONG max_count, CK_ULONG_PTR count)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_FindObjects (gck_module_lookup_session (session), 
		                                objects, max_count, count);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_FindObjectsFinal (CK_SESSION_HANDLE session)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_FindObjectsFinal (gck_module_lookup_session (session));
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_EncryptInit (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                   CK_OBJECT_HANDLE key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_EncryptInit (gck_module_lookup_session (session),
		                                mechanism, key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_Encrypt (CK_SESSION_HANDLE session, CK_BYTE_PTR data, CK_ULONG data_len,
               CK_BYTE_PTR encrypted_data, CK_ULONG_PTR encrypted_data_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_Encrypt (gck_module_lookup_session (session),
		                            data, data_len, encrypted_data, encrypted_data_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_EncryptUpdate (CK_SESSION_HANDLE session, CK_BYTE_PTR part,
                     CK_ULONG part_len, CK_BYTE_PTR encrypted_part,
                     CK_ULONG_PTR encrypted_part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_EncryptUpdate (gck_module_lookup_session (session),
		                                  part, part_len, encrypted_part, encrypted_part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_EncryptFinal (CK_SESSION_HANDLE session, CK_BYTE_PTR last_part,
                    CK_ULONG_PTR last_part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_EncryptFinal (gck_module_lookup_session (session),
		                                 last_part, last_part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DecryptInit (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                   CK_OBJECT_HANDLE key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DecryptInit (gck_module_lookup_session (session),
		                                mechanism, key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_Decrypt (CK_SESSION_HANDLE session, CK_BYTE_PTR enc_data,
               CK_ULONG enc_data_len, CK_BYTE_PTR data, CK_ULONG_PTR data_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_Decrypt (gck_module_lookup_session (session),
		                            enc_data, enc_data_len, data, data_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DecryptUpdate (CK_SESSION_HANDLE session, CK_BYTE_PTR enc_part,
                     CK_ULONG enc_part_len, CK_BYTE_PTR part, CK_ULONG_PTR part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DecryptUpdate (gck_module_lookup_session (session),
		                                  enc_part, enc_part_len, part, part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DecryptFinal (CK_SESSION_HANDLE session, CK_BYTE_PTR last_part,
                    CK_ULONG_PTR last_part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DecryptFinal (gck_module_lookup_session (session),
		                                 last_part, last_part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DigestInit (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DigestInit (gck_module_lookup_session (session),
		                               mechanism);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_Digest (CK_SESSION_HANDLE session, CK_BYTE_PTR data, CK_ULONG data_len,
              CK_BYTE_PTR digest, CK_ULONG_PTR digest_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_Digest (gck_module_lookup_session (session),
		                           data, data_len, digest, digest_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DigestUpdate (CK_SESSION_HANDLE session, CK_BYTE_PTR part, CK_ULONG part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DigestUpdate (gck_module_lookup_session (session),
		                                  part, part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DigestKey (CK_SESSION_HANDLE session, CK_OBJECT_HANDLE key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DigestKey (gck_module_lookup_session (session),
		                              key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DigestFinal (CK_SESSION_HANDLE session, CK_BYTE_PTR digest,
                   CK_ULONG_PTR digest_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DigestFinal (gck_module_lookup_session (session),
		                                digest, digest_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SignInit (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                CK_OBJECT_HANDLE key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_SignInit (gck_module_lookup_session (session),
		                             mechanism, key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_Sign (CK_SESSION_HANDLE session, CK_BYTE_PTR data, CK_ULONG data_len,
            CK_BYTE_PTR signature, CK_ULONG_PTR signature_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_Sign (gck_module_lookup_session (session),
		                         data, data_len, signature, signature_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SignUpdate (CK_SESSION_HANDLE session, CK_BYTE_PTR part, CK_ULONG part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_SignUpdate (gck_module_lookup_session (session),
		                               part, part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SignFinal (CK_SESSION_HANDLE session, CK_BYTE_PTR signature,
                 CK_ULONG_PTR signature_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_SignFinal (gck_module_lookup_session (session),
		                              signature, signature_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SignRecoverInit (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                       CK_OBJECT_HANDLE key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_SignRecoverInit (gck_module_lookup_session (session),
		                                    mechanism, key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SignRecover (CK_SESSION_HANDLE session, CK_BYTE_PTR data, CK_ULONG data_len, 
                   CK_BYTE_PTR signature, CK_ULONG_PTR signature_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_SignRecover (gck_module_lookup_session (session),
		                                data, data_len, signature, signature_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_VerifyInit (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                  CK_OBJECT_HANDLE key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_VerifyInit (gck_module_lookup_session (session),
		                               mechanism, key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_Verify (CK_SESSION_HANDLE session, CK_BYTE_PTR data, CK_ULONG data_len,
              CK_BYTE_PTR signature, CK_ULONG signature_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_Verify (gck_module_lookup_session (session),
		                           data, data_len, signature, signature_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_VerifyUpdate (CK_SESSION_HANDLE session, CK_BYTE_PTR part, CK_ULONG part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_VerifyUpdate (gck_module_lookup_session (session),
		                                 part, part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_VerifyFinal (CK_SESSION_HANDLE session, CK_BYTE_PTR signature,
                   CK_ULONG signature_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_VerifyFinal (gck_module_lookup_session (session),
		                                signature, signature_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_VerifyRecoverInit (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                         CK_OBJECT_HANDLE key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_VerifyRecoverInit (gck_module_lookup_session (session),
		                                      mechanism, key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_VerifyRecover (CK_SESSION_HANDLE session, CK_BYTE_PTR signature,
                     CK_ULONG signature_len, CK_BYTE_PTR data, CK_ULONG_PTR data_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_VerifyRecover (gck_module_lookup_session (session),
		                                  signature, signature_len, data, data_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DigestEncryptUpdate (CK_SESSION_HANDLE session, CK_BYTE_PTR part,
                           CK_ULONG part_len, CK_BYTE_PTR enc_part,
                           CK_ULONG_PTR enc_part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DigestEncryptUpdate (gck_module_lookup_session (session),
		                                        part, part_len, enc_part, enc_part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DecryptDigestUpdate (CK_SESSION_HANDLE session, CK_BYTE_PTR enc_part,
                           CK_ULONG enc_part_len, CK_BYTE_PTR part, 
                           CK_ULONG_PTR part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DecryptDigestUpdate (gck_module_lookup_session (session),
		                                        enc_part, enc_part_len, part, part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SignEncryptUpdate (CK_SESSION_HANDLE session, CK_BYTE_PTR part,
                         CK_ULONG part_len, CK_BYTE_PTR enc_part,
                         CK_ULONG_PTR enc_part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_SignEncryptUpdate (gck_module_lookup_session (session),
		                                      part, part_len, enc_part, enc_part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DecryptVerifyUpdate (CK_SESSION_HANDLE session, CK_BYTE_PTR enc_part,
                           CK_ULONG enc_part_len, CK_BYTE_PTR part, 
                           CK_ULONG_PTR part_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_DecryptVerifyUpdate (gck_module_lookup_session (session),
		                                        enc_part, enc_part_len, part, part_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GenerateKey (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                   CK_ATTRIBUTE_PTR template, CK_ULONG count, 
                   CK_OBJECT_HANDLE_PTR key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_GenerateKey (gck_module_lookup_session (session),
		                                mechanism, template, count, key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GenerateKeyPair (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                       CK_ATTRIBUTE_PTR pub_template, CK_ULONG pub_count,
                       CK_ATTRIBUTE_PTR priv_template, CK_ULONG priv_count,
                       CK_OBJECT_HANDLE_PTR pub_key, CK_OBJECT_HANDLE_PTR priv_key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_GenerateKeyPair (gck_module_lookup_session (session),
		                                    mechanism, pub_template, pub_count, priv_template, priv_count, pub_key, priv_key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_WrapKey (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
               CK_OBJECT_HANDLE wrapping_key, CK_OBJECT_HANDLE key,
               CK_BYTE_PTR wrapped_key, CK_ULONG_PTR wrapped_key_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_WrapKey (gck_module_lookup_session (session),
		                            mechanism, wrapping_key, key, wrapped_key, wrapped_key_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_UnwrapKey (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                 CK_OBJECT_HANDLE unwrapping_key, CK_BYTE_PTR wrapped_key,
                 CK_ULONG wrapped_key_len, CK_ATTRIBUTE_PTR template,
                 CK_ULONG count, CK_OBJECT_HANDLE_PTR key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_UnwrapKey (gck_module_lookup_session (session),
		                              mechanism, unwrapping_key, wrapped_key, wrapped_key_len, template, count, key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_DeriveKey (CK_SESSION_HANDLE session, CK_MECHANISM_PTR mechanism,
                 CK_OBJECT_HANDLE base_key, CK_ATTRIBUTE_PTR template,
                 CK_ULONG count, CK_OBJECT_HANDLE_PTR key)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_UnwrapKey (gck_module_lookup_session (session),
		                              mechanism, base_key, template, count, key);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_SeedRandom (CK_SESSION_HANDLE session, CK_BYTE_PTR seed, CK_ULONG seed_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_SeedRandom (gck_module_lookup_session (session),
		                               seed, seed_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

static CK_RV
gck_C_GenerateRandom (CK_SESSION_HANDLE session, CK_BYTE_PTR random_data,
                      CK_ULONG random_len)
{
	CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;
		
	g_static_mutex_lock (&pkcs11_module_mutex);
	
	if (pkcs11_module != NULL)
		rv = gck_session_C_GenerateRandom (gck_module_lookup_session (session),
		                                   random_data, random_len);
				
	g_static_mutex_unlock (&pkcs11_module_mutex);
}

/* --------------------------------------------------------------------
 * MODULE ENTRY POINT
 */

/* 
 * PKCS#11 is broken here. It states that Unix compilers automatically byte 
 * pack structures. This is wrong. GCC on Linux aligns to 4 by default. 
 * 
 * This results in incompatibilities. Where this structure's first version
 * members take up too much or too little space depending on how this module
 * is compiled.
 */

static CK_FUNCTION_LIST gck_function_list = {
	{ CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },  /* version */
	gck_C_Initialize,
	gck_C_Finalize,
	gck_C_GetInfo,
	C_GetFunctionList,
	gck_C_GetSlotList,
	gck_C_GetSlotInfo,
	gck_C_GetTokenInfo,
	gck_C_GetMechanismList,
	gck_C_GetMechanismInfo,
	gck_C_InitToken,
	gck_C_InitPIN,
	gck_C_SetPIN,
	gck_C_OpenSession,
	gck_C_CloseSession,
	gck_C_CloseAllSessions,
	gck_C_GetSessionInfo,
	gck_C_GetOperationState,
	gck_C_SetOperationState,
	gck_C_Login,
	gck_C_Logout,
	gck_C_CreateObject,
	gck_C_CopyObject,
	gck_C_DestroyObject,
	gck_C_GetObjectSize,
	gck_C_GetAttributeValue,
	gck_C_SetAttributeValue,
	gck_C_FindObjectsInit,
	gck_C_FindObjects,
	gck_C_FindObjectsFinal,
	gck_C_EncryptInit,
	gck_C_Encrypt,
	gck_C_EncryptUpdate,
	gck_C_EncryptFinal,
	gck_C_DecryptInit,
	gck_C_Decrypt,
	gck_C_DecryptUpdate,
	gck_C_DecryptFinal,
	gck_C_DigestInit,
	gck_C_Digest,
	gck_C_DigestUpdate,
	gck_C_DigestKey,
	gck_C_DigestFinal,
	gck_C_SignInit,
	gck_C_Sign,
	gck_C_SignUpdate,
	gck_C_SignFinal,
	gck_C_SignRecoverInit,
	gck_C_SignRecover,
	gck_C_VerifyInit,
	gck_C_Verify,
	gck_C_VerifyUpdate,
	gck_C_VerifyFinal,
	gck_C_VerifyRecoverInit,
	gck_C_VerifyRecover,
	gck_C_DigestEncryptUpdate,
	gck_C_DecryptDigestUpdate,
	gck_C_SignEncryptUpdate,
	gck_C_DecryptVerifyUpdate,
	gck_C_GenerateKey,
	gck_C_GenerateKeyPair,
	gck_C_WrapKey,
	gck_C_UnwrapKey,
	gck_C_DeriveKey,
	gck_C_SeedRandom,
	gck_C_GenerateRandom,
	gck_C_GetFunctionStatus,
	gck_C_CancelFunction,
	gck_C_WaitForSlotEvent
};

CK_RV
C_GetFunctionList (CK_FUNCTION_LIST_PTR_PTR list)
{
	return_val_if_fail (list, CKR_ARGUMENTS_BAD);

	*list = &gck_function_list;
	return CKR_OK;
}
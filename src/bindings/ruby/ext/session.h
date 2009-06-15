/*
 * $Id$
 */

#ifndef __RB_SESSION_H
#define __RB_SESSION_H

typedef struct {
	ds_session_t *real;
	ds_playlist_t *rootpl;
	bool connected;
} rb_ds_session;

VALUE Init_Session (VALUE mDespotify);
void rb_despotify_session_error (VALUE session);

#define VALUE2SESSION(obj, var) \
	Data_Get_Struct ((obj), rb_ds_session, (var))

#endif

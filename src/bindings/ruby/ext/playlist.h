/*
 * $Id$
 */

#ifndef __RB_PLAYLIST_H
#define __RB_PLAYLIST_H

typedef struct {
	ds_playlist_t *real;
	bool ischild;
} rb_ds_playlist;


VALUE Init_Playlist (VALUE mDespotify);
VALUE PLAYLIST2VALUE (VALUE session, ds_playlist_t *pl, bool ischild);

#define VALUE2PLAYLIST(obj, var) \
	Data_Get_Struct ((obj), rb_ds_playlist, (var))

#endif

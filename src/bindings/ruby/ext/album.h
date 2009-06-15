/*
 * $Id$
 */

#ifndef __RB_ALBUM_H
#define __RB_ALBUM_H

typedef struct {
	ds_album_t *real;
} rb_ds_album;

VALUE Init_Album (VALUE mDespotify);
VALUE ALBUM2VALUE (ds_album_t *a);

#define VALUE2ALBUM(obj, var) \
		Data_Get_Struct ((obj), rb_ds_album, (var))

#endif

/*
 * $Id$
 */

#ifndef __RB_ARTIST_H
#define __RB_ARTIST_H

typedef struct {
	ds_artist_t *real;
} rb_ds_artist;


VALUE Init_Artist (VALUE mDespotify);
VALUE ARTIST2VALUE (ds_artist_t *a);

#define VALUE2ARTIST(obj, var) \
	Data_Get_Struct ((obj), rb_ds_artist, (var))

#endif

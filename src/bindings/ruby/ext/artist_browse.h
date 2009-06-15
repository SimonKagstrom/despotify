/*
 * $Id: artist.h 266 2009-03-27 02:36:26Z chripppa $
 */

#ifndef __RB_ARTISTBROWSE_H
#define __RB_ARTISTBROWSE_H

typedef struct {
	ds_artist_browse_t *real;
} rb_ds_artist_browse;


VALUE Init_ArtistBrowse (VALUE mDespotify);
VALUE ARTISTBROWS2VALUE (ds_artist_browse_t *a);

#define VALUE2ARTISTBROWSE(obj, var) \
	Data_Get_Struct ((obj), rb_ds_artist_browse, (var))

#endif

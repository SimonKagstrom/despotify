/*
 * $Id: album.h 284 2009-03-28 17:42:27Z chripppa $
 */

#ifndef __RB_ALBUMBROWSE_H
#define __RB_ALBUMBROWSE_H

typedef struct {
	ds_album_browse_t *real;
	bool ischild;
} rb_ds_album_browse;

VALUE Init_AlbumBrowse (VALUE mDespotify);
VALUE ALBUMBROWSE2VALUE (ds_album_browse_t *a, bool ischild);

#define VALUE2ALBUMBROWSE(obj, var) \
		Data_Get_Struct ((obj), rb_ds_album_browse, (var))

#endif

/*
 * $Id$
 */

#ifndef __RB_TRACK_H
#define __RB_TRACK_H

typedef struct {
	ds_track_t *real;
	bool ischild;
} rb_ds_track;

VALUE Init_Track (VALUE mDespotify);
VALUE TRACK2VALUE(ds_track_t *t, bool ischild);

#define VALUE2TRACK(obj, var) \
		Data_Get_Struct ((obj), rb_ds_track, (var))

#endif

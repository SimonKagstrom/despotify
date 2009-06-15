/*
 * $Id: search.h 284 2009-03-28 17:42:27Z chripppa $
 */

#ifndef __RB_SEARCH_H
#define __RB_SEARCH_H

typedef struct {
	ds_search_t *real;
} rb_ds_search;


VALUE Init_ds_search(VALUE mDespotify);
VALUE SEARCH2VALUE(VALUE session, ds_search_t *search);

#define VALUE2SEARCH(obj, var) \
	Data_Get_Struct ((obj), rb_ds_search, (var))

#endif

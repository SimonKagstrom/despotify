/*
 * $Id: album.c 285 2009-03-28 18:01:51Z chripppa $
 */

#include <ruby.h>
#include <despotify.h>

#include "main.h"
#include "album_browse.h"
#include "session.h"
#include "track.h"

#define ALBUMBROWSE_METHOD_HEADER \
	rb_ds_album_browse *album; \
	VALUE2ALBUMBROWSE (self, album);

static VALUE
rb_ds_album_browse_free (rb_ds_album_browse *album)
{
	if (!album->ischild)
		despotify_free_album_browse(album->real);

	free(album);
}

static VALUE
rb_ds_album_browse_alloc (VALUE klass)
{
	rb_ds_album_browse *album;
	VALUE obj;

	obj = Data_Make_Struct(klass, rb_ds_album_browse,
	                       NULL, rb_ds_album_browse_free,
	                       album);

	return obj;
}

VALUE
ALBUMBROWSE2VALUE (ds_album_browse_t *a, bool ischild)
{
	rb_ds_album_browse *album;
	VALUE obj;

	if (!a)
		return Qnil;

	obj = rb_ds_album_browse_alloc(cAlbumBrowse);
	VALUE2ALBUMBROWSE(obj, album);

	album->real = a;
	album->ischild = ischild;

	return obj;
}

static VALUE
rb_ds_album_browse_new (VALUE self, VALUE sessionv, VALUE id)
{
	rb_ds_album_browse *album;
	rb_ds_session *session;
	ds_album_browse_t *a;
	char *album_id;

	VALUE2ALBUMBROWSE(self, album);
	VALUE2SESSION(sessionv, session);
	album_id = StringValuePtr(id);

	if (strlen(album_id) != 32)
		return Qnil;

	a = despotify_get_album(session->real, album_id);

	if (!a)
		return Qnil;

	album->real = a;
	album->ischild = false;

	if (rb_block_given_p())
		rb_yield(self);

	return self;
}

static VALUE
rb_ds_album_browse_tracks(VALUE self)
{
	ALBUMBROWSE_METHOD_HEADER
	ds_track_t *t;
	VALUE tracks;

	if (rb_iv_get(self, "tracks") == Qnil) {
		tracks = rb_ary_new();

		for (t = album->real->tracks; t; t = t->next)
			rb_ary_push(tracks, TRACK2VALUE(t, true));

		rb_iv_set(self, "tracks", tracks);
	}

	return rb_iv_get(self, "tracks");
}

static VALUE
rb_ds_album_browse_metadata(VALUE self) {
	ALBUMBROWSE_METHOD_HEADER

	if (rb_iv_get(self, "metadata") == Qnil) {
		VALUE metadata = rb_hash_new();

		HASH_VALUE_ADD(metadata, "name", rb_str_new2(album->real->name));
		HASH_VALUE_ADD(metadata, "id", rb_str_new2(album->real->id));
		HASH_VALUE_ADD(metadata, "num_tracks", INT2FIX(album->real->num_tracks));
		HASH_VALUE_ADD(metadata, "year", INT2FIX(album->real->year));
		HASH_VALUE_ADD(metadata, "cover_id", rb_str_new2(album->real->cover_id));
		HASH_VALUE_ADD(metadata, "popularity", rb_float_new(album->real->popularity));

		rb_iv_set(self, "metadata", metadata);
	}

	return rb_iv_get(self, "metadata");
}

static VALUE
rb_ds_album_browse_lookup (VALUE self, VALUE key)
{
	ALBUMBROWSE_METHOD_HEADER
	VALUE metadata;

	metadata = rb_ds_album_browse_metadata(self);

	return rb_hash_aref(metadata, key);
}


static VALUE
rb_ds_album_browse_name (VALUE self)
{
	ALBUMBROWSE_METHOD_HEADER

	return rb_str_new2(album->real->name);
}

static VALUE
rb_ds_album_browse_id (VALUE self)
{
	ALBUMBROWSE_METHOD_HEADER

	return rb_str_new2(album->real->id);
}

static VALUE
rb_ds_album_browse_to_uri (VALUE self)
{
	ALBUMBROWSE_METHOD_HEADER

	char buf[1024];

	despotify_album_to_uri(album->real, buf);

	return rb_str_new2(buf);
}


VALUE
Init_AlbumBrowse (VALUE mDespotify)
{
	VALUE c;

	/* Despotify::AlbumBrowse */
	c = rb_define_class_under(mDespotify, "AlbumBrowse", rb_cObject);
	rb_define_alloc_func (c, rb_ds_album_browse_alloc);

	rb_define_method(c, "initialize", rb_ds_album_browse_new, 2);
	rb_define_method(c, "name", rb_ds_album_browse_name, 0);
	rb_define_method(c, "id", rb_ds_album_browse_id, 0);
	rb_define_method(c, "tracks", rb_ds_album_browse_tracks, 0);

	rb_define_method(c, "[]", rb_ds_album_browse_lookup, 1);
	rb_define_method(c, "metadata", rb_ds_album_browse_metadata, 0);
	rb_define_method(c, "to_uri", rb_ds_album_browse_to_uri, 0);

	return c;
}

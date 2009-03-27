/*
 * $Id$
 */

#include <ruby.h>
#include <despotify.h>

#include "main.h"
#include "session.h"
#include "playlist.h"
#include "track.h"
#include "artist.h"
#include "album.h"

#define ALBUM_METHOD_HEADER \
	rb_despotify_album *album; \
	VALUE2ALBUM(self, album);


static VALUE
rb_despotify_album_alloc(VALUE klass) {
	rb_despotify_album *track;
	VALUE obj = Data_Make_Struct(klass, rb_despotify_album,
	                             NULL, free, track);

	return obj;
}

static VALUE
rb_despotify_album_new(VALUE self) {
	return self;
}

VALUE
rb_despotify_album_new_from_album(despotify_album *a) {
	VALUE obj;
	rb_despotify_album *album;

	if (!a)
		return Qnil;

	obj = rb_despotify_album_alloc(cAlbum);
	VALUE2ALBUM(obj, album);

	album->real = a;

	return obj;
}


static VALUE
rb_despotify_album_tracks(VALUE self) {
	ALBUM_METHOD_HEADER
	despotify_track *t;
	VALUE tracks;

	if (rb_iv_get(self, "tracks") == Qnil) {
		tracks = rb_ary_new();

		for (t = album->real->tracks; t; t = t->next)
			rb_ary_push(tracks, rb_despotify_track_new_from_track(t));

		rb_iv_set(self, "tracks", tracks);
	}

	return rb_iv_get(self, "tracks");
}

static VALUE
rb_despotify_album_metadata(VALUE self) {
	ALBUM_METHOD_HEADER

	if (rb_iv_get(self, "metadata") == Qnil) {
		VALUE metadata = rb_hash_new();

		HASH_VALUE_ADD(metadata, "name", rb_str_new2(album->real->name));
		HASH_VALUE_ADD(metadata, "id", rb_str_new2(album->real->id));
		HASH_VALUE_ADD(metadata, "num_tracks", INT2NUM(album->real->num_tracks));
		HASH_VALUE_ADD(metadata, "year", INT2NUM(album->real->year));
		HASH_VALUE_ADD(metadata, "cover_id", rb_str_new2(album->real->cover_id));
		HASH_VALUE_ADD(metadata, "popularity", rb_float_new(album->real->popularity));

		rb_iv_set(self, "metadata", metadata);
	}

	return rb_iv_get(self, "metadata");
}

static VALUE
rb_despotify_album_lookup(VALUE self, VALUE key) {
	ALBUM_METHOD_HEADER
	VALUE metadata;

	metadata = rb_despotify_album_metadata(self);

	return rb_hash_aref(metadata, key);
}


static VALUE
rb_despotify_album_name(VALUE self) {
	ALBUM_METHOD_HEADER

	return rb_str_new2(album->real->name);
}

static VALUE
rb_despotify_album_id(VALUE self) {
	ALBUM_METHOD_HEADER

	return rb_str_new2(album->real->id);
}


VALUE
Init_despotify_album(VALUE mDespotify) {
	VALUE c;

	/* Despotify::Track */
	c = rb_define_class_under(mDespotify, "Album", rb_cObject);

	/* Remove new function until we can request album by id */
	rb_undef_method (rb_singleton_class(c), "new");

	rb_define_method(c, "name", rb_despotify_album_name, 0);
	rb_define_method(c, "id", rb_despotify_album_id, 0);
	rb_define_method(c, "tracks", rb_despotify_album_tracks, 0);

	rb_define_method(c, "[]", rb_despotify_album_lookup, 1);
	rb_define_method(c, "metadata", rb_despotify_album_metadata, 0);

	return c;
}

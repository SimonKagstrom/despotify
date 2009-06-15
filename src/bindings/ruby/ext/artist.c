/*
 * $Id$
 */

#include <ruby.h>
#include <despotify.h>

#include "main.h"
#include "artist.h"
#include "session.h"
#include "track.h"

#define ARTIST_METHOD_HEADER \
	rb_ds_artist *artist; \
	VALUE2ARTIST(self, artist);

static VALUE
rb_ds_artist_alloc (VALUE klass)
{
	rb_ds_artist *artist;
	VALUE obj;

	obj = Data_Make_Struct(klass, rb_ds_artist,
	                       NULL, NULL, artist);

	return obj;
}

VALUE
ARTIST2VALUE (ds_artist_t *a)
{
	rb_ds_artist *artist;
	VALUE obj;

	if (!a)
		return Qnil;

	obj = rb_ds_artist_alloc(cArtist);
	VALUE2ARTIST(obj, artist);

	artist->real = a;

	return obj;
}

static VALUE
rb_ds_artist_metadata (VALUE self)
{
	ARTIST_METHOD_HEADER

	if (rb_iv_get(self, "metadata") == Qnil) {
		VALUE metadata = rb_hash_new();

		HASH_VALUE_ADD(metadata, "name", rb_str_new2(artist->real->name));
		HASH_VALUE_ADD(metadata, "id", rb_str_new2(artist->real->id));
		HASH_VALUE_ADD(metadata, "portrait_id", rb_str_new2(artist->real->portrait_id));
		HASH_VALUE_ADD(metadata, "popularity", rb_float_new(artist->real->popularity));

		rb_iv_set(self, "metadata", metadata);
	}

	return rb_iv_get(self, "metadata");
}

static VALUE
rb_ds_artist_lookup (VALUE self, VALUE key)
{
	ARTIST_METHOD_HEADER
	VALUE metadata;

	metadata = rb_ds_artist_metadata(self);

	return rb_hash_aref(metadata, key);
}

static VALUE
rb_ds_artist_name (VALUE self)
{
	ARTIST_METHOD_HEADER

	return rb_str_new2(artist->real->name);
}

static VALUE
rb_ds_artist_id (VALUE self)
{
	ARTIST_METHOD_HEADER

	return rb_str_new2(artist->real->id);
}

VALUE
Init_Artist (VALUE mDespotify)
{
	VALUE c;

	/* Despotify::Artist */
	c = rb_define_class_under(mDespotify, "Artist", rb_cObject);

	rb_define_method(c, "name", rb_ds_artist_name, 0);
	rb_define_method(c, "id", rb_ds_artist_id, 0);

	rb_define_method(c, "[]", rb_ds_artist_lookup, 1);
	rb_define_method(c, "metadata", rb_ds_artist_metadata, 0);

	return c;
}

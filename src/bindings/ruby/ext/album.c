/*
 * $Id$
 */

#include <ruby.h>
#include <despotify.h>

#include "main.h"
#include "album.h"
#include "artist.h"
#include "playlist.h"
#include "session.h"
#include "track.h"

#define ALBUM_METHOD_HEADER \
	rb_ds_album *album; \
	VALUE2ALBUM(self, album);

static VALUE
rb_ds_album_alloc (VALUE klass)
{
	rb_ds_album *album;
	VALUE obj;

	obj = Data_Make_Struct(klass, rb_ds_album,
	                       NULL, NULL, album);

	return obj;
}

VALUE
ALBUM2VALUE (ds_album_t *a)
{
	rb_ds_album *album;
	VALUE obj;

	if (!a)
		return Qnil;

	obj = rb_ds_album_alloc(cAlbum);
	VALUE2ALBUM(obj, album);

	album->real = a;

	return obj;
}

static VALUE
rb_ds_album_metadata (VALUE self)
{
	ALBUM_METHOD_HEADER

	if (rb_iv_get(self, "metadata") == Qnil) {
		VALUE metadata = rb_hash_new();

		HASH_VALUE_ADD(metadata, "name", rb_str_new2(album->real->name));
		HASH_VALUE_ADD(metadata, "id", rb_str_new2(album->real->id));
		HASH_VALUE_ADD(metadata, "artist", rb_str_new2(album->real->artist));
		HASH_VALUE_ADD(metadata, "artist_id", rb_str_new2(album->real->artist_id));
		HASH_VALUE_ADD(metadata, "cover_id", rb_str_new2(album->real->cover_id));
		HASH_VALUE_ADD(metadata, "popularity", rb_float_new(album->real->popularity));

		rb_iv_set(self, "metadata", metadata);
	}

	return rb_iv_get(self, "metadata");
}

static VALUE
rb_ds_album_lookup (VALUE self, VALUE key)
{
	ALBUM_METHOD_HEADER
	VALUE metadata;

	metadata = rb_ds_album_metadata(self);

	return rb_hash_aref(metadata, key);
}


static VALUE
rb_ds_album_name (VALUE self)
{
	ALBUM_METHOD_HEADER

	return rb_str_new2(album->real->name);
}

static VALUE
rb_ds_album_id (VALUE self)
{
	ALBUM_METHOD_HEADER

	return rb_str_new2(album->real->id);
}


VALUE
Init_Album (VALUE mDespotify)
{
	VALUE c;

	/* Despotify::Album */
	c = rb_define_class_under(mDespotify, "Album", rb_cObject);

	rb_define_method(c, "name", rb_ds_album_name, 0);
	rb_define_method(c, "id", rb_ds_album_id, 0);

	rb_define_method(c, "[]", rb_ds_album_lookup, 1);
	rb_define_method(c, "metadata", rb_ds_album_metadata, 0);

	return c;
}

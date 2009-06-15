/*
 * $Id: artist.c 285 2009-03-28 18:01:51Z chripppa $
 */

#include <ruby.h>
#include <despotify.h>

#include "main.h"
#include "artist_browse.h"
#include "album_browse.h"
#include "session.h"
#include "track.h"

#define ARTISTBROWSE_METHOD_HEADER \
	rb_ds_artist_browse *artist; \
	VALUE2ARTISTBROWSE(self, artist);


static void
rb_ds_artist_browse_free (rb_ds_artist_browse *artist)
{
	despotify_free_artist_browse(artist->real);

	free(artist);
}

static VALUE
rb_ds_artist_browse_alloc (VALUE klass)
{
	rb_ds_artist_browse *artist;
	VALUE obj;

	obj = Data_Make_Struct(klass, rb_ds_artist_browse, NULL,
	                       rb_ds_artist_browse_free, artist);

	return obj;
}

VALUE
ARTISTBROWSE2VALUE (ds_artist_browse_t *a)
{
	rb_ds_artist_browse *artist;
	VALUE obj;

	if (!a)
		return Qnil;

	obj = rb_ds_artist_browse_alloc(cArtistBrowse);
	VALUE2ARTISTBROWSE(obj, artist);

	artist->real = a;

	return obj;
}

static VALUE
rb_ds_artist_browse_new (VALUE self, VALUE sessionv, VALUE id)
{
	rb_ds_session *session;
	rb_ds_artist_browse *artist;
	ds_artist_browse_t *a;
	unsigned char *artist_id;

	VALUE2ARTISTBROWSE(self, artist);
	VALUE2SESSION(sessionv, session);
	artist_id = StringValuePtr(id);

	if (strlen(artist_id) != 32)
		return Qnil;

	a = despotify_get_artist(session->real, artist_id);

	if (!a)
		return Qnil;

	artist->real = a;

	if (rb_block_given_p())
		rb_yield(self);

	return self;
}


static VALUE
rb_ds_artist_browse_albums (VALUE self)
{
	ARTISTBROWSE_METHOD_HEADER

	if (rb_iv_get(self, "albums") == Qnil) {
		VALUE albums = rb_ary_new();
		ds_album_browse_t *a;

		for(a = artist->real->albums; a; a = a->next) {
			rb_ary_push(albums, ALBUMBROWSE2VALUE(a, true));
		}

		rb_iv_set(self, "albums", albums);
	}

	return rb_iv_get(self, "albums");
}

static VALUE
rb_ds_artist_browse_metadata (VALUE self)
{
	ARTISTBROWSE_METHOD_HEADER

	if (rb_iv_get(self, "metadata") == Qnil) {
		VALUE metadata = rb_hash_new();

		HASH_VALUE_ADD(metadata, "name", rb_str_new2(artist->real->name));
		HASH_VALUE_ADD(metadata, "id", rb_str_new2(artist->real->id));
		if(artist->real->text)
			HASH_VALUE_ADD(metadata, "text", rb_str_new2(artist->real->text));
		HASH_VALUE_ADD(metadata, "portrait_id", rb_str_new2(artist->real->portrait_id));
		HASH_VALUE_ADD(metadata, "genres", rb_str_new2(artist->real->genres));
		HASH_VALUE_ADD(metadata, "years_active", rb_str_new2(artist->real->years_active));
		HASH_VALUE_ADD(metadata, "num_albums", INT2FIX(artist->real->num_albums));
		HASH_VALUE_ADD(metadata, "popularity", rb_float_new(artist->real->popularity));

		rb_iv_set(self, "metadata", metadata);
	}

	return rb_iv_get(self, "metadata");
}

static VALUE
rb_ds_artist_browse_lookup(VALUE self, VALUE key)
{
	ARTISTBROWSE_METHOD_HEADER
	VALUE metadata;

	metadata = rb_ds_artist_browse_metadata(self);

	return rb_hash_aref(metadata, key);
}

static VALUE
rb_ds_artist_browse_name (VALUE self)
{
	ARTISTBROWSE_METHOD_HEADER

	return rb_str_new2(artist->real->name);
}

static VALUE
rb_ds_artist_browse_id (VALUE self)
{
	ARTISTBROWSE_METHOD_HEADER

	return rb_str_new2(artist->real->id);
}

static VALUE
rb_ds_artist_browse_to_uri (VALUE self)
{
	ARTISTBROWSE_METHOD_HEADER

	char buf[1024];

	despotify_artist_to_uri(artist->real, buf);

	return rb_str_new2(buf);
}


VALUE
Init_ArtistBrowse (VALUE mDespotify)
{
	VALUE c;

	/* Despotify::ArtistBrowse */
	c = rb_define_class_under(mDespotify, "ArtistBrowse", rb_cObject);
	rb_define_alloc_func (c, rb_ds_artist_browse_alloc);

	rb_define_method(c, "initialize", rb_ds_artist_browse_new, 2);
	rb_define_method(c, "name", rb_ds_artist_browse_name, 0);
	rb_define_method(c, "id", rb_ds_artist_browse_id, 0);
	rb_define_method(c, "albums", rb_ds_artist_browse_albums, 0);

	rb_define_method(c, "[]", rb_ds_artist_browse_lookup, 1);
	rb_define_method(c, "metadata", rb_ds_artist_browse_metadata, 0);
	rb_define_method(c, "to_uri", rb_ds_artist_browse_to_uri, 0);

	return c;
}

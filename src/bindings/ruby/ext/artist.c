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

#define ARTIST_METHOD_HEADER \
	rb_despotify_artist *artist; \
	VALUE2ARTIST(self, artist);


static void
rb_despotify_artist_free(rb_despotify_artist *artist) {
	if (artist->real)
		despotify_free_artist(artist->real);

	free(artist);
}

static VALUE
rb_despotify_artist_alloc(VALUE klass) {
	rb_despotify_artist *artist;

	VALUE obj = Data_Make_Struct(klass, rb_despotify_artist, NULL,
	                             rb_despotify_artist_free, artist);

	return obj;
}

VALUE
rb_despotify_artist_new_from_artist(despotify_artist *a) {
	VALUE obj;
	rb_despotify_artist *artist;

	if (!a)
		return Qnil;

	obj = rb_despotify_artist_alloc(cArtist);
	VALUE2ARTIST(obj, artist);

	artist->real = a;

	return obj;
}

static VALUE
rb_despotify_artist_new(VALUE self, VALUE session, VALUE id) {
	rb_despotify_session *sessionptr;
	rb_despotify_artist *artist;

	despotify_artist *a = NULL;
	unsigned char *artist_id;

	VALUE2ARTIST(self, artist);
	VALUE2SESSION(session, sessionptr);
	artist_id = StringValuePtr(id);

	CHECKIDLEN(artist_id, 32);

	a = despotify_get_artist(sessionptr->real, artist_id);
	if(!a)
		return Qnil;

	artist->real = a;

	return self;
}


static VALUE
rb_despotify_artist_albums(VALUE self) {
	ARTIST_METHOD_HEADER
	despotify_album *a = NULL;
	VALUE albums;

	albums = rb_ary_new();

	for(a = artist->real->albums; a; a = a->next) {
		rb_ary_push(albums, rb_despotify_album_new_from_album(a));
	}

	return albums;
}

static VALUE
rb_despotify_artist_lookup(VALUE self, VALUE key) {
	ARTIST_METHOD_HEADER
	char *keyval = NULL;

	keyval = StringValuePtr(key);

	if (!strcmp(keyval, "name"))
		return rb_str_new2(artist->real->name);
	else if (!strcmp(keyval, "id"))
		return rb_str_new2(artist->real->id);
	else if (!strcmp(keyval, "text")) {
		if(artist->real->text)
			return rb_str_new2(artist->real->text);
	} else if (!strcmp(keyval, "portrait_id"))
		return rb_str_new2(artist->real->portrait_id);
	else if (!strcmp(keyval, "genres"))
		return rb_str_new2(artist->real->genres);
	else if (!strcmp(keyval, "years_active"))
		return rb_str_new2(artist->real->years_active);
	else if (!strcmp(keyval, "num_albums"))
		return INT2NUM(artist->real->num_albums);
	else if (!strcmp(keyval, "popularity"))
		return rb_float_new(artist->real->popularity);

	return Qnil;
}


#define ARTIST_METADATA_ADD(hash, key) \
	keyval = rb_str_new2(key); \
	rb_hash_aset(hash, keyval, rb_despotify_artist_lookup(self, keyval))

static VALUE
rb_despotify_artist_metadata(VALUE self) {
	ARTIST_METHOD_HEADER
	VALUE metadata;
	VALUE keyval;

	metadata = rb_hash_new();
	ARTIST_METADATA_ADD(metadata, "name");
	ARTIST_METADATA_ADD(metadata, "id");
	ARTIST_METADATA_ADD(metadata, "text");
	ARTIST_METADATA_ADD(metadata, "portrait_id");
	ARTIST_METADATA_ADD(metadata, "genres");
	ARTIST_METADATA_ADD(metadata, "years_active");
	ARTIST_METADATA_ADD(metadata, "num_albums");
	ARTIST_METADATA_ADD(metadata, "popularity");


	return metadata;
}


static VALUE
rb_despotify_artist_name(VALUE self) {
	ARTIST_METHOD_HEADER

	return rb_str_new2(artist->real->name);
}


static VALUE
rb_despotify_artist_id(VALUE self) {
	ARTIST_METHOD_HEADER

	return rb_str_new2(artist->real->id);
}


VALUE
Init_despotify_artist(VALUE mDespotify) {
	VALUE c;

	/* Despotify::Artist */
	c = rb_define_class_under(mDespotify, "Artist", rb_cObject);
	rb_define_alloc_func (c, rb_despotify_artist_alloc);

	rb_define_method(c, "initialize", rb_despotify_artist_new, 2);
	rb_define_method(c, "name", rb_despotify_artist_name, 0);
	rb_define_method(c, "id", rb_despotify_artist_id, 0);
	rb_define_method(c, "albums", rb_despotify_artist_albums, 0);

	rb_define_method(c, "[]", rb_despotify_artist_lookup, 1);
	rb_define_method(c, "metadata", rb_despotify_artist_metadata, 0);

	return c;
}

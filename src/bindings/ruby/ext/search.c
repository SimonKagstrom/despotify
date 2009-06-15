/*
 * $Id: playlist.c 285 2009-03-28 18:01:51Z chripppa $
 */

#include <ruby.h>
#include <despotify.h>

#include "main.h"
#include "session.h"
#include "playlist.h"
#include "track.h"
#include "artist.h"
#include "album.h"
#include "search.h"

#define SEARCH_METHOD_HEADER \
	rb_ds_search *search; \
	VALUE2SEARCH(self, search);


static void
rb_ds_search_free (rb_ds_search *search)
{
	despotify_free_search(search->real);

	free(search);
}

static VALUE
rb_ds_search_alloc (VALUE klass)
{
	rb_ds_search *search;
	VALUE obj;

	obj = Data_Make_Struct(klass, rb_ds_search, NULL,
	                       rb_ds_search_free, search);

	return obj;
}

VALUE
SEARCH2VALUE (VALUE session, ds_search_t *s)
{
	VALUE obj;
	rb_ds_search *search;

	if (!s)
		return Qnil;

	obj = rb_ds_search_alloc(cSearch);
	VALUE2SEARCH(obj, search);

	search->real = s;

	rb_iv_set(obj, "session", session);

	return obj;
}

static VALUE
rb_ds_search_new (VALUE self, VALUE sessionv, VALUE sterm, VALUE maxresults)
{
	rb_ds_session *session;
	rb_ds_search *search;
	ds_search_t *s;
	char *searchterm;

	VALUE2SEARCH(self, search);
	VALUE2SESSION(sessionv, session);
	searchterm = StringValuePtr(sterm);

	s = despotify_search(session->real, searchterm, NUM2INT(maxresults));
	if (!s)
		return Qnil;

	search->real = s;

	rb_iv_set(self, "session", sessionv);

	if (rb_block_given_p())
		rb_yield(self);

	return self;
}


static VALUE
rb_ds_search_artists (VALUE self)
{
	SEARCH_METHOD_HEADER

	VALUE artists;
	ds_artist_t *a;

	if (rb_iv_get(self, "artists") == Qnil) {
		artists = rb_ary_new();

		for (a = search->real->artists; a; a = a->next) {
			rb_ary_push(artists, ARTIST2VALUE(a));
		}

		rb_iv_set(self, "artists", artists);
	}

	return rb_iv_get(self, "artists");
}

static VALUE
rb_ds_search_albums (VALUE self)
{
	SEARCH_METHOD_HEADER

	VALUE albums;
	ds_album_t *a;

	if (rb_iv_get(self, "albums") == Qnil) {
		albums = rb_ary_new();

		for (a = search->real->albums; a; a = a->next) {
			rb_ary_push(albums, ALBUM2VALUE(a));
		}

		rb_iv_set(self, "albums", albums);
	}

	return rb_iv_get(self, "albums");
}

static VALUE
rb_ds_search_tracks (VALUE self)
{
	SEARCH_METHOD_HEADER

	VALUE tracks;
	ds_track_t *t;

	if (rb_iv_get(self, "tracks") == Qnil) {
		tracks = rb_ary_new();

		for (t = search->real->tracks; t; t = t->next) {
			rb_ary_push(tracks, TRACK2VALUE(t, true));
		}

		rb_iv_set(self, "tracks", tracks);
	}

	return rb_iv_get(self, "tracks");
}

static VALUE
rb_ds_search_playlist (VALUE self)
{
	SEARCH_METHOD_HEADER

	return PLAYLIST2VALUE(rb_iv_get(self, "session"),
	                      search->real->playlist, true);
}

static VALUE
rb_ds_search_append (VALUE self, VALUE offset, VALUE maxresults)
{
	SEARCH_METHOD_HEADER
	rb_ds_session *session;

	VALUE2SESSION(rb_iv_get(self, "session"), session);

	rb_iv_set(self, "tracks", Qnil);
	despotify_search_more(session->real, search->real,
	                      NUM2INT(offset), NUM2INT(maxresults));

	return self;
}

static VALUE
rb_ds_search_metadata (VALUE self)
{
	SEARCH_METHOD_HEADER
	VALUE metadata;

	if (rb_iv_get(self, "metadata") == Qnil) {
		metadata = rb_hash_new();

		HASH_VALUE_ADD(metadata, "query", rb_str_new2(
		               search->real->query));

		HASH_VALUE_ADD(metadata, "suggestion", rb_str_new2(
		               search->real->suggestion));

		HASH_VALUE_ADD(metadata, "total_artists", INT2FIX(
		               search->real->total_artists));

		HASH_VALUE_ADD(metadata, "total_albums", INT2FIX(
		               search->real->total_albums));

		HASH_VALUE_ADD(metadata, "total_tracks", INT2FIX(
		               search->real->total_tracks));

		rb_iv_set(self, "metadata", metadata);
	}

	return rb_iv_get(self, "metadata");
}


static VALUE
rb_ds_search_lookup (VALUE self, VALUE key)
{
	SEARCH_METHOD_HEADER
	VALUE metadata;

	metadata = rb_ds_search_metadata(self);

	return rb_hash_aref(metadata, key);
}

static VALUE
rb_ds_search_to_uri (VALUE self)
{
	SEARCH_METHOD_HEADER

	char buf[1024];

	despotify_search_to_uri(search->real, buf);

	return rb_str_new2(buf);
}

VALUE
Init_Search (VALUE mDespotify)
{
	VALUE c;

	/* Despotify::Search */
	c = rb_define_class_under(mDespotify, "Search", rb_cObject);
	rb_define_alloc_func (c, rb_ds_search_alloc);

	rb_define_method(c, "initialize", rb_ds_search_new, 3);
	rb_define_method(c, "append", rb_ds_search_append, 2);
	rb_define_method(c, "albums", rb_ds_search_albums, 0);
	rb_define_method(c, "artists", rb_ds_search_artists, 0);
	rb_define_method(c, "tracks", rb_ds_search_tracks, 0);
	rb_define_method(c, "playlist", rb_ds_search_playlist, 0);
	rb_define_method(c, "metadata", rb_ds_search_metadata, 0);
	rb_define_method(c, "[]", rb_ds_search_lookup, 1);
	rb_define_method(c, "to_uri", rb_ds_search_to_uri, 0);

	return c;
}

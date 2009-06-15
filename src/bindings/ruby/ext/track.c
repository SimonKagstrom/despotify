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

#define TRACK_METHOD_HEADER \
	rb_ds_track *track; \
	VALUE2TRACK(self, track);


static void
rb_ds_free (rb_ds_track *track)
{
	if (!track->ischild)
		despotify_free_track(track->real);

	free(track);
}

static VALUE
rb_ds_track_alloc (VALUE klass)
{
	rb_ds_track *track;
	VALUE obj;

	obj = Data_Make_Struct(klass, rb_ds_track,
	                       NULL, free, track);

	return obj;
}

static VALUE
rb_ds_track_new (VALUE self, VALUE sessionv, VALUE id)
{
	rb_ds_session *session;
	rb_ds_track *track;
	ds_track_t *t;
	char *track_id;

	VALUE2TRACK(self, track);
	VALUE2SESSION(sessionv, session);
	track_id = StringValuePtr(id);

	if (strlen(track_id) != 32)
		return Qnil;

	t = despotify_get_track(session->real, track_id);

	if (!t)
		return Qnil;

	track->real = t;
	track->ischild = false;

	rb_iv_set(self, "session", sessionv);

	if (rb_block_given_p())
		rb_yield(self);

	return self;
}

VALUE
TRACK2VALUE (ds_track_t *t, bool ischild)
{
	VALUE obj;
	rb_ds_track *track;

	if (!t)
		return Qnil;

	obj = rb_ds_track_alloc(cTrack);
	VALUE2TRACK(obj, track);

	track->real = t;
	track->ischild = ischild;

	return obj;
}

static VALUE
rb_ds_track_metadata (VALUE self)
{
	TRACK_METHOD_HEADER

	if (rb_iv_get(self, "metadata") == Qnil) {
		VALUE metadata = rb_hash_new();
		VALUE artists = rb_ary_new();
		ds_artist_t *a;

		for (a = track->real->artist; a; a = a->next) {
			rb_ary_push(artists, ARTIST2VALUE(a));
		}

		HASH_VALUE_ADD(metadata, "id", rb_str_new2(track->real->track_id));
		HASH_VALUE_ADD(metadata, "artists", artists);
		HASH_VALUE_ADD(metadata, "album", rb_str_new2(track->real->album));
		HASH_VALUE_ADD(metadata, "title", rb_str_new2(track->real->title));
		HASH_VALUE_ADD(metadata, "length", INT2FIX(track->real->length));
		HASH_VALUE_ADD(metadata, "tracknumber", INT2FIX(track->real->tracknumber));
		HASH_VALUE_ADD(metadata, "year", INT2FIX(track->real->year));
		HASH_VALUE_ADD(metadata, "file_id", rb_str_new2(track->real->file_id));
		HASH_VALUE_ADD(metadata, "album_id", rb_str_new2(track->real->album_id));
		HASH_VALUE_ADD(metadata, "cover_id", rb_str_new2(track->real->cover_id));
		HASH_VALUE_ADD(metadata, "playable", BOOL2VALUE(track->real->playable));
		HASH_VALUE_ADD(metadata, "popularity", rb_float_new(track->real->popularity));

		rb_iv_set(self, "metadata", metadata);
	}

	return rb_iv_get(self, "metadata");
}

static VALUE
rb_ds_track_lookup (VALUE self, VALUE key)
{
	TRACK_METHOD_HEADER
	VALUE metadata;

	metadata = rb_ds_track_metadata(self);

	return rb_hash_aref(metadata, key);
}

static VALUE
rb_ds_track_id (VALUE self)
{
	TRACK_METHOD_HEADER

	return rb_ds_track_lookup(self, rb_str_new2("id"));
}

static VALUE
rb_ds_track_artists (VALUE self)
{
	TRACK_METHOD_HEADER

	return rb_ds_track_lookup(self, rb_str_new2("artists"));
}


static VALUE
rb_ds_track_to_uri (VALUE self)
{
	TRACK_METHOD_HEADER

	char buf[1024];

	despotify_track_to_uri(track->real, buf);

	return rb_str_new2(buf);
}



VALUE
Init_Track (VALUE mDespotify)
{
	VALUE c;

	/* Despotify::Track */
	c = rb_define_class_under(mDespotify, "Track", rb_cObject);
	rb_define_alloc_func(c, rb_ds_track_alloc);
	rb_define_method(c, "initialize", rb_ds_track_new, 2);
	rb_define_method(c, "id", rb_ds_track_id, 0);
	rb_define_method(c, "artists", rb_ds_track_artists, 0);
	rb_define_method(c, "[]", rb_ds_track_lookup, 1);
	rb_define_method(c, "metadata", rb_ds_track_metadata, 0);
	rb_define_method(c, "to_uri", rb_ds_track_to_uri, 0);

	return c;
}

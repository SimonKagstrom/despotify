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
	rb_despotify_track *track; \
	VALUE2TRACK(self, track);


static VALUE
rb_despotify_track_alloc(VALUE klass) {
	rb_despotify_track *track;
	VALUE obj = Data_Make_Struct(klass, rb_despotify_track,
	                             NULL, free, track);

	return obj;
}

static VALUE
rb_despotify_track_new(VALUE self) {
	return self;
}

VALUE
rb_despotify_track_new_from_track(despotify_track *t) {
	VALUE obj;
	rb_despotify_track *track;

	if(!t)
		return Qnil;

	obj = rb_despotify_track_alloc(cTrack);
	VALUE2TRACK(obj, track);

	track->real = t;

	return obj;
}

static VALUE
rb_despotify_track_lookup(VALUE self, VALUE key) {
	TRACK_METHOD_HEADER
	char *keyval = NULL;

	keyval = StringValuePtr(key);

	if (!strcmp(keyval, "artist")) {
		VALUE artists = rb_ary_new();
		despotify_artist *a;
		for (a = track->real->artist; a; a = a->next)
			rb_ary_push(artists, rb_str_new2(a->name));

		return artists;
	} else if(!strcmp(keyval, "album"))
		return rb_str_new2(track->real->album);
	else if(!strcmp(keyval, "title"))
		return rb_str_new2(track->real->title);
	else if(!strcmp(keyval, "length"))
		return INT2NUM(track->real->length);
	else if(!strcmp(keyval, "tracknumber"))
		return INT2NUM(track->real->tracknumber);
	else if(!strcmp(keyval, "year"))
		return INT2NUM(track->real->year);
	else if(!strcmp(keyval, "id"))
	    return rb_str_new2(track->real->track_id);
	else if(!strcmp(keyval, "artist_id")) {
		VALUE ids = rb_ary_new();
		despotify_artist *a;
		for (a = track->real->artist; a; a = a->next)
			rb_ary_push(ids, rb_str_new2(a->id));

		return ids;
	} else if(!strcmp(keyval, "file_id"))
	    return rb_str_new2(track->real->file_id);
	else if(!strcmp(keyval, "album_id"))
	    return rb_str_new2(track->real->album_id);
	else if(!strcmp(keyval, "cover_id"))
	    return rb_str_new2(track->real->cover_id);
	else if(!strcmp(keyval, "playable"))
		return BOOL2VALUE(track->real->playable);
	else if(!strcmp(keyval, "popularity"))
		return rb_float_new(track->real->popularity);

	return Qnil;
}

#define TRACK_METADATA_ADD(hash, key) \
	keyval = rb_str_new2(key); \
	rb_hash_aset(hash, keyval, rb_despotify_track_lookup(self, keyval))

static VALUE
rb_despotify_track_metadata(VALUE self) {
	TRACK_METHOD_HEADER
	VALUE metadata;
	VALUE keyval;

	metadata = rb_hash_new();
	TRACK_METADATA_ADD(metadata, "artist");
	TRACK_METADATA_ADD(metadata, "album");
	TRACK_METADATA_ADD(metadata, "title");
	TRACK_METADATA_ADD(metadata, "length");
	TRACK_METADATA_ADD(metadata, "tracknumber");
	TRACK_METADATA_ADD(metadata, "year");
	TRACK_METADATA_ADD(metadata, "id");
	TRACK_METADATA_ADD(metadata, "artist_id");
	TRACK_METADATA_ADD(metadata, "album_id");
	TRACK_METADATA_ADD(metadata, "file_id");
	TRACK_METADATA_ADD(metadata, "cover_id");
	TRACK_METADATA_ADD(metadata, "playable");

	return metadata;
}



VALUE
Init_despotify_track(VALUE mDespotify) {
	VALUE c;

	/* Despotify::Track */
	c = rb_define_class_under(mDespotify, "Track", rb_cObject);

	/* Remove new function until we can request track by id */
	rb_undef_method (rb_singleton_class (c), "new");

	rb_define_method(c, "[]", rb_despotify_track_lookup, 1);
	rb_define_method(c, "metadata", rb_despotify_track_metadata, 0);

	return c;
}

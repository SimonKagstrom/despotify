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

#define PLAYLIST_METHOD_HEADER \
	rb_ds_playlist *pls; \
	VALUE2PLAYLIST(self, pls);


static void
rb_ds_playlist_free (rb_ds_playlist *pl)
{
	if (!pl->ischild)
		despotify_free_playlist(pl->real);

	free(pl);
}

static VALUE
rb_ds_playlist_alloc (VALUE klass)
{
	rb_ds_playlist *pls;
	VALUE obj;

	obj = Data_Make_Struct(klass, rb_ds_playlist, NULL,
	                       rb_ds_playlist_free, pls);

	return obj;
}

VALUE
PLAYLIST2VALUE (VALUE session, ds_playlist_t *pl, bool ischild)
{
	rb_ds_playlist *pls;
	VALUE obj;

	if (!pl)
		return Qnil;

	obj = rb_ds_playlist_alloc(cPlaylist);
	VALUE2PLAYLIST(obj, pls);

	pls->real = pl;
	pls->ischild = ischild;

	rb_iv_set(obj, "session", session);

	return obj;
}

static VALUE
rb_ds_playlist_new (VALUE self, VALUE sessionv, VALUE id)
{
	rb_ds_session *session;
	rb_ds_playlist *pls;
	ds_playlist_t *pl;
	char *playlist_id;

	VALUE2PLAYLIST(self, pls);
	VALUE2SESSION(sessionv, session);
	playlist_id = StringValuePtr(id);

	if (strlen(playlist_id) != 34)
		return Qnil;

	pl = despotify_get_playlist(session->real, playlist_id);
	if (!pl)
		return Qnil;

	pls->real = pl;
	pls->ischild = false;

	rb_iv_set(self, "session", sessionv);

	if (rb_block_given_p())
		rb_yield(self);

	return self;
}


static VALUE
rb_ds_playlist_tracks (VALUE self)
{
	PLAYLIST_METHOD_HEADER
	ds_track_t *t;
	VALUE tracks;

	if (rb_iv_get(self, "tracks") == Qnil) {
		tracks = rb_ary_new();

		for(t = pls->real->tracks; t; t = t->next) {
			rb_ary_push(tracks, TRACK2VALUE(t, true));
		}

		rb_iv_set(self, "tracks", tracks);
	}

	return rb_iv_get(self, "tracks");
}


static VALUE
rb_ds_playlist_name (VALUE self)
{
	PLAYLIST_METHOD_HEADER

	return rb_str_new2(pls->real->name);
}


static VALUE
rb_ds_playlist_author (VALUE self)
{
	PLAYLIST_METHOD_HEADER

	return rb_str_new2(pls->real->author);
}

static VALUE
rb_ds_playlist_id (VALUE self)
{
	PLAYLIST_METHOD_HEADER

	return rb_str_new2(pls->real->playlist_id);
}

static VALUE
rb_ds_playlist_to_uri (VALUE self)
{
	PLAYLIST_METHOD_HEADER

	char buf[1024];

	despotify_playlist_to_uri(pls->real, buf);

	return rb_str_new2(buf);
}

static VALUE
rb_ds_playlist_rename (VALUE self, VALUE name)
{
	PLAYLIST_METHOD_HEADER

	rb_ds_session *session;
	VALUE sessionv;
	char *newname;

	sessionv = rb_iv_get(self, "session");
	VALUE2SESSION(sessionv, session);
	newname = StringValuePtr(name);

	return BOOL2VALUE(despotify_rename_playlist(session->real, pls->real, newname));
}


VALUE
Init_Playlist (VALUE mDespotify)
{
	VALUE c;

	/* Despotify::Playlist */
	c = rb_define_class_under(mDespotify, "Playlist", rb_cObject);
	rb_define_alloc_func (c, rb_ds_playlist_alloc);

	rb_define_method(c, "initialize", rb_ds_playlist_new, 2);
	rb_define_method(c, "tracks", rb_ds_playlist_tracks, 0);
	rb_define_method(c, "name", rb_ds_playlist_name, 0);
	rb_define_method(c, "name=", rb_ds_playlist_rename, 1);
	rb_define_method(c, "author", rb_ds_playlist_author, 0);
	rb_define_method(c, "id", rb_ds_playlist_id, 0);
	rb_define_method(c, "to_uri", rb_ds_playlist_to_uri, 0);
	rb_define_method(c, "rename", rb_ds_playlist_rename, 1);

	return c;
}

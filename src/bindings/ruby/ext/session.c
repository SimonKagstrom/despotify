/*
 * $Id$
 */

#include <ruby.h>
#include <despotify.h>

#include "main.h"
#include "album.h"
#include "album_browse.h"
#include "artist.h"
#include "artist_browse.h"
#include "playlist.h"
#include "search.h"
#include "session.h"
#include "track.h"

#define SESSION_METHOD_HEADER \
	rb_ds_session *session; \
	VALUE2SESSION(self, session); \
	if (!session->connected) \
		rb_raise(eDespotifyError, "session is not connected");

static void rb_ds_session_callback (ds_session_t *session, int signal,
                                    void *data, void *udata);


static void
rb_ds_session_free (rb_ds_session *session)
{
	if (session->rootpl)
		despotify_free_playlist(session->rootpl);

	if (session->real)
		despotify_exit(session->real);

	free(session);
}

static VALUE
rb_ds_session_alloc (VALUE klass)
{
	rb_ds_session *session;
	VALUE obj;

	obj = Data_Make_Struct(klass, rb_ds_session, NULL,
	                       rb_ds_session_free, session);

	return obj;
}

void
rb_ds_session_error (VALUE session)
{
	rb_ds_session *sessionptr;
	VALUE2SESSION(session, sessionptr);

	rb_raise(eDespotifyError, sessionptr->real->last_error);
}

static VALUE
rb_ds_session_new (VALUE self)
{
	rb_ds_session *session;
	VALUE2SESSION(self, session);

	if (!(session->real = despotify_init_client(rb_ds_session_callback, (void *) self))) {
		rb_raise(rb_eNoMemError, "failed to allocate memory");
		return Qnil;
	}

	session->rootpl = NULL;
	session->connected = false;

	if (rb_block_given_p())
		rb_yield(self);

	return self;
}

static VALUE
rb_ds_session_authenticate (VALUE self, VALUE username, VALUE password)
{
	rb_ds_session *session;
	VALUE2SESSION(self, session);

	if (!despotify_authenticate(session->real, StringValuePtr(username),
	                            StringValuePtr(password)))
		rb_ds_session_error(self);
	else
		session->connected = true;

	return self;
}

static VALUE
rb_ds_session_stored_playlists (VALUE self)
{
	SESSION_METHOD_HEADER
	ds_playlist_t *pl;
	VALUE playlists;

	playlists = rb_ary_new();

	if (!session->rootpl)
		session->rootpl = despotify_get_stored_playlists(session->real);

	if (!session->rootpl)
		return playlists;

	for (pl = session->rootpl; pl; pl = pl->next) {
		rb_ary_push(playlists, PLAYLIST2VALUE(self, pl, true));
	}

	return playlists;
}

static VALUE
rb_ds_session_get_image (VALUE self, VALUE image_id)
{
	SESSION_METHOD_HEADER

	VALUE rval;
	int len;
	void *data;
	char *id;

	id = StringValuePtr(image_id);
	data = despotify_get_image(session->real, id, &len);

	rval = rb_str_new((char *) data, len);
	free(data);

	return rval;
}


static VALUE
rb_ds_session_user_info (VALUE self) {
	SESSION_METHOD_HEADER
	VALUE userinfo;
	ds_user_info_t *info;

	info = session->real->user_info;
	userinfo = rb_hash_new();

	if (info) {
		HASH_VALUE_ADD(userinfo, "username", rb_str_new2(info->username));
		HASH_VALUE_ADD(userinfo, "country", rb_str_new2(info->country));
		HASH_VALUE_ADD(userinfo, "type", rb_str_new2(info->type));
		HASH_VALUE_ADD(userinfo, "expiry", INT2FIX(info->expiry));
		HASH_VALUE_ADD(userinfo, "server_host", rb_str_new2(info->server_host));
		HASH_VALUE_ADD(userinfo, "server_port", INT2FIX(info->server_port));
		HASH_VALUE_ADD(userinfo, "last_ping", INT2FIX(info->last_ping));
	}

	return userinfo;
}

static VALUE
rb_ds_session_play (VALUE self, VALUE track)
{
	SESSION_METHOD_HEADER
	rb_ds_track *t;

	VALUE2TRACK(track, t);

	return BOOL2VALUE(despotify_play(session->real, t->real, false));
}

static VALUE
rb_ds_session_play_playlist (VALUE self, VALUE playlist)
{
	SESSION_METHOD_HEADER
	rb_ds_playlist *pl;

	VALUE2PLAYLIST(playlist, pl);

	return BOOL2VALUE(despotify_play(session->real, pl->real->tracks, true));
}

static VALUE
rb_ds_session_stop (VALUE self)
{
	SESSION_METHOD_HEADER

	return BOOL2VALUE(despotify_stop(session->real));
}

static VALUE
rb_ds_session_pause (VALUE self)
{
	SESSION_METHOD_HEADER

	return BOOL2VALUE(despotify_pause(session->real));
}

static VALUE
rb_ds_session_resume (VALUE self)
{
	SESSION_METHOD_HEADER

	return BOOL2VALUE(despotify_resume(session->real));
}

static VALUE
rb_ds_session_current_track (VALUE self)
{
	SESSION_METHOD_HEADER

	return TRACK2VALUE(despotify_get_current_track(session->real), false);
}

static VALUE
rb_ds_session_get_error (VALUE self)
{
	rb_ds_session *session;
	VALUE2SESSION(self, session);

	if (session->real->last_error)
		return rb_str_new2(session->real->last_error);

	return Qnil;
}

static VALUE
rb_ds_session_playlist (VALUE self, VALUE id)
{
	VALUE args[2] = { self, id };

	return rb_class_new_instance(2, args, cPlaylist);
}

static VALUE
rb_ds_session_artist (VALUE self, VALUE id)
{
	VALUE args[2] = { self, id };

	return rb_class_new_instance(2, args, cArtistBrowse);
}

static VALUE
rb_ds_session_album (VALUE self, VALUE id)
{
	VALUE args[2] = { self, id };

	return rb_class_new_instance(2, args, cAlbumBrowse);
}

static VALUE
rb_ds_session_search (VALUE self, VALUE searchterm, VALUE maxresults)
{
	VALUE args[3] = { self, searchterm, maxresults };

	return rb_class_new_instance(3, args, cSearch);
}

static VALUE
rb_ds_session_track (VALUE self, VALUE id)
{
	VALUE args[2] = { self, id };

	return rb_class_new_instance(2, args, cTrack);
}

static VALUE
rb_ds_session_uri (VALUE self, VALUE uri)
{
	SESSION_METHOD_HEADER

	ds_link_t *link;
	VALUE rval;

	link = despotify_link_from_uri(StringValuePtr(uri));

	switch(link->type) {
		case LINK_TYPE_ALBUM:
			rval = ALBUMBROWSE2VALUE(despotify_link_get_album(
			                         session->real, link), false);
			break;

		case LINK_TYPE_ARTIST:
			rval = ARTISTBROWSE2VALUE(despotify_link_get_artist(
			                          session->real, link), false);
			break;

		case LINK_TYPE_PLAYLIST:
			rval = PLAYLIST2VALUE(self, despotify_link_get_playlist(
			                      session->real, link), false);
			break;

		case LINK_TYPE_SEARCH:
			rval = SEARCH2VALUE(self, despotify_link_get_search(
			                    session->real, link));
			break;

		case LINK_TYPE_TRACK:
			rval = TRACK2VALUE(despotify_link_get_track(
			                   session->real, link), false);
			break;

		default:
			rval = Qnil;
	}

	despotify_free_link(link);

	return rval;
}

static VALUE
rb_ds_session_signal (VALUE self, VALUE signal)
{
	VALUE allcallbacks;
	VALUE callback;
	VALUE callbacks;

	if (!rb_block_given_p())
		return Qnil;

	allcallbacks = rb_iv_get(self, "callbacks");

	if (allcallbacks == Qnil) {
		allcallbacks = rb_hash_new();

		rb_iv_set(self, "callbacks", allcallbacks);
	}

	callbacks = rb_hash_aref(allcallbacks, signal);

	if (callbacks == Qnil) {
		callbacks = rb_ary_new();

		rb_hash_aset(allcallbacks, signal, callbacks);
	}

	callback = rb_block_proc();
	rb_ary_push(callbacks, callback);

	return self;
}

static void
rb_ds_session_callback (ds_session_t *session, int signal,
                        void *data, void *udata)
{
	VALUE allcallbacks;
	VALUE callback;
	VALUE callbacks;
	VALUE calldata;
	VALUE self;

	int i, hasdata;

	self = (VALUE) udata;

	allcallbacks = rb_iv_get(self, "callbacks");

	if (allcallbacks == Qnil)
		return;

	callbacks = rb_hash_aref(allcallbacks, INT2FIX(signal));

	if (callbacks == Qnil)
		return;

	switch (signal) {
		default:
			hasdata = 0;
			break;
	}

	for (i = 0; i < RARRAY(callbacks)->len; i++) {
		callback = rb_ary_entry(callbacks, i);

		if (hasdata > 0)
			rb_funcall(callback, rb_intern("call"), hasdata, calldata);
		else
			rb_funcall(callback, rb_intern("call"), 0);
	}
}


VALUE
Init_Session (VALUE mDespotify)
{
	VALUE c;

	/* Despotify::Session */
	c = rb_define_class_under(mDespotify, "Session", rb_cObject);
	rb_define_alloc_func (c, rb_ds_session_alloc);
	rb_define_method(c, "initialize", rb_ds_session_new, 0);
	rb_define_method(c, "authenticate", rb_ds_session_authenticate, 2);
	rb_define_method(c, "playlists", rb_ds_session_stored_playlists, 0);
	rb_define_method(c, "stored_playlists", rb_ds_session_stored_playlists, 0);
	rb_define_method(c, "get_image", rb_ds_session_get_image, 1);
	rb_define_method(c, "get_error", rb_ds_session_get_error, 0);
	rb_define_method(c, "user_info", rb_ds_session_user_info, 0);

	rb_define_method(c, "play", rb_ds_session_play, 1);
	rb_define_method(c, "play_playlist", rb_ds_session_play_playlist, 1);
	rb_define_method(c, "stop", rb_ds_session_stop, 0);
	rb_define_method(c, "pause", rb_ds_session_pause, 0);
	rb_define_method(c, "resume", rb_ds_session_resume, 0);
	rb_define_method(c, "current_track", rb_ds_session_current_track, 0);
	rb_define_method(c, "signal", rb_ds_session_signal, 1);

	/* Shortcuts */
	rb_define_method(c, "playlist", rb_ds_session_playlist, 1);
	rb_define_method(c, "artist", rb_ds_session_artist, 1);
	rb_define_method(c, "album", rb_ds_session_album, 1);
	rb_define_method(c, "track", rb_ds_session_track, 1);
	rb_define_method(c, "search", rb_ds_session_search, 2);
	rb_define_method(c, "uri", rb_ds_session_uri, 1);


	return c;
}

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


static VALUE
rb_despotify_id2uri (VALUE self, VALUE idv)
{
	char *id;
	char buf[128];

	id = StringValuePtr(idv);

	despotify_id2uri(id, buf);

	return rb_str_new2(buf);
}

static VALUE
rb_despotify_uri2id (VALUE self, VALUE uriv)
{
	char *uri;
	char buf[128];

	uri = StringValuePtr(uriv);

	despotify_uri2id(uri, buf);

	return rb_str_new2(buf);
}

void
Init_despotify (void)
{
	if (!despotify_init()) {
		printf("despotify_init() failed\n");
		return;
	}

	VALUE mDespotify = rb_define_module("Despotify");

	rb_define_singleton_method(mDespotify, "id2uri", rb_despotify_id2uri, 1);
	rb_define_singleton_method(mDespotify, "uri2id", rb_despotify_uri2id, 1);
	rb_define_const(mDespotify, "TRACK_CHANGE", INT2FIX(DESPOTIFY_TRACK_CHANGE));
	rb_define_const(mDespotify, "MAX_SEARCH_RESULTS", INT2FIX(MAX_SEARCH_RESULTS));

	cAlbum        = Init_Album(mDespotify);
	cAlbumBrowse  = Init_AlbumBrowse(mDespotify);
	cArtist       = Init_Artist(mDespotify);
	cArtistBrowse = Init_ArtistBrowse(mDespotify);
	cPlaylist     = Init_Playlist(mDespotify);
	cSearch       = Init_Search(mDespotify);
	cSession      = Init_Session(mDespotify);
	cTrack        = Init_Track(mDespotify);

	eDespotifyError = rb_define_class_under(mDespotify, "DespotifyError", rb_eException);
}

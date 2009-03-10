#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "despotify.h"

#include "auth.h"
#include "buffer.h"
#include "commands.h"
#include "handlers.h"
#include "keyexchange.h"
#include "network.h"
#include "packet.h"
#include "playlist.h"
#include "session.h"
#include "util.h"

static int despotify_handle_packet(struct despotify_session* ds);


bool despotify_init()
{
    if (network_init() != 0)
        return false;
    return true;
}

bool despotify_cleanup()
{
    if (network_cleanup() != 0)
        return false;
    return true;
}

struct despotify_session* despotify_init_client()
{
    struct despotify_session* ds = calloc(1,sizeof(struct despotify_session));
    if (!ds)
        return NULL;

    ds->session = session_init_client();
    if (!ds->session)
        return NULL;

    return ds;
}

bool despotify_authenticate(struct despotify_session* ds,
                            const char* user,
                            const char* password)
{
    assert(ds != NULL && ds->session != NULL);

    session_auth_set(ds->session, user, password);

    if (session_connect(ds->session) < 0)
    {
        ds->last_error = "Could not connect to server.";
        return false;
    }
    DSFYDEBUG("%s", "session_connect() completed\n");

    if (do_key_exchange(ds->session) < 0)
    {
        ds->last_error = "Key exchange failed.";
        return false;
    }
    DSFYDEBUG("%s", "do_key_exchange() completed\n");

    auth_generate_auth_hash(ds->session);
    key_init(ds->session);

    if (do_auth(ds->session) < 0)
    {
        ds->last_error = "Authentication failed.";
        return false;
    }
    DSFYDEBUG("%s", "do_auth() completed\n");

    /* handle all packets until we get the big welcome */
    while (!ds->session->welcomed) {
        if (despotify_handle_packet(ds))
            return false;
    }
    
    return true;
}

void despotify_exit(struct despotify_session* ds)
{
    despotify_free(ds, true);
}

void despotify_free(struct despotify_session* ds, bool should_disconnect)
{
    assert(ds != NULL && ds->session != NULL);

    if (should_disconnect)
        session_disconnect(ds->session);

    session_free(ds->session);
    free(ds);
}

const char* despotify_get_error(struct despotify_session* ds)
{
    const char* error;

    /* Only session_init_client() failing can cause this. */
    if (!ds)
        return "Could not allocate memory for a new session.";

    error = ds->last_error;
    ds->last_error = NULL;

    return error;
}

struct search
{
    struct buffer* response;
    struct playlist* playlist;
};

static int despotify_search_callback(CHANNEL*  ch, 
                                     unsigned char* buf,
                                     unsigned short len)
{
    struct search* s = (struct search*) ch->private;

    switch (ch->state) {
        case CHANNEL_HEADER:
            /* ignore unknown data */
            return 0;

        case CHANNEL_DATA:
            /* Skip a minimal gzip header */
            if (ch->total_data_len < 10) {
		int skip_len = 10 - ch->total_data_len;
		while (skip_len && len) {
                    skip_len--;
                    len--;
                    buf++;
		}

		if (len == 0)
                    return 0;
            }

            buffer_check_and_extend(s->response, len);
            buffer_append_raw(s->response, buf, len);
            break;
            
        case CHANNEL_END:
            s->playlist->flags |= PLAYLIST_LOADED;

            /* Add tracks */
            playlist_track_update_from_gzxml(s->playlist,
                                             s->response->buf,
                                             s->response->buflen);
            
            /* Since this is a newly added playlist
               we know it's the first one */
            playlist_select (1);
            break;

        case CHANNEL_ERROR:
            return 1;

        default:
            /* unknown state */
            return 2;
    }

    return 0;
}

struct playlist* despotify_search(struct despotify_session* ds,
                                  char* searchtext)
{
    struct search s;
    s.response = buffer_init();
    s.playlist = playlist_new();

    char buf[80];
    snprintf(buf, sizeof buf, "Search: %s", searchtext);
    buf[(sizeof buf)-1] = 0;
    playlist_set_name(s.playlist, buf);
    playlist_set_author(s.playlist, ds->session->username);

    int ret = cmd_search(ds->session, searchtext, 
                         despotify_search_callback, &s);
    if (ret) {
        ds->last_error = "cmd_search() failed";
        return NULL;
    }

    do {
        if (despotify_handle_packet(ds))
            return NULL;
    } while (!(s.playlist->flags & PLAYLIST_LOADED));
    
    buffer_free(s.response);

    return s.playlist;
}

void despotify_free_playlist(struct playlist* playlist)
{
    playlist_free(playlist, 1);
}

static int despotify_handle_packet(struct despotify_session* ds)
{
    SESSION* s = ds->session;
    PHEADER hdr;
    unsigned char* payload;

    int err = packet_read(s, &hdr, &payload);
    if (!err) {
        err = handle_packet(s, hdr.cmd, payload, hdr.len);
        DSFYfree(payload); /* Allocated in packet_read() */
    }

    return err;
}

struct playlist_request
{
    struct buffer* response;
    struct playlist *playlist;
    unsigned char *track_id_list;
};

static int despotify_playlist_callback (CHANNEL *ch, unsigned char *buf,
                                          unsigned short len)
{
	struct playlist_request *r = (struct playlist_request *) ch->private;
	int skip_len;

	switch (ch->state) {
	case CHANNEL_DATA:
		/* In case of retrieving track meta info, skip a minimal gzip header */
		if (r->track_id_list && ch->total_data_len < 10) {
			skip_len = 10 - ch->total_data_len;
			while (skip_len && len) {
				skip_len--;
				len--;
				buf++;
			}

			if (len == 0)
				break;
		}

                buffer_check_and_extend (r->response, len);
		buffer_append_raw (r->response, buf, len);
		break;

	case CHANNEL_ERROR:
		if (r->track_id_list) // PLAYLIST_BROWSE_ERROR
                {
                    r->playlist->flags |= PLAYLIST_TRACKS_ERROR;
                    DSFYfree (r->track_id_list);
                }
		else if (r->playlist != NULL) // PLAYLIST_TRACKS_ERROR
                    r->playlist->flags |= PLAYLIST_ERROR;
                /* else PLAYLIST_LIST_ERROR */

                buffer_free (r->response);

		break;

	case CHANNEL_END:
		if (r->track_id_list)
                {

                    playlist_track_update_from_gzxml (r->playlist,
                                  r->response->buf,
                                  r->response->buflen);

                    DSFYfree (r->track_id_list);
                }
                else
                {
                    playlist_create_from_xml ((char *)r->response->buf,
                                              r->playlist);
                }
                buffer_free (r->response);

		break;

	default:
		break;
	}

	return 0;
}

bool despotify_get_playlists(struct despotify_session *ds)
{
    int err;
    struct playlist_request r;

    r.response = buffer_init ();
    r.playlist = NULL;
    r.track_id_list = NULL;

    buffer_append_raw (r.response, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<playlist>\n", 51);
    if ((err = cmd_getplaylist (ds->session, (unsigned char *)
                    PLAYLIST_LIST_PLAYLISTS, -1,
                    despotify_playlist_callback,
                    (void *) &r)) != 0) {
        buffer_free (r.response);

        ds->last_error = "Network error.";
        session_disconnect(ds->session);

        return false;
    }

    ds->playlists = playlist_root();
    return true;
}

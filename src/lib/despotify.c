#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "despotify.h"

#include "auth.h"
#include "keyexchange.h"
#include "session.h"

#define panic(...) { printf(__VA_ARGS__); abort(); } while(0)

despotify_session *despotify_new_session()
{
    despotify_session *session = (despotify_session *) malloc(sizeof (despotify_session));
    if (!session)
        return NULL;

    memset(session, 0, sizeof (despotify_session));

    session->session = session_init_client ();
    if (!session->session)
        return NULL;

    return session;
}

bool despotify_authenticate(despotify_session *session, const char *user, const char *password)
{
    assert(session != NULL && session->session != NULL);

    session_auth_set (session->session, user, password);

    if (session_connect (session->session) < 0)
    {
        session->last_error = "Could not connect to server.";
        return false;
    }

    if (do_key_exchange (session->session) < 0)
    {
        session->last_error = "Key exchange failed.";
        return false;
    }

    auth_generate_auth_hash (session->session);
    key_init (session->session);

    if (do_auth (session->session) < 0)
    {
        session->last_error = "Authentication failed.";
        return false;
    }

    return true;
}

void despotify_close(despotify_session *session)
{
    despotify_free(session, true);
}

void despotify_free(despotify_session *session, bool should_disconnect)
{
    assert(session != NULL && session->session != NULL);

    if (should_disconnect)
        session_disconnect (session->session);

    session_free (session->session);
    free (session);
}

const char *despotify_get_error(despotify_session *session)
{
    const char *error;
    /* Only session_init_client() failing can cause this. */
    if (!session)
        return "Could not allocate memory for a new session.";

    error = session->last_error;
    session->last_error = NULL;

    return error;
}

/* Information. */
TRACK *despotify_get_currently_playing(despotify_session *session)
{
    panic("despotify_get_currently_playing() not implemented!\n");
    return NULL;
}
/* We need to determine if there is any / enough info to warrant this:
 * user despotify_get_user_info(despotify_session *session); */

/* Playlist handling. */
PLAYLIST *despotify_search(despotify_session *session, const char *terms)
{
    panic("despotify_search() not implemented!\n");
    return NULL;
}
PLAYLIST **despotify_get_playlists(despotify_session *session)
{
    panic("despotify_get_playlists() not implemented!\n");
    return NULL;
}
bool despotify_append_song(despotify_session *session, PLAYLIST *playlist, TRACK *song)
{
    panic("despotify_append_song() not implemented!\n");
    return false;
}
bool despotify_remove_song(despotify_session *session, PLAYLIST *playlist, TRACK *song)
{
    panic("despotify_remove_song() not implemented!\n");
    return false;
}
bool despotify_delete_playlist(despotify_session *session, PLAYLIST *playlist)
{
    panic("despotify_delete_playlist() not implemented!\n");
    return false;
}
PLAYLIST *despotify_create_playlist(despotify_session *session, const char *name)
{
    panic("despotify_create_playlist() not implemented!\n");
    return NULL;
}
bool despotify_rename_playlist(despotify_session *session, PLAYLIST *playlist, const char *new_name)
{
    panic("despotify_rename_playlist() not implemented!\n");
    return false;
}
PLAYLIST *despotify_free_playlist(despotify_session *session, PLAYLIST *playlist)
{
    panic("despotify_free_playlist() not implemented!\n");
    return NULL;
}

/* Playback control. */
bool despotify_stop(despotify_session *session)
{
    panic("despotify_stop() not implemented!\n");
    return false;
}
bool despotify_pause(despotify_session *session)
{
    panic("despotify_pause() not implemented!\n");
    return false;
}
bool despotify_resume(despotify_session *session)
{
    panic("despotify_resume() not implemented!\n");
    return false;
}

bool despotify_play(despotify_session *session, TRACK *song)
{
    panic("despotify_play() not implemented!\n");
    return false;
}

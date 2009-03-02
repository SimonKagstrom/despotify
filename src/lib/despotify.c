#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "despotify.h"

#include "auth.h"
#include "keyexchange.h"
#include "session.h"

#define panic(...) { printf(__VA_ARGS__); abort(); } while(0)

connection *despotify_new_connection()
{
    connection *conn = (connection *) malloc(sizeof (connection));
    if (!conn)
        return NULL;

    memset(conn, 0, sizeof (connection));

    conn->session = session_init_client ();
    if (!conn->session)
        return NULL;

    return conn;
}

BOOL despotify_authenticate(connection *conn, const char *user, const char *password)
{
    assert(conn != NULL && conn->session != NULL);

    session_auth_set (conn->session, user, password);

    if (session_connect (conn->session) < 0)
    {
        conn->last_error = "Could not connect to server.";
        return FALSE;
    }

    if (do_key_exchange (conn->session) < 0)
    {
        conn->last_error = "Key exchange failed.";
        return FALSE;
    }

    auth_generate_auth_hash (conn->session);
    key_init (conn->session);
    
    if (do_auth (conn->session) < 0)
    {
        conn->last_error = "Authentication failed.";
        return FALSE;
    }

    return TRUE;
}

void despotify_close(connection *conn)
{
    despotify_free(conn, TRUE);
}

void despotify_free(connection *conn, BOOL should_disconnect)
{
    assert(conn != NULL && conn->session != NULL);

    if (should_disconnect)
        session_disconnect (conn->session);

    session_free (conn->session);
    free (conn);
}

const char *despotify_get_error(connection *conn)
{
    const char *error;
    /* Only session_init_client() failing can cause this. */
    if (!conn)
        return "Could not allocate memory for a new connection/session.";

    error = conn->last_error;
    conn->last_error = NULL;

    return error;
}

/* Information. */
TRACK *despotify_get_currently_playing(connection *conn)
{
    panic("despotify_get_currently_playing() not implemented!\n");
    return NULL;
}
/* We need to determine if there is any / enough info to warrant this:
 * user despotify_get_user_info(connection *conn); */

/* Playlist handling. */
PLAYLIST *despotify_search(connection *conn, const char *terms)
{
    panic("despotify_search() not implemented!\n");
    return NULL;
}
PLAYLIST **despotify_get_playlists(connection *conn)
{
    panic("despotify_get_playlists() not implemented!\n");
    return NULL;
}
BOOL despotify_append_song(connection *conn, PLAYLIST *playlist, TRACK *song)
{
    panic("despotify_append_song() not implemented!\n");
    return FALSE;
}
BOOL despotify_remove_song(connection *conn, PLAYLIST *playlist, TRACK *song)
{
    panic("despotify_remove_song() not implemented!\n");
    return FALSE;
}
BOOL despotify_delete_playlist(connection *conn, PLAYLIST *playlist)
{
    panic("despotify_delete_playlist() not implemented!\n");
    return FALSE;
}
PLAYLIST *despotify_create_playlist(connection *conn, const char *name)
{
    panic("despotify_create_playlist() not implemented!\n");
    return NULL;
}
BOOL despotify_rename_playlist(connection *conn, PLAYLIST *playlist, const char *new_name)
{
    panic("despotify_rename_playlist() not implemented!\n");
    return FALSE;
}
PLAYLIST *despotify_free_playlist(connection *conn, PLAYLIST *playlist)
{
    panic("despotify_free_playlist() not implemented!\n");
    return NULL;
}

/* Playback control. */
BOOL despotify_stop(connection *conn)
{
    panic("despotify_stop() not implemented!\n");
    return FALSE;
}
BOOL despotify_pause(connection *conn)
{
    panic("despotify_pause() not implemented!\n");
    return FALSE;
}
BOOL despotify_resume(connection *conn)
{
    panic("despotify_resume() not implemented!\n");
    return FALSE;
}

BOOL despotify_play(connection *conn, TRACK *song)
{
    panic("despotify_play() not implemented!\n");
    return FALSE;
}

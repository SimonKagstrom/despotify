#ifndef LIBDESPOTIFY_H
#define LIBDESPOTIFY_H

#include "session.h"
/* This is for track and playlist structs. */
#include "playlist.h"

#ifndef FALSE
# define FALSE 0
#endif

#ifndef TRUE
# define TRUE 1
#endif

#ifndef BOOL
# define BOOL int
#endif

typedef struct 
{
    SESSION *session;
    const char *last_error;
} connection;

/* Session stuff. */
connection *despotify_new_connection();
BOOL despotify_authenticate(connection *conn, const char *user, const char *password);

#define despotify_change_user(conn, user, password) { \
                        despotify_close(conn); \
                        (conn) = despotify_new_connection(); \
                        despotify_authenticate(conn, user, password); \
                    } while (0)

void despotify_close(connection *conn);
void despotify_free(connection *conn, BOOL should_disconnect);

const char *despotify_get_error(connection *conn);

/* Information. */
TRACK *despotify_get_currently_playing(connection *conn);
/* We need to determine if there is any / enough info to warrant this:
 * user despotify_get_user_info(connection *conn); */

/* Playlist handling. */
PLAYLIST *despotify_search(connection *conn, const char *terms);
PLAYLIST **despotify_get_playlists(connection *conn);
BOOL despotify_append_song(connection *conn, PLAYLIST *playlist, TRACK *song);
BOOL despotify_remove_song(connection *conn, PLAYLIST *playlist, TRACK *song);
BOOL despotify_delete_playlist(connection *conn, PLAYLIST *playlist);
PLAYLIST *despotify_create_playlist(connection *conn, const char *name);
BOOL despotify_rename_playlist(connection *conn, PLAYLIST *playlist, const char *new_name);
PLAYLIST *despotify_free_playlist(connection *conn, PLAYLIST *playlist);

/* Playback control. */
BOOL despotify_stop(connection *conn);
BOOL despotify_pause(connection *conn);
BOOL despotify_resume(connection *conn);

BOOL despotify_play(connection *conn, TRACK *song);

#endif

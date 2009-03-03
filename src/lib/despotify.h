#ifndef LIBDESPOTIFY_H
#define LIBDESPOTIFY_H

#include <stdbool.h>
#include "session.h"
/* This is for track and playlist structs. */
#include "playlist.h"

typedef struct 
{
    SESSION *session;
    const char *last_error;
} despotify_session;

/* Session stuff. */
despotify_session *despotify_new_session();
bool despotify_authenticate(despotify_session *session, const char *user, const char *password);

#define despotify_change_user(session, user, password) { \
                        despotify_close(session); \
                        (session) = despotify_new_session(); \
                        despotify_authenticate(session, user, password); \
                    } while (0)

void despotify_close(despotify_session *session);
void despotify_free(despotify_session *session, bool should_disconnect);

const char *despotify_get_error(despotify_session *session);

/* Information. */
TRACK *despotify_get_currently_playing(despotify_session *session);
/* We need to determine if there is any / enough info to warrant this:
 * user despotify_get_user_info(despotify_session *session); */

/* Playlist handling. */
PLAYLIST *despotify_search(despotify_session *session, const char *terms);
PLAYLIST **despotify_get_playlists(despotify_session *session);
bool despotify_append_song(despotify_session *session, PLAYLIST *playlist, TRACK *song);
bool despotify_remove_song(despotify_session *session, PLAYLIST *playlist, TRACK *song);
bool despotify_delete_playlist(despotify_session *session, PLAYLIST *playlist);
PLAYLIST *despotify_create_playlist(despotify_session *session, const char *name);
bool despotify_rename_playlist(despotify_session *session, PLAYLIST *playlist, const char *new_name);
PLAYLIST *despotify_free_playlist(despotify_session *session, PLAYLIST *playlist);

/* Playback control. */
bool despotify_stop(despotify_session *session);
bool despotify_pause(despotify_session *session);
bool despotify_resume(despotify_session *session);

bool despotify_play(despotify_session *session, TRACK *song);

#endif

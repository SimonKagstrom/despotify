#ifndef LIBDESPOTIFY_H
#define LIBDESPOTIFY_H

/* This can be opaque, AFAIK the user doesn't need to know. ;-)
 * (if we decide he does need to know, #include <session.h> instead. */
struct SESSION;

/* This is for track and playlist structs. */
#include "playlist.h"

/* Session stuff. */
SESSION *despotify_authenticate(const char *user, const char *password);

#define despotify_change_user(session, user, password) { \
                        despotify_close(session); \
                        (session) = despotify_authenticate(user, password); \
                    } while (0)

int despotify_close(SESSION *session);

/* Information. */
track despotify_get_currently_playing(SESSION *session);
/* We need to determine if there is any / enough info to warrant this:
 * user despotify_get_user_info(SESSION *session); */

/* Playlist handling. */
PLAYLIST *despotify_search(SESSION *session, const char *terms);
PLAYLIST **despotify_get_playlists(SESSION *session);
int despotify_append_song(SESSION *session, PLAYLIST *playlist, TRACK *song);
int despotify_remove_song(SESSION *session, PLAYLIST *playlist, TRACK *song);
int despotify_delete_playlist(SESSION *session, PLAYLIST *playlist);
PLAYLIST *despotify_create_playlist(SESSION *session, const char *name);
int despotify_rename_playlist(SESSION *session, playlist, const char *new_name);
PLAYLIST *despotify_free_playlist(SESSION *session, PLAYLIST *playlist);

/* Playback control. */
int despotify_stop(SESSION *session);
int despotify_pause(SESSION *session);
int despotify_resume(SESSION *session);

int despotify_play(SESSION *session, TRACK *song);

#endif

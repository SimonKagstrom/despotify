#ifndef LIBDESPOTIFY_H
#define LIBDESPOTIFY_H

#include <pthread.h>
#include <stdbool.h>

#define STRING_LENGTH 256
#define MAX_SEARCH_RESULTS 100 /* max search results per request */

struct track
{
    bool has_meta_data;
    bool playable;
    unsigned char track_id[33];
    unsigned char file_id[41];
    unsigned char album_id[33];
    unsigned char cover_id[41];
    unsigned char *key;
    char title[STRING_LENGTH];
    struct artist* artist;
    char album[STRING_LENGTH];
    int length;
    int tracknumber;
    int year;
    float popularity;
    struct track *next; /* in case of multiple tracks
                           in an album or playlist struct */
};

struct search_result
{
    unsigned char query[STRING_LENGTH];
    unsigned char suggestion[STRING_LENGTH];
    int total_artists;
    int total_albums;
    int total_tracks;
};

struct playlist
{
    char name[STRING_LENGTH];
    char author[STRING_LENGTH];
    unsigned char playlist_id[35];
    bool is_collaborative;
    int num_tracks;
    struct track *tracks;
    struct search_result *search; /* in case the playlist is a search result */
    struct playlist *next; /* in case of multiple playlists in the root list */
};

struct album {
    char name[STRING_LENGTH];
    char id[33];
    int num_tracks;
    struct track* tracks;
    int year;
    char cover_id[41];
    float popularity;
    struct album* next; /* in case of multiple albums in an artist struct */
};

struct artist {
    char name[STRING_LENGTH];
    char id[33];
    char* text;
    char portrait_id[41];
    char genres[STRING_LENGTH];
    char years_active[STRING_LENGTH];
    float popularity;
    int num_albums;
    struct album* albums;
    struct artist* next; /* in case of multiple artists in a track struct */
};

struct despotify_session
{
    bool initialized;
    struct session* session;
    struct snd_session* snd_session;
    const char *last_error;

    /* AES CTR state */
    struct {
        unsigned int  state[4 * (10 + 1)];
        unsigned char IV[16];
        unsigned char keystream[16];
    } aes;

    pthread_t thread;

    struct album* album;
    struct artist* artist;
    struct track* track;
    struct playlist* playlist;
    struct buf* response;
    int offset;

    /* client/lib synchronization */
    pthread_mutex_t sync_mutex;
    pthread_cond_t  sync_cond;

    bool list_of_lists;
};

/* Global init / deinit library. */
bool despotify_init(void);
bool despotify_cleanup(void);

/* Session stuff. */
struct despotify_session *despotify_init_client(void);

void despotify_exit(struct despotify_session *ds);

bool despotify_authenticate(struct despotify_session *ds, 
                            const char *user, 
                            const char *password);

#define despotify_change_user(session, user, password) \
                    do { \
                        despotify_close(session); \
                        (session) = despotify_new_session(); \
                        despotify_authenticate(session, user, password); \
                    } while (0)

void despotify_free(struct despotify_session *ds, bool should_disconnect);

const char *despotify_get_error(struct despotify_session *ds);

/* Information. */
struct track *despotify_get_current_track(struct despotify_session *ds);
struct artist* despotify_get_artist(struct despotify_session* ds,
                                    char* artist_id);
struct album* despotify_get_album(struct despotify_session* ds,
                                  char* album_id);
void despotify_free_artist(struct artist* a);
void despotify_free_album(struct album* a);
void* despotify_get_image(struct despotify_session* ds,
                          char* image_id, int* len);

/* We need to determine if there is any / enough info to warrant this:
 * user despotify_get_user_info(struct despotify_session *ds); */

/* Playlist handling. */
struct playlist* despotify_search(struct despotify_session *ds,
                                  char *searchtext);
struct playlist* despotify_search_more(struct despotify_session *ds,
                                       struct playlist *playlist);

struct playlist* despotify_get_stored_playlists(struct despotify_session *ds);
void despotify_free_playlist(struct playlist* playlist);

/* Playback control. */
bool despotify_play(struct despotify_session *ds,
                    struct playlist* playlist,
                    struct track *song);
bool despotify_stop(struct despotify_session *ds);
bool despotify_pause(struct despotify_session *ds);
bool despotify_resume(struct despotify_session *ds);

void despotify_id2uri(char* id, char* uri);
void despotify_uri2id(char* uri, char* id);

#endif

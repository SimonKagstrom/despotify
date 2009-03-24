/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_PLAYLIST_H
#define DESPOTIFY_PLAYLIST_H

#include <stdbool.h>

#define STRING_LENGTH 256

typedef struct track
{
	int id;
	int has_meta_data;
	unsigned char track_id[16];
	unsigned char file_id[20];
	unsigned char album_id[16];
	unsigned char artist_id[33];
	unsigned char cover_id[16];
	unsigned char *key;
	char title[STRING_LENGTH];
	char artist[STRING_LENGTH];
	char album[STRING_LENGTH];
	int length;
	int tracknumber;
	int year;
	struct track *next;
} TRACK;

enum playlist_flags
{
	PLAYLIST_LOADED = 0x01,
	PLAYLIST_ERROR = 0x02,
	PLAYLIST_TRACKS_LOADED = 0x04,
	PLAYLIST_TRACKS_ERROR = 0x08,
	PLAYLIST_SELECTED = 0x10
};
typedef struct playlist
{
	enum playlist_flags flags;
	char name[STRING_LENGTH];
	char author[STRING_LENGTH];
	unsigned char playlist_id[17];
	int num_tracks;
	struct track *tracks;
	struct playlist *next;
} PLAYLIST;

struct album {
    char name[STRING_LENGTH];
    char id[16];
    int num_tracks;
    struct track* tracks;
    int year;
    char cover_id[20];
    void* cover_jpg;
    struct album* next;
};

struct artist {
    char name[STRING_LENGTH];
    char id[16];
    char* text;
    char portrait_id[20];
    char* portrait;
    char genres[STRING_LENGTH];
    char years_active[STRING_LENGTH];
    int num_albums;
    struct album* albums;
};

#define PLAYLIST_LIST_PLAYLISTS	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

struct playlist* playlist_new(void);
void playlist_free(struct playlist* pl);

struct playlist* playlist_parse_playlist(struct playlist* pl,
                                         unsigned char* xml,
                                         int len,
                                         bool list_of_lists);

bool playlist_parse_searchlist(struct playlist* pl,
                             unsigned char* xml,
                             int len );

bool playlist_parse_artist(struct artist* a,
                           unsigned char* xml,
                           int len );

void playlist_set_name (struct playlist *, char *);
void playlist_set_author (struct playlist *, char *);


#endif

/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_PLAYLIST_H
#define DESPOTIFY_PLAYLIST_H

typedef struct track
{
	int id;
	int has_meta_data;
	unsigned char track_id[16];
	unsigned char file_id[20];
	unsigned char *key;
	char title[1024];
	char artist[1024];
	char album[1024];
	int length;
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
	char name[1024];
	char author[1024];
	unsigned char *playlist_id;
	int num_tracks;
	struct track *tracks;
	struct playlist *next;
} PLAYLIST;

#define PLAYLIST_LIST_PLAYLISTS	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

struct playlist *playlist_root (void);
struct playlist *playlist_new (void);
void playlist_free (struct playlist *, int);

int playlist_set_id (struct playlist *, unsigned char *);
void playlist_set_name (struct playlist *, char *);
void playlist_set_author (struct playlist *, char *);

struct track *playlist_track_add (struct playlist *, unsigned char *);
void playlist_track_del (struct playlist *, unsigned char *);

struct track *playlist_next_track (struct playlist *, struct track *);
struct track *playlist_next_playable (struct playlist *, struct track *);

struct playlist *playlist_select (int);
struct playlist *playlist_selected (void);

int playlist_create_from_xml (char *, struct playlist *);

int playlist_track_update_from_gzxml (struct playlist *, void *, int);

struct track *tracklist_add (struct playlist *p, unsigned char *,
			     unsigned char *, char *, char *, char *, int);
void tracklist_free (struct track **, struct track *);
#endif

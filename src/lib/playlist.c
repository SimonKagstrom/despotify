/*
 * $Id$
 *
 * Playlist related stuff
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "despotify.h"
#include "ezxml.h"
#include "playlist.h"
#include "util.h"

struct playlist* playlist_parse_playlist(struct playlist* pl,
                                         unsigned char* xml,
                                         int len,
                                         bool list_of_lists)
{
    ezxml_t top = ezxml_parse_str(xml, len);
    ezxml_t tmpx = ezxml_get(top, "next-change",0, "change", 0, "ops", 0,
                             "add", 0, "items", -1);
    char* items;
    if (tmpx)
        items = tmpx->txt;
    else
        return false;

    while (*items && isspace(*items))
        items++;

    if (list_of_lists) {
        /* create list of playlists */
        struct playlist* prev = NULL;
        struct playlist* p = pl;

        for (char* id = strtok(items, ",\n"); id; id = strtok(NULL, ",\n"))
        {
            if (prev) {
                p = calloc(1, sizeof(struct playlist));
                prev->next = p;
            }
            p->playlist_id = malloc(17);
            hex_ascii_to_bytes(id, p->playlist_id, 17);
            prev = p;
        }
    }
    else {
        /* create list of tracks */
        struct track* prev = NULL;
        struct track* t = calloc(1, sizeof(struct track));
        pl->tracks = t;

        int track_count = 0;

        for (char* id = strtok(items, ",\n"); id; id = strtok(NULL, ",\n"))
        {
            if (prev) {
                t = calloc(1, sizeof(struct track));
                prev->next = t;
            }
            hex_ascii_to_bytes(id, t->track_id, sizeof(t->track_id));
            prev = t;
            track_count++;
        }
        pl->num_tracks = track_count;
    }

    tmpx = ezxml_get(top, "next-change",0, "change", 0, "user", -1);
    if (tmpx)
        DSFYstrncpy(pl->author, tmpx->txt, sizeof pl->author);

    tmpx = ezxml_get(top, "next-change",0, "change", 0, "ops", 0, "name", -1);
    if (tmpx)
        DSFYstrncpy(pl->name, tmpx->txt, sizeof pl->name);

    ezxml_free(top);
    return pl;
}

bool playlist_parse_tracks(struct playlist* pl,
                           unsigned char* xml,
                           int len )
{
    if (!pl->tracks)
        pl->tracks = calloc(1, sizeof(struct track));
    struct track* t = pl->tracks;
    struct track* prev = NULL;

    int track_count = 0;
    
    ezxml_t top = ezxml_parse_str(xml, len);
    for (ezxml_t track = ezxml_get(top, "tracks", 0, "track",-1); track; track = track->next)
    {
        if (!t) {
            t = calloc(1, sizeof(struct track));
            prev->next = t;
        }

        DSFYstrncpy(t->title, ezxml_child(track, "title")->txt, sizeof t->title);
        DSFYstrncpy(t->artist, ezxml_child(track, "artist")->txt, sizeof t->artist);
        DSFYstrncpy(t->album, ezxml_child(track, "album")->txt, sizeof t->album);
        hex_ascii_to_bytes(ezxml_child(track, "id")->txt, t->track_id, sizeof t->track_id);
        hex_ascii_to_bytes(ezxml_child(track, "album-id")->txt, t->album_id, sizeof t->album_id);
        hex_ascii_to_bytes(ezxml_child(track, "artist-id")->txt, t->artist_id, sizeof t->artist_id);
        hex_ascii_to_bytes(ezxml_child(track, "cover")->txt, t->cover_id, sizeof t->cover_id);
        t->length = atoi(ezxml_child(track, "length")->txt);
        t->tracknumber = atoi(ezxml_child(track, "track-number")->txt);
        t->year = atoi(ezxml_child(track, "year")->txt);

        ezxml_t file = ezxml_get(track, "files", 0, "file", -1);
        if (file)
            hex_ascii_to_bytes((char*)ezxml_attr(file, "id"), t->file_id, sizeof t->file_id);

        prev = t;
        t = t->next;
        track_count++;
    }
    pl->num_tracks = track_count;
    ezxml_free(top);

    return true;
}

struct playlist* playlist_new(void)
{
    return calloc(1, sizeof(struct playlist));
}

void playlist_free(struct playlist* pl)
{
    void* next_list = pl;
    for (struct playlist* p = next_list; next_list; p = next_list) {
        if (p->playlist_id)
            free(p->playlist_id);

        void* next_track = p->tracks;
        for (struct track* t = next_track; next_track; t = next_track) {
            if (t->key)
                free(t->key);
            next_track = t->next;
            free(t);
        }

        next_list = p->next;
        free(p);
    }
}

void playlist_set_name(struct playlist* pl, char* name)
{
    strcpy(pl->name, name);
}

void playlist_set_author(struct playlist* pl, char* author)
{
    strcpy(pl->author, author);
}

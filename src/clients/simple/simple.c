/*
 * A very simple despotify client, to show how the library is used.
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "despotify.h"

struct playlist* get_playlist(struct playlist* rootlist, int num)
{
    struct playlist* p = rootlist;

    if (!p) {
        printf("Stored lists not loaded. Run 'list' without parameter to load.\n");
    }
    else {
        /* skip to playlist number <num> */
        for (int i = 1; i < num && p; i++)
            p = p->next;


        if (!p)
            printf("Invalid playlist number %d\n", num);
    }

    return p;
}

void print_list_of_lists(struct playlist* rootlist)
{
    if (!rootlist) {
        printf(" <no stored playlists>\n");
    }
    else {
        int count=1;
        for (struct playlist* p = rootlist; p; p = p->next)
            printf("%2d: %-40s %3d %c %s\n", count++, p->name, p->num_tracks,
                   p->is_collaborative ? '*' : ' ', p->author);
    }
}

void print_tracks(struct track* head)
{
    if (!head) {
        printf(" <empty playlist>\n");
        return;
    }

    int count = 1;
    for (struct track* t = head; t; t = t->next) {
        if (t->has_meta_data) {
            printf("%3d: %-40s %2d:%02d ", count++, t->title,
                   t->length / 60000, t->length % 60000 / 1000);
            for (struct artist* a = t->artist; a; a = a->next)
                printf("%s%s", a->name, a->next ? ", " : "");
            printf(" %s\n", t->playable ? "" : "(Unplayable)");
        }
        else
            printf("%3d: N/A\n", count++);
    }
}

void print_info(struct despotify_session* ds)
{
    struct user_info* user = ds->user_info;
    printf("Username       : %s\n", user->username);
    printf("Country        : %s\n", user->country);
    printf("Account type   : %s\n", user->type);
    printf("Account expiry : %s", ctime(&user->expiry));
    printf("Host           : %s:%d\n", user->server_host, user->server_port);
    printf("Last ping      : %s", ctime(&user->last_ping));

    if (strncmp(user->type, "premium", 7)) {
        printf("\n=================================================\n"
               "                  N O T I C E\n"
               "       You do not have a premium account.\n"
               "     Spotify services will not be available.\n"
               "=================================================\n");
    }
}

void print_help(void)
{
    printf("\nAvailable commands:\n\n"
           "list [num]              List stored playlists\n"
           "rename [num] [string]   Rename playlist\n"
           "collab [num]            Toggle playlist collaboration\n"
           "\n"
           "search [string]         Search for [string] or get next 100 results\n"
           "artist [num]            Show information about artist for track [num]\n"
           "album [num]             List album for track [num]\n"
           "portrait [num]          Save artist portrait to portrait.jpg\n"
           "\n"
           "play [num]              Play track [num] in the last viewed list\n"
           "playalbum [num]         Play album for track [num]\n"
           "stop, pause, resume     Control playback\n"
           "\n"
           "info                    Details about your account and current connection\n"
           "help                    This text\n"
           "quit                    Quit\n");
}

void command_loop(struct despotify_session* ds)
{
    bool loop = true;
    char buf[80];
    struct playlist* rootlist = NULL;
    struct playlist* searchlist = NULL;
    struct playlist* lastlist = NULL;
    struct search_result *search = NULL;
    struct album_browse* playalbum = NULL;

    print_help();

    do {
        printf("\n> ");
        fflush(stdout);
        bzero(buf, sizeof buf);
        fgets(buf, sizeof buf -1, stdin);
        buf[strlen(buf) - 1] = 0; /* remove newline */

        /* list */
        if (!strncmp(buf, "list", 4)) {
            int num = atoi(buf + 5);
            if (num) {
                struct playlist* p = get_playlist(rootlist, num);

                if (p) {
                    print_tracks(p->tracks);
                    lastlist = p;
                }
            }
            else {
                if (!rootlist)
                    rootlist = despotify_get_stored_playlists(ds);
                print_list_of_lists(rootlist);
            }
        }

        /* rename */
        else if (!strncmp(buf, "rename", 6)) {
            int num = 0;
            char *name = 0;
            if (strlen(buf) > 9) {
                num = atoi(buf + 7);
                name = strchr(buf + 7, ' ') + 1;
            }

            if (num && name && name[0]) {
                struct playlist* p = get_playlist(rootlist, num);

                if (p) {
                    if (despotify_rename_playlist(ds, p, name))
                        printf("Renamed playlist %d to \"%s\".\n", num, name);
                    else
                        printf("Rename failed: %s\n", despotify_get_error(ds));
                }
            }
            else
                printf("Usage: rename [num] [string]\n");
        }

        /* collab */
        else if (!strncmp(buf, "collab", 6)) {
            int num = 0;
            if (strlen(buf) > 7)
                num = atoi(buf + 7);

            if (num) {
                struct playlist* p = get_playlist(rootlist, num);

                if (p) {
                    if (despotify_set_playlist_collaboration(ds, p, !p->is_collaborative))
                        printf("Changed playlist %d collaboration to %s.\n",
                                num, p->is_collaborative ? "ON" : "OFF");
                    else
                        printf("Setting playlist collaboration state failed: %s\n",
                                despotify_get_error(ds));
                }
            }
            else
                printf("Usage: collab [num]\n");
        }

        /* search */
        else if (!strncmp(buf, "search", 6)) {
            if (buf[7]) {
                if (search)
                    despotify_free_search(search);

                despotify_stop(ds); /* since we replace the list */
                search = despotify_search(ds, buf + 7, MAX_SEARCH_RESULTS);
                if (!search) {
                    printf("Search failed: %s\n", despotify_get_error(ds));
                    continue;
                }
                searchlist = search->playlist;
            }
            else if (searchlist && (searchlist->num_tracks < search->total_tracks))
                if (!despotify_search_more(ds, search, searchlist->num_tracks, MAX_SEARCH_RESULTS)) {
                    printf("Search failed: %s\n", despotify_get_error(ds));
                    continue;
                }

            if (searchlist) {
                if (search->suggestion[0])
                    printf("Did you mean \"%s\"?\n", search->suggestion);

                if (search->total_artists > 0) {
                    printf("Artists found (%d):\n", search->total_artists);
                    for (struct artist* artist = search->artists; artist; artist = artist->next)
                        printf(" %s\n", artist->name);
                }

                if (search->total_albums > 0) {
                    printf("\nAlbums found (%d):\n", search->total_albums);
                    for (struct album* album = search->albums; album; album = album->next)
                        printf(" %s\n", album->name);
                }

                if (search->total_tracks > 0) {
                    printf("\nTracks found (%d/%d):\n", searchlist->num_tracks, search->total_tracks);
                    print_tracks(search->tracks);
                }

                lastlist = searchlist;
            }
            else
                printf("No previous search\n");
        }

        /* artist */
        else if (!strncmp(buf, "artist", 6)) {
            int num = atoi(buf + 7);
            if (!num) {
                printf("usage: artist [num]\n");
                continue;
            }
            if (!lastlist) {
                printf("No playlist\n");
                continue;
            }

            /* find the requested track */
            struct track* t = lastlist->tracks;
            for (int i=1; i<num; i++)
                t = t->next;

            for (struct artist* aptr = t->artist; aptr; aptr = aptr->next) {
                struct artist_browse* a = despotify_get_artist(ds, aptr->id);
                printf("\nName: %s\n"
                       "Genres: %s\n"
                       "Years active: %s\n"
                       "%d albums:\n",
                       a->name, a->genres, a->years_active, a->num_albums);
                for (struct album_browse* al = a->albums; al; al = al->next)
                    printf(" %s (%d)\n", al->name, al->year);
                despotify_free_artist_browse(a);
            }
        }

        /* album */
        else if (!strncmp(buf, "album", 5)) {
            int num = atoi(buf + 6);
            if (!num) {
                printf("usage: album [num]\n");
                continue;
            }
            if (!lastlist) {
                printf("No playlist\n");
                continue;
            }

            /* find the requested track */
            struct track* t = lastlist->tracks;
            for (int i=1; i<num; i++)
                t = t->next;

            if (t) {
                struct album_browse* a = despotify_get_album(ds, t->album_id);
                if (a) {
                    printf("\nName: %s\n"
                           "Year: %d\n",
                           a->name, a->year);
                    print_tracks(a->tracks);
                    despotify_free_album_browse(a);
                }
                else
                    printf("Got no album for id %s\n", t->album_id);
            }
        }

        /* playalbum */
        else if (!strncmp(buf, "playalbum", 9)) {
            int num = atoi(buf + 10);
            if (!num) {
                printf("usage: playalbum [num]\n");
                continue;
            }
            if (!lastlist) {
                printf("No playlist\n");
                continue;
            }

            /* find the requested track */
            struct track* t = lastlist->tracks;
            for (int i=1; i<num; i++)
                t = t->next;


            if (t) {
                if (playalbum)
                    despotify_free_album_browse(playalbum);

				despotify_stop(ds);
                playalbum = despotify_get_album(ds, t->album_id);

                if (playalbum)
                    despotify_play(ds, playalbum->tracks, true);
                else
                    printf("Got no album for id %s\n", t->album_id);
            }
        }

        /* portrait */
        else if (!strncmp(buf, "portrait", 8)) {
            int num = atoi(buf + 9);
            if (!num) {
                printf("usage: portrait [num]\n");
                continue;
            }
            if (!lastlist) {
                printf("No playlist\n");
                continue;
            }

            /* find the requested artist */
            struct track* t = lastlist->tracks;
            for (int i=1; i<num; i++)
                t = t->next;
            struct artist_browse* a = despotify_get_artist(ds, t->artist->id);
            if (a && a->portrait_id[0]) {
                int len;
                void* portrait = despotify_get_image(ds, a->portrait_id, &len);
                if (portrait && len) {
                    printf("Writing %d bytes into portrait.jpg\n", len);
                    FILE* f = fopen("portrait.jpg", "w");
                    if (f) {
                        fwrite(portrait, len, 1, f);
                        fclose(f);
                    }
                    free(portrait);
                }
            }
            else
                printf("Artist %s has no portrait.\n", a->name);
            despotify_free_artist_browse(a);
        }
        
        /* play */
        else if (!strncmp(buf, "play", 4)) {
            if (!lastlist) {
                printf("No list to play from. Use 'list' or 'search' to select a list.\n");
                continue;
            }

            /* skip to track <num> */
            int num = atoi(buf + 5);
            struct track* t = lastlist->tracks;
            for (int i=1; i<num && t; i++)
                t = t->next;
            if (t)
                despotify_play(ds, t, true);
            else
                printf("Invalid track number %d\n", num);
        }

        /* stop */
        else if (!strncmp(buf, "stop", 4)) {
            despotify_stop(ds);
        }

        /* pause */
        else if (!strncmp(buf, "pause", 5)) {
            despotify_pause(ds);
        }

        /* resume */
        else if (!strncmp(buf, "resume", 5)) {
            despotify_resume(ds);
        }

        /* info */
        else if (!strncmp(buf, "info", 4)) {
            print_info(ds);
        }
        
        /* help */
        else if (!strncmp(buf, "help", 4)) {
            print_help();
        }

        /* quit */
        else if (!strncmp(buf, "quit", 4)) {
            loop = false;
        }
    } while(loop);

    if (rootlist)
        despotify_free_playlist(rootlist);

    if (search)
        despotify_free_search(search);

    if (playalbum)
        despotify_free_album_browse(playalbum);
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Usage: %s <username> <password>\n", argv[0]);
        return 1;
    }

    if (!despotify_init())
    {
        printf("despotify_init() failed\n");
        return 1;
    }

    struct despotify_session* ds = despotify_init_client();
    if (!ds) {
        printf("despotify_init_client() failed\n");
        return 1;
    }
    
    if (!despotify_authenticate(ds, argv[1], argv[2])) {
        printf( "Authentication failed: %s\n", despotify_get_error(ds));
        despotify_exit(ds);
        return 1;
    }

    print_info(ds);

    command_loop(ds);

    despotify_exit(ds);

    if (!despotify_cleanup())
    {
        printf("despotify_cleanup() failed\n");
        return 1;
    }

    return 0;
}

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

void print_help(void)
{
    printf("\nAvailable commands:\n"
           "list [num]           List stored playlists\n"
           "search [string]      Search for [string] or get next 100 results\n"
           "artist [num]         Show information about artist for track [num]\n"
           "album [num]          List album for track [num]\n"
           "play [num]           Play track [num] in the last viewed list\n"
           "stop, pause, resume  Control playback\n"
           "portrait [num]       Save artist portrait to portrait.jpg\n"
           "help                 This text\n"
           "quit                 Quit\n");
}

void command_loop(struct despotify_session* ds)
{
    bool loop = true;
    char buf[80];
    struct playlist* rootlist = NULL;
    struct playlist* searchlist = NULL;
    struct playlist* lastlist = NULL;

    print_help();

    do {
        printf("\n> ");
        fflush(stdout);
        bzero(buf, sizeof buf);
        fgets(buf, sizeof buf -1, stdin);

        /* list */
        if (!strncmp(buf, "list", 4)) {
            int num = atoi(buf + 5);
            if (num) {
                if (!rootlist) {
                    printf("Stored lists not loaded. Run 'list' without parameter to load.\n");
                    continue;
                }

                /* skip to playlist number <num> */
                struct playlist* p = rootlist;
                for (int i=1; i<num && p; i++)
                    p = p->next;

                if (p) {
                    print_tracks(p->tracks);
                    lastlist = p;
                }
                else
                    printf("Invalid playlist number %d\n", num);
            }
            else {
                if (!rootlist)
                    rootlist = despotify_get_stored_playlists(ds);
                print_list_of_lists(rootlist);
            }
        }

        /* search */
        else if (!strncmp(buf, "search", 6)) {
            if (buf[7]) {
                if (searchlist)
                    despotify_free_playlist(searchlist);

                despotify_stop(ds); /* since we replace the list */
                searchlist = despotify_search(ds, buf + 7);
                if (!searchlist) {
                    printf("Search failed: %s\n", despotify_get_error(ds));
                    continue;
                }
            }
            else if (searchlist && (searchlist->num_tracks < searchlist->search->total_tracks))
                if (!despotify_search_more(ds, searchlist)) {
                    printf("Search failed: %s\n", despotify_get_error(ds));
                    continue;
                }

            if (searchlist) {
                printf("Total tracks: %d (got %d)\n",
                        searchlist->search->total_tracks, searchlist->num_tracks);
                if (searchlist->search->suggestion[0])
                    printf("Did you mean \"%s\"?\n", searchlist->search->suggestion);
                print_tracks(searchlist->tracks);
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
                struct artist* a = despotify_get_artist(ds, aptr->id);
                printf("\nName: %s\n"
                       "Genres: %s\n"
                       "Years active: %s\n"
                       "%d albums:\n",                   
                       a->name, a->genres, a->years_active, a->num_albums);
                for (struct album* al = a->albums; al; al = al->next)
                    printf(" %s (%d)\n", al->name, al->year);
                despotify_free_artist(a);
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
                struct album* a = despotify_get_album(ds, t->album_id);
                if (a) {
                    printf("\nName: %s\n"
                           "Year: %d\n",
                           a->name, a->year);
                    print_tracks(a->tracks);
                    despotify_free_album(a);
                }
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
            struct artist* a = despotify_get_artist(ds, t->artist->id);
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
            despotify_free_artist(a);
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
                despotify_play(ds, lastlist, t);
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

    if (searchlist)
        despotify_free_playlist(searchlist);
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

    command_loop(ds);

    despotify_exit(ds);

    if (!despotify_cleanup())
    {
        printf("despotify_cleanup() failed\n");
        return 1;
    }

    return 0;
}

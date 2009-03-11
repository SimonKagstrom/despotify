/*
 * A very simple despotify client, to show how the library is used.
 *
 * $Id: despotify.c 734 2009-02-23 18:27:54Z x $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "despotify.h"

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

    if (despotify_get_playlists(ds))
    {
        struct playlist *p; 
        printf("List of playlists on account:\n");
        for (p = ds->playlists; p; p = p->next)
        {
            printf("  Playlist name:   %s\n", p->name);
            printf("  Playlist author: %s\n", p->author);
        }
    }

    struct playlist* pl = despotify_search(ds, "machinae");
    if (!pl) {
        printf("Search failed: %s\n", despotify_get_error(ds));
        return 1;
    }

    printf("Playlist name: %s\n", pl->name);
    printf("Playlist author: %s\n", pl->author);

    int i=1;
    for (struct track* t = pl->tracks; t; t = t->next) {
        printf("%2d: %s\n", i++, t->title);
    }

    /* play first track in playlist */
    despotify_play(ds, pl, pl->tracks);

    /* let it play a while */
    sleep(10);
    
    despotify_free_playlist(pl);
    despotify_exit(ds);

    if (!despotify_cleanup())
    {
        printf("despotify_cleanup() failed\n");
        return 1;
    }

    return 0;
}

void app_packet_callback(SESSION *s, int cmd, unsigned char* buf, int len)
{
    (void)s;
    (void)buf;

    printf("Got packet type 0x%02x (%3d bytes)\n", cmd, len);
}

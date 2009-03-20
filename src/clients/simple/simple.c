/*
 * A very simple despotify client, to show how the library is used.
 *
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

    printf("List of playlists on account:\n");
    struct playlist *sp = despotify_get_stored_playlists(ds);
    if (!sp) {
        printf(" (Account has no stored playlists)\n");
    }
    else {
        printf("Account has %d stored playlists:\n", sp->num_tracks);
        for (struct playlist* p = sp; p; p = p->next)
        {
            printf("  Playlist name:   %s\n", p->name);
            printf("  Playlist author: %s\n", p->author);

            int i = 1;
            for (struct track* t = p->tracks; t; t = t->next) {
                if (t->has_meta_data) {
                    printf("  %3d: %s (%s)\n", i++, t->title, t->artist);
                }
                else {
                    printf("  %3d: N/A\n", i++);
                }
            }
        }
        despotify_free_playlist(sp);
    }

    printf("Search:\n");
    struct playlist* pl = despotify_search(ds, "machinae");
    if (!pl) {
        printf("Search failed: %s\n", despotify_get_error(ds));
        return 1;
    }

    printf("Playlist name: %s\n", pl->name);
    printf("Playlist author: %s\n", pl->author);

    int i=1;
    for (struct track* t = pl->tracks; t; t = t->next) {
        printf("%2d: %s (%d:%02d)\n", i++, t->title,
               t->length / 60000, t->length % 60000 / 1000);
    }

    /* play first track in playlist */
    printf("Playing track 1\n");
    despotify_play(ds, pl, pl->tracks);

    /* let it play a while */
    sleep(5);

    printf("Pause 1s\n");
    despotify_pause(ds);
    sleep(1);
    printf("Resume\n");
    despotify_resume(ds);

    sleep(5);
    printf("Stop\n");
    despotify_stop(ds);

    despotify_free_playlist(pl);
    despotify_exit(ds);

    if (!despotify_cleanup())
    {
        printf("despotify_cleanup() failed\n");
        return 1;
    }

    return 0;
}

void app_packet_callback(struct session *s, int cmd, unsigned char* buf, int len)
{
    (void)s;
    (void)buf;
    (void)cmd;
    (void)len;
}

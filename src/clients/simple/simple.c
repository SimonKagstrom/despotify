/*
 * A very simple despotify client, to show how the library is used.
 *
 * $Id$
 *
 */

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <wchar.h>
#include "despotify.h"
#include "util.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int listen_fd = -1;
static int client_fd = -1;
char *wrapper_read_command(void);
int wrapper_listen(int port);
void wrapper_wprintf(wchar_t *fmt, ...);


struct playlist* get_playlist(struct playlist* rootlist, int num)
{
    struct playlist* p = rootlist;

    if (!p) {
        wrapper_wprintf(L"Stored lists not loaded. Run 'list' without parameter to load.\n");
    }
    else {
        /* skip to playlist number <num> */
        for (int i = 1; i < num && p; i++)
            p = p->next;


        if (!p)
            wrapper_wprintf(L"Invalid playlist number %d\n", num);
    }

    return p;
}

void print_list_of_lists(struct playlist* rootlist)
{
    if (!rootlist) {
        wrapper_wprintf(L" <no stored playlists>\n");
    }
    else {
        int count=1;
        for (struct playlist* p = rootlist; p; p = p->next)
            wrapper_wprintf(L"%2d: %-40s %3d %c %s\n", count++, p->name, p->num_tracks,
                   p->is_collaborative ? '*' : ' ', p->author);
    }
}

void print_tracks(struct track* head)
{
    if (!head) {
        wrapper_wprintf(L" <empty playlist>\n");
        return;
    }

    int count = 1;
    for (struct track* t = head; t; t = t->next) {
        if (t->has_meta_data) {
            wrapper_wprintf(L"%3d: %-40s %2d:%02d ", count++, t->title,
                   t->length / 60000, t->length % 60000 / 1000);
            for (struct artist* a = t->artist; a; a = a->next)
                wrapper_wprintf(L"%s%s", a->name, a->next ? ", " : "");
            wrapper_wprintf(L" %s\n", t->playable ? "" : "(Unplayable)");
        }
        else
            wrapper_wprintf(L"%3d: N/A\n", count++);
    }
}

void print_track_full(struct track* t)
{
    if(t->has_meta_data) {
        wrapper_wprintf(L"\nTitle: %s\nAlbum: %s\nArtist(s): ",
               t->title, t->album);

        for (struct artist* a = t->artist; a; a = a->next)
            wrapper_wprintf(L"%s%s", a->name, a->next ? ", " : "");

        wrapper_wprintf(L"\nYear: %d\nLength: %02d:%02d\n\n",
               t->year, t->length / 60000, t->length % 60000 / 1000);
    } else {
        wrapper_wprintf(L" <track has no metadata>\n");
    }
}

void print_album(struct album_browse* a)
{
    wrapper_wprintf(L"\nName: %s\nYear: %d\n",
            a->name, a->year);
    print_tracks(a->tracks);
}

void print_artist(struct artist_browse* a)
{
    wrapper_wprintf(L"\nName: %s\n"
           "Genres: %s\n"
           "Years active: %s\n"
           "%d albums:\n",
           a->name, a->genres, a->years_active, a->num_albums);
    for (struct album_browse* al = a->albums; al; al = al->next)
        wrapper_wprintf(L" %s (%d)\n", al->name, al->year);
}

void print_playlist(struct playlist* pls)
{
    wrapper_wprintf(L"\nName: %s\nAuthor: %s\n",
           pls->name, pls->author);
    print_tracks(pls->tracks);
}

void print_search(struct search_result *search)
{
    if (search->suggestion[0])
        wrapper_wprintf(L"\nDid you mean \"%s\"?\n", search->suggestion);

    if (search->total_artists > 0) {
        wrapper_wprintf(L"\nArtists found (%d):\n", search->total_artists);

        for (struct artist* artist = search->artists; artist; artist = artist->next)
            wrapper_wprintf(L" %s\n", artist->name);
    }

    if (search->total_albums > 0) {
        wrapper_wprintf(L"\nAlbums found (%d):\n", search->total_albums);
        for (struct album* album = search->albums; album; album = album->next)
            wrapper_wprintf(L" %s\n", album->name);
    }

    if (search->total_tracks > 0) {
        wrapper_wprintf(L"\nTracks found (%d/%d):\n", search->playlist->num_tracks, search->total_tracks);
        print_tracks(search->tracks);
    }
}

void print_info(struct despotify_session* ds)
{
    struct user_info* user = ds->user_info;
    wrapper_wprintf(L"Username       : %s\n", user->username);
    wrapper_wprintf(L"Country        : %s\n", user->country);
    wrapper_wprintf(L"Account type   : %s\n", user->type);
    wrapper_wprintf(L"Account expiry : %s", ctime(&user->expiry));
    wrapper_wprintf(L"Host           : %s:%d\n", user->server_host, user->server_port);
    wrapper_wprintf(L"Last ping      : %s", ctime(&user->last_ping));

    if (strncmp(user->type, "premium", 7)) {
        wrapper_wprintf(L"\n=================================================\n"
               "                  N O T I C E\n"
               "       You do not have a premium account.\n"
               "     Spotify services will not be available.\n"
               "=================================================\n");
    }
}

void print_help(void)
{
    wrapper_wprintf(L"\nAvailable commands:\n\n"
           "list [num]              List stored playlists\n"
           "rename [num] [string]   Rename playlist\n"
           "collab [num]            Toggle playlist collaboration\n"
           "\n"
           "search [string]         Search for [string] or get next 100 results\n"
           "artist [num]            Show information about artist for track [num]\n"
           "album [num]             List album for track [num]\n"
           "uri [string]            Display info about Spotify URI\n"
           "portrait [num]          Save artist portrait to portrait.jpg\n"
           "\n"
           "play [num]              Play track [num] in the last viewed list\n"
           "playalbum [num]         Play album for track [num]\n"
           "next                    Play next track in the current list\n"
           "stop, pause, resume     Control playback\n"
           "\n"
           "info                    Details about your account and current connection\n"
           "help                    This text\n"
           "quit                    Quit\n");
}

void command_loop(struct despotify_session* ds)
{
    bool loop = true;
    char *buf;
    struct playlist* rootlist = NULL;
    struct playlist* searchlist = NULL;
    struct playlist* lastlist = NULL;
    struct search_result *search = NULL;
    struct album_browse* playalbum = NULL;

    print_help();

    do {
        wprintf(L"\n> ");
        fflush(stdout);
	if((buf = wrapper_read_command()) == NULL)
		break;

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
                        wrapper_wprintf(L"Renamed playlist %d to \"%s\".\n", num, name);
                    else
                        wrapper_wprintf(L"Rename failed: %s\n", despotify_get_error(ds));
                }
            }
            else
                wrapper_wprintf(L"Usage: rename [num] [string]\n");
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
                        wrapper_wprintf(L"Changed playlist %d collaboration to %s.\n",
                                num, p->is_collaborative ? "ON" : "OFF");
                    else
                        wrapper_wprintf(L"Setting playlist collaboration state failed: %s\n",
                                despotify_get_error(ds));
                }
            }
            else
                wrapper_wprintf(L"Usage: collab [num]\n");
        }

        /* search */
        else if (!strncmp(buf, "search", 6)) {
            if (buf[7]) {
                if (search)
                    despotify_free_search(search);

                despotify_stop(ds); /* since we replace the list */
                search = despotify_search(ds, buf + 7, MAX_SEARCH_RESULTS);
                if (!search) {
                    wrapper_wprintf(L"Search failed: %s\n", despotify_get_error(ds));
                    continue;
                }
                searchlist = search->playlist;
            }
            else if (searchlist && (searchlist->num_tracks < search->total_tracks))
                if (!despotify_search_more(ds, search, searchlist->num_tracks, MAX_SEARCH_RESULTS)) {
                    wrapper_wprintf(L"Search failed: %s\n", despotify_get_error(ds));
                    continue;
                }

            if (searchlist) {
                print_search(search);


                lastlist = searchlist;
            }
            else
                wrapper_wprintf(L"No previous search\n");
        }

        /* artist */
        else if (!strncmp(buf, "artist", 6)) {
            int num = atoi(buf + 7);
            if (!num) {
                wrapper_wprintf(L"usage: artist [num]\n");
                continue;
            }
            if (!lastlist) {
                wrapper_wprintf(L"No playlist\n");
                continue;
            }

            /* find the requested track */
            struct track* t = lastlist->tracks;
            for (int i=1; i<num; i++)
                t = t->next;

            for (struct artist* aptr = t->artist; aptr; aptr = aptr->next) {
                struct artist_browse* a = despotify_get_artist(ds, aptr->id);
                print_artist(a);
                despotify_free_artist_browse(a);
            }
        }

        /* album */
        else if (!strncmp(buf, "album", 5)) {
            int num = atoi(buf + 6);
            if (!num) {
                wrapper_wprintf(L"usage: album [num]\n");
                continue;
            }
            if (!lastlist) {
                wrapper_wprintf(L"No playlist\n");
                continue;
            }

            /* find the requested track */
            struct track* t = lastlist->tracks;
            for (int i=1; i<num; i++)
                t = t->next;

            if (t) {
                struct album_browse* a = despotify_get_album(ds, t->album_id);
                if (a) {
                    print_album(a);
                    despotify_free_album_browse(a);
                }
                else
                    wrapper_wprintf(L"Got no album for id %s\n", t->album_id);
            }
        }

        /* playalbum */
        else if (!strncmp(buf, "playalbum", 9)) {
            int num = atoi(buf + 10);
            if (!num) {
                wrapper_wprintf(L"usage: playalbum [num]\n");
                continue;
            }
            if (!lastlist) {
                wrapper_wprintf(L"No playlist\n");
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
                    wrapper_wprintf(L"Got no album for id %s\n", t->album_id);
            }
        }

        /* uri */
        else if (!strncmp(buf, "uri", 3)) {
            char *uri = buf + 4;
            if(strlen(uri) == 0) {
                wrapper_wprintf(L"usage: info <uri>\n");
                continue;
            }

            struct link* link = despotify_link_from_uri(uri);
            struct album_browse* al;
            struct artist_browse* ar;
            struct playlist* pls;
            struct search_result* s;
            struct track* t;

            switch(link->type) {
                case LINK_TYPE_ALBUM:
                    al = despotify_link_get_album(ds, link);
                    if(al) {
                        print_album(al);
                        despotify_free_album_browse(al);
                    }
                    break;

                case LINK_TYPE_ARTIST:
                    ar = despotify_link_get_artist(ds, link);
                    if(ar) {
                        print_artist(ar);
                        despotify_free_artist_browse(ar);
                    }
                    break;

                case LINK_TYPE_PLAYLIST:
                    pls = despotify_link_get_playlist(ds, link);
                    if(pls) {
                        print_playlist(pls);
                        despotify_free_playlist(pls);
                    }
                    break;

                case LINK_TYPE_SEARCH:
                    s = despotify_link_get_search(ds, link);
                    if(s) {
                        print_search(s);
                        despotify_free_search(s);
                    }
                    break;

                case LINK_TYPE_TRACK:
                    t = despotify_link_get_track(ds, link);
                    if(t) {
                        print_track_full(t);
                        despotify_free_track(t);
                    }
                    break;

                default:
                    wrapper_wprintf(L"%s is a invalid Spotify URI\n", uri);
            }

            despotify_free_link(link);
        }

        /* portrait */
        else if (!strncmp(buf, "portrait", 8)) {
            int num = atoi(buf + 9);
            if (!num) {
                wrapper_wprintf(L"usage: portrait [num]\n");
                continue;
            }
            if (!lastlist) {
                wrapper_wprintf(L"No playlist\n");
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
                    wrapper_wprintf(L"Writing %d bytes into portrait.jpg\n", len);
                    FILE* f = fopen("portrait.jpg", "w");
                    if (f) {
                        fwrite(portrait, len, 1, f);
                        fclose(f);
                    }
                    free(portrait);
                }
            }
            else
                wrapper_wprintf(L"Artist %s has no portrait.\n", a->name);
            despotify_free_artist_browse(a);
        }

        /* play */
        else if (!strncmp(buf, "play", 4) || !strncmp(buf, "next", 4)) {
            if (!lastlist) {
                wrapper_wprintf(L"No list to play from. Use 'list' or 'search' to select a list.\n");
                continue;
            }

            /* skip to track <num>, else play next */
            struct track* t;
            if (buf[4]) {
                int listoffset = atoi(buf + 5);
                t = lastlist->tracks;
                for (int i=1; i<listoffset && t; i++)
                    t = t->next;

                if (!t) {
                    wrapper_wprintf(L"Invalid track number %d\n", listoffset);
                }
            }
            else {
                t = despotify_get_current_track(ds);
                if (t)
                    t = t->next;
            }

            if (t) {
                despotify_play(ds, t, true);
                wrapper_wprintf(L"New track: %s / %s (%d:%02d)\n",
                        t->title, t->artist->name,
                        t->length / 60000, t->length % 60000 / 1000);
            }
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

void callback(struct despotify_session* ds, int signal, void* data, void* callback_data)
{
    (void)data; (void)ds; (void)callback_data;

    switch (signal) {
        case DESPOTIFY_TRACK_CHANGE: {
            struct track* t = data;
            wrapper_wprintf(L"New track: %s / %s (%d:%02d)\n",
                    t->title, t->artist->name,
                    t->length / 60000, t->length % 60000 / 1000);
            break;
        }
    }
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");

    if (argc < 3) {
        wrapper_wprintf(L"Usage: %s <username> <password> [remote control port]\n", argv[0]);
        return 1;
    }

    DSFYDEBUG("$Id$\n");
    if (!despotify_init())
    {
        wrapper_wprintf(L"despotify_init() failed\n");
        return 1;
    }

    struct despotify_session* ds = despotify_init_client(callback, NULL);
    if (!ds) {
        wrapper_wprintf(L"despotify_init_client() failed\n");
        return 1;
    }

    if(argc == 4 && wrapper_listen(atoi(argv[3]))) {
        wrapper_wprintf(L"wrapper_listen() failed to listen on local port %s\n", argv[3]);
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
        wrapper_wprintf(L"despotify_cleanup() failed\n");
        return 1;
    }

    return 0;
}


int wrapper_listen(int port) {
	struct sockaddr_in sin;
	int value;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = PF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");

	if((listen_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		return -1;
	
	value = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &value, sizeof(int));

	if(bind(listen_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0
		|| listen(listen_fd, 1) < 0) {
		close(listen_fd);
		listen_fd = -1;

		return -1;
	}

	return 0;
}

char *wrapper_read_command(void) {
	static char sock_buf[256] = { 0 };
	static char stdin_buf[256] = { 0 };
	static int sock_buf_len;
	static int stdin_buf_len;
	static char *command = NULL;

	fd_set rfds;
	int max_fd = 0;
	char *ptr;
	int ret;

	if(command) {
		free(command);
		command = NULL;
	}

	for(;;) {
		FD_ZERO(&rfds);
		if(isatty(0))
			FD_SET(0, &rfds);
	
		if(listen_fd != -1 && client_fd == -1) {
			FD_SET(listen_fd, &rfds);
			max_fd = listen_fd;
		}
		else if(client_fd != -1) {
			FD_SET(client_fd, &rfds);
			max_fd = client_fd;
		}

		if(select(max_fd + 1, &rfds, NULL, NULL, NULL) < 0)
			break;

		if(FD_ISSET(0, &rfds)) {
			ret = read(0, stdin_buf + stdin_buf_len, sizeof(stdin_buf) - stdin_buf_len - 1);
			if(ret > 0) {
				stdin_buf_len += ret;
				stdin_buf[stdin_buf_len] = 0;
			}
		}

		if(listen_fd >= 0 && FD_ISSET(listen_fd, &rfds)) {
			client_fd = accept(listen_fd, NULL, NULL);
			continue;
		}

		if(client_fd >= 0 && FD_ISSET(client_fd, &rfds)) {
			ret = read(client_fd, sock_buf + sock_buf_len, sizeof(sock_buf) - sock_buf_len - 1);
			if(ret > 0) {
				sock_buf_len += ret;
				sock_buf[sock_buf_len] = 0;
			}
			else {
				close(client_fd);
				client_fd = -1;
				sock_buf_len = 0;
				*sock_buf = 0;
			}
		}

		if((ptr = strchr(stdin_buf, '\n')) != NULL) {
			*ptr++ = 0;
			if(strlen(stdin_buf))
				command = strdup(stdin_buf);
			stdin_buf_len -= ptr - stdin_buf;
			memmove(stdin_buf, ptr, stdin_buf_len + 1);
			if(command)
				break;
		}
		else if((ptr = strchr(sock_buf, '\n')) != NULL) {
			*ptr++ = 0;
			if(strlen(sock_buf))
				command = strdup(sock_buf);
			sock_buf_len -= ptr - sock_buf;
			memmove(sock_buf, ptr, sock_buf_len + 1);
			if(command)
				break;
		}
	}

	return command;
}


void wrapper_wprintf(wchar_t *fmt, ...) {
	wchar_t wstr[4096];
	char mbstr[4096];
	char *ptr;
	va_list ap;
	int ret, written;

	va_start(ap, fmt);
	ret = vswprintf(wstr, sizeof(wstr) - 1, fmt, ap);
	va_end(ap);

	if(ret >= (int)sizeof(wstr))
		return;

	wcstombs(mbstr, wstr, sizeof(mbstr) - 1);
	mbstr[sizeof(mbstr) - 1] = 0;

	if(isatty(0))
		wprintf(L"%S", wstr);
	
	if(client_fd == -1)
		return;

	ptr = mbstr;
	ret = strlen(mbstr);
	while(ret > 0) {
		if((written = write(client_fd, ptr, ret)) <= 0) {
			close(client_fd);
			client_fd = -1;
			break;
		}

		ptr += written;
		ret -= written;
	}
}

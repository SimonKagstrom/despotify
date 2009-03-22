/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_UI_PLAYLIST_H
#define DESPOTIFY_UI_PLAYLIST_H

#include <ncursesw/curses.h>

#include "event.h"
#include "playlist.h"

void gui_playlist (WINDOW *, char *);
void gui_playlist_display (WINDOW *, struct playlist *);
int gui_playlists_download (EVENT *, enum ev_flags);
#endif

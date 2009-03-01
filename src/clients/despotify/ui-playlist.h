/*
 * $Id: ui-playlist.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_UI_PLAYLIST_H
#define DESPOTIFY_UI_PLAYLIST_H

#include <curses.h>

#include "event.h"
#include "playlist.h"

void gui_playlist (WINDOW *, char *);
void gui_playlist_display (WINDOW *, struct playlist *);
int gui_playlists_download (EVENT *, enum ev_flags);
#endif

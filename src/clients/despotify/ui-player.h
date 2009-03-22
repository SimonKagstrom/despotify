/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_UI_PLAYER_H
#define DESPOTIFY_UI_PLAYER_H

#include <ncursesw/curses.h>

#include "event.h"

void gui_player (char *);
int gui_player_event_processor (EVENT *, enum ev_flags);
#endif

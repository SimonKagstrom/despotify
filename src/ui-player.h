/*
 * $Id: ui-player.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_UI_PLAYER_H
#define DESPOTIFY_UI_PLAYER_H

#include <curses.h>

#include "event.h"


void gui_player(char *);
int gui_player_event_processor(EVENT *, enum ev_flags);
#endif

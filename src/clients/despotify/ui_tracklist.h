/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_UI_TRACKLIST_H
#define DESPOTIFY_UI_TRACKLIST_H

#include "ui.h"

void tracklist_draw(ui_t *ui);
int  tracklist_keypress(int ch);

void tracklist_set(int pos, bool focus);

#endif

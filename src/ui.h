/*
 * $Id: ui.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_UI_H
#define DESPOTIFY_UI_H

#include "event.h"
#include "packet.h"
#include "session.h"

void gui_init (void);
void gui_update_view (void);
void gui_finish (int);

int gui_action_handler (EVENT * e, enum ev_flags ev_kind);
void app_packet_callback (SESSION *, int, unsigned char *, int);
#endif

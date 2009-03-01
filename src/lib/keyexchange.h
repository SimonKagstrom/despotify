/*
 * $Id: keyexchange.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_KEYEXCHANGE_H
#define DESPOTIFY_KEYEXCHANGE_H

#include "session.h"

int send_client_initial_packet (SESSION *);
int read_server_initial_packet (SESSION *);
int do_key_exchange (SESSION * c);
void key_init (SESSION *);
#endif

/*
 * $Id: handlers.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_HANDLERS_H
#define DESPOTIFY_HANDLERS_H

#include "packet.h"


int handle_secret_block(PHANDLER *, unsigned char *, unsigned short);
int handle_ping(PHANDLER *, unsigned char *, unsigned short);
int handle_channel(PHANDLER *, unsigned char *, unsigned short);
int handle_aeskey(PHANDLER *, unsigned char *, unsigned short);
int handle_sha_hash(PHANDLER *, unsigned char *, unsigned short);
int handle_countrycode(PHANDLER *, unsigned char *, unsigned short);
int handle_p2p_initblock(PHANDLER *, unsigned char *, unsigned short);
int handle_notification(PHANDLER *, unsigned char *, unsigned short);
int handle_product_information(PHANDLER *, unsigned char *, unsigned short);
int handle_welcome(PHANDLER *, unsigned char *, unsigned short);
#endif

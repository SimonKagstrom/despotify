/*
 * $Id: packet.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_PACKET_H
#define DESPOTIFY_PACKET_H

#include "session.h"


/*
 * Packet header
 *
 */
struct packet_header {
	unsigned char cmd;
	unsigned short len;
} __attribute__((packed));
typedef struct packet_header PHEADER;


/* lowlevel packet functions */
int packet_read(SESSION *c, PHEADER *, unsigned char **);
int packet_write(SESSION *, unsigned char, unsigned char *, unsigned short);
#endif

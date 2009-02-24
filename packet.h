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


/*
 * Function prototype for packet callback
 * - Packet handler (has SESSION and private data)
 * - Packet payload and length
 *
 */
struct _packethandler;
typedef struct _packethandler PHANDLER;
typedef int (*packet_callback)(PHANDLER *, unsigned char *, unsigned short);


/*
 * Linked list of assigned packet handlers
 *
 */
struct _packethandler {
	/* Needed to be able to send back responses */
	SESSION *session;

	/* Command this handler listens for */
	int cmd;

	/* Callback routine */
	packet_callback callback;

	/* Private storage */
	void *private;

	struct _packethandler *next;
};


/* highlevel packet handling routines */
PHANDLER *packethandler_register(SESSION *, int, packet_callback, void *);
void packethandler_unregister(SESSION *, PHANDLER *);
PHANDLER *packethandler_by_cmd(SESSION *, int, PHANDLER *);

/* lowlevel packet functions */
int packet_read(SESSION *c, PHEADER *, unsigned char **);
int packet_write(SESSION *, unsigned char, unsigned char *, unsigned short);
#endif

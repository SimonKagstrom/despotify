/*
 * $Id: commands.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_COMMANDS_H
#define DESPOTIFY_COMMANDS_H

#include "session.h"
#include "channel.h"


/* Core functionality */
#define CMD_SECRETBLK	0x02
#define CMD_PING	0x04
#define CMD_CHANNELDATA	0x09
#define CMD_CHANNELERR	0x0a
#define CMD_CHANNELABRT	0x0b
#define CMD_AESKEY	0x0d

/* Rights management */
#define CMD_COUNTRYCODE	0x1b

/* P2P related */
#define CMD_P2P_SETUP	0x20
#define CMD_P2P_INITBLK	0x21

/* Search and meta data */
#define CMD_BROWSE	0x30
#define CMD_SEARCH	0x31
#define CMD_PLAYLIST	0x35

/* Session management */
#define CMD_NOTIFY	0x42
#define CMD_PONG	0x49
#define CMD_PONGACK	0x4a
#define CMD_REQUESTAD	0x4e
#define CMD_REQUESTPLAY	0x4f

/* Internal */
#define CMD_PRODINFO	0x50
#define CMD_GETUSERDATA	0x57
#define CMD_WELCOME	0x69


int cmd_send_cache_hash(SESSION *);
int cmd_aeskey(SESSION *, unsigned char *, unsigned char *, channel_callback, void *);
int cmd_search(SESSION *, char *, channel_callback, void *);
int cmd_requestad(SESSION *, unsigned char);
int cmd_getuserdata(SESSION *, char *, channel_callback, void *);
int cmd_action(SESSION *, unsigned char *, unsigned char *);
int cmd_getsubstreams(SESSION *, unsigned char *, unsigned int, unsigned int, unsigned int, channel_callback, void *);
int cmd_browse(SESSION *, unsigned char, unsigned char *, int, channel_callback, void *);
int cmd_getplaylist(SESSION *, unsigned char *, int, channel_callback, void *);
int cmd_ping_reply(SESSION *);
#endif

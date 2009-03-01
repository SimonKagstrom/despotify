/*
 * $Id: gw-stream.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_GW_STREAM_H
#define DESPOTIFY_GW_STREAM_H

#include "gw.h"

int gw_file_key (SPOTIFYSESSION *, unsigned char *, unsigned char *);
int gw_file_stream (SPOTIFYSESSION *, unsigned char *, unsigned int,
		    unsigned int, unsigned char *);
#endif

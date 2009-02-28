/*
 * $Id$
 *
 * This file contains a basic implementations of request
 * you may send to Spotify's service.
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "buffer.h"
#include "channel.h"
#include "commands.h"
#include "packet.h"
#include "util.h"

/*
 * Writes to /tmp
 *
 */
static int dump_generic (CHANNEL * ch, unsigned char *buf, unsigned short len)
{
	FILE *fd;
	char filename[512];

	if (len == 0)
		return 0;

	if (ch->state == CHANNEL_HEADER)
		snprintf (filename, sizeof (filename),
			  "/tmp/channel-%d-%s.hdr-%d", ch->channel_id,
			  ch->name, ch->header_id);
	else
		snprintf (filename, sizeof (filename), "/tmp/channel-%d-%s",
			  ch->channel_id, ch->name);

	if ((fd = fopen (filename, "ab")) != NULL) {
		fwrite (buf, 1, len, fd);
		fclose (fd);

		return 0;
	}

	return -1;
}

int cmd_send_cache_hash (SESSION * session)
{
	BUFFER *b;
	int ret;

	b = buffer_init ();
	buffer_append_raw (b, session->cache_hash,
			   sizeof (session->cache_hash));

	ret = packet_write (session, 0x0f, b->buf, b->buflen);
	DSFYDEBUG ("cmd_send_cache_hash: packet_write() returned %d\n", ret);
	buffer_free (b);

	return ret;
}

/*
 * Request ads
 * The response is plain XML
 *
 */
int cmd_requestad (SESSION * session, unsigned char ad_type)
{
	BUFFER *b;
	CHANNEL *ch;
	int ret;
	char buf[100];

	b = buffer_init ();
	snprintf (buf, sizeof (buf), "RequestAd-with-type-%d", ad_type);
	ch = channel_register (buf, dump_generic, NULL);

	DSFYDEBUG
		("cmd_requestad: allocated channel %d, retrieving ads with type id %d\n",
		 ch->channel_id, ad_type)

		buffer_append_short (b, ch->channel_id);
	buffer_append_raw (b, &ad_type, 1);

	ret = packet_write (session, CMD_REQUESTAD, b->buf, b->buflen);
	DSFYDEBUG ("cmd_requestad: packet_write() returned %d\n", ret)

		buffer_free (b);

	return ret;
}

/*
 * Request image using a 20 byte hash
 * The response is a JPG
 *
 */
int cmd_request_image (SESSION * session, unsigned char *hash,
		       channel_callback callback, void *private)
{
	BUFFER *b;
	CHANNEL *ch;
	int ret;
	char buf[100];

	b = buffer_init ();
	strcpy (buf, "image-");
	hex_bytes_to_ascii (hash, buf + 6, 20);

	ch = channel_register (buf, callback, private);
	DSFYDEBUG
		("cmd_requestimg: allocated channel %d, retrieving img with UUID %s\n",
		 ch->channel_id, buf + 6);

	buffer_append_short (b, ch->channel_id);
	buffer_append_raw (b, hash, 20);

	ret = packet_write (session, CMD_IMAGE, b->buf, b->buflen);
	DSFYDEBUG ("cmd_requestimg: packet_write() returned %d\n", ret)

		buffer_free (b);

	return ret;
}

/*
 * Search music
 * The response comes as compressed XML
 *
 */
int cmd_search (SESSION * session, char *searchtext,
		channel_callback callback, void *private)
{
	BUFFER *b;
	CHANNEL *ch;
	int ret;
	char buf[100];
	unsigned char searchtext_length;
	unsigned int dummyint;
	unsigned short dummyshort;

	b = buffer_init ();

	snprintf (buf, sizeof (buf), "Search-%s", searchtext);
	ch = channel_register (buf, callback, private);

	DSFYDEBUG ("cmd_search: allocated channel %d, searching for '%s'\n",
		   ch->channel_id, searchtext)

		buffer_append_short (b, ch->channel_id);

	dummyint = 0;
	buffer_append_int (b, dummyint);

	dummyint = 0xffffffff;
	buffer_append_int (b, dummyint);

	dummyshort = 0;
	buffer_append_short (b, dummyshort);

	searchtext_length = (unsigned char) strlen (searchtext);
	buffer_append_raw (b, &searchtext_length, 1);

	buffer_append_raw (b, searchtext, searchtext_length);

	ret = packet_write (session, CMD_SEARCH, b->buf, b->buflen);
	DSFYDEBUG ("cmd_search: packet_write() returned %d\n", ret)

		buffer_free (b);

	return ret;
}

/*
 * Notify server we're going to play
 *
 */
int cmd_token_notify (SESSION * session)
{
	int ret;
	BUFFER *b;

	/* Request the playing token before playing a song */
	b = buffer_init ();

	ret = packet_write (session, CMD_TOKENNOTIFY, b->buf, b->buflen);
	buffer_free (b);
	if (ret != 0) {
		DSFYDEBUG
			("cmd_token_notify(): packet_write(cmd=0x4f) returned %d, aborting!\n",
			 ret)
	}

	return ret;
}

int cmd_aeskey (SESSION * session, unsigned char *file_id,
		unsigned char *track_id, channel_callback callback,
		void *private)
{
	int ret;
	BUFFER *b;
	unsigned short zero;
	CHANNEL *ch;
	char buf[256];

	/* Request the AES key for this file by sending the file ID and track ID */
	b = buffer_init ();
	buffer_append_raw (b, file_id, 20);
	buffer_append_raw (b, track_id, 16);
	zero = 0;
	buffer_append_short (b, zero);

	/* Allocate a channel and set its name to key-<file id> */
	strcpy (buf, "key-");
	hex_bytes_to_ascii (file_id, buf + 4, 20);
	ch = channel_register (buf, callback, private);
	DSFYDEBUG
		("cmd_key(): allocated channel %d, retrieving AES key for file '%.40s'\n",
		 ch->channel_id, buf);

	/* Force DATA state to be able to handle these packets with the channel infrastructure */
	ch->state = CHANNEL_DATA;
	buffer_append_short (b, ch->channel_id);

	ret = packet_write (session, CMD_REQKEY, b->buf, b->buflen);
	buffer_free (b);
	if (ret != 0) {
		DSFYDEBUG
			("cmd_key(): packet_write(cmd=0x0c) returned %d, aborting!\n",
			 ret)
	}

	return ret;
}

/*
 * A demo wrapper for playing a track
 *
 */
int cmd_action (SESSION * session, unsigned char *file_id,
		unsigned char *track_id)
{
	int ret;

	/*
	 * Notify the server about our intention to play music, there by allowing
	 * it to request other players on the same account to pause.
	 *
	 * Yet another client side restriction to annoy those who share their 
	 * Spotify account with not yet invited friends.
	 * And as a bonus it won't play commercials and waste bandwidth in vain.
	 *
	 */
	if ((ret =
	     packet_write (session, CMD_REQUESTPLAY, (unsigned char *) "",
			   0)) != 0) {
		DSFYDEBUG
			("cmd_action(): packet_write(0x4f) returned %d, aborting!\n",
			 ret)
			return ret;
	}

#ifdef P2P
	/* Request a 100 byte P2P initialization block */
	b = buffer_init ();
	buffer_append_raw (b, file_id, 20);

	ret = packet_write (session, 0x20, b->buf, b->buflen);
	buffer_free (b);
	if (ret != 0) {
		DSFYDEBUG
			("cmd_action(): packet_write(cmd=0x20) returned %d, aborting!\n",
			 ret)
			return ret;
	}
#endif

	/* Request the AES key for this file by sending the file ID and track ID */
	return cmd_aeskey (session, file_id, track_id, dump_generic, NULL);
}

/*
 * Request a part of the encrypted file from the server.
 *
 * The data should be decrypted using the AES in CTR mode
 * with AES key provided and a static IV, incremeted for
 * each 16 byte data processed.
 *
 */
int cmd_getsubstreams (SESSION * session, unsigned char *file_id,
		       unsigned int offset, unsigned int length,
		       unsigned int unknown_200k, channel_callback callback,
		       void *private)
{
	char buf[512];
	BUFFER *b;
	CHANNEL *ch;
	int ret;
	unsigned short dummyshort;

	hex_bytes_to_ascii (file_id, buf, 20);
	ch = channel_register (buf, callback, private);
	DSFYDEBUG
		("cmd_getsubstreams: allocated channel %d, retrieving song '%s'\n",
		 ch->channel_id, ch->name)

		b = buffer_init ();
	buffer_append_short (b, ch->channel_id);

	/* I have no idea wtf these 10 bytes are for */
	dummyshort = 0x0800;
	buffer_append_short (b, dummyshort);
	dummyshort = 0x0000;
	buffer_append_short (b, dummyshort);
	buffer_append_short (b, dummyshort);
	buffer_append_short (b, dummyshort);
	dummyshort = 0x4e20;	/* ??? If I just wasn't drunk when I wrote this.. */
	buffer_append_short (b, dummyshort);

	buffer_append_int (b, unknown_200k);

	buffer_append_raw (b, file_id, 20);

	assert (offset % 4096 == 0);
	assert (length % 4096 == 0);
	offset >>= 2;
	length >>= 2;
	buffer_append_int (b, offset);
	buffer_append_int (b, offset + length);

	hex_bytes_to_ascii (file_id, buf, 20);
	DSFYDEBUG
		("Sending GetSubstreams(file_id=%s, offset=%u [%u bytes], length=%u [%u bytes])\n",
		 buf, offset, offset << 2, length, length << 2);

	ret = packet_write (session, 0x08, b->buf, b->buflen);
	buffer_free (b);

	if (ret != 0) {
		channel_unregister (ch);
		DSFYDEBUG
			("cmd_getsubstreams(): packet_write(cmd=0x08) returned %d, aborting!\n",
			 ret);
	}

	return ret;
}

/*
 * Get metadata for an artist (kind=1) or a list of tracks (kind=3)
 * The response comes as compressed XML
 *
 */
int cmd_browse (SESSION * session, unsigned char kind, unsigned char *idlist,
		int num, channel_callback callback, void *private)
{
	BUFFER *b;
	CHANNEL *ch;
	char buf[256];
	unsigned int dummyint;
	int i, ret;

	assert (((kind == 1 || kind == 2) && num == 1) || kind == 3);

	strcpy (buf, "browse-");
	for (i = 0; i < 17; i++)
		sprintf (buf + 7 + 2 * i, "%02x", idlist[i]);
	ch = channel_register (buf, callback, private);

	b = buffer_init ();
	buffer_append_short (b, ch->channel_id);

	buffer_append_raw (b, &kind, 1);

	for (i = 0; i < num; i++)
		buffer_append_raw (b, idlist + i * 16, 16);

	if (kind == 0x01) {
		assert (num == 1);
		dummyint = 0;
		buffer_append_int (b, dummyint);
	}

	if ((ret =
	     packet_write (session, CMD_BROWSE, b->buf, b->buflen)) != 0) {
		DSFYDEBUG
			("cmd_browse(): packet_write(cmd=0x30) returned %d, aborting!\n",
			 ret)
	}

	buffer_free (b);

	return ret;
}

/*
 * Request playlist details
 * The response comes as plain XML
 *
 */
int cmd_getplaylist (SESSION * session, unsigned char *playlist_id,
		     int unknown, channel_callback callback, void *private)
{
	BUFFER *b;
	CHANNEL *ch;
	unsigned int dummyint;
	unsigned char kind;
	char buf[256];
	int ret;

	strcpy (buf, "playlist-");
	hex_bytes_to_ascii (playlist_id, buf + 9, 17);
	buf[9 + 2 * 17] = 0;
	ch = channel_register (buf, callback, private);

	b = buffer_init ();
	buffer_append_short (b, ch->channel_id);
	buffer_append_raw (b, playlist_id, 17);
	buffer_append_int (b, unknown);
	dummyint = 0;
	buffer_append_int (b, dummyint);
	dummyint = 0xffffffff;
	buffer_append_int (b, dummyint);
	kind = 0x01;
	buffer_append_raw (b, &kind, 1);

	if ((ret =
	     packet_write (session, CMD_PLAYLIST, b->buf, b->buflen)) != 0) {
		DSFYDEBUG
			("cmd_getplaylist(): packet_write(cmd=0x35) returned %d, aborting!\n",
			 ret);
	}

	buffer_free (b);

	return ret;
}

int cmd_ping_reply (SESSION * session)
{
	BUFFER *b;
	unsigned int zero;
	int ret;

	b = buffer_init ();
	zero = 0;
	buffer_append_int (b, zero);

	if ((ret = packet_write (session, CMD_PONG, b->buf, b->buflen)) != 0) {
		DSFYDEBUG
			("cmd_ping_reply(): packet_write(cmd=0x49) returned %d, aborting!\n",
			 ret);
	}

	buffer_free (b);

	return ret;
}

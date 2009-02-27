/*
 * $Id: handlers.c 733 2009-02-23 18:16:17Z x $
 *
 * Default handlers for different types of commands
 *
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <assert.h>

#include "channel.h"
#include "commands.h"
#include "packet.h"
#include "util.h"
#include "event.h"

extern void app_packet_callback (SESSION *, int, unsigned char *, int);

int handle_secret_block (SESSION * session, unsigned char *payload, int len)
{
	unsigned int *t;

	if (len != 336) {
		DSFYDEBUG ("Got cmd=0x02 with len %d, expected 336!\n", len);
		return -1;
	}

	t = (unsigned int *) payload;
	DSFYDEBUG
		("handle_secret_block(): Initial time %u (%ld seconds from now)\n",
		 ntohl (*t), time (NULL) - ntohl (*t));
	t++;
	DSFYDEBUG
		("handle_secret_block(): Invalid at %u (%ld in the future)\n",
		 ntohl (*t), ntohl (*t) - time (NULL));

	t++;
	DSFYDEBUG ("handle_secret_block(): Next value is %u\n", ntohl (*t));
	t++;
	DSFYDEBUG ("handle_secret_block(): Next value is %u\n", ntohl (*t));

	assert (memcmp (session->rsa_pub_exp, payload + 16, 128) == 0);

	/* At payload+16+128 is a  144 bytes (1536-bit) RSA signature */

	/*
	 * Actually the cache hash is sent before the server has sent any
	 * packets. It's just put here out of convenience, because this is
	 * one of the first packets ever by the server, and also not
	 * repeated during a session.
	 *
	 */

	return cmd_send_cache_hash (session);
}

int handle_ping (SESSION * session)
{

	/* Ignore the timestamp but respond to the request */

	return cmd_ping_reply (session);
}

int handle_channel (int cmd, unsigned char *payload, int len)
{
	if (cmd == CMD_CHANNELERR) {
		DSFYDEBUG
			("handle_channel_error: Channel %d got error %d (0x%02x)\n",
			 ntohs (*(unsigned short *) payload),
			 ntohs (*(unsigned short *) (payload + 2)),
			 ntohs (*(unsigned short *) (payload + 2)))
	}

	return channel_process (payload, len, cmd == CMD_CHANNELERR);
}

int handle_aeskey (unsigned char *payload, int len)
{
	CHANNEL *ch;
	int ret;

	DSFYDEBUG ("Server said 0x0d (AES key) for channel %d\n",
		   ntohs (*(unsigned short *) (payload + 2)))
		if ((ch =
		     channel_by_id (ntohs
				    (*(unsigned short *) (payload + 2)))) !=
			   NULL) {
		ret = ch->callback (ch, payload + 4, len - 4);
		channel_unregister (ch);
	}
	else {
		DSFYDEBUG
			("Command 0x0d: Failed to find channel with ID %d\n",
			 ntohs (*(unsigned short *) (payload + 2)));
	}

	return ret;
}

int handle_welcome (SESSION * session, unsigned char *payload, int len)
{
	int ret = 0;

	(void) payload;
	(void) len;		/* don't warn */

#if 0				/* Don't fuck shut up stats */
	unsigned char buf[64 * 1024];
	unsigned char hash[32 + 1];

	len = sprintf ((char *) buf, "ConnectionInfo\t%d\t%s:%u\t%s", 2,
		       session->server_host, session->server_port,
		       "127.0.0.1:1080@socks5");
	if ((ret = packet_write (session, CMD_LOG, buf, len)))
		return -ret;

	hex_bytes_to_ascii (session->cache_hash, (char *) hash, 16);
	len = sprintf ((char *) buf,
		       "CacheReport\t%d\t%.16s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
		       2, hash, 0, 0, 0, 0, 0, 0, 0, 1024 * 1024, 0);
	if ((ret = packet_write (session, CMD_LOG, buf, len)))
		return ret;

	len = sprintf ((char *) buf, "ComputerInfo\t%d\t%s\t%s\t%s\t%s", 1,
		       "", "4096", "1920x1200", "");
	ret = packet_write (session, CMD_LOG, buf, len);
#endif

	/* Request Ad with flag 0 */
	if (ret != -1)
		ret = cmd_requestad (session, 0);

	/* Request Ad with flag 1 */
	if (ret == 0)
		ret = cmd_requestad (session, 1);

	return ret;
}

int handle_packet (SESSION * session,
		   int cmd, unsigned char *payload, int len)
{
	int error = 0;

	switch (cmd) {
	case CMD_SECRETBLK:
		error = handle_secret_block (session, payload, len);
		break;

	case CMD_PING:
		error = handle_ping (session);
		break;

	case CMD_CHANNELDATA:
		error = handle_channel (cmd, payload, len);
		break;

	case CMD_CHANNELERR:
		error = handle_channel (cmd, payload, len);
		break;

	case CMD_AESKEY:
		error = handle_aeskey (payload, len);
		break;

	case CMD_SHAHASH:
		break;

	case CMD_COUNTRYCODE:
		break;

	case CMD_P2P_INITBLK:
		DSFYDEBUG ("Server said 0x21 (P2P initalization block)\n")
			break;

	case CMD_NOTIFY:
		/* HTML-notification, shown in a yellow bar in the official client */
		break;

	case CMD_PRODINFO:
		/* Payload is uncompressed XML */
		break;

	case CMD_WELCOME:
		error = handle_welcome (session, payload, len);
		break;

	case CMD_PAUSE:
		event_msg_post (MSG_CLASS_GUI, MSG_GUI_PAUSE, NULL);
		break;
	}

	app_packet_callback (session, cmd, payload, len);

	return error;
}

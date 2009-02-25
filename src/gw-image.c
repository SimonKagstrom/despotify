/*
 * $Id: gw-browse.c 392 2009-01-23 02:34:11Z x $
 *
 */

#include <string.h>

#include "buffer.h"
#include "channel.h"
#include "commands.h"
#include "gw.h"
#include "gw-image.h"
#include "util.h"


static int gw_image_result_callback(CHANNEL *, unsigned char *, unsigned short);


int gw_image(SPOTIFYSESSION *s, char *id_as_hex) {
	unsigned char id[16];

	s->output = buffer_init();
	s->output_len = 0;

	hex_ascii_to_bytes(id_as_hex, id, 16);

	return cmd_request_image(s->session, id, gw_image_result_callback, (void *)s);
}


static int gw_image_result_callback(CHANNEL *ch, unsigned char *data, unsigned short len) {
	SPOTIFYSESSION *s = (SPOTIFYSESSION *)ch->private;
	BUFFER *b = (BUFFER *)s->output;


	/* Ignore those unknown data bytes */
	if(ch->state == CHANNEL_HEADER)
		return 0;


	if(ch->state == CHANNEL_ERROR) {
		s->state = CLIENT_STATE_COMMAND_COMPLETE;

		buffer_free(b);

		s->output = NULL;
		s->output_len = -1;

		return 0;
	}
	else if(ch->state == CHANNEL_END) {
		s->state = CLIENT_STATE_COMMAND_COMPLETE;

		s->output_len = b->buflen;

		return 0;
	}


	buffer_check_and_extend(b, len);
	buffer_append_raw(b, data, len);

	return 0;
}

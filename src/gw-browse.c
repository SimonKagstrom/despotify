/*
 * $Id: gw-browse.c 392 2009-01-23 02:34:11Z x $
 *
 */

#include <string.h>
#include <assert.h>

#include "buffer.h"
#include "channel.h"
#include "commands.h"
#include "gw.h"
#include "gw-browse.h"
#include "util.h"


static int gw_browse_result_decompress(DECOMPRESSCTX *, unsigned char *, unsigned short);
static int gw_browse_result_callback(CHANNEL *, unsigned char *, unsigned short);


int gw_browse(SPOTIFYSESSION *s, unsigned char kind, char *id_as_hex) {
	DECOMPRESSCTX *dctx;
	unsigned char id[16];
	int i;
	unsigned int value;


	hex_ascii_to_bytes(id_as_hex, id, 16);

	dctx = (DECOMPRESSCTX *)malloc(sizeof(DECOMPRESSCTX));

	dctx->z.zalloc = Z_NULL;
	dctx->z.zfree = Z_NULL;
	dctx->z.opaque = Z_NULL;
	dctx->z.avail_in = 0;
	dctx->z.next_in = Z_NULL;
	if(inflateInit2(&dctx->z, -MAX_WBITS) != Z_OK) {
		free(dctx);
		return -1;
	}

	dctx->decompression_done = 0;
	dctx->b = buffer_init();

	s->output = dctx;
	s->output_len = 0;

	return cmd_browse(s->session, kind, id, 1, gw_browse_result_callback, (void *)s);
}


static int gw_browse_result_callback(CHANNEL *ch, unsigned char *buf, unsigned short len) {
	SPOTIFYSESSION *s = (SPOTIFYSESSION *)ch->private;
	DECOMPRESSCTX *dctx = (DECOMPRESSCTX *)s->output;
	int skip_len;

	/* Ignore those unknown data bytes */
	if(ch->state == CHANNEL_HEADER)
		return 0;


	if(ch->state == CHANNEL_ERROR) {
		s->state = CLIENT_STATE_COMMAND_COMPLETE;

		inflateEnd(&dctx->z);
		buffer_free(dctx->b);
		free(dctx);

		s->output = NULL;
		s->output_len = -1;

		return 0;
	}
	else if(ch->state == CHANNEL_END) {
		s->state = CLIENT_STATE_COMMAND_COMPLETE;

		/*
		 * Return the buffer with the uncompressed XML
		 * in ->output
		 *
		 */
		s->output = dctx->b;
		s->output_len = dctx->b->buflen;

		free(dctx);

		return 0;
	}


	/* Skip a minimal gzip header */
	if(ch->total_data_len < 10) {
		skip_len = 10 - ch->total_data_len;
		while(skip_len && len) {
			skip_len--;
			len--;
			buf++;
		}

		if(len == 0)
			return 0;
	}


	/* Parse more data */
	return gw_browse_result_decompress(dctx, buf, len);
}


#define CHUNKSZ   65536
static int gw_browse_result_decompress(DECOMPRESSCTX *dctx, unsigned char *data, unsigned short len) {
	int e;
	int ret;


	/* For some reason there are 8 extra bytes after the z-data */
	if(dctx->decompression_done) {
		return 0;
	}

	dctx->z.next_in = data;
	dctx->z.avail_in = len;

	ret = 1;
	do {
		buffer_check_and_extend(dctx->b, CHUNKSZ);
		dctx->z.avail_out = dctx->b->allocated - dctx->b->buflen;
		dctx->z.next_out = dctx->b->buf + dctx->b->buflen;

		e = inflate(&dctx->z, Z_NO_FLUSH);
		if(e != Z_OK && e != Z_STREAM_END)
			break;

		dctx->b->buflen += CHUNKSZ - dctx->z.avail_out;
	} while(dctx->z.avail_out == 0);

	dctx->z.next_in = Z_NULL;
	dctx->z.next_out = Z_NULL;

	if(e == Z_OK && ret == 1)
		return 0;


	/*
	 * Avoid entering decompression again once we've 
	 * reached Z_STREAM_END. Shouldn't be necessary if
	 * we fully understood the protocol. ;)
	 *
	 */
	dctx->decompression_done = e == Z_STREAM_END;



        /* XXX - Upper layer doesn't handle the error at the moment,
                 will crash if it's free'd before last packet */
        inflateEnd(&dctx->z);

        return Z_STREAM_END - e;
}

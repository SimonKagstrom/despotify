/*
 * $Id$
 *
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "buffer.h"


BUFFER *buffer_init(void) {
	BUFFER *b;

	b = (BUFFER *)malloc(sizeof(BUFFER));
	if(!b)
		return NULL;

	/* Lazy alloc; nothing in the client will ever use this much data */
	b->buf = calloc(1, 0x10000);
	if(!b->buf) {
		free(b);
		return NULL;
	}

	b->allocated = 0x10000;
	b->buflen = 0;

	return b;
}


void buffer_free(BUFFER *b) {
	if(b->buf)
		free(b->buf);

	free(b);
}


void buffer_check_and_extend(BUFFER *b, int len) {
	int need;

	need = b->buflen + len;
	if(need <= b->allocated)
		return;

	b->allocated = need;
	b->buf = realloc(b->buf, b->allocated);
	assert(b->buf != NULL);
}


unsigned char *buffer_append_raw(BUFFER *b, void *data, int len) {
	unsigned char *offset;
	
	buffer_check_and_extend(b, len);

	offset = b->buf + b->buflen;
	memcpy(offset, data, len);
	b->buflen += len;

	return offset;
}


unsigned char *buffer_append_short(BUFFER *b, unsigned short value) {
	unsigned short *dst;
	unsigned char *offset;

	buffer_check_and_extend(b, sizeof(unsigned short));

	offset = b->buf + b->buflen;
	dst = (unsigned short *)offset;
	*dst = htons(value);
	b->buflen += sizeof(unsigned short);

	return offset;
}


unsigned char *buffer_append_int(BUFFER *b, int value) {
	unsigned int *dst;
	unsigned char *offset;

	buffer_check_and_extend(b, sizeof(int));

	offset = b->buf + b->buflen;
	dst = (unsigned int *)offset;
	*dst = htonl(value);
	b->buflen += sizeof(int);

	return offset;
}

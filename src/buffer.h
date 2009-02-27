/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_BUFFER_H
#define DESPOTIFY_BUFFER_H

typedef struct buffer
{
	/* number of bytes allocated */
	int allocated;

	/* number of bytes actually used */
	int buflen;

	/* pointer to memory */
	unsigned char *buf;
} BUFFER;

BUFFER *buffer_init (void);
void buffer_free (BUFFER *);
void buffer_check_and_extend (BUFFER *, int);
unsigned char *buffer_append_raw (BUFFER *, void *, int);
unsigned char *buffer_append_short (BUFFER *, unsigned short);
unsigned char *buffer_append_int (BUFFER *, int);
#endif

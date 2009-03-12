/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_UTIL_H
#define DESPOTIFY_UTIL_H

#ifdef DEBUG
#define DSFYDEBUG(...) { FILE *fd = fopen("/tmp/gui.log","at"); fprintf(fd, "%s:%d ", __FILE__, __LINE__); fprintf(fd, __VA_ARGS__); fclose(fd); }
#else
#define DSFYDEBUG(...)
#endif

#ifdef DEBUG_SNDQUEUE
#define DSFYDEBUG_SNDQUEUE(...) DSFYDEBUG(__VA_ARGS__)
#else
#define DSFYDEBUG_SNDQUEUE(...)
#endif

#define DSFYfree(p) free(p); (p) = NULL
#define DSFYstrncat(target, data, len) strncat(target, data, sizeof(target)-strlen(target)-1-len)

unsigned char *hex_ascii_to_bytes (char *, unsigned char *, int);
char *hex_bytes_to_ascii (unsigned char *, char *, int);
void hexdump8x32 (char *, void *, int);
void fhexdump8x32 (FILE *, char *, void *, int);
void logdata (char *, int, void *, int);
ssize_t block_read (int, void *, size_t);
ssize_t block_write (int, void *, size_t);
#endif

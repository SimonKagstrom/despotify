/*
 * $Id: util.h 769 2009-02-24 20:12:52Z a $
 *
 */

#ifndef DESPOTIFY_UTIL_H
#define DESPOTIFY_UTIL_H


#ifdef DEBUG
#define DSFYDEBUG(fmt,...) { FILE *fd = fopen("/tmp/gui.log","at"); fprintf(fd, "%s:%d ", __FILE__, __LINE__); fprintf(fd, fmt, __VA_ARGS__); fclose(fd); }
#else
#define DSFYDEBUG(fmt,...)
#endif


unsigned char *hex_ascii_to_bytes(char *, unsigned char *, int);
char *hex_bytes_to_ascii(unsigned char *, char *, int);
void hexdump8x32(char *, void *, int);
void logdata(char *, int, void *, int);
ssize_t block_read(int, void *, size_t);
#endif

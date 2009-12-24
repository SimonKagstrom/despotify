/*
 * $Id: audio.h 100 2009-03-01 13:05:35Z jorgenpt $
 *
 */

#ifndef AUDIO_H
#define AUDIO_H

#ifdef DEBUG
#define DSFYDEBUG(...) { FILE *fd = fopen("/tmp/despotify-libao.log","at"); fprintf(fd, "%s:%d %s() ", __FILE__, __LINE__, __func__); fprintf(fd, __VA_ARGS__); fclose(fd); }
#else
#define DSFYDEBUG(...)
#endif

#include "despotify.h"

void* audio_init (void);
int audio_exit (void*);
int audio_play_pcm(void*, struct pcm_data*);

#endif

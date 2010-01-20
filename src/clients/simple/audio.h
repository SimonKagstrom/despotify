/*
 * $Id: audio.h 100 2009-03-01 13:05:35Z jorgenpt $
 *
 */

#ifndef AUDIO_H
#define AUDIO_H

#include "despotify.h"

void* audio_init (void);
int audio_exit (void*);
int audio_play_pcm(void*, struct pcm_data*);

#endif

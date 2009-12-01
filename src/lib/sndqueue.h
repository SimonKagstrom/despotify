/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_SNDQUEUE_H
#define DESPOTIFY_SNDQUEUE_H

#include <pthread.h>
#include <vorbis/vorbisfile.h>

#include "despotify.h"

#if defined(__linux__) 
	#include <endian.h>
#endif
#if defined(__FreeBSD__)
	#include <machine/endian.h>
	#define __BYTE_ORDER _BYTE_ORDER
	#define __LITTLE_ENDIAN _LITTLE_ENDIAN
#endif


#if defined(__BYTE_ORDER)
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		#define SYSTEM_ENDIAN 0
	#else
		#define SYSTEM_ENDIAN 1
	#endif
#else
	#warning "Unknown endian - Assuming little endian"
	#define SYSTEM_ENDIAN 0
#endif


/* Default buffer treshold value is 80 % */
#define BUFFER_THRESHOLD 80;

enum
{
    SND_CMD_START,
    SND_CMD_DATA,
    SND_CMD_END,
    SND_CMD_CHANNEL_END
};

typedef int (*audio_request_callback) (void *);
typedef void (*time_tell_callback) (struct despotify_session *, int cur_time);

void snd_reset (struct despotify_session* ds);
bool snd_init (struct despotify_session* ds);
void snd_destroy (struct despotify_session *);
void snd_set_data_callback (struct despotify_session *, audio_request_callback, void *);
void snd_set_end_callback (struct despotify_session* ds,
			   audio_request_callback callback, void *arg);
void snd_set_timetell_callback (struct despotify_session* ds,
                                time_tell_callback callback);

int snd_stop (struct despotify_session* ds);
int snd_next (struct despotify_session *ds);
void snd_start (struct despotify_session* ds);
void snd_ioctl (struct despotify_session *session, int cmd, void *data, int length);
long pcm_read (struct despotify_session* ds, char *buffer, int length,
               int bigendianp, int word, int sgned, int *bitstream);

void snd_mark_dlding (struct despotify_session* ds);
void snd_mark_idle (struct despotify_session* ds);
void snd_mark_end (struct despotify_session* ds);

size_t snd_ov_read_callback(void *ptr, size_t size, size_t nmemb, void* ds);
long snd_pcm_read(struct despotify_session* ds,
                  char *buffer, int length, int bigendianp,
                  int word, int sgned, int *bitstream);
int snd_get_pcm(struct despotify_session*, struct pcm_data*);
#endif

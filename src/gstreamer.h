#ifndef DESPOTIFY_GSTREAMER_H
#define DESPOTIFY_GSTREAMER_H

#include "audio.h"
enum
{
        GST_IDLE = 0,
        GST_PLAYING,
        GST_PAUSED,
        GST_END
};

int gstaudio_init_device (void *);
int gstaudio_free_device (void);
int gstaudio_prepare_device (AUDIOCTX *);
int gstaudio_play (AUDIOCTX *);
int gstaudio_stop (AUDIOCTX *);

/* Need to be exposed to audio.c, declared in coreaudio.c */
extern AUDIODRIVER *driver;
#endif

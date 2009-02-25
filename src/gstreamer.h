#ifndef DESPOTIFY_GSTREAMER_H
#define DESPOTIFY_GSTREAMER_H

#include <gst/gst.h>

#include "audio.h"

typedef struct gst_private
{
        GstElement *pipeline;
        GstElement *src;
        GMainLoop *loop;
} gst_PRIVATE;

int gstaudio_init_device (void *);
int gstaudio_free_device (void);
int gstaudio_prepare_device (AUDIOCTX *);
int gstaudio_play (AUDIOCTX *);
int gstaudio_stop (AUDIOCTX *);
#endif

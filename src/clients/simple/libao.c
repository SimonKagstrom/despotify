/*
 * $Id: libao.c 433 2009-10-26 10:36:25Z noah-w $
 *
 * Libao audio output driver for Despotify
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include <ao/ao.h>
#include "audio.h"
#include "util.h"

void* audio_init(void)
{
    ao_device *device;
    ao_sample_format format;
    int default_driver;

    ao_initialize ();
    default_driver = ao_default_driver_id ();

    format.bits = 16;
    format.rate = 44100;
    format.channels = 2;
    format.byte_format = AO_FMT_LITTLE;

    device = ao_open_live (default_driver, &format, NULL);
    if (!device) {
        DSFYDEBUG ("ao_open_live() failed\n");
        return NULL;
    }

    return device;
}

int audio_exit (void* device)
{
    if (ao_close ((ao_device*)device) == 0) {
        DSFYDEBUG ("ao_close() failed\n");
        return -1;
    }

    ao_shutdown ();

    return 0;
}

int audio_play_pcm (void* device, struct pcm_data* pcm)
{
    if (ao_play ((ao_device*)device, pcm->buf, pcm->len) == 0) {
        DSFYDEBUG ("ao_play() failed\n");
        return -1;
    }

    return 0;
}

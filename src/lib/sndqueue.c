/*
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "sndqueue.h"
#include "util.h"

/* Reset for new song */
void snd_reset(struct despotify_session* ds)
{
	DSFYDEBUG ("snd_reset(): Setting state to DL_IDLE\n");
	ds->fifo->totbytes = 0;
	ds->dlstate = DL_IDLE;
        ov_clear(ds->vf);
        DSFYfree(ds->vf);
}

/* Initialize sound session, called once */
bool snd_init(struct despotify_session *ds)
{
	DSFYDEBUG ("Initializing sound FIFO etc (happens once)\n");

	ds->dlstate = DL_IDLE;

	/* This is the fifo that will hold fragments of compressed audio */
        ds->fifo = calloc(1, sizeof(struct snd_fifo));
        if (!ds->fifo)
		return false;

        if (pthread_mutex_init (&ds->fifo->lock, NULL)) {
            DSFYfree (ds->fifo);
            return NULL;
	}

	if (pthread_cond_init (&ds->fifo->cs, NULL)) {
            DSFYfree (ds->fifo);
		pthread_mutex_destroy (&ds->fifo->lock);
		return NULL;
	}

        return true;
}

/* Destroy a sound session */
void snd_destroy (struct despotify_session* ds)
{
	DSFYDEBUG ("Destroying sound FIFO etc\n");

	/* Free the fifo */
	if (ds->fifo) {
		pthread_mutex_lock(&ds->fifo->lock);

		/* free buffers */
		while (ds->fifo->start) {
                        void* b = ds->fifo->start;
			ds->fifo->start = ds->fifo->start->next;
			free (b);
		}

                pthread_mutex_unlock(&ds->fifo->lock);
		pthread_cond_destroy(&ds->fifo->cs);
		pthread_mutex_destroy(&ds->fifo->lock);
                
		DSFYfree (ds->fifo);
	}
}

/* called by snd_get_pcm() */
static void snd_fill_fifo(struct despotify_session* ds)
{
    pthread_mutex_lock(&ds->fifo->lock);
    
	/* Make sure we've always got 10 seconds of data buffered */
	if ((ds->dlstate != DL_END) &&
            ds->fifo->totbytes <
            ov_raw_tell (ds->vf) + 10 * ds->track->file_bitrate / 8) {

		DSFYDEBUG_SNDQUEUE
			("we're low on data (FIFO total %.3fkB, vorbis offset %.3fkB, want %.3fkB)\n",
			 ds->fifo->totbytes / 1024.0,
			 ov_raw_tell (ds->vf) / 1024.0,
			 ((int) ov_raw_tell (ds->vf) +
			  10 * ds->bitrate / 8 -
			  ds->fifo->totbytes) / 1024.0);

		/* threshold level of the available buffer has been consumed,
		   request more data */

		if ( ds->dlstate == DL_IDLE) {

			/* Call audio request function */
			DSFYDEBUG("Low on data (%d / %lld), calling despotify_snd_read_stream\n",
                                 ds->fifo->totbytes,
                                 ov_raw_tell(ds->vf));
			despotify_snd_read_stream(ds);

			DSFYDEBUG("read_stream() has been called, setting snd state to DL_DOWNLOADING\n");
			ds->dlstate = DL_DOWNLOADING;
		}

		DSFYDEBUG_SNDQUEUE("pcm_read(): Unlocking ds->fifo after being low on data\n");
	}

	DSFYDEBUG_SNDQUEUE("pcm_read(): Calling ov_read(len=%d), totbytes=%d, ov_raw_tell=%lld\n",
                           length, ds->fifo->totbytes, ov_raw_tell (ds->vf));

    pthread_mutex_unlock(&ds->fifo->lock);    
}

/* This function stops the player thread */
int snd_stop (struct despotify_session *ds)
{
	int ret = 0;

	DSFYDEBUG ("Entering with arg %p. dl state is %d\n", ds, ds->dlstate);

	/* free the ogg fifo */
	while (ds->fifo->start) {
		void* b = ds->fifo->start;
		ds->fifo->start = ds->fifo->start->next;
		free(b);
	}

	ds->fifo->start = NULL;
	ds->fifo->end = NULL;

	/* Reset the session */
	snd_reset(ds);

	return ret;
}

void snd_ioctl (struct despotify_session* ds, int cmd, void *data, int length)
{
        struct snd_buffer* buff = calloc(1, sizeof(struct snd_buffer) + length);
	if (!buff) {
		perror ("malloc failed");
		exit (-1);
	}

	buff->cmd = cmd;

	if (length > 0) {
		/* Copy data into buffer */
		memcpy (buff->data, data, length);
		buff->length = length;
	}

        pthread_mutex_lock (&ds->fifo->lock);
        
	DSFYDEBUG_SNDQUEUE("Current FIFO totbytes=%d, pushed data length is %d\n", ds->fifo->totbytes, length);
	/* Drop the first 167 bytes due to Spotify's
           weird replay gain header(?) at the start of each stream */
	/* XXX - Ugly hack but I'm too tired to do the math right now ;) */
	if (ds->fifo->totbytes < 167 && length > 167) {
		memcpy(buff->data, data + 167, length - 167);
		buff->length = length - 167;
		DSFYDEBUG("Dropping the first 167 bytes of data in this stream, new length is %d\n", buff->length);
	}

	/* Hook in entry in linked list */
	if (ds->fifo->end != NULL) {
		ds->fifo->end->next = buff;
	}

	ds->fifo->end = buff;

	/* If this is the first entry */
	if (ds->fifo->start == NULL)
		ds->fifo->start = buff;

	ds->fifo->totbytes += buff->length;

	DSFYDEBUG_SNDQUEUE("added a new buffer with %d bytes data, signalling receiver\n", length);

	pthread_mutex_unlock (&ds->fifo->lock);

	/* Signal receiver */
	pthread_cond_signal (&ds->fifo->cs);
}

/*
 * Ogg-Vorbis read() callback
 * Called by both ov_info() and ov_read()
 * 
 * This functions dequeues buffers from the fifo
 *
 * This function is first called from snd_thread when ov_open_callbacks() 
 * is called in order to init the ogg-layer. Then it is called from
 * whatever thread calls pcm_read(). On mac os x this is the coreaudio-thread.
 *
 */
size_t snd_ov_read_callback(void *ptr, size_t size, size_t nmemb, void* session)
{
	size_t length;
	int ptrsize = size * nmemb;
	int remaining;
        struct despotify_session* ds = session;

        pthread_mutex_lock(&ds->fifo->lock);
        
	/* Check queue status */
	if (ds->fifo->start == NULL) {
                /* queue is empty */
		DSFYDEBUG ("FIFO is empty.\n");

		if (ds->dlstate == DL_IDLE) {
                    DSFYDEBUG("State is DL_IDLE, calling despotify_snd_read_stream()\n");
                    /* Request more data */
                    despotify_snd_read_stream(ds);
                    DSFYDEBUG ("Returned from despotify_snd_read_stream()\n");
		}

		/* pthread_cond_wait will lock the queue again
                   as soon as we are signaled */
		DSFYDEBUG ("Waiting for more data using pthread condition ds->fifo->cs\n");
		pthread_cond_wait (&ds->fifo->cs, &ds->fifo->lock);
		DSFYDEBUG ("Condition (ds->fifo->cs) signalled, ds->fifo->lock unlocked!\n");
	}

	DSFYDEBUG_SNDQUEUE ("Processing one buffer at ds->fifo->start."
                            " %zd items of size %zd requested. Totbytes: %d\n",
                            size, nmemb, ds->fifo->totbytes );

	/* We have data .. process one buffer */
	struct snd_buffer* b = ds->fifo->start;

	/* Check if this is the last pkt */
	if (b->cmd == SND_CMD_END) {
		/* Call end callback and return 0 */
		DSFYDEBUG ("Got SND_CMD_END\n");

		/* Increment by one */
		ds->fifo->start = ds->fifo->start->next;

		/* If this was the last entry */
		if (b == ds->fifo->end)
			ds->fifo->end = NULL;

		DSFYfree (b);

                pthread_mutex_unlock(&ds->fifo->lock);
                
                /* Stop sound processing, reset buffers and counters */
                snd_stop(ds);
                snd_reset(ds);

		DSFYDEBUG("Calling despotify_end_of_track\n");
                despotify_snd_end_of_track(ds);

                return 0;
	}

	remaining = b->length - b->consumed;

	if (remaining < ptrsize)
		length = remaining;	/* The entire buffer will fit */
	else
		length = ptrsize;	/* Don't overrun ptrsize */

	memcpy (ptr, &b->data[b->consumed], length);

	b->consumed += length;

	/* If we have used the entire buffer, free it */
	if (b->consumed == b->length) {
		ds->fifo->start = ds->fifo->start->next;

		/* If this was the last entry */
		if (b == ds->fifo->end)
			ds->fifo->end = NULL;

		DSFYfree (b);
	}

        pthread_mutex_unlock(&ds->fifo->lock);
	/* Return number of bytes read to ogg-layer */
	/* If the ogg-layer needs more data it will call us again */
	return length;
}

    
int snd_get_pcm(struct despotify_session* ds, struct pcm_data* pcm)
{
    if (!ds->vf) {
        ov_callbacks callbacks;

        DSFYDEBUG ("Initializing vorbisfile struct\n");

        /* Allocate Vorbis struct */
        ds->vf = calloc(1, sizeof (OggVorbis_File));
        if (!ds->vf)
            return -1;

        /* Initialize Vorbis struct with the appropriate callbacks */
        callbacks.read_func = snd_ov_read_callback;
        callbacks.seek_func = NULL;
        callbacks.close_func = NULL;
        callbacks.tell_func = NULL;
        
        /* Now call ov_open_callbacks(). This will trigger the read_func */
        DSFYDEBUG("Calling ov_open_callbacks()\n");

        int ret = ov_open_callbacks(ds, ds->vf, NULL, 0, callbacks);
        if (ret) {
            DSFYfree(ds->vf);
            DSFYDEBUG("ov_open_callbacks(): error %d (%s)\n",
                      ret,
                      ret == OV_ENOTVORBIS? "not Vorbis":
                      ret == OV_EBADHEADER? "bad header":
                      "unknown, check <vorbis/codec.h>")
                return ret * 10;
        }
        DSFYDEBUG ("Returned from ov_open_callbacks()\n");

    }
    
    vorbis_info* vi = ov_info(ds->vf, -1);

    pcm->samplerate = vi->rate;
    pcm->channels = vi->channels;

    while (1) {
        /* get ogg data */
        snd_fill_fifo(ds);

        if (ds->client_callback) {
            double point = ov_time_tell(ds->vf);
            ds->client_callback(ds, DESPOTIFY_TIME_TELL, &point,
                                ds->client_callback_data);
        }

        /* decode to pcm */
        ssize_t r = ov_read(ds->vf, pcm->buf, sizeof(pcm->buf),
                            SYSTEM_ENDIAN, 2, 1, NULL);
        if (r == OV_HOLE) {
            /* vorbis got garbage */
            DSFYDEBUG ("pcm_read() == OV_HOLE\n");
            continue;
        }
        
        if (r <= 0) {
            DSFYDEBUG ("pcm_read() == %zd\n", r);
            if (r == 0)	/* EOF */
                break;
            return r;
        }

        pcm->len = r;
        break;
    }

    return 0;
}

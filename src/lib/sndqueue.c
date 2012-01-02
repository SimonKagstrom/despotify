/*
 * $Id: sndqueue.c 517 2011-12-11 20:13:34Z dalus $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include <mpg123.h>

#include "sndqueue.h"
#include "util.h"

enum {
    DL_FILLING,
    DL_FILLING_BUSY,
    DL_DRAINING,
    DL_END_OF_LIST
};

static void shortsleep(void)
{
    /* sleep 100 ms */
    struct timespec delay = {0, 100000000};
    nanosleep(&delay, NULL);
}

void snd_reset_codec(struct despotify_session* ds) {
	DSFYDEBUG("Resetting audio codec\n");
	if ( ds->vf ) {
		ov_clear(ds->vf);
		DSFYfree(ds->vf);
	} else if ( ds->mf ) {
		mpg123_close(ds->mf);
		mpg123_delete(ds->mf);
		ds->mf = NULL;
	}
}


/* Reset for new song */
void snd_reset(struct despotify_session* ds)
{
	DSFYDEBUG("Setting state to DL_DRAINING\n");
	ds->fifo->totbytes = 0;
	ds->dlstate = DL_DRAINING;
	snd_reset_codec(ds);
}

/* Initialize sound session, called once */
bool snd_init(struct despotify_session *ds)
{
	DSFYDEBUG ("Initializing sound FIFO etc (happens once)\n");

	DSFYDEBUG("Setting state to DL_FILLING\n");
	ds->dlstate = DL_FILLING;

	/* This is the fifo that will hold fragments of compressed audio */
        ds->fifo = calloc(1, sizeof(struct ds_snd_fifo));
        if (!ds->fifo)
		return false;
        ds->fifo->maxbytes = 1024 * 1024; /* 1 MB default buffer size */
        ds->fifo->watermark = 200 * 1024; /* 200 KB default watermark */

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
                        struct ds_snd_buffer* b = ds->fifo->start;
			ds->fifo->start = ds->fifo->start->next;
			free (b->ptr);
			free (b);
		}

                pthread_mutex_unlock(&ds->fifo->lock);
		pthread_cond_destroy(&ds->fifo->cs);
		pthread_mutex_destroy(&ds->fifo->lock);
                
		DSFYfree (ds->fifo);
	}
}

static void snd_fill_fifo(struct despotify_session* ds)
{
    if (ds->dlabort) {
        while (ds->dlstate == DL_FILLING_BUSY) {
            DSFYDEBUG("dlstate = %d. waiting...\n", ds->dlstate);
            shortsleep();
        }
        ds->dlstate = DL_DRAINING;
        return;
    }

    switch (ds->dlstate) {
        case DL_DRAINING:
            if (ds->fifo->totbytes < ds->fifo->watermark ) {
                DSFYDEBUG("Low on data (%d / %d), fetching another channel\n",
                          ds->fifo->totbytes, ds->fifo->maxbytes);
                DSFYDEBUG("dlstate = DL_FILLING_BUSY\n");
                ds->dlstate = DL_FILLING_BUSY;
                despotify_snd_read_stream(ds);
            }
            break;

        case DL_FILLING:
            if (ds->fifo->totbytes < (ds->fifo->maxbytes - SUBSTREAM_SIZE)) {
                DSFYDEBUG("dlstate = DL_FILLING_BUSY\n");
                ds->dlstate = DL_FILLING_BUSY;
                despotify_snd_read_stream(ds);
            }
            else {
                DSFYDEBUG("buffer filled. setting dlstate DL_DRAINING\n");
                ds->dlstate = DL_DRAINING;
            }
            break;
    }
}
            
/* This function stops the player thread */
int snd_stop (struct despotify_session *ds)
{
	int ret = 0;

	DSFYDEBUG ("Entering with arg %p. dl state is %d\n", ds, ds->dlstate);

        if (ds->dlstate < DL_DRAINING)
            ds->dlabort = true;
            
        while (ds->dlstate == DL_FILLING_BUSY) {
            DSFYDEBUG("dlstate = %d. waiting...\n", ds->dlstate);
            shortsleep();
        }
        
        /* Don't request more data in pcm_read(),
           even if the buffer gets low */
        DSFYDEBUG("dlstate = DL_DRAINING\n");
        ds->dlstate = DL_DRAINING;

        pthread_mutex_lock (&ds->fifo->lock);

	/* free the ogg fifo */
	while (ds->fifo->start) {
		struct ds_snd_buffer* b = ds->fifo->start;
		ds->fifo->start = ds->fifo->start->next;
		free(b->ptr);
		free(b);
	}

	ds->fifo->start = NULL;
	ds->fifo->end = NULL;

	/* Reset the session */
	snd_reset(ds);

        ds->dlabort = false;
        pthread_mutex_unlock (&ds->fifo->lock);

	return ret;
}

int snd_next(struct despotify_session *ds)
{
    pthread_mutex_lock(&ds->fifo->lock);

    /* go through fifo and look for next track */
    struct ds_snd_buffer* b;
    struct ds_snd_buffer* next;
    for (b = ds->fifo->start; b; b = next) {
        if (b->cmd == SND_CMD_START)
            break;

        if (b->ptr)
            free(b->ptr);
        ds->fifo->totbytes -= b->length;
        next = b->next;
        free(b);
    }

    ds->fifo->start = b;

    if (!b) {
        /* we didn't have next track in memory */
        ds->fifo->end = NULL;
        pthread_mutex_unlock(&ds->fifo->lock);
        return 0;
    }
    
    pthread_mutex_unlock(&ds->fifo->lock);

    /* notify client */
    if (ds->client_callback)
        ds->client_callback(ds, DESPOTIFY_NEW_TRACK,
                            b->ptr,
                            ds->client_callback_data);

    /* tell decoder to start over */
    snd_reset_codec(ds);

    return 1;
}

void snd_ioctl (struct despotify_session* ds, int cmd, void *data, int length)
{
        switch (cmd) {
            case SND_CMD_CHANNEL_END:
                /* end of substream */
                if (ds->dlabort) {
                    DSFYDEBUG("ds->dlstate = DL_DRAINING\n");
                    ds->dlstate = DL_DRAINING;
                }
                else
                    if (ds->dlstate != DL_END_OF_LIST) {
                        DSFYDEBUG("ds->dlstate = DL_FILLING\n");
                        ds->dlstate = DL_FILLING; /* step down from DL_FILLING_BUSY */
                    }
                return;

            case SND_CMD_END:
                /* end of track. end of playlist? */
                if (!ds->track) {
                    DSFYDEBUG("ds->dlstate = DL_END_OF_LIST\n");
                    ds->dlstate = DL_END_OF_LIST;
                }
                break;
        }

        if (ds->dlabort) {
            if (data)
                free(data);
            return;
        }

        struct ds_snd_buffer* buff = malloc(sizeof(struct ds_snd_buffer));
	if (!buff) {
		perror ("malloc failed");
		exit (-1);
	}

        buff->length = length;
	buff->cmd = cmd;
        buff->consumed = 0;
        buff->ptr = data;
	buff->next = NULL;

        pthread_mutex_lock (&ds->fifo->lock);

	DSFYDEBUG_SNDQUEUE("Current FIFO totbytes=%d, pushed data length is %d\n", ds->fifo->totbytes, length);

	/* Drop the first ogg page if it is Spotify's
           spec-violating single-page same-serial-number stream */
	if (ds->fifo->lastcmd == SND_CMD_START && (buff->ptr[5] == 6)) {
            int offset = 28; /* size of ogg header */

            /* calculate size of page */
            for (int i=0; i < buff->ptr[26]; i++)
                offset += buff->ptr[i+27];

            if (offset < buff->length) {
                memmove(buff->ptr, buff->ptr + offset, length - offset);
                buff->length -= offset;
                DSFYDEBUG("Dropping the first %d bytes of data in this stream, new length is %d\n", offset, buff->length);
            }
            else {
                DSFYDEBUG("Corrupt first OGG packet gave offset %d. Skipping.", offset);
            }
        }

	/* Hook in entry in linked list */
	if (ds->fifo->end)
            ds->fifo->end->next = buff;

	ds->fifo->end = buff;

	/* If this is the first entry */
	if (ds->fifo->start == NULL)
		ds->fifo->start = buff;

	ds->fifo->totbytes += buff->length;

	/* Signal receiver */
	pthread_cond_signal (&ds->fifo->cs);
	pthread_mutex_unlock (&ds->fifo->lock);

        ds->fifo->lastcmd = cmd;
}

int snd_consume_data(struct despotify_session* ds, int req_bytes, void* private, int (*consumer)(void* source, int bytes, void* private, int offset))
{
    int totlength = 0;
    bool loop = true;

    pthread_mutex_lock(&ds->fifo->lock);


    /* process data */
    while (loop) {
        /* Check queue status */
        if (ds->fifo->start == NULL) {
            _DSFYDEBUG ("Waiting for data (%d bytes)\n", ds->fifo->totbytes);
            pthread_cond_wait (&ds->fifo->cs, &ds->fifo->lock);
            _DSFYDEBUG ("Got data\n");
        }

        DSFYDEBUG_SNDQUEUE("Processing one buffer at ds->fifo->start."
                           " %zd bytes requested. Totbytes: %d\n",
                           req_bytes, ds->fifo->totbytes );

        struct ds_snd_buffer* b = ds->fifo->start;
        if (!b)
            break;

        _DSFYDEBUG("loop cmd:%d bytes:%d\n", b->cmd, ds->fifo->totbytes);

        switch (b->cmd)
        {
            case SND_CMD_START:
                /* first packet of a track */
		DSFYDEBUG ("Got SND_CMD_START\n");

		/* Increment by one */
                ds->fifo->start = ds->fifo->start->next;

                /* notify client */
                if (ds->client_callback)
                    ds->client_callback(ds, DESPOTIFY_NEW_TRACK,
                                        b->ptr,
                                        ds->client_callback_data);
                /* If this was the last entry */
                if (b == ds->fifo->end)
                    ds->fifo->end = NULL;
                if (b->ptr)
                    DSFYfree (b->ptr);
                DSFYfree (b);
                break;
                
            case SND_CMD_DATA:
            {
                /* data packet */
                int remaining = b->length - b->consumed;
                //int ptrsize = size * nmemb;
                int length;
                
                if (totlength + remaining < req_bytes)
                    length = remaining;	/* The entire buffer will fit */
                else {
                    length = req_bytes - totlength; /* Don't overrun ptrsize */
                }

                consumer(b->ptr+b->consumed, length, private, totlength);

                b->consumed += length;
                totlength += length;
        
                /* If we have used the entire buffer, free it */
                if (b->consumed == b->length) {
                    ds->fifo->start = ds->fifo->start->next;
                    ds->fifo->totbytes -= b->length;

                    /* If this was the last entry */
                    if (b == ds->fifo->end)
			ds->fifo->end = NULL;
                    
                    DSFYfree (b->ptr);
                    DSFYfree (b);
                }

                /* exit if input is empty or output is full */
                if (!ds->fifo->start || totlength == (int)(req_bytes))
                    loop = false;
                break;
            }

            case SND_CMD_END:
                /* last packet of a track, return 0 bytes to signal EOF */
		DSFYDEBUG ("Got SND_CMD_END\n");

                /* if there already are bytes to return,
                   send them first and then come back here empty */
                if (totlength) {
                    loop = false;
                    break;
                }

		/* Increment by one */
		ds->fifo->start = ds->fifo->start->next;

		/* If this was the last entry */
		if (b == ds->fifo->end)
                    ds->fifo->end = NULL;
                
                /* If this was the last entry */
                if (b == ds->fifo->end)
                    ds->fifo->end = NULL;
                if (b->ptr)
                    DSFYfree (b->ptr);
		DSFYfree (b);

		_DSFYDEBUG("Calling despotify_end_of_track\n");

                if (!ds->fifo->start) {
                    /* (snd_stop locks the mutex internally) */
                    pthread_mutex_unlock(&ds->fifo->lock);
                    snd_stop(ds);
                    pthread_mutex_lock(&ds->fifo->lock);

                    if (ds->client_callback)
                        ds->client_callback(ds, DESPOTIFY_END_OF_PLAYLIST,
                                            NULL, ds->client_callback_data);
                }
                /* return 0 bytes as EOF marker to decoder */
                loop = false;
                break;
        }
    }

    pthread_mutex_unlock(&ds->fifo->lock);

    /* Return number of bytes read to ogg-layer */
    DSFYDEBUG("Returning %d bytes. %d left.\n",
               totlength, ds->fifo->totbytes);
    return totlength;
}


static int vorbis_consume(void* source, int bytes, void* private, int offset)
{
    memcpy(private+offset,source,bytes);
    return bytes;
}

/*
 * Ogg-Vorbis read() callback
 * Called by both ov_info() and ov_read()
 * 
 * This functions dequeues buffers from the fifo
 *
 */
size_t snd_ov_read_callback(void *ptr, size_t size, size_t nmemb, void* session)
{
    struct despotify_session* ds = session;

    /* TODO: Add function ptr */
    return snd_consume_data(ds,size*nmemb,ptr,vorbis_consume);
}

static int mpeg_consume(void* source, int bytes, void* private, int offset)
{
   (void)offset;
    mpg123_feed(private,source,bytes);
    return bytes;
}


int snd_mpeg_feed_more_data(struct despotify_session* ds) 
{
    /* TODO: Fix this sizeof hack */
    struct ds_pcm_data data;

    return snd_consume_data(ds,sizeof(data.buf),ds->mf,mpeg_consume);
}

int snd_do_vorbis(struct despotify_session* ds, struct ds_pcm_data* pcm ) {
    if (!ds->vf) {
        DSFYDEBUG ("Initializing vorbisfile struct\n");

        /* Allocate Vorbis struct */
        ds->vf = calloc(1, sizeof (OggVorbis_File));
        if (!ds->vf)
            return -1;

        /* Initialize Vorbis struct with the appropriate callbacks */
        ov_callbacks callbacks;
        callbacks.read_func = snd_ov_read_callback;
        callbacks.seek_func = NULL;
        callbacks.close_func = NULL;
        callbacks.tell_func = NULL;
        
        /* Now call ov_open_callbacks(). This will trigger the read_func */
        _DSFYDEBUG("Calling ov_open_callbacks()\n");

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
    }
    
    vorbis_info* vi = ov_info(ds->vf, -1);

    pcm->samplerate = vi->rate;
    pcm->channels = vi->channels;

    while (1) {
        ssize_t r;
        /* decode to pcm */
#if defined(USE_TREMOR)
        r = ov_read(ds->vf, pcm->buf, sizeof(pcm->buf),
                NULL);
#else
        r = ov_read(ds->vf, pcm->buf, sizeof(pcm->buf),
                SYSTEM_ENDIAN, 2, 1, NULL);
#endif

        /* assume no valid data read. */
        pcm->len = 0;

        if (r == OV_HOLE) {
            /* vorbis got garbage */
            DSFYDEBUG ("pcm_read() == OV_HOLE\n");
            continue;
        }
        
        if (r < 0) {
            DSFYDEBUG ("pcm_read() == %zd\n", r);
            return r;
        }

        if (r == 0) {
            /* end of file */
            ov_clear(ds->vf);
            DSFYfree(ds->vf);
            return 0;
        }

        /* valid data *was* read, update length. */
        pcm->len = r;

        if (ds->client_callback) {
            double point = ov_time_tell(ds->vf);
            ds->client_callback(ds, DESPOTIFY_TIME_TELL, &point,
                                ds->client_callback_data);
        }

        /* top up fifo */
        snd_fill_fifo(ds);

        break;
    }
    return 0;
}

int snd_do_mpeg(struct despotify_session* ds, struct ds_pcm_data* pcm) {
	int err = MPG123_OK;
	size_t bytes = 0;
	long rate;
	int channels, enc;

	if ( !ds->mf ) {
		err = mpg123_init();
		
		if ( err != MPG123_OK ) {
			DSFYDEBUG("Unable to initialize mpg123\n");
			return err*10;
		}
	
		ds->mf = mpg123_new(NULL,&err);
		
		if ( ds->mf == NULL || err != MPG123_OK ) {
			DSFYDEBUG("Unable to initialize mpg123 (alloc)\n");
			return err*10;
		}
		
		err = mpg123_format_none(ds->mf);
		if ( err != MPG123_OK ) {
			DSFYDEBUG("Unable to clear output formats (%d)\n",err);
			mpg123_delete(ds->mf);
			ds->mf = NULL;
			return err*10;
		}
		
		err = mpg123_format(ds->mf, 44100, MPG123_MONO | MPG123_STEREO, MPG123_ENC_SIGNED_16);
		
		if ( err != MPG123_OK ) {
			DSFYDEBUG("Unable to set output format (%d)\n",err);
			mpg123_delete(ds->mf);
			ds->mf = NULL;
			return err*10;
		}
		
		err = mpg123_param(ds->mf, MPG123_RVA, MPG123_RVA_MIX, 0);
		
		if ( err != MPG123_OK ) {
			DSFYDEBUG("Unable to set RVA (%d)\n",err);
			mpg123_delete(ds->mf);
			ds->mf = NULL;
			return err*10;
		}
		
#ifdef DEBUG
		err = mpg123_param(ds->mf, MPG123_VERBOSE, 3, 0);
		
		if ( err != MPG123_OK ) {
			DSFYDEBUG("Unable to set verbose (%d)\n",err);
			mpg123_delete(ds->mf);
			ds->mf = NULL;
			return err*10;
		}
#endif
		err = mpg123_param(ds->mf, MPG123_ADD_FLAGS, MPG123_SEEKBUFFER, 0);
		
		if ( err != MPG123_OK ) {
			DSFYDEBUG("Unable to add seekbuffer (%d)\n",err);
			mpg123_delete(ds->mf);
			ds->mf = NULL;
			return err*10;
		}
		
		mpg123_open_feed(ds->mf);
		/* Initial feed of data */
		if ( snd_mpeg_feed_more_data(ds) == 0 ) {
			DSFYDEBUG("Failed during initial feed of data\n");
			return -1*10;
		}
	}
	
	while ( 1 ) {
		do {
			err = mpg123_read(ds->mf,pcm->buf,sizeof(pcm->buf),&bytes);
			
			if ( err == MPG123_NEED_MORE ) {
				if ( snd_mpeg_feed_more_data(ds) == 0) {
					DSFYDEBUG("Track is done\n");
					err = MPG123_DONE;
				}
			}
		}
		while ( bytes == 0 && err == MPG123_NEED_MORE);
		
		if ( err == MPG123_NEW_FORMAT ) {
			mpg123_getformat(ds->mf,&rate,&channels,&enc);
			
			DSFYDEBUG("New format: %li Hz, %i channels, encoding value %i\n",rate,channels,enc);
			pcm->channels = channels;
			pcm->samplerate = rate;
			
		} else if ( err == MPG123_DONE ) {
			
			mpg123_close(ds->mf);
			mpg123_delete(ds->mf);
			ds->mf = NULL;
			return 0;
		}
		
		pcm->len = bytes;
		if (ds->client_callback) {
			off_t frame_cur;
			off_t frame_left;
			double seconds_cur;
			double seconds_left;
			mpg123_position(ds->mf,0,bytes,&frame_cur,&frame_left,&seconds_cur,&seconds_left);
			
			double point = (double) seconds_cur;
			ds->client_callback(ds, DESPOTIFY_TIME_TELL, &point,
					ds->client_callback_data);
		}
		
		snd_fill_fifo(ds);
		break;
	}
	
	return 0;
}

int snd_stream_is_vorbis(struct despotify_session* ds) {
    int res = 0;
    const char ogg_magic[4] = "OggS";
    pthread_mutex_lock(&ds->fifo->lock);

    do {
    	/* Need two buffer to access the real stream */
    	while ( ds->fifo->start == NULL || ds->fifo->start->next == NULL ) {
    	   _DSFYDEBUG ("Waiting for data (%d bytes)\n", ds->fifo->totbytes);
    	   pthread_cond_wait (&ds->fifo->cs, &ds->fifo->lock);
    	   _DSFYDEBUG ("Got data\n");
    	}

    	struct ds_snd_buffer* b_start = ds->fifo->start;
    	struct ds_snd_buffer* b_head = ds->fifo->start->next;
    	if (!b_start || !b_head) {
    	    res = -2;
    	    break;
    	}

    	_DSFYDEBUG("cmd:%d bytes:%d\n", b_start->cmd, ds->fifo->totbytes);

    	if (b_start->cmd == SND_CMD_START) {
    		/* First buffer in the stream. Detect OGG magic */
    		if (memcmp(b_head->ptr,ogg_magic,sizeof(ogg_magic)) == 0 ) {
    			res = 1;
    		} else {
    			res = 0;
    		}
    	} else {
    		/* Errr.. Not called at start of stream */
    		res = -1;
    	}
    } while ( 0 );
    pthread_mutex_unlock(&ds->fifo->lock);
    return res;
}



int snd_get_pcm(struct despotify_session* ds, struct ds_pcm_data* pcm)
{
    if (!ds || !ds->fifo || !ds->fifo->start) {
        pcm->len = 0;
        shortsleep();
        return 0;
    }

    /* top up fifo */
    snd_fill_fifo(ds);

    if (!ds->vf && !ds->mf) {
    	int res = snd_stream_is_vorbis(ds);
    	if ( res == 1 ) {
    		/* Vorbis */
    		return snd_do_vorbis(ds,pcm);
    	} else if ( res == 0 ) {
    		/* Probably mpeg */
    		return snd_do_mpeg(ds,pcm);
    	} else {
    		/* Errr */
    		return res;
    	}
    }
    
    if (ds->vf ) {
    	return snd_do_vorbis(ds,pcm);
    } else if ( ds->mf ) {
    	return snd_do_mpeg(ds,pcm);
    } else {
    	/* err */
    	return -3;
    }
}

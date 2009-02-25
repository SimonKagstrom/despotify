/*
 * $Id: ui-player.c 760 2009-02-24 08:19:47Z x $
 *
 */


#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "aes.h"
#include "buffer.h"
#include "channel.h"
#include "commands.h"
#include "event.h"
#include "playlist.h"
#include "session.h"
#include "sndqueue.h"
#include "ui-player.h"
#include "util.h"


/* Request 10 seconds worth of compressed audio */
#define REQ_1_SECOND 4096*5
#define REQ_SIZE 5*REQ_1_SECOND

struct playerctx {
	EVENT *event;
	struct playlist *playlist;
	struct track *track;

	/* Offset for next request to cmd_getsubstream() */
	unsigned int offset;
	unsigned int request_size;

        snd_SESSION *snd;

	/* AES CTR state */
	unsigned int state[4 * (10 + 1)];
	unsigned char IV[16];
	unsigned char keystream[16];
};


static void gui_player_play(char *input);
static void gui_player_aes_set_key(struct playerctx *, unsigned char *);
static int gui_player_aes_callback(CHANNEL *, unsigned char *, unsigned short);
static int gui_player_data_callback(CHANNEL *, unsigned char *, unsigned short);
static int gui_player_audio_callback(void *);
static int gui_player_end_callback(void *);

static SESSION *session;
static snd_SESSION *snd;


void gui_player(char *input) {
	if(!strncasecmp(input, "play", 4)) {
		input += 4;
		while(*input && *input == ' ') input++;
		gui_player_play(input);
	}
	else if(!strcasecmp(input, "stop")) {
		event_msg_post(MSG_CLASS_GUI, MSG_GUI_STOP, NULL);
	}
	else if(!strcasecmp(input, "pause")) {
		event_msg_post(MSG_CLASS_GUI, MSG_GUI_PAUSE, NULL);
	}
}


static void gui_player_play(char *input) {
	struct playlist *p, **rootptr;
	struct track *t;
	struct playerctx *playerctx;
	int track_num;
	void **container;

	rootptr = playlist_root();
	p = *rootptr;

	/* Find the currently selected playlist */
	for(p = *rootptr; p; p = p->next)
		if(p->flags & PLAYLIST_SELECTED)
			break;

	/* Fail if none selected or if empty */
	if(p == NULL || p->tracks == NULL) {
		/* XXX - Tell the user there's nothing to play */

		return;
	}

	/* Find song with the specified number, or use the first one */
	if((track_num = atoi(input)) < 1)
		track_num = 1;

	for(t = p->tracks; t; t = t->next)
		if(--track_num == 0 || t->next == NULL)
			break;


	/* Allocate player context to keep track of stuff */
	playerctx = malloc(sizeof(struct playerctx));
	playerctx->event = NULL; /* Filled in by gui_player_event_processor() */
	playerctx->playlist = p;
	playerctx->track = t;
	playerctx->offset = 0;
	playerctx->request_size = REQ_SIZE;

	/* Tell the event processor to fetch key and start playing */
	container = (void **)malloc(sizeof(void *));
	*container = playerctx;
	event_msg_post(MSG_CLASS_GUI, MSG_GUI_PLAY, container);

	DSFYDEBUG("gui_player_play(): Sending MSG_GUI_PLAY for song %s - %s\n", t->title, t->artist);
}


int gui_player_event_processor(EVENT *e, enum ev_flags ev_kind) {
	int err = 0;
	struct track *t = NULL;
	struct playerctx *playerctx = NULL;


	if(ev_kind == EV_MSG) {
		if(e->msg->class == MSG_CLASS_APP) {
			switch(e->msg->msg) {
			case MSG_APP_EXIT:
				/* We could do this in a clearner way.. */
				event_mark_done(e);
				break;

			default:
				break;
			}
		}
		else if(e->msg->class == MSG_CLASS_GUI) {
			switch(e->msg->msg) {
			case MSG_GUI_SESSIONPTR:
				session = *(SESSION **)e->msg->data;
				if(e->state == 0) {
					e->state = 1;

					/* Idle until we explicitly mark ourself as busy */
					event_mark_idle(e);
				}
				break;

			case MSG_GUI_PLAY:
				DSFYDEBUG("%s\n", "gui_player_event_processor(): Got MSG_GUI_PLAY");

				/*
				 * This shouldn't happen. Really.
				 * (except when someone requests play before we've received the song's key)
				 *
				 * 1. Fix your driver,
				 * 2. ..or fix our broken audio system ;)
				 *
				 */
				 assert(e->private && snd->actx == NULL /* can't be NULL with a playerctx */);


				if(e->private && snd->actx != NULL && snd->actx->is_paused ) {
					DSFYDEBUG("%s\n", "MSG_GUI_PLAY: Resuming audio because private and actx->is_paused");
					audio_resume(snd->actx);
				}
				else {
					DSFYDEBUG("%s\n", "gui_player_event_processor(MSG_GUI_PLAY): Playing a new song");
					if(e->private && snd->actx != NULL) {
					    DSFYDEBUG("%s\n", "gui_player_event_processor(MSG_GUI_PLAY): Killing playing song, calling snd_stop()");

					    /* Kill playing song before starting new */
					    snd_stop(snd);
					    
					    if(playerctx)
					      {
						free(playerctx);
						playerctx = NULL;
					      }
					  }

					/* Load playing context from posted message */
					playerctx = *(struct playerctx **)e->msg->data;

					/* Allow the event processor to access this context */
					e->private = playerctx;

					/* Set the event pointer */
					playerctx->event = e;

					/* Set snd session pointer */
					playerctx->snd = snd; 

					/* Start processing.. */
					e->state = 1;
					event_mark_busy(e);
					DSFYDEBUG("%s", "gui_player_event_processor(MSG_GUI_PLAY): Proceeding to state 1 (AES key)\n");
				}

				break;

			case MSG_GUI_PAUSE:
				if(e->private == NULL) {
					break;
				}

				DSFYDEBUG("gui_player_event_processor(MSG_GUI_PAUSE): Pausing current song using global snd=%p with actx=%p\n", snd, snd->actx);
				audio_pause(snd->actx);
				break;

			case MSG_GUI_STOP:
				if((playerctx = (struct playerctx *)e->private) == NULL)
					break;

				DSFYDEBUG("%s", "gui_player_event_processor(MSG_GUI_STOP): Calling snd_stop(), freeing playerctx and marking event as idle in state 1 with private=NULL\n");
				snd_stop(snd);

				free(playerctx);
				playerctx = NULL;
				e->private = NULL;
				e->state = 1;
				event_mark_idle(e);
				break;

			default:
				break;
			}
		}

		return 0;
	}


	DSFYDEBUG("gui_player_event_processor(): Processing EV_RUN in state %d\n", e->state);
	switch(e->state) {
	case 0:
		/* Never reached .. we're waiting for GUI_MSG_SESSIONPTR anyways */
		event_mark_idle(e);
		break;

	/* Fetch the song's AES key if not already present */
	case 1:
		/* Idle until we explicitly mark ourself as busy */
		event_mark_idle(e);

		playerctx = (struct playerctx *)e->private;
		assert(playerctx != NULL);

		t = playerctx->track;
		if(t->key == NULL) {
			DSFYDEBUG("gui_player_event_processor(state=1): Requesting AES key for %s - %s\n", t->title, t->artist);
			if(cmd_aeskey(session, t->file_id, t->track_id, gui_player_aes_callback, playerctx)) {
				event_msg_post(MSG_CLASS_APP, MSG_APP_NET_ERROR, NULL);

				DSFYDEBUG("Fuck, cmd_aeskey() failed for %s - %s\n", t->title, t->artist);

				/*
				 * XXX - Notify about failure?
				 * Implement retrying when we're online again.
				 *
				 */
			}

			/* The AES channel handler will upgrade our state */
			break;
		}

		if(cmd_token_notify(session)) {
			event_msg_post(MSG_CLASS_APP, MSG_APP_NET_ERROR, NULL);

			DSFYDEBUG("cmd_token_notify() failed before attempting to play %s - %s\n", t->title, t->artist);

			/*
			 * XXX - Notify about failure?
			 * Implement retrying when we're online again.
			 *
			 */

			 break;
		}

		/* Expand key and set IV */
		gui_player_aes_set_key(playerctx, t->key);

		e->state++;
		/* Fall through */


	/* Start audio processing */
	case 2:
		/* Idle until we explicitly mark ourself as busy */
		event_mark_idle(e);

		playerctx = (struct playerctx *)e->private;
		assert(playerctx != NULL);

		if(playerctx->track->key == NULL) {
			DSFYDEBUG("%s\n", "In state 2, key is still NULL");
			break;
		}

		if(snd == NULL) {
			playerctx->snd = snd = snd_init();
			DSFYDEBUG("gui_player_event_processor(state=2): called snd_init(), returned session is %p\n", snd);
		}
								  
		/* Setup callback used for audio layer to request ogg-data */
		snd_set_data_callback(snd, gui_player_audio_callback, (void *)playerctx);
		snd_set_end_callback(snd, gui_player_end_callback, (void *)playerctx);


		DSFYDEBUG("gui_player_event_processor(state=2): configured data callback with private ptr=%p, starting sound\n", (void *)playerctx);


		e->state++;

		/* Mark for downloading so we don't accidently get called until done */
		snd_mark_dlding(snd);

		/*
		 * Start sound thread which will:
		 * - Call ov_open_callbacks()
		 *   - Call snd_read_and_dequeue_callback() which is the Ogg/Vorbis reader
		 *     - Call gui_player_audio_callback() to get more data
		 *       - Which will mark this event processor as busy so we're called 
		 *         with EV_RUN in state 3, where we'll call cmd_getsubstream()
		 *     - Sleep until more data is available
		 *
		 * - When run in EV_RUN state 3, the callback for cmd_getsubstream()
		 *   will call snd_ioctl() to deliver more data, which in turn will
		 *   signal snd_read_and_dequeue_callback() to wake up
		 *
		 */

		snd_start(snd); 

		/* Fall through */

	/* Request more data */
	case 3:
		/* Idle until we explicitly mark ourself as busy */
		event_mark_idle(e);

		if((playerctx = (struct playerctx *)e->private) == NULL) {
			DSFYDEBUG("%s", "gui_player_event_processor(state=3): Got NULL e->private (shouldn't happen)\n");
			break;
		}

		DSFYDEBUG("Calling cmd_getsubstreams() with offset %d, size %d data\n",
				playerctx->offset, playerctx->request_size);
		if(cmd_getsubstreams(session, playerctx->track->file_id,
					playerctx->offset,
				        playerctx->request_size,
					200*1000 /* unknown, static value */,
					gui_player_data_callback, playerctx)) {

			event_msg_post(MSG_CLASS_APP, MSG_APP_NET_ERROR, NULL);
			t = playerctx->track;
			DSFYDEBUG("Fuck, cmd_getsubstreams() failed for %s - %s\n", t->title, t->artist);

			/* XXX - Notify about network failure somehow */
		}
		else {
#ifndef X_TEST
			/* Tell ogg-layer not to request more data untill we have finished downloading */
			snd_mark_dlding(snd);
#endif
			DSFYDEBUG("%s\n", "cmd_getsubstreams() succeeded");
		}
		
		break;

	default:
		break;
	}


	return err;
}


/* Handle an incoming AES key */
static int gui_player_aes_callback(CHANNEL *ch, unsigned char *buf, unsigned short len) {
	struct playerctx *p = (struct playerctx *)ch->private;
	struct track *t = p->track;


	switch(ch->state) {
	case CHANNEL_DATA:
		if(t->key)
			free(t->key);

		t->key = malloc(len);
		memcpy(t->key, buf, len);

		/* Prepare for decryption */
		gui_player_aes_set_key(p, t->key);

		/* Start audio system */
		p->event->state = 2;
		event_mark_busy(p->event);
		DSFYDEBUG("%s\n", "Got AES key");
		break;

	default:
		break;
	}

	return 0;
}


/* Prepare for decryption */
static void gui_player_aes_set_key(struct playerctx *p, unsigned char *key) {
	/* Expand file key */
	rijndaelKeySetupEnc(p->state, key, 128);

	/* Set initial IV */
	memcpy(p->IV, "\x72\xe0\x67\xfb\xdd\xcb\xcf\x77\xeb\xe8\xbc\x64\x3f\x63\x0d\x93", 16);
}


/* Handle encrypted song data */
static int gui_player_data_callback(CHANNEL *ch, unsigned char *buf, unsigned short len) {
	struct playerctx *p = (struct playerctx *)ch->private;

	unsigned char *ciphertext, *plaintext;
	unsigned char *w, *x, *y, *z;
	int block, i, j;


	switch(ch->state) {
	case CHANNEL_DATA:
		DSFYDEBUG("gui_player_data_callback(id=%d): CHANNEL_DATA with %d bytes of song data (previously processed a total of %d bytes)\n", ch->channel_id, len, ch->total_data_len);
		plaintext = (unsigned char *)malloc(len + 1024);

		/* Decrypt each 1024 byte block */
		for(block = 0; block < len/1024; block++) {

			/* Deinterleave the 4x256 byte blocks */
			ciphertext = plaintext + block*1024;
			w = buf + block*1024 + 0*256;
			x = buf + block*1024 + 1*256;
			y = buf + block*1024 + 2*256;
			z = buf + block*1024 + 3*256;
			for(i = 0; i < 1024 && (block*1024+i) < len; i += 4) {
				*ciphertext++ = *w++;
				*ciphertext++ = *x++;
				*ciphertext++ = *y++;
				*ciphertext++ = *z++;
			}

			/* Decrypt 1024 bytes block. This will fail for the last block. */
			for(i = 0; i < 1024 && (block*1024 + i) < len; i += 16) {
				/* Produce 16 bytes of keystream from the IV */
				rijndaelEncrypt(p->state, 10, p->IV, p->keystream);

				/* Update IV counter. This loop is an awesome construction! */
				for(j = 15; j >= 0; j--) {
					p->IV[j] += 1;
					if(p->IV[j] != 0)
						break;
				}

				/* Produce plaintext by XORing ciphertext with keystream */
				for(j = 0; j < 16; j++)
					plaintext[block*1024+i+j] ^= p->keystream[j];
			}
		}

		/* Push data onto the sound buffer queue */
		snd_ioctl(snd, SND_CMD_DATA, plaintext, len);

		free(plaintext);

		break;

	case CHANNEL_ERROR:
		DSFYDEBUG("%s\n", "gui_player_data_callback(): got CHANNEL_ERROR\n");
		/* XXX - handle cleanly */
		exit(1);
		break;

	case CHANNEL_END:
		DSFYDEBUG("gui_player_data_callback(): got CHANNEL_END, processed %d bytes data\n", ch->total_data_len);


		/* Reflect the current offset in the player context */
		p->offset += ch->total_data_len;


		if(ch->total_data_len == p->request_size) {
			/* We have finished downloading the requested data */ 
			snd_mark_idle(snd);
		}
		else {
			DSFYDEBUG("gui_player_data_callback(): Stream returned short coutn (%d of %d requested), marking END\n",
					ch->total_data_len, p->request_size);

			/* Add SND_CMD_END to buffer chain */
			snd_ioctl(snd, SND_CMD_END, NULL, 0 );

			/* Don't request more data in pcm_read(), even if the buffer gets low */
			snd_mark_end(snd); 

			/* NULL event processor's private pointer */
			p->event->private = NULL;
		}

		break;

	default:
		break;
	}


	return 0;
}


static int gui_player_audio_callback(void *arg) {
	struct playerctx *pctx = (struct playerctx *)arg;

	DSFYDEBUG("gui_player_audio_callback(arg=%p): current offset is %d, marking event in state %d busy\n",
			arg, pctx->offset, pctx->event->state);
	event_mark_busy(pctx->event);

	return 0;
}


static int gui_player_end_callback(void *arg) {
	struct playerctx *pctx = (struct playerctx *)arg;
	struct track *t = NULL;

	DSFYDEBUG("gui_player_end_callback(arg=%p)\n",arg);
		
	snd_stop(pctx->snd); 
	snd_reset(pctx->snd);
	
	pctx->event->state = 1; 
	
	/* If at end of this playlist, pick next one */ 
	if((t = pctx->track->next) == NULL)
	  t = pctx->playlist->tracks;
	
	pctx->track = t; 

	/* Fulhack */
	pctx->event->private = pctx;
	pctx->offset = 0;

	DSFYDEBUG("gui_player_end_callback() - pctx->track == %p\n",pctx->track);

	event_mark_busy(pctx->event);

	return 0;
}

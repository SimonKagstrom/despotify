#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "aes.h"
#include "auth.h"
#include "buf.h"
#include "channel.h"
#include "commands.h"
#include "despotify.h"
#include "handlers.h"
#include "keyexchange.h"
#include "network.h"
#include "packet.h"
#include "playlist.h"
#include "session.h"
#include "sndqueue.h"
#include "util.h"

#define BUFFER_SIZE (160*1024 * 5 / 8) /* 160 kbit * 5 seconds */

bool despotify_init()
{
    if (audio_init())
        return false;

    if (network_init() != 0)
        return false;
    return true;
}

bool despotify_cleanup()
{
    if (network_cleanup() != 0)
        return false;
    return true;
}

static void* despotify_thread(void* arg)
{
    struct despotify_session* ds = arg;
    while (1) {
        SESSION* s = ds->session;
        PHEADER hdr;
        unsigned char* payload;

        int err = packet_read(s, &hdr, &payload);
        if (!err) {
            err = handle_packet(s, hdr.cmd, payload, hdr.len);
            DSFYfree(payload); /* Allocated in packet_read() */
        }
    }
}

struct despotify_session* despotify_init_client()
{
    struct despotify_session* ds = calloc(1,sizeof(struct despotify_session));
    if (!ds)
        return NULL;

    ds->session = session_init_client();
    if (!ds->session)
        return NULL;

    return ds;
}

bool despotify_authenticate(struct despotify_session* ds,
                            const char* user,
                            const char* password)
{
    assert(ds != NULL && ds->session != NULL);

    session_auth_set(ds->session, user, password);

    if (session_connect(ds->session) < 0)
    {
        ds->last_error = "Could not connect to server.";
        return false;
    }
    DSFYDEBUG("%s", "session_connect() completed\n");

    if (do_key_exchange(ds->session) < 0)
    {
        ds->last_error = "Key exchange failed.";
        return false;
    }
    DSFYDEBUG("%s", "do_key_exchange() completed\n");

    auth_generate_auth_hash(ds->session);
    key_init(ds->session);

    if (do_auth(ds->session) < 0)
    {
        ds->last_error = "Authentication failed.";
        return false;
    }
    DSFYDEBUG("%s", "do_auth() completed\n");

    pthread_create( &ds->thread, NULL, &despotify_thread, ds);
    
    /* wait until login is ready */
    pthread_mutex_lock(&ds->session->login_mutex);
    pthread_cond_wait(&ds->session->login_cond, &ds->session->login_mutex);
    pthread_mutex_unlock(&ds->session->login_mutex);

    return true;
}

void despotify_exit(struct despotify_session* ds)
{
    despotify_free(ds, true);
}

void despotify_free(struct despotify_session* ds, bool should_disconnect)
{
    assert(ds != NULL && ds->session != NULL);

    if (should_disconnect)
        session_disconnect(ds->session);

    session_free(ds->session);
    free(ds);
}

const char* despotify_get_error(struct despotify_session* ds)
{
    const char* error;

    /* Only session_init_client() failing can cause this. */
    if (!ds)
        return "Could not allocate memory for a new session.";

    error = ds->last_error;
    ds->last_error = NULL;

    return error;
}

/****************************************************
 *
 *  Playback
 *
 */


/* called by channel */
static int despotify_aes_callback(CHANNEL* ch,
                                  unsigned char* buf,
                                  unsigned short len)
{
    if (ch->state == CHANNEL_DATA) {
        struct despotify_session* ds = ch->private;
        struct track* t = ds->track;

        if (t->key)
            DSFYfree(t->key);

        t->key = malloc(len);
        memcpy(t->key, buf, len);

        /* Expand file key */
	rijndaelKeySetupEnc (ds->aes.state, t->key, 128);

	/* Set initial IV */
	memcpy(ds->aes.IV,
               "\x72\xe0\x67\xfb\xdd\xcb\xcf\x77"
               "\xeb\xe8\xbc\x64\x3f\x63\x0d\x93",
               16);

        DSFYDEBUG ("Got AES key\n");

        //snd_mark_dlding(ds->snd_session);
        snd_start(ds->snd_session);
    }
    return 0;
}

static int despotify_substream_callback(CHANNEL * ch,
                                        unsigned char *buf,
                                        unsigned short len)
{
    struct despotify_session* ds = ch->private;

    switch (ch->state) {
	case CHANNEL_HEADER:
            DSFYDEBUG("CHANNEL_HEADER\n");
            break;
            
	case CHANNEL_DATA: {
            int block;

            DSFYDEBUG("id=%d: CHANNEL_DATA with %d bytes of song data (previously processed a total of %d bytes)\n",
                      ch->channel_id, len, ch->total_data_len);
            unsigned char* plaintext = (unsigned char *) malloc (len + 1024);

            /* Decrypt each 1024 byte block */
            for (block = 0; block < len / 1024; block++) {
                int i;

                /* Deinterleave the 4x256 byte blocks */
                unsigned char* ciphertext = plaintext + block * 1024;
                unsigned char* w = buf + block * 1024 + 0 * 256;
                unsigned char* x = buf + block * 1024 + 1 * 256;
                unsigned char* y = buf + block * 1024 + 2 * 256;
                unsigned char* z = buf + block * 1024 + 3 * 256;

                for (i = 0; i < 1024 && (block * 1024 + i) < len; i += 4) {
                    *ciphertext++ = *w++;
                    *ciphertext++ = *x++;
                    *ciphertext++ = *y++;
                    *ciphertext++ = *z++;
                }

                /* Decrypt 1024 bytes block. This will fail for the last block. */
                for (i = 0; i < 1024 && (block * 1024 + i) < len; i += 16) {
                    int j;

                    /* Produce 16 bytes of keystream from the IV */
                    rijndaelEncrypt(ds->aes.state, 10,
                                    ds->aes.IV,
                                    ds->aes.keystream);

                    /* Update IV counter */
                    for (j = 15; j >= 0; j--) {
                        ds->aes.IV[j] += 1;
                        if (ds->aes.IV[j] != 0)
                            break;
                    }

                    /* Produce plaintext by XORing ciphertext with keystream */
                    for (j = 0; j < 16; j++)
                        plaintext[block * 1024 + i + j] ^= ds->aes.keystream[j];
                }
            }


            /* Push data onto the sound buffer queue */
            snd_ioctl(ds->snd_session, SND_CMD_DATA, plaintext, len);
            DSFYfree(plaintext);

            break;
        }

	case CHANNEL_ERROR:
            DSFYDEBUG("got CHANNEL_ERROR\n");
            /* XXX - handle cleanly */
            exit (1);
            break;

	case CHANNEL_END:
            DSFYDEBUG("got CHANNEL_END, processed %d bytes data\n",
                      ch->total_data_len);

            /* Reflect the current offset in the player context */
            ds->offset += ch->total_data_len;

            if (ch->total_data_len == BUFFER_SIZE) {
                /* We have finished downloading the requested data */
                snd_mark_idle(ds->snd_session);
            }
            else {
                DSFYDEBUG("Stream returned short coutn (%d of %d requested), marking END\n",
                          ch->total_data_len, BUFFER_SIZE);

                /* Add SND_CMD_END to buffer chain */
                snd_ioctl(ds->snd_session, SND_CMD_END, NULL, 0);

                /* Don't request more data in pcm_read(),
                   even if the buffer gets low */
                snd_mark_end(ds->snd_session);
            }

            break;

	default:
            break;
    }

    return 0;
}

/* called by pcm_read() when it wants more data */
static int despotify_snd_data_callback(void* arg)
{
    struct despotify_session* ds = arg;

    DSFYDEBUG("Calling cmd_getsubstreams() with offset %d and len %d\n", ds->offset, BUFFER_SIZE);
    if (cmd_getsubstreams(ds->session, ds->track->file_id,
                          ds->offset, BUFFER_SIZE,
                          200 * 1000, /* unknown, static value */
                          despotify_substream_callback, ds))
    {
        DSFYDEBUG("cmd_getsubstreams() failed for %s - %s\n",
                  ds->track->title, ds->track->artist);
        return -1;
    }

    /* we are downloading - don't request more */
    ds->snd_session->dlstate = DL_DOWNLOADING;

    return 0;
}

/* called by snd_read_and_dequeue_callback() at end of song */
static int despotify_snd_end_callback(void* arg)
{
    struct despotify_session* ds = arg;

    /* Stop sound processing, reset buffers and counters */
    snd_stop(ds->snd_session);
    snd_reset(ds->snd_session);
    ds->offset = 0;

    /* Select the next available track in the playlist */
    ds->track = playlist_next_playable(ds->playlist, ds->track);

    int error = 0;
    if (ds->track) {
        /* request key for next track */
        error = cmd_aeskey(ds->session, ds->track->file_id,
                           ds->track->track_id,
                           despotify_aes_callback, ds);
    }
    
    return error;
}

/* called at head of pcm_read() */
static void despotify_snd_timetell_callback(snd_SESSION* snd, int t)
{
    (void)snd;
    (void)t;
}

bool despotify_play(struct despotify_session* ds,
                    struct playlist* pl,
                    struct track* t)
{
    if (ds->snd_session) {
        snd_stop(ds->snd_session);
        snd_reset(ds->snd_session);
        ds->offset = 0;
    }
    else
        ds->snd_session = snd_init();

    /* notify server we're starting playback */
    if (packet_write(ds->session, CMD_TOKENNOTIFY, NULL, 0)) {
        printf("packet_write(CMD_TOKENNOTIFY) failed\n");
        return false;
    }
    
    /* TODO: change to static callbacks */
    snd_set_data_callback(ds->snd_session, despotify_snd_data_callback, ds);
    snd_set_end_callback(ds->snd_session, despotify_snd_end_callback, ds);
    snd_set_timetell_callback(ds->snd_session, despotify_snd_timetell_callback);

    ds->track = t;
    ds->playlist = pl;
    int error = cmd_aeskey(ds->session, t->file_id, t->track_id,
                           despotify_aes_callback, ds);
    if (error) {
        DSFYDEBUG("cmd_aeskey() failed for %s - %s\n", t->title, t->artist);
        return false;
    }

    /* from here everything is handled in despotify_thread() */
    return true;
}

bool despotify_stop(struct despotify_session* ds)
{
    snd_stop(ds->snd_session);
    return true;
}

bool despotify_pause(struct despotify_session* ds)
{
    audio_pause(ds->snd_session->actx);
    return true;
}

bool despotify_resume(struct despotify_session* ds)
{
    audio_resume(ds->snd_session->actx);
    return true;
}

/****************************************************
 *
 *  Search
 *
 */

static int despotify_search_callback(CHANNEL*  ch, 
                                     unsigned char* buf,
                                     unsigned short len)
{
    struct despotify_session* ds = ch->private;
    bool done = false;

    switch (ch->state) {
        case CHANNEL_DATA:
            /* Skip a minimal gzip header */
            if (ch->total_data_len < 10) {
		int skip_len = 10 - ch->total_data_len;
		while (skip_len && len) {
                    skip_len--;
                    len--;
                    buf++;
		}

		if (len == 0)
                    return 0;
            }

            buf_append_data(ds->response, buf, len);
            break;
            
        case CHANNEL_END:
            /* Add tracks */
            playlist_track_update_from_gzxml(ds->playlist,
                                             ds->response->ptr,
                                             ds->response->len);
            
            ds->playlist->flags |= PLAYLIST_LOADED;
            done = true;
            break;

        case CHANNEL_ERROR:
            if (ds->playlist)
                ds->playlist->flags |= PLAYLIST_ERROR;
            done = true;
            break;

        default:
            /* unknown state */
            break;
    }

    if (done) {
        /* tell despotify_search() we're done */
        pthread_mutex_lock(&ds->sync_mutex);
        pthread_cond_signal(&ds->sync_cond);
        pthread_mutex_unlock(&ds->sync_mutex);
    }

    return 0;
}

struct playlist* despotify_search(struct despotify_session* ds,
                                  char* searchtext)
{
    ds->response = buf_new();
    ds->playlist = playlist_new();

    char buf[80];
    snprintf(buf, sizeof buf, "Search: %s", searchtext);
    buf[(sizeof buf)-1] = 0;
    playlist_set_name(ds->playlist, buf);
    playlist_set_author(ds->playlist, ds->session->username);

    int ret = cmd_search(ds->session, searchtext, 
                         despotify_search_callback, ds);
    if (ret) {
        ds->last_error = "cmd_search() failed";
        return NULL;
    }

    /* wait until search is ready */
    pthread_mutex_lock(&ds->sync_mutex);
    pthread_cond_wait(&ds->sync_cond, &ds->sync_mutex);
    pthread_mutex_unlock(&ds->sync_mutex);
    
    buf_free(ds->response);

    if (!ds->playlist->num_tracks) {
        playlist_free(ds->playlist, 1);
        ds->last_error = "No tracks found";
        return NULL;
    }

    return ds->playlist;
}

void despotify_free_playlist(struct playlist* playlist)
{
    playlist_free(playlist, 1);
}

static bool despotify_load_tracks(struct despotify_session *ds)
{
    ds->response = buf_new();

    struct playlist* pl = ds->playlist;

    /* construct an array of 16-byte track ids */
    char* tracklist = malloc(pl->num_tracks * 16);
    struct track* t;
    int i = 0;
    for (t = pl->tracks; t; t = t->next) {
        memcpy(tracklist + i*16, t->track_id, 16);
        i++;
    }
    
    int error = cmd_browse(ds->session, 3, tracklist, pl->num_tracks,
                           despotify_search_callback, ds);
    if (error) {
        DSFYDEBUG("cmd_browse() failed with %d\n", error);
        ds->last_error = "Network error.";
        session_disconnect(ds->session);
        return false;
    }

    /* wait until track fetch is ready */
    pthread_mutex_lock(&ds->sync_mutex);
    pthread_cond_wait(&ds->sync_cond, &ds->sync_mutex);
    pthread_mutex_unlock(&ds->sync_mutex);

    free(tracklist);
    buf_free(ds->response);

    return true;
}

static int despotify_get_playlist_callback (CHANNEL *ch,
                                             unsigned char *buf,
                                             unsigned short len)
{
    struct despotify_session* ds = ch->private;
    bool done = false;

    switch (ch->state) {
	case CHANNEL_DATA:
            buf_append_data(ds->response, buf, len);
            break;

	case CHANNEL_ERROR:
            if (ds->playlist)
                ds->playlist->flags |= PLAYLIST_ERROR;
            done = true;
            break;

	case CHANNEL_END:
            buf_append_u8(ds->response, 0); /* null terminate xml string */
            DSFYDEBUG("playlist response xml:\n%s\n", ds->response->ptr);
            playlist_create_from_xml(ds->response->ptr, ds->playlist);
            done = true;
            break;

	default:
            break;
    }

    if (done) {
        /* tell despotify_get_playlists() that we're done */
        pthread_mutex_lock(&ds->sync_mutex);
        pthread_cond_signal(&ds->sync_cond);
        pthread_mutex_unlock(&ds->sync_mutex);
    }

    return 0;
}

struct playlist* despotify_get_playlist(struct despotify_session *ds,
                                        unsigned char* playlist_id)
{
    ds->response = buf_new();

    /* when loading lists, we fill out an already existing playlist */
    if (!ds->null_playlist)
        ds->playlist = playlist_new();

    static const char* load_lists = 
        "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<playlist>\n";

    buf_append_data(ds->response, (char*)load_lists, strlen(load_lists));

    int error = cmd_getplaylist(ds->session, playlist_id ? playlist_id : (unsigned char*)PLAYLIST_LIST_PLAYLISTS,
                                -1, despotify_get_playlist_callback, ds);
    if (error) {
        DSFYDEBUG("Failed getting playlists\n");
        ds->last_error = "Network error.";
        session_disconnect(ds->session);

        return NULL;
    }

    /* wait until playlist fetch is ready */
    pthread_mutex_lock(&ds->sync_mutex);
    pthread_cond_wait(&ds->sync_cond, &ds->sync_mutex);
    pthread_mutex_unlock(&ds->sync_mutex);

    buf_free(ds->response);

    if (playlist_id) {
        /* fill the playlist with track info */
        if (!despotify_load_tracks(ds)) {
            ds->last_error = "Failed loading track info";
            DSFYDEBUG("%s", ds->last_error);
            return NULL;
        }
    }

    return ds->playlist;
}

struct playlist* despotify_get_stored_playlists(struct despotify_session *ds)
{
    /* load list of lists */
    ds->null_playlist = true;

    ds->playlist = NULL;
    despotify_get_playlist(ds, NULL);

    struct playlist* p;
    for (p = playlist_root(); p; p = p->next) {
        ds->playlist = p;
        despotify_get_playlist(ds, p->playlist_id);
    }

    ds->null_playlist = false;

    return playlist_root();
}

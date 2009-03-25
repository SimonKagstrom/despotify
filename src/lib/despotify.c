#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>

#include "aes.h"
#include "auth.h"
#include "buf.h"
#include "channel.h"
#include "commands.h"
#include "despotify.h"
#include "ezxml.h"
#include "handlers.h"
#include "keyexchange.h"
#include "network.h"
#include "packet.h"
#include "session.h"
#include "sndqueue.h"
#include "util.h"
#include "xml.h"

#define BUFFER_SIZE (160*1024 * 5 / 8) /* 160 kbit * 5 seconds */
#define MAX_BROWSE_REQ 244 /* max entries to load in one browse request */

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
    char fid[20];
    hex_ascii_to_bytes(ds->track->file_id, fid, sizeof fid);

    if (cmd_getsubstreams(ds->session, fid,
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
    struct track* t;
    for (t = ds->playlist->tracks; t && t != ds->track; t = t->next);
    if (t)
        ds->track = t->next;

    int error = 0;
    if (ds->track) {
        char fid[20], tid[16];
        hex_ascii_to_bytes(ds->track->file_id, fid, sizeof fid);
        hex_ascii_to_bytes(ds->track->track_id, tid, sizeof tid);

        /* request key for next track */
        error = cmd_aeskey(ds->session, fid, tid, despotify_aes_callback, ds);
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
    //DSFYDEBUG("pl=%p, t=%p\n", pl, t);

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

    char fid[20], tid[16];
    hex_ascii_to_bytes(ds->track->file_id, fid, sizeof fid);
    hex_ascii_to_bytes(ds->track->track_id, tid, sizeof tid);

    int error = cmd_aeskey(ds->session, fid, tid, despotify_aes_callback, ds);
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


static struct buf* despotify_inflate(unsigned char* data, int len)
{
    struct z_stream_s z;
    memset(&z, 0, sizeof z);

    int rc = inflateInit2(&z, -MAX_WBITS);
    if (rc != Z_OK) {
        DSFYDEBUG("error: inflateInit() returned %d\n", rc);
        return false;
    }

    z.next_in = data;
    z.avail_in = len;

    struct buf* b = buf_new();
    buf_extend(b, 4096);
    bool loop = true;

    int offset = 0;
    while (loop) {
        z.avail_out = b->size - offset;
        z.next_out = b->ptr + offset;

        rc = inflate(&z, Z_NO_FLUSH);
        switch (rc) {
            case Z_OK:
                /* inflated fine */
                if (z.avail_out == 0) {
                    /* zlib needs more output buffer */
                    offset = b->size;
                    buf_extend(b, b->size * 2);
                }
                break;

            case Z_STREAM_END:
                /* end of input */
                loop = false;
                break;

            default:
                /* error */
                DSFYDEBUG("error: inflate() returned %d\n", rc);
                loop = false;
                buf_free(b);
                b = NULL;
                break;
        }
    }
    b->len = b->size - z.avail_out;
    buf_append_u8(b, 0); /* null terminate string */

    inflateEnd(&z);

    return b;
}

static int despotify_plain_callback(CHANNEL *ch,
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
            DSFYDEBUG("!!! channel error\n");
            done = true;
            break;

	case CHANNEL_END:
            done = true;
            break;

	default:
            break;
    }

    if (done) {
        /* tell caller we're done */
        pthread_mutex_lock(&ds->sync_mutex);
        pthread_cond_signal(&ds->sync_cond);
        pthread_mutex_unlock(&ds->sync_mutex);
    }

    return 0;
}

static int despotify_gzip_callback(CHANNEL*  ch, 
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
            done = true;
            break;

        case CHANNEL_ERROR:
            DSFYDEBUG("!!! channel error\n");
            done = true;
            break;

        default:
            /* unknown state */
            break;
    }

    if (done) {
        /* tell caller we're done */
        pthread_mutex_lock(&ds->sync_mutex);
        pthread_cond_signal(&ds->sync_cond);
        pthread_mutex_unlock(&ds->sync_mutex);
    }

    return 0;
}

/****************************************************
 *
 *  Search
 *
 */

struct playlist* despotify_search(struct despotify_session* ds,
                                  char* searchtext)
{
    ds->response = buf_new();
    ds->playlist = calloc(1, sizeof(struct playlist));

    char buf[80];
    snprintf(buf, sizeof buf, "Search: %s", searchtext);
    buf[(sizeof buf)-1] = 0;
    DSFYstrncpy(ds->playlist->name, buf, sizeof ds->playlist->name);
    DSFYstrncpy(ds->playlist->author, ds->session->username, sizeof ds->playlist->author);

    int ret = cmd_search(ds->session, searchtext, 
                         despotify_gzip_callback, ds);
    if (ret) {
        ds->last_error = "cmd_search() failed";
        return NULL;
    }

    /* wait until search is ready */
    pthread_mutex_lock(&ds->sync_mutex);
    pthread_cond_wait(&ds->sync_cond, &ds->sync_mutex);
    pthread_mutex_unlock(&ds->sync_mutex);
    
    /* Add tracks */
    struct buf* b = despotify_inflate(ds->response->ptr, ds->response->len);
    if (b) {
        xml_parse_searchlist(ds->playlist, b->ptr, b->len);
        buf_free(b);
    }
    buf_free(ds->response);

    if (!ds->playlist->num_tracks) {
        despotify_free_playlist(ds->playlist);
        ds->last_error = "No tracks found";
        return NULL;
    }

    return ds->playlist;
}

/**************************************************************
 *
 *  Playlists
 *
 */

void despotify_free_playlist(struct playlist* pl)
{
    void* next_list = pl;
    for (struct playlist* p = next_list; next_list; p = next_list) {
        void* next_track = p->tracks;
        for (struct track* t = next_track; next_track; t = next_track) {
            if (t->key)
                free(t->key);
            next_track = t->next;
            free(t);
        }

        next_list = p->next;
        free(p);
    }
}

static bool despotify_load_tracks(struct despotify_session *ds)
{
    struct playlist* pl = ds->playlist;
    struct track* t = pl->tracks;

    /* construct an array of 16-byte track ids */
    char* tracklist = malloc(MAX_BROWSE_REQ * 16);
 
    /* don't request too many tracks at once */
    int count;
    for (int totcount=0; totcount < pl->num_tracks; totcount += count) {

        ds->response = buf_new();
 
        for (count = 0; t && count < MAX_BROWSE_REQ; t = t->next, count++)
            hex_ascii_to_bytes(t->track_id, tracklist + count * 16, 16);
 
        int error = cmd_browse(ds->session, BROWSE_TRACK, tracklist, count, 
                               despotify_gzip_callback, ds);
 
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
 
        /* Add tracks */
        struct buf* b = despotify_inflate(ds->response->ptr, ds->response->len);
        if (b) {
            xml_parse_searchlist(ds->playlist, b->ptr, b->len);
            buf_free(b);
        }

        buf_free(ds->response);
    }
    free(tracklist);

    return true;
}

struct playlist* despotify_get_playlist(struct despotify_session *ds,
                                        unsigned char* playlist_id)
{
    ds->response = buf_new();
    ds->playlist = calloc(1, sizeof(struct playlist));

    static const char* load_lists = 
        "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<playlist>\n";

    buf_append_data(ds->response, (char*)load_lists, strlen(load_lists));

    char pid[17];
    if (playlist_id)
        hex_ascii_to_bytes(playlist_id, pid, 17);
    else {
        /* enable list_of_lists state */
        ds->list_of_lists = true;
        memset(pid, 0, sizeof pid);
    }
        
    int error = cmd_getplaylist(ds->session, pid,
                                -1, despotify_plain_callback, ds);
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

    buf_append_u8(ds->response, 0); /* null terminate xml string */
    ds->playlist = xml_parse_playlist(ds->playlist,
                                      ds->response->ptr,
                                      ds->response->len,
                                      ds->list_of_lists);
    ds->list_of_lists = false;
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
    struct playlist* metalist = despotify_get_playlist(ds, NULL);
    struct playlist* root = NULL;
    struct playlist* prev = NULL;
    
    int count = 0;

    for (struct playlist* p = metalist; p; p = p->next) {
        struct playlist* new = despotify_get_playlist(ds, p->playlist_id);
        if (prev)
            prev->next = new;
        else
            root = new;
        prev = new;
        count++;
    }
    despotify_free_playlist(metalist);

    root->num_tracks = count;
    return root;
}


/*****************************************************************
 *
 *  Artist / album information
 *
 */

struct artist* despotify_get_artist(struct despotify_session* ds,
                                    char* artist_id)
{
    ds->response = buf_new();
    ds->artist = calloc(1, sizeof(struct artist));

    unsigned char id[16];
    hex_ascii_to_bytes(artist_id, id, sizeof id);
    int error = cmd_browse(ds->session, BROWSE_ARTIST, id, 1, 
                           despotify_gzip_callback, ds);
 
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
 
    struct buf* b = despotify_inflate(ds->response->ptr, ds->response->len);
    if (b) {
        xml_parse_artist(ds->artist, b->ptr, b->len);
        buf_free(b);
    }
    buf_free(ds->response);

    return ds->artist;
}

void despotify_free_artist(struct artist* a)
{
    if (a->text)
        free(a->text);
    
    void* next_album = a->albums;
    for (struct album* al = next_album; next_album; al = next_album) {
        void* next_track = al->tracks;
        for (struct track* t = next_track; next_track; t = next_track) {
            if (t->key)
                free(t->key);
            next_track = t->next;
            free(t);
        }

        next_album = al->next;
        free(al);
    }
    free(a);
}

void* despotify_get_image(struct despotify_session* ds, char* image_id, int* len)
{
    ds->response = buf_new();

    unsigned char id[20];
    hex_ascii_to_bytes(image_id, id, sizeof id);
    int error = cmd_request_image(ds->session, id,
                                  despotify_plain_callback, ds);
    if (error) {
        DSFYDEBUG("cmd_request_image() failed with %d\n", error);
        ds->last_error = "Network error.";
        session_disconnect(ds->session);
        return false;
    }
 
    /* wait until image fetch is ready */
    pthread_mutex_lock(&ds->sync_mutex);
    pthread_cond_wait(&ds->sync_cond, &ds->sync_mutex);
    pthread_mutex_unlock(&ds->sync_mutex);
 
    void* image = ds->response->ptr;
    if (len)
        *len = ds->response->len;
    free(ds->response); /* free() instead of buf_free() since ptr must
                           remain allocated */
    return image;
}

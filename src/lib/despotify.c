#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <zlib.h>

#include "aes.h"
#include "auth.h"
#include "buf.h"
#include "cache.h"
#include "channel.h"
#include "commands.h"
#include "despotify.h"
#include "ezxml.h"
#include "handlers.h"
#include "keyexchange.h"
#include "network.h"
#include "packet.h"
#include "session.h"
#include "sha1.h"
#include "sndqueue.h"
#include "util.h"
#include "xml.h"

#define MAX_BROWSE_REQ 244 /* max entries to load in one browse request */

bool despotify_init()
{
    DSFYDEBUG("Initializing networking\n");
    if (network_init() != 0) {
        DSFYDEBUG("Failed to initialize networking\n");
        return false;
    }

    return true;
}

bool despotify_cleanup()
{
    DSFYDEBUG("Uninitializing networking\n");
    if (network_cleanup() != 0)
        return false;

    return true;
}

static void* despotify_thread(void* arg)
{
    struct despotify_session* ds = arg;

    DSFYDEBUG("Networking thread started\n");

    while (1) {
        SESSION* s = ds->session;
        PHEADER hdr;
        unsigned char* payload;

        if(packet_read(s, &hdr, &payload))
		break;

        handle_packet(s, hdr.cmd, payload, hdr.len);
        DSFYfree(payload); /* Allocated in packet_read() */
    }

    DSFYDEBUG("Networking thread exiting\n");

    return NULL;
}

struct despotify_session* despotify_init_client(void(*callback)(struct despotify_session*, int, void*, void*), void* callback_data, bool high_bitrate, bool use_cache)
{
    struct despotify_session* ds = calloc(1,sizeof(struct despotify_session));
    if (!ds)
        return NULL;

    ds->session = session_init_client();
    if (!ds->session)
        return NULL;

    ds->thread = (pthread_t)0;
    pthread_cond_init(&ds->sync_cond, NULL);
    pthread_mutex_init(&ds->sync_mutex, NULL);

    ds->user_info = &ds->session->user_info;
    ds->client_callback = callback;
    ds->client_callback_data = callback_data;
    ds->high_bitrate = high_bitrate;
    ds->use_cache = use_cache;

    if (use_cache)
        cache_init();

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
    DSFYDEBUG("session_connect() completed\n");

    switch (do_key_exchange(ds->session))
    {
        case 0: /* all ok */
            break;

        case -11:
            ds->last_error = "Client upgrade required";
            return false;

        case -13:
            ds->last_error = "User not found";
            return false;

        case -14:
            ds->last_error = "Account has been disabled";
            return false;

        case -16:
            ds->last_error = "You need to complete your account details";
            return false;

        case -19:
            ds->last_error = "Account/use country mismatch";
            return false;

        default:
            ds->last_error = "Key exchanged failed";
            return false;
    }
    DSFYDEBUG("do_key_exchange() completed\n");

    auth_generate_auth_hash(ds->session);
    if (key_init(ds->session) < 0)
    {
        ds->last_error = "Key computation failed.";
        return false;
    }

    if (do_auth(ds->session) < 0)
    {
        ds->last_error = "Authentication failed. Wrong password?";
        return false;
    }
    DSFYDEBUG("%s", "do_auth() completed\n");

    pthread_create(&ds->thread, NULL, &despotify_thread, ds);

    pthread_mutex_lock(&ds->session->login_mutex);
    pthread_cond_wait(&ds->session->login_cond, &ds->session->login_mutex);
    pthread_mutex_unlock(&ds->session->login_mutex);

    return true;
}

void despotify_exit(struct despotify_session* ds)
{
    DSFYDEBUG("Calling despotify_free() and requesting disconnection\n");
    despotify_free(ds, true);
}

void despotify_free(struct despotify_session* ds, bool should_disconnect)
{
    int r;
    assert(ds != NULL && ds->session != NULL);

    if (should_disconnect) {
        DSFYDEBUG("Disconnection requested, calling session_disconnect()\n");
        session_disconnect(ds->session);
    }

    if (ds->thread) {
        DSFYDEBUG("Canceling despotify networking thread\n");
        r = pthread_cancel(ds->thread);
        DSFYDEBUG("pthread_cancel() returned %d\n", r);
    
        r = pthread_join(ds->thread, NULL);
        DSFYDEBUG("Joined despotify networking thread, return value is %d\n", r);
    }

    snd_destroy(ds);

    session_free(ds->session);
    free(ds);

    DSFYDEBUG("Session uninitialized and free'd\n");
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

void despotify_set_buffer_size(struct despotify_session* ds, int size)
{
    ds->fifo->maxbytes = size;
}

void despotify_set_watermark(struct despotify_session* ds, int watermark)
{
    ds->fifo->watermark = watermark;
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

        /* Add SND_CMD_START to buffer chain,
           with a copy of the track struct */
        struct track* copy = malloc(sizeof(struct track));
        memcpy(copy, ds->track, sizeof(struct track));
        snd_ioctl(ds, SND_CMD_START, copy, 0);
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
            snd_ioctl(ds, SND_CMD_DATA, plaintext, len);
            break;
        }

    case CHANNEL_ERROR:
            DSFYDEBUG("got CHANNEL_ERROR #%d\n", ds->errorcount);
            ds->errorcount += 1;
            if (ds->errorcount > 3)
                exit(-1);
            break;

    case CHANNEL_END:
            DSFYDEBUG("got CHANNEL_END, processed %d bytes data\n",
                      ch->total_data_len);
            ds->errorcount = 0;

            /* Reflect the current offset in the player context */
            ds->offset += ch->total_data_len;

            if (ch->total_data_len < SUBSTREAM_SIZE) {
                DSFYDEBUG("Stream returned short coutn (%d of %d requested), marking END\n",
                          ch->total_data_len, SUBSTREAM_SIZE);

                /* find next playable track */
                do {
                    ds->track = ds->track->next;
                } while (ds->track && !ds->track->playable);
                
                /* Add SND_CMD_END to buffer chain */
                snd_ioctl(ds, SND_CMD_END, NULL, 0);

                ds->offset = 0;

                int error = 0;
                if (ds->track && ds->play_as_list) {
                    char fid[20], tid[16];
                    hex_ascii_to_bytes(ds->track->file_id, fid, sizeof fid);
                    hex_ascii_to_bytes(ds->track->track_id, tid, sizeof tid);
                    
                    /* request key for next track */
                    error = cmd_aeskey(ds->session, fid, tid, despotify_aes_callback, ds);
                }
            }
            snd_ioctl(ds, SND_CMD_CHANNEL_END, NULL, 0);
            break;

    default:
            break;
    }

    return 0;
}

/* called by pcm_read() when it wants more data */
int despotify_snd_read_stream(struct despotify_session* ds)
{
    DSFYDEBUG("Calling cmd_getsubstreams() with offset %d and len %d\n", ds->offset, SUBSTREAM_SIZE);
    char fid[20];
    hex_ascii_to_bytes(ds->track->file_id, fid, sizeof fid);

    if (cmd_getsubstreams(ds->session, fid,
                          ds->offset, SUBSTREAM_SIZE,
                          200 * 1000, /* unknown, static value */
                          despotify_substream_callback, ds))
    {
        DSFYDEBUG("cmd_getsubstreams() failed for %s\n", ds->track->title);
        return -1;
    }

    return 0;
}

bool despotify_play(struct despotify_session* ds,
                    struct track* t, bool play_as_list)
{
    if (ds->fifo) {
        snd_stop(ds);
        ds->offset = 0;
    }
    else
        snd_init(ds);

    /* notify server we're starting playback */
    if (packet_write(ds->session, CMD_TOKENNOTIFY, NULL, 0)) {
        DSFYDEBUG("packet_write(CMD_TOKENNOTIFY) failed\n");
        return false;
    }

    ds->track = t;
    ds->play_as_list = play_as_list;

    char fid[20], tid[16];
    hex_ascii_to_bytes(ds->track->file_id, fid, sizeof fid);
    hex_ascii_to_bytes(ds->track->track_id, tid, sizeof tid);

    int error = cmd_aeskey(ds->session, fid, tid, despotify_aes_callback, ds);
    if (error) {
        DSFYDEBUG("cmd_aeskey() failed for %s\n", t->title);
        return false;
    }

    /* from here everything is handled in despotify_thread() */
    return true;
}

void despotify_next(struct despotify_session* ds)
{
    if (snd_next(ds))
        return;

    /* find next playable track */
    do {
        ds->track = ds->track->next;
    } while (ds->track && !ds->track->playable);

    if (ds->track)
        despotify_play(ds, ds->track, ds->play_as_list);
}

bool despotify_stop(struct despotify_session* ds)
{
    if (!ds->fifo)
        return false;

    snd_stop(ds);
    ds->offset = 0;
    return true;
}

struct track *despotify_get_current_track(struct despotify_session *ds) {
    if (ds->track)
        return ds->track;
    return NULL;
}

static struct buf* despotify_inflate(unsigned char* data, int len)
{
    if (!len)
        return NULL;

    struct z_stream_s z;
    memset(&z, 0, sizeof z);

    int rc = inflateInit2(&z, -MAX_WBITS);
    if (rc != Z_OK) {
        DSFYDEBUG("error: inflateInit() returned %d\n", rc);
        return NULL;
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

    if (b) {
        b->len = b->size - z.avail_out;
        buf_append_u8(b, 0); /* null terminate string */
    }

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
 *  Timeouts
 *
 */
bool despotify_wait_timeout(struct despotify_session* ds){
    struct timeval  tv;
    struct timespec ts;

    /* Get system time */
    gettimeofday(&tv, NULL);

    /* Setup timespec structure */
    ts.tv_sec  = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
    ts.tv_sec += TIMEOUT;

    /* timed wait until response is ready */
    pthread_mutex_lock(&ds->sync_mutex);

    if (pthread_cond_timedwait(&ds->sync_cond, &ds->sync_mutex, &ts) == ETIMEDOUT) {
        pthread_mutex_unlock(&ds->sync_mutex);

        DSFYDEBUG("Timeout while waiting on sync condition\n");

        return false;
    }

    pthread_mutex_unlock(&ds->sync_mutex);

    return true;
}

/****************************************************
 *
 *  Search
 *
 */

struct search_result* despotify_search(struct despotify_session* ds,
                                       char* searchtext, int maxresults)
{
    struct search_result* search = NULL;

    ds->response = buf_new();
    ds->playlist = calloc(1, sizeof(struct playlist));

    char buf[80];
    snprintf(buf, sizeof buf, "Search: %s", searchtext);
    buf[(sizeof buf)-1] = 0;
    DSFYstrncpy(ds->playlist->name, buf, sizeof ds->playlist->name);
    DSFYstrncpy(ds->playlist->author, ds->session->username, sizeof ds->playlist->author);

    int ret = cmd_search(ds->session, searchtext, 0, maxresults,
                         despotify_gzip_callback, ds);
    if (ret) {
        ds->last_error = "cmd_search() failed";
        return NULL;
    }

    /* wait until search is ready */
    if (!despotify_wait_timeout(ds)) {
        ds->last_error = "Timeout when searching";
        return NULL;
    }

    /* Add tracks */
    if (!ds->playlist->tracks)
        ds->playlist->tracks = calloc(1, sizeof(struct track));

    struct buf* b = despotify_inflate(ds->response->ptr, ds->response->len);
    if (b) {
        search = calloc(1, sizeof(struct search_result));
        DSFYstrncpy(search->query, searchtext, sizeof search->query);
        search->playlist = ds->playlist;
        search->tracks = ds->playlist->tracks;

        ds->playlist->num_tracks = xml_parse_search(search, ds->playlist->tracks, b->ptr, b->len, ds->high_bitrate);

        buf_free(b);
    }
    buf_free(ds->response);

    if (!search) {
        ds->last_error = "Error when searching";
        return NULL;
    }

    return search;
}

struct search_result* despotify_search_more(struct despotify_session *ds,
                                            struct search_result *search,
                                            int offset, int maxresults)
{
    if (!search || !search->tracks)
        return NULL;

    if (offset >= search->total_tracks)
        return search;

    ds->response = buf_new();

    int ret = cmd_search(ds->session, search->query,
                         offset, maxresults,
                         despotify_gzip_callback, ds);
    if (ret) {
        ds->last_error = "cmd_search() failed";
        return NULL;
    }

    /* wait until search is ready */
    if (!despotify_wait_timeout(ds)) {
        ds->last_error = "Timeout when searching";
        return NULL;
    }

    struct buf* b = despotify_inflate(ds->response->ptr, ds->response->len);
    if (b) {
        /* append at end of list */
        struct track *t;
        for (t = search->tracks; t; t = t->next)
            if (!t->next)
                break;

        t = t->next = calloc(1, sizeof(struct track));

        ds->playlist->num_tracks += xml_parse_tracklist(t, b->ptr, b->len,
                                                        false, ds->high_bitrate);
        buf_free(b);
    }

    buf_free(ds->response);

    return search;
}

void despotify_free_search(struct search_result *search) {
    despotify_free_playlist(search->playlist);
    xml_free_album(search->albums);
    xml_free_artist(search->artists);
    free(search);
}

/**************************************************************
 *
 *  Playlists
 *
 */

static bool despotify_load_tracks(struct despotify_session *ds, bool cache_do_store)
{
    struct playlist* pl = ds->playlist;

    if (!pl->num_tracks)
        return true;

    struct track* t = pl->tracks;

    /* construct an array of 16-byte track ids */
    char* tracklist = malloc(MAX_BROWSE_REQ * 16);
    SHA1_CTX sha_ctx;
    unsigned char hash[20], tracks_hash[33];
    int track_count = 0;

    /* don't request too many tracks at once */
    int count;
    for (int totcount=0; totcount < pl->num_tracks; totcount += count) {
        ds->response = buf_new();

        struct track* firsttrack = t;
        for (count = 0; t && count < MAX_BROWSE_REQ; t = t->next, count++)
            hex_ascii_to_bytes(t->track_id, tracklist + count * 16, 16);

        /* create a hash of the tracklist */
        SHA1Init(&sha_ctx);
        SHA1Update(&sha_ctx, tracklist, count * 16);
        SHA1Final(hash, &sha_ctx);

        hex_bytes_to_ascii(hash, tracks_hash, 20);

        /* check cache */
        if (ds->use_cache && cache_contains(tracks_hash)) {
            unsigned char* data;
            int len;

            DSFYDEBUG("Loading cached tracks...\n");

            if ((data = cache_load(tracks_hash, &len)) != NULL) {
                track_count += xml_parse_tracklist(firsttrack, data, len,
                                               true, ds->high_bitrate);
                free(data);

                continue;
            }
        }

        int error = cmd_browse(ds->session, BROWSE_TRACK, tracklist, count, 
                               despotify_gzip_callback, ds);

        if (error) {
            DSFYDEBUG("cmd_browse() failed with %d\n", error);
            ds->last_error = "Network error.";
            session_disconnect(ds->session);
            return false;
        }

        /* wait until track fetch is ready */
        if (!despotify_wait_timeout(ds)) {
            ds->last_error = "Timeout while loading tracks";
            return NULL;
        }

        /* add tracks to playlist */
        struct buf* b = despotify_inflate(ds->response->ptr, ds->response->len);
        if (b) {
            /* store tracks xml in cache. */
            if (cache_do_store)
                cache_store(tracks_hash, b->ptr, b->len);

            track_count += xml_parse_tracklist(firsttrack, b->ptr, b->len,
                                               true, ds->high_bitrate);
            buf_free(b);
        }

        buf_free(ds->response);
    }
    free(tracklist);

    /* tracks that still lack meta data are likely duplicates */
    if (track_count < pl->num_tracks) {
        for (t = pl->tracks; t; t = t->next) {
            if (!t->has_meta_data) {
                struct track* tt;
                for (tt = pl->tracks; tt; tt = tt->next) {
                    if (tt->has_meta_data &&
                        !strncmp(tt->track_id, t->track_id, sizeof tt->track_id)) {
                        struct track* next = t->next;
                        *t = *tt;
                        t->next = next;

                        /* deep copy of artist list */
                        struct artist* a = calloc(1, sizeof(struct artist));
                        t->artist = a;
                        struct artist* ta;
                        for (ta = tt->artist; ta; ta = ta->next) {
                            *a = *ta;
                            if (ta->next)
                                a = a->next = calloc(1, sizeof(struct artist));
                        }
                        t->has_meta_data = true;
                        track_count++;
                        break;
                    }
                }
            }
        }
    }
    pl->num_tracks = track_count;

    return true;
}

struct playlist* despotify_get_playlist(struct despotify_session *ds,
                                        char* playlist_id, bool cache_do_store)
{
    ds->playlist = calloc(1, sizeof(struct playlist));

    /* check cache */
    if (ds->use_cache && playlist_id && cache_contains(playlist_id)) {
        unsigned char* data;
        int len;

        DSFYDEBUG("Loading cached playlist '%s'...\n", playlist_id);

        if ((data = cache_load(playlist_id, &len)) != NULL) {
            ds->playlist = xml_parse_playlist(ds->playlist, data, len, false);
            free(data);

            DSFYstrncpy(ds->playlist->playlist_id, playlist_id, sizeof ds->playlist->playlist_id);

            /* fill the playlist with track info */
            if (!despotify_load_tracks(ds, cache_do_store)) {
                ds->last_error = "Failed loading track info";
                DSFYDEBUG("%s", ds->last_error);
                free(ds->playlist);
                return NULL;
            }

            return ds->playlist;
        }
    }

    ds->response = buf_new();

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

    int error = cmd_getplaylist(ds->session, pid, PLAYLIST_CURRENT,
                                despotify_plain_callback, ds);
    if (error) {
        DSFYDEBUG("Failed getting playlist\n");
        ds->last_error = "Network error.";
        session_disconnect(ds->session);

        return NULL;
    }

    /* wait until playlist fetch is ready */
    if (!despotify_wait_timeout(ds)) {
        ds->last_error = "Timeout while loading playlist";
        return NULL;
    }

    buf_append_u8(ds->response, 0); /* null terminate xml string */

    /* store playlist xml in cache. */
    if (cache_do_store && playlist_id)
        cache_store(playlist_id, ds->response->ptr, ds->response->len);

    ds->playlist = xml_parse_playlist(ds->playlist,
                                      ds->response->ptr,
                                      ds->response->len,
                                      ds->list_of_lists);

    if (! ds->list_of_lists && playlist_id)
        DSFYstrncpy(ds->playlist->playlist_id, playlist_id, sizeof ds->playlist->playlist_id);

    ds->list_of_lists = false;
    buf_free(ds->response);

    if (playlist_id) {
        /* fill the playlist with track info */
        if (!despotify_load_tracks(ds, cache_do_store)) {
            ds->last_error = "Failed loading track info";
            DSFYDEBUG("%s", ds->last_error);
            free(ds->playlist);
            return NULL;
        }
    }

    return ds->playlist;
}

void despotify_free_playlist(struct playlist* p)
{
    xml_free_playlist(p);
}

struct playlist* despotify_get_stored_playlists(struct despotify_session *ds)
{
    /* save cache setting */
    bool old_use_cache = ds->use_cache;

    /* disable caching for meta playlist */
    ds->use_cache = false;

    /* load list of lists */
    DSFYDEBUG("Requesting meta playlist\n");
    struct playlist* metalist = despotify_get_playlist(ds, NULL, false);
    struct playlist* root = NULL;
    struct playlist* prev = NULL;

    /* check meta playlist revision */
    DSFYDEBUG("Meta-Playlist revisions (remote: %d, local: %d)\n",
                        metalist->revision, cache_get_meta_playlist_revision());

    if (!old_use_cache) {
        DSFYDEBUG("Not using cache for playlists.\n");
    }
    else if (metalist->revision > cache_get_meta_playlist_revision()) {
        cache_set_meta_playlist_revision(metalist->revision);

        /* disable caching if something changed */
        ds->use_cache = false;

        DSFYDEBUG("Stored playlists changed! Requesting playlist data...\n");
    }
    else {
        /* reset cache setting to old value */
        ds->use_cache = old_use_cache;

        DSFYDEBUG("Stored playlists didn't change! use_cache = %d\n", ds->use_cache);
    }

    for (struct playlist* p = metalist; p && p->playlist_id[0]; p = p->next) {
        DSFYDEBUG("Requesting playlist with ID '%s'\n", p->playlist_id);
        struct playlist* new = despotify_get_playlist(ds, p->playlist_id, old_use_cache);
        if (prev)
            prev->next = new;
        else
            root = new;
        prev = new;
    }
    xml_free_playlist(metalist);

    /* reset cache setting to old value */
    ds->use_cache = old_use_cache;

    return root;
}

bool despotify_rename_playlist(struct despotify_session *ds,
                               struct playlist *playlist, char *name)
{
    if (strcmp(playlist->author, ds->user_info->username)) {
        ds->last_error = "Not your playlist.";
        return false;
    }

    ds->response = buf_new();
    char xml[512];
    char* nametag = xml_gen_tag("name", name);
    char* usertag = xml_gen_tag("user", ds->user_info->username);
    snprintf(xml, sizeof xml, "<change><ops>%s</ops>"
                              "<time>%u</time>%s</change>"
                              "<version>%010u,%010u,%010u,%u</version>",
                              nametag, (unsigned int)time(NULL),
                              usertag, playlist->revision + 1,
                              playlist->num_tracks, playlist->checksum,
                              playlist->is_collaborative);
    free(nametag);
    free(usertag);

    unsigned char pid[17];
    hex_ascii_to_bytes(playlist->playlist_id, pid, 17);
    int error = cmd_changeplaylist(ds->session, pid, xml, playlist->revision,
                                   playlist->num_tracks, playlist->checksum,
                                   playlist->is_collaborative,
                                   despotify_plain_callback, ds);

    if (error) {
        DSFYDEBUG("Failed renaming playlist\n");
        ds->last_error = "Network error.";
        session_disconnect(ds->session);
        return false;
    }

    /* wait until server responds */
    if (!despotify_wait_timeout(ds)) {
        ds->last_error = "Timeout while renaming playlist";
        return NULL;
    }

    buf_append_u8(ds->response, 0); /* null terminate xml string */

    bool confirm = xml_parse_confirm(playlist, ds->response->ptr, ds->response->len);
    if (confirm) {
        /* success, update local name */
        DSFYstrncpy(playlist->name, name, sizeof playlist->name);
    }
    else {
         ds->last_error = "Failed renaming playlist";
         DSFYDEBUG("%s (response: \"%s\")", ds->last_error, ds->response->ptr);
    }

    buf_free(ds->response);
    return confirm;
}

bool despotify_set_playlist_collaboration(struct despotify_session *ds,
                                          struct playlist *playlist,
                                          bool collaborative)
{
    if (strcmp(playlist->author, ds->user_info->username)) {
        ds->last_error = "Not your playlist.";
        return false;
    }

    ds->response = buf_new();
    char xml[512];
    char* usertag = xml_gen_tag("user", ds->user_info->username);
    snprintf(xml, sizeof xml, "<change><ops><pub>%u</pub></ops>"
                              "<time>%u</time>%s</change>"
                              "<version>%010u,%010u,%010u,%u</version>",
                              collaborative, (unsigned int)time(NULL),
                              usertag, playlist->revision + 1,
                              playlist->num_tracks, playlist->checksum,
                              playlist->is_collaborative);
    free(usertag);

    unsigned char pid[17];
    hex_ascii_to_bytes(playlist->playlist_id, pid, 17);
    int error = cmd_changeplaylist(ds->session, pid, xml, playlist->revision,
                                   playlist->num_tracks, playlist->checksum,
                                   playlist->is_collaborative,
                                   despotify_plain_callback, ds);

    if (error) {
        DSFYDEBUG("Failed setting playlist collaboration\n");
        ds->last_error = "Network error.";
        session_disconnect(ds->session);
        return false;
    }

    /* wait until server responds */
    if (!despotify_wait_timeout(ds)) {
        ds->last_error = "Timeout while setting playlist collaboration";
        return NULL;
    }

    buf_append_u8(ds->response, 0); /* null terminate xml string */

    bool confirm = xml_parse_confirm(playlist, ds->response->ptr, ds->response->len);
    if (confirm) {
        /* success, update local collaboration state */
        playlist->is_collaborative = collaborative;
    }
    else {
        ds->last_error = "Failed setting playlist collabor";
        DSFYDEBUG("%s (response: \"%s\")", ds->last_error, ds->response->ptr);
    }

    buf_free(ds->response);
    return confirm;
}

/*****************************************************************
 *
 *  Artist / album / track information
 *
 */

struct artist_browse* despotify_get_artist(struct despotify_session* ds,
                                           char* artist_id)
{
    ds->artist_browse = calloc(1, sizeof(struct artist_browse));

    /* check cache */
    if (ds->use_cache && cache_contains(artist_id)) {
        unsigned char* data;
        int len;

        DSFYDEBUG("Loading cached artist '%s'...\n", artist_id);

        if ((data = cache_load(artist_id, &len)) != NULL) {
            xml_parse_browse_artist(ds->artist_browse, data, len, ds->high_bitrate);
            free(data);

            return ds->artist_browse;
        }
    }

    ds->response = buf_new();

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

    /* wait until artist fetch is ready */
    if (!despotify_wait_timeout(ds)) {
        ds->last_error = "Timeout while browsing artist";
        return NULL;
    }

    struct buf* b = despotify_inflate(ds->response->ptr, ds->response->len);
    if (b) {
        /* store artist xml in cache. */
        if (ds->use_cache)
            cache_store(artist_id, b->ptr, b->len);

        xml_parse_browse_artist(ds->artist_browse, b->ptr, b->len, ds->high_bitrate);
        buf_free(b);
    }
    buf_free(ds->response);

    return ds->artist_browse;
}

void despotify_free_artist_browse(struct artist_browse* a)
{
    xml_free_artist_browse(a);
}

void* despotify_get_image(struct despotify_session* ds, char* image_id, int* len)
{
    /* check cache */
    if (ds->use_cache && cache_contains(image_id)) {
        DSFYDEBUG("Loading cached image '%s'...\n", image_id);

        return (void *)cache_load(image_id, len);
    }

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
    if (!despotify_wait_timeout(ds)) {
        ds->last_error = "Timeout while loading image";
        return NULL;
    }

    /* store image in cache. */
    if (ds->use_cache)
        cache_store(image_id, ds->response->ptr, ds->response->len);

    void* image = ds->response->ptr;

    if (len)
        *len = ds->response->len;
    free(ds->response); /* free() instead of buf_free() since ptr must
                           remain allocated */
    return image;
}

struct album_browse* despotify_get_album(struct despotify_session* ds,
                                         char* album_id)
{
    ds->album_browse = calloc(1, sizeof(struct album_browse));

    /* check cache */
    if (ds->use_cache && cache_contains(album_id)) {
        unsigned char* data;
        int len;

        DSFYDEBUG("Loading cached album '%s'...\n", album_id);

        if ((data = cache_load(album_id, &len)) != NULL) {
            xml_parse_browse_album(ds->album_browse, data, len, ds->high_bitrate);
            free(data);

            return ds->album_browse;
        }
    }

    ds->response = buf_new();

    unsigned char id[16];
    hex_ascii_to_bytes(album_id, id, sizeof id);
    int error = cmd_browse(ds->session, BROWSE_ALBUM, id, 1,
                           despotify_gzip_callback, ds);

    if (error) {
        DSFYDEBUG("cmd_browse() failed with %d\n", error);
        ds->last_error = "Network error.";
        session_disconnect(ds->session);
        return false;
    }

    /* wait until album fetch is ready */
    if (!despotify_wait_timeout(ds)) {
        ds->last_error = "Timeout while browsing album";
        return NULL;
    }

    struct buf* b = despotify_inflate(ds->response->ptr, ds->response->len);
    if (b) {
        /* store album xml in cache. */
        if (ds->use_cache)
            cache_store(album_id, b->ptr, b->len);

        xml_parse_browse_album(ds->album_browse, b->ptr, b->len, ds->high_bitrate);
        buf_free(b);
    }
    buf_free(ds->response);

    return ds->album_browse;
}

void despotify_free_album_browse(struct album_browse* a)
{
    xml_free_album_browse(a);
}

struct track* despotify_get_tracks(struct despotify_session* ds, char* track_ids[], int num_tracks)
{
    if (num_tracks > MAX_BROWSE_REQ) {
        ds->last_error = "Too many ids in track browse request";
        return NULL;
    }

    /* construct an array of 16-byte track ids */
    char* tracklist = malloc(num_tracks * 16);
    struct track* first = calloc(1, sizeof(struct track));
    ds->response = buf_new();

    for (int i = 0; i < num_tracks; i++)
        hex_ascii_to_bytes(track_ids[i], tracklist + i * 16, 16);

    int error = cmd_browse(ds->session, BROWSE_TRACK, tracklist, num_tracks,
                           despotify_gzip_callback, ds);

    if (error) {
        DSFYDEBUG("cmd_browse() failed with %d\n", error);
        ds->last_error = "Network error.";
        session_disconnect(ds->session);
        return NULL;
    }

    /* wait until track fetch is ready */
    if (!despotify_wait_timeout(ds)) {
        ds->last_error = "Timeout while browsing track(s)";
        return NULL;
    }

    struct buf* b = despotify_inflate(ds->response->ptr, ds->response->len);
    if (b) {
        xml_parse_tracklist(first, b->ptr, b->len, false, ds->high_bitrate);
        buf_free(b);
    }

    buf_free(ds->response);
    free(tracklist);

    return first;
}

struct track* despotify_get_track(struct despotify_session* ds, char* track_id)
{
    char* track_ids[1];
    track_ids[0] = track_id;

    return despotify_get_tracks(ds, track_ids, 1);
}

void despotify_free_track(struct track* t)
{
    xml_free_track(t);
}


/*****************************************************************
 *
 *  Misc functions
 *
 */

static void baseconvert(char *src, char *dest,
                      int frombase, int tobase, int padlen)
{
    static const char alphabet[] =
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+/";
    int number[128];
    int i, len, newlen, divide;

    len = strlen(src);

    for (i = 0; i < len; i++)
        number[i] = strchr(alphabet, src[i]) - alphabet;

    memset(dest, '0', padlen);
    dest[padlen] = 0;

    padlen--;

    do {
        divide = 0;
        newlen = 0;

        for (i = 0; i < len; i++) {
            divide = divide * frombase + number[i];
            if (divide >= tobase) {
                number[newlen++] = divide / tobase;
                divide = divide % tobase;
            } else if (newlen > 0) {
                number[newlen++] = 0;
            }
        }
        len = newlen;

        dest[padlen--] = alphabet[divide];
    } while (newlen != 0);
}

void despotify_id2uri(char* id, char* uri)
{
    baseconvert(id, uri, 16, 62, 22);
}

void despotify_uri2id(char* uri, char* id)
{
    baseconvert(uri, id, 62, 16, 32);
}


struct link* despotify_link_from_uri(char *uri)
{
    struct link* link = calloc(1, sizeof(struct link));

    link->type = LINK_TYPE_INVALID;
    link->uri = uri;

    if (!strncmp(uri, "spotify:album:", 13)) {
        char* id = uri + 14;

        if (strlen(id) != 22) /* id must be 22 chars */
            return link;

        link->type = LINK_TYPE_ALBUM;
        link->arg = id;

    } else if (!strncmp(uri, "spotify:artist:", 14)) {
        char* id = uri + 15;

        if (strlen(id) != 22) /* id must be 22 chars */
            return link;

        link->type = LINK_TYPE_ARTIST;
        link->arg = id;

    } else if (!strncmp(uri, "spotify:search:", 14)) {
        char* searcharg = uri + 15;

        if (strlen(searcharg) == 0) /* must search for something */
            return link;

        link->type = LINK_TYPE_SEARCH;
        link->arg = searcharg;

    } else if (!strncmp(uri, "spotify:user:", 12)) {
        int userlen = strchr(uri + 13, ':') - (uri + 13);
        char *id = uri + userlen + 23;

        if (strlen(id) != 22) /* id must be 22 chars */
            return link;

        link->type = LINK_TYPE_PLAYLIST;
        link->arg = id;

    } else if (!strncmp(uri, "spotify:track:", 13)) {
        char* id = uri + 14;

        if (strlen(id) != 22) /* id must be 22 chars */
            return link;

        link->type = LINK_TYPE_TRACK;
        link->arg = id;
    }

    return link;
}

struct album_browse* despotify_link_get_album(struct despotify_session* ds, struct link* link)
{
    char buf[33];

    despotify_uri2id(link->arg, buf);

    return despotify_get_album(ds, buf);
}

struct artist_browse* despotify_link_get_artist(struct despotify_session* ds, struct link* link)
{
    char buf[33];

    despotify_uri2id(link->arg, buf);

    return despotify_get_artist(ds, buf);
}

struct search_result* despotify_link_get_search(struct despotify_session* ds, struct link* link)
{
    return despotify_search(ds, link->arg, MAX_SEARCH_RESULTS);
}

struct playlist* despotify_link_get_playlist(struct despotify_session* ds, struct link* link)
{
    char buf[35];

    despotify_uri2id(link->arg, buf);
    strncat(buf, "02", 2); /* Playlist uris are missing the last 2 chars of the id */

    /* save cache setting */
    bool old_use_cache = ds->use_cache;

    /* disable caching */
    ds->use_cache = false;

    struct playlist *playlist = despotify_get_playlist(ds, buf, old_use_cache);

    /* reset cache setting to old value */
    ds->use_cache = old_use_cache;

    return playlist;
}

struct track* despotify_link_get_track(struct despotify_session* ds, struct link* link)
{
    char buf[33];

    despotify_uri2id(link->arg, buf);

    return despotify_get_track(ds, buf);
}

void despotify_free_link(struct link* link)
{
    free(link);
}

char* despotify_album_to_uri(struct album_browse* album, char* dest)
{
    char uri[23];

    despotify_id2uri(album->id, uri);
    sprintf(dest, "spotify:album:%s", uri);

    return dest;
}

char* despotify_artist_to_uri(struct artist_browse* artist, char* dest)
{
    char uri[23];

    despotify_id2uri(artist->id, uri);
    sprintf(dest, "spotify:artist:%s", uri);

    return dest;
}

char* despotify_playlist_to_uri(struct playlist* playlist, char* dest)
{
    char uri[23];
    char id[33];

    /* remove two last chars before creating a uri */
    strncpy(id, playlist->playlist_id, 32);
    id[32] = '\0';
    despotify_id2uri(id, uri);

    sprintf(dest, "spotify:user:%s:playlist:%s", playlist->author, uri);

    return dest;
}


char* despotify_search_to_uri(struct search_result* search, char* dest)
{
    sprintf(dest, "spotify:search:%s", search->query);

    return dest;
}


char* despotify_track_to_uri(struct track* track, char* dest)
{
    char uri[23];

    despotify_id2uri(track->track_id, uri);
    sprintf(dest, "spotify:track:%s", uri);

    return dest;
}

int despotify_get_pcm(struct despotify_session* ds, struct pcm_data* pcm)
{
    return snd_get_pcm(ds, pcm);
}

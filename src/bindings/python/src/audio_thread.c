/*
 * A very simple despotify client, to show how the library is used.
 *
 * $Id$
 *
 */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <despotify.h>

#include "audio_thread.h"
#include "audio.h"

static void* thread_loop(void* arg)
{
    struct thread_state *ts = arg;
    struct pcm_data pcm;
    
    pthread_mutex_init(&ts->thread_mutex, NULL);
    pthread_cond_init(&ts->thread_cond, NULL);

    bool loop = true;
    while (loop) {
        switch (ts->state) {
            case PAUSE:
                pthread_mutex_lock(&ts->thread_mutex);
                pthread_cond_wait(&ts->thread_cond, &ts->thread_mutex);
                pthread_mutex_unlock(&ts->thread_mutex);
                break;

            case PLAY: {
                int rc = despotify_get_pcm(ts->session, &pcm);
                if (rc == 0)
                    audio_play_pcm(ts->audio_device, &pcm);
                else {
                    exit(-1);
                }
                break;
            }

            case EXIT:
                loop = false;
                break;
        }
    }

    pthread_cond_destroy(&ts->thread_cond);
    pthread_mutex_destroy(&ts->thread_mutex);

    return NULL;
}

void thread_play(struct thread_state* ts)
{
    pthread_mutex_lock(&ts->thread_mutex);
    ts->state = PLAY;
    pthread_cond_signal(&ts->thread_cond);
    pthread_mutex_unlock(&ts->thread_mutex);
}

void thread_pause(struct thread_state* ts)
{
    pthread_mutex_lock(&ts->thread_mutex);
    ts->state = PAUSE;
    pthread_mutex_unlock(&ts->thread_mutex);
}

void thread_exit(struct thread_state* ts)
{
    pthread_mutex_lock(&ts->thread_mutex);
    ts->state = EXIT;
    pthread_cond_signal(&ts->thread_cond);
    pthread_mutex_unlock(&ts->thread_mutex);

    pthread_join(ts->thread, NULL);
}

struct thread_state* thread_init(struct despotify_session* session)
{
    /* Allocate a new thread state. */
    struct thread_state* ts = malloc(sizeof(struct thread_state));

    /* .. and initialize the values to sane stuff. */
    memset(ts, 0, sizeof(struct thread_state));
    ts->audio_device = audio_init();
    ts->session = session;
    ts->state = PAUSE;

    /* Fire off a thread to play audio! */
    pthread_create(&ts->thread, NULL, &thread_loop, ts);

    return ts;
}

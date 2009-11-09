/*
 * $Id$
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "audio.h"
#include "main.h"
#include "session.h"
#include "ui_footer.h"
#include "ui_log.h"
#include "ui_sidebar.h"

session_t g_session;

static void sess_callback(struct despotify_session *ds, int signal, void *data, void *callback_data);

/****** PCM thread stuff: ***********/
static void* audio_device;
static pthread_t thread;
static pthread_mutex_t thread_mutex;
static pthread_cond_t thread_cond;
static enum {
    PAUSE,
    PLAY,
    EXIT
} play_state = PAUSE;

static void thread_play(void)
{
    pthread_mutex_lock(&thread_mutex);
    play_state = PLAY;
    pthread_cond_signal(&thread_cond);
    pthread_mutex_unlock(&thread_mutex);
}

static void thread_pause(void)
{
    pthread_mutex_lock(&thread_mutex);
    play_state = PAUSE;
    pthread_mutex_unlock(&thread_mutex);
}

static void thread_exit(void)
{
    pthread_mutex_lock(&thread_mutex);
    play_state = EXIT;
    pthread_mutex_unlock(&thread_mutex);
}

static void* thread_loop(void* arg)
{
    struct despotify_session* ds = arg;
    struct pcm_data pcm;

    pthread_mutex_init(&thread_mutex, NULL);
    pthread_cond_init(&thread_cond, NULL);

    bool loop = true;
    while (loop) {
        switch (play_state) {
            case PAUSE:
                pthread_mutex_lock(&thread_mutex);
                log_append("thread_loop(): PAUSE, sleeping");
                pthread_cond_wait(&thread_cond, &thread_mutex);
                log_append("thread_loop(): PAUSE, woke up");
                pthread_mutex_unlock(&thread_mutex);
                break;

            case PLAY: {
                log_append("thread_loop(): PLAY, requesting PCM");
                int rc = despotify_get_pcm(ds, &pcm);
                log_append("thread_loop(): PLAY, returned %d, commencing playback", rc);
                if (rc == 0)
                    audio_play_pcm(audio_device, &pcm);
                else {
		    log_append("despotify_get_pcm() failed");
                    //wrapper_wprintf(L"despotify_get_pcm() returned error %d\n", rc);
		}
                break;
            }

            case EXIT:
                loop = false;
                break;
        }
    }

    log_append("thread_loop() exiting");
    return NULL;
}


void sess_init()
{
  if (!despotify_init())
    panic("despotify_init() failed");

   audio_device = audio_init();
  log_append("Initialized audio output");
}

void sess_cleanup()
{
  sess_disconnect();
  audio_exit(audio_device);
  log_append("Destroyed audio output");

  // Free search results.
  for (sess_search_t *s = g_session.search; s;) {
    despotify_free_search(s->res);
    sess_search_t *p = s;
    s = s->next;
    free(p);
  }

  if (!despotify_cleanup())
    panic("despotify_cleanup() failed");

  free(g_session.username);
  g_session.username = 0;
  free(g_session.password);
  g_session.password = 0;
}

void sess_connect()
{
  assert(g_session.username && g_session.password);

  sess_disconnect();

  if (!(g_session.dsfy = despotify_init_client(sess_callback, NULL, true)))
    panic("despotify_init_client(...) failed");

  play_state = PAUSE;
  pthread_create(&thread, NULL, &thread_loop, g_session.dsfy);

  // Login with credentials set by sess_username/sess_password.
  if (!despotify_authenticate(g_session.dsfy, g_session.username, g_session.password)) {
    g_session.state = SESS_ERROR;
    log_append(despotify_get_error(g_session.dsfy));
    despotify_exit(g_session.dsfy);
    // Switch to log view.
    ui_show(UI_SET_LOG);
  }
  else {
    g_session.state = SESS_ONLINE;
    log_append("Logged in as %s@%s:%d (%s)",
        g_session.dsfy->user_info->username,
        g_session.dsfy->user_info->server_host,
        g_session.dsfy->user_info->server_port,
        g_session.dsfy->user_info->country);
    // Switch to browser view.
    ui_show(UI_SET_BROWSER);
  }

  // Redraw status info.
  ui_dirty(UI_FOOTER);
}

void sess_disconnect()
{
  thread_exit();

  if (g_session.state == SESS_ONLINE) {
    sess_stop();
    despotify_exit(g_session.dsfy);
    log_append("Disconnected");
    // Return to splash screen.
    ui_show(UI_SET_SPLASH);
  }

  g_session.state = SESS_OFFLINE;
  // Redraw status info.
  ui_dirty(UI_FOOTER);
}

void sess_username(const char *username)
{
  free(g_session.username);
  g_session.username = strdup(username);

  // Prompt for password.
  footer_input(INPUT_PASSWORD);
}

void sess_password(const char *password)
{
  free(g_session.password);
  g_session.password = strdup(password);

  sess_connect();
}

void sess_search(const char *query)
{
  if (g_session.state != SESS_ONLINE) {
    log_append("Not connected");
    return;
  }

  log_append("Searching for: <%s>", query);
  struct search_result *sr = despotify_search(g_session.dsfy, (char*)query, 100);

  if (!sr) {
    log_append(despotify_get_error(g_session.dsfy));
    return;
  }

  log_append("Got %d/%d tracks", sr->playlist->num_tracks, sr->total_tracks);

  sess_search_t *prev = g_session.search;
  sess_search_t *search = malloc(sizeof(sess_search_t));
  search->res  = sr;
  search->next = prev;
  g_session.search = search;
  ++g_session.search_len;

  sidebar_reset();
}

// Start playback.
void sess_play(struct track *t)
{
  if (g_session.state != SESS_ONLINE) {
    log_append("Not connected");
    return;
  }

  if (despotify_play(g_session.dsfy, t, true)) {
    g_session.playing = true;
    g_session.paused = false;
    log_append("Playing %s", t->title);
    thread_play();
  }
  else
    log_append("Playback failed");
}

// Stop playback.
void sess_stop()
{
  thread_pause();
  if (g_session.state != SESS_ONLINE)
    return;

  g_session.playing = false;
  despotify_stop(g_session.dsfy);
  log_append("Stopped playback");
}

// Toggle playback pause.
void sess_pause()
{
  if (!g_session.playing) {
    log_append("Not pausing since nothing is playing");
    return;
  }

  if (g_session.paused) {
    thread_play();
    log_append("Resumed playback");
    g_session.paused = false;
  }
  else {
    thread_pause();
    log_append("Paused playback");
    g_session.paused = true;
  }
}

static void sess_callback(struct despotify_session* ds, int signal, void *data, void* callback_data)
{
  (void)data;
  (void)ds;
  (void)callback_data;

  switch (signal) {
    case DESPOTIFY_NEW_TRACK:
      thread_play();
      break;
  }
}

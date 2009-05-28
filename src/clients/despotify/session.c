/*
 * $Id$
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "session.h"
#include "ui_footer.h"
#include "ui_log.h"
#include "ui_sidebar.h"

session_t g_session;

static void sess_callback(int signal, void *data);

void sess_init()
{
  if (!despotify_init())
    panic("despotify_init() failed");
}

void sess_cleanup()
{
  sess_disconnect();

  // Free search results.
  for (sess_search_t *s = g_session.search; s;) {
    despotify_free_search(s->res);
    sess_search_t *p = s;
    s = s->next;
    free(p);
  }

  if (!despotify_cleanup())
    panic("despotify_cleanup() failed");
}

void sess_connect()
{
  assert(g_session.username && g_session.password);

  sess_disconnect();

  if (!(g_session.dsfy = despotify_init_client(sess_callback)))
    panic("despotify_init_client(...) failed");

  // Login with credentials set by sess_username/sess_password.
  if (!despotify_authenticate(g_session.dsfy, g_session.username, g_session.password)) {
    g_session.state = SESS_ERROR;
    log_append(despotify_get_error(g_session.dsfy));
    despotify_exit(g_session.dsfy);
  }
  else {
    g_session.state = SESS_ONLINE;
    log_append("Logged in as %s@%s:%d (%s)",
        g_session.dsfy->user_info->username,
        g_session.dsfy->user_info->server_host,
        g_session.dsfy->user_info->server_port,
        g_session.dsfy->user_info->country);
  }

  // Redraw status info.
  ui_dirty(UI_FOOTER);
}

void sess_disconnect()
{
  if (g_session.state == SESS_ONLINE) {
    despotify_stop(g_session.dsfy);
    despotify_exit(g_session.dsfy);
    log_append("Disconnected");
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

  log_append("Searching for: \"%s\"", query);
  struct search_result *sr = despotify_search(g_session.dsfy, (char*)query, 20);
  log_append("Got %d/%d tracks", sr->playlist->num_tracks, sr->total_tracks);

  sess_search_t *prev = g_session.search;
  sess_search_t *search = malloc(sizeof(sess_search_t));
  search->res  = sr;
  search->next = prev;
  g_session.search = search;
  ++g_session.search_len;

  sidebar_reset();
}

static void sess_callback(int signal, void *data)
{
  (void)data;

  switch (signal) {
    case DESPOTIFY_TRACK_CHANGE:
      break;
  }
}

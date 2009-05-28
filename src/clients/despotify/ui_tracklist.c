/*
 * $Id$
 *
 */

#include "event.h"
#include "session.h"
#include "ui_log.h"
#include "ui_tracklist.h"

extern session_t g_session;

// Cursor position.
static unsigned int g_pos = ~0;

// Current search result.
static struct search_result *g_res = 0;

// Print tracks in search result.
void tracklist_draw(ui_t *ui)
{
  if (!g_res)
    return;

  wattron(ui->win, A_UNDERLINE);
  mvwprintw(ui->win, 0, 0, "%3s %-40.40s %-40.40s", "#", "Title", "Artist");
  wattroff(ui->win, A_UNDERLINE);

  // TODO: Scrolling.
  unsigned int line = 0;
  for (struct track *t = g_res->playlist->tracks; t && line < (ui->height - 2); t = t->next) {
    mvwprintw(ui->win, line + 1, 0, "%3d %-40.40s %-40.40s", line + 1, t->title, t->artist->name);

    if (line == g_pos)
      mvwchgat(ui->win, line + 1, 0, -1, (ui->flags & UI_FLAG_FOCUS ? A_REVERSE : A_BOLD), COLOR_PAIR(0), NULL);
    ++line;
  }

  // Additional info at bottom.
  wattron(ui->win, A_DIM);
  mvwprintw(ui->win, ui->height - 1, 0, "Query: '%s' Hits: %d/%d",
      g_res->query,
      g_res->playlist->num_tracks,
      g_res->total_tracks);
  wattroff(ui->win, A_DIM);
}

int tracklist_keypress(int ch)
{
  struct track *t;

  switch (ch) {
    case KEY_LEFT:
    case 'h':
      ui_focus(UI_SIDEBAR);
      break;

    case KEY_ENTER:
    case '\n':
    case '\r':
      // Find track and play.
      //for (struct track *t = g_res->playlist->tracks; t; t = t->next)
      t = g_res->playlist->tracks;
      for (unsigned int i = 0; t && i < g_pos; ++i, t = t->next);
      if (t) {
        log_append("Playing %s", t->title);
        despotify_play(g_session.dsfy, t, true);
      }
      else
        log_append("Can't play track at pos %d", g_pos);
      break;

    case KEY_UP:
    case 'k':
      g_pos = DSFY_MAX((signed)g_pos - 1, 0);
      break;

    case KEY_DOWN:
    case 'j':
      g_pos = DSFY_MIN((signed)g_pos + 1, g_res->playlist->num_tracks - 1);
      break;

    case KEY_HOME:
      g_pos = 0;
      break;

    case KEY_END:
      g_pos = g_res->playlist->num_tracks - 1;
      break;

    default:
      return ch;
  }

  ui_dirty(UI_TRACKLIST);
  event_msg_post(MSG_CLASS_APP, MSG_APP_UPDATE, NULL);

  return 0;
}

// Set search result for displaying.
void tracklist_set(int pos, bool focus)
{
  // Find search.
  sess_search_t *s = g_session.search;
  for (int i = 0; s && i != pos; ++i, s = s->next);

  if (g_res != s->res) {
    g_res = s->res;
    g_pos = g_res->playlist->num_tracks ? 0 : ~0;
  }

  if (focus)
    ui_focus(UI_TRACKLIST);
  else
    ui_dirty(UI_TRACKLIST);

  event_msg_post(MSG_CLASS_APP, MSG_APP_UPDATE, NULL);
}

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
static int g_pos = 0;

// Available height for list (needed for PGDN/PGUP keys).
static int g_availy = 10;

// Current search result.
static struct search_result *g_res = 0;

// Print tracks in search result.
void tracklist_draw(ui_t *ui)
{
  if (!g_res)
    return;

  int i = 0, line = 0;
  g_availy = ui->height - 2;

  // Title/artist columns width.
  int slen = (ui->width - 4 - 6 - 1) / 2;

  mvwprintw(ui->win, 0, 0, "%3s %-*.*s %-*.*sLength", "#",
      slen, slen, "Title", slen, slen, "Artist");
  mvwchgat(ui->win, 0, 0, -1, A_UNDERLINE, COLOR_PAIR(0), NULL);

  if (g_res->playlist->num_tracks) {
    struct track *t = g_res->playlist->tracks;

    // Scroll offset.
    int offset = DSFY_MIN(DSFY_MAX(g_pos - (g_availy / 2), 0),
        g_res->playlist->num_tracks - g_availy);

    // Find first track to be displayed in list.
    if (offset) for (; t && i < offset; ++i, t = t->next);

    for (; t && line < g_availy; t = t->next) {
      // Concat list of artists.
      wchar_t art[slen];
      int len = 0;
      for (struct artist* a = t->artist; a && len < slen; a = a->next)
        len += swprintf(art + len, slen - len, L"%s%s", a->name, a->next ? "/" : "");

      wchar_t str[ui->width];
      swprintf(str, sizeof(str), L"%3d %-*.*s %-*.*ls %2d:%02d",
          i + line + 1, slen, slen, t->title, slen, slen, art,
          t->length / 60000, t->length % 60000 / 1000);
      mvwaddnwstr(ui->win, line + 1, 0, str, ui->width);

      if (i + line == g_pos)
        mvwchgat(ui->win, line + 1, 0, -1,
            (ui->flags & UI_FLAG_FOCUS ? A_REVERSE : A_BOLD),
            COLOR_PAIR(0), NULL);

      ++line;
    }
  }

  // Combine bottom line with possible attributes from last track in list.
  mvwchgat(ui->win, g_availy, 0, -1,
      A_UNDERLINE | (line == g_availy && (i + line - 1 == g_pos) ?
        (ui->flags & UI_FLAG_FOCUS ? A_REVERSE : A_BOLD) : 0),
      COLOR_PAIR(0), NULL);

  // Additional info at bottom.
  wattron(ui->win, A_DIM);
  mvwprintw(ui->win, ui->height - 1, 0, "Query: <%s> Hits: %d/%d",
      g_res->query,
      g_res->playlist->num_tracks,
      g_res->total_tracks);
  wattroff(ui->win, A_DIM);
}

int tracklist_keypress(wint_t ch, bool code)
{
  (void)code;

  // Change focus back to sidebar.
  if (ch == KEY_LEFT || ch == 'h') {
    ui_focus(UI_SIDEBAR);
    return 0;
  }

  // Nothing left to do if we've got an empty list.
  if (!g_res || !g_res->playlist->num_tracks)
    return ch;

  struct track *t;

  switch (ch) {
    case KEY_ENTER:
    case '\n':
    case '\r':
      // Find track and play.
      t = g_res->playlist->tracks;
      for (int i = 0; t && i < g_pos; ++i, t = t->next);
      if (t)
        sess_play(t);
      else
        log_append("Can't play track at pos %d", g_pos);
      break;

    case KEY_UP:
    case 'k':
      g_pos = DSFY_MAX(g_pos - 1, 0);
      break;

    case KEY_DOWN:
    case 'j':
      g_pos = DSFY_MIN(g_pos + 1, g_res->playlist->num_tracks - 1);
      break;

    case KEY_HOME:
      g_pos = 0;
      break;

    case KEY_END:
      g_pos = g_res->playlist->num_tracks - 1;
      break;

    case KEY_PPAGE:
      g_pos = DSFY_MAX(g_pos - g_availy, 0);
      break;

    case KEY_NPAGE:
      g_pos = DSFY_MIN(g_pos + g_availy, g_res->playlist->num_tracks - 1);
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
    g_pos = 0;
  }

  if (focus)
    ui_focus(UI_TRACKLIST);
  else
    ui_dirty(UI_TRACKLIST);

  event_msg_post(MSG_CLASS_APP, MSG_APP_UPDATE, NULL);
}

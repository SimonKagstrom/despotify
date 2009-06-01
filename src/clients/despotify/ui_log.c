/*
 * $Id$
 *
 */

#include <stdlib.h>
#include <time.h>

#include "event.h"
#include "ui_log.h"

#define MSG_MAX 128

typedef struct entry {
  int           priority;
  time_t        time;
  char         *msg;
  struct entry *prev;
} entry_t;

static entry_t *g_last_entry = 0;

void log_init(ui_t *ui)
{
  ui->win          = newwin(0, 0, 0, 0);
  ui->flags        = 0;
  ui->set          = UI_SET_LOG;
  ui->fixed_width  = 0;
  ui->fixed_height = 0;
  ui->draw_cb      = log_draw;
  ui->keypress_cb  = log_keypress;
}

void log_draw(ui_t *ui)
{
  unsigned int i;

  // Draw as many entries as we can fit inside the window.
  entry_t *e = g_last_entry;
  for (i = 1; i < ui->height && e; ++i, e = e->prev) {
    struct tm *t = localtime(&e->time);
    mvwprintw(ui->win, ui->height - i, 0, "(%02d:%02d:%02d) %s",
        t->tm_hour, t->tm_min, t->tm_sec, e->msg);
  }
}

int log_keypress(wint_t ch, bool code)
{
  (void)code;

  // TODO: Scrolling...
  switch (ch) {
    case KEY_LEFT:
    case KEY_RIGHT:
    case KEY_ESC:
    case 'D' - '@':
      ui_show(UI_SET_BROWSER);
      return 0;

    default:
      return ch;
  }
}

void log_append(const char *fmt, ...)
{
  char *msg = malloc(MSG_MAX);
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(msg, MSG_MAX, fmt, ap);
  va_end(ap);

  entry_t *entry = malloc(sizeof(entry_t));

  time(&entry->time);
  entry->priority = 0;
  entry->msg      = msg;
  entry->prev     = g_last_entry ? g_last_entry : 0;

  g_last_entry = entry;

  ui_dirty(UI_LOG);
  event_msg_post(MSG_CLASS_APP, MSG_APP_UPDATE, NULL);
}

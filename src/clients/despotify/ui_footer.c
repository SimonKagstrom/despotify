/*
 * $Id$
 *
 */

#include <string.h>

#include "commands.h"
#include "event.h"
#include "session.h"
#include "ui_footer.h"

#define BUF_MAX 256

typedef struct input {
  input_type_t type;
  char         buf[BUF_MAX];
  unsigned int pos;
  unsigned int len;
  // TODO: History.
  ui_elem_t    prev_focus;
} input_t;

static input_t g_input = { .type = INPUT_NONE };
static char *g_titles[INPUT_END] = { 0, "command", "search", "username", "password" };

extern session_t g_session;

void footer_draw(ui_t *ui)
{
  if (g_input.type == INPUT_NONE) {
    switch (g_session.state) {
      case SESS_OFFLINE:
        mvwprintw(ui->win, 0, 0, "OFFLINE");
        break;

      case SESS_ONLINE:
        mvwprintw(ui->win, 0, 0, "ONLINE %s", g_session.dsfy->user_info->server_host);
        break;

      case SESS_ERROR:
         mvwprintw(ui->win, 0, 0, "ERROR (see log for details)");
        break;
    }
  }
  else {
    char *buf;
    int curx = strlen(g_titles[g_input.type]) + 2;

    // Don't echo password input.
    if (g_input.type == INPUT_PASSWORD) {
      buf = "";
    }
    else  {
      buf = g_input.buf;
      curx += g_input.pos;
    }

    // TODO: Scroll text when cursor exceeds screen width.
    mvwprintw(ui->win, 0, 0, "%s> %s", g_titles[g_input.type], buf);

    // Draw fake cursor.
    mvwchgat(ui->win, 0, curx, 1, A_REVERSE, COLOR_PAIR(0), NULL);
  }
}

int footer_keypress(int ch)
{
  if (g_input.type == INPUT_NONE)
    return ch;

  // TODO: Wide-char support.
  if (ch >= 0x20 && ch <= 0x7E) {
    if (g_input.len < (BUF_MAX - 1)) {
      memmove(g_input.buf + g_input.pos + 1, g_input.buf + g_input.pos, g_input.len + 1 - g_input.pos);
      g_input.buf[g_input.pos] = ch;
      ++g_input.len;
      ++g_input.pos;
    }
  }
  else {
    switch (ch) {
      case 'H' - '@':
      case KEY_BACKSPACE:
        if (g_input.pos) {
          memmove(g_input.buf + g_input.pos - 1, g_input.buf + g_input.pos, g_input.len + 1 - g_input.pos);
          --g_input.len;
          --g_input.pos;
        }
        break;

      case KEY_DC:
        if (g_input.pos != g_input.len) {
          memmove(g_input.buf + g_input.pos, g_input.buf + g_input.pos + 1, g_input.len + 1 - g_input.pos);
          --g_input.len;
        }
        break;

      case KEY_LEFT:
        if (g_input.pos)
          --g_input.pos;
        break;

      case KEY_RIGHT:
        if (g_input.pos < g_input.len)
          ++g_input.pos;
        break;

      case 'A' - '@':
      case KEY_HOME:
        g_input.pos = 0;
        break;

      case 'E' - '@':
      case KEY_END:
        g_input.pos = g_input.len;
        break;

      case 'U' - '@':
        g_input.len -= g_input.pos;
        memmove(g_input.buf, g_input.buf + g_input.pos, g_input.len + 1);
        g_input.pos = 0;
        break;

      case 'K' - '@':
        g_input.buf[g_input.pos] = 0;
        g_input.len = g_input.pos;
        break;

      // Process input.
      case '\n':
      case '\r':
      case KEY_ENTER:
        footer_input(INPUT_END);
        return 0;

      // Cancel input.
      case 'D' - '@':
      case KEY_ESC:
        footer_input(INPUT_NONE);
        return 0;

      default:
        return ch;
    }
  }

  ui_dirty(UI_FOOTER);
  event_msg_post(MSG_CLASS_APP, MSG_APP_UPDATE, NULL);

  return 0;
}

void footer_input(input_type_t type)
{
  input_type_t cur_type;

  switch (type) {
    case INPUT_COMMAND:
    case INPUT_SEARCH:
    case INPUT_USERNAME:
    case INPUT_PASSWORD:
      g_input.type = type;
      g_input.buf[0] = g_input.len = g_input.pos = 0;
      g_input.prev_focus = ui_focused();
      ui_focus(UI_FOOTER);
      break;

    // Process input.
    case INPUT_END:
      cur_type = g_input.type;
      g_input.type = INPUT_NONE;
      ui_focus(g_input.prev_focus);
      ui_dirty(UI_FOOTER);

      switch (cur_type) {
        case INPUT_COMMAND:
          command_process(g_input.buf);
          break;

        case INPUT_SEARCH:
          sess_search(g_input.buf);
          break;

        case INPUT_USERNAME:
          sess_username(g_input.buf);
          break;

        case INPUT_PASSWORD:
          sess_password(g_input.buf);
          break;

        case INPUT_NONE:
        case INPUT_END:
          break;
      }
      break;

    // Cancel input.
    case INPUT_NONE:
      g_input.type = INPUT_NONE;
      ui_focus(g_input.prev_focus);
      ui_dirty(UI_FOOTER);
      break;
  }
}

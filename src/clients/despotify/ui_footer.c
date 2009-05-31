/*
 * $Id$
 *
 */

#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "event.h"
#include "session.h"
#include "ui_footer.h"

#define BUF_MAX 128

typedef struct input {
  input_type_t type;
  wchar_t      wbuf[BUF_MAX];
  char         cbuf[BUF_MAX * sizeof(wchar_t)];
  unsigned int pos;
  unsigned int len;
  // TODO: History.
  ui_elem_t    prev_focus;
} input_t;

static input_t g_input = { .type = INPUT_NONE };
static char *g_titles[INPUT_END] = { 0, "command", "search", "username", "password" };

extern session_t g_session;

void footer_init(ui_t *ui)
{
  ui->win          = newwin(0, 0, 0, 0);
  ui->flags        = 0;
  ui->set          = UI_SET_NONE;
  ui->fixed_width  = 0;
  ui->fixed_height = 1;
  ui->draw_cb      = footer_draw;
  ui->keypress_cb  = footer_keypress;
}

void footer_draw(ui_t *ui)
{
  // Print status info by default.
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
  // Draw input field.
  else {
    wchar_t *wbuf;
    // Cursor position.
    unsigned int curx = strlen(g_titles[g_input.type]) + 2;
    // Available screen space for input string.
    unsigned int avail = DSFY_MAX(ui->width - curx - 1, 0);
    // Offset for keeping cursor on-screen.
    unsigned int offset = g_input.pos > avail ? g_input.pos - avail : 0;

    // Don't echo password input.
    if (g_input.type == INPUT_PASSWORD) {
      wbuf = L"";
    }
    else  {
      wbuf = g_input.wbuf;
      curx = DSFY_MIN(curx + g_input.pos, curx + avail);
    }

    wchar_t str[ui->width];
    swprintf(str, sizeof(str), L"%s> %.*ls", g_titles[g_input.type], avail, wbuf + offset);
    mvwaddwstr(ui->win, 0, 0, str);

    // Draw fake cursor.
    mvwchgat(ui->win, 0, curx, 1, A_REVERSE, COLOR_PAIR(0), NULL);
  }
}

int footer_keypress(wint_t ch, bool code)
{
  if (g_input.type == INPUT_NONE)
    return ch;

  switch (ch) {
    case 'H' - '@':
    case KEY_BACKSPACE:
      if (g_input.pos) {
        memmove(g_input.wbuf + g_input.pos - 1,
                g_input.wbuf + g_input.pos,
                (g_input.len + 1 - g_input.pos) * sizeof(wchar_t));
        --g_input.len;
        --g_input.pos;
      }
      break;

    case KEY_DC:
      if (g_input.pos != g_input.len) {
        memmove(g_input.wbuf + g_input.pos,
                g_input.wbuf + g_input.pos + 1,
                (g_input.len + 1 - g_input.pos) * sizeof(wchar_t));
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
      memmove(g_input.wbuf, g_input.wbuf + g_input.pos,
              (g_input.len + 1) * sizeof(wchar_t));
      g_input.pos = 0;
      break;

    case 'K' - '@':
      g_input.wbuf[g_input.pos] = 0;
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
      if (!code && ch >= ' ') {
        if (g_input.len < (BUF_MAX - 1)) {
          memmove(g_input.wbuf + g_input.pos + 1,
                  g_input.wbuf + g_input.pos,
                  (g_input.len + 1 - g_input.pos) * sizeof(wchar_t));
          g_input.wbuf[g_input.pos] = ch;
          ++g_input.len;
          ++g_input.pos;
        }
      }
      else
        return ch;
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
      g_input.wbuf[0] = g_input.len = g_input.pos = 0;
      g_input.prev_focus = ui_focused();
      ui_focus(UI_FOOTER);
      break;

    // Process input.
    case INPUT_END:
      cur_type = g_input.type;
      g_input.type = INPUT_NONE;
      ui_focus(g_input.prev_focus);
      ui_dirty(UI_FOOTER);

      // FIXME: Convert to UTF-8, don't rely on env locale.
      wcstombs(g_input.cbuf, g_input.wbuf, sizeof(g_input.cbuf));

      switch (cur_type) {
        case INPUT_COMMAND:
          command_process(g_input.cbuf);
          break;

        case INPUT_SEARCH:
          sess_search(g_input.cbuf);
          break;

        case INPUT_USERNAME:
          sess_username(g_input.cbuf);
          break;

        case INPUT_PASSWORD:
          sess_password(g_input.cbuf);
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

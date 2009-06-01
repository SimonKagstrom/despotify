/*
 * $Id$
 *
 */

#include "ui_splash.h"

#define LOGO_HEIGHT  8
#define LOGO_WIDTH  61

static const char g_clogo[LOGO_HEIGHT][LOGO_WIDTH] = {
  "     88                                  ,8  db  ,88        ",
  "  __ 88   ___     ___      __     ___    88  `'  88         ",
  ",888b88 ,88888. ,88888. 88d888. ,88888. 8888 88 8888 88   88",
  "88' `88 88' `88 `8'     88' `88 88' `88  88  88  88  88   88",
  "88. ,88 88.~~~   ~~~,8. 88. ,88 88. ,88  88  88  88  88. ,88",
  "`888P88 `88888' `88888' 88q888' `88888'  q8. 88  88  `888P88",
  "  ~~      ~~~     ~~~   88 ~~     ~~~     ~            ~~,88",
  "88888888888888888888888 88 8888888            8888888888888'"
};

static wchar_t g_wlogo[LOGO_HEIGHT][LOGO_WIDTH];

void splash_init(ui_t *ui)
{
  int i, j;

  ui->win          = newwin(0, 0, 0, 0);
  ui->flags        = 0;
  ui->set          = UI_SET_SPLASH;
  ui->fixed_width  = 0;
  ui->fixed_height = 0;
  ui->draw_cb      = splash_draw;
  ui->keypress_cb  = splash_keypress;

  // Generate unicode logo from ASCII source.
  for (i = 0; i < LOGO_HEIGHT; ++i) {
    for (j = 0; j < LOGO_WIDTH; ++j) {
      switch (g_clogo[i][j]) {
        case '8':  g_wlogo[i][j] = L'\u2588'; break; // '█'
        case ',':  g_wlogo[i][j] = L'\u2597'; break; // '▗'
        case '.':  g_wlogo[i][j] = L'\u2596'; break; // '▖'
        case '`':  g_wlogo[i][j] = L'\u259D'; break; // '▝'
        case '\'': g_wlogo[i][j] = L'\u2598'; break; // '▘'
        case '~':  g_wlogo[i][j] = L'\u2594'; break; // '▔'
        case '_':  g_wlogo[i][j] = L'\u2581'; break; // '▁'
        case 'b':  g_wlogo[i][j] = L'\u2599'; break; // '▙'
        case 'P':  g_wlogo[i][j] = L'\u259B'; break; // '▛'
        case 'd':  g_wlogo[i][j] = L'\u259F'; break; // '▟'
        case 'q':  g_wlogo[i][j] = L'\u259C'; break; // '▜'
        default:   g_wlogo[i][j] = g_clogo[i][j]; break;
      }
    }
  }
}

void splash_draw(ui_t *ui)
{
  int i;
  int line = DSFY_MAX((ui->height - LOGO_HEIGHT * 2) / 2, 0);

  int x = DSFY_MAX((ui->width - LOGO_WIDTH) / 2, 0);

  wattron(ui->win, COLOR_PAIR(UI_STYLE_DIM));
  for (i = 0; i < LOGO_HEIGHT; ++i) {
    mvwaddnwstr(ui->win, ++line, x, g_wlogo[i], ui->width - x);
  }
  wattroff(ui->win, COLOR_PAIR(UI_STYLE_DIM));

  mvwprintw(ui->win, line, x + 34, "despotify.se");
  mvwchgat(ui->win, line, x + 34, 12, A_REVERSE, UI_STYLE_DIM, NULL);

  line += 2;

  mvwprintw(ui->win, ++line, x, "Type ':connect' to login, then press '/' to search for hits.");
  mvwprintw(ui->win, ++line, x + 3, "Press F1 for a list of all commands and key bindings.");

  line += 2;
  x += 18;

  mvwprintw(ui->win, ++line, x, "Web: http://despotify.se/");
  mvwprintw(ui->win, ++line, x, "IRC: #despotify @ EFNet");
}

int splash_keypress(wint_t ch, bool code)
{
  (void)code;
  return ch;
}

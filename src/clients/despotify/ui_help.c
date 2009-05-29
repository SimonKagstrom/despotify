/*
 * $Id$
 *
 */

#include "ui_help.h"

void help_draw(ui_t *ui)
{
  // TODO: Print arguments.
  unsigned int line = 0;
  wattron(ui->win, A_BOLD);
  mvwprintw(ui->win,   line, 2, "Key     Command         Description");
  wattroff(ui->win, A_BOLD);

  mvwprintw(ui->win, ++line, 2, ":                        Open command input");
  mvwprintw(ui->win, ++line, 2, "^D                       Cancel input");
  ++line;
  mvwprintw(ui->win, ++line, 2, "c          c/connect     (Re)connect to Spotify");
  mvwprintw(ui->win, ++line, 2, "d          d/disconnect  Disconnect from Spotify");
  mvwprintw(ui->win, ++line, 2, "/          s/search      Search");
  ++line;
  mvwprintw(ui->win, ++line, 2, "Enter                    Play highlighted track");
  mvwprintw(ui->win, ++line, 2, "Backspace  t/stop        Stop playback");
  mvwprintw(ui->win, ++line, 2, "Space      p/pause       Toggle playback pause");
  ++line;
  mvwprintw(ui->win, ++line, 2, "|          l/log         Display log");
  mvwprintw(ui->win, ++line, 2, "F1         ?/h/help      Display this text");
  mvwprintw(ui->win, ++line, 2, "Esc        m/main        Return to main screen");
  ++line;
  mvwprintw(ui->win, ++line, 2, "^L         r/redraw      Force screen redraw");
  mvwprintw(ui->win, ++line, 2, "^Q         q/quit        Quit");
}

int help_keypress(wint_t ch, bool code)
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

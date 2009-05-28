/*
 * $Id$
 *
 */

#include "ui_help.h"

void help_draw(ui_t *ui)
{
  // TODO: Print arguments.
  wattron(ui->win, A_BOLD);
  mvwprintw(ui->win,  0, 2, "Key     Command       Description");
  wattroff(ui->win, A_BOLD);

  mvwprintw(ui->win,  1, 2, ":                     Open command input");
  mvwprintw(ui->win,  2, 2, "^D                    Cancel input");

  mvwprintw(ui->win,  4, 2, "c       c/connect     (Re)connect to Spotify");
  mvwprintw(ui->win,  5, 2, "d       d/disconnect  Disconnect from Spotify");
  mvwprintw(ui->win,  6, 2, "/       s/search      Search");

  mvwprintw(ui->win,  8, 2, "|       l/log         Display log");
  mvwprintw(ui->win,  9, 2, "F1      ?/h/help      Display this text");
  mvwprintw(ui->win, 10, 2, "Space   m/main        Return to main screen");

  mvwprintw(ui->win, 12, 2, "^L      r/redraw      Force screen redraw");
  mvwprintw(ui->win, 13, 2, "^Q      q/quit        Quit");
}

int help_keypress(int ch)
{
  // TODO: Scrolling...
  if (ch == ' ') {
    ui_show(UI_SET_BROWSER);
    return 0;
  }

  return ch;
}

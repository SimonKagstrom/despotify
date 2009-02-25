/*
 * $Id: ui-help.c 755 2009-02-24 02:18:18Z x $
 *
 */

#include <curses.h>

#include "ui.h"
#include "ui-help.h"

int gui_help(WINDOW *w, char *helpstring) {
	werase(w);
	box(w, 0, 0);

	mvwprintw(w, 1, 2, "available commands: list, play, pause, stop, search, help\n");

	wattron(w, A_BOLD);
	mvwprintw(w, 3, 2, "list [number]");
	wattroff(w, A_BOLD);
	mvwprintw(w, 3, 20, "Show all playlist, or select playlist 'number'.");

	wattron(w, A_BOLD);
	mvwprintw(w, 4, 2, "play [number]");
	wattroff(w, A_BOLD);
	mvwprintw(w, 4, 20, "Play first track, or track 'number' in current playlist");

	wattron(w, A_BOLD);
	mvwprintw(w, 5, 2, "pause");
	wattroff(w, A_BOLD);
	mvwprintw(w, 5, 20, "Pause current playing track");
	
	wattron(w, A_BOLD);
	mvwprintw(w, 6, 2, "stop");
	wattroff(w, A_BOLD);
	mvwprintw(w, 6, 20, "Stop current playing track");

	wattron(w, A_BOLD);
	mvwprintw(w, 7, 2, "search <string>");
	wattroff(w, A_BOLD);
	mvwprintw(w, 7, 20, "Search tracks");

	wrefresh(w);

	return 0;
}

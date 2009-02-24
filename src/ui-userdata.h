/*
 * $Id: ui-userdata.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_UI_USERDATA_H
#define DESPOTIFY_UI_USERDATA_H

#include <curses.h>

#include "session.h"
#include "xml.h"


struct userdata {
	char *birthdate;
	char *city;
	char *country;
	char *gender;
	int deleted;
	int disabled;
	char *email;
	int sendemail;
	char *postal_code;
	char *username;
};

struct gui_userdata_state {
	/* the last track in the list */
	struct userdata *userdata;

	GZXML *g;
	WINDOW *win;
	struct tagstack *taglist;
};


int gui_userdata(SESSION *, WINDOW *, char *);
#endif

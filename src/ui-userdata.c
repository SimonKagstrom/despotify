/*
 * $Id: ui-userdata.c 383 2009-01-22 15:36:17Z x $
 *
 */

#include <string.h>
#include <netinet/in.h>
#include <assert.h>

#include "channel.h"
#include "commands.h"
#include "ui.h"
#include "ui-userdata.h"
#include "xml.h"


static int gui_userdata_result_callback(CHANNEL *, unsigned char *, unsigned short);
static void gui_userdata_xml_handle_startelement(void *, const XML_Char *, const XML_Char **);
static void gui_userdata_xml_handle_endelement(void *, const XML_Char *);
static void gui_userdata_xml_handle_text(void *, const XML_Char *, int);


int gui_userdata(SESSION *ctx, WINDOW *w, char *username) {
	struct gui_userdata_state *gus;

	gus = malloc(sizeof(struct gui_userdata_state));
	gus->win = w;
	gus->taglist = NULL;
	gus->userdata = NULL;


	/* Initialize zlib and expat
	   They will be deallocated when the data processing completes */
	if((gus->g = gzxml_init()) == NULL) {
		mvwprintw(gus->win, 1, 2, "Initialization of compressed XML decoding failed. Sorry!\n");
		wrefresh(gus->win);
		free(gus);
		return -1;
	}


	/* Setup XML handlers */
	XML_SetUserData(gus->g->p, (void *)gus);
	XML_SetElementHandler(gus->g->p, gui_userdata_xml_handle_startelement, gui_userdata_xml_handle_endelement);
	XML_SetCharacterDataHandler(gus->g->p, gui_userdata_xml_handle_text);


	mvwprintw(gus->win, 1, 1, " ");

	return cmd_getuserdata(ctx, username, gui_userdata_result_callback, (void *)gus);
}


static int gui_userdata_result_callback(CHANNEL *ch, unsigned char *buf, unsigned short len) {
	struct gui_userdata_state *gus = (struct gui_userdata_state *)ch->private;
	int skip_len;

	static int a;
	FILE *fd;
	char tmp[100];

	sprintf(tmp, "/tmp/x-userdata-ch%d-id%d", ch->state, ++a);
	fd = fopen(tmp, "a");
	fwrite(buf, 1, len, fd);
	fclose(fd);


	/* Ignore eventual headers */
	if(ch->state == CHANNEL_HEADER)
		return 0;


	/* Present result and cleanup if done */
	if(ch->state == CHANNEL_END || ch->state == CHANNEL_ERROR) {
		struct userdata *u = gus->userdata;

		assert(gus->g->p == NULL);
		wclear(gus->win);
		wprintw(gus->win, "\n");
		if(u) {
			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 1, 3, "Username:");
			wattroff(gus->win, A_BOLD);
			if(u->username) {
				mvwprintw(gus->win, 1, 16, "%s", u->username);
				free(u->username);
			}

			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 2, 3, "Email:");
			wattroff(gus->win, A_BOLD);
			if(u->email) {
				mvwprintw(gus->win, 2, 16, "%s", u->email);
				free(u->email);
			}

			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 3, 3, "Birthdate:");
			wattroff(gus->win, A_BOLD);
			if(u->birthdate) {
				mvwprintw(gus->win, 3, 16, "%s", u->birthdate);
				free(u->birthdate);
			}

			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 4, 3, "Gender:");
			wattroff(gus->win, A_BOLD);
			if(u->gender) {
				mvwprintw(gus->win, 4, 16, "%s", u->gender);
				free(u->gender);
			}

			#if 0
			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 5, 3, "City:");
			wattroff(gus->win, A_BOLD);
			if(u->city) {
				mvwprintw(gus->win, 5, 16, "%s", u->city);
				free(u->city);
			}
			#endif

			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 6, 3, "Postal code:");
			wattroff(gus->win, A_BOLD);
			if(u->postal_code) {
				mvwprintw(gus->win, 6, 16, "%s", u->postal_code);
				free(u->postal_code);
			}

			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 7, 3, "Country:");
			wattroff(gus->win, A_BOLD);
			if(u->country) {
				mvwprintw(gus->win, 7, 16, "%s", u->country);
				free(u->country);
			}

			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 8, 3, "Send mail:");
			wattroff(gus->win, A_BOLD);
			mvwprintw(gus->win, 8, 16, "%s", u->sendemail? "Yes": "No");

			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 9, 3, "Disabled:");
			wattroff(gus->win, A_BOLD);
			mvwprintw(gus->win, 9, 16, "%s", u->disabled? "Yes": "No");

			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 10, 3, "Deleted:");
			wattroff(gus->win, A_BOLD);
			mvwprintw(gus->win, 10, 16, "%s", u->deleted? "Yes": "No");

			free(u);
		}
		else {
			wattron(gus->win, A_BOLD);
			mvwprintw(gus->win, 1, 3, "An error occured (error = %d)", ntohs(*(unsigned short *)buf));
			wattroff(gus->win, A_BOLD);
		}

		wrefresh(gus->win);
		return 0;
	}

	if(ch->state == CHANNEL_DATA) {
		/* Assume minimal gzip header with no extras and skip it */
		if(ch->total_data_len < 10) {
			skip_len = 10 - ch->total_data_len;
			while(skip_len && len) {
				skip_len--;
				len--;
				buf++;
			}

			if(len == 0)
				return 0;
		}

		/* Parse more data */
		wprintw(gus->win, "..");
		wrefresh(gus->win);
	}


	return gzxml_process(gus->g, buf, len);
}


static void gui_userdata_xml_handle_startelement(void *private, const XML_Char *name, const XML_Char **attr) {
	struct gui_userdata_state *gus = (struct gui_userdata_state *)private;

	xml_push_tag(&gus->taglist, name);
	if(!xml_has_parent_path(gus->taglist, "/users"))
		return;

	if(gus->userdata == NULL)
		gus->userdata = calloc(1, sizeof(struct userdata));

}


static void gui_userdata_xml_handle_endelement(void *private, const XML_Char *name) {
	struct gui_userdata_state *gus = (struct gui_userdata_state *)private;

	xml_pop_tag(&gus->taglist);
}


static void gui_userdata_xml_handle_text(void *private, const XML_Char *s, int len) {
	char *buf;
	struct gui_userdata_state *gus = (struct gui_userdata_state *)private;
	struct tagstack *ts = gus->taglist;
	int i;

	buf = (char *)malloc(len + 1);
	memcpy(buf, s, len);
	buf[len] = 0;

	/* Skip whitespace nodes */
	for(i = 0; i < len; i++)
		if(s[i] != ' ' && s[i] != '\r' && s[i] != '\n' && s[i] != '\t')
			break;
	if(i == len) {
		free(buf);
		return;
	}

	if((ts = xml_has_parent_path(ts, "/users/user")) != NULL) {
		if(!strcmp(ts->name, "birthdate"))
			gus->userdata->birthdate = strdup(buf);
		else if(!strcmp(ts->name, "city"))
			gus->userdata->city = strdup(buf);
		else if(!strcmp(ts->name, "country"))
			gus->userdata->country = strdup(buf);
		else if(!strcmp(ts->name, "username"))
			gus->userdata->username = strdup(buf);
		else if(!strcmp(ts->name, "email"))
			gus->userdata->email = strdup(buf);
		else if(!strcmp(ts->name, "gender"))
			gus->userdata->gender = strdup(buf);
		else if(!strcmp(ts->name, "postal_code"))
			gus->userdata->postal_code = strdup(buf);
		else if(!strcmp(ts->name, "disabled"))
			gus->userdata->disabled = (!strcmp(buf, "False")? 0: 1);
		else if(!strcmp(ts->name, "deleted"))
			gus->userdata->deleted = (!strcmp(buf, "False")? 0: 1);
		else if(!strcmp(ts->name, "sendemail"))
			gus->userdata->sendemail = (!strcmp(buf, "False")? 0: 1);
	}

	free(buf);
}

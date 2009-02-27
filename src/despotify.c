/*
 * $Id: despotify.c 734 2009-02-23 18:27:54Z x $
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <ncurses.h>
#include <pthread.h>

#include "ADMclubbing.h"
#include "audio.h"
#include "auth.h"
#include "event.h"
#include "session.h"
#include "commands.h"
#include "handlers.h"
#include "keyexchange.h"
#include "packet.h"
#include "puzzle.h"
#include "ui.h"
#include "ui-player.h"
#include "ui-playlist.h"
#include "util.h"


static int despotify_login(EVENT *, enum ev_flags);
static int despotify_packet_io(EVENT *, enum ev_flags);
static int despotify_no_support_for_introduction_accounts(EVENT *, enum ev_flags);



int main(int argc, char **argv) {
	SESSION *c;
	EVENT *e;
	

	printf(	"\n"
		"despotify  by  #hack.se  (W0RLD D0M1N4T10N '08/09 @!#$@!$)\n"
		"  -- a free, open, crossplatform Spotify-compatible client\n"
		"\n");

	if(argc != 3) {
		fprintf(stderr, "Usage:\t%s <username> <password>\n", argv[0]);
		printf("\n		--- by courtesy of antilove and dek of ADM\n\n");
		ADMclubbing();
		return -1;
	}


	/* Initialize audio output device */
	if(audio_init()) {
		fprintf(stderr, "Though luck, bitch. No audio available.\n");
		return -1;
	}


	/* Initialize client context */
	c = session_init_client();

	session_auth_set(c, argv[1], argv[2]);

	/* Register our login handler */	
	e = event_register_action(NULL, 0, despotify_login, (void *)c);
	event_msg_subscription_class_set(e, MSG_CLASS_APP);

	
#ifdef GUI
	/* Register the GUI handler */
	e = event_register_action(NULL, 0, gui_action_handler, NULL);
	event_msg_subscription_class_set(e, MSG_CLASS_APP|MSG_CLASS_GUI);

	/* Watch stdin for readable data */
	event_register_fd(e, 0);

	/* Mark event processor as idle and wait for data on stdin */
	event_mark_idle(e);
#endif

	
	/* Main loop, exists only when we run out of things to do */
	event_loop(-1);


	printf("* Shutting down gracefully\n");
	session_free(c);
	
	return 0;
}


static int despotify_login(EVENT *ev, enum ev_flags ev_kind) {
	SESSION *s = (SESSION *)ev->private;
	int err = 0;
	static int is_initialized = 0;

	if(ev_kind == EV_MSG) {
		if(ev->msg->class == MSG_CLASS_APP) {
			switch(ev->msg->msg) {
			case MSG_APP_EXIT:
				session_disconnect(s);
				event_unregister_fd(ev);
				event_mark_done(ev);
				break;

			default:
				break;
			}
		}

		return 0;
	}

	switch(ev->state) {
	case 0:
		/* Connect */
		DSFYDEBUG("despotify_login(): Connecting..\n");
		if((err = session_connect(s)) == 0) {
			ev->state++;
			event_register_fd(ev, s->ap_sock);
		}
		else
			event_mark_done(ev);
		break;

	case 1:
		/* Send initial packet */
		if((err = send_client_initial_packet(s)) == 0) {
			event_mark_idle(ev);
			ev->state++;
		}
		else
			event_msg_post(MSG_CLASS_APP, MSG_APP_EXIT, NULL);

		break;

	case 2:
		/* Read response */
		if(ev_kind != EV_FD)
			break;

		if((err = read_server_initial_packet(s)) == 0)
			ev->state++;
		else
			event_msg_post(MSG_CLASS_APP, MSG_APP_EXIT, NULL);

		break;

	case 3:
		/* Compute shared keys etc */
		auth_generate_auth_hash(s);
		key_init(s);
		puzzle_solve(s);
		auth_generate_auth_hmac(s, s->auth_hmac, sizeof(s->auth_hmac));

		/* Tell the server we're cool */
		if((err = send_client_auth(s)) == 0) {
			event_mark_idle(ev);
			ev->state++;
		}
		else 
			event_msg_post(MSG_CLASS_APP, MSG_APP_EXIT, NULL);

		break;

	case 4:
		if(ev_kind != EV_FD)
			break;

		/* Now, let's see how long Spotify agree's we're indeed cool */
		if((err = read_server_auth_response(s)) == 0) {
			event_mark_idle(ev);
			ev->state++;
		}
		else
			event_msg_post(MSG_CLASS_APP, MSG_APP_EXIT, NULL);

		break;

	case 5:
		/* Login process complete, shutdown this processing unit */
		event_mark_done(ev);

		/* Move on to do packet I/O */
		ev = event_register_action(NULL, 0, despotify_packet_io, (void *)s);
		event_register_fd(ev, s->ap_sock);
		event_msg_subscription_class_set(ev, MSG_CLASS_APP);

		/* Notify anyone interested that we're online */
		event_msg_post(MSG_CLASS_APP, MSG_APP_ONLINE, NULL);


		/*
		 * Usability fix for being able to see any eventual error
		 * messages during the login.
		 * (Curses debug output? Hell no!)
		 *
		 */
		if(!is_initialized++) {
#ifdef GUI
			void **container;

		        /* Initialize ncurses */
		        gui_init();

			/* Automatically free'd by the message processor */
			container = (void **)malloc(sizeof(void *));
			*container = s;

		        /* The GUI needs access to the session context */
			event_msg_post(MSG_CLASS_GUI, MSG_GUI_SESSIONPTR, container);

			/* Register handler for downloading playlists */
			ev = event_register_action(NULL, 0, gui_playlists_download, NULL);
			event_msg_subscription_class_set(ev, MSG_CLASS_APP|MSG_CLASS_GUI);

			/*
			 * Register event processor responsible for taking care of 
			 * of the play command, fetching AES keys for tracks, downloading
			 * ogg data, decrypting it and sending it down to the sound system
			 *
			 */
			ev = event_register_action(NULL, 0, gui_player_event_processor, NULL);
			event_msg_subscription_class_set(ev, MSG_CLASS_APP|MSG_CLASS_GUI);
#endif
		}

		break;
	}

	return err;
}


static int despotify_packet_io(EVENT *ev, enum ev_flags ev_kind) {
	SESSION *s = (SESSION *)ev->private;
	int err = 0;
	PHEADER hdr;
	unsigned char *payload;


	if(ev_kind == EV_MSG) {
		if(ev->msg->class == MSG_CLASS_APP) {
			switch(ev->msg->msg) {
			case MSG_APP_EXIT:
				event_mark_busy(ev);
				ev->state = 1;
				break;

			case MSG_APP_NET_ERROR:
				DSFYDEBUG("despotify_packet_io(): MSG_APP_NET_ERROR, disconnecting..\n");
				/* Disconnect */
				session_disconnect(s);
				event_msg_post(MSG_CLASS_APP, MSG_APP_DISCONNECTED, NULL);

				/* Shutdown this handler */
				event_unregister_fd(ev);
				event_mark_done(ev);

				/* Try to re-login */
				ev = event_register_action(NULL, 0, despotify_login, ev->private);
				event_msg_subscription_class_set(ev, MSG_CLASS_APP);
				break;

			case MSG_APP_NOTFAIRGAME:
				ev->state = 2;
				event_mark_busy(ev);
				break;

			default:
				break;
			}
		}

		return 0;
	}


	switch(ev->state) {
	case 0:
		/* Read packets */
		if((err = packet_read(s, &hdr, &payload)) == 0) {
                        err = handle_packet(s, hdr.cmd, payload, hdr.len);
                        DSFYfree(payload); /* Allocated in packet_read() */
			payload = NULL;
                        if (err)
                                break;
			event_mark_idle(ev);
		}
		else {
			/* Fuq, signal an error. We'll catch it and re-login */
			event_msg_post(MSG_CLASS_APP, MSG_APP_NET_ERROR, NULL);
			event_mark_busy(ev);
			err = 0;
		}

		break;
		
	case 1:
		/* Exit */
		session_disconnect(s);
		event_unregister_fd(ev);
		event_mark_done(ev);
		break;

	case 2:
		event_msg_post(MSG_CLASS_APP, MSG_APP_EXIT, NULL);
		ev->state = 1;

		ev = event_register_action(NULL, 0, despotify_no_support_for_introduction_accounts, NULL);
		event_delay(ev, 1);
		break;

	default:
		break;

	}

	if(err) {
		printf("despotify_packet_io(): fuq, something went wrong, registering login handler again\n");
		ev = event_register_action(NULL, 0, despotify_login, ev->private);
		event_msg_subscription_class_set(ev, MSG_CLASS_APP);
	}
	
	return err;
}


static int despotify_no_support_for_introduction_accounts(EVENT *ev, enum ev_flags ev_kind) {

	if(ev_kind != EV_RUN)
		return 0;

	printf( "\n\nHi!\n"
		"\n"
		"You do not have a premium account.\n"
		"\n"
		"We see a number of problems with providing an open source client for non-paying\n"
		"users and hence this implementation only supports premium accounts. Sorry!\n"
		"\n"
		"Consider upgrading to a premium account to support not only Spotify,\n"
		"but also despotify's cause of opening up their service to their\n"
		"paying (i.e, loyal) user base.\n"
		"\n"
		"  -- Love, the despotify team.\n"
		"\n"
	       	"\n"
	);


	event_mark_done(ev);

	return 0;
}


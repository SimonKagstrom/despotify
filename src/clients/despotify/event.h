/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_EVENT_H
#define DESPOTIFY_EVENT_H

#include <sys/select.h>

/* Message classes */
typedef struct
{
	int class;
	int msg;
	void *data;
} EVENT_MSG;

#define MSG_CLASS_APP            1

#define MSG_APP_QUIT             1
#define MSG_APP_UPDATE           2
#define MSG_APP_REDRAW           3

enum ev_flags
{
	EV_RUN = 1,		/* Just call the callback */
	EV_RUN_FAIL = 2,	/* Run fail, delete */
	EV_MSG = 4,		/* Process a message */
	EV_MSG_FAIL = 8,	/* Message delievery failed */
	EV_FD = 16,		/* Process fd callback */
	EV_FD_FAIL = 32,	/* An fd event failed */
	EV_IDLE = 64,		/* Idle (i.e, waiting for socket data) */
	EV_DONE = 128,		/* Done, start calling stacked shit */
	EV_DELAY = 256		/* Delay execution for a number of seconds */
};

struct _event;
typedef struct _event EVENT;
typedef int (*event_callback) (EVENT * ev, enum ev_flags flags);
struct _event
{
	/* The receiving function */
	event_callback callback;

	/* Private data */
	void *private;

	/* Private state */
	int state;

	/* A fd watched for incoming data; maybe this should be a fd list? */
	int fd;

	/* The incoming message */
	EVENT_MSG *msg;

	/* Don't mess with anything below this line. ;) */
	time_t delay_until;

	int msg_class_filter;

	/* Event flags */
	enum ev_flags flags;

	/* Event that are called when this one is done */
	EVENT *stack;

	/* Next event */
	EVENT *next;
};

EVENT *event_register_action (EVENT *, enum ev_flags, event_callback, void *);
void event_unregister_action (EVENT **, EVENT *);
int event_register_fd (EVENT *, int);
void event_unregister_fd (EVENT *);

void event_mark_done (EVENT *);
void event_mark_idle (EVENT *);
void event_mark_busy (EVENT *);
void event_delay (EVENT *, int);

void event_msg_subscription_class_set (EVENT *, int);
int event_msg_post (int, int, void *);

int event_loop (int);
#endif

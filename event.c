/*
 * $Id: event.c 703 2009-02-23 02:46:13Z x $
 *
 * Event subsystem for despotify
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#include "event.h"


static int event_fd_add_to_set(fd_set *fds, EVENT *e);
static void event_fd_callbacks_run(fd_set *, EVENT *e);
static int event_callbacks_run(EVENT *e);
static void event_msg_deliver(EVENT *e, EVENT_MSG *m);



EVENT *evhead;
static EVENT_MSG **msgs;
static int num_msgs;


/*
 * Register an event/action handler with choosen flags
 *
 * If the first parameter is non-NULL, stack this event.
 * Stacked event are only called once the first event has
 * called event_mark_done() on itself.
 *
 */
EVENT *event_register_action(EVENT *head, enum ev_flags flags, event_callback callback, void *private) {
	EVENT *e;
	
	if((e = head) == NULL)
		e = evhead;
	
	if(e == NULL)
		e = evhead = malloc(sizeof(EVENT));
	else {
		while(e->next)
			e = e->next;
		
		e->next = malloc(sizeof(EVENT));
		e = e->next;
	}
	
	if(!e)
		return NULL;

	/* The callback routine */
	e->callback = callback;
	
	/* Private variables */
	e->private = private;
	e->state = 0;

	/* Any posted messages are delievered in here */
	e->msg = NULL;
	e->msg_class_filter = 0;

	/* Used by event_register_fd, event_fd_callbacks_run, .. */
	e->fd = -1;

	/* For internal stuff */
	e->flags = flags;
	e->stack = NULL;
	e->next = NULL;
	
	return e;
}


/*
 * Unregister an event/action and all stacked event/actions
 *
 */
void event_unregister_action(EVENT **head, EVENT *bad) {
	EVENT *e;
	
	if(head == NULL)
		head = &evhead;

	for(e = *head; e->next; e = e->next) {
		if(e->next == bad) {
			while(bad->stack)
				event_unregister_action(&bad->stack, bad->stack);
				
			e->next = bad->next;
			break;
		}
	}

	*head = bad->next;
	free(bad);
}


/* Register a file descriptor to issue a callback on when it becomes being readable */
int event_register_fd(EVENT *e, int fd) {
	if(e->flags & EV_FD_FAIL)
		e->flags &= ~(EV_FD|EV_FD_FAIL);

	if(e->flags & EV_FD) {
		return -1;
	}
	
	e->fd = fd;
	e->flags |= EV_FD;
		
	return 0;
}


/* Reset fd */
void event_unregister_fd(EVENT *e) {
	e->fd = 0;
	e->flags &= EV_FD;
}


/* Helper routine for recursively adding file descriptors to a fd_set */
static int event_fd_add_to_set(fd_set *fds, EVENT *e) {
	struct timeval tv;
	int max_fd = 0;
	int ret;

	for(; e; e = e->next) {
		/* Ignore those that are delayed */
		if(e->flags & EV_DELAY) {
			if(e->delay_until > tv.tv_sec)
				continue;

			e->flags &= ~EV_DELAY;
		}

		if(e->fd != -1) {
			FD_SET(e->fd, fds);
			if(e->fd > max_fd)
				max_fd = e->fd;
		}

		if(e->stack) {
			ret = event_fd_add_to_set(fds, e->stack);
			if(ret > max_fd)
				max_fd = ret;
		}
	}

	return max_fd;
}


/* Recursively run callbacks on readable fds */
static void event_fd_callbacks_run(fd_set *fds, EVENT *e) {
	for(; e; e = e->next) {
		if(e->fd != -1 && FD_ISSET(e->fd, fds)) {
			/* Clear idle flag before running */
			e->flags &= ~EV_IDLE;
			
			if(e->callback(e, EV_FD))
				e->flags |= EV_FD_FAIL;
		}
		
		if(e->stack)
			event_fd_callbacks_run(fds, e->stack);
	}
}


/* Run the event/action callbacks */
static int event_callbacks_run(EVENT *e) {
	int busy = 0;
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	for(; e; e = e->next) {
		/* Ignore those that are delayed */
		if(e->flags & EV_DELAY) {
			if(e->delay_until > tv.tv_sec)
				continue;

			e->flags &= ~EV_DELAY;
		}

		/* Don't EV_RUN on those marked EV_IDLE */
		if(e->flags & EV_IDLE)
			continue;

		if(e->callback(e, EV_RUN))
			e->flags |= EV_RUN_FAIL;

		/* If the idle flag isn't set, increment busy counter */
		if((e->flags & EV_IDLE) == 0)
			busy++;

#if 0
		if(e->stack)
			busy += event_callbacks_run(e->stack);
#endif
	}

	/* Return number of busy (non-idle) events */	
	return busy;
}


/* Set a list of messages classes to receive notifications for */
void event_msg_subscription_class_set(EVENT *e, int class) {
	e->msg_class_filter = class;
}


/*
 * Queue a message in a certain class for delivery to interested parties.
 *
 * If the third parameter (data) is non-NULL, it's automatically
 * free'd after delivering the message to all clients.
 * Use a container to deliever pointers!
 *
 */
int event_msg_post(int class, int msg, void *data) {
	EVENT_MSG **tmp;
	if((tmp = (EVENT_MSG **)realloc(msgs, sizeof(EVENT_MSG *) * (num_msgs + 1))) == NULL)
		return -1;

	msgs = tmp;
	msgs[num_msgs] = (EVENT_MSG *)malloc(sizeof(EVENT_MSG));
	msgs[num_msgs]->class = class;
	msgs[num_msgs]->msg = msg;
	msgs[num_msgs]->data = data;
	num_msgs++;
	
	return 0;
}


/*
 * Helper routine for recursively delivering messages to 
 * registered callbacks.
 *
 *
 */
static void event_msg_deliver(EVENT *e, EVENT_MSG *m) {

	for(; e; e = e->next) {
		if((e->msg_class_filter & m->class) == 0)
			continue;

		/* Temporarily set e->msg to this message */
		e->msg = m;
		if(e->callback(e, EV_MSG)) {
			e->flags |= EV_MSG_FAIL;
		}

		e->msg = NULL;
#if 0
		/* Hmm..? */			
		if(e->stack)
			event_msg_deliver(e, class, msg);
#endif
	}
}


/*
 * Mark an event/action as done, schedule for deletion
 *
 * The event handler might be called once more if there's
 * messages to be delivered (with EV_MSG)
 *
 */
void event_mark_done(EVENT *e) {
	e->flags |= EV_DONE;
}


/*
 * Mark an event/action as idle, to allow for sleeps
 *
 * The idle-flags is automatically cleared if there's data
 * to be read on a registered file descriptor.
 * It can also be cleared using event_mark_busy()
 *
 */
void event_mark_idle(EVENT *e) {
	e->flags |= EV_IDLE;
}


/*
 * Mark an event/action as busy
 * It will be called as soon as possible again by event_loop()
 *
 */
void event_mark_busy(EVENT *e) {
	e->flags &= ~EV_IDLE;
}


/*
 * Don't call the callback for the specified number of seconds
 *
 * It might still be called prematurely if there's data to be
 * read on a registered file descriptor.
 *
 */
void event_delay(EVENT *e, int seconds) {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	e->delay_until = tv.tv_sec + seconds;
	e->flags |= EV_DELAY;
}

/*
 * The main event loop
 * Pass the number of loops you want to do, or -1 to run forever 
 *
 */
int event_loop(int loop_counter) {
	EVENT *e, *prev;
	fd_set rfds;
	struct timeval tv;
	int busy, i, n, ret;

	ret = 0;
	for(;;) {
		if(loop_counter != -1 && !(ret = loop_counter--))
			break;

		if(evhead == NULL) {
			/* If we're out of things to do, return */
			ret = 0;
			break;
		}
		
		
		/* Run callbacks */
		busy = event_callbacks_run(evhead);
		
		/* Allow sleep in idle situations */
		tv.tv_sec = tv.tv_usec = 0;
		if(!busy && num_msgs == 0)
			tv.tv_sec = 1;

		/* Process network I/O */
		FD_ZERO(&rfds);
		n = event_fd_add_to_set(&rfds, evhead);

		if(n) {
			if((ret = select(n + 1, &rfds, NULL, NULL, &tv)) < 0) {
				if(errno == EINTR)
					continue;

				printf("select() failed\n");
				break;
			}


			if(ret)
				event_fd_callbacks_run(&rfds, evhead);
		}

		
		/* Deliever queued messages, then clear the queue */
		for(i = 0; i < num_msgs; i++) {
			event_msg_deliver(evhead, msgs[i]);
			if(msgs[i]->data)
				free(msgs[i]->data);
			free(msgs[i]);
		}
		
		if(num_msgs) {
			free(msgs);
			msgs = NULL;
			num_msgs = 0;
		}


		/* Handle EV_DONE and EV_FAIL */
		for(prev = NULL, e = evhead; e; e = e->next) {
			if(e->flags & EV_DONE) {
				if(prev) {
					/* If there are stacked events, make the first one replace this one's place */
					if((prev->next = e->stack) != NULL) {
						e->stack = e->stack->next;
						prev->next->next = e->next;
					}
					else
						prev->next = e->next;
					
					free(e);
					e = prev;
					continue;
				}
				else {
					if((evhead = e->stack) != NULL) {
						e->stack = e->stack->next;
						evhead->next = e->next;
					}
					else
						evhead = e->next;

					free(e);
					if((e = evhead) == NULL)
						break;
					else {
						prev = e;
						continue;
					}
				}
				
			}

			if(e->flags & (EV_RUN_FAIL|EV_FD_FAIL)) {
				/* Unregister all stacked calls since this one failed */
				while(e->stack)
					event_unregister_action(&e->stack, e->stack);

				/* Unlink and continue if possible */
				if(prev) {
					prev->next = e->next;
					free(e);
					e = prev;
					continue;
				}
				else {
					evhead = e->next;
					free(e);
					if((e = evhead) == NULL)
						break;
					else {
						prev = e;
						continue;
					}
				}
			}
			
			prev = e;
		}

	} /* for(;;) */

	return ret;
}

/*
 * $Id$
 *
 */

#include <locale.h>
#include <signal.h>
#include <stdlib.h>

#include "event.h"
#include "session.h"
#include "ui.h"

int event_handler(EVENT *e, enum ev_flags ev_kind);

void panic(const char *msg)
{
  stdscr_cleanup();
  fprintf(stderr, "%s\n", msg);
  abort();
}

void sig_int(int signum)
{
  (void)signum;
  event_msg_post(MSG_CLASS_APP, MSG_APP_QUIT, NULL);
}

void sig_segv(int signum)
{
  (void)signum;
  // Avoid infinite loop.
  signal(signum, SIG_IGN);
  panic("Ouch, I segfaulted. How embarrassing. :-(\n");
}

void sig_winch(int signum)
{
  (void)signum;
  event_msg_post(MSG_CLASS_APP, MSG_APP_REDRAW, NULL);
}

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  setlocale(LC_ALL, "");

  // Register event handler.
  EVENT *e = event_register_action(NULL, 0, event_handler, NULL);
  event_msg_subscription_class_set(e, MSG_CLASS_APP);

  // Read keyboard input.
  event_register_fd(e, /*STDIN_FILENO*/ 0);
  event_mark_idle(e);

  // Register signal handlers.
  signal(SIGINT, sig_int);
  signal(SIGTERM, sig_int);
  signal(SIGSEGV, sig_segv);
  signal(SIGWINCH, sig_winch);

  // Initialize libdespotify.
  sess_init();

  // Initialize UI.
  ui_init();

  // Main loop.
  event_loop(-1);

  // Cleanup UI.
  ui_cleanup();

  // Cleanup libdespotify.
  sess_cleanup();

  return 0;
}

// Main event handler. Reads keyboard input and updates screen output.
int event_handler(EVENT *e, enum ev_flags ev_kind)
{
  if (ev_kind == EV_MSG && e->msg->class == MSG_CLASS_APP) {
    switch (e->msg->msg) {
      case MSG_APP_QUIT:
        // Remove keyboard listener.
        event_unregister_fd(e);
        event_mark_done(e);
        break;

      case MSG_APP_UPDATE:
        // Apply UI changes.
        ui_update(false);
        break;

      case MSG_APP_REDRAW:
        // Force full UI redraw.
        ui_balance();
        break;
    }
  }
  else if (ev_kind == EV_FD) {
    int ch = getch();
    if (ch == ERR)
      return 1;
    else
      ui_keypress(ch);
    event_mark_idle(e);
  }

  return 0;
}

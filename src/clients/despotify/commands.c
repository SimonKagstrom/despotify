/*
 * $Id$
 *
 */

#include <string.h>

#include "commands.h"
#include "event.h"
#include "session.h"
#include "ui.h"
#include "ui_footer.h"
#include "ui_log.h"

void cmd_cb_connect()
{
  // Prompt for username.
  footer_input(INPUT_USERNAME);
}

void cmd_cb_disconnect()
{
  sess_disconnect();
}

void cmd_cb_search()
{
  footer_input(INPUT_SEARCH);
}

void cmd_cb_log()
{
  ui_show(UI_SET_LOG);
}

void cmd_cb_help()
{
  ui_show(UI_SET_HELP);
}

void cmd_cb_main()
{
  ui_show(UI_SET_BROWSER);
}

void cmd_cb_redraw()
{
  event_msg_post(MSG_CLASS_APP, MSG_APP_REDRAW, NULL);
}

void cmd_cb_quit()
{
  event_msg_post(MSG_CLASS_APP, MSG_APP_QUIT, NULL);
}

typedef void (*cmd_cb_t)();
typedef struct cmd {
  char    *name;
  int      args;
  cmd_cb_t cmd_cb;
} cmd_t;

static cmd_t g_commands[] = {
  { "connect",    0, cmd_cb_connect    },
  { "c",          0, cmd_cb_connect    },
  { "disconnect", 0, cmd_cb_disconnect },
  { "d",          0, cmd_cb_disconnect },
  { "search",     0, cmd_cb_search     },
  { "s",          0, cmd_cb_search     },
  { "log",        0, cmd_cb_log        },
  { "l",          0, cmd_cb_log        },
  { "help",       0, cmd_cb_help       },
  { "h",          0, cmd_cb_help       },
  { "?",          0, cmd_cb_help       },
  { "main",       0, cmd_cb_main       },
  { "m",          0, cmd_cb_main       },
  { "redraw",     0, cmd_cb_redraw     },
  { "r",          0, cmd_cb_redraw     },
  { "quit",       0, cmd_cb_quit       },
  { "q",          0, cmd_cb_quit       },
  { 0, 0, 0 }
};

void command_process(char *str)
{
  char cmd[16];
  if (sscanf(str, "%15s", cmd) == 1) {
    for (int i = 0; g_commands[i].name; ++i) {
      if (!strcmp(cmd, g_commands[i].name)) {
        g_commands[i].cmd_cb();
        return;
      }
    }
    log_append("Unknown command: '%s'", cmd);
  }
}

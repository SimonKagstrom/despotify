/*
 * $Id$
 *
 */

#include <sys/ioctl.h>

#include "commands.h"
#include "event.h"
#include "session.h"
#include "ui.h"
#include "ui_footer.h"
#include "ui_help.h"
#include "ui_log.h"
#include "ui_sidebar.h"
#include "ui_tracklist.h"

#define UI_FOREACH_START_END(uis, var, start, end) for (ui_t *var = &uis[start], *_i = (ui_t*)start; \
                                                   (long)_i != end; _i = (ui_t*)((long)_i + 1), \
                                                   var = &uis[(long)_i])
#define UI_FOREACH_START(uis, var, start)          UI_FOREACH_START_END(uis, var, start, UI_END)
#define UI_FOREACH(uis, var)                       UI_FOREACH_START_END(uis, var, 0,     UI_END)

static ui_t      g_ui_elements[UI_END];
static int       g_stdscr_initialized = 0;
static ui_elem_t g_ui_focus;

// Draw header UI element.
static void header_draw(ui_t *ui)
{
  mvwprintw(ui->win, 0, DSFY_MAX(0, ((signed)ui->width - 5) / 2), "despotify");
}

// Draw playback status placeholder.
static void player_draw(ui_t *ui)
{
  wbkgd(ui->win, '.');
}

// Ncurses initialization.
void stdscr_init()
{
  if (g_stdscr_initialized)
    return;

  initscr();
  raw();
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  curs_set(0);

  g_stdscr_initialized = 1;
}

// Ncurses cleanup.
void stdscr_cleanup()
{
  if (!g_stdscr_initialized)
    return;

  noraw();
  endwin();

  g_stdscr_initialized = 0;
}

// Initialize UI elements.
void ui_init()
{
  stdscr_init();

  g_ui_elements[UI_HEADER].win             = newwin(0, 0, 0, 0);
  g_ui_elements[UI_HEADER].flags           = 0;
  g_ui_elements[UI_HEADER].set             = UI_SET_NONE;
  g_ui_elements[UI_HEADER].fixed_width     = 0;
  g_ui_elements[UI_HEADER].fixed_height    = 1;
  g_ui_elements[UI_HEADER].draw_cb         = header_draw;
  g_ui_elements[UI_HEADER].keypress_cb     = 0;

  g_ui_elements[UI_SIDEBAR].win            = newwin(0, 0, 0, 0);
  g_ui_elements[UI_SIDEBAR].flags          = 0;
  g_ui_elements[UI_SIDEBAR].set            = UI_SET_BROWSER;
  g_ui_elements[UI_SIDEBAR].fixed_width    = 25;
  g_ui_elements[UI_SIDEBAR].fixed_height   = 0;
  g_ui_elements[UI_SIDEBAR].draw_cb        = sidebar_draw;
  g_ui_elements[UI_SIDEBAR].keypress_cb    = sidebar_keypress;

  g_ui_elements[UI_TRACKLIST].win          = newwin(0, 0, 0, 0);
  g_ui_elements[UI_TRACKLIST].flags        = 0;
  g_ui_elements[UI_TRACKLIST].set          = UI_SET_BROWSER;
  g_ui_elements[UI_TRACKLIST].fixed_width  = 0;
  g_ui_elements[UI_TRACKLIST].fixed_height = 0;
  g_ui_elements[UI_TRACKLIST].draw_cb      = tracklist_draw;
  g_ui_elements[UI_TRACKLIST].keypress_cb  = tracklist_keypress;

  g_ui_elements[UI_LOG].win                = newwin(0, 0, 0, 0);
  g_ui_elements[UI_LOG].flags              = 0;
  g_ui_elements[UI_LOG].set                = UI_SET_LOG;
  g_ui_elements[UI_LOG].fixed_width        = 0;
  g_ui_elements[UI_LOG].fixed_height       = 0;
  g_ui_elements[UI_LOG].draw_cb            = log_draw;
  g_ui_elements[UI_LOG].keypress_cb        = log_keypress;

  g_ui_elements[UI_HELP].win               = newwin(0, 0, 0, 0);
  g_ui_elements[UI_HELP].flags             = 0;
  g_ui_elements[UI_HELP].set               = UI_SET_HELP;
  g_ui_elements[UI_HELP].fixed_width       = 0;
  g_ui_elements[UI_HELP].fixed_height      = 0;
  g_ui_elements[UI_HELP].draw_cb           = help_draw;
  g_ui_elements[UI_HELP].keypress_cb       = help_keypress;

  g_ui_elements[UI_PLAYER].win             = newwin(0, 0, 0, 0);
  g_ui_elements[UI_PLAYER].flags           = 0;
  g_ui_elements[UI_PLAYER].set             = UI_SET_NONE;
  g_ui_elements[UI_PLAYER].fixed_width     = 0;
  g_ui_elements[UI_PLAYER].fixed_height    = 3;
  g_ui_elements[UI_PLAYER].draw_cb         = player_draw;
  g_ui_elements[UI_PLAYER].keypress_cb     = 0;

  g_ui_elements[UI_FOOTER].win             = newwin(0, 0, 0, 0);
  g_ui_elements[UI_FOOTER].flags           = 0;
  g_ui_elements[UI_FOOTER].set             = UI_SET_NONE;
  g_ui_elements[UI_FOOTER].fixed_width     = 0;
  g_ui_elements[UI_FOOTER].fixed_height    = 1;
  g_ui_elements[UI_FOOTER].draw_cb         = footer_draw;
  g_ui_elements[UI_FOOTER].keypress_cb     = footer_keypress;

  ui_show(UI_SET_BROWSER);
  ui_balance();
}

// Cleanup UI elements.
void ui_cleanup()
{
  stdscr_cleanup();

  UI_FOREACH(g_ui_elements, ui) {
    delwin(ui->win);
    ui->win = 0;
  }
}

// Resize and stack UI elements on available screen space. Dynamic-height
// elements are sized after static-height ones. Multiple dynamic-height
// elements are combined in horizontal sets.
void ui_balance()
{
  struct winsize ws;
  ioctl(0, TIOCGWINSZ, &ws);
  resizeterm(ws.ws_row, ws.ws_col);
  unsigned int scrwidth = ws.ws_col, scrheight = ws.ws_row;


  // Set size for fixed-height elements.
  UI_FOREACH(g_ui_elements, ui) {
    if (ui->fixed_height) {
      ui->width = scrwidth;
      ui->height = DSFY_MIN(ui->fixed_height, scrheight);
      scrheight -= ui->height;
    }
  }

  // Set remaining height to dynamic-height elements.
  UI_FOREACH(g_ui_elements, ui)
    if (!ui->fixed_height)
      ui->height = scrheight;

  // Balance widths in horizontal sets for dynamic-height elements.
  unsigned int setwidth[UI_SET_END];
  for (ui_set_t set = UI_SET_NONE + 1; set < UI_SET_END; ++set)
    setwidth[set] = scrwidth;

  // Set width for fixed-width elements.
  UI_FOREACH(g_ui_elements, ui) {
    if (ui->set > UI_SET_NONE && ui->fixed_width) {
      ui->width = DSFY_MIN(ui->fixed_width, setwidth[ui->set]);
      setwidth[ui->set] -= ui->width;
    }
  }

  // Set remaining width to dynamic-width elements.
  UI_FOREACH(g_ui_elements, ui) {
    if (ui->set > UI_SET_NONE && !ui->fixed_width)
      ui->width = setwidth[ui->set];
  }
  
  // Stack elements. Set all dynamic elements at same y-position, only one
  // horizontal set will be visible at a time.
  unsigned int scry = 0,
               staty = 0,
               setx[UI_SET_END] = { };
  UI_FOREACH(g_ui_elements, ui) {
    // Resize and mark dirty.
    wresize(ui->win, ui->height, ui->width);
    ui->flags |= UI_FLAG_DIRTY;

    // Move.
    if (ui->fixed_height) {
      mvwin(ui->win, scry, 0);
      scry += ui->height;
    }
    else {
      if (!staty) {
        staty = scry;
        scry += ui->height;
      }
      mvwin(ui->win, staty, setx[ui->set]);
      setx[ui->set] += ui->width;
    }
  }

  // Full draw.
  ui_update(true);
}

// Update all UI elements that are visible and marked dirty. If the redraw
// argument is set the dirty flag is ignored and a full redraw is performed
// by flushing ncurses' internal cache.
void ui_update(bool redraw)
{
  if (redraw)
    redrawwin(stdscr);

  wnoutrefresh(stdscr);

  UI_FOREACH(g_ui_elements, ui) {
    if (!(ui->flags & UI_FLAG_OFFSCREEN)
        && ((ui->flags & UI_FLAG_DIRTY) || redraw)
        && ui->height && ui->width) {
      ui->flags &= ~UI_FLAG_DIRTY;
      werase(ui->win);
      ui->draw_cb(ui);

      if (redraw)
        redrawwin(ui->win);

      wnoutrefresh(ui->win);
    }
  }

  doupdate();
}

// Show and focus a UI element set. Elements in current visible set will be disabled.
void ui_show(ui_set_t show)
{
  bool focused = false;

  for (ui_elem_t i = 0; i < UI_END; ++i) {
    if (g_ui_elements[i].set == show) {
      // Focus only first element in set.
      if (!focused) {
        g_ui_elements[i].flags |= UI_FLAG_FOCUS;
        g_ui_focus = i;
        focused = true;
      }
      g_ui_elements[i].flags |= UI_FLAG_DIRTY;
      g_ui_elements[i].flags &= ~UI_FLAG_OFFSCREEN;
      
    }
    else if (g_ui_elements[i].set != UI_SET_NONE) {
      g_ui_elements[i].flags &= ~(UI_FLAG_FOCUS | UI_FLAG_DIRTY);
      g_ui_elements[i].flags |= UI_FLAG_OFFSCREEN;
    }
  }

  event_msg_post(MSG_CLASS_APP, MSG_APP_UPDATE, NULL);
}

// Mark UI element as dirty. Will cause redraw of the element at next ui_update().
void ui_dirty(ui_elem_t dirty)
{
  g_ui_elements[dirty].flags |= UI_FLAG_DIRTY;
}

// Move focus to given UI element. Focused element receives keypresses and
// focus may affect how highlighted content is drawn.
void ui_focus(ui_elem_t focus)
{
  // Don't focus offscreen UI elements or elements that can't handle keypresses.
  if ((g_ui_elements[focus].flags & UI_FLAG_OFFSCREEN)
      || !g_ui_elements[focus].keypress_cb)
    return;

  for (ui_elem_t i = 0; i < UI_END; ++i) {
    if (i == focus) {
      g_ui_elements[i].flags |= (UI_FLAG_FOCUS | UI_FLAG_DIRTY);
      g_ui_focus = focus;
    }
    else if (g_ui_elements[i].flags & UI_FLAG_FOCUS) {
      g_ui_elements[i].flags &= ~UI_FLAG_FOCUS;
      g_ui_elements[i].flags |= UI_FLAG_DIRTY;
    }
  }

  event_msg_post(MSG_CLASS_APP, MSG_APP_UPDATE, NULL);
}

// Return currently focused UI element.
ui_elem_t ui_focused()
{
  return g_ui_focus;
}

// Handle keyboard bindings and redirect keypresses to focused UI element.
void ui_keypress(wint_t ch, bool code)
{
  // Very special keys.
  switch (ch) {
    // Quit program.
    case 'Q' - '@':
      event_msg_post(MSG_CLASS_APP, MSG_APP_QUIT, NULL);
      return;

    // Force screen redraw.
    case 'L' - '@':
      event_msg_post(MSG_CLASS_APP, MSG_APP_REDRAW, NULL);
      return;

    case KEY_F(1):
      ui_show(UI_SET_HELP);
      return;
  }

  // Forward regular keys to focused element.
  ch = g_ui_elements[g_ui_focus].keypress_cb(ch, code);

  if (!ch)
    return;

  // Check remaining keys. Text input will override most of these.
  switch (ch) {
    case ':':
      footer_input(INPUT_COMMAND);
      return;

    case '/':
      footer_input(INPUT_SEARCH);
      return;

    case KEY_BACKSPACE:
      cmd_cb_stop();
      return;

    case ' ':
      cmd_cb_pause();
      break;

    case '|':
      ui_show(UI_SET_LOG);
      return;

    case 'c':
      cmd_cb_connect();
      return;

    case 'd':
      cmd_cb_disconnect();
      return;
  }
}

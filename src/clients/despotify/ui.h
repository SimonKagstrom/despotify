/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_UI_H
#define DESPOTIFY_UI_H

#include <ncurses.h>
#include <stdbool.h>

#define DSFY_MAX(a, b) ((a) > (b) ? (a) : (b))
#define DSFY_MIN(a, b) ((a) < (b) ? (a) : (b))

#define KEY_ESC 0x1b

typedef enum ui_flags {
  UI_FLAG_OFFSCREEN = 1 << 0,
  UI_FLAG_FOCUS     = 1 << 1,
  UI_FLAG_DIRTY     = 1 << 2
} ui_flags_t;

typedef enum ui_elem {
  UI_HEADER = 0,
  UI_SIDEBAR,
  UI_TRACKLIST,
  UI_LOG,
  UI_HELP,
  UI_PLAYER,
  UI_FOOTER,
  UI_END
} ui_elem_t;

typedef enum ui_set {
  UI_SET_NONE,
  UI_SET_BROWSER,
  UI_SET_LOG,
  UI_SET_HELP,
  UI_SET_END
} ui_set_t;

struct ui;
typedef void (*ui_draw_cb_t)(struct ui*);
typedef int  (*ui_keypress_cb_t)(int);

typedef struct ui {
  WINDOW       *win;
  ui_flags_t    flags;
  ui_set_t      set;

  unsigned int  width;
  unsigned int  height;
  unsigned int  fixed_width;
  unsigned int  fixed_height;

  ui_draw_cb_t  draw_cb;
  ui_keypress_cb_t keypress_cb;
} ui_t;

void stdscr_init();
void stdscr_cleanup();

void ui_init();
void ui_cleanup();

void ui_balance();
void ui_update(bool redraw);
void ui_show(ui_set_t show);
void ui_dirty(ui_elem_t dirty);
void ui_focus(ui_elem_t focus);
ui_elem_t ui_focused();

void ui_keypress(int ch);

#endif

/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_UI_FOOTER_H
#define DESPOTIFY_UI_FOOTER_H

#include "ui.h"

typedef enum input_type {
  INPUT_NONE,
  INPUT_COMMAND,
  INPUT_SEARCH,
  INPUT_USERNAME,
  INPUT_PASSWORD,
  INPUT_END
} input_type_t;

void footer_init(ui_t *ui);
void footer_draw(ui_t *ui);
int  footer_keypress(wint_t ch, bool code);
void footer_input(input_type_t type);

#endif

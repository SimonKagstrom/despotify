/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_SESSION_H
#define DESPOTIFY_SESSION_H

#include "despotify.h"

typedef enum sess_state {
  SESS_OFFLINE,
  SESS_ONLINE,
  SESS_ERROR
} sess_state_t;

typedef struct sess_search {
  struct search_result *res;
  struct sess_search   *next;
} sess_search_t;

typedef struct session {
  sess_state_t state;
  struct despotify_session *dsfy;
  char *username;
  char *password;
  // FIXME: Username/password may contain wide characters.

  sess_search_t *search;
  unsigned int   search_len;
} session_t;

void sess_init();
void sess_cleanup();

void sess_connect();
void sess_disconnect();

void sess_username(const char *username);
void sess_password(const char *password);

void sess_search(const char *query);

#endif

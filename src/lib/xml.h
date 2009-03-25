/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_XML_H
#define DESPOTIFY_XML_H

#include <stdbool.h>

struct playlist* xml_parse_playlist(struct playlist* pl,
                                         unsigned char* xml,
                                         int len,
                                         bool list_of_lists);

bool xml_parse_searchlist(struct playlist* pl,
                             unsigned char* xml,
                             int len );

bool xml_parse_artist(struct artist* a,
                           unsigned char* xml,
                           int len );

#endif

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

int xml_parse_searchlist(struct track* firsttrack,
                         unsigned char* xml,
                         int len,
                         bool ordered);

bool xml_parse_artist(struct artist* a,
                      unsigned char* xml,
                      int len );

#endif

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

bool xml_parse_artist(struct artist* a, unsigned char* xml, int len );
bool xml_parse_album(struct album* a, unsigned char* xml, int len );

void xml_free_playlist(struct playlist* pl);
void xml_free_track(struct track* head);
void xml_free_artist(struct artist* artist);
void xml_free_album(struct album* album);

#endif

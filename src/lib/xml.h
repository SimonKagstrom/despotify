/*
 * $Id$
 *
 */

#ifndef DESPOTIFY_XML_H
#define DESPOTIFY_XML_H

#include <stdbool.h>

struct ds_playlist* xml_parse_playlist(struct ds_playlist* pl,
                                         unsigned char* xml,
                                         int len,
                                         bool list_of_lists);

bool xml_parse_confirm(struct ds_playlist* pl,
                       unsigned char* xml,
                       int len);

int xml_parse_search(struct ds_search_result* search,
                     struct ds_track* firsttrack,
                     unsigned char* xml,
                     int len,
                     bool high_bitrate);

int xml_parse_tracklist(struct ds_track* firsttrack,
                        unsigned char* xml,
                        int len,
                        bool ordered,
                        bool high_bitrate);

bool xml_parse_browse_artist(struct ds_artist_browse* a,
                             unsigned char* xml,
                             int len,
                             bool high_bitrate);
bool xml_parse_browse_album(struct ds_album_browse* a,
                            unsigned char* xml,
                            int len,
                            bool high_bitrate);

void xml_free_playlist(struct ds_playlist* pl);
void xml_free_track(struct ds_track* head);
void xml_free_artist(struct ds_artist* artist);
void xml_free_artist_browse(struct ds_artist_browse* artist);
void xml_free_album(struct ds_album* album);
void xml_free_album_browse(struct ds_album_browse* album);

void xml_parse_prodinfo(struct ds_user_info* u, unsigned char* xml, int len);

char* xml_gen_tag(char* name, char* content);

#endif

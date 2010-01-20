#ifndef _DESPOTIFY_CACHE_H_
#define _DESPOTIFY_CACHE_H_

#include <stdbool.h>

bool cache_init();
void cache_clear();
bool cache_contains(unsigned char *id);
unsigned char *cache_load(unsigned char *id, unsigned int *size);
void cache_remove(unsigned char *id);
void cache_store(unsigned char *id, unsigned char *data, unsigned int size);

unsigned int cache_get_meta_playlist_revision();
void cache_set_meta_playlist_revision(unsigned int revision);

#endif /* _DESPOTIFY_CACHE_H_ */


#ifndef DESPOTIFY_SORT_H
#define DESPOTIFY_SORT_H

struct ds_track;
struct ds_track *tracklist_sort(struct ds_track *list);
void tracklist_dedup(struct ds_track *list);

#endif

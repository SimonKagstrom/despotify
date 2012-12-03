#include <string.h>
#include "despotify.h"
#include "sort.h"

static int track_compare(struct ds_track *t1, struct ds_track *t2)
{
    if (!t1 || !t2)
        return 0;

    int rv = 
        strcmp(t1->artist->name, t2->artist->name) * 1000 +
        strcmp(t1->album, t2->album) * 10;

    if (!rv)
        rv = t1->tracknumber - t2->tracknumber;

    return rv;
}

int tracklist_dedup(struct ds_track *list)
{
    int deletions = 0;
    while (list) {
        if (!track_compare(list, list->next)) {
            struct ds_track *dupe = list->next;
            if (dupe) {
                list->next = dupe->next;
                dupe->next = NULL;
                despotify_free_track(dupe);
                deletions++;
            }
        }
        list = list->next;
    }

    return deletions;
}

/*
 * The mergesort below originally from the following link, with minor changes
 *  http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */

/*
 * This file is copyright 2001 Simon Tatham.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

struct ds_track *tracklist_sort(struct ds_track *list)
{
    struct ds_track *p, *q, *e, *tail;
    int insize, nmerges, psize, qsize, i;

    if (!list)
        return NULL;

    insize = 1;

    while (1) {
        p = list;
        list = NULL;
        tail = NULL;

        nmerges = 0;

        while (p) {
            nmerges++;
            q = p;
            psize = 0;
            for (i = 0; i < insize; i++) {
                psize++;
                q = q->next;
                if (!q)
                    break;
            }

            qsize = insize;

            while (psize > 0 || (qsize > 0 && q)) {
                if (psize == 0) {
                    e = q;
                    q = q->next;
                    qsize--;
                } else if (qsize == 0 || !q) {
                    e = p;
                    p = p->next;
                    psize--;
                } else if (track_compare(p, q) <= 0) {
                    e = p;
                    p = p->next;
                    psize--;
                } else {
                    e = q;
                    q = q->next;
                    qsize--;
                }

                if (tail)
                    tail->next = e;
                else
                    list = e;
                tail = e;
            }

            p = q;
        }

        tail->next = NULL;

        if (nmerges <= 1)
            return list;

        insize *= 2;
    }
}



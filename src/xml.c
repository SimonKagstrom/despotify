/*
 * $Id: xml.c 690 2009-02-22 22:59:13Z x $
 *
 */

#include <stdio.h>
#include <string.h>

#include "xml.h"
#include "util.h"

GZXML *gzxml_init(void) {
	GZXML *g;

	g = malloc(sizeof(GZXML));
	if(g == NULL)
		return NULL;

	g->z.zalloc = Z_NULL;
	g->z.zfree = Z_NULL;
	g->z.opaque = Z_NULL;
	g->z.avail_in = 0;
	g->z.next_in = Z_NULL;
	if(inflateInit2(&g->z, -MAX_WBITS) != Z_OK) {
		DSFYfree(g);
		return NULL;
	}

	g->p = XML_ParserCreate(NULL);
	if(g->p == NULL) {
		inflateEnd(&g->z);
		DSFYfree(g);
		return NULL;
	}

	return g;
}


#define CHUNKSZ	65536
int gzxml_process(GZXML *g, void *data, int len) {
	int e;
	int ret;

	g->z.next_in = data;
	g->z.avail_in = len;

	ret = 1;
	do {
		g->z.avail_out = CHUNKSZ;
		g->z.next_out = XML_GetBuffer(g->p, g->z.avail_out);

		e = inflate(&g->z, Z_NO_FLUSH);
		if(e != Z_OK && e != Z_STREAM_END)
			break;

		if((ret = XML_ParseBuffer(g->p, CHUNKSZ - g->z.avail_out, e == Z_STREAM_END)) == 0)
			break;

	} while(g->z.avail_out == 0);

	g->z.next_in = Z_NULL;
	g->z.next_out = Z_NULL;

	if(e == Z_OK && ret == 1)
		return 0;

	/* XXX - Upper layer doesn't handle the error at the moment,
	         will crash if it's free'd before last packet */
	inflateEnd(&g->z);
	XML_ParserFree(g->p);
	g->p = NULL;

	return Z_STREAM_END - e;
}


void gzxml_free(GZXML *g) {
	if(g->p)
		XML_ParserFree(g->p);

	DSFYfree(g);
}


void xml_push_tag(struct tagstack **head, const XML_Char *tag) {
	struct tagstack *e; 

	e = malloc(sizeof(struct tagstack));
	if(*head == NULL)
		*head = e = malloc(sizeof(struct tagstack));
	else {
		for(e = *head; e->next; e = e->next);
		e->next = malloc(sizeof(struct tagstack));
		e = e->next;
	}
	e->next = NULL;
	e->name = strdup(tag);
}


void xml_pop_tag(struct tagstack **head) {
	struct tagstack *e = *head;

	if(e->next == NULL) {
		DSFYfree(e->name);
		DSFYfree(e);
		*head = NULL;
		return;
	}

	while(e->next->next)
		e = e->next;

	DSFYfree(e->next->name);
	DSFYfree(e->next);
	e->next = NULL;
}


struct tagstack *xml_has_parent_path(struct tagstack *e, char *path) {
	char foo[1024];
	char *p;

	if(!e)
		return NULL;

	p = path;
	while(*p && e) {
		*foo = 0;
		sscanf(p, "/%[^/]", foo);
		p += strlen(foo) + 1;
		if(strcmp(e->name, foo) && *p != '/')
			return NULL;

		e = e->next;
	}

	if(*p != 0 || !e || e->next)
		return NULL;

	return e;
}

/*
 * $Id: xml.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_XML_H
#define DESPOTIFY_XML_H

#include <expat.h>
#include <zlib.h>

/* For keeping track of the XML path */
struct tagstack
{
	char *name;
	struct tagstack *next;
};

void xml_push_tag (struct tagstack **, const XML_Char *);
void xml_pop_tag (struct tagstack **);
struct tagstack *xml_has_parent_path (struct tagstack *, char *);

/* For parsing compressed XML */
typedef struct
{
	z_stream z;
	XML_Parser p;
} GZXML;

GZXML *gzxml_init (void);
int gzxml_process (GZXML *, void *, int);
void gzxml_free (GZXML *);
#endif

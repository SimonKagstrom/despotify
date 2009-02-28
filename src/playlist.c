/*
 * $Id: playlist.c 698 2009-02-23 01:56:59Z x $
 *
 * Playlist related stuff
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "playlist.h"
#include "util.h"
#include "xml.h"

struct parsingctx
{
	GZXML *gzxml;
	XML_Parser p;
	struct playlist *pl;
	struct tagstack *taglist;

	/* the current track we're adding info for, point to an item in pl->tracks */
	struct track *track;

	int list_playlists;
};

static struct track *playlist_track_by_id (struct playlist *,
					   unsigned char *);
static void playlist_xml_handle_startelement (void *, const XML_Char *,
					      const XML_Char **);
static void playlist_xml_handle_endelement (void *, const XML_Char *);
static void playlist_xml_handle_text (void *, const XML_Char *, int);
static void tracks_meta_xml_handle_startelement (void *, const XML_Char *,
						 const XML_Char **);
static void tracks_meta_xml_handle_endelement (void *, const XML_Char *);
static void tracks_meta_xml_handle_text (void *, const XML_Char *, int);

static struct playlist *root;

struct playlist **playlist_root (void)
{
	return &root;
}

struct playlist *playlist_new (void)
{
	struct playlist *p;

	if ((p = malloc (sizeof (struct playlist))) == NULL)
		return NULL;

	p->flags = 0;

	p->name = NULL;
	p->author = NULL;

	p->tracks = NULL;
	p->playlist_id = NULL;
	p->num_tracks = 0;

	p->next = root;
	root = p;

	return p;
}

int playlist_set_id (struct playlist *p, unsigned char *id)
{
	if (p->playlist_id) {
		DSFYfree(p->playlist_id);
	}

	if ((p->playlist_id = malloc (17)) == NULL)
		return -1;

	memcpy (p->playlist_id, id, 17);

	return 0;
}

void playlist_set_name (struct playlist *p, char *name)
{
	if (name)
		p->name = strdup (name);
}

void playlist_set_author (struct playlist *p, char *author)
{
	if (author)
		p->author = strdup (author);
}

void playlist_free (struct playlist *p, int free_tracks)
{
	struct playlist *tmp;

	if (p->playlist_id)
		DSFYfree (p->playlist_id);

	if (p->name)
		DSFYfree (p->name);

	if (p->author)
		DSFYfree (p->author);

	if (free_tracks)
		while (p->tracks)
			playlist_track_del (p, p->tracks->track_id);

	if (p == root)
		root = p->next;
	else {
		for (tmp = root; tmp->next != p; tmp = tmp->next);
		tmp->next = p->next;
	}

	DSFYfree (p);
}

/* Mark playlist (1..N) as the currently selected playlist */
struct playlist *playlist_select (int num)
{
	struct playlist *p, *ret;

	ret = NULL;
	for (p = root; p; p = p->next) {
		p->flags &= ~PLAYLIST_SELECTED;
		if (--num != 0)
			continue;

		p->flags |= PLAYLIST_SELECTED;
		ret = p;
	}

	return ret;
}

struct playlist *playlist_selected (void)
{
	struct playlist *p;

	for (p = root; p; p = p->next)
		if (p->flags & PLAYLIST_SELECTED)
			break;

	return p;
}

struct track *playlist_track_add (struct playlist *p, unsigned char *id)
{
	struct track *t;

	if ((t = p->tracks) == NULL) {
		p->tracks = t =
			(struct track *) calloc (1, sizeof (struct track));
	}
	else {
		while (t->next)
			t = t->next;
		t->next = (struct track *) calloc (1, sizeof (struct track));
		t = t->next;
	}

	memcpy (t->track_id, id, 16);
	t->id = p->num_tracks++;

	return t;
}

void playlist_track_del (struct playlist *p, unsigned char *id)
{
	struct track *t, *bad;
	int i;

	if ((bad = playlist_track_by_id (p, id)) == NULL)
		return;

	if ((t = p->tracks) == bad)
		p->tracks = bad->next;
	else {
		while (t->next != bad)
			t = t->next;

		t->next = bad->next;
	}

	if (bad->title)
		DSFYfree (bad->title);
	if (bad->artist)
		DSFYfree (bad->artist);
	if (bad->album)
		DSFYfree (bad->album);
	if (bad->key)
		DSFYfree (bad->key);

	DSFYfree (bad);

	for (i = 0, t = p->tracks; t; i++, t = t->next)
		t->id = i;

	p->num_tracks = i;
}

static struct track *playlist_track_by_id (struct playlist *p,
					   unsigned char *id)
{
	struct track *t;

	for (t = p->tracks; t; t = t->next)
		if (!memcmp (t->track_id, id, 16))
			break;

	return t;
}

/*
 * Create playlist from XML
 *
 * If target is NULL, consider the returned items as playlists, not tracks
 *
 */
int playlist_create_from_xml (char *xmldata, struct playlist *target)
{
	struct parsingctx ctx;
	int ret;

	ctx.p = XML_ParserCreate (NULL);
	ctx.pl = target;
	ctx.gzxml = NULL;	/* Unused */
	ctx.taglist = NULL;
	ctx.track = NULL;
	ctx.list_playlists = target ? 0 : 1;

	XML_SetUserData (ctx.p, (void *) &ctx);
	XML_SetElementHandler (ctx.p, playlist_xml_handle_startelement,
			       playlist_xml_handle_endelement);
	XML_SetCharacterDataHandler (ctx.p, playlist_xml_handle_text);

	ret = XML_Parse (ctx.p, xmldata, strlen (xmldata), 1);
	XML_ParserFree (ctx.p);

	return ret;
}

/*
 * Update tracks meta data from BrowseTrack response XML
 *
 */

int playlist_track_update_from_gzxml (struct playlist *target, void *data,
				      int len)
{
	int ret;
	struct parsingctx ctx;

	ctx.gzxml = gzxml_init ();
	ctx.p = NULL;
	ctx.pl = target;
	ctx.taglist = NULL;
	ctx.track = NULL;
	ctx.list_playlists = 0;

	XML_SetUserData (ctx.gzxml->p, (void *) &ctx);
	XML_SetElementHandler (ctx.gzxml->p,
			       tracks_meta_xml_handle_startelement,
			       tracks_meta_xml_handle_endelement);
	XML_SetCharacterDataHandler (ctx.gzxml->p,
				     tracks_meta_xml_handle_text);

	ret = gzxml_process (ctx.gzxml, data, len);
	gzxml_free (ctx.gzxml);

	if (ret == 0)
		target->flags |= PLAYLIST_TRACKS_LOADED;
	else
		target->flags |= PLAYLIST_TRACKS_ERROR;

	return ret;
}

static void playlist_xml_handle_startelement (void *private,
					      const XML_Char * name,
					      const XML_Char ** attr)
{
	struct parsingctx *ctx = (struct parsingctx *) private;

	(void) attr;
	(void) name;		/* don't warn */

	xml_push_tag (&ctx->taglist, name);
}

static void playlist_xml_handle_endelement (void *private,
					    const XML_Char * name)
{
	struct parsingctx *ctx = (struct parsingctx *) private;

	/* Mark loading of playlist complete */
	if (!ctx->list_playlists
			&& xml_has_parent_path (ctx->taglist,
						"//next-change/change/ops")
			&& !strcmp (name, "create")) {
		ctx->pl->flags |= PLAYLIST_LOADED;
	}

	xml_pop_tag (&ctx->taglist);
}

static void playlist_xml_handle_text (void *private, const XML_Char * s,
				      int len)
{
	struct parsingctx *ctx = (struct parsingctx *) private;
	struct tagstack *ts = ctx->taglist;
	int i;
	char *buf, *ptr, *ptr_next;
	unsigned char id[17];
	struct playlist *p;

	buf = (char *) malloc (len + 1);
	memcpy (buf, s, len);
	buf[len] = 0;

	/* Skip whitespace nodes */
	for (i = 0; len; i++, len--)
		if (s[i] != ' ' && s[i] != '\r' && s[i] != '\n'
				&& s[i] != '\t')
			break;

	if (!len) {
		DSFYfree (buf);
		return;
	}

	if (!ctx->list_playlists
			&& (ts =
			    xml_has_parent_path (ctx->taglist,
						 "//next-change/change")) !=
			NULL && !strcmp (ts->name, "user")) {

		playlist_set_author (ctx->pl, (char *) buf);
	}
	else if (!ctx->list_playlists
			&& (ts =
			    xml_has_parent_path (ctx->taglist,
						 "//next-change/change/ops"))
			!= NULL && !strcmp (ts->name, "name")) {

		playlist_set_name (ctx->pl, (char *) buf);
	}
	else if ((ts =
		  xml_has_parent_path (ctx->taglist,
				       "//next-change/change/ops/add")) !=
		   NULL && !strcmp (ts->name, "items")) {

		ptr = buf;
		while (len > 0) {
			while (len && (*ptr == ' ' || *ptr == '\r'
				       || *ptr == '\n'))
				ptr++, len--;

			ptr_next = ptr;
			while (len && ((*ptr_next >= '0' && *ptr_next <= '9')
				       || (*ptr_next >= 'a'
					   && *ptr_next <= 'f')))
				ptr_next++, len--;

			*ptr_next++ = 0;
			len--;
			if (ptr_next - ptr == 35) {
				hex_ascii_to_bytes (ptr, id, 17);

				if (!ctx->list_playlists)
					playlist_track_add (ctx->pl, id);
				else {
					p = playlist_new ();
					playlist_set_id (p, id);
				}
			}

			ptr = ptr_next;
		}
	}

	DSFYfree (buf);
}

static void tracks_meta_xml_handle_startelement (void *private,
						 const XML_Char * name,
						 const XML_Char ** attr)
{
	struct parsingctx *ctx = (struct parsingctx *) private;
	int i;

	xml_push_tag (&ctx->taglist, name);

	if (xml_has_parent_path (ctx->taglist, "//tracks/track/files")
			&& !strcmp (name, "file")) {
		for (i = 0; attr[i]; i += 2) {
			if (strcmp (attr[i], "id"))
				continue;

			if (ctx->track)
				hex_ascii_to_bytes ((char *) attr[i + 1],
						    ctx->track->file_id, 20);
			break;
		}
	}
}

static void tracks_meta_xml_handle_endelement (void *private,
					       const XML_Char * name)
{
	struct parsingctx *ctx = (struct parsingctx *) private;
	static unsigned char invalid_file_id[20];

	(void) name;		/* don't warn. */

	if (xml_has_parent_path (ctx->taglist, "//tracks")) {
		/* Remove unplayable tracks from playlist */
		if (ctx->track) {
			ctx->track->has_meta_data = 1;

			if (!memcmp (ctx->track->file_id, invalid_file_id,
				     20))
				playlist_track_del (ctx->pl,
						    ctx->track->track_id);
		}

		ctx->track = NULL;
	}

	xml_pop_tag (&ctx->taglist);
}

static void tracks_meta_xml_handle_text (void *private, const XML_Char * s,
					 int len)
{
	struct parsingctx *ctx = (struct parsingctx *) private;
	struct tagstack *ts = ctx->taglist;
	int i;
	char *buf;
	unsigned char id[17];

	buf = (char *) malloc (len + 1);
	memcpy (buf, s, len);
	buf[len] = 0;

	/* Skip whitespace nodes */
	for (i = 0; len; i++, len--)
		if (s[i] != ' ' && s[i] != '\r' && s[i] != '\n'
				&& s[i] != '\t')
			break;

	if (!len) {
		DSFYfree (buf);
		return;
	}

	if ((ts = xml_has_parent_path (ts, "//tracks/track")) != NULL) {
		if (!strcmp (ts->name, "id")) {
			hex_ascii_to_bytes (buf, id, strlen (buf) / 2);
			if ((ctx->track =
			     playlist_track_by_id (ctx->pl, id)) == NULL) {
				/* Create track since it wasn't previously found */
				ctx->track = playlist_track_add (ctx->pl, id);
			}
		}
		else if (ctx->track) {
			if (!strcmp (ts->name, "artist"))
				ctx->track->artist = strdup (buf);
			else if (!strcmp (ts->name, "title"))
				ctx->track->title = strdup (buf);
			else if (!strcmp (ts->name, "album"))
				ctx->track->album = strdup (buf);
			else if (!strcmp (ts->name, "length"))
				ctx->track->length = atoi (buf);
		}
	}

	DSFYfree (buf);
}

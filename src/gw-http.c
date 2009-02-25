/*
 * $Id: gw-http.c 511 2009-02-09 00:10:34Z x $
 * Process HTTP requests
 *
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "base64.h"
#include "buffer.h"
#include "gw.h"
#include "gw-http.h"


static int http_reply(RESTSESSION *, int, BUFFER *);
static int http_reply_need_auth(RESTSESSION *);
static void http_cleanup(RESTSESSION *r);
static int http_complete_login(RESTSESSION *);


static char content_type[] = "Content-Type: text/html; charset=utf-8\r\n";
static char connection_close[] = "Connection: close\r\n";


int http_handle_request(RESTSESSION *r) {
	BUFFER *b;
	char buf[512];
	char *p;
	SPOTIFYSESSION *client;

	/*
	 * r->httpreq->url has path that was requested
	 * r->httpreq->authheader MIGHT be non-NULL and 
	 * have username:password in base64
	 *
	 */



	/* Default to process next command */
	r->state = REST_STATE_LOAD_COMMAND;


	if((client = spotify_find_http_client()) == NULL) {
		/* Force auth if not sent or likely invalid */
		if(!r->httpreq->authheader || strlen(r->httpreq->authheader) > 100)
			return http_reply_need_auth(r);


		memset(buf, 0, sizeof(buf));
		b64decode(r->httpreq->authheader, buf);
		if((p = strchr(buf, ':')) == NULL) {
			printf("b64 decode failed '%s'\n", buf);
			return http_reply_need_auth(r);
		}

		*p++ = 0;
		strcpy(r->username, buf);
		strcpy(r->password, p);
		spotify_client_allocate(r);
		spotify_client_mark_for_http(r->client);
		if(r->client->state == CLIENT_STATE_IDLE_CONNECTED)
			return http_complete_login(r);


		r->state = REST_STATE_WAITING;
		r->httpreq->callback = http_complete_login;
		return 0;
	}
		



	if(0) {

	}
	else {
		b = buffer_init();
		sprintf(buf, "You're logged in as '%s' with password '%s'<br />\n",
				r->username, r->password);
		if(client)
			buffer_append_raw(b, buf, strlen(buf));
		sprintf(buf, "The requested URL '%s' was not found!\n", r->httpreq->url);
		buffer_append_raw(b, buf, strlen(buf));
		return http_reply(r, 404, b);
	}

	return 0;
}


static void http_cleanup(RESTSESSION *r) {
	if(r->httpreq->authheader)
		free(r->httpreq->authheader);
	
	free(r->httpreq->url);
	free(r->httpreq);
	r->httpreq = NULL;
}


static int http_reply_need_auth(RESTSESSION *r) {
	BUFFER *b;
	int ret;
	char buf[256];

	b = buffer_init();
	strcpy(buf, "HTTP/1.1 401\r\n");
	buffer_append_raw(b, buf, strlen(buf));
	strcpy(buf, "WWW-Authenticate: Basic realm=\"Spotify\"\r\n");
	buffer_append_raw(b, buf, strlen(buf));
	buffer_append_raw(b, content_type, strlen(content_type));
	buffer_append_raw(b, connection_close, strlen(connection_close));
	strcpy(buf, "Content-Length: 0\r\n");
	buffer_append_raw(b, buf, strlen(buf));
	buffer_append_raw(b, "\r\n", 2);

	ret = 0;
	if(write(r->socket, b->buf, b->buflen) != b->buflen)
		ret = -1;

	buffer_free(b);
	http_cleanup(r);

	return ret;
}



static int http_reply(RESTSESSION *r, int status, BUFFER *response) {
	BUFFER *b;
	int ret;
	char respcode[100];
	char content_len[256];

	b = buffer_init();
	sprintf(respcode, "HTTP/1.1 %03d\r\n", status);
	buffer_append_raw(b, respcode, strlen(respcode));
	buffer_append_raw(b, content_type, strlen(content_type));
	buffer_append_raw(b, connection_close, strlen(connection_close));
	sprintf(content_len, "Content-Length: %d\r\n", response->buflen);
	buffer_append_raw(b, content_len, strlen(content_len));
	buffer_append_raw(b, "\r\n", 2);
	buffer_append_raw(b, response->buf, response->buflen);

	ret = 0;
	if(write(r->socket, b->buf, b->buflen) != b->buflen)
		ret = -1;

	buffer_free(b);
	buffer_free(response);
	http_cleanup(r);

	return ret;
}


static int http_complete_login(RESTSESSION *r) {
	BUFFER *response;

	if(r->client->state != CLIENT_STATE_IDLE_CONNECTED)
		return http_reply_need_auth(r);

	response = buffer_init();
	buffer_append_raw(response, "logged in!\n", 11);

	return http_reply(r, 200, response);
}

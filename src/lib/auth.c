/*
 * $Id$
 *
 * Code for dealing with authentication against
 * the server.
 *
 * Used after exchanging the two first packets to
 * exchange the next two packets.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "network.h"

#include "auth.h"
#include "buf.h"
#include "puzzle.h"
#include "util.h"
#include "sha1.h"
#include "hmac.h"

void auth_generate_auth_hash (SESSION * session)
{
	SHA1_CTX ctx;

	SHA1Init (&ctx);

	SHA1Update (&ctx, session->salt, 10);
	SHA1Update (&ctx, " ", 1);
	SHA1Update (&ctx, session->password, strlen (session->password));

	SHA1Final (session->auth_hash, &ctx);

#ifdef DEBUG_LOGIN
	hexdump8x32 ("auth_generate_auth_hash, auth_hash", session->auth_hash,
		     20);
#endif
}

int do_auth (SESSION * session)
{
	/*
	 * !! cr4zy 0pp3rtun1ty 2 s4v3 s0m3 pr3c10uz 3n3rgy !@)#&$!@#
	 *
	 * g00gl3 w4z b4d but th1z 1z w0rz3 - w4zt1ng CPU cycl3z f0r
	 * th3 s4k3 0f w4zt1ng th3m & d3l4y1ng th3 l0g1n pr0c3zz,
	 *
	 * 3d1t0rz r3m4rk:
	 *   sk1pp1n' th1z l4m3 puzzl3 w0u1d b3 4n 4w3z0m3
	 *   1Ph0n3 b4tt3ry 0pt1m1z4t10n t3kn1qu3 !!
	 *
	 * b4ckgr0und th30ry
	 *   http://google.com/search?q=aura-nikander-leiwo-protocols00.pdf
	 *   http://google.com/search?q=005-candolin.pdf
	 *
	 */
	puzzle_solve (session);

	/*
	 * Compute HMAC over random data, public keys,
	 * more random data and finally some username-
	 * related parts
	 *
	 * Key is part of a digest computed in key_init()
	 *
	 */
	auth_generate_auth_hmac (session, session->auth_hmac,
				 sizeof (session->auth_hmac));

	if (send_client_auth (session)) {
		printf ("do_auth(): send_client_auth() failed\n");
		return -1;
	}

	if (read_server_auth_response (session)) {
		printf ("do_auth(): read_server_auth_response() failed\n");
		return -1;
	}

	return 0;
}

void auth_generate_auth_hmac (SESSION * session, unsigned char *auth_hmac,
			      unsigned int mac_len)
{
        (void)mac_len;
        struct buf* buf = buf_new();
	
	buf_append_data (buf, session->client_random_16,
                         sizeof (session->client_random_16));
	buf_append_data (buf, session->server_random_16,
			   sizeof (session->server_random_16));
	buf_append_data (buf, session->my_pub_key, 96);
	buf_append_data (buf, session->remote_pub_key, 96);
	buf_append_data (buf, session->rsa_pub_exp,
			   sizeof (session->rsa_pub_exp));
	buf_append_data (buf, &session->username_len, 1);
	buf_append_data (buf, session->username, session->username_len);
	buf_append_data (buf, "\x01\x40", 2);

#ifdef DEBUG_LOGIN
	hexdump8x32 ("auth_generate_auth_hmac, HMAC message", buf->ptr,
		     buf->len);
	hexdump8x32 ("auth_generate_auth_hmac, HMAC key", session->key_hmac,
		     sizeof (session->key_hmac));
#endif

	sha1_hmac ( session->key_hmac, sizeof (session->key_hmac),
		    buf->ptr, buf->len, auth_hmac);

#ifdef DEBUG_LOGIN
	hexdump8x32 ("auth_generate_auth_hmac, HMAC digest", auth_hmac,
		     mac_len);
#endif

	buf_free(buf);
}

int send_client_auth (SESSION * session)
{
	int ret;
        struct buf* buf = buf_new();

	buf_append_data (buf, session->puzzle_solution,
                         sizeof (session->puzzle_solution));
	buf_append_data (buf, session->auth_hmac,
                         sizeof (session->auth_hmac));

	/*
	 * Unknown
	 *
	 */
	buf_append_u8 (buf, 0x0);

	/*
	 * Payload length (including length byte itself) and payload
	 * The payload can be anything and doesn't appear to be used.
	 *
	 */
	buf_append_u8(buf, 0x1); /* zero payload */

#ifdef DEBUG_LOGIN
	hexdump8x32 ("send_client_auth, second client packet", buf->ptr,
		     buf->len);
#endif

	if ((ret = sock_send (session->ap_sock, buf->ptr, buf->len)) <= 0) {
		printf ("send_client_auth(): connection lost\n");
		buf_free(buf);
		return -1;
	}
	else if (ret != buf->len) {
		printf ("send_client_auth(): only wrote %d of %d bytes\n",
			ret, buf->len);
		buf_free(buf);
		return -1;
	}

	buf_free(buf);
	
	return 0;
}

int read_server_auth_response (SESSION * session)
{
	unsigned char buf[256];
	unsigned char payload_len;
	int ret;

	if ((ret = block_read (session->ap_sock, buf, 2)) != 2) {
		printf ("read_server_initial_packet(): Failed to read 'status' + length byte, got %d bytes\n", ret);
		return -1;
	}

	if (buf[0] != 0x00) {
		printf ("read_server_auth_response(): Authentication failed with error 0x%02x, bad password?\n", buf[1]);
		return -1;
	}

	/* Payload length + this byte must not be zero(?) */
	assert (buf[1] > 0);

	payload_len = buf[1] - 1;
	if ((ret =
	     block_read (session->ap_sock, buf,
			 payload_len)) != payload_len) {
		printf ("read_server_initial_packet(): Failed to read 'payload', got %d of %u bytes\n", ret, payload_len);
		return -1;
	}

#ifdef DEBUG_LOGIN
	hexdump8x32 ("read_server_auth_response, payload", buf, payload_len);
#endif

	return 0;
}

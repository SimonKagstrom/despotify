/*
 * $Id: keyexchange.c 769 2009-02-24 20:12:52Z a $
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <assert.h>

#include <openssl/sha.h>
#include <openssl/hmac.h>

#include "auth.h"
#include "buffer.h"
#include "session.h"
#include "keyexchange.h"
#include "util.h"

int send_client_initial_packet(SESSION *);
int read_server_initial_packet(SESSION *);


/*
 * Handle the first four packets
 *
 */
int do_key_exchange(SESSION *session) {
	if(send_client_initial_packet(session)) {
		printf("do_key_exchange(): send_client_initial_packet() failed\n");
		return -1;
	}

	if(read_server_initial_packet(session)) {
		printf("do_key_exchange(): send_client_initial_packet() failed\n");
		return -1;
	}

	return 0;
}


int send_client_initial_packet(SESSION *session) {
	BUFFER *b;
	unsigned short version;
	unsigned short len;
	unsigned short *len_ptr;
	unsigned char num_random_bytes;
	int ret;

	b = buffer_init();

	version = 2;
	buffer_append_short(b, version);

	/* Default to zero, update later */
	len = 0;
	len_ptr = (unsigned short *)buffer_append_short(b, len);


	buffer_append_raw(b, &session->client_OS, 1);
	buffer_append_raw(b, session->client_id, 4);
	buffer_append_int(b, session->client_revision);

	buffer_append_raw(b, session->client_random_16, 16);

	buffer_append_raw(b, session->my_pub_key, 96);

	BN_bn2bin(session->rsa->n, session->rsa_pub_exp);
	buffer_append_raw(b, session->rsa_pub_exp, sizeof(session->rsa_pub_exp));

	buffer_append_raw(b, &session->username_len, 1);
	buffer_append_raw(b, (unsigned char *)session->username, session->username_len);
	buffer_append_raw(b, "\x01\x40", 2);


	/*
	 * Append zero or more random bytes
	 * The first byte should be 1 + length
	 *
	 */
	num_random_bytes = 1;
	buffer_append_raw(b, &num_random_bytes, 1);


	/*
	 * Update length byte
	 *
	 */
	*len_ptr = htons(b->buflen);


#ifdef DEBUG_LOGIN
	hexdump8x32("initial client packet", b->buf, b->buflen);
#endif


	if((ret = write(session->ap_sock, b->buf, b->buflen)) <= 0) {
		printf("send_client_initial_packet(): connection lost\n");
		buffer_free(b);
		return -1;
	}
	else if(ret != b->buflen) {
		printf("send_client_initial_packet(): only wrote %d of %d bytes\n", ret, b->buflen);
		buffer_free(b);
		return -1;
	}

	buffer_free(b);

	return 0;
}


int read_server_initial_packet(SESSION *session) {
	char buf[512];
	unsigned char padlen;
	int ret;

	DSFYDEBUG("read_server_initial_packet(): Reading 16 bytes..\n");
	if((ret = read(session->ap_sock, session->server_random_16, 16)) < 16) {
		DSFYDEBUG("read_server_initial_packet(): Failed to read 'server_random_16'\n");
		DSFYDEBUG("read_server_initial_packet(): Remote host was %s:%d\n", session->server_host, session->server_port);
		if(ret > 0)
			hexdump8x32("read_server_initial_packet, server_random_16", session->server_random_16, ret);
		return -1;
	}
	DSFYDEBUG("Done.. read_server_initial_packet(): Reading 16 bytes..\n");

#ifdef DEBUG_LOGIN
	hexdump8x32("read_server_initial_packet, server_random_16", session->server_random_16, ret);
#endif
	if(session->server_random_16[0] != 0 || ret != 16) {
		printf("read_server_initial_packet(): Status was 0x%02x, substatus 0x%02x: %s\n",
			session->server_random_16[0], session->server_random_16[1], 
			session->server_random_16[1] == 1? "Client upgrade required":
			session->server_random_16[1] == 3? "Non-existant user": "Unknown error");
		printf("read_server_initial_packet(): remote host is %s:%d\n", session->server_host, session->server_port);

		if(session->server_random_16[1] == 1) {
			ret = block_read(session->ap_sock, buf, sizeof(buf));
			if(ret > 0) {
				hexdump8x32("read_server_initial_packet, upgrade packet", buf, ret);

				padlen = buf[0x010b];
				buf[ret] = 0;
				printf("Upgrade URL:\n%s\n", buf + 0x10c);
			}
		}

		return -1;
	}

	if(block_read(session->ap_sock, &session->puzzle_denominator, 1) != 1) {
		printf("read_server_initial_packet(): Failed to read 'puzzle_denominator'\n");
		return -1;
	}
#ifdef DEBUG_LOGIN
	hexdump8x32("read_server_initial_packet, puzzle_denominator", &session->puzzle_denominator, 1);
#endif


	if(block_read(session->ap_sock, &session->username_len, 1) != 1) {
		printf("read_server_initial_packet(): Failed to read 'username_len'\n");
		return -1;
	}

	if(block_read(session->ap_sock, session->username, session->username_len) != session->username_len) {
		printf("read_server_initial_packet(): Failed to read 'username'\n");
		return -1;
	}
	session->username[session->username_len] = 0;
#ifdef DEBUG_LOGIN
	hexdump8x32("read_server_initial_packet, username", session->username, session->username_len);
#endif


	if(block_read(session->ap_sock, session->remote_pub_key, 96) != 96) {
		printf("read_server_initial_packet(): Failed to read 'remote_pub_key'\n");
		return -1;
	}
#ifdef DEBUG_LOGIN
	hexdump8x32("read_server_initial_packet, server pub key", session->remote_pub_key, 96);
#endif


	if((ret = block_read(session->ap_sock, session->random_256, 256)) != 256) {
		printf("read_server_initial_packet(): Failed to read 'random_256', got %d of 256 bytes\n", ret);
		return -1;
	}
#ifdef DEBUG_LOGIN
	hexdump8x32("read_server_initial_packet, random_256", session->random_256, 256);
#endif


	if(block_read(session->ap_sock, session->salt, 10) != 10) {
		printf("read_server_initial_packet(): Failed to read 'salt'\n");
		return -1;
	}
#ifdef DEBUG_LOGIN
	hexdump8x32("read_server_initial_packet, salt", session->salt, 10);
#endif

	if((ret = block_read(session->ap_sock, &padlen, 1)) != 1) {
		printf("read_server_initial_packet(): Failed to read 'padding length'\n");
		return -1;
	}
	assert(padlen > 0);
	padlen--;

	if((ret = block_read(session->ap_sock, buf, padlen)) != padlen) {
		printf("read_server_initial_packet(): Failed to read 'padding'\n");
		return -1;
	}
#ifdef DEBUG_LOGIN
	hexdump8x32("read_server_initial_packet, padding", buf, padlen);
#endif

	return 0;
}


/*
 * Initialize common crypto keys used for communication
 *
 * This step takes place after the initial two packets
 * have been exchanged.
 *
 */
void key_init(SESSION *session) {
	BIGNUM *pub_key;
	unsigned char message[53];
	unsigned char hmac_output[20 * 5];
	unsigned char *ptr, *hmac_ptr;
	unsigned int mac_len;
	int i;


	/*
	 * Compute DH shared key
	 * It's used in the call to HMAC() below
	 *
	 * It's funny how the assert() triggers every now and then.
	 * OpenSSL might be more careful with the primes than Spotify's
	 * own libtommath-based implementation is.
	 *
	 */
	pub_key = BN_bin2bn(session->remote_pub_key, 96, NULL);
	if((i = DH_compute_key(session->shared_key, pub_key, session->dh)) != 96) {
		FILE *fd = fopen("/tmp/despotify-spotify-pubkey", "w");
		fwrite(pub_key, 1, 96, fd);
		fclose(fd);
		fprintf(stderr, "Failed to compute shared key, error code %d\n", i);
		exit(1);
	}


#ifdef DEBUG_LOGIN
	hexdump8x32("key_init, my private key", session->my_priv_key, 96);
	hexdump8x32("key_init, my public key", session->my_pub_key, 96);
	hexdump8x32("key_init, remote public key", session->remote_pub_key, 96);
	hexdump8x32("key_init, shared key", session->shared_key, 96);
#endif


	/*
	 * Prepare a message to authenticate.
	 *
	 * Prior to the 19th of December 2008 Spotify happily told clients 
	 * (including ours!) almost everything it knew about a particular
	 * user, if they asked for it.
	 *
	 * Legitimate requests for this is for example when you add
	 * someone else's shared playlist.
	 *
	 * This allowed clients to see not only the last four digits of the 
	 * credit card used to subscribe to the premium service, whether
	 * the user was a paying customer or preferred commercials, but 
	 * also very interesting stuff such as the hash computed from
	 * SHA(salt || " " || password).
	 *
	 * In theory (HE HE!) this allowed any registered user to request
	 * somebody else's user data, get ahold of the hash, and then use
	 * it to authenticate as that user.
	 *
	 * Fortunately, at lest for Spotify and it's users, this is not
	 * the case anymore. (R.I.P poor misfeature)
	 *
	 * However, we urge people to change their passwords for reasons
	 * left as an exercise for the reader to figure out.
	 *
	 */
	ptr = message;
	memcpy(ptr, session->auth_hash, sizeof(session->auth_hash));
	ptr += sizeof(session->auth_hash);

	memcpy(ptr, session->client_random_16, 16);
	ptr += 16;

	memcpy(ptr, session->server_random_16, 16);
	ptr += 16;


	/*
	 * Run HMAC over the message, using the DH shared key as key
	 *
	 */
	hmac_ptr = hmac_output;
	mac_len = 20;
	for(i = 1; i <= 5; i++) {
		/*
		 * Change last byte of message to authenticate
		 *
		 */
		*ptr = i;

#ifdef DEBUG_LOGIN
		hexdump8x32("key_init, HMAC message", message, sizeof(message));
#endif
		HMAC(EVP_sha1(), session->shared_key, 96, message, sizeof(message), hmac_ptr, &mac_len); 

		/*
		 * Overwrite the 20 first bytes of the message with output from this round
		 *
		 */
		memcpy(message, hmac_ptr, 20);
		hmac_ptr += 20;
	}


	/*
	 * Use computed HMAC to setup keys for the
	 * stream cipher
	 *
	 */
	memcpy(session->key_send, hmac_output + 20, 32);
	memcpy(session->key_recv, hmac_output + 52, 32);

	shn_key(&session->shn_send, session->key_send, 32);
	shn_key(&session->shn_recv, session->key_recv, 32);
	session->key_send_IV = 0;
	session->key_recv_IV = 0;


	/*
	 * The first 20 bytes of the HMAC output is used
	 * to key another HMAC computed for the second
	 * authentication packet sent by the client.
	 *
	 */
	memcpy(session->key_hmac, hmac_output, 20);


#ifdef DEBUG_LOGIN
	hexdump8x32("key_init, key_hmac", session->key_hmac, 20);
	hexdump8x32("key_init, key_send", session->key_send, 32);
	hexdump8x32("key_init, key_recv", session->key_recv, 32);
#endif
}

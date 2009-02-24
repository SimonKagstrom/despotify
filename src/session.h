/*
 * $Id: session.h 700 2009-02-23 02:21:42Z x $
 *
 */

#ifndef DESPOTIFY_SESSION_H
#define DESPOTIFY_SESSION_H

#include <openssl/dh.h>

#include "shn.h"


struct _packethandler;
typedef struct {
	/* connection to Spotify's service */
	int ap_sock;

	/*
	 * Just for keeping track of which server
	 * the client is connected to
	 *
	 */
	char server_host[256];
	unsigned short server_port;

	/*
	 * The client we're posing as
	 *
	 */
	unsigned char client_OS;
	unsigned char client_id[4];
	unsigned int client_revision;

	/*
	 * 16 bytes of shn_encrypt() output with random key
	 *
	 */
	unsigned char client_random_16[16];
	unsigned char server_random_16[16];

	/*
	 * The the_blob[128] is sent in the initial packet 
	 * to the server and is received at offset 16 in the 
	 * cmd=0x02 packet.
	 *
	 * The client sends a 128 byte blob and
	 * the server sends a 256 byte blob.
	 *
	 */
	unsigned char the_blob[128];
	unsigned char random_256[256];

	/*
	 * Username, password, salt
	 *
	 */
	char username[256];
	unsigned char username_len;
	char password[256];
	char salt[10];

	/*
	 * Computed as SHA(salt || " " || password)
	 *
	 * Knowing somebody else's hash allows for
	 * impersonation.
	 *
	 * Spotify quit being overly informative
	 * about people's hashes 2008-12-19.
	 *
	 */
	unsigned char auth_hash[20];


	DH *dh;
	unsigned char my_priv_key[96];
	unsigned char my_pub_key[96];
	unsigned char remote_pub_key[96];
	unsigned char shared_key[96];

	/*
	 * Output from HMAC/SHA1
	 * 
	 * Used for keying HMAC() in auth_generate_auth_hmac()
	 * and for keying Shannon
	 *
	 */
	unsigned char key_hmac[20];
	unsigned char key_recv[32];
	unsigned char key_send[32];

	unsigned int key_recv_IV;
	unsigned int key_send_IV;

	/*
	 * Stream cipher contexts
	 *
	 */
	shn_ctx shn_recv;
	shn_ctx shn_send;

	/*
	 * Waste some CPU time while computing
	 * a 32-bit value, that byteswapped and
	 * XOR'ed with a magic, modulus
	 * 2^puzzle_denominator becomes zero.
	 *
	 */
	unsigned char puzzle_denominator;
	unsigned char puzzle_solution[8];

	/*
	 * HMAC over random data, public keys
	 * and username.
	 * The key used is key_hmac[]
	 *
	 */
	unsigned char auth_hmac[20];


	unsigned char cache_hash[20];

	/* Assigned country */
	char country[4];


	struct _packethandler *packet_handlers;

} SESSION;


SESSION *session_init_client(void);
void session_auth_set(SESSION *, char *, char *);
int session_connect(SESSION *);
SESSION *session_init_client(void);
void session_disconnect(SESSION *);
void session_free(SESSION *c);
#endif

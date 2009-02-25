/*
 * $Id: session.c 687 2009-02-22 21:37:50Z w $
 *
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>

#include <openssl/rand.h>

#include "dns.h"
#include "keyexchange.h"
#include "session.h"
#include "packet.h"


static unsigned char DH_generator[1] = { 2 };
static unsigned char DH_prime[] = {
	/* Well-known Group 1, 768-bit prime */
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc9,
	0x0f, 0xda, 0xa2, 0x21, 0x68, 0xc2, 0x34, 0xc4, 0xc6,
	0x62, 0x8b, 0x80, 0xdc, 0x1c, 0xd1, 0x29, 0x02, 0x4e,
	0x08, 0x8a, 0x67, 0xcc, 0x74, 0x02, 0x0b, 0xbe, 0xa6,
	0x3b, 0x13, 0x9b, 0x22, 0x51, 0x4a, 0x08, 0x79, 0x8e,
	0x34, 0x04, 0xdd, 0xef, 0x95, 0x19, 0xb3, 0xcd, 0x3a,
	0x43, 0x1b, 0x30, 0x2b, 0x0a, 0x6d, 0xf2, 0x5f, 0x14,
	0x37, 0x4f, 0xe1, 0x35, 0x6d, 0x6d, 0x51, 0xc2, 0x45,
	0xe4, 0x85, 0xb5, 0x76, 0x62, 0x5e, 0x7e, 0xc6, 0xf4,
	0x4c, 0x42, 0xe9, 0xa6, 0x3a, 0x36, 0x20, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};


SESSION *session_init_client(void) {
	SESSION *session;

	if((session = (SESSION *)calloc(1, sizeof(SESSION))) == NULL)
		return NULL;

	/* Spotify 0.3.11, r42849 on Windows */
	session->client_OS = 0x00;	/* 0x00 == Windows, 0x01 == Mac OS X */
	memcpy(session->client_id, "\x01\x09\x10\x01", 4);
	session->client_revision = 42974;	/* 0.3.11 testing, r42849 */


	/*
	 * Client and server generate 16 random bytes each.
	 */
	RAND_bytes(session->client_random_16,16);

	/*
	 * We've have left this as a friendly backdoor to allow
	 * Spotify to lock out 'free/ad'-users using our code. 
	 * We know how it's generated, and if its used to lock out
	 * premium users we will release code for this as well.
	 *
	 * Try us...
	 *
	 */
	memcpy(session->the_blob,
		"\x89\xdf\x70\xa6\x57\x6e\xe4\x11"
		"\xc0\xe4\x38\xe2\x3e\x9f\x3d\x2e"
		"\x3e\x7a\x25\x35\x70\x3c\xed\xaa"
		"\x27\x28\xf1\x56\x1e\xab\x6e\x39"
		"\x98\x9b\xa6\xad\xe0\x03\x69\x77"
		"\x08\x53\x82\xe5\x9a\xbb\x99\x4d"
		"\xac\x67\xea\xce\x29\x2a\x51\x9a"
		"\xdd\x1e\x2f\xbe\x4e\x46\x8b\x0f"
		"\xdc\x6f\x87\x62\xeb\x15\x0c\xae"
		"\x36\xc4\x0c\x18\x78\xbe\xd3\xda"
		"\xc7\x1a\x8f\xfd\x97\xfa\x24\x61"
		"\x77\xdd\x9b\x75\x84\xa7\x72\xf0"
		"\xd2\x83\x70\x88\x99\x69\xcd\xec"
		"\x98\xc6\x36\xe4\x8e\x71\x08\xb3"
		"\x26\xee\x0e\xc8\x8a\xca\x41\x4d"
		"\xb0\x88\x8e\x4f\x9b\x5a\xcc\x29", 128);


	/*
	 * Create a private and public key.
	 * This, along with key signing, is used to securely
	 * agree on a session key for the Shannon stream cipher.
	 *
	 */
	session->dh = DH_new();
	session->dh->p = BN_bin2bn(DH_prime, 96, NULL);
	session->dh->g = BN_bin2bn(DH_generator, 1, NULL);
	assert(DH_generate_key(session->dh) == 1);

	BN_bn2bin(session->dh->priv_key, session->my_priv_key);
	BN_bn2bin(session->dh->pub_key, session->my_pub_key);


	/*
	 * Found in Storage.dat (cache) at offset 16.
	 * Automatically generated, but we're lazy.
	 *
	 */
	memcpy(session->cache_hash, "\xf4\xc2\xaa\x05\xe8\x25\xa7\xb5\xe4\xe6\x59\x0f\x3d\xd0\xbe\x0a\xef\x20\x51\x95", 20);
	session->cache_hash[0] = (unsigned char)getpid();

	session->ap_sock = -1;
	session->username[0] = 0;
	session->country[0] = 0;
	session->server_host[0] = 0;
	session->server_port = 0;

	session->key_recv_IV = 0;
	session->key_send_IV = 0;

	return session;
}


void session_auth_set(SESSION *session, char *username, char *password) {
	strncpy(session->username, username, sizeof(session->username) - 1);
	session->username[sizeof(session->username) - 1] = 0;
	session->username_len = strlen(session->username);

	strncpy(session->password, password, sizeof(session->password) - 1);
	session->password[sizeof(session->password) - 1] = 0;
}

int session_connect(SESSION *session) {
	struct sockaddr_in sin;
	char host[1025 + 1], *service_list, *service;
	int port;


	/* Lookup service hosts in DNS */
	if((service_list = dns_srv_list("_spotify-client._tcp.spotify.com")) == NULL)
		return -1;


	for(service = service_list; *service; ) {
		if(sscanf(service, "%[^:]:%d\n", host, &port) != 2)
			return -1;

		service += strlen(host) + 7;


		memset(&sin, 0, sizeof(sin));
		sin.sin_family = PF_INET;
		sin.sin_port = htons(port);
		sin.sin_addr.s_addr = dns_resolve_name(host);
		if(sin.sin_addr.s_addr == INADDR_NONE)
			continue;


		session->ap_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(connect(session->ap_sock, (struct sockaddr *)&sin, sizeof(sin)) != -1)
			break;


		close(session->ap_sock);
		session->ap_sock = -1;
	}

	free(service_list);
	if(sin.sin_addr.s_addr == INADDR_NONE)
		return -1;


	/*
	 * Save for later use in ConnectionInfo message
	 * (too lazy to do getpeername() later ;)
	 */
	strncpy(session->server_host, host, sizeof(session->server_host) - 1);
	session->server_host[sizeof(session->server_host) - 1] = 0;
	session->server_port = port;


	return 0;
}


void session_disconnect(SESSION *session) {
	if(session->ap_sock != 0) {
		close(session->ap_sock);
		session->ap_sock = -1;
	}

	session->key_recv_IV = 0;
	session->key_send_IV = 0;
	session->country[0] = 0;
}


void session_free(SESSION *session) {
	session_disconnect(session);

	if(session->dh)
		DH_free(session->dh);


	free(session);
}

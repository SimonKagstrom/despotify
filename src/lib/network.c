/*
 * $Id$
 *
 * Cross platform networking for despotify
 *
 */
 
#include <stdlib.h>
#include <unistd.h>

#include "network.h"
 
int network_init (void)
{
	#ifdef __use_winsock__
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
		fprintf (stderr, "Winsock failed. \n");
		return -1;
	}
	#endif
	return 0;
}
int network_cleanup (void)
{
	#ifdef __use_winsock__
	return WSACleanup();
	#endif
	return 0;
}

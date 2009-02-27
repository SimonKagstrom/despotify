/*
 * $Id: dns.c 722 2009-02-23 14:14:19Z x $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <arpa/nameser_compat.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "dns.h"

static int initialized;

/*
 * Return a string with all SRV records for a given hostname
 * Example: foo.example.net:42\nrazor.example.net:1911\n
 *
 */
char *dns_srv_list (char *hostname)
{
	unsigned char answer[1024], *p;
	char host[1024];
	int alen, i, j, n_srv, lowest_prio, hlen;
	unsigned short atype, prio, weight, port;
	char *svr_list = NULL;
	HEADER *h = (HEADER *) answer;
	char *svr[10];
	unsigned short svr_prio[10];
	unsigned short svr_port[10];

	if (!initialized++)
		res_init ();

	if ((alen =
	     res_search (hostname, ns_c_in, ns_t_srv, answer,
			 sizeof (answer))) < 0)
		return NULL;

	p = answer + NS_HFIXEDSZ;
	h->qdcount = ntohs (h->qdcount);
	h->ancount = ntohs (h->ancount);
	while (h->qdcount--) {
		if ((hlen = dn_skipname (p, answer + alen)) < 0)
			return NULL;
		p += hlen + NS_QFIXEDSZ;
	}

	n_srv = 0;
	while (h->ancount-- && n_srv < (int) (sizeof (svr) / sizeof (svr[0]))) {
		if ((hlen =
		     dn_expand (answer, answer + alen, p, host,
				sizeof (host))) < 0)
			break;

		p += hlen;
		GETSHORT (atype, p);

		p += 6;
		GETSHORT (hlen, p);
		if (atype != ns_t_srv) {
			p += hlen;
			continue;
		}

		GETSHORT (prio, p);
		GETSHORT (weight, p);
		GETSHORT (port, p);
		if ((hlen =
		     dn_expand (answer, answer + alen, p, host,
				sizeof (host))) < 0)
			break;

		p += hlen;

		svr[n_srv] = strdup (host);
		svr_prio[n_srv] = prio;
		svr_port[n_srv] = port;
		n_srv++;
	}

	lowest_prio = 0;
	for (i = 0; i < n_srv; i++) {
		for (j = 0; j < n_srv; j++)
			if (svr_prio[lowest_prio] > svr_prio[j])
				lowest_prio = j;
		svr_list =
			realloc (svr_list, (svr_list ? strlen (svr_list) : 0)
				 + strlen (svr[lowest_prio]) + 6 + 2);
		if (!i)
			sprintf (svr_list, "%s:%05d\n", svr[lowest_prio],
				 svr_port[lowest_prio]);
		else
			sprintf (svr_list + strlen (svr_list), "%s:%d\n",
				 svr[lowest_prio], svr_port[lowest_prio]);

		svr_prio[lowest_prio] = (unsigned short) ~0;
		free (svr[lowest_prio]);
	}

	return svr_list;
}

/*
 * Return the IP adress of a hostname, in network byte order
 *
 */
in_addr_t dns_resolve_name (char *hostname)
{
	struct hostent *he;
	in_addr_t ip;

	if ((ip = inet_addr (hostname)) == INADDR_NONE) {
		if ((he = gethostbyname (hostname)) != NULL)
			memcpy (&ip, he->h_addr_list[0], 4);
	}

	return ip;
}

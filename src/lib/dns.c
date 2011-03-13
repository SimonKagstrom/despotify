/*
 * $Id$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "network.h"
#ifdef __use_winsock__
#include <windns.h>
#elif defined __use_posix__
#include <arpa/nameser.h>
#include <arpa/nameser_compat.h>
#include <resolv.h>
#endif

#include "dns.h"

static int initialized;

#include <sys/types.h>
#include <features.h>
#include <resolv.h>

/* taken from OpenBSD source */
/*
 * ++Copyright++ 1985, 1993
 * -
 * Copyright (c) 1985, 1993
 *    The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

/*
 * Skip over a compressed domain name. Return the size or -1.
 */
int
__dn_skipname(const u_char *comp_dn, const u_char *eom)
{
       const u_char *cp;
       int n;

       cp = comp_dn;
       while (cp < eom && (n = *cp++)) {
               /*
                * check for indirection
                */
               switch (n & INDIR_MASK) {
               case 0:                 /* normal case, n == len */
                       cp += n;
                       continue;
               case INDIR_MASK:        /* indirection */
                       cp++;
                       break;
               default:                /* illegal type */
                       return (-1);
               }
               break;
       }
       if (cp > eom)
               return (-1);
       return (cp - comp_dn);
}

/*
 * Return a string with all SRV records for a given hostname
 * Example: foo.example.net:42\nrazor.example.net:1911\n
 *
 */
char *dns_srv_list (char *hostname)
{
	char *svr_list = NULL;
    int i,j;
	char *svr[10];
	unsigned short svr_prio[10];
	unsigned short svr_port[10];
	int n_srv, lowest_prio;
	
	#ifdef __use_posix__
	int alen, hlen;
	char host[1024];
	unsigned char answer[1024], *p;
	unsigned short atype, prio, weight, port;
	HEADER *h = (HEADER *) answer;
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
    #elif defined __use_winsock__
	PDNS_RECORD pRoot = NULL, p;

	if (DnsQuery_A(hostname, DNS_TYPE_SRV, DNS_QUERY_STANDARD, NULL, &pRoot, NULL) != 0)
		return NULL;

	i = 0;
	for (p = pRoot; pt != NULL && i < 10; p = p->pNext) {
		if(p->wType != DNS_TYPE_SRV)
			continue;

		svr[i] = strdup (p->Data.SRV.pNameTarget);
		svr_prio[i] = p->Data.SRV.wPriority;
		svr_port[i++] = p->Data.SRV.wPort;
	}

	n_srv = i;
	DnsRecordListFree(pRoot, DnsFreeRecordListDeep);

    #endif

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

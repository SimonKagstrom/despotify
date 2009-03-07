/*
 * $Id: hmac.h 100 2009-03-01 13:05:35Z jorgenpt $
 *
 */

#ifndef DESPOTIFY_HMAC_H
#define DESPOTIFY_HMAC_H

void sha1_hmac(unsigned char *inputkey, size_t inputkeylen,
               unsigned char *inputmsg, size_t msglen,
               unsigned char *dst);
#endif

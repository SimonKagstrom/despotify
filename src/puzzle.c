/*
 * $Id: puzzle.c 733 2009-02-23 18:16:17Z x $
 *
 * Zero-modulus bruteforce puzzle to prevent 
 * Denial of Service and password bruteforce attacks
 *
 */

#include <stdlib.h>
#include <netinet/in.h>

#include <openssl/sha.h>
#include <openssl/hmac.h>

#include "puzzle.h"
#include "session.h"
#include "util.h"

void puzzle_solve (SESSION * session)
{
	SHA_CTX ctx;
	unsigned char digest[20];
	unsigned int *nominator_from_hash;
	unsigned int denominator;
	int i;

	/*
	 * Modulus operation by a power of two.
	 * "Most programmers learn this trick early"
	 * Well, fuck me. I'm just here for the party.
	 *
	 */
	denominator = 1 << session->puzzle_denominator;
	denominator--;

	/*
	 * Compute a hash over random data until
	 * (last dword byteswapped XOR magic number) mod
	 * denominator by server produces zero.
	 *
	 */

	srandom (*(unsigned int *) &ctx);
	nominator_from_hash = (unsigned int *) (digest + 16);
	do {
		SHA1_Init (&ctx);
		SHA1_Update (&ctx, session->server_random_16, 16);

		/* Let's waste some precious pseudorandomness */
		for (i = 0; i < 8; i++)
			session->puzzle_solution[i] = random ();
		SHA1_Update (&ctx, session->puzzle_solution, 8);

		SHA1_Final (digest, &ctx);

		/* byteswap (XXX - htonl() won't work on bigendian machines!) */
		*nominator_from_hash = htonl (*nominator_from_hash);

		/* XOR with a fancy magic */
		*nominator_from_hash ^= 0xb9671267;
	} while (*nominator_from_hash & denominator);

#ifdef DEBUG_LOGIN
	hexdump8x32 ("auth_solve_puzzle, puzzle_solution",
		     session->puzzle_solution, 8);
#endif
}

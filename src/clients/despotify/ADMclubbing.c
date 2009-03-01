/*
 * $Id$
 * 
 * Below is the most awesome piece of C code created, ever.
 * Modified to fit despotify.
 *
 */

/* 
clubbing.c 
for all the geek clubber out there who miss the lasers !
-dek
-antilove

TODO:
 - Add Synchronisation with the rythm

USAGE:
./a.out <number of dancer you want>

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DELTA 100000
#define MIN_SLEEP 100000

static int PARTY_PPL = 5;

static char *people[] = { "_o/", "_o_", "\\o_", "\\\\o",
	"o//", "\\o/", "<o_", "_o>",
	"(o/", "\\o)", "<o/", "\\o>",
	"<o>", "(o)"
};

static char *people_leg[] =
	{ "/ \\", "/ /", "| \\", "/ |", "\\ \\", "< \\", "< >", "/ >" };

static char *laser[] = { "~~", "^~", "_-_-", "++", "~~~", "-" };

static char *color[] =
	{ "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m",
	"\033[36m", "\033[37m", "\033[1m", "\033[05m"
};

/*

^^^^~~~~~^^^^~~~~~ <== laser
  \o/          
   |                 <== dancer 
  / |
  
*/

void ADMclubbing (void)
{
	unsigned int seed;
	int fd;
	int i;
	int rounds = 0;
	fd = open ("/dev/urandom", O_RDONLY);
	if (fd != -1) {
		read (fd, &seed, sizeof (unsigned int));
		close (fd);
	}
	else {
		(void) printf
			("You may have already seen this dance before...\n");
		seed = 1234;
	}
	srand (seed);

	printf ("HI! What's Your Name ? , Want To get High ?!, REACH FOR THE LASERS !!! , SAFE AS FUCK!!!!\n\n");

	while (1) {
		for (fd = 0; fd < (PARTY_PPL * 3); fd++) {
			(void) printf (color
				       [rand () %
					(sizeof (color) / sizeof (char *))]);
			(void) printf ("%s",
				       laser[rand () %
					     (sizeof (laser) /
					      sizeof (char *))]);
		}
		(void) printf ("\033[0m\n");

		for (i = 0; i < PARTY_PPL; i++)
			(void) printf ("   %s ",
				       people[rand () %
					      (sizeof (people) /
					       sizeof (char *))]);
		(void) printf ("\n");

		for (i = 0; i < PARTY_PPL; i++)
			(void) printf ("    |  ");
		(void) printf ("\n");
		for (i = 0; i < PARTY_PPL; i++)
			(void) printf ("   %s ",
				       people_leg[rand () %
						  (sizeof (people_leg) /
						   sizeof (char *))]);
		(void) printf ("\n");
		(void) fflush (stdout);
		if (++rounds == 25) {
			printf ("\n");
			break;
		}
		(void) printf ("\033[Am\033[Am\033[Am\033[Am\r");
		usleep (MIN_SLEEP + rand () % DELTA);
	}

}

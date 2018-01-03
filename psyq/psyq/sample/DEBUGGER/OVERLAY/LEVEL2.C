/***********************************************/
/*                                             */
/* Source file for Level 2 Overlay code & data */
/*                                             */
/***********************************************/

#include "common.h"

/* level's private variables */

static int var = ITERATIONS;
static char c[ITERATIONS];

/* level's initialisation routine */

void init_level_2 ( void )
{
	int i;

	msg("\nLevel 2 Init\n");

	for (i = 0; i < ITERATIONS; i++)
		c[i] = (char) i;
}

/* level's main body */

void body_level_2 ( void )
{
	msg("B2");

	var--;
	
	if (c[var] & 1)
		common2();
	else if (c[var] == 4)
		game_over = 1;
	else if (var == 0)
		level_over = 1;
}



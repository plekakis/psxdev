/***********************************************/
/*                                             */
/* Source file for Level 3 Overlay code & data */
/*                                             */
/***********************************************/

#include "common.h"		/* common defines & external var declarations */

/* level's private variables */

static int my_var;

/* level's initialisation routine */

void init_level_3 ( void )
{
	msg("\nLevel 3 Init\n");

	common1();
	my_var = ITERATIONS;
}

/* level's main body */

void body_level_3 ( void )
{
	msg("B3");

	my_var--;

	if (my_var)
		common2();
	else
		game_over = 1;
}




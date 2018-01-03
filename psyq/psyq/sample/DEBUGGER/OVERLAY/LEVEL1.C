/***********************************************/
/*                                             */
/* Source file for Level 1 Overlay code & data */
/*                                             */
/***********************************************/

#include "common.h"

/* level's private variables */

static int my_var;

/* level's initialisation routine */

void init_level_1 ( void )
{
	msg("\nLevel 1 Init\n");
	my_var = ITERATIONS;
}

/* level's main body */

void body_level_1 ( void )
{
	int	reg1=1,reg2=2;
	
	register struct t{
		int	spango;
	} regstruc;

	struct s{
		int	gs1;
		int	gs2;
		short	gs3;
		char *ptr;
	} gamestruc;

	gamestruc.ptr="B1";
	regstruc.spango=45;

	msg(gamestruc.ptr);

	gamestruc.gs1=0x12345678;
	gamestruc.gs2=0x87654321;
	gamestruc.gs2=0x5A5A;

	for(reg1=0;reg1<100;reg1++){
		reg2--;
		regstruc.spango++;
	}

	my_var--;

	if (my_var)
		common2();
	else
		level_over = 1;
}



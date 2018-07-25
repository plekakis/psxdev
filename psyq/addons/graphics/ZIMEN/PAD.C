/* $PSLibId: Runtime Library Release 3.6$ */
/*				pad
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Jun,20,1995	suzu	
 *
 *		update geometry environment
 */
#include "sys.h"

int padRead(GEOMENV *genv)
{
	/* read PAD */
	u_long	padd = PadRead(1);	
	
	/* rotate in x direction (right and left) */
	if (padd & PADRup)	genv->angle.vx += -8;
	if (padd & PADRdown)	genv->angle.vx +=  8;
	
	/* rotate in z direction (up and down) */
	if (padd & PADRleft) 	genv->angle.vz += -8;
	if (padd & PADRright)	genv->angle.vz +=  8;
	
	/* rotate in y direction */
	if (padd & PADR1)	genv->angle.vy += 16;
	if (padd & PADR2) 	genv->angle.vy += -16;
	
	/* move in x direction (right and left) */
	if (padd & PADLleft)	genv->dvs.vx =  16;
	if (padd & PADLright) 	genv->dvs.vx = -16;

	/* move in z direction (foreward and backward) */
	/*genv->dvs.vz = 32;*/
	if (padd & PADLup)	genv->dvs.vz += 64/*32*/;
	if (padd & PADLdown) 	genv->dvs.vz = -64/*-32*/;
	
	/* move in the world z position (up and down) */
	if (padd & PADL1)	genv->home.vz +=  8;
	if (padd & PADL2) 	genv->home.vz += -8;
}

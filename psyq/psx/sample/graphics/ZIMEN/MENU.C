/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *		      multi-purpose simple menu 
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00		1995/08/28	suzu
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

static int	first = 1, sx = 80, sy = 64, id = 0;

void menuInit(int x, int y)
{
	if (first) {
		first = 0;
		FntLoad(960, 256);	
		id = FntOpen((sx = x), (sy = y), 0, 0, 2, 512);	
	}
}
	
void menuFlush(void)
{
	first = 1;
}

int menuUpdate(char *title, char **path, u_long padd)
{
	static int	cp = 0, opadd = 0;
	int i, ispress = 0;
	
	if (first) {
		first = 0;
		FntLoad(960, 256);	
		id = FntOpen(sx, sy, 0, 0, 2, 512);	
	}
	
	FntPrint(id, "%s\n\n", title);
	for (i = 0; path[i]; i++) {
		if (i == cp)
			FntPrint(id, "~c888 %s\n", path[i]);
		else
			FntPrint(id, "~c444 %s \n", path[i]);
	}
	if (opadd == 0 && (padd&PADLup))    cp--;
	if (opadd == 0 && (padd&PADLdown))  cp++;
	if (opadd == 0 && (padd&PADRdown))  ispress = 1;
	if (opadd == 0 && (padd&PADRright)) ispress = 1;
	
	limitRange(cp, 0, i-1);
	opadd = padd;
	FntFlush(id);
	return(ispress? cp: -1);
}

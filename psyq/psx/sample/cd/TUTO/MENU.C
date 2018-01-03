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
 *	1.10		1996/03/29	suzu
*/
/*	
 *--------------------------------------------------------------------------
 * menuInit	initialize menu system
 *
 * SYNOPSIS
 *	void menuInit(int mode, int x, int y, int nstr)
 *
 * ARGUMENT
 *	mode	0:	initialize with font texture loading
 *		1:	initialize without font texture loading
 *		2:	load font texturn only
 *	x, y	menu left upper corner position
 *	nstr	maximum number of characters to be used
 *
 * DESCRIPTION
 *	menuInit() initializes a stream for menu printing.
 *
 * NOTES
 *	If given mode is 2, 'x', 'y' and 'nstr' value is ignored and
 *	previous mode is preserved.
 *	Font pattern is loaded in (960, 256).
 *
 *--------------------------------------------------------------------------
 * menuUpdate	update menu contents
 *
 * SYNOPSIS
 *	int menuUpdate(char *title, char **path, padd)
 *
 * ARGUMENT
 *	title	title bar string
 *	path	menu strings
 *	padd	controller status
 *
 * DESCRIPTION
 *	menuUpdate() displays a text window with specified menu strings.
 *	and get the selected item ID.
 *
 * RETURNS
 *	-1	no button is pressed.
 *	else	selected menu ID */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

static int	sx = 80, sy = 64, nstr = 64, id = 0;

void menuInit(int mode, int x, int y, int n)
{
	if (mode == 0 || mode == 2)
		FntLoad(960, 256);	
	if (mode == 0 || mode == 1)
		id = FntOpen((sx = x), (sy = y), 0, 0, 2, (nstr = n));	
}
	
int menuUpdate(char *title, char **path, u_long padd)
{
	static int	cp = 0, opadd = 0;
	int i, ispress = 0;
	
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

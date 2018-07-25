/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _MAXTYPES_H_
#define _MAXTYPES_H_

/*****************************************************************
 *
 * file: maxtypes.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include "maxconf.h"
#include "turtle.h"
#include "const.h"

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_F4 	bg;			/* background */
#ifdef OVAL
	OVAL_PRIM	oval;                   /* background oval */
#else
	POLY_G4         box[2];                 /* background */
	MESH_PRIM       mesh[2];                /* lines for mesh pattern */
#endif /* OVAL */
} DB;

typedef struct {
	long	tim;
	int	x;
	int	y;
	long	prio;	/* ordering priority */
	int	copy;
	int	scale;
	long	newW;
	long	newH;
	int	useMenu;
} _INIT;


#endif /* _MAXTYPES_H_ */
/* DON'T ADD STUFF AFTER THIS */

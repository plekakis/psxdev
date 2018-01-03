/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _MAXSTAT_H_
#define _MAXSTAT_H_

/*****************************************************************
 *
 * file: maxstat.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include "psxload.h"
#include "spritex.h"
#include "menux.h"

typedef struct {
	int	spriteCount;
	SPRTX   Sprites[2][128];
	int 	bgsprtCount;
	SPRTX	bgsprt[2][64];

	int 	menusprtStart;
	int	menusprtCount;

	int	pageNumber;

	SPRTX   cursor[2];
} MAXSTAT;

extern  int  fot;

#endif /* _MAXSTAT_H_ */
/* DON'T ADD STUFF AFTER THIS */

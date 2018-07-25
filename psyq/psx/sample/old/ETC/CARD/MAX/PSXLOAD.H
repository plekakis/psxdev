/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _PSXLOAD_H_
#define _PSXLOAD_H_

/*****************************************************************
 *
 * file: psxload.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

typedef	struct {
	short	tpage;
	short	u;
	short	v;
} _TPAGE;


int	PSXgetInterlace( void );
void	PSXsetInterlace( int inter );
void	ClearTexture( void );
int	PSXloadTexture( u_long* adrs, GsIMAGE* RectImage, _TPAGE* RectTpage );

#endif /* _PSXLOAD_H_ */
/* DON'T ADD STUFF AFTER THIS */

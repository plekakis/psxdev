/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _VSYNC_H_
#define _VSYNC_H_

/*****************************************************************
 *
 * file: vsync.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

void	VSNopen( void );
void	VSNclose( void );
void	VSNdrawOTag( long* ot, DRAWENV dr, DISPENV dp );
int	VSNstatus( int mode );
void	VSNstoreOTag( long* ot, long* prim );
void	VSNsetEnviro( DRAWENV drw, DISPENV dsp );

#endif /* _VSYNC_H_ */
/* DON'T ADD STUFF AFTER THIS */

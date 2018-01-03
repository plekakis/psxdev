/* $PSLibId: Run-time Library Release 4.4$ */
/*****************************************************************
 *
 * file: vsync.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include <memory.h>
#include "libps.h"
#include "maxconf.h"
#include "vsync.h"

#define	_END_OF_PRIM	0xffffff
#define	PRIMBUFMAX	16*1024L
#define H_SYSCLOCK8     264

typedef struct {
	unsigned p:24;
	unsigned num:8;
} _TAG;

long	PrimBuff[2][PRIMBUFMAX];

static	DRAWENV	draw[2];
static	DISPENV	disp[2];
static	int	side;
static	unsigned long ev;
static	volatile int req;
static	volatile int inter;
static	void BackDrawFunc( void );
void	memcpy4(long *dst, long *src, int size);
/*/////////////////////////////////////
// VSYNC handler
/////////////////////////////////////*/
static void BackDrawFunc( void )
{
        int delay;

	if( !req ) return;

	if( GetVideoMode() ) delay = 74;
	else             delay = 22;

	ResetRCnt( RCntCNT2 );

	while( GetRCnt( RCntCNT2 ) < delay*H_SYSCLOCK8 );

	PutDrawEnv( &draw[side] );
	PutDispEnv( &disp[side] );
	DrawOTag( PrimBuff[side] );

	req--;
}
/*/////////////////////////////////////
//
/////////////////////////////////////*/
void VSNopen( void )
{
	EnterCriticalSection();

	VSyncCallback(BackDrawFunc);
	SetRCnt( RCntCNT2, 0xffff, RCntMdNOINTR );
	StartRCnt( RCntCNT2 );

	ExitCriticalSection();

	req = 0;
	inter = 0;
	side = 0;
}
/*/////////////////////////////////////
//
/////////////////////////////////////*/
void	VSNclose( void )
{
	CloseEvent( ev );
	SetRCnt( RCntCNT2, 0, RCntMdNOINTR );
	StopRCnt( RCntCNT2 );
}
/*/////////////////////////////////////
// Draw start trigger
/////////////////////////////////////*/
void	VSNdrawOTag( long* ot, DRAWENV dr, DISPENV dp )
{
	VSNstoreOTag( ot, PrimBuff[side^1] );
	draw[side^1] = dr;
	disp[side^1] = dp;
	side ^= 1;
	req = 2;
}
/*/////////////////////////////////////
//
/////////////////////////////////////*/
int VSNstatus( int mode )
{
	if( mode==1 ) while( req>1 );
	if( mode==0 ) while( req );
	return( req );
}
/*/////////////////////////////////////
// Copy the ordering table
/////////////////////////////////////*/
void	VSNstoreOTag( long* ot, long* prim )
{
	_TAG*	stag;
	_TAG*	dtag;
	long	pPrims;

	pPrims = 0;
	stag = (_TAG*)ot;
	dtag = (_TAG*)prim;
	while( 1 ) {
		if( pPrims+stag->num >= PRIMBUFMAX ) break;
		dtag = (_TAG*)(&prim[pPrims++]);
		dtag->num = stag->num;
		if( stag->num ) {
			memcpy4( &prim[pPrims], (long*)stag+1, (int)stag->num );
			pPrims += stag->num;
		}
		if( stag->p != _END_OF_PRIM ) {
			dtag->p = (unsigned)&prim[pPrims];
			stag = (_TAG*)( stag->p );
		}
		else	break;
	}
	dtag->num = 0;
	dtag->p = _END_OF_PRIM;
}

void	memcpy4(long *dst, long *src, int size)
{
        while (size--) {
                *dst++ = *src++;
        }
}

#ifdef DEBUG
/*/////////////////////////////////////
// Dubug stuff
/////////////////////////////////////*/
void	VSNprint( void )
{
	int	i;
	_TAG*	tag;

	i = 0;
	while( 1 ) {
		tag = (_TAG*)(&PrimBuff[side][i]);
		FntPrint( "NUM=%d, PTR=%x\n", tag->num, tag->p );
		if( tag->p == _END_OF_PRIM ) break;
		i += ( tag->num + 1 );
	}
}
#endif /* DEBUG */

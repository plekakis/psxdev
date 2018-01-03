/* $PSLibId: Run-time Library Release 4.3$ */
/*
 * Sychronous Communications Sample
 *
 *	Copyright (C) 1998 by Sony Computer Entertainment
 *			All rights Reserved
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsn.h>
#include <libcomb.h>
#include "balls.h"

/**************************************************************************/
#define	PIH		(320)
#define	PIV		(240)
#define	OTLEN		(16)
#define	BUFFSIZE	(256)
#define	TIMEOUT_VALUE	(99999)	/* Timeout value for inside WAIT callback */
/**************************************************************************/
static volatile int wait_count = 0;
static int	side;
static long	ot[2][OTLEN];
static DISPENV	disp[2];
static DRAWENV	draw[2];
static unsigned long fr, fw;
static char recbuf[BUFFSIZE];
static char senbuf[BUFFSIZE];
static long sencnt, reccnt;
static long hcnt = 0;
static long master = 0;
/**************************************************************************/
static int sync_read_write( int mode, char* recbuf, char* senbuf, long size );
static int sync_vsync_trger( int side );
static int sync_wait_call_back( long spec, unsigned long count );
/**************************************************************************/

int main(void)
{
	/* Initialize System */
	SetDispMask(0);
	ResetGraph(0);
	SetGraphDebug(0);
	ResetCallback();
	PadInit(0);
	ExitCriticalSection();

	/* Initialize drawing environment & screen double buffer */
	SetDefDrawEnv(&draw[0], 0,   0, PIH, PIV);
	SetDefDrawEnv(&draw[1], 0, PIV, PIH, PIV);
	SetDefDispEnv(&disp[0], 0, PIV, PIH, PIV);
	SetDefDispEnv(&disp[1], 0,   0, PIH, PIV);
	draw[0].isbg = draw[1].isbg = 1;
	setRGB0( &draw[0], 0, 0, 64 );
	setRGB0( &draw[1], 0, 0, 64 );
	PutDispEnv(&disp[0]);
	PutDrawEnv(&draw[0]);

	/* init balls module */
	_make_balls_data();

	/* Initialize onscreen font and text output system */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	/* Initialize link cable communications */
	AddCOMB();
	CombSetBPS( 2073600 );		/* Set communication rate */
	CombSetPacketSize(4);		/* Set the number of characters per transmission packet */
	fw = open("sio:", O_WRONLY);	/* Open stream for WRITING */
	fr = open("sio:", O_RDONLY);	/* Open stream for READING */

	/* Register our WAIT callback function */
	CombWaitCallback( (long)sync_wait_call_back );

	/* wait for DSR line to be asserted */
	/* wait for other PlayStation to be powered-up */
	while( (CombSioStatus() & COMB_DSR) != 0 );

	/* Test for CTS to be asserted */
	/* Synchronize with other PlayStation and determine master-slave relationship */
	if ( CombCTS() !=0 )
	{
		CombSetRTS(1);		/* assert RTS */
		CombResetVBLANK();	/* restart vblank */
		master = 0;		/* assume we are SLAVE machine */
	}
	else
	{
		CombSetRTS(1);		/* assert RTS */
		while(CombCTS() == 0);	/* wait for CTS to be asserted */
		CombResetVBLANK();	/* restart vblank */
		master = 1;		/* assume we are MASTER machine */
	}

	VSync(0);			/* Wait for VBLANK */
	CombSetRTS(0);			/* clear RTS */
	VSync(0);			/* Wait for VBLANK */
	SetDispMask(1);			/* Turn on the display */

	side = 0;
	sencnt = reccnt = 0;

	while(1)
	{
		ClearOTag( ot[side], OTLEN );

		/* Data transmission */
		if (sync_read_write( master, recbuf, senbuf, BUFFSIZE )==0)
		{
			reccnt++;
			sencnt++;
		}

		_draw_balls_data( side, ot[side] );
		hcnt = VSync(0);

		/* periodically match vertical synchronization with the other PlayStation */
		if (sync_vsync_trger( side ))
			continue;

		side ^= 1;
		PutDispEnv(&disp[side]);
		PutDrawEnv(&draw[side]);
		DrawOTag( ot[side^1] );
		FntPrint("\nREMOTE CONTROLLER\n\n");
		FntPrint("SYNCHRONOUS READ\n");
		FntPrint("SYNCHRONOUS WRITE\n\n");
		FntPrint("SEND       %d\nRECEIVE    %d\n", sencnt, reccnt );
		FntPrint("MODE       %s\n", master ? "MASTER":"SLAVE" );
		FntPrint("HSYNC      %d\n", hcnt );
		FntPrint("WAIT COUNT %d\n", wait_count );
		if (wait_count!=TIMEOUT_VALUE) FntPrint("STATUS     CONNECT\n");
		else FntPrint("STATUS     DISCONNECT\n");
		FntFlush(-1);
	}
	return 0;
}

/**************************************************************************/
/* Do read/write or write/read depending on if we are slave or master */

int sync_read_write( int mode, char* recbuf, char* senbuf, long size )
{
	if (mode)
	{
		/* Master processing */
		if ( write( fw, senbuf, BUFFSIZE ) != BUFFSIZE ) return(-1);
		if ( read( fr, recbuf, BUFFSIZE ) != BUFFSIZE ) return(-1);
	}
	else
	{
		/* Slave processing */
		if ( read( fr, recbuf, BUFFSIZE ) != BUFFSIZE ) return(-1);
		if ( write( fw, senbuf, BUFFSIZE ) != BUFFSIZE ) return(-1);
	}
	return(0);
}
/**************************************************************************/
int sync_wait_call_back( long spec, unsigned long count )
{
	wait_count = count;
	if ( count >= TIMEOUT_VALUE )
		return(0);
	return(1);
}
/**************************************************************************/
/* match vertical synchronization every three minutes with the other PlayStation */

#define	_REFRESH_INTERVAL	(3*60*60)

int sync_vsync_trger( int side )
{
	RECT	rect;
	static long refreshcount = _REFRESH_INTERVAL;

	if (refreshcount)
	{
		refreshcount--;
		return(0);
	}

	ResetGraph(1);
	CombSetRTS(1);
	while( CombCTS() == 0 );

	SetDispMask(0);
	CombResetVBLANK();
	setRECT( &rect, 0, 0, PIH, 480 );
	ClearImage(&rect,0,0,0);
	refreshcount = _REFRESH_INTERVAL;
	CombSetRTS(0);
	VSync(0);
	SetDispMask(1);

	return(1);
}



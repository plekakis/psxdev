/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	Memory Card Sample Program  - TUTO1 -
 *
 *
 *	Copyright (C) 1997 by Sony Computer Entertainment
 *			All rights Reserved
 *
*/
#include <sys/types.h>
#include <libapi.h>
#include <stdio.h>
#include <strings.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libmcrd.h>
#include "balls.h"

#define	PIH		320
#define	PIV		240
#define	OTLEN		16

/* variables for rendering */
static int	side;
static long	ot[2][OTLEN];
static DISPENV	disp[2];
static DRAWENV	draw[2];

/* get card information */
#define BLOCK_MAX	(15)
static struct DIRENTRY fileList[BLOCK_MAX];

/* Other */
static long hcount = 0;
/****************************************************************************

*****************************************************************************/
int	main( void )
{
	int i;
	int n;
	int ballcount = 16;
	long chan;
	long files;
	long rslt, cmds;
	RECT rect;
	char mes[32];

	ResetCallback();
	SetDispMask(0);
	ResetGraph(0);
	SetGraphDebug(0);
	PadInit(0);

	/* initialize environment for double buffer */
	SetDefDrawEnv(&draw[0], 0,   0, PIH, PIV);
	SetDefDrawEnv(&draw[1], 0, PIV, PIH, PIV);
	SetDefDispEnv(&disp[0], 0, PIV, PIH, PIV);
	SetDefDispEnv(&disp[1], 0,   0, PIH, PIV);
	draw[0].isbg = draw[1].isbg = 1;
	setRGB0( &draw[0], 0, 0, 64 );
	setRGB0( &draw[1], 0, 0, 64 );
	PutDispEnv(&disp[0]);
	PutDrawEnv(&draw[0]);
	VSync(0);
	SetDispMask(1);

	/* init balls module */
	_make_balls_data();

	/* font system */
	setRECT( &rect, 80, 32, 192, 160 );
	FntLoad(960, 256);
	FntOpen( rect.x, rect.y, rect.w, rect.h, 0, 512);

	/* initialize variables */
	cmds = 0; rslt = 0; files = 0;

	/* initialize memory card system */
	MemCardInit(1);
	MemCardStart();

	/* port number */
	chan = 0x00;

	while(1) {
		switch( MemCardSync( 1, &cmds, &rslt)) {

			/* no registration handling */
			case -1:
			MemCardExist( chan );	/* execute connection test */
			break;

			/* registration handling being executed */
			case 0:
			break;

			/* registration handling completed */
			case 1:
			switch(cmds) {
				case McFuncExist:
				switch(rslt) {
					case McErrNone:
					break;
					case McErrNewCard:	/* new card detected */
					strcpy( mes, "CARD INSERT" );
					MemCardAccept( chan );	/* get more detailed information */
					break;
					default:	/* no card, etc. */
					strcpy( mes, "CARD LOST" );
					files = 0;
					break;
				}
				break;
				case McFuncAccept:
				switch(rslt) {
					case McErrNone:
					case McErrNewCard:
					/* get directory information */
					MemCardGetDirentry( chan, "*", fileList, &files, 0, BLOCK_MAX );
					for( i=0, n=0; i<files; i++ )
						n += fileList[i].size/8192 + (fileList[i].size%8192 ? 1:0);
					sprintf( mes, "%d file(s) %d block(s)\n", (int)files, (int)n );
					break;
					case McErrNotFormat:	/* detected unformatted card */
					strcpy( mes, "NOT FORMAT" );
					break;
					default:
					sprintf( mes, "ERROR(%d)", (int)rslt );
					break;
				}
				break;
			}
			break;
		}

		/* render balls  */
		ClearOTag( ot[side], OTLEN );
		_draw_balls_data( side, ot[side], ballcount );
		hcount = VSync(0);
		side ^= 1;
		PutDispEnv(&disp[side]);
		PutDrawEnv(&draw[side]);
		DrawOTag( ot[side^1] );

		/* display message */
		FntPrint("MEMORY CARD SAMPLE %3d\n\n", hcount );
		for( i=0; i<files; i++ ) {
			FntPrint( "%s\n", fileList[i].name );
		}
		if( files ) FntPrint( "\n" );
		FntPrint( "%s\n", mes );
		FntFlush(-1);
		if (PadRead(1) == PADselect) break;
	}
	MemCardStop();
	MemCardEnd();

	PadStop();

	ResetGraph(3);
	StopCallback();
	return 0;
}

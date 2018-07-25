/*
 * $PSLibId: Run-time Library Release 4.3$
 */
/*
 *	Memory Card Sample Program  - TUTO2 -
 *
 *
 *	Copyright (C) 1997 by Sony Computer Entertainment
 *			All rights Reserved
 *
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <libapi.h>
#include <strings.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libmcrd.h>
#include "balls.h"


#define	PIH		512
#define	PIV		240
#define	OTLEN		16
#define CARD_BUFF_ADDR	((long*)0x80100000L)	/* read buffer for card data */

/* variables for rendering */
static int	side;
static long	ot[2][OTLEN];
static DISPENV	disp[2];
static DRAWENV	draw[2];

/* get card information */
#define BLOCK_MAX	(15)
static struct DIRENTRY fileList[2][BLOCK_MAX];
static long files[2];

/* for displaying balls */
static int ballcount = 64;

/* for displaying message */
static char mes[2][32] = { "", "" };
static char stdmes[64] = "";
static long stdio, fd[2];

/* Other */
static int cursor = 0;
static long hcount = 0;
/****************************************************************************

*****************************************************************************/
/* trigger input function */
long PadRead2( short port )
{
	static long repcnt = 0;
	static long padrec = 0;
	long	pad, trgdata, repdata;

	pad = PadRead( port );
	trgdata = pad &(~padrec);

	repdata = pad&padrec;
	if( pad && pad==padrec ) {
		if( ++repcnt >=15 ) {
			trgdata = pad;
			repcnt -= 2;
		}
	} else	repcnt = 0;

	padrec = pad;
	return( trgdata );
}

/* calculate number of blocks */
int _calc_total_blocks( int files, struct DIRENTRY* dir )
{
	int i;
	int n;

	for( i=0, n=0; i<files; i++ )
		n += dir[i].size/8192 + (dir[i].size%8192 ? 1:0);
	return(n);
}

/* check to see if file exists */
int _check_file_exist( int files, struct DIRENTRY* dir, char* name )
{
	int i;

	for( i=0; i<files; i++ )
		if( !strcmp( dir[i].name, name)) return(1);
	return(0);
}

/* Yes No selection */
int _ask_yes_no( long pad, char* mes, long* mode )
{
	static int first = 0;

	if(first==0) {
		first = 1;
		*mode = 0;
	}

	if(pad&(PADRdown|PADRright)) {
		first = 0;
		return(1);
	}

	if(pad&(PADLleft|PADLright)) *mode ^= 1;
	if(*mode==0) strcpy(strchr(mes,'>'), "> NO" );
	if(*mode==1) strcpy(strchr(mes,'>'), "> YES" );

	return(0);
}

/* main function */
int	main( void )
{
	int i;
	int n;
	long chan;
	long pad;
	long yesno;
	unsigned long func;
	unsigned long rslt, cmds;
	char fnam[28];
	long fsize = 0;

	ResetCallback();
	SetDispMask(0);
	ResetGraph(0);
	SetGraphDebug(0);
	PadInit(0);
	MemCardInit(1);	/* initialize memory card system */

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

	/* initialize balls */
	_make_balls_data();

	/* initialize font system */
	FntLoad(960, 256);	/* load basic font pattern */
	stdio = FntOpen( 152,  24, 208,	 40, 0, 128);
	fd[0] = FntOpen(  32,  64, 192, 160, 0, 512);
	fd[1] = FntOpen( 272,  64, 192, 160, 0, 512);

	/* initialize variables */
	func = 0; cmds = 0; rslt = 0;
	files[0] = 0;
	files[1] = 0;

	VSync(0);
	SetDispMask(1);

	/* start up memory card system */
	MemCardStart();


	/* initialize port number */
	chan = 0x10;

	while(1) {
		/* pad input processing */
		pad = PadRead2(0);

		/* cursor movement */
		if( pad==PADLup && cursor>0 ) cursor--;
		if( pad==PADLdown && cursor<(files[0]-1)) cursor++;

		/* end */
		if(pad&PADselect) break;

		if( func==0 ) {
			/* register file copy processing */
			if((pad&(PADRdown|PADRright)) && files[0]>0 ) {
				func = 10;
				fsize = fileList[0][cursor].size;
				strcpy( fnam, fileList[0][cursor].name );
				strcpy( stdmes, "FILE READING..." );
			}
		}

		switch( MemCardSync( 1, &cmds, &rslt)) {

			/* no registration handling */
			case -1:
			switch(func) {
				case 10:
				/* check for card and get directory information from copy destination */
				chan = 0x10;
				MemCardAccept( chan );
				func = 11;
				break;
				case 11:
				if(rslt==McErrNotFormat) {
					func = 30;	/* format */
					strcpy( stdmes, "CARD-2 IS NOT FORMAT\n   FORMAT? >" );
					break;
				}
				if(rslt!=McErrNone) {
					func = 0;
					strcpy( stdmes, "CARD-2 IS NOT READY" );
					break;
				}
				/* check to see if file is already at copy destination */
				if( _check_file_exist( files[1], fileList[1], fnam )==1 ) {
					func = 15;
					strcpy( stdmes, "FILE ALREADY EXIST\n   OVERWRITE? >" );
					break;
				}
				/* check for empty blocks */
				if((BLOCK_MAX-_calc_total_blocks( files[1], fileList[1] ))<(fsize/8192)+(fsize%8192?1:0)) {
					func = 0;
					strcpy( stdmes, "CARD-2 IS FREE BLOCK EMPTY" );
					break;
				}
				/* create new files */
				if( MemCardCreateFile( 0x10, fnam, fsize/8192 + (fsize%8192 ? 1:0))!=0 ) {
					strcpy( stdmes, "CARD-2 IS NOT READY" );
					func = 0; break;
				}
				/* begin reading copy source */
				case 12:
				strcpy( stdmes, "FILE READING..." );
				MemCardReadFile( 0x00, fnam, CARD_BUFF_ADDR, 0, fsize );
				break;
				case 15:	/* YES:NO switching */
				if(_ask_yes_no(pad, stdmes, &yesno)==0 ) break;
				if( yesno==1 ) func = 12; else { func = 0; strcpy( stdmes, "" ); }
				break;

				case 20:
				/* begin writing to copy destination */
				strcpy( stdmes, "FILE WRITING..." );
				MemCardWriteFile( 0x10, fnam, CARD_BUFF_ADDR, 0, fsize );
				break;

				case 30:
				if(_ask_yes_no(pad, stdmes, &yesno)==0 ) break;
				if( yesno==1 ) { func = 31; strcpy( stdmes, "FORMATING..." ); }
				if( yesno==0 ) { func =	 0; strcpy( stdmes, "" ); }
				break;
				case 31:
				func++;
				break;
				case 32:
				/* format card */
				if( MemCardFormat( 0x10 )==McErrNone ) {
					func = 10;
					strcat( stdmes, "COMPLETE" );
				} else {
					func = 0;
					strcat( stdmes, "FAILD" );
				}
				break;

				default:
				/* execute connection test if no processing has been reserved */
				chan ^= 0x10;
				MemCardExist( chan );
				break;
			}
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
					strcpy( mes[chan>>4], "CARD INSERT" );
					MemCardAccept( chan );	/* get more detailed information */
					if(chan==0x00 ) cursor = 0;
					break;
					default:	/* no card, etc. */
					strcpy( mes[chan>>4], "CARD LOST" );
					files[chan>>4] = 0;
					break;
				}
				break;
				case McFuncAccept:
				switch(rslt) {
					case McErrNone:
					case McErrNewCard:
					/* get directory information */
					MemCardGetDirentry( chan, "*", fileList[chan>>4], &files[chan>>4], 0, BLOCK_MAX );
					n = _calc_total_blocks( files[chan>>4], fileList[chan>>4] );
					sprintf( mes[chan>>4], "%d file(s) %d block(s)\n", (int)files[chan>>4], (int)n );
					break;
					case McErrNotFormat:	/* detected unformatted card */
					strcpy( mes[chan>>4], "NOT FORMAT" );
					files[chan>>4] = 0;
					break;
					case McErrCardNotExist:
					strcpy( mes[chan>>4], "CARD LOST" );
					break;
					default:
					sprintf( mes[chan>>4], "ERROR(%d)", (int)rslt );
					break;
				}
				break;
				/* end of reading from copy source */
				case McFuncReadFile:
				if(rslt==McErrNone) {
					func = 20;
				} else	{ strcat( stdmes, "FAILD" ); func = 0; }
				break;

				/* end of writing to copy destination */
				case McFuncWriteFile:
				switch(rslt) {
					case McErrNone:
					strcat( stdmes, "COMPLETE" );
					MemCardGetDirentry( 0x10, "*", fileList[1], &files[1], 0, BLOCK_MAX );
					n = _calc_total_blocks( files[1], fileList[1] );
					sprintf( mes[1], "%d file(s) %d block(s)\n", (int)files[1], (int)n );
					func = 0;
					break;
					default:
					strcat( stdmes, "FAILD" );
					chan = 0x10;
					MemCardAccept(chan);
					func = 0;
					break;
				}
				break;
			}
		}

		/* render balls */
		ClearOTag( ot[side], OTLEN );
		_draw_balls_data( side, ot[side], ballcount );
		hcount = VSync(0);
		side ^= 1;
		PutDispEnv(&disp[side]);
		PutDrawEnv(&draw[side]);
		DrawOTag( ot[side^1] );

		/* display message */
		FntPrint(stdio, "MEMORY CARD SAMPLE %3d\n\n", hcount );
		FntPrint(stdio, "%s\n", stdmes	);
		for( n=0; n<2; n++ ) {
			if( n==0 )
				FntPrint( fd[n], "CARD-1  SOURCE\n\n" );
			else	FntPrint( fd[n], "CARD-2  DESTINATION\n\n" );
			for( i=0; i<files[n]; i++ ) {
				if( n==0 && cursor==i )
					FntPrint( fd[n], "*%s\n", fileList[n][i].name );
				else
					FntPrint( fd[n], " %s\n", fileList[n][i].name );
			}
			if( files[n] ) FntPrint( fd[n], "\n" );
			FntPrint( fd[n], "%s\n", mes[n] );
			FntFlush(fd[n]);
		}
		FntFlush(stdio);
	}

	MemCardSync(0,0,0);
        PadStop();
	ResetGraph(1);

	/* finished memory card system */
	MemCardStop();
	MemCardEnd();
	StopCallback();
	return 0;
}

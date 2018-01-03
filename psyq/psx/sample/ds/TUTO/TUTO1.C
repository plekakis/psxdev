/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			tuto1: CD player (2)
 *
 *	Copyright(C) 1994 1997  Sony Computer Entertainment Inc.,
 *	All rights Reserved.
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul.29,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *		2.00		Apr.22,1997	makoto
*/
/*		simplest CD-Player (interrupt type)
 *	This program can play the normal audio CD discs with balls
 * */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

static int track, min, sec, level;	/* for callbacks */
static int is_bg_error = 0;	/* background error flag */

#define dsSetVol( vol, v ) \
	( vol )->val0 = ( vol )->val2 = v, \
	( vol )->val1 = ( vol )->val3 = 0, \
	DsMix( vol )

char* title = "    SELECT MENU    ";
static char* menu[] = {
	"Forward",	"Backward",	"Play",		"Pause",
	"Mute",		"Demute",	"Next Track",	"Prev Track",
	"Last Track",	"First Track",	"Volume Up",	"Volume Down",
	0,
};

static void cbdataready( u_char intr, u_char* result );

int main( void )
{
	DslLOC loc[ 100 ];
	DslATV atv;
	int vol = 0x80;

	u_long padd;
	int i, ret, ntoc;
	int id;
	u_char mute_com = DslDemute;

	/* initialize environment */
	ResetGraph( 0 );
	PadInit( 0 );

	menuInit( 0, 80, 88, 256 );
	SetDumpFnt( FntOpen( 16, 16, 320, 200, 0, 512 ) );

	DsInit();
	DsSetDebug( 0 );

	/* set initila volume*/
	dsSetVol( &atv, vol );

	/* read TOC*/
	if( ( ntoc = DsGetToc( loc ) ) == 0 ) {
		printf( "No TOC found: please use CD-DA disc...\n" );
		goto abort;
	}

	/* start playing*/
	id = DsPacket( DslModeRept | DslModeDA, &loc[ 2 ], DslPlay, 0, -1 );
	track = 2;
	min = loc[ 2 ].minute;
	sec = loc[ 2 ].second;
	level = 0;

	/* hook callbacks*/
	DsReadyCallback( cbdataready );

	while( ( ( padd = PadRead( 1 ) ) & PADselect ) == 0 ) {
		balls();	/* display */

		/* if error is detected, retry to play the first track. */
		if( ( ret = DsSync( id, 0 ) ) == DslDiskError || is_bg_error) {
			id = DsPacket( DslModeRept | DslModeDA, &loc[ 1 ],
			  DslPlay, 0, -1 );
			is_bg_error = 0;
		}

		FntPrint( "CD-PLAYER (interrupt version)\n\n" );
		FntPrint( "Use Audio DISC for this test\n\n" );
		FntPrint( "pos =(%2d:%02x:%02x)\n", track, min, sec );
		FntPrint( "level =%d\n", level );
		FntPrint( "stat =%s(%s)\n",
		  DsIntstr( ret ), DsComstr( DsLastCom() ) );
		FntPrint( "vol  =%d(mute=%s)\n", vol,
		  mute_com == DslMute ? "ON" : "OFF" );

		FntFlush( -1 );

		switch( menuUpdate( title, menu, padd ) ) {
		case 0:
			DsCommand( DslForward, 0, 0, -1 );
			break;

		case 1:
			DsCommand( DslBackward, 0, 0, -1 );
			break;

		case 2:
			DsCommand( DslPlay, 0, 0, -1 );
			break;

		case 3:
			DsCommand( DslPause, 0, 0, -1 );
			break;

		case 4:
			DsCommand( ( mute_com = DslMute ), 0, 0, -1 );
			break;

		case 5:
			DsCommand( ( mute_com = DslDemute ), 0, 0, -1 );
			break;

		case 6:	/* next track */
			if( ++track > ntoc )
				track = ntoc;
			id = DsPacket( DslModeRept | DslModeDA, &loc[ track ],
			  DslPlay, 0, -1 );
			break;

		case 7:	/* previous track */
			if( --track < 1 )
				track = 1;
			id = DsPacket( DslModeRept | DslModeDA, &loc[ track ],
			  DslPlay, 0, -1 );
			break;

		case 8:	/* last track */
			id = DsPacket( DslModeRept | DslModeDA, &loc[ ntoc ],
			  DslPlay, 0, -1 );
			break;

		case 9:	/* 1st track */
			id = DsPacket( DslModeRept | DslModeDA, &loc[ 1 ],
			  DslPlay, 0, -1 );
			break;

		case 10:	/* volume up */
			vol += 10;
			limitRange( vol, 0, 128 );
			dsSetVol( &atv, vol );
			break;

		case 11:	/* volume down */
			vol -= 10;
			limitRange( vol, 0, 128 );
			dsSetVol( &atv, vol );
			break;
		}
	}

abort:
	DsControlB( DslPause, 0, 0 );
	DsClose();
	ResetGraph( 1 );
	PadStop();
	StopCallback();
	return 0;
}

/*
 * callback procedure which is called at the DataReady interrupt. */
static void cbdataready( u_char intr, u_char* result )
{
	if( intr == DslDataReady ) {
		if( ( result[ 4 ] & 0x80 ) == 0 ) {
			track = btoi( result[ 1 ] );
			min = result[ 3 ];
			sec = result[ 4 ];
			level = ( result[ 6 ] << 8 ) | result[ 7 ];
		}
	} else if( intr == DslDiskError ) {
		printf( "cbdataready error:%s\n", DsIntstr( intr ) );
		is_bg_error = 1;
	}
}

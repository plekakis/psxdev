/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			tuto0: CD player (1)
 *
 *	Copyright(C) 1994 1997 Sony Computer Entertainment Inc.,
 *	All rights Reserved.
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul.29,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *		2.00		Apr.17,1997	makoto
*/
/*		simplest CD-Player (polling type)
 *	This program plays normal audio CD discs with balls
 * */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

#define dsSetVol( vol, v ) \
	( vol )->val0 = ( vol )->val2 = v, \
	( vol )->val1 = ( vol )->val3 = 0, \
	DsMix( vol )

char *title = "    SELECT MENU    ";
static char* menu[] = {
	"Forward",	"Backward",	"Play",		"Pause",
	"Mute",		"Demute",	"Next Track",	"Prev Track",
	"Last Track",	"First Track",	"Volume Up",	"Volume Down",
	0,
};

int main( void )
{
	DslLOC loc[ 100 ];
	DslATV atv;
	int vol = 0x80;

	u_long padd;
	u_char result[ 8 ];
	int id;
	int i, ret, ntoc;
	int track, min, sec, level;
	u_char mute_com = DslDemute;	/* previous MUTE command */

	/* initialize graphics and controller */
	ResetGraph( 0 );
	PadInit( 0 );

	/* initialize menu */
	menuInit( 0, 80, 88, 256 );

	/* open message window */
	SetDumpFnt( FntOpen( 16, 16, 320, 200, 0, 512 ) );

	/* initialize CD subsytem*/
	DsInit();
	DsSetDebug( 0 );

	/* set initila volume*/
	dsSetVol( &atv, vol );

	/* check DiskStatus */
	while( DsSystemStatus() != DslReady ) {
		printf( "Shell open waiting ...\n" );
		if( DsSystemStatus() == DslNoCD )
			goto abort;
	}

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

	while( ( ( padd = PadRead( 1 ) ) & PADselect ) == 0 ) {
		balls();	/* display */

		/* check the report from CD-ROM*/
		if( ( ret = DsReady( result ) ) == DslDataReady ) {
			if( ( result[ 4 ] & 0x80 ) == 0 ) {
				track = btoi( result[ 1 ] );
				min = result[ 3 ];
				sec = result[ 4 ];
				level = ( result[ 6 ] << 8 ) | result[ 7 ];
			}
		}
		/* if error is detected, retry to play the first track. */
		else if( ret == DslDiskError )
			id = DsPacket( DslModeRept | DslModeDA, &loc[ 1 ],
			  DslPlay, 0, -1 );

		/* error check */
		if( ( ret = DsSync( id, 0 ) ) == DslDiskError ) {
			id = DsPacket( DslModeRept | DslModeDA, &loc[ 1 ],
			  DslPlay, 0, -1 );
			FntPrint( "CDROM: DiskError. retrying..\n" );
		}

		FntPrint( "CD-PLAYER (polling version)\n\n" );
		FntPrint( "Use Audio DISC for this test\n\n" );
		FntPrint( "pos  =(%2d:%02x:%02x)\n",track, min, sec );
		FntPrint( "level=%d\n", level );
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

		case 6:				/* next track */
			if( ++track > ntoc )
				track = ntoc;
			id = DsPacket( DslModeRept | DslModeDA, &loc[ track ],
			  DslPlay, 0, -1 );
			break;

		case 7:				/* previous track */
			if( --track < 1 )
				track = 1;
			id = DsPacket( DslModeRept | DslModeDA, &loc[ track ],
			  DslPlay, 0, -1 );
			break;

		case 8:				/* last track */
			id = DsPacket( DslModeRept | DslModeDA, &loc[ ntoc ],
			  DslPlay, 0, -1 );
			break;

		case 9:				/* 1st track */
			id = DsPacket( DslModeRept | DslModeDA, &loc[ 1 ],
			  DslPlay, 0, -1 );
			break;

		case 10:			/* volume up */
			vol += 10;
			limitRange( vol, 0, 128 );
			dsSetVol( &atv, vol );
			break;

		case 11:			/* volume down */
			vol -= 10;
			limitRange( vol, 0, 128 );
			dsSetVol( &atv, vol );
			break;
		}
	}

abort:
	DsControlB( DslPause, 0, 0 );
	DsClose();
	PadStop();
	ResetGraph( 1 );
	StopCallback();
	return 0;
}

/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *		tuto3: Audio repeat using DslDataEnd
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul.07,1995	suzu
 *		2.00		Apr.22,1997	makoto
*/
/*
 *                 Repeat Play
 *        auto repeat play using DslDataEnd
 *
 *   Auto repeat using DslDataEnd is fast because it requires less
 *   interrupts from CD-ROM. But the track ID is needed to all
 *   repeating point, and total number of tracks is limited to 100,
 *   If it is acceptable, this repeating strategy is the simplest.
 * */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

DslLOC loc[ 100 ];
int errcnt = 0;
int nplay  = 0;
int curp;
int ntoc;
int track[] = { 7, 2, 6, 3, 5, 4, 0 };
int track_id = 0;
int is_play = 0;

static void print_gage( int startp, int endp, int curp );
static void play( int id );
static void check_play( u_char* result );
static void cbsync( u_char intr, u_char* result );
static void cbdataready( u_char intr, u_char* result );

char* title = "    SELECT MENU    ";
static char* menu[] = { "Normal", "Double", 0 };

static u_char mode = 0;

int main( void )
{
	u_long padd;
	u_char result[ 8 ];
	char* str;
	int i;

	/* initialize */
	ResetGraph( 0 );
	PadInit( 0 );
	menuInit( 0, 80, 96, 256 );
	SetDumpFnt( FntOpen( 32, 32, 320, 200, 0, 512 ) );

	DsInit();
	DsSetDebug( 0 );

	/* read TOC */
	if( ( ntoc = DsGetToc( loc ) ) == 0 ) {
		printf( "No TOC found: please use CD-DA disc...\n" );
		goto abort;
	}

	/* define callback */
	DsReadyCallback( cbdataready );

	/* start playing */
	mode = DslModeAP | DslModeDA;
	play( track_id );

	while( ( ( padd = PadRead( 1 ) ) & PADselect ) == 0 ) {
		balls();

		/* Check to see if playing has begun */
		if( is_play == 0 && DsStatus() & DslStatPlay )
			is_play = 1;

		switch( menuUpdate( title, menu, padd ) ) {
		case 0:	/* normal speed */
			mode &= ~DslModeSpeed;
			DsCommand( DslSetmode, &mode, 0, 0 );
			break;

		case 1:	/* double speed */
			mode |= DslModeSpeed;
			DsCommand( DslSetmode, &mode, 0, 0 );
			break;

		default:	/* if no key pushed, check playing */
			if( is_play )
				check_play( result );
		}

		/* print status */
		FntPrint( "CD-DA REPEAT (DataEnd detect)\n\n" );
		FntPrint( "Use Audio DISC for this test\n\n" );
		FntPrint( "stat=%02x,errors=%d/%d\n", result[ 0 ], errcnt,
		  nplay );
		FntPrint( "playing=" );
		for( i = 0; track[ i ] != 0; i++ ) {
			str = ( i == track_id ) 
			  ? ( VSync( -1 ) & 0x20 ? "888" : "880" ) : "444";
			FntPrint( "~c%s%d", str, track[ i ] );
		}
		FntPrint( "\n" );
		print_gage( DsPosToInt( &loc[ track[ track_id ] ] ),
		  DsPosToInt( &loc[ track[ track_id ] + 1 ] ), curp );
		FntFlush( -1 );
	}

abort:
	DsControlB( DslPause, 0, 0 );
	DsClose();
	ResetGraph( 1 );
	PadStop();
	StopCallback();
	return 0;
}

/* callback generated with DslDataEnd*/
static void cbdataready( u_char intr, u_char* result )
{
	if( intr == DslDataEnd ) {
		printf( "cbdataready: DslDataEnd (track=%d,time=%d)\n",
		  track_id, VSync( -1 ) );
		if( track[ ++track_id ] == 0 )
			track_id = 0;

		is_play = 0;
		play( track_id );

	} else if( intr == DslDiskError ) {
		printf( "cbdataready error:%s\n", DsIntstr( intr ) );
		errcnt++;
	}
}

static void check_play( u_char* result )
{
	int com, ret;

	/* check seeking/playing status */
	DsControl( DslNop, 0, result );
	if( ( result[ 0 ] & 0xc0 ) == 0 ) {
		printf( "check_play error(%02x)\n", result[ 0 ] );
		errcnt++;
		play( track_id );
	} else
		DsCommand( DslGetlocP, 0, cbsync, 0 );
}

static void cbsync( u_char intr, u_char* result )
{
	if( intr == DslComplete )
		curp = DsPosToInt( ( DslLOC* )&result[ 5 ] );
}

static void play( int id )
{
	nplay++;
	if( track[ id ] > ntoc ) {
		printf( "%d: track overflow\n", track[ id ] );
		track[ id ] = ntoc;
	}
	DsPacket( mode, &loc[ track[ id ] ], DslPlay, 0, -1 );
	is_play = 0;
}

static void print_gage( int startp, int endp, int curp )
{
	int i = 0, rate;

	rate = 32 * ( curp - startp ) / ( endp - startp );

	FntPrint( "~c444" );
	while( i++ < rate )
		FntPrint( "*" );
	FntPrint( ( VSync( -1 ) & 0x08 ) ? "~c888*~c444" : "*" );
	while( i++ < 32 )
		FntPrint( "*" );
}

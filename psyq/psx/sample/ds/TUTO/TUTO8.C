/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			tuto8: multi DsRead
 *
 *	Copyright(C) 1994 1997  Sony Computer Entertainment Inc.,
 *	All rights Reserved.
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Oct.16,1994	suzu
 *		1.20		Mar,12,1995	suzu
 *		2.00		Jul,20,1995	suzu
 *		3.00		Apr.25,1997	makoto
*/
/*			multi file DsRead
 *		read many files from CD-ROM in background */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

void bgFlush( void );
void bgUpdate( u_char col );

void sndInit( void );
void sndMute( int mode );
void sndEnd( void );

static void read_test( void );
static void notfound( void );
static void cbvsync( void );

/* for long term test*/

char file[] = "\\XDATA\\STR\\MOV.STR;1";

int main( void )
{
	/* initialize*/
	ResetGraph( 0 );
	PadInit( 0 );
	FntLoad( 960, 256 );
	SetDumpFnt( FntOpen( 16, 64, 0, 0, 1, 512 ) );

	DsInit();
	DsSetDebug( 0 );

	sndInit();

	VSyncCallback( cbvsync );
	SetDispMask( 1 );

	while( ( PadRead( 1 ) & PADselect ) == 0 )
		read_test();

	/* ending*/
	PadStop();
	DsClose();
	StopCallback();
	return 0;
}

/* DsRead() reads NSECTOR sectors each, and it is invoked NREAD times
 * in the background, so MAXSECTOR sectors is read at last. */
#define MAXSECTOR	256
#define NSECTOR		32
#define NREAD		( MAXSECTOR / NSECTOR )

int dsReadChainSync( void );
int dsReadChain( DslLOC* _postbl, int* _scttbl, u_long** _buftbl, int _ntbl );

#define StatREADIDLE	0
#define StatREADBURST	1
#define StatREADCHAIN	2

static int read_stat = StatREADIDLE;

static void read_test( void )
{
	static u_long sectbuf[ 2 ][ MAXSECTOR * 2048 / 4 ];
	static int errcnt = 0;

	/* The positions, sector numbers and destination buffer pointers
	 * are indicated through following queue arrays.
	 * dsReadChain() reads the data from CD-ROM according to this
	 * instruction queue. */
	DslLOC postbl[ NREAD ];		/* table for the position on CD-ROM */
	int scttbl[ NREAD ];		/* table for sector number */
	u_long* buftbl[ NREAD ];	/* table for destination buffer */

	int i, j, cnt, ipos;
	DslLOC pos;
	DslFILE fp;
	int nsector;

	/* notify to callback */
	read_stat = StatREADIDLE;

	/* clear destination buffer*/
	for( i = 0; i < sizeof( sectbuf[ 0 ] ) / 4; i++ ) {
		sectbuf[ 0 ][ i ] = 0;
		sectbuf[ 1 ][ i ] = 1;
	}

	/* get the file postion and size */
	if( DsSearchFile( &fp, file ) == 0 ) {
		notfound();
		return;
	}

	if( ( nsector = ( fp.size + 2047 ) / 2048 ) > MAXSECTOR )
		nsector = MAXSECTOR;

	/* notify to callback */
	read_stat = StatREADBURST;

	/* read in one time*/
	DsRead( &fp.pos, nsector, sectbuf[ 0 ], DslModeSpeed );

	/* wait for end of reading*/
	while( ( cnt = DsReadSync( 0 ) ) > 0 );

	/* notify to callback */
	read_stat = StatREADCHAIN;

	/* get the file postion and size */
	if( DsSearchFile( &fp, file ) == 0 ) {
		FntPrint( "%s: cannot open\n", file );
		return;
	}
	if( ( nsector = ( fp.size + 2047 ) / 2048 ) > MAXSECTOR )
		nsector = MAXSECTOR;

	/* make tables for dsReadChain()*/
	ipos = DsPosToInt( &fp.pos );
	for( i = j = 0; i < nsector; i += NSECTOR, ipos += NSECTOR, j++ ) {
		DsIntToPos( ipos, &postbl[ j ] );
		scttbl[ j ] = NSECTOR;
		buftbl[ j ] = &sectbuf[ 1 ][ i * 2048 / 4 ];
	}

	/* start chained DsRead*/
	dsReadChain( postbl, scttbl, buftbl, nsector / NSECTOR );

	/* wait for end of reading*/
	while( ( cnt = dsReadChainSync() ) > 0 );

	/* notify to callback */
	read_stat = StatREADIDLE;

	/* compare*/
	for( i = 0; i < nsector * 2048 / 4; i++ ) {
		if( sectbuf[ 0 ][ i ] != sectbuf[ 1 ][ i ] ) {
			printf( "verify ERROR at (%08x:%08x)\n\n",
			  &sectbuf[ 0 ][ i ], &sectbuf[ 1 ][ i ] );
			errcnt++;
			break;
		}
	}
	FntPrint( "verify done(err=%d)\n", errcnt );
}

static void notfound( void )
{
	int n = 60 * 4;
	while( n-- ) {
		FntPrint( "\n\n%s: not found\n", file );
		FntFlush( -1 );
		VSync( 0 );
	}
	printf( "%s: not found\n", file );
}

/*
 *	VSync Callback:
 *	Drawing function is automatically called by V-BLNK interrupt
 *	to avoid flicker of the background animation.
 *	Note that callback should return immediately becausse all
 *	other callbacks are suspended during that period.
 *	callback returns.
 * */
static void cbvsync( void )
{
	FntPrint( "\t\t CONTINUOUS DsRead \t\t\n\n" );
	FntPrint( "file:%s\n\n", file );
	switch( read_stat ) {
	case StatREADIDLE:
		FntPrint( "idle...\n" );
		break;

	case StatREADBURST:
		FntPrint( "Burst Read....%d sectors\n", DsReadSync( 0 ) );
		break;

	case StatREADCHAIN:
		FntPrint( "DIvided Read ...%d blocks\n", dsReadChainSync() );
		break;
	}
	bgUpdate( 96 );
	FntFlush( -1 );
}

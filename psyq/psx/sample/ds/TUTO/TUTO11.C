/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*                  tuto11:
 *             Checks performed when new CD is inserted
 *
 *   Copyright (C)  1996 1997 Sony Computer Entertainment Inc.,
 *   All Rights Reserved.
 * */

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>
#include <libetc.h>

static u_long padd_read( void );

int main( void )
{
	unsigned long padd;
	RECT rect;
	DRAWENV draw;
	DISPENV disp;

	int status = 1;

	/* reset system */
	ResetGraph( 0 );
	PadInit( 0 );

	/* reset CD-ROM system */
	DsInit();
	DsSetDebug( 0 );

	/* load basic font pattern */
	FntLoad( 960, 256 );
	SetDumpFnt( FntOpen( 16, 64, 0, 0, 1, 512 ) );

	/* prepare for screen print */
	PutDrawEnv( SetDefDrawEnv( &draw, 0, 0, 320, 240 ) );
	PutDispEnv( SetDefDispEnv( &disp, 0, 0, 320, 240 ) );
	setRECT( &rect, 0, 0, 320, 240 );
	ClearImage( &rect, 60, 120, 120 );
	SetDispMask( 1 );

	status = 1;
	while( !( ( padd = padd_read() ) & PADselect ) ) {
		switch( status ) {
		case 1:
			FntPrint( "                                    \n" );
			FntPrint( " Set CD and press x button          \n" );
			FntPrint( "                                    \n" );
			FntPrint( "                                    \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			if( padd & PADRdown ) {
				status = 2;
			}
			break;
		case 2:
			FntPrint( "                                    \n" );
			FntPrint( " Scanning ...                       \n" );
			FntPrint( "                                    \n" );
			FntPrint( "                                    \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			status = 4;
			break;
		case 4:
			switch( DsGetDiskType() ) {
			case DslCdromFormat:
				status = 5;
				break;
			case DslOtherFormat:
				status = 6;
				break;
			case DslStatShellOpen:
				status = 7;
				break;
			case DslStatNoDisk:
				status = 8;
				break;
			}
			break;
		case 5:
			FntPrint( "                                    \n" );
			FntPrint( " PlayStation format                 \n" );
			FntPrint( "                                    \n" );
			FntPrint( " press any button                   \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			if( padd )
				status = 1;
			break;
		case 6:
			FntPrint( "                                    \n" );
			FntPrint( " other format                       \n" );
			FntPrint( "                                    \n" );
			FntPrint( " press any button                   \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			if( padd )
				status = 1;
			break;
		case 7:
			FntPrint( "                                    \n" );
			FntPrint( " shell is opened                    \n" );
			FntPrint( "                                    \n" );
			FntPrint( " press any button                   \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			if( padd )
				status = 1;
			break;
		case 8:
			FntPrint( "                                    \n" );
			FntPrint( " no disk                            \n" );
			FntPrint( "                                    \n" );
			FntPrint( " press any button                   \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			if( padd )
				status = 1;
			break;
		default:
			break;
		}
		VSync( 0 );
	}

	DsControlB( DslPause, 0, 0 );
	DsClose();
	PadStop();
	ResetGraph( 1 );
	StopCallback();
	return 0;
}

/*
 * mask pressed button until it is released */
static u_long padd_read( void )
{
	u_long padd, ret;
	static u_long padd_mask = 0;

	padd = PadRead( 1 );
	padd_mask &= padd;
	ret = padd & ~padd_mask;
	padd_mask |= padd;
	return ret;
}

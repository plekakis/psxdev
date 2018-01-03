/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*				tuto11
 *			CD 交換時のチェック
 *
 *	Copyright(C) 1996 Sony Computer Entertainment Inc.
 *			All Rights Reserved
 *
 *	Version		Date		Design
 *	------------------------------------------
 *	1.00		Sep. 04, 1996	makoto
 *
 */

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <libetc.h>

int cdDiskReady( int mode );
int cdGetDiskType( void );

int main( void )
{
	int ret = -1;
	unsigned long padd;
	RECT rect;
	DRAWENV draw;
	DISPENV disp;

	int status = 0;
	char com;

	/* reset PAD : コントローラのリセット */
	PadInit( 0 );

	/* reset graphics system : グラフィックシステムのリセット */
	ResetGraph( 0 );

	/* reset CD-ROM system : CD-ROM システムのリセット */
	CdInit();

	/* load basic Font patterns : 基本フォントパターンをロード */
	FntLoad( 960, 256 );

	/* : フォントの表示位置の設定 */
	SetDumpFnt( FntOpen( 16, 64, 0, 0, 1, 512 ) );

	/* initialize drawing/display environment : 画面プリントのための準備 */
	PutDrawEnv( SetDefDrawEnv( &draw, 0, 0, 320, 240 ) );
	PutDispEnv( SetDefDispEnv( &disp, 0, 0, 320, 240 ) );
	setRECT( &rect, 0, 0, 320, 240 );
	ClearImage( &rect, 60, 120, 120 );
	SetDispMask( 1 );

	status = 0;
	while( 1 ) {
		padd = PadRead( 1 );
		if( padd & PADselect )
			break;			/* exit */

		switch( status ) {
		case 0:			/* change CD speed to NORMAL */
			com = 0;
			CdControlB( CdlSetmode, &com, 0 );
			VSync( 3 );
			status = 1;
			break;
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
			status = 3;
			break;
		case 3:			/* wait until CD is ready */
			switch( cdDiskReady( 0 ) ) {
			case CdlComplete:
				status = 4;
				break;
			case CdlDiskError:
				status = 8;
				break;
			case CdlStatShellOpen:
				status = 7;
				break;
			}
			break;
		case 4:			/* check if CD is PlayStation format */
			switch( cdGetDiskType() ) {
			case CdlComplete:
				status = 5;
				break;
			case CdlOtherFormat:
				status = 6;
				break;
			case CdlStatNoDisk:
				status = 8;
				break;
			}
			break;
		case 5:
			FntPrint( "                                    \n" );
			FntPrint( " PlayStation format                 \n" );
			FntPrint( "                                    \n" );
			FntPrint( " press Start button to normal speed \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			if( padd & PADstart )
				status = 0;
			break;
		case 6:
			FntPrint( "                                    \n" );
			FntPrint( " other format                       \n" );
			FntPrint( "                                    \n" );
			FntPrint( " press Start button to normal speed \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			if( padd & PADstart )
				status = 0;
			break;
		case 7:
			FntPrint( "                                    \n" );
			FntPrint( " shell is opened                    \n" );
			FntPrint( "                                    \n" );
			FntPrint( " press Start button to normal speed \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			if( padd & PADstart )
				status = 0;
			break;
		case 8:
			FntPrint( "                                    \n" );
			FntPrint( " no disk                            \n" );
			FntPrint( "                                    \n" );
			FntPrint( " press Start button to normal speed \n" );
			FntPrint( "                                    " );
			FntFlush( -1 );
			if( padd & PADstart )
				status = 0;
			break;
		default:
			break;
		}
		VSync( 0 );
	}

	PadStop();
	return 0;
}


/*
 * following functions are same as the functions in libcd.lib
 */

#define SpindleTimeOut	10
int cdDiskReady( int mode )
{
	unsigned char result[ 8 ];
	int i, ret;

	CdControlB( CdlNop, 0, result );	/* check shell open */
	if( result[ 0 ] & CdlStatShellOpen )
		return CdlStatShellOpen;
	ret = CdControlB( CdlGetTN, 0, result );
	if( mode == 1 ) {			/* non block mode */
		if( result[ 0 ] == 0x02 && ret ) {
			return CdlComplete;
		} else {
			return CdlDiskError;
		}
	} else {				/* block mode */
		for( i = 0; i < SpindleTimeOut; i++ ) {
			if( result[ 0 ] & 0x02 ) {
				while( !( result[ 0 ] == 0x02 && ret ) ) {
					VSync( 30 );
					ret = CdControlB( CdlGetTN, 0, result );
				}
				return CdlComplete;
			}
			VSync( 30 );
			ret = CdControlB( CdlGetTN, 0, result );
		}
	}
	return CdlDiskError;
}

#define RETRY_COUNT	10
int cdGetDiskType( void )
{
	CdlLOC pos;
	unsigned char buf[ 2048 ];
	unsigned char result[ 8 ];
	unsigned char com = CdlModeSpeed;

	int ret, retry;

	CdControl( CdlNop, 0, result );		/* check shell open */
	if( result[ 0 ] & CdlStatShellOpen )
		return CdlStatShellOpen;

	CdIntToPos( 0x10, &pos );
	CdControl( CdlSetmode, &com, 0 );
	VSync( 3 );
	CdControl( CdlReadS, ( unsigned char* )&pos, 0 );
	retry = 0;
	while( ( ret = CdReady( 0, result ) ) != CdlDataReady ) {
		if( ++retry >= RETRY_COUNT )
			break;			/* timeout */
		CdControl( CdlReadS, ( unsigned char* )&pos, 0 );
	}
	if( ret != CdlDataReady ) {
		if( result[ 0 ] & CdlStatShellOpen )
			return CdlStatShellOpen;
		else if( result[ 0 ] & 0x01 && result[ 1 ] & 0x40 ) {
			printf( "Command Error: " );
			return CdlOtherFormat;
		} else if( result[ 0 ] & 0x02 )	/* check spindle */
			return CdlOtherFormat;
		else
			return CdlStatNoDisk;
	}
	CdControl( CdlPause, 0, 0 );
	CdGetSector( &buf[ 0 ], 2048 / 4 );
	if( strncmp( &buf[ 1 ], "CD001", 5 ) != 0 )
		return CdlOtherFormat;
	else
		return CdlCdromFormat;
}

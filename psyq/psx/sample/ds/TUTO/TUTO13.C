/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *   tuto1
 *   Pre-seek verification (logical (XA) track)
 *
 *   Copyright  (C)  1997 Sony Computer Entertainment Inc.,
 *   All rights reserved.
 * */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

static u_long padd_read( void );

int main( void )
{
	u_long	padd;

	int pre_seek = 0;
	DslFILE file;
	DslLOC startpos;
	DslFILTER filter;
	int i, cntflag = 0;
	u_char status;
	int start, response = 0;
	int id = 0;

	ResetGraph( 0 );
	PadInit( 0 );

	FntLoad( 960, 256 );
	SetDumpFnt( FntOpen( 16, 16, 320, 200, 0, 512 ) );

	DsInit();
	DsSetDebug( 0 );

	/* get file position */
	if( DsSearchFile( &file, "\\XDATA\\XA\\MULTI8.XA;1" ) == 0 ) {
		printf( "file not found\n" );
		goto abort;
	}

	startpos.minute = 0;
	startpos.second = 2;
	startpos.sector = 0;

	filter.file = 1;
	filter.chan = 0;

	DsControl( DslSetfilter, ( u_char* )&filter, 0 );

	while( ( ( padd = padd_read() ) & PADselect ) == 0 ) {
		balls();

		/* goto start position */
		if( padd & PADRleft ) {
			if( pre_seek ) {
				id = DsPacket(
				  DslModeSpeed | DslModeRT | DslModeSF,
				  &file.pos, DslSeekL, 0, -1 );
			} else {
				id = DsPacket(
				  DslModeSpeed | DslModeRT | DslModeSF,
				  &startpos, DslSeekL, 0, -1 );
			}
		}

		/* tune from next channel */
		if( padd & PADLright ) {
			if( ++filter.chan > 7 )
				filter.chan = 7;
			DsControl( DslSetfilter, ( u_char* )&filter, 0 );
		}

		/* tune from previous channel */
		if( padd & PADLleft ) {
			if( filter.chan != 0 )
				filter.chan--;
			DsControl( DslSetfilter, ( u_char* )&filter, 0 );
		}

		/* begin playing */
		if( padd & PADRdown ) {
			if( pre_seek ) {
				DsCommand( DslReadS, 0, 0, -1 );
			} else {
				DsPacket(
				  DslModeSpeed | DslModeRT | DslModeSF,
				  &file.pos, DslReadS, 0, -1 );
			}
			start = VSync( -1 );
		}

		/* change with toggle */
		if( padd & PADRup ) {
			pre_seek = pre_seek ? 0 : 1;
		}

		status = DsStatus();
		if( status & DslStatSeek )
			cntflag = 1;
		if( cntflag && ( status & DslStatRead ) ) {
			response = VSync( -1 ) - start;
			cntflag = 0;
		}

		FntPrint( "\n\n" );
		FntPrint( "\tPRE SEEK:      %s\n", pre_seek ? " ON" : "OFF" );
		FntPrint( "\t\t" );
		for( i = 0; i < 8; i++ )
			FntPrint( "~c%s%d",
			  i == filter.chan ? "888" : "444", i );
		FntPrint( "~c888\n" );
		FntPrint( "\tQUEUE LENGTH   %3d\n", DsQueueLen() );
		FntPrint( "\tSTATUS          %02x\n", status );
		FntPrint( "\tTIME TO START  %3d\n", response );
		if( DsSync( id, 0 ) == DslComplete )
			FntPrint( "\tSEEK COMPLETE\n" );
		FntFlush( -1 );
		VSync( 0 );
	}

abort:
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

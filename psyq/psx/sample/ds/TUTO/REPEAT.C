/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			repeat: CD-DA/XA repeat
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Sep.12,1994	suzu
 *		1.10		Oct,24,1994	suzu
 *		2.00		Feb,02,1995	suzu
 *		3.00		Apr.22,1997	makoto
*/
/*			    Repeat Play
 *--------------------------------------------------------------------------
 * dsRepeat	Auto repeat play of CD-DA track
 *
 * SYNOPSIS	int dsRepeat(int startp, int endp)
 *
 * ARGUMENT	startp	start position
 *		endp	end position
 *
 * DESCRIPTION	backgound repeat play the audio sector between
 *		'startp' and 'endp'
 *
 * RETURN	always 0
 *
 * NOTE		ReportMode is used for faster position detection.
 *
 *--------------------------------------------------------------------------
 * dsRepeatXA	Auto repeat play of CD-XA track
 *
 * SYNOPSIS	int dsRepeatXA(int startp, int endp)
 *
 * ARGUMENT	startp	start position
 *		endp	end position
 *
 * DESCRIPTIOS	backgound repeat play the audio sector between
 *		'startp' and 'endp'
 *
 * RETURN	always 0
 *
 * NOTE		Since VSyncCallback() is used for background position
 *		detection, be careful when you use VSyncCallback.
 *		For double speed CD-XA only
 *		Call DslSetfilter before start playing if you use
 *		multi-channel CD-XA track.
 *--------------------------------------------------------------------------
 * dsGetPos	get current position of the playing CD-ROM
 *
 * SYNOPSIS	int dsGetPos(void)
 *
 * ARGUMENT	none
 *
 * DESCRIPTION	get the current playing postion (sector number)
 *
 * RETURNS	current playing sector postion
 *
 *--------------------------------------------------------------------------
 * dsGetRepPos	get the total repeated times
 *
 * SYNOPSIS	int dsGetRepTime()
 *
 * ARGUMENT	none
 *
 * DESCRIPTION	get the total repeated times for timeout monitoring.
 *
 * RETURNS	total repeate times
 *-------------------------------------------------------------------------- */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

/* position underflow insurance */
#define SP_MARGIN	( 4 * 75 )	/* 4sec */

/* polling period   */
#define	XA_FREQ		32

static int StartPos, EndPos;	/* start/end point */
static int CurPos;		/* current position */
static int RepTime;		/* current repeat times */

/* callback of DslDataReady */
static void cbready( u_char intr, u_char* result );

/* callback of VSync */
static void cbvsync( void );

/* callback using DslGetlocP */
static void cbsync( u_char intr, u_char* result );

static int dsplay( u_char mode, u_char com );

int dsRepeat( int startp, int endp )
{
	StartPos = startp;
	EndPos = endp;
	CurPos = StartPos;
	RepTime = 0;

	DsReadyCallback( cbready );
	dsplay( DslModeRept | DslModeDA, DslPlay );

	return 0;
}

int dsRepeatXA( int startp, int endp )
{
	StartPos = startp;
	EndPos = endp;
	CurPos = StartPos;
	RepTime = 0;

	VSyncCallback( cbvsync );
	dsplay( DslModeSpeed | DslModeRT | DslModeSF, DslReadS );

	return 0;
}

int dsGetPos( void )
{
	return CurPos;
}

int dsGetRepTime( void )
{
	return RepTime;
}

/*
 * callback used in dsRepeat() */
static void cbready( u_char intr, u_char* result )
{
	DslLOC loc;

	if( intr == DslDataReady ) {
		if( ( result[ 4 ] & 0x80 ) == 0 ) {
			loc.minute = result[ 3 ];
			loc.second = result[ 4 ];
			loc.sector = 0;
			CurPos = DsPosToInt( &loc );
		}
		if( CurPos > EndPos || CurPos < StartPos - SP_MARGIN )
			dsplay( DslModeRept | DslModeDA, DslPlay );
	} else
		dsplay( DslModeRept | DslModeDA, DslPlay );
}

/*
 * callback used in dsRepeatXA() */
static void cbvsync( void )
{
	if( VSync( -1 ) % XA_FREQ )
		return;

	if( CurPos > EndPos || CurPos < StartPos - SP_MARGIN )
		dsplay( DslModeSpeed | DslModeRT | DslModeSF, DslReadS );
	else
		DsCommand( DslGetlocP, 0, cbsync, 0 );
}

/*
 * callback using DslGetlocP */
static void cbsync( u_char intr, u_char* result )
{
	int cnt;

	if( intr == DslComplete ) {
		cnt = DsPosToInt( ( DslLOC* )&result[ 5 ] );
		if( cnt > 0 )
			CurPos = cnt;
	}
}

static int dsplay( u_char mode, u_char com )
{
	DslLOC loc;
	int id;

	DsIntToPos( StartPos, &loc );
	id = DsPacket( mode, &loc, com, 0, -1 );
	CurPos = StartPos;
	RepTime++;
	return id;
}

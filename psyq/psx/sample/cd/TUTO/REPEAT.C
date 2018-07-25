/* $PSLibId: Run-time Library Release 4.4$ */
/*			repeat: CD-DA/XA repeat
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Sep.12,1994	suzu
 *		1.10		Oct,24,1994	suzu
 *		2.00		Feb,02,1995	suzu
*/
/*			    Repeat Play
 *--------------------------------------------------------------------------
 * cdRepeat	Auto repeat play of CD-DA track
 *
 * SYNOPSIS	int cdRepeat(int startp, int endp)
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
 * cdRepeatXA	Auto repeat play of CD-XA track
 *
 * SYNOPSIS	int cdRepeatXA(int startp, int endp)
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
 *		Call CdlSetfilter before start playing if you use
 *		multi-channel CD-XA track.
 *--------------------------------------------------------------------------
 * cdGetPos	get current position of the playing CD-ROM
 *
 * SYNOPSIS	int cdGetPos(void)
 *
 * ARGUMENT	none
 *
 * DESCRIPTION	get the current playing postion (sector number) 
 *
 * RETURNS	current playing sector postion
 *	
 *--------------------------------------------------------------------------
 * cdGetRepPos	get the total repeated times
 *
 * SYNOPSIS	int cdGetRepTime()
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
#include <libcd.h>

/* position underflow insurance*/
#define SP_MARGIN	(4*75)	/* 4sec */

/* polling period   */
#define	XA_FREQ	32/*15*/			

static int	StartPos, EndPos;	/* start/end point*/
static int	CurPos;			/* current position*/
static int	RepTime;		/* current repeat times*/

/* callback of CdlDataReady*/
static void cbready(u_char intr, u_char *result);	

/* callback of VSync*/
static void cbvsync(void);			

static cdplay(u_char com);

int cdRepeat(int startp, int endp)
{
	u_char	param[4];

	StartPos = startp;
	EndPos   = endp;
	CurPos   = StartPos;
	RepTime  = 0;

	param[0] = CdlModeRept|CdlModeDA;
	CdControlB(CdlSetmode, param, 0);
	VSync( 3 );

	CdReadyCallback(cbready);
	cdplay(CdlPlay);

	return(0);
}

int cdRepeatXA(int startp, int endp)
{
	u_char	param[4];

	StartPos = startp;
	EndPos   = endp;
	CurPos   = StartPos;
	RepTime  = 0;

	param[0] = CdlModeSpeed|CdlModeRT|CdlModeSF;
	CdControlB(CdlSetmode, param, 0);
	VSync( 3 );

	VSyncCallback(cbvsync);
	cdplay(CdlReadS);

	return(0);
}

int cdGetPos()
{
	return(CurPos);
}

int cdGetRepTime()
{
	return(RepTime);
}

/*
 * callback used in cdRepeat()*/
static void cbready(u_char intr, u_char *result)
{
	CdlLOC	pos;
	if (intr == CdlDataReady) {
		if ((result[4]&0x80) == 0) {
			pos.minute = result[3];
			pos.second = result[4];
			pos.sector = 0;
			CurPos = CdPosToInt(&pos);
		}
		if (CurPos > EndPos || CurPos < StartPos - SP_MARGIN) 
			cdplay(CdlPlay);
	}
	else {
		/*printf("cdRepeat: error:%s\n", CdIntstr(intr));*/
		while (cdplay(CdlPlay) != 0);
	}
}	


/*
 * callback used in cdRepeatXA()*/
static void cbvsync(void)
{
	u_char		result[8];
	int		cnt, ret;
	
	if (VSync(-1)%XA_FREQ)	return;
	
	if ((ret = CdSync(1, result)) == CdlDiskError) {
		/*printf("cdRepeatXA: DiskError\n");*/
		cdplay(CdlReadS);
	}
	else if (ret == CdlComplete) {
		if (CurPos > EndPos || CurPos < StartPos - SP_MARGIN) 
			cdplay(CdlReadS);
		else {
			if (CdLastCom() == CdlGetlocP &&
			    (cnt = CdPosToInt((CdlLOC *)&result[5])) > 0)
				CurPos = cnt;
			CdControlF(CdlGetlocP, 0);
		}
	}
}


static cdplay(u_char com)
{
	CdlLOC	loc;
	
	CdIntToPos(StartPos, &loc);
	if (CdControl(com, (u_char *)&loc, 0) != 1)
		return(-1);
	
	CurPos = StartPos;
	RepTime++;
	return(0);
}


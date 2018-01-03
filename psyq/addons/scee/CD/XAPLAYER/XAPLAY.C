/******************************************************************************
XAplay.c

	CD-ROM interface routines

	multi stream XA test
	by Buzz Burrowes
	Sony Interactive Studios America


******************************************************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include "xaplay.h"


static int      StartPos, EndPos;
static int      CurPos, XAAddon;

static void cbready(int intr, u_char *result);
static cdplay(u_char com);

static int gPlaying = 0;

u_long oldreadycallback= 0;

void PrepareXA(void)
{
	u_char param[4];

	/* setup for XA playback... */
	param[0] = CdlModeSpeed|CdlModeRT|CdlModeSF;
	CdControlB(CdlSetmode, param, 0);
	CdControlF(CdlPause,0);
	oldreadycallback = CdReadyCallback(cbready);
}

int PlayXA(int startp, int endp, int index, int addon)
{
	CdlFILTER filt;

	StartPos = startp;
	EndPos   = endp;
	CurPos   = StartPos;
	XAAddon  = addon;

	filt.file=1;
	filt.chan=index;

	CdControlF(CdlSetfilter, (u_char *)&filt);

	cdplay(CdlReadN);
	gPlaying=1;
}

int PlayingXA(void)
{
	return(gPlaying);
}

void UnprepareXA(void)
{
	u_char param[4];

	CdReadyCallback((void *)oldreadycallback);
	CdControlF(CdlStop,0);

	/* clear mode... */
	param[0] = CdlModeSpeed;
	CdControlB(CdlSetmode, param, 0);

}

static void cbready(int intr, u_char *result)
{
	if (intr == CdlDataReady)
	{
		CurPos+=XAAddon;
		if (CurPos > EndPos || CurPos < StartPos)
		{
			CdControlF(CdlPause,0);
			gPlaying=0;
		}

	}
}

static cdplay(u_char com)
{
	CdlLOC  loc;

	CdIntToPos(StartPos, &loc);
	CdControlF(com, (u_char *)&loc);
	CurPos = StartPos;
}




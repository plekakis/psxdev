/******************************************************************************
main.c

	test CD-ROM interface routines in xaplay.c

	multi stream XA test
	by Buzz Burrowes
	Sony Interactive Studios America
******************************************************************************/
#include <sys/types.h>
#include <rand.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include "xaplay.h"
#include <libsn.h>

void DoRandomAnn();

unsigned long randseed = 1;

typedef struct {
	int start;
	int end;
} XA_TRACK;

#define XA_TRACKS 1 /* number of XA files in the bank */
XA_TRACK XATrack[XA_TRACKS]; /* structures which hold start/end times */
#define INDEXES_IN_XA   1 /* number of XA streams in each XA file */
#define TOTAL_TRACKS    (XA_TRACKS*INDEXES_IN_XA)
int gLastID; /* last ID played... */
int gFileID; /* was in this file... */
int gIndexID; /* at this index (stream in file). */

char *TrackName[] = {
        "\\XA\\MULTI8.XA;1",
        "\\XA\\MULTI8.XA;1",
        "\\XA\\MULTI8.XA;1",
        "\\XA\\MULTI8.XA;1",
        "\\XA\\MULTI8.XA;1",
        "\\XA\\MULTI8.XA;1",

/*
        "\\MUSIC2.XA;1",
	"\\MUSIC3.XA;1",
	"\\MUSIC4.XA;1",
	"\\MUSIC5.XA;1",
        "\\MUSIC5.XA;1",
*/
	}; /* file names for use by CdSearchFile */

main()
{
	u_char  param[4], result[8];
	CdlFILE fp;
	int     padd, opadd, n;
	u_char  p_com;
	int x;

	/* init sound and CD-ROM... */
	SsInit();
	CdInit();

	PadInit(0);
	ResetGraph(0);
	CdSetDebug(0);

	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 320, 200, 0, 512));

	/* find each XA track in the bank... */
	for(x=0;x<XA_TRACKS;x++)
	{
		while (CdSearchFile(&fp, TrackName[x]) == 0)
			printf("%s: not found\n", TrackName[x]);
		XATrack[x].start = CdPosToInt(&fp.pos);
		XATrack[x].end = CdPosToInt(&fp.pos) + ((fp.size)/2340);
	}

		/* get the player spinning and ready... */
	PrepareXA();

	/* main loop ... */
	while (((padd = PadRead(1))&PADk) == 0) {
		if(randseed!=0) randseed++;
		balls(); /* Sony's speed test ?!? */

		if (opadd == 0 && padd&PADLup)
			DoRandomAnn();

		opadd = padd;

		/* show info... */
		FntPrint("\t\t XA-AUDIO TEST\n\n");
		FntPrint("ID = %d\n\n", gLastID);
		FntPrint("File = %s\n", TrackName[gFileID]);
		FntPrint("Index = %d\n", gIndexID);
		if(PlayingXA())
			FntPrint("Playing\n");
		else
			FntPrint("Paused\n");
		FntFlush(-1);
	pollhost();
	}
	/* stop the CD-ROM... */
	UnprepareXA();
      abort:
	/* shut down... */
	CdSync(0, 0);
	StopCallback();
	PadStop();
	return(0);
}

void DoRandomAnn()
{       
	int theAdd;

	if(randseed!=0)
	{       /* first time so seed random number generator... */
		srand(randseed);
		randseed=0;
	}
	/* get random id... */
	gLastID = ((TOTAL_TRACKS * rand())/RAND_MAX)+1;
	gFileID = (gLastID-1)/8;
	gIndexID = (gLastID - (gFileID*8))-1;
	gFileID = 1;
	if(gIndexID>INDEXES_IN_XA-1) gIndexID=INDEXES_IN_XA-1;
	if(gFileID>XA_TRACKS-1) gFileID>XA_TRACKS-1;
	/* play it... */
	if (gFileID < 2) 
		theAdd = 8;
	else
		theAdd = 4;
	PlayXA(XATrack[gFileID].start,XATrack[gFileID].end,gIndexID, theAdd);
}

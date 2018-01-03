/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			simple sound driver
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00		1994		okita
 *	2.00		1995/08/28	suzu
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libsnd.h>

#define MVOL		127			/* main volume */
#define SVOL		64			/* seq data volume */

short vab;	/* vab data id */
short seq;	/* seq data id */

sndInit(void)
{
	extern u_long strings_vh[];
	extern u_long strings_vb[];
	extern u_long mozart_seq[];
	
	static char stab[SS_SEQ_TABSIZ*4*5]; 
	
	SsInit();			
	SsSetTableSize(stab, 4, 5); 
	SsSetTickMode(SS_TICK60);	/* set tick mode = TICK60 */
	/* SsSetTickMode(SS_TICK240);	/* set tick mode = TICK240 */

	if ((vab = SsVabOpenHead ((u_char *)strings_vh, -1)) == -1) {
		printf ("SsVabOpenHead : failed !!!\n");
		return;
	}
	if (SsVabTransBody ((u_char *)strings_vb, vab) != vab) {
		printf ("SsVabTransBody : failed !!!\n");
		return;
	}
	SsVabTransCompleted (SS_WAIT_COMPLETED);
	seq = SsSeqOpen(mozart_seq, vab);		/* open seq data */
	SsStart();					/* start sound */
	SsSetMVol(MVOL, MVOL);				/* set main volume */
	SsSeqSetVol(seq, SVOL, SVOL); 			/* set data volume */
	SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY);
}

sndMute(int mode)
{
	if (mode)	/* mute */	
		SsSeqSetVol(seq, 0, 0);
	else
		SsSeqSetVol(seq, SVOL, SVOL);
}

/* This may be never called..*/
sndEnd(void)
{
	SsSeqClose(seq);		/* close seq data */
	SsVabClose(vab);		/* close vab data */
	SsEnd();			/* sound system end */
	SsQuit();
}

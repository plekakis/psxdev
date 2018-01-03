/* $PSLibId: Runtime Library Release 3.6$ */
/*		tuto3: Audio repeat using CdlDataEnd
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Jul.07,1995	suzu
 *
 *	
 *			    Repeat Play
 *		auto repeat play using CdlDataEnd
 *
 *	Auto repeat using CdlDataEnd is fast because it requires less
 *	interrupts from CD-ROM. But the track ID is needed to all
 *	repeating point, and total number of tracks is limitted to 100,
 *	If it is acceptable, this repeating strategy is the simplest.
 *	
 :			   リピート再生
 *		CdlDataEnd 割り込みを使ったリピート
 *
 *	少ない割り込みでリピートできる。ただしリピート単位でトラック番号を別に
 *	しなくてはならない。それが可能ならばもっとも簡単なリピート再生
 *
 */
	
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "libcd.h"

CdlLOC	loc[100];
int	errcnt = 0;
int	nplay  = 0;
int	curp;
int	ntoc;
int	track[] = {7, 2, 6, 3, 5, 4, 0};
int	track_id = 0;

static print_gage(int startp, int endp, int curp);
static play(int id);
static forward(void);
static check_play(u_char *result);
static void cbdataready(u_char intr, u_char *result);

char *title = "    SELECT MENU    ";
static char *menu[] = {"Normal", "Double", 0}; 

main()
{
	u_long	padd;
	u_char	param[4], result[8];
	char	*str;
	int	i;
	
	/* initialize */
	ResetGraph(0);
	PadInit(0);
	menuInit(0, 80, 96, 256);
	SetDumpFnt(FntOpen(32, 32, 320, 200, 0, 512));
	CdInit();
	CdSetDebug(0);
	
	/* read TOC: TOC を読む */
	if ((ntoc = CdGetToc(loc)) == 0) {
		printf("No TOC found: please use CD-DA disc...\n");
		goto abort;
	}
	
#ifdef TOCBUG	
	for (i = 1; i < ntoc; i++) 
		CdIntToPos(CdPosToInt(&loc[i]) - 74, &loc[i]);
#endif
	
	/* start playing: 最初の曲を演奏 */
	param[0] = CdlModeAP|CdlModeDA;
	CdControl(CdlSetmode, param, 0);
	
	/* define callback: コールバックを追加 */
	CdReadyCallback(cbdataready);

	play(track_id);
	while (((padd = PadRead(1))&PADselect) == 0) {
		
		balls();		
		
		switch (menuUpdate(title, menu, padd)) {
		    case 0:	/* normal speed */
			param[0] &= ~CdlModeSpeed;
			CdControl(CdlSetmode, param, 0);
			VSync( 3 );
			break;
		    case 1:	/* double speed */
			param[0] |= CdlModeSpeed;
			CdControl(CdlSetmode, param, 0);
			VSync( 3 );
			break;

		    default:	/* if no key pushed, check playing */
			check_play(result);	
		}
		
		/* print status: ステータスの表示 */
		FntPrint("CD-DA REPEAT (DataEnd detect)\n\n");
		FntPrint("Use Audio DISC for this test\n\n");
		FntPrint("stat=%02x,errors=%d/%d\n", result[0], errcnt, nplay);
		FntPrint("playing=");
		for (i = 0; track[i] != 0; i++) {
			str = (i==track_id)?(VSync(-1)&0x20?"888":"880"):"444";
			FntPrint("~c%s%d", str, track[i]);
		}
		FntPrint("\n");
		print_gage(CdPosToInt(&loc[track[track_id]]),
			   CdPosToInt(&loc[track[track_id]+1]),
			   curp);
		FntFlush(-1);
	}
    abort:
	CdControlB(CdlPause, 0, 0);
	CdFlush();
	ResetGraph(1);
	PadStop();
	StopCallback();
	return;
}

/* callback which is called by the CdlDataEnd interrupt
 : CdlDataEnd で発生するコールバック
 */
static void cbdataready(u_char intr, u_char *result)
{
	if (intr == CdlDataEnd) {
		printf("cbdataready: CdlDataEnd (track=%d,time=%d)\n",
		       track_id, VSync(-1));
		if (track[++track_id] == 0)
			track_id = 0;
		
		play(track_id);
	}
	else if (CdlDiskError) {
		printf("cbdataready error:%s\n", CdIntstr(intr));
		errcnt++;
	}
}

static check_play(u_char *result)
{
	int com, ret;
	
	if ((ret = CdSync(1, result)) == CdlComplete) {
		/* check seeking/playing status */
		if ((com = CdLastCom()) == CdlNop) {
			if ((result[0]&0xc0) == 0) {
				printf("check_play error(%02x)\n", result[0]);
				errcnt++;
				play(track_id);
			}
			else 
				CdControlF(CdlGetlocP, 0);
		}
		else {
			curp = CdPosToInt((CdlLOC *)&result[5]);
			CdControlF(CdlNop, 0);
		}
	}
}

static play(int id)
{
	nplay++;
	if (track[id] > ntoc) {
		printf("%d: track overflow\n", track[id]);
		track[id] = ntoc;
	}
	CdControl(CdlPlay, (u_char *)&loc[track[id]], 0);
}

static print_gage(int startp, int endp, int curp)
{
	
	int i = 0, rate;
	
	rate = 32 * (curp - startp)/(endp - startp);

	FntPrint("~c444");
	while (i++ < rate) FntPrint("*");
	FntPrint((VSync(-1)&0x08)? "~c888*~c444": "*");
	while (i++ < 32)   FntPrint("*");
}

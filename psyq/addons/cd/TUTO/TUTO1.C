/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto1: CD player (2)
 *			
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Jul.29,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *	
 *		simplest CD-Player (interrupt type)
 *	This program can play the normal audio CD discs with balls
 *
 :		  最も単純なCD-Player（割り込み型）
 *		メディアは、AUDIO CD を使用します。	
 *			
 *	メカコンは十分賢いので実はほとんど割り込みは必要ないが、
 *	ここでは、デバッグのために無理に割り込みを使用する
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "libcd.h"

static int	track, min, sec, level;	/* for callbacks */
static int	is_bg_error = 0;	/* background error flag */

#define cdSetVol(vol, v) \
	(vol)->val0=(vol)->val2=v,(vol)->val1=(vol)->val3=0,CdMix(vol)

char *title = "    SELECT MENU    ";
static char *menu[] = {
	"Foreward",	"Backword",	"Play",		"Pause",
	"Mute",		"Demute",	"Next Track",	"Prev Track",
	"Last Track",	"First Track",	"Volume Up",	"Volume Down",
	0,
};

static void cbcomplete(u_char intr, u_char *result);
static void cbdataready(u_char intr, u_char *result);

main()
{
	CdlLOC	loc[100];
	CdlATV	atv;
	int	vol = 0x80;
	u_long	padd;
	u_char	param[4], result[8];
	int	i, ret, ntoc;
	u_char	mute_com = CdlDemute;	
	
	/* initialize environment */
	ResetGraph(0);
	PadInit(0);		
	menuInit(0, 80, 88, 256);
	SetDumpFnt(FntOpen(16, 16, 320, 200, 0, 512));
	CdInit();
	CdSetDebug(0);
	
	/* set initila volume: ボリューム設定 */
	cdSetVol(&atv, vol);

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
	param[0] = CdlModeRept|CdlModeDA;	
	CdControlB(CdlSetmode, param, 0);	
	CdControlB(CdlPlay, (u_char *)&loc[2], 0);	
	track = 2, min = loc[2].minute, sec = loc[2].second, level = 0; 

	/* hook callbacks: コールバックを追加 */
	CdSyncCallback(cbcomplete);
	CdReadyCallback(cbdataready);
	
	while (((padd = PadRead(1))&PADselect) == 0) {
		
		balls();	/* display */
		
		/* if error is detected, retry to play the first track.
		 : 来ていなければもう一度 Play する
		 */
		if ((ret = CdSync(1, 0)) == CdlDiskError || is_bg_error) {
			CdControl(CdlSeekP, (u_char *)&loc[1], 0);
			is_bg_error = 0;
		}

		FntPrint("CD-PLAYER (interrupt version)\n\n");
		FntPrint("Use Audio DISC for this test\n\n");
		FntPrint("pos =(%2d:%02x:%02x)\n",track, min, sec);
		FntPrint("level =%d\n", level);
		FntPrint("stat =%s(%s)\n",
			 CdIntstr(ret), CdComstr(CdLastCom()));
		FntPrint("vol  =%d(mute=%s)\n", vol,
			 mute_com==CdlMute? "ON":"OFF");
		
		FntFlush(-1);
		
		switch (menuUpdate(title, menu, padd)) {
		    case 0:	CdControl(CdlForward,  0, 0); break;
		    case 1:	CdControl(CdlBackward, 0, 0); break;
		    case 2:	CdControl(CdlPlay,     0, 0); break;
		    case 3:	CdControl(CdlPause,    0, 0); break;
		    case 4:	CdControl((mute_com = CdlMute),   0, 0); break;
		    case 5:	CdControl((mute_com = CdlDemute), 0, 0); break;
			
		    case 6:	/* next track */
			if (++track > ntoc)	track = ntoc;
			CdControl(CdlSeekP,(u_char *)&loc[track], 0);
			break;
			
		    case 7:	/* previous track */
			if (--track < 1)	track = 1;
			CdControl(CdlSeekP,(u_char *)&loc[track], 0);
			break;
			
		    case 8:	/* last track */
			CdControl(CdlSeekP,(u_char *)&loc[ntoc], 0);
			break;

		    case 9:	/* 1st track */
			CdControl(CdlSeekP,(u_char *)&loc[1], 0);
			break;
			
		    case 10:	/* volume up */
			vol += 10; limitRange(vol, 0, 255);
			cdSetVol(&atv, vol);
			break;
			
		    case 11:	/* volume down */
			vol -= 10; limitRange(vol, 0, 255);
			cdSetVol(&atv, vol);
			break;
		}
	}
	
    abort:
	CdControlB(CdlPause, 0, 0);
	CdFlush();
	ResetGraph(1);
	PadStop();
	StopCallback();
	return;
}

/*
 * Callback procedure which is called at the end of primitive commands.
 * If error is detected in the callback, 'is_bg_error' turns to 1.
 : ブロックコマンドの終了で呼ばれるコールバックを定義する
 * バックグラウンドでエラーが検出された場合は、is_bg_error が 1 になる
 */
static void cbcomplete(u_char intr, u_char *result)
{
	if (intr == CdlComplete) {
		if (CdLastCom() == CdlSeekP) 
			CdControl(CdlPlay, 0, 0);
	}
	else {
		printf("cbcomplete error: %s\n", CdIntstr(intr));
		is_bg_error = 1;
	}
}

/*
 * callback procedure which is called at the DataReady interrupt.
 : データレディで呼ばれるコールバックを定義する
 */
static void cbdataready(u_char intr, u_char *result)
{
	if (intr == CdlDataReady) {
		if ((result[4]&0x80) == 0) {
			track = btoi(result[1]);
			min   = result[3];
			sec   = result[4];
			level = (result[6]<<8)|result[7];
		}
	}
	else if (intr == CdlDiskError) {
		printf("cbdataready error:%s\n", CdIntstr(intr));
		is_bg_error = 1;
	}
}



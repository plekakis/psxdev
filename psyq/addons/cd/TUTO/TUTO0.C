/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto0: CD player (1)
 *			
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Jul.29,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *
 *		simplest CD-Player (polling type)
 *	This program plays normal audio CD discs with balls
 *
 :		最も単純なCD-Player （ポーリング型）
 *		メディアは、AUDIO CD を使用します。	
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

#define cdSetVol(vol, v) \
	(vol)->val0=(vol)->val2=v,(vol)->val1=(vol)->val3=0,CdMix(vol)
		 
char *title = "    SELECT MENU    ";
static char *menu[] = {
	"Foreward",	"Backword",	"Play",		"Pause",
	"Mute",		"Demute",	"Next Track",	"Prev Track",
	"Last Track",	"First Track",	"Volume Up",	"Volume Down",
	0,
};

main()
{
	CdlLOC	loc[100];
	CdlATV	atv;
	int	vol = 0x80;
	
	u_long	padd;
	u_char	param[4], result[8];
	int	i, ret, ntoc;
	int	track, min, sec, level;
	u_char	mute_com = CdlDemute;	/* previous MUTE command */
	
	/* initialize graphics and controller */
	ResetGraph(0);
	PadInit(0);		

	/* initialize menu */
	menuInit(0, 80, 88, 256);

	/* open message window */
	SetDumpFnt(FntOpen(16, 16, 320, 200, 0, 512));

	/* initialize CD subsytem: CD サブシステムを初期化 */
	CdInit();
	CdSetDebug(0);
	
	/* set initila volume: ボリューム設定 */
	cdSetVol(&atv, vol);

	/* check DiskStatus */
	CdControl(CdlNop, 0, result);
	if (result[0]&CdlStatShellOpen) {
		if (CdDiskReady(1) != CdlComplete) {
			printf("Shell open waiting ...\n");
			while (CdDiskReady(0) != CdlComplete);
		}
	}
		
	/* read TOC: TOC を読む */
	if ((ntoc = CdGetToc(loc)) == 0) {
		printf("No TOC found: please use CD-DA disc...\n");
		goto abort;
	}
#ifdef TOCBUG	
	/* TOC doesn't have sector information, so the start position
	 *  may differs 74 srctors in the worst
	 : もしも、TOC の情報がずれていたらここで補正してしまう
	 */
	for (i = 1; i < ntoc; i++) 
		CdIntToPos(CdPosToInt(&loc[i]) - 74, &loc[i]);
#endif
	
	/* start playing: 最初の曲を演奏 */
	param[0] = CdlModeRept|CdlModeDA;	
	CdControl(CdlSetmode, param, 0);	
	CdControl(CdlPlay, (u_char *)&loc[2], 0);	
	track = 2, min = loc[2].minute, sec = loc[2].second, level = 0; 
	
	while (((padd = PadRead(1))&PADselect) == 0) {

		balls();	/* display */
		 
		/* check the report from CD-ROM: レポートをチェック */
		if ((ret = CdReady(1, result)) == CdlDataReady) {
			if ((result[4]&0x80) == 0) {
				track = btoi(result[1]);
				min   = result[3];
				sec   = result[4];
				level = (result[6]<<8)|result[7];
			}
		}
		/* if error is detected, retry to play the first track.
		 : 来ていなければもう一度 Play する
		 */
		else if (ret == CdlDiskError) 
			CdControl(CdlPlay, (u_char *)&loc[1], 0);
		
		/* error check; エラーが発生していれば最初から演奏 */
		if ((ret = CdSync(1, 0)) == CdlDiskError) {
			CdControl(CdlPlay, (u_char *)&loc[1], 0);
			FntPrint("CDROM: DiskError. retrying..\n");
		}
			
		FntPrint("CD-PLAYER (polling version)\n\n");
		FntPrint("Use Audio DISC for this test\n\n");
		FntPrint("pos  =(%2d:%02x:%02x)\n",track, min, sec);
		FntPrint("level=%d\n", level);
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
			CdControl(CdlPlay, (u_char *)&loc[track], 0);
			break;
			
		    case 7:	/* previous track */
			if (--track < 1)	track = 1;
			CdControl(CdlPlay, (u_char *)&loc[track], 0);
			break;
			
		    case 8:	/* last track */
			CdControl(CdlPlay, (u_char *)&loc[ntoc], 0);
			break;

		    case 9:	/* 1st track */
			CdControl(CdlPlay, (u_char *)&loc[1], 0);
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


	
	

/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto6: multi channel CD-XA
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Jul.30,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *		1.20		Jun,02,1995	suzu
 *
 *		     interleaved audio/data channel 
 *	We can record 8 interleaved data/audio channels in double speed
 *	CD-ROM disc. This program decodes the disc which contains following
 *	data/audio channels:
 *
 *		0ch-6ch		audio (CD-XA) channel
 *		7ch		data channel
 *
 *	This program reads data on 7th channel with playing one of
 *	7 audio channel, and we can select the audio channel to play.
 *		
 :		    マルチチャンネル CD-XA テストプログラム
 *
 *	CD 倍速再生時には、最大 8ch まで音声・データをインターリーブして
 *	記録することができる。ここでは、以下のデータが記録されているディ
 *	スクを想定して音声・データの再生を行なう。
 *
 *		0ch-6ch		オーディオ(CD-XA)チャンネル
 *		7ch		データチャンネル
 *
 *	7ch 目のデータを読み込みながら 0-6ch のいずれか一つのチャンネルの
 *	オーディオを再生する。
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "libcd.h"

#define SCTSIZE	2048
#define RNGSIZE	8
#define RNGMASK	7

static u_long	sector[RNGSIZE][SCTSIZE/4];
static int	rid = 0, errflag = 0;
static void	notfound(char *file);
static void cbdataready(u_char intr, u_char *result);

main()
{
	u_char		param[4], result[8];
	/* char		*file = "\\DATA\\XA7V.XA;1"; */
	char		*file = "\\XA\\MULTI8.XA;1"; /* lib3.3 */
	CdlFILE		fp;
	CdlFILTER	filter;
	CdlLOC		loc;
	int		padd, opadd, ret, i, j;
	
	/* initialize graphics and controller */
	ResetGraph(0);
	PadInit(0);		
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 320, 200, 0, 512));
	
	CdInit();
	CdSetDebug(0);
	
	/* get file position: ファイルの位置を確定 */
	if (CdSearchFile(&fp, file) == 0) {
		notfound(file);
		goto abort;
	}

	/* kick auto loop: 自動ループスタート */
	cdRepeatXA(CdPosToInt(&fp.pos), CdPosToInt(&fp.pos) + fp.size/2048);
	
	/* set mode */
	param[0] = CdlModeSpeed|CdlModeRT|CdlModeDA|CdlModeSF;
	CdControlB(CdlSetmode, param, 0);
	VSync( 3 );

	/* set playing channel: ADPCM 再生チャネル設定 */
	filter.file = 1;
	filter.chan = 0;
	CdControlB(CdlSetfilter, (u_char *)&filter, result);
	
	/* hook callback: コールバックをフックする */
	CdReadyCallback(cbdataready);
		     
	param[0] = CdlModeSpeed|CdlModeRT|CdlModeDA|CdlModeSF;
	while (((padd = PadRead(1))&PADselect) == 0) {
		
		balls();	
		
		switch (CdSync(1, 0)) {
			
		    case CdlComplete:
		
			/* select channel: 再生チャネル選択 */
			if (opadd == 0 && padd&(PADLright|PADLleft)) {
				filter.chan = (padd&PADLright)?
					(filter.chan+1)%8:(filter.chan+7)%8;

				CdControlB(CdlSetfilter,
					   (u_char *)&filter, result);
			}
			break;
			
		    case CdlDiskError:
			CdControl(CdlReadS, 0, 0);
			break;
		}
		
		/* check repeated count: 位置の獲得 */
		CdIntToPos(cdGetPos(), &loc);
		
		/* print status; ステータスの表示 */
		FntPrint("\tMulti-Channl XA-AUDIO \n\n");
		FntPrint("\tUse DTL-S2002 for this test\n\n");
		FntPrint("position=(%02x:%02x:%02X)\n\n",
			loc.minute, loc.second, loc.sector);
		
		/* display data on 7th channel: 読み込んだデータを表示 */
		for (i = 0; i < RNGSIZE; i++) {
			for (j = 0; j < 3; j++)
				FntPrint("%08x", sector[i][j]);
			FntPrint("\n");
		}
		
		/* display current channel */
		FntPrint("\nchannel=");
		for (i = 0; i < 8; i++)
			FntPrint("~c%s%d",
				 i==filter.chan? "888":"444", i);
		
		
		FntFlush(-1);
		opadd = padd;
	}
	
      abort:
	CdControl(CdlStop, 0, 0);
	CdFlush();
	ResetGraph(1);
	PadStop();
	StopCallback();
	return;
}


/*
 * Callback which is called when CD-ROM data is ready.
 * data on CD-ROM can be read with playing the CD-XA.
 * reading data is stored in the ring buffer.
 : CD-XA  再生しながらインターリーブデータを読み出す
 * インターリーブデータはここではリングバッファに格納される
 */
static void cbdataready(u_char intr, u_char *result)
{
	if (intr == CdlDataReady) {
		CdGetSector(sector[rid], 2048/4);
		rid = ((rid+1)&RNGMASK);
	}
	else 
		errflag++;
}	

static void notfound(char *file)
{
	int n = 60*4;
	while (n--) {
		balls();
		FntPrint("\n\n%s: not found (waiting)\n", file);
		FntFlush(-1);
	}
	printf("%s: not found\n", file);
}


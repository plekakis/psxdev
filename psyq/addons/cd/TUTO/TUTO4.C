/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto4: CdControlF
 *			
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Jul.07,1995	suzu
 *
 *			    CdControlF
 *
 *	CdControlF() is fast because it never waits for acknowledge from
 *	CD-ROM subsystem. We can apply this strategy for CD-XA repeat play.
 *	But to write a reliable code using CdControlF is a little complicate.
 *
 :	CdControlF() はサブシステムとハンドシェークをしないので高速。
 *	またオートリピートを使用しないので CD-XA の再生にも適用できる。
 *	しかしその実装はかなり繁雑になる。
 *
 */
	
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

static play(int ipos);
static void cbvsync(void);

main()
{
	int	max_time, min_time, time;	
	int	next_v = 0;
	u_long	padd;
	CdlLOC	loc[100];
	u_char	param[4], result[8];
	
	/* initialize */
	ResetGraph(0);
	PadInit(0);
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 320, 200, 0, 512));
	CdInit();
	CdSetDebug(0);

	/* read TOC: TOC を読む */
	if (CdGetToc(loc) == 0) {			
		printf("No TOC found: please use CD-DA disc...\n");
		goto abort;
	}

	/* play between 2nd track and the las track
	 : ２トラック－最終トラックで演奏
	 */
	min_time = CdPosToInt(&loc[2]);
	max_time = CdPosToInt(&loc[0]);

	/* set mode: モード設定 */
	param[0] = CdlModeDA;
	CdControl(CdlSetmode, param, 0);	
	
	/* start the first track */
	play(0);

	/* hook callback: コールバック設定 */
	VSyncCallback(cbvsync);
	
	while (((padd = PadRead(1))&PADselect) == 0) {
		
		balls();		
		
		/* track jump in every 5 sec: 5 sec ごとにランダム再生 */
		if (VSync(-1) > next_v) {
			time = min_time + (rand()<<5)%(max_time-min_time);
			play(time);
			next_v = VSync(-1) + 5*60;
		}
		/* print status: ステータスの表示 */
		FntPrint("CD-DA REPEAT (ControlF)\n\n");
		FntPrint("Use Audio DISC for this test\n\n");
		FntPrint("Zapping Play (%d sec)\n", (next_v-VSync(-1))/60);
		
		/* If you don't want to use VSyncCallback() interrupt,
		 * call callback function here by yourself.
		 : VSyncCallback() を使用しない時はここでポーリング
		 */
		/*cbvsync();*/
		
		FntFlush(-1);
	}
    abort:
	VSyncCallback(0);
	CdControlB(CdlPause, 0, 0);
	CdFlush();
	ResetGraph(1);
	PadStop();
	StopCallback();
	return;
}

static int	is_play = 0;	/* play request */
static CdlLOC	pos;		/* play position */
static u_char	p_com = 0;	/* previous command */
static u_char	result[8];	/* result */
static int	errcnt = 0;	/* debug */

/* register the CD play request (which will be executed at the next V-BLNK
 : Play 要求を登録する
 */
static play(int ipos)
{
	CdIntToPos(ipos, &pos);
	is_play = 1;
}
	
/*
 * status flow which is called in V-BLNK. 
 : Vsync の度に呼ばれるステータスマシン
 * 状態遷移図を書くとわかり易い。	
 */
static void cbvsync(void)
{
	int ret;

	switch (p_com) {
	    case 0:		/* IDLE -> Nop or Setloc */
		/*FntPrint("IDLE(%d)\n");*/
		if (is_play) 
			CdControlF((p_com = CdlSetloc), (u_char *)&pos);
		else {
			int f = CdSetDebug(0);
			CdControlF((p_com = CdlNop), 0);
			CdSetDebug(f);
		}
		break;
		
	    case CdlSetloc:	/* Setloc -> Play */
		/* for debug 
		FntPrint("(%02x:%02x:%02x):%d\n",
		       pos.minute, pos.second, pos.sector, VSync(-1));
	        */ 
		if ((ret = CdSync(1, result)) == CdlComplete)
			CdControlF((p_com = CdlPlay), 0);
		else if (ret == CdlDiskError) {
			errcnt++;
			p_com = 0;
		}
		break;
		
	    case CdlPlay:	/* Play -> IDLE */
		if ((ret = CdSync(1, result)) == CdlComplete) {
			p_com = 0;
			is_play = 0;
		}
		else if (ret == CdlDiskError) {
			errcnt++;
			p_com = 0;
		}
		break;
		
	    case CdlNop:	/* Nop -> IDLE */
		if (CdSync(1, result)) {
			if ((result[0]&(CdlStatPlay|CdlStatSeek)) == 0)
				is_play = 1;
			p_com = 0;
		}
		break;
		
	    default:
		printf("%s: unexpected command\n", CdComstr(p_com));
		p_com = 0;
	}
	/* for debug 
	FntPrint("pos=(%02x:%02x:%02x), stat=%02x, errcnt=%d\n",
		pos.minute, pos.second, pos.sector, result[0], errcnt);
	*/
}
		
		
	



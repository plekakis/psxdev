/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto7: simple CdRead
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Aug.05,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *		1.20		Mar,12,1995	suzu
 *		1.30		Feg,12,1996	suzu
 *	
 *				CdRead
 *	This program reads the data recorded in DTS-2190 CD-ROM disc.
 *	The program reads the same data 2 times, and verifis.
 *
 :	DTS-2190 のディスクに格納されている PSX.EXE を２回読みだして
 *	データを比較する。
 *	耐久テストを兼ねて無限回くりかえす。
 *	
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

static void read_test(char *file);
static void notfound(char *file);
static void cbvsync(void);

main()
{
	
	/* initialize graphics and controller */
	ResetGraph(0);
	PadInit(0);
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 64, 0, 0, 1, 512));

	/* initialize CD subsytem: CD サブシステムを初期化 */
	CdInit();
	CdSetDebug(0);
	
	/* initialize sound : サウンドドライバを初期化 */
	sndInit();
	
	/* start display */
	SetDispMask(1);

	/* test loop */
	while ((PadRead(1)&PADselect) == 0)
		read_test("\\DATA\\MOV.STR;1");
	
	/* ending */
	sndEnd();
	CdFlush();
	PadStop();
	StopCallback();
	return;
}


/* maximus mector size: 読み出すセクタサイズ */
#define MAXSECTOR	256		
static void read_test(char *file)
{
	static u_long	sectbuf[2][MAXSECTOR*2048/4];
	static int	n_trial = 0, n_err = 0, n_fatal = 0;
	int		i, cnt;
	CdlFILE		fp;
	int		nsector;
	unsigned char com;
	
	/* update trial counter */
	n_trial++;
	
	/* clear reading buffer: リードバッファを一旦クリア */
	for (i = 0; i < sizeof(sectbuf[0])/4; i++) 
		sectbuf[0][i] = sectbuf[1][i] = 0;
	
	
	/* start reading: バックグラウンドリード */
	for (i = 0; i < 2; i++) {
		if  (CdSearchFile(&fp, file) == 0) {
			notfound(file);
			return;
		}
		    
		/* get file position: ファイルの位置を確定 */
		if ((nsector = (fp.size+2047)/2048) > MAXSECTOR)
			nsector = MAXSECTOR;
		nsector = MAXSECTOR;	/* for debug */
		
		/* start reading: リード開始 */
		CdControl(CdlSetloc, (u_char *)&fp.pos, 0);
		com = CdlModeSpeed;
		CdControlB( CdlSetmode, &com, 0 );
		VSync( 3 );
		CdRead(nsector, sectbuf[i], CdlModeSpeed);

		/* Since CdRead() runs in background. the program can
		 * do another task in foreground.  The current reading 
		 * status can be monitored in CdReadSync().
		 * In this sample, VSync(0) is simply called in foreground.
		 : リードの裏で通常の処理は実行できる。
		 * ここでは、Read が終了するまで残りのセクタ数を監視する
		 */
		while ((cnt = CdReadSync(1, 0)) > 0 ) {
			VSync(0);
			
			FntPrint("\t\t SIMPLE CDREAD\n\n");
			FntPrint("Use DTL-S2002 DISC FOR THIS TEST\n\n");
			FntPrint("file name %s\n", file);
			FntPrint("trial count %d\n", n_trial);
			FntPrint("read  error %d\n", n_err);
			FntPrint("fatal error %d\n\n", n_fatal);
			FntPrint("reading(%d) ...%d Sectors\n", i, cnt);
			
			balls();
			FntFlush(-1);
		}

		/* check retur value: 返り値が非零の場合はエラー */
		if (cnt != 0) {
			FntPrint("Read ERROR in %d\n\n", i);
			n_err++;
			return;
		}
	}
	       
	/* compare: 二つのデータを比較する */
	for (i = 0; i < sizeof(sectbuf[0])/4; i++) 
		if (sectbuf[0][i] != sectbuf[1][i]) {
			printf("verify ERROR at (%08x:%08x)\n\n",
				 &sectbuf[0][i], &sectbuf[1][i]);
			n_fatal++;
			return;
		}
}

static void notfound(char *file)
{
	int n = 60*4;
	while (n--) {
		FntPrint("\n\n%s: not found\n", file);
		FntFlush(-1);
		VSync(0);
	}
	printf("%s: not found\n", file);
}


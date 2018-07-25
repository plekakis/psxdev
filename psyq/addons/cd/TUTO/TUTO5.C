/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto5: CD-XA repeat
 *			
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Jul.30,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *		2.00		Feb,02,1995	suzu
 *
 *			    Repeat Play
 *		auto repeat play among 2 point of CD-XA.
 *
 *	Since we cannot use report mode callback when playing CD-XA
 *	audio, we have to use CdlGetloc command to monitor current
 *	position on CD-ROM. If CdControl is slow, CdControlF is
 *	possible.
 *
 :			    リピート再生
 *		CD-XA トラックの任意の２点間をリピートする。
 *
 *	CD-XA 再生時はレポートモードのコールバックが使用できない。
 *	そのため CdlGetloc で場所を探す必要がある。CdControl() が重い
 *	場合は、CdControlF() を使用する。
 */
	
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

int cdRepeat(int startp, int endp);
int cdRepeatXA(int startp, int endp);
int cdGetPos();
int cdGetRepTime();

char *title = "    SELECT MENU    ";
static char *menu[] = {"Play", "Pause",	"Forward", "Backword", 0};

static print_gage(int startp, int endp, int curp);
static notfound(char *file);
main()
{
	u_char	param[4], result[8];
	/* char	*file = "\\DATA\\XA7V.XA;1"; */
	char	*file = "\\XA\\MULTI8.XA;1"; /* lib3.3 */

	CdlFILE	fp;
	CdlLOC	loc;
	int	padd, n, id;
	int	startp, endp;
	
	/* initialize graphics and controller */
	ResetGraph(0);
	PadInit(0);		
	menuInit(0, 80, 88, 256);
	SetDumpFnt(FntOpen(16, 16, 320, 200, 0, 512));
	
	/* initialize CD subsytem: CD サブシステムを初期化 */
	CdInit();
	CdSetDebug(0);
	
	/* get file position: ファイルの位置を確定 */
	if (CdSearchFile(&fp, file) == 0) {
		notfound(file);
		goto abort;
	}
	
	/* kick auto loop: 自動ループスタート */
	startp = CdPosToInt(&fp.pos);
	endp   = startp + fp.size/2048;
	cdRepeatXA(startp, endp);
	
	while (((padd = PadRead(1))&PADselect) == 0) {
		
		balls();		
		id = menuUpdate(title, menu, padd);
		
		if (CdSync(1, 0) == CdlComplete) {
			switch (id) {
			    case 0: CdControl(CdlReadS,    0, 0); break;
			    case 1: CdControl(CdlPause,    0, 0); break;
			    case 2: CdControl(CdlForward,  0, 0); break;
			    case 3: CdControl(CdlBackward, 0, 0); break;
			}
		}

		/* print status:  ステータスの表示 */
		CdIntToPos(cdGetPos(), &loc);
		FntPrint("\t\t XA-AUDIO Repeat\n\n");
		FntPrint("Use DTL-S2002 for this test.\n\n");
		FntPrint("position=(%02x:%02x:%02X)\n",
			loc.minute, loc.second, loc.sector);
		FntPrint("%s...\n", CdComstr(CdLastCom()));
		print_gage(startp, endp, CdPosToInt(&loc));
		
		FntFlush(-1);
	}
	
	CdControl(CdlStop, 0, 0);
      abort:
	CdControlB(CdlPause, 0, 0);
	CdFlush();
	ResetGraph(1);
	PadStop();
	StopCallback();
	return;
}

static print_gage(int startp, int endp, int curp)
{
	int i = 0, rate;
	
	rate = 32 * (curp - startp)/(endp - startp);

	FntPrint("~c444");
	while (i++ < rate) FntPrint("*");
	FntPrint((VSync(-1)>>4)&0x01? "~c888*~c444": "*");
	while (i++ < 32)   FntPrint("*");
}

static notfound(char *file)
{
	int n = 60*4;
	while (n--) {
		balls();
		FntPrint("\n\n%s: not found (waiting)\n", file);
		FntFlush(-1);
	}
	printf("%s: not found\n", file);
}

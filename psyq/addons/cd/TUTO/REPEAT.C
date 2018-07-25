/* $PSLibId: Runtime Library Release 3.6$ */
/*			repeat: CD-DA/XA repeat
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Sep.12,1994	suzu
 *		1.10		Oct,24,1994	suzu
 *		2.00		Feb,02,1995	suzu
 *
 *			    Repeat Play
 *--------------------------------------------------------------------------
 * cdRepeat	Auto repeat play of CD-DA track
 *
 * SYNOPSIS	int cdRepeat(int startp, int endp)
 *
 * ARGUMENT	startp	start position
 *		endp	end position
 *
 * DESCRIPTION	backgound repeat play the audio sector between
 *		'startp' and 'endp' 
 *
 * RETURN	always 0
 *	
 * NOTE		ReportMode is used for faster position detection.
 *	
 *--------------------------------------------------------------------------
 * cdRepeatXA	Auto repeat play of CD-XA track
 *
 * SYNOPSIS	int cdRepeatXA(int startp, int endp)
 *
 * ARGUMENT	startp	start position
 *		endp	end position
 *
 * DESCRIPTIOS	backgound repeat play the audio sector between
 *		'startp' and 'endp' 
 *
 * RETURN	always 0
 *	
 * NOTE		Since VSyncCallback() is used for background position
 *		detection, be careful when you use VSyncCallback. 
 *		For double speed CD-XA only
 *		Call CdlSetfilter before start playing if you use
 *		multi-channel CD-XA track.
 *--------------------------------------------------------------------------
 * cdGetPos	get current position of the playing CD-ROM
 *
 * SYNOPSIS	int cdGetPos(void)
 *
 * ARGUMENT	none
 *
 * DESCRIPTION	get the current playing postion (sector number) 
 *
 * RETURNS	current playing sector postion
 *	
 *--------------------------------------------------------------------------
 * cdGetRepPos	get the total repeated times
 *
 * SYNOPSIS	int cdGetRepTime()
 *
 * ARGUMENT	none
 *
 * DESCRIPTION	get the total repeated times for timeout monitoring.
 *
 * RETURNS	total repeate times
 *--------------------------------------------------------------------------
 :
 *		     リピート再生ライブラリサンプル
 *	    CD-DA/XA トラックの任意の２点間をオートリピートする。
 *--------------------------------------------------------------------------
 * cdRepeat	CD-DA の自動リピート再生をする。
 *
 * 形式		int cdRepeat(int startp, int endp)
 *
 * 引数		startp	演奏開始位置
 *		endp	演奏終了位置
 *
 * 解説		startp と endp で指定された間の CD-DA データを繰り返し
 *		てバックグラウンドで再生する。 
 *
 * 返り値	つねに 0
 *	
 * 備考		内部の位置検出は、レポートモードを使用しているため高速
 *	
 *--------------------------------------------------------------------------
 * cdRepeatXA	CD-XA の自動リピート再生をする。
 *
 * 形式		int cdRepeatXA(int startp, int endp)
 *
 * 引数		startp	演奏開始位置
 *		endp	演奏終了位置
 *
 * 解説		startp と endp で指定された間の CD-XA データを繰り返し
 *		てバックグラウンドで再生する。 
 *
 * 返り値	つねに 0
 *	
 * 備考		内部の位置検出は、VSyncCallback() を使用して行なうので、
 *		するので、このソースコードをそのまま使用する場合は注意
 *		すること。
 *		再生は倍速度 XA のみ。
 *		マルチチャネルを使用する場合は、前もって CdlSetfilter 
 *		を使用してチャネルを指定しておくこと。
 *--------------------------------------------------------------------------
 * cdGetPos	現在再生中の位置を知る
 *
 * 形式		int cdGetPos(void)
 *
 * 引数		なし
 *
 * 解説		現在再生中の位置（セクタ番号）を調べる。
 *
 * 返り値	現在再生中のセクタ番号
 *	
 *--------------------------------------------------------------------------
 * cdGetRepPos	現在までの繰り返し回数を調べる
 *
 * 形式		int cdGetRepTime()
 *
 * 引数		なし
 *
 * 解説		現在までの繰り返し回数を調べる。タイムアウトによるエラー
 *		の検出に使用する。
 *
 * 返り値	現在までの繰り返し回数
 *--------------------------------------------------------------------------
 */
	
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

/* polling period   :ポーリング間隔 */
#define	XA_FREQ	32/*15*/			

/* macro indicating XA mode: XA モード */
#define	XA_MODE	(CdlModeSpeed|CdlModeRT) 

static int	StartPos, EndPos;	/* start/end point: 開始・終了位置 */
static int	CurPos;			/* current position: 現在位置 */
static int	RepTime;		/* current repeat times:繰り返し回数 */

/* callback of CdlDataReady: CdlDataReady 発生時のコールバック */
static void cbready(u_char intr, u_char *result);	

/* callback of VSync: VSync コールバック */
static void cbvsync(void);			

static cdplay(u_char com);

int cdRepeat(int startp, int endp)
{
	u_char	param[4];

	StartPos = startp;
	EndPos   = endp;
	CurPos   = StartPos;
	RepTime  = 0;

	param[0] = CdlModeRept|CdlModeDA;
	CdControlB(CdlSetmode, param, 0);
	VSync( 3 );

	CdReadyCallback(cbready);
	cdplay(CdlPlay);

	return(0);
}

int cdRepeatXA(int startp, int endp)
{
	u_char	param[4];

	StartPos = startp;
	EndPos   = endp;
	CurPos   = StartPos;
	RepTime  = 0;

	param[0] = CdlModeSpeed|CdlModeRT;
	CdControlB(CdlSetmode, param, 0);
	VSync( 3 );

	VSyncCallback(cbvsync);
	cdplay(CdlReadS);

	return(0);
}

int cdGetPos()
{
	return(CurPos);
}

int cdGetRepTime()
{
	return(RepTime);
}

/*
 * callback used in cdRepeat(): cdRepeat() で使用するコールバック
 */
static void cbready(u_char intr, u_char *result)
{
	CdlLOC	pos;
	if (intr == CdlDataReady) {
		if ((result[4]&0x80) == 0) {
			pos.minute = result[3];
			pos.second = result[4];
			pos.sector = 0;
			CurPos = CdPosToInt(&pos);
		}
		if (CurPos > EndPos || CurPos < StartPos) 
			cdplay(CdlPlay);
	}
	else {
		/*printf("cdRepeat: error:%s\n", CdIntstr(intr));*/
		while (cdplay(CdlPlay) != 0);
	}
}	


/*
 * callback used in cdRepeatXA(): cdRepeatXA() で使用するコールバック
 */
static void cbvsync(void)
{
	u_char		result[8];
	int		cnt, ret;
	
	if (VSync(-1)%XA_FREQ)	return;
	
	if ((ret = CdSync(1, result)) == CdlDiskError) {
		/*printf("cdRepeatXA: DiskError\n");*/
		cdplay(CdlReadS);
	}
	else if (ret == CdlComplete) {
		if (CurPos > EndPos )
			cdplay(CdlReadS);
		else {
			if (CdLastCom() == CdlGetlocP &&
			    (cnt = CdPosToInt((CdlLOC *)&result[5])) > 0)
				CurPos = cnt;
			CdControlF(CdlGetlocP, 0);
		}
	}
}


static cdplay(u_char com)
{
	CdlLOC	loc;
	
	CdIntToPos(StartPos, &loc);
	if (CdControl(com, (u_char *)&loc, 0) != 1)
		return(-1);
	
	CurPos = StartPos;
	RepTime++;
	return(0);
}


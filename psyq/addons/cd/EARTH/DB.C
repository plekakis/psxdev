/* $PSLibId: Runtime Library Release 3.6$ */
/*				db.c
 *			
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Mar,15,1994	suzu
 *		1.10		Jun,19,1995	suzu
 *
 *		      frame double buffer handler
 :		        ダブルバッファハンドラ
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Setup frame double buffer
 *	(0,0)-(xxx, 240), (0, 240)-(xxx, 480)	(xxx = 256/320/512/640)	
 *	(0,0)-(xxx, 480), (0,   0)-(xxx, 480)	(xxx = 256/320/512/640)	
 */	
static int	id = 0;		
static DRAWENV	draw[2];	
static DISPENV	disp[2];	

/* initiaize graphic environment for double buffer
 : ダブルバッファのための描画・表示環境を初期化する
 */
/* int	  w, h;		drawing/display area size: 描画・表示エリア */
/* int	  z;		projection: プロジェクション */
/* u_char r0, g0, b0;	background color: 背景色 */

void db_init(int w, int h, int z, u_char r0, u_char g0, u_char b0)
{
	/*
	 * If width == 480, not to do double buffering
	 : インターレースでない場合はダブルバッファする。
	 */	 
	int	oy = (h==480)? 0: 240;
	
	PadInit(0);			
	ResetGraph(0);			
	InitGeom();			

	/* set GTE origin at the center of screen: 原点を画面中央にする */
	SetGeomOffset(w/2, h/2);	

	/* set distance to the screen : 投影面までの距離 */
	SetGeomScreen(z);	
	
	/* inititalize double buffer
	 : フレームバッファ上でダブルバッファを構成するための描画環境・
	 * 表示環境構造体のメンバを設定する。
	 */	 
	SetDefDrawEnv(&draw[0], 0,  0, w, h);
	SetDefDispEnv(&disp[0], 0, oy, w, h);
	SetDefDrawEnv(&draw[1], 0, oy, w, h);
	SetDefDispEnv(&disp[1], 0,  0, w, h);

	/* if isbg == 1, PutDrawEnv clears drawing area automatically.
	 : isbg = 1 にすると PutDrawEnv() の時点で描画エリアがクリアされる
	 */
	draw[0].isbg = draw[1].isbg = 1;
	setRGB0(&draw[0], r0, g0, b0);
	setRGB0(&draw[1], r0, g0, b0);

	/* load font on (960,256)
	 :  フォント表示環境を設定 (VRAM アドレス=(960,256))
	 */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 0, 0, 2, 1024));
}

/* swap double buffer: ダブルバッファをスワップする */
void db_swap(u_long *ot)
{
	id = id? 0: 1;			/* swap id */
	if (draw[id].dfe) {
		DrawSync(0);		/* sync GPU */
		VSync(0);		/* wait for V-BLNK */
	}
	else {
		VSync(0);		/* wait for V-BLNK */
		ResetGraph(1);		/* reset GPU */
	}
	PutDrawEnv(&draw[id]);		/* update DRAWENV */
	PutDispEnv(&disp[id]);		/* update DISPENV */
	DrawOTag(ot);			/* draw OT */
	FntFlush(-1);			/* flush debug message */
}

/*
 * set world-screen matrix adjusting with aspect ratio of display mode
 * Notice: This routine is only for NTSC
 : ワールドスクリーンマトリクスを表示のアスペクト比に応じて補正する
 */
void db_set_matrix(MATRIX *m)
{
	/* adjust x */
	m->m[0][0] = m->m[0][0]*disp[0].disp.w/640;
	m->m[0][1] = m->m[0][1]*disp[0].disp.w/640;
	m->m[0][2] = m->m[0][2]*disp[0].disp.w/640;
		
	/* adjust y */
	m->m[1][0] = m->m[1][0]*disp[0].disp.h/480;
	m->m[1][1] = m->m[1][1]*disp[0].disp.h/480;
	m->m[1][2] = m->m[1][2]*disp[0].disp.h/480;

	/* set matrix to GTE */
	SetRotMatrix(m);
	SetTransMatrix(m);
}

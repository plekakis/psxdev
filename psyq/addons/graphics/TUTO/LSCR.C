/* $PSLibId: Runtime Library Release 3.6$ */
/*			lscr.c: line scroll sample
 *
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------------------	
 *	1.00		95/04/01	suzu
 *
 *			Pseudo line scroll
 *
 *	This is a pseudo line-scroll sample program. This driver does
 *	not use H-Sync interrupt, but uses recursive texture mapping
 *	technique. Sine the mapping source is read from previous line,
 *	the additional 3rd buffer for recursive mapping is not required.
 *
 *	To add this function to your application:
 *	 * Call LscrInit() to notify the double buffer position.
 *	 * Set scroll parameters to the external array 'LscrOffset[240]'
 *	   For example,
 *	
 *			LscrOffset[10] = 20;
 *	
 *	   means 10th line is scrolled 20 dot to right.
 *	
 *	* After calling DrawOTag(), call LscrFlush() to execute
 *	  scrolling.
 *
 *	To scroll only BG plane,
 *	
 *	* separete BG OTs and FG(Sprite) OTs, and call LscrFlush()
 *	  after BG drawing.
 *	
 * 		DrawOtag(bg_ot[id]);		// OT of BG
 * 		LscrFlush(id);
 * 		DrawOtag(fg_ot[id]);		// OT of FG
 *	
 *	  Each drawing request qued and executed in the background.
 *	  
 :			ラインスクロールサンプル
 * 
 *	ここでのラインスクロールは、水平帰線の割り込みを使用せずに
 *	リカーシブマッピング（再帰マッピング）を使用して実現してい
 *	ます。ただし再帰マップ用のサードバッファは必要ありません。
 *
 *	既存のプログラムにラインスクロール機能を追加するには、
 *
 *	・最初に LscrInit() を呼んでダブルバッファの位置を設定する。
 *	
 *	・外部変数 int LscrOffset[] にラインごとスクロール量を設定する。
 *	  例えば、LscrOffset[10] = 20; とすれば、10 ライン目が 20 ドット
 *	  右にスクロールすることを意味します。
 *
 *	・ DrawOTag()/GsDrawOt() のあとで、LscrFlush() を呼ぶ。このと
 *	   き LscrOffset[] の値に応じたラインスクロールを行ないます。
 *
 *	を行なって下さい。
 *
 *	また、BG(バックグラウンド） のみをラインスクロールする場合は、
 *
 *		DrawOtag(bg_ot[id]);		// BG の OT 
 *		LscrFlush(id);
 *		DrawOtag(fg_ot[id]);		// FG の OT 
 *
 *	と BG とFG (フォアグラウンド) の ＯＴを分割することで実現でき
 *	ます。
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define MAXW		512	/* maximum size of drawing width
				 : 描画領域の幅の最大値 */
#define MAXH		240

int	LscrOffset[MAXH];

typedef struct {		/* Complex primitive: 合成プリミティブ */
	DR_MODE	mode;
	SPRT	sprt;
} TSPRT;

struct {
	TILE	bg[MAXH];
	TSPRT	l[MAXH], r[MAXW];
} Lscr[2];

static setBgLine(TILE *bg, int y, int r, int g, int b);
static setTSprt(TSPRT *tsprt, int w, int h, int x, int y, int tx, int ty);

static int	Width, Height;

LscrFlush(int drawbuf)
{
	int	y;
	for (y = 0; y < Height; y++) {
		Lscr[drawbuf].l[y].sprt.x0 = LscrOffset[y];
		Lscr[drawbuf].r[y].sprt.x0 = LscrOffset[y]+256;
	}
	DrawOTag((u_long *)&Lscr[drawbuf].bg[0]);
	
}

LscrInit(int w, int h, int ox0, int oy0, int ox1, int oy1)
{
	int y, i;
	
	if (w > MAXW) {
		printf("LscrInit: size overflow (%d,%d)\n", w, h);
		w = MAXW;
	}
	if (h > MAXH) {
		printf("LscrInit: size overflow (%d,%d)\n", w, h);
		h = MAXH;
	}
	Width  = w;
	Height = h;	
	
	for (y = 0; y < Height; y++) {
		
		/* clear scroll offset: スクロールオフセット初期化 */
		LscrOffset[y] = 0;
		
		/* initialize background color to black(0,0,0)
		 *  背景色を黒(0,0,0) に初期化
		 */
		setBgLine(&Lscr[0].bg[y], y, 0, 0, 0);
		setBgLine(&Lscr[1].bg[y], y, 0, 0, 0);
		
		/* initialize scroll sprites
		 :  ラインスクロール用のスプライトを初期化
		 */
		setTSprt(&Lscr[0].l[y], 256,   1,   0, y, ox0,     oy0+y+1);
		setTSprt(&Lscr[0].r[y], w-256, 1, 256, y, ox0+256, oy0+y+1);

		setTSprt(&Lscr[1].l[y], 256,   1,   0, y, ox1,     oy1+y+1);
		setTSprt(&Lscr[1].r[y], w-256, 1, 256, y, ox1+256, oy1+y+1);

		/* make primitive links: リンクを張る 順番は 左上→右下 */
		for (i = 0; i < 2; i++) {
			CatPrim(&Lscr[i].bg[y], &Lscr[i].l[y]);
			CatPrim(&Lscr[i].l[y], &Lscr[i].r[y]);
			if (y > 0)
				CatPrim(&Lscr[i].r[y-1], &Lscr[i].bg[y]);
			if (y == Height-1)
				TermPrim(&Lscr[i].r[y]);
		}
	}
}

/*
 * initialize background line (black line)
 : 背景の直線を初期化
 */
static setBgLine(TILE *bg, int y, int r, int g, int b)
{
	SetTile(bg);
	setRGB0(bg, r, g, b);
	setXY0(bg, 0, y);
	setWH(bg, Width, 1);
}

/*
 * initialize sprite with tpage using MargePrim()
 : tpage つきスプライトプリミティブを初期化。合成には MargePrim() を使用
 */
static setTSprt(TSPRT *tsprt, int w, int h, int x, int y, int tx, int ty)
{
	
	/* Set primitive mode.
	 * dfe = 1, dtd = 1, tp = 2(16bit), abr=0, tw=0
	 * u, v is used lower 8bit of (tx, ty)
	 * tpage is calculated by higher 8bit of (tx,ty)
	 *
	 : モードを設定する					
	 * dfe = 1, dtd = 1, tp = 2(16bit), abr=0, tw=0
	 * u, v は、tx, ty の下位 8bit を使用
	 * テクスチャページは、tx, ty の 9bit 以上を使用
	 */
	SetDrawMode(&tsprt->mode, 1, 1,
		    GetTPage(2, 0, tx&~0xff, ty&~0xff), 0);
	
	/* initialize SPRT: スプライトを設定する */
	SetSprt(&tsprt->sprt);
	setRGB0(&tsprt->sprt, 0x80, 0x80, 0x80);
	setUV0(&tsprt->sprt, tx&0xff, ty&0xff);
	setXY0(&tsprt->sprt, x, y);
	setWH(&tsprt->sprt, w, h);

	/* connect SPRT and DR_MODE primitive: 合成する */
	if (MargePrim(&tsprt->mode, &tsprt->sprt) != 0) {
		printf("Marge failed\n");
		return(-1);
	}
}

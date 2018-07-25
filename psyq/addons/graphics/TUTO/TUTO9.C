/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto9: cell type BG
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul,29,1994	suzu 
 *		2.00		May,22,1995	sachiko
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 * 			All rights Reserved
 *
 * Here is a sample program for coventional cell type BG estimation using
 * 8bit/4bit textures.
 * 8bit textured primitive is slower than 4bit textured primite.
 * this sample can draw 4-full-BGs in 1/60 sec using 8bit POLY_FT4 
 *
 * When you want to use special effect like magnify, rotate, and so 
 * on at the BG, you have to use POLY_FT4 instead of SPRT even though
 * POLY_FT4  is 2 times slower than SPRT.
 *
 * POLY_FT4 primitives are neccesary for magnified or rotated BG cells, 
 * but it is slower than SPRT. therefore if you do not need these functions,
 * you should use SPRT instead of POLY_FT4. Please define BG_SPRT when you
 * want to use SPRT primitevs as each BG cells.
 *
 :			セルタイプの BG の実現 
 *
 *  	このプログラムは、4bit/8bit テクスチャを使用した BG の速度評価
 *	プログラムです。
 *	8bit テクスチャの場合は 4bit テクスチャに比べて速度が低下します。
 *	この例では POLY_FT4 のアレイを使用して、4 枚のフルサイズ BG を描
 *	画しています。
 *
 *	BG_SPRT を define するとスプライトを使用した BG を描画します。
 *
 *	BG を回転・拡大したい場合は SPRT を使用する代わりに POLY_FT4
 *	を使用する必要があります。POLY_FT4 は SPRT よりも描画速度が低下し
 *	ます。従ってこの例の様に等倍のマッピングのみを使用する場合は SPRT
 *	の方が高速です。
 *
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* BG セルの単位は 32x32 */
#define BG_CELLX	32
#define BG_CELLY	32

#define BG_WIDTH	288	/* max BG width: BG の最大幅 */
#define BG_HEIGHT	192	/* max BG height: BG の最大高さ */

/* number of BG cells: セルの個数（横方向）*/
#define BG_NX		(BG_WIDTH/BG_CELLX+2)
#define BG_NY		(BG_HEIGHT/BG_CELLY+2)

/* depth of OT: OT の分解能は 4 */
#define OTSIZE		4	

/* Define BG structure: BG 構造体を新しく定義する */
typedef struct {
	/* local DRAWENV for each BG: 各 BG ごとの描画環境 */
	DRAWENV		env;	
	
	/* local OT for each BG: 各 BG ごとの OT */
	u_long		ot[OTSIZE];	
	
	SPRT		cell[BG_NY*BG_NX];	/* BG cells (SPRT) */
	POLY_FT4	cell2[BG_NY*BG_NX];	/* BG cells (POLY_FT4) */
} BG;

/*
 * primitive double buffer: プリミティブダブルバッファ
 */
#define MAXBG	4			/* max 4 BGs: 最大 BG 面 */
typedef struct {
	DISPENV	disp;			/* display environment: 表示環境 */
	BG	bg[MAXBG];		/* BG */
} DB;

/* define this when you use SPRT BG
   : スプライト BGを使用する場合（ポリゴンのときは、コメントアウト） */
#define BG_SPRT

/* define this when you use 4bit texture
   : 4bit テクスチャを使用する場合（8bit のときはコメントアウト） */
#define BG_4bit

static void bg_init(BG *bg, int x, int y);
static void bg_update(BG *bg, SVECTOR *mapxy);
static void bg_draw(BG *bg);
static int pad_read(SVECTOR *mapxy);

main()
{
	/* primitive double buffer: パケットダブルバッファの実体 */
	DB	db[2];		
	
	/* current double buffer: 設定パケットバッファへポインタ */
	DB	*cdb;		
	
	/* current OT: 設定パケットＯＴ */
	u_long	*ot;		
	
	/* current location of BG: bgの現在のマップ位置（ｚは使用しない） */
	SVECTOR	mapxy[4];	
	
	int	i;
	PadInit(0);		/* reset PAD: コントローラをリセット */
	ResetGraph(0);		/* reset GPU: グラフィックシステムをリセット */

	/* clear frame buffer: フレームダブルバッファをクリア */
	{
		RECT rect;
		setRECT(&rect, 0, 0, 320, 480);
		ClearImage(&rect, 0, 0, 0);
	}

	/* define frame double buffer: ダブルバッファの定義 */
	/*	Buffer0   #0:	(  0   0) -(320,240)
	 *	Buffer1   #1:	(  0,240) -(320,480)
	 */
	SetDefDispEnv(&db[0].disp,  0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp,  0,   0, 320, 240);

	/* Set Initial value of primitive buffer
	 * link primitive list for BG here.
	 : プリミティブバッファの内容の初期値を設定
	 * ここで、BG 用のプリミティブリストのリンクまで張ってしまう
	 */

	bg_init(&db[0].bg[0], 0, 4);		/* BG #0 */
	bg_init(&db[1].bg[0], 0, 4+240);

	bg_init(&db[0].bg[1], 20, 12);		/* BG #1 */
	bg_init(&db[1].bg[1], 20, 12+240);

	bg_init(&db[0].bg[2], 40, 20);		/* BG #2 */
	bg_init(&db[1].bg[2], 40, 20+240);

	bg_init(&db[0].bg[3], 60, 28);		/* BG #3 */
	bg_init(&db[1].bg[3], 60, 28+240);

	/* マップの初期値を設定 */
	setVector(&mapxy[0],  0,  0,0);	/* default position (  0,  0) */
	setVector(&mapxy[1],256,256,0);	/* default position (256,256) */
	setVector(&mapxy[2],  0,256,0);	/* default position (  0,256) */
	setVector(&mapxy[3],256,  0,0);	/* default position (256,  0) */

	/* display enable: 表示開始 */
	SetDispMask(1);

	/* main loop: メインループ */
	while (pad_read(mapxy) == 0) {		/* mapxy を獲得 */

		/* swapt primitive buffer ID
		   : 設定プリミティブバッファポインタを獲得する。*/
		cdb = (cdb==db)? db+1: db;

		/* update BG: BG を更新 */
		for (i = 0; i < 4; i++)
			bg_update(&cdb->bg[i], &mapxy[i]);

		/* swap double buffer: ダブルバッファをスワップ */
		DrawSync(0);
		VSync(0);
		PutDispEnv(&cdb->disp); 

		/* draw 4 plane of BG: BG を４面描画する */
		for (i = 0; i < 4; i++)
			bg_draw(&cdb->bg[i]);
	}

	/* quit: 終了 */
        PadStop();		
	ResetGraph(1);
	StopCallback();
	return;
}

/***************************************************************************
 *
 *	BG management
 :	BG ハンドリング
 *
 ***************************************************************************/
/* BG cell structure
 * This sample does not use clut member because it uses only 1 clut.
 * This sample does not use attr member because no attributes are defined here.
 * See bgmap.h for detail.
 : BG セル構造体
 * この例では CLUT は１本なので clut メンバは使用しない。
 * この例では 属性判定をしないので attr メンバは使用しない。
 * bgmap.h を参照 
 */
typedef struct {
	u_char	u, v;	/* cell texture UV: セルテクスチャパターン座標 */
	u_short	clut;	/* cell texture CLUT: セルテクスチャパターン CLUT */
	u_long	attr;	/* attribute: アトリビュート */
} CTYPE;

/* 2D array for cell type: セルタイプを記述した２次元配列 */
#include "bgmap.h"		

#ifdef BG_4bit
extern	u_long	bgtex[];	/* BG texture・CLUT (4bit) */
#else
extern	u_long	bgtex8[];	/* BG texture・CLUT (8bit) */
#endif

/*
 * initialize BG.
 * All parameters which would not be changed is set here.
 : BG の初期化:あらかじめできることはみんなここで設定してしまう。
 */
/* BG	*bg,	BG data: BG データ */
/* int	x,y	location on screen: スクリーン上の表示位置（Ｘ） */
static void bg_init(BG *bg, int x, int y)
{
	SPRT		*cell;
	POLY_FT4	*cell2;
	u_short		clut, tpage;
	int		ix, iy;

	/* set drawing environment: 描画環境を設定 */
	SetDefDrawEnv(&bg->env, x, y, BG_WIDTH, BG_HEIGHT);

	/* load texture data and CLUT: テクスチャ・テクスチャ CLUT をロード */

#ifdef BG_4bit	/* 4bit mode */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 480);
#else		/* 8bit mode */
	tpage = LoadTPage(bgtex8+0x80, 1, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex8, 0, 480);
#endif
	/* set default texture page: デフォルトテクスチャページを設定 */
	bg->env.tpage = tpage;

	/* clear local OT: ローカルＯＴをクリア */
	ClearOTag(bg->ot, OTSIZE);

	/* make primitive list for BG
	   : BG を構成するプリミティブリストを作成する。*/

	for (cell = bg->cell, cell2 = bg->cell2, iy = 0; iy < BG_NY; iy++)
	    for (ix = 0; ix < BG_NX; ix++, cell++, cell2++) {

#ifdef BG_SPRT
		SetSprt(cell);			/* SPRT Primitive */
		SetShadeTex(cell, 1);		/* ShadeTex forbidden */
		setWH(cell, BG_CELLX, BG_CELLY);/* Set the sizes of the cell */
		cell->clut = clut;		/* Set texture CLUT  */
		AddPrim(&bg->ot[0], cell);	/* Put SPRT primitive into OT */
#else
		SetPolyFT4(cell2);		/* POLY_FT4 Primitive */
		SetShadeTex(cell2, 1);		/* ShadeTex forbidden */
		cell2->tpage = tpage;		/* Set texture pages*/
		cell2->clut  = clut;		/* Set texture CLUT*/
		AddPrim(&bg->ot[0], cell2);	/* Put POLY_FT4 primitives 
						   into OT  */
#endif
	    }
}

/* updatea BG members: BG のメンバを更新する */
/* BG		*bg,	BG data: BG データ */
/* SVECTOR	*mapxy	map location: テクスチャの BG へのマップ位置 */

static void bg_update(BG *bg, SVECTOR *mapxy)
{
	/* cell data for sprites: スプライト用セルデータ */
	SPRT		*cell;		
	
	/* cell data for polygon: ポリゴン用セルデータ */
	POLY_FT4	*cell2;		
	
	/* cell type: セルタイプ */
	CTYPE		*ctype;		
	
	/* absolute postion on map: マップのオフセット値 */
	int		mx, my;		
	
	/* relative position on map: マップの中でのオフセット値 */
	int		dx, dy;		
	
	int		tx, ty;		/* work */
	int		x, y, ix, iy;

	/* (tx, ty) is rap-rounded at BG_CELLX*BG_MAPX 
	   : (tx, ty) 現在位置（BG_CELLX*BG_MAPX でラップラウンド）*/
	tx = (mapxy->vx)&(BG_CELLX*BG_MAPX-1);
	ty = (mapxy->vy)&(BG_CELLY*BG_MAPY-1);

	/*: tx を BG_CELLX で割った値がマップの位置 (mx)
	 *  tx を BG_CELLX で割った余りが表示移動量 (dx)
	 */
	mx =  tx/BG_CELLX;	my =  ty/BG_CELLY;
	dx = -(tx%BG_CELLX);	dy = -(ty%BG_CELLY);

	/* update (u0,v0), (x0,y0) in the primitive list.
	   : プリミティブリストの (u0,v0), (x0,y0) メンバを変更 */

	cell  = bg->cell;
	cell2 = bg->cell2;
	for (iy = y = 0; iy < BG_NY; iy++, y += BG_CELLY) {
		for (ix = x = 0; ix < BG_NX; ix++, x += BG_CELLX) {

			/* rap-rounded at BG_MAPX, BG_MAPY 
			   :(BG_MAPX, BG_MAPY) でラップラウンド */
			tx = (mx+ix)&(BG_MAPX-1);
			ty = (my+iy)&(BG_MAPY-1);

			/* get cell-type from the map data.
			 * Id code is stored  as a ASCII code in Map[][] 
			 : マップからセルタイプを獲得 
			 * Map[][] はキャラクタコードで ID が入っている 
			 */ 
			ctype = &CType[(Map[ty])[tx]-'0'];
#ifdef BG_SPRT
			/* to reduce the memory access, update only
			 * (u0,v0), (x0, y0) menbers
			 : (u0,v0), (x0, y0) のみを更新
			 */
			setUV0(cell, ctype->u, ctype->v);
			setXY0(cell, x+dx, y+dy);
#else
			/* to reduce the memory access, update only
			 *  (u0,v0), (x0, y0) menbers (POLY_FT4)
			 : (u,v), (x, y) を更新 (POLY_FT4)
			 */
			setUVWH(cell2, ctype->u, ctype->v,
				BG_CELLX-1, BG_CELLY-1);
			setXYWH(cell2, x+dx, y+dy, BG_CELLX, BG_CELLY);
#endif
			cell++;
			cell2++;
		}
	}
}

/*
 * draw BG: BG を描画
 */
static void bg_draw(BG *bg)
{
	/* Update Drawing environment for each BG
	   : BG ごとに設定した描画環境に更新 */
	PutDrawEnv(&bg->env);	
	
	DrawOTag(bg->ot);	/* draw: 描画 */
}

/*
 * read controller: コントローラのデータを読んで、 BG の移動方向を決める。
 */
/* SVECTOR *mapxy	map location: テクスチャの BG へのマップ位置 */
static int pad_read(SVECTOR	*mapxy)
{
	static SVECTOR	v[4] = {
		 2, 0, 0, 0,	0,  2, 0, 0,
		-2, 0, 0, 0,	0, -2, 0, 0,
	};

	int	i,id;
	u_long	padd = PadRead(1);

	/* quit: 終了 */
	if(padd & PADselect) 	return(-1);

	id = 2;
	if (padd & (PADR1|PADL1|PADR2|PADL2)) id = 0;

	/* BG #0 （#2 if PADR1 or PADL1 is pushed)
	   : BG #0 （ PADR1 または PADL1 も押されていた場合は BG #2 ）*/
	if(padd & PADLup)	setVector(&v[id],  0, -2, 0);
	if(padd & PADLdown)	setVector(&v[id],  0,  2, 0);
	if(padd & PADLleft)	setVector(&v[id], -2,  0, 0);
	if(padd & PADLright)	setVector(&v[id],  2,  0, 0);

	/* BG #1 （#3 if PADR1 or PADL1 is pushed)
	   : BG #1 （ PADR1 または PADL1 も押されていた場合は BG #3 ）*/
	if(padd & PADRup)	setVector(&v[id+1],  0, -2, 0);
	if(padd & PADRdown)	setVector(&v[id+1],  0,  2, 0);
	if(padd & PADRleft)	setVector(&v[id+1], -2,  0, 0);
	if(padd & PADRright)	setVector(&v[id+1],  2,  0, 0);

	/* Add the motion range 
	   : 今回の移動量を加算する */
	for (i = 0; i < 4; i++)
		addVector(&mapxy[i], &v[i]);

	return(0);
}



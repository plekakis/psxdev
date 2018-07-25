/* $PSLibId: Runtime Library Release 3.6$ */
/*		   tuto10: 3 dimentional cell type BG
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul,29,1994	suzu 
 *		2.00		May,22,1995	sachiko
 *
 *			3D cell-typed-BG
 :		      ３Ｄセルタイプの BG の実現
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* unit of BG cell is 32x32: BG セルの単位は 32x32 */
#define BG_CELLX	32			
#define BG_CELLY	32

/* max width and heght of BG: BG の最大幅 */
#define BG_WIDTH	1024			
#define BG_HEIGHT	512			
	
/* number of cell: セルの個数 */
#define BG_NX		(BG_WIDTH/BG_CELLX)	
#define BG_NY		(BG_HEIGHT/BG_CELLY)	

/* The depth of OT is4 : ＯＴの分解能は 4 */
#define OTSIZE		4			

/* screen size is 640x240: スクリーンサイズ(640x240) */
#define SCR_W		640	
#define SCR_H		240

/* depth of screen: スクリーンの深さ */
#define SCR_Z		512			

/*
 * Define structure to deal BG
 : BG 構造体を新しく定義する
 */
typedef struct {
	SVECTOR		*ang;		/* rotation: 回転角 */
	VECTOR		*vec;		/* translation: 移動量 */
	SVECTOR		*ofs;		/* offset on map: マップ上オフセット */
	POLY_GT4	cell[BG_NY*BG_NX];	/* BG cells: BG セル配列 */
} BG;

/*
 * Define structure to deal BG
 : パケットダブルバッファ
 */
typedef struct {
	DRAWENV		draw;		/* drawing environment: 描画環境 */
	DISPENV		disp;		/* display environment:表示環境 */
	u_long		ot[OTSIZE];	/* OT: ＯＴ */
	BG		bg0;		/* BG 0 */
} DB;

static void init_prim(DB *db,
		      int dr_x, int dr_y, int ds_x, int ds_y, int w, int h);
static void bg_init(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs);
static void bg_update(u_long *ot, BG *bg);
static int pad_read(BG *bg);

main()
{
	/* double buffer: ダブルバッファ */
	DB		db[2];		
	
	/* current buffer: 現在のバッファアドレス */
	DB		*cdb;		

	/* initialize GTE: GTE の初期化 */
	init_system(SCR_W/2, SCR_H/2, SCR_Z);
	
	/* Set initial value of packet buffers.
	 * and make links of primitive list for BG.
	 : パケットバッファの内容の初期値を設定 			
	 * ここで、BG 用のプリミティブリストのリンクまで張ってしまう
	 */
	init_prim(&db[0], 0,     0, 0, SCR_H, SCR_W, SCR_H);
	init_prim(&db[1], 0, SCR_H, 0,     0, SCR_W, SCR_H);

	/* enable to screen: 表示開始 */
	SetDispMask(1);

	/* main loop: メインループ */
	cdb = db;
	while (pad_read(&cdb->bg0) == 0) {

		/* exchange primitive buffer: パケットバッファの交換 */
		cdb = (cdb==db)? db+1: db;	
		
		/* clear OT:  OT のクリア */
		ClearOTag(cdb->ot, OTSIZE);	

		bg_update(cdb->ot, &cdb->bg0);

		DrawSync(0);	/* wait for end of drawing: 描画の終了を待つ */
		VSync(0);	/* wait for V-BLNK: 垂直同期の発生を待つ */
		
		/* swap double buffer draw: ダブルバッファ交換 */
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);
		DrawOTag(cdb->ot);	
	}
	/* quit: 終了 */
        PadStop();
	ResetGraph(1);
	StopCallback();
	return;
}

/*
 * initialize primitive double buffers 
 * Parameters which would not be changed any more must be set here.
 :
 * パケットダブルバッファの各メンバの初期化
 * メインループ内で変更されないものはすべてここであらかじめ設定しておく。
 */
/* DB	*db,		primitive buffer: パケットバッファ  */
/* int	dr_x, dr_y	drawing area location: 描画環境の左上 ＸY */
/* int	ds_x, ds_y	display area location: 表示環境の左上 ＸY */
/* int	w,h		drawing/display  area: 描画・表示領域の幅と高さ */
static void init_prim(DB *db,
		      int dr_x, int dr_y, int ds_x, int ds_y, int w, int h)
{
	/* Buffer BG location 
	 * GTE treat angles like follows: 360°= 4096 (ONE) 
	 : BG の位置バッファ 
	 * GTE では、角度を 360°= 4096 (ONE) とします。*/
	static SVECTOR	ang = {-ONE/5, 0,       0};
	static VECTOR	vec = {0,      SCR_H/2, SCR_Z/2};
	static SVECTOR	ofs = {0,      0,       0};

	/* set double buffer: ダブルバッファの設定 */
	SetDefDrawEnv(&db->draw, dr_x, dr_y, w, h);
	SetDefDispEnv(&db->disp, ds_x, ds_y, w, h);

	/* set auto clear mode for background
	   : バックグラウンド自動クリアの設定 */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 0, 0, 0);

	/* initialize for BG: BG の初期化 */
	bg_init(&db->bg0, &ang, &vec, &ofs);
}

/***************************************************************************
 * 
 *			BG management
 :			BG ハンドリング
 *
 ***************************************************************************/
/*
 * BG cell structure: BG セル構造体
 */
typedef struct {
	u_char	u, v;	/* cell texture UV: セルテクスチャパターン座標 */
	u_short	clut;	/* cell texture CLUT: セルテクスチャパターン CLUT */
	u_long	attr;	/* attribute: アトリビュート */
} CTYPE;

/*
 * BG mesh structure: BG Mesh 構造体（内部のワーク用）
 */
/* 2D array for cell type: セルタイプを記述した２次元配列 */
#include "bgmap.h"	
/* BG texrure pattern/CLUT: BG テクスチャ画像パターン・テクスチャ CLUT */
extern	u_long	bgtex[];

/* initialize BG */
/* BG		*bg,	BG data: BG データ */
/* int		x,y	location on screen: スクリーン上の表示位置（Ｘ） */
/* VECTOR	*vec	translation vector: 平行移動ベクトル */
/* SVECTOR	*ofs	map offset: オフセット */

static void bg_init(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs)
{
	POLY_GT4	*cell;
	u_short		tpage, clut;
	int		i, x, y, ix, iy;
	u_char		col;

	/* set location data: 位置バッファを設定 */
	bg->ang = ang;
	bg->vec = vec;
	bg->ofs = ofs;

	/* load texture and CLUT: テクスチャ・テクスチャ CLUT をロード */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 481);

	/* getnerate primitive list */
	for (cell = bg->cell, iy = 0; iy < BG_NY; iy++) {
		for (ix = 0; ix < BG_NX; ix++, cell++) {

			/* define POLY_GT4: POLY_GT4 プリミティブ */
			SetPolyGT4(cell);	

			/* change luminace according to Z value
			   : 奥を暗く、手前を明るくするために色を設定する */
			/* far side: 奥側 */
			col = 224*iy/BG_NY+16;		
			setRGB0(cell, col, col, col);
			setRGB1(cell, col, col, col);

			/* near side: 手前側 */
			col = 224*(iy+1)/BG_NY+16;	
			setRGB2(cell, col, col, col);
			setRGB3(cell, col, col, col);

			/* set tpage: テクスチャ tpage を設定 */
			cell->tpage = tpage;	
			
			/* set CLUT: テクスチャ CLUT を設定 */
			cell->clut  = clut;	
		}
	}
	
}

/*
 * Update BG members
 : BG のメンバを更新する
 */	
/* u_long	*ot,	OT: オーダリングテーブル */
/* BG		*bg	BG buffer: BG バッファ */
static void bg_update(u_long *ot, BG *bg)
{
	static SVECTOR	Mesh[BG_NY+1][BG_NX+1];

	MATRIX		m;
	POLY_GT4	*cell;
	CTYPE		*ctype;		/* cell type */
	SVECTOR		mp;
	
	/* current absolute position: 現在位置（ワールド座標） */
	int		tx, ty;		
	
	/* current left upper corner of map: 現在位置（マップの区画） */
	int		mx, my;		
	
	/* current relative position: 現在位置（マップの区画内） */
	int		dx, dy;		
	
	int		ix, iy;		/* work */
	int		xx, yy;		/* work */
	long		dmy, flg;	/* work */

	/* current postion of left upper corner of BG
	   : 現在位置（ BG の左上） */
	
	/* Lap-round at  BG_CELLX*BG_MAPX , BG_CELLY*BG_MAPY   
	   : ( BG_CELLX*BG_MAPX , BG_CELLY*BG_MAPY ) でラップラウンド */
	tx = (bg->ofs->vx)&(BG_CELLX*BG_MAPX-1);
	ty = (bg->ofs->vy)&(BG_CELLY*BG_MAPY-1);

	/*: tx を BG_CELLX で割った値がマップの位置 (mx)*/
	/*: tx を BG_CELLX で割った余りが表示移動量 (dx)*/
	mx =  tx/BG_CELLX;	my =  ty/BG_CELLY;
	dx = -(tx%BG_CELLX);	dy = -(ty%BG_CELLY);

	PushMatrix();

	/* calculate matrix: マトリクスの計算 */
	RotMatrix(bg->ang, &m);		/*: 回転角度 */
	TransMatrix(&m, bg->vec);	/*: 平行移動ベクトル */
	
	/* set matrix: マトリクスの設定 */
	SetRotMatrix(&m);		/*: 回転角度 */
	SetTransMatrix(&m);		/*: 平行移動ベクトル */

	mp.vy = -BG_HEIGHT + dy;
	mp.vz = 0;

	/* generate mesh: メッシュを生成 */
	for (iy = 0; iy < BG_NY+1; iy++, mp.vy += BG_CELLY) {
		mp.vx = -BG_WIDTH/2 + dx; 
		for (ix = 0; ix < BG_NX+1; ix++, mp.vx += BG_CELLX) 
			RotTransPers(&mp, (long *)&Mesh[iy][ix], &dmy, &flg);
	}

	/* Update (u0,v0), (x0,y0) members 
	   : プリミティブリストの (u0,v0), (x0,y0) メンバを変更 */
	for (cell = bg->cell, iy = 0; iy < BG_NY; iy++) {
		for (ix = 0; ix < BG_NX; ix++, cell++) {

			/* check if mesh is in display area or not 
			   : 表示領域内かどうかのチェック */
			if (Mesh[iy  ][ix+1].vx <     0) continue;
			if (Mesh[iy  ][ix  ].vx > SCR_W) continue;
			if (Mesh[iy+1][ix  ].vy <     0) continue;
			if (Mesh[iy  ][ix  ].vy > SCR_H) continue;

			/* (BG_MAPX, BG_MAPY) でラップラウンド */
			xx = (mx+ix)&(BG_MAPX-1);
			yy = (my+iy)&(BG_MAPY-1);

			/* get cell type from map database
			   : マップからセルタイプを獲得 */

			/* notice that Map[][] has a ASCII type ID code.
			   : Map[][] はキャラクタコードで ID が入っている */
			ctype = &CType[(Map[yy])[xx]-'0'];

			/* updatea (u,v),(x,y): (u,v), (x, y) を更新 */
			setUVWH(cell, ctype->u, ctype->v,
				BG_CELLX-1, BG_CELLY-1);

			setXY4(cell,
			       Mesh[iy  ][ix  ].vx, Mesh[iy  ][ix  ].vy,
			       Mesh[iy  ][ix+1].vx, Mesh[iy  ][ix+1].vy,
			       Mesh[iy+1][ix  ].vx, Mesh[iy+1][ix  ].vy,
			       Mesh[iy+1][ix+1].vx, Mesh[iy+1][ix+1].vy);

			/* add to OT: ＯＴに登録 */
			AddPrim(ot, cell);
		}
	}
	/* pop matrix: マトリクスの復帰 */
	PopMatrix();
}

/*
 * read controller: コントロールパッドのデータを読む。
 */
#define DT	8	/* speed */
static int pad_read(BG *bg)
{
	u_long	padd = PadRead(1);

	/* quit: プログラムの終了 */
	if(padd & PADselect) 	return(-1);

	bg->ofs->vy -= 4;

	/* translate: 平行移動 */
	if(padd & PADLup)	bg->ofs->vy -= 2;
	if(padd & PADLdown)	bg->ofs->vy += 2;
	if(padd & PADLleft)	bg->ofs->vx -= 2;
	if(padd & PADLright)	bg->ofs->vx += 2;

	/* rotate: 回転 */
	if (padd & PADRup)	bg->ang->vx += DT;
	if (padd & PADRdown)	bg->ang->vx -= DT;
	if (padd & PADRleft) 	bg->ang->vy += DT;
	if (padd & PADRright)	bg->ang->vy -= DT;

	return(0);
}


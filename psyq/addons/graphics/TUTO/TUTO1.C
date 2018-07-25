/* $PSLibId: Runtime Library Release 3.6$ */
/*				tuto1: OT
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *			  simple sample to use OT
 :			ＯＴを使用した描画のテスト
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Primitive double buffer.
 * Primitive buffer should be 2 buffers for CPU and GPU.
 : プリミティブダブルバッファ
 * 描画と CPU を並列で実行させるためには２組のバッファが必要
 */	
typedef struct {		
	DRAWENV		draw;		/* Drawing Environment : 描画環境 */
	DISPENV		disp;		/* Display Environment	: 表示環境 */
	u_long		ot[9];		/* Ordering Table : ＯＴ */
	SPRT_16		ball[8][8][8];	/* sprite buffer : スプライト */
					/* Z-direction	: 奥行き方向: ８個 */
					/* X-direction	: 横方向:     ８個 */
					/* Y-direction	: 縦方向:     ８個 */
} DB;

static void init_prim(DB *db);
static int pad_read(int *dx, int *dy);

main()
{
	DB	db[2];		/* primitive double buffer
				   : プリミティブダブルバッファ */
	DB	*cdb;		/* current primitive buffer pointer
				   : 設定パケットバッファへポインタ */
	SPRT_16	*bp;		/* work */
	int	ox, oy;		/* work */
	int	dx = 0, dy = 0;	/* work */
	int	depth;		/* work */
	int	x, y;		/* work */
	int	ix, iy;		/* work */

	/* reset controller: コントローラのリセット */
	PadInit(0);		
	
	/* reset graphic subsystem: 描画・表示環境のリセット */
	ResetGraph(0);		
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));
	
	/* define frame double buffer :  ダブルバッファの定義 */
	/*	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,240)-(320,480) (320x240)
	 */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* set primitive members of each buffer
	   : それぞれのプリミテブバッファの内容の初期値を設定 */
	init_prim(&db[0]);	/* パケットバッファ # 0 */
	init_prim(&db[1]);	/* パケットバッファ # 1 */

	/* display enable : 表示開始 */
	SetDispMask(1);

	/* main loop : メインループ */
	while (pad_read(&dx, &dy) == 0) {

		/* swap db[0], db[1]: ダブルバッファポインタの切り替え */
		cdb = (cdb==db)? db+1: db;

		/* clear ordering table (8 entry)
		   : ＯＴを初期化 （エントリ数＝８）*/
		ClearOTag(cdb->ot,8);

		/* calculate postion of sprites : スプライトの位置を計算 */
		for (depth = 0; depth < 8; depth++) {
			/* Far sprites move slowly, and near sprites
			 * move fast.
			 : 奥のスプライトほど遅く、
			 * 近くのスプライトほど速く動くように設定
			 */
			/* left upper corner (Y): 左上座標値 (Y) */
			oy =  56 + dy*(depth+1);	
			
			/* left upeer corner (X): 左上座標値 (X) */
			ox =  96 + dx*(depth+1);	
 			for (iy = 0, y = oy; iy < 8; iy++, y += 16) 
			for (ix = 0, x = ox; ix < 8; ix++, x += 16) {

				bp = &cdb->ball[depth][iy][ix];

				/* set upper-left corner of sprites
				   : スプライトの右上点を指定 */
				setXY0(bp, x, y);

				/* add primitives to OT
				   ot[depth+1] に登録 */
				AddPrim(&cdb->ot[depth], bp);
			}
		}
		/* wait for previous 'DrawOTag' 
		   : バックグラウンドで走っている描画の終了を待つ */
		DrawSync(0);

		/* wait for next V-BLNK
		   : V-BLNK が来るのを待つ */
		VSync(0);

		/* swap frame double buffer:
		 *  set the drawing environment and display environment.
		 : フレームダブルバッファを交換する
		 * カレントパケットバッファの描画環境・表示環境を設定する。
		 */
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);

		/* start Drawing
		   : ＯＴ上のプリミティブをバックグラウンドで描画する。*/
		DrawOTag(cdb->ot);
		FntPrint("tuto1: OT\n");
		FntFlush(-1);
	}
        PadStop();
	ResetGraph(1);
	StopCallback();
	return;
}

/* Intitalize the members of primitives in each primitive-buffer.
 * All of unchanged parameters are set here.
 : プリミティブダブルバッファの各メンバの初期化
 * メインループ内で変更されないものはすべてここであらかじめ設定しておく。
 */	
/* DB	*db;	プリミティブバッファ */
static void init_prim(DB *db)
{
	/* 16x16 ball texture pattern: 16x16 テクスチャパターン（ボール）*/
	extern u_long	ball16x16[];	
	
	/* ball CLUT (16 colorsx32):  ボール CLUT (16色x32) */
	extern u_long	ballclut[][8];	

	SPRT_16	*bp;
	u_short	clut;
	int	depth, x, y;

	/* Translate 4bit-texture pattern to (640, 0) on the frame-buffer
	 * and set the default texture page.
	 : 4bit モードのボールのテクスチャパターンをフレームバッファに
	 * 転送する。その時のテクスチャページをデフォルトテクスチャページに
	 * 設定する。	 
	 */	 
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);

	/* background clear enable : 自動背景クリアモードにする */
	/* 自動背景クリアモードにする */
	db->draw.isbg = 1;
	/* : 背景色の設定 */
	setRGB0(&db->draw, 60, 120, 120);	

	/* Initialize 8x8x8=256 sprites at this point.
	 * CLUT of each texture is changed accroding to the sprites' depth.
	 : 8x8x8=256 個のスプライトをまとめて初期化する。
	 * CLUT は、奥行きに応じて切替える。	 
	 */	 
	for (depth = 0; depth < 8; depth++) {
		/* load CLUT on frame buffer (notice the CLUT address)
		   : CLUT をロード CLUT ロードアドレスに注意 */
		clut = LoadClut(ballclut[depth], 0, 480+depth);

		/* Unchanged members of primitives are set here 
		   : プリミティブのメンバで変化しないものをここで設定する。*/
		for (y = 0; y < 8; y++) 
			for (x = 0; x < 8; x++) {
				bp = &db->ball[depth][y][x];
			
				/* SPRT_16 primitve: 16x16のスプライト */
				SetSprt16(bp);		
			
				/* ShadeTex disable: シェーディング禁止 */
				SetShadeTex(bp, 1);	
			
				/* (u0,v0) = (0, 0) */
				setUV0(bp, 0, 0);	
			
				/* set texture CLUT ID: テクスチャCLUT ID  */
				bp->clut = clut;	
			}
	}
}

/*: コントローラの解析
 */
/* int	*dx;	スプライトの座標値のキーワード（Ｘ） */
/* int	*dy;	スプライトの座標値のキーワード（Ｙ） */

static int pad_read(int *dx, int *dy)
{
	/* get controller value: コントローラからデータを読み込む */
	u_long	padd = PadRead(1);	

	/* exit program: プログラムの終了 */
	if (padd & PADselect)	return(-1);

	/* move sprite position: スプライトの移動 */
	if (padd & PADLup)	(*dy)--;
	if (padd & PADLdown)	(*dy)++;
	if (padd & PADLleft)	(*dx)--;
	if (padd & PADLright)	(*dx)++;

	return(0);
}


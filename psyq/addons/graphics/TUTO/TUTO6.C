/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto6: cube with depth-queue
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *		    rotating cubes with depth queue
 :		デプスキューイング効果を用いた立方体の描画
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define SCR_Z	(512)		
#define	OTSIZE	(4096)

/* start point of fog effect: フォグ（デプスキュー）のかかるスタート位置 */
#define FOGNEAR (300)		
					   
/* primitive buffer: プリミティブ関連のバッファ */
typedef struct {
	DRAWENV		draw;		/* drawing environment: 描画環境 */
	DISPENV		disp;		/* display environment: 表示環境 */
	u_long		ot[OTSIZE];	/* OT: オーダリングテーブル */
	POLY_F4		s[6];		/* sides of cube: 立方体の側面 */
} DB;

static int pad_read(MATRIX *rottrans);
static void init_prim(DB *db);
main()
{
	DB	db[2];		/* double buffer */
	DB	*cdb	;	/* current buffer */
	MATRIX	rottrans;	/* rot-trans matrix */
	CVECTOR	col[6];		/* color of cube surface */
	
	int	i;		/* work */
	int	dmy, flg;	/* dummy */

	/* initialize environment for double buffer (interlace)
	 :ダブルバッファ用の環境設定（インターレースモード）
	 */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* FarColor have to be the same as background:
	   ファーカラー（遠方色）の設定背景色と同色にする */
	SetFarColor(60,120,120);	
	
	/* start point of depth quweue: デプスキューイングを開始する地点 */
	SetFogNear(FOGNEAR,SCR_Z);	

	/* set primitive parametes on buffer: プリミティブバッファの初期設定 */
	init_prim(&db[0]);	/* buffer #0 */
	init_prim(&db[1]);	/* buffer #1 */

	/* set surface colors: 立方体の側面の色の設定 */
	for (i = 0; i < 6; i++) {
		col[i].cd = db[0].s[0].code;	/* code */
		col[i].r = rand();		/* R */
		col[i].g = rand();		/* G */
		col[i].b = rand();		/* B */
	}

	SetDispMask(1);		/* start displaying: 表示開始 */

	PutDrawEnv(&db[0].draw);
	PutDispEnv(&db[0].disp);

	while (pad_read(&rottrans) == 0) {

		cdb = (cdb==db)? db+1: db;	
		ClearOTagR(cdb->ot, OTSIZE);	
		
		/* apend cubes into OT: 立方体をＯＴに登録する */
		add_cubeF4F(cdb->ot, cdb->s, &rottrans, col);

		VSync(0);
		ResetGraph(1);
		
		/* clear background: 背景のクリア */
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		/* draw: 描画 */
		DrawOTag(cdb->ot+OTSIZE-1);
		FntFlush(-1);
	}
        PadStop();		
	ResetGraph(1);
	StopCallback();
	return;
}

/* 
 * Analyzing PAD and Calcurating Matrix
 : コントローラの解析と、変換マトリックスの計算
 */
/* MATRIX *rottrans; 	rot-trans matrix: 立方体の回転・平行移動マトリックス */
static int pad_read(MATRIX *rottrans)
{
	/* angle of rotation for the cube: angle 立方体の回転角度 */
	static SVECTOR	ang  = { 0, 0, 0};	
	
	/* translation vector: 平行移動ベクトル */
	static VECTOR	vec  = {0, 0, SCR_Z};	

	/* read from controller: コントローラからデータを読み込む */
	u_long	padd = PadRead(1);	

	int	ret = 0;
	
	/* quit program: プログラムの終了 */
	if (padd & PADselect) 	ret = -1;

	/* change the rotation angles for the cube and the light source
	   : 光源と立方体の回転角度の変更 */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* distance from screen : 原点からの距離 */
	if (padd & PADR1)	vec.vz += 8;
	if (padd & PADR2) 	vec.vz -= 8;

	/* matrix calcuration: マトリックスの計算 */
	/* rotation angle of the cube: 立方体の回転角度 */
	RotMatrix(&ang, rottrans);	
	
	/* translation vector of the cube: 立方体の平行移動ベクトル */
	TransMatrix(rottrans, &vec);	
	
	FntPrint("tuto6: fog angle=(%d,%d,%d)\n",
		 ang.vx, ang.vy, ang.vz);
	return(ret);
}

/*
 *	Initialization assosiate with Primitives.
 :	プリミティブ関連の初期設定
 */
/* DB	*db;	primitive buffer: プリミティブバッファ */
static void init_prim(DB *db)
{
	int i;

	for(i = 0;i < 6; i++)
		SetPolyF4(&db->s[i]);
}

/* $PSLibId: Runtime Library Release 3.6$ */

/*			tuto4: cube with lighting
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *		Draw 3D objects (cube) with lighting
 *			光源のある立方体の描画
 :
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define SCR_Z	(512)		
#define	OTSIZE	(4096)

/* primitive buffer: プリミティブ関連のバッファ */
typedef struct {
	DRAWENV		draw;		/* drawing environment: 描画環境 */
	DISPENV		disp;		/* display environment: 表示環境 */
	u_long		ot[OTSIZE];	/* OT: オーダリングテーブル */
	POLY_F4		s[6];		/* sides of cube: 立方体の側面 */
} DB;

/* light source（Local Color Matrix）:
   光源色（ローカルカラーマトリックス） */
static MATRIX	cmat = {
/* light source    #0, #1, #2, */
		ONE*3/4,  0,  0, /* R */
		ONE*3/4,  0,  0, /* G */
		ONE*3/4,  0,  0, /* B */
};

/* light vector (local light matrix)
   : 光源ベクトル（ローカルライトマトリックス） */
static MATRIX lgtmat = {
	/*          X     Y     Z */
	          ONE,  ONE, ONE,	/* 光源 #0 */
		    0,    0,    0,	/*      #1 */
		    0,    0,    0	/*      #2 */
};

static int pad_read(MATRIX *rottrans, MATRIX *rotlgt);
static void init_prim(DB *db);

main()
{
	DB	db[2];		/* double buffer */
	DB	*cdb	;	/* current buffer */
	MATRIX	rottrans;	/* rot-trans matrix */
	
	/* rot-trans matrix for light source: 光源の回転マトリックス */
	MATRIX		rotlgt;		
	
	/* lighint matrix: 最終的な光源マトリックス */
	MATRIX		light;

	/* color of cube surface: 立方体の側面の色 */
	CVECTOR		col[6];	
	
	int		i;		/* work */
	int		dmy, flg;	/* dummy */

	/* initialize environment for double buffer (interlace)
	 :ダブルバッファ用の環境設定（インターレースモード）
	 */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* set background color: バックカラー(アンビエント色)の設定 */
	SetBackColor(64, 64, 64);	
	
	/* set local color matrix: ローカルカラーマトリックスの設定 */
	SetColorMatrix(&cmat);		

	/* set primitive parametes on buffer: プリミティブバッファの初期設定 */
	init_prim(&db[0]);	
	init_prim(&db[1]);	

	/* set surface colors. 
	 * NormalColorCol() overwrite the 'code' field of the primitive.
	 * So set the code here.
	 : 立方体の側面の色の設定
	 * 後の NormalColorCol() で db[0].s[0].code の内容が壊されて
	 * しまうため、ここで待避しておく
	 */
	for (i = 0; i < 6; i++) {
		col[i].cd = db[0].s[0].code;	/* CODE */
		col[i].r  = rand();		/* R */
		col[i].g  = rand();		/* G */
		col[i].b  = rand();		/* B */
	}

	SetDispMask(1);			/* start displaying: 表示開始 */
	PutDrawEnv(&db[0].draw);	/* set DRAWENV: 描画環境の設定 */
	PutDispEnv(&db[0].disp);	/* set DISPENV: 表示環境の設定 */

	while (pad_read(&rottrans, &rotlgt) == 0) {

		cdb = (cdb==db)? db+1: db;	/* change current buffer */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear OT */

		/* Calcurate Matrix for the light source 
		   : 光源マトリックスの設定 */
		MulMatrix0(&lgtmat, &rotlgt, &light);
		SetLightMatrix(&light);

		/* apend cubes into OT: 立方体をＯＴに登録する */
		add_cubeF4L(cdb->ot, cdb->s, &rottrans, col);

		VSync(0);
		ResetGraph(1);
		
		/* clear background: 背景のクリア */
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		/* draw primitives: 描画 */
		/*DumpOTag(cdb->ot+OTSIZE-1);	/* for debug */
		DrawOTag(cdb->ot+OTSIZE-1);	
		FntFlush(-1);
	}
        /* close controller: コントローラのクローズ */
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
/* MATRIX *rotlgt;	light source matrix: 光源マトリックス */
static int pad_read(MATRIX *rottrans, MATRIX *rotlgt)
{
	/* angle of rotation for the cube: angle 立方体の回転角度 */
	static SVECTOR	ang  = { 0, 0, 0};	
	
	/* 光源の回転角度: angle of rotation for the light source */
	static SVECTOR	lgtang = {1024, -512, 1024};	
	
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
	
	/* change the rotation angles only for the light source 
	   : 光源のみの回転角度の変更 */
	if (padd & PADLup)	lgtang.vx += 32;
	if (padd & PADLdown)	lgtang.vx -= 32;
	if (padd & PADLleft) 	lgtang.vy += 32;
	if (padd & PADLright)	lgtang.vy -= 32;

	/* distance from screen : 原点からの距離 */
	if (padd & PADL1)	vec.vz += 8;
	if (padd & PADR1) 	vec.vz -= 8;

	/* rotation matrix of the light source: 光源の回転マトリクス計算 */
	RotMatrix(&lgtang, rotlgt);	
	MulMatrix(rotlgt, rottrans);

	/* set rot-trans-matrix of the cube: 立方体の回転マトリクス計算 */
	RotMatrix(&ang, rottrans);	
	TransMatrix(rottrans, &vec);	

	FntPrint("tuto4: lighting angle=(%d,%d,%d)\n",
		 lgtang.vx, lgtang.vy, lgtang.vz);
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

	/* Flat shading 4 point Polygon declared
	   : フラットシェーディング４角形プリミティブの初期設定 */
	for(i = 0;i < 6;i++)
		SetPolyF4(&db->s[i]);	
}

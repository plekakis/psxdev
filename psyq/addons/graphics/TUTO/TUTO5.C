/* $PSLibId: Runtime Library Release 3.6$ */
/*		    tuto5: cube with texture mapping
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *		Draw 3D objects (cube) with lighting texture
 *		  光源のあるテクスチャ立方体の描画
 :
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define SCR_Z	(1024)		
#define	OTSIZE	(4096)

/* primitive buffer: プリミティブ関連のバッファ */
typedef struct {
	DRAWENV		draw;		/* drawing environment: 描画環境 */
	DISPENV		disp;		/* display environment: 表示環境 */
	u_long		ot[OTSIZE];	/* OT: オーダリングテーブル */
	POLY_FT4	s[6];		/* sides of cube: 立方体の側面 */
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
static void init_prim(DB *db, u_short tpage, u_short clut);

main()
{
	extern u_long	bgtex[];	/* trexture pattern (see bgtex.c) */
	
	DB	db[2];			/* double buffer */
	DB	*cdb	;		/* current buffer */
	MATRIX	rottrans;		/* rot-trans matrix */
	MATRIX		rotlgt;		/* light rotation matrix */
	MATRIX		light;		/* light matrix */
	CVECTOR		col[6];		/* cube color */
	
	int		i;		/* work */
	int		dmy, flg;	/* dummy */
	u_short		tpage, clut;	/* clut  and tpage */
	
	/* initialize environment for double buffer (interlace)
	 :ダブルバッファ用の環境設定（インターレースモード）
	 */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* load texture and CLUT: テクスチャ、CLUT のロード */
	clut  = LoadClut(bgtex, 0, 480);
	tpage = LoadTPage(bgtex + 0x80, 0, 0, 640, 0, 256, 256);

	SetBackColor(64, 64, 64);	
	SetColorMatrix(&cmat);		

	/* set primitive parametes on buffer: プリミティブバッファの初期設定 */
	init_prim(&db[0], tpage, clut);	
	init_prim(&db[1], tpage, clut);	

	/* set surface colors. 
	 * When ShadeTex is enable, the unit value of the color is 0x80
	 * not 0xff.
	 : 立方体の側面の色の設定
	 * 輝度値に 0x80 が設定されるともとのテクスチャ色がでる。
	 */
	for (i = 0; i < 6; i++) {
		col[i].cd = db[0].s[0].code;	/* CODE */
		col[i].r  = 0x80;
		col[i].g  = 0x80;
		col[i].b  = 0x80;
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
		add_cubeFT4L(cdb->ot, cdb->s, &rottrans, col);
		
		VSync(0);
		ResetGraph(1);
		
		/* clear background: 背景のクリア */
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		/* draw primitives: 描画 */
		/* DumpOTag(cdb->ot+OTSIZE-1);	/* for debug */
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

	FntPrint("tuto5: lighting angle=(%d,%d,%d)\n",
		 lgtang.vx, lgtang.vy, lgtang.vz);
	return(ret);
}

/*
 *	Initialization assosiate with Primitives.
 :	プリミティブ関連の初期設定
 */
/* DB	*db;	primitive buffer: プリミティブバッファ */
static void init_prim(DB *db, u_short tpage, u_short clut)
{
	int i;

	/* Textrued 4 point Polygon declared
	   : テクスチャ４角形プリミティブの初期設定 */
	for(i = 0; i < 6; i++) {
		SetPolyFT4(&db->s[i]);	
		setUV4(&db->s[i], 0, 0, 0, 64, 64, 0, 64, 64);
		setRGB0(&db->s[i], 128, 128, 128);
		db->s[i].tpage = tpage;
		db->s[i].clut  = clut;
	}
}

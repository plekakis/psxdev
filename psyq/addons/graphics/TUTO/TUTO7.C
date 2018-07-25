/* $PSLibId: Runtime Library Release 3.6$ */
/*		  tuto7: many cubes with local coordinates
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		      Drawing many 3D objects.
 :		    複数の 3D オブジェクトの描画
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define SCR_Z	1024		
#define	OTSIZE	4096
#define NCUBE	256		/* max number of cubes:
				   表示する立方体の数の最大値 */

typedef struct {
	POLY_F4		s[6];		/* surface of cube: 立方体の側面 */
} CUBE;

typedef struct {
	DRAWENV		draw;		/* drawing environment: 描画環境 */
	DISPENV		disp;		/* display environment: 表示環境 */
	u_long		ot[OTSIZE];	/* OT: オーダリングテーブル */
	CUBE		c[NCUBE];	/* pointer to CUBE: 立方体のデータ */
} DB;

typedef struct {
	CVECTOR	col[6];			/* color of cube surface: 側面の色 */
	SVECTOR	trans;			/* translation vector (local coord.)
					   : 平行移動ベクトル（ローカル座標）*/
} CUBEATTR;

/* 光源色（ローカルカラーマトリックス） */
static MATRIX	cmat = {
/*  light source   #0, #1, #2,
	 : 光源    #0, #1, #2, */
		ONE*3/4,  0,  0, /* R */
		ONE*3/4,  0,  0, /* G */
		ONE*3/4,  0,  0, /* B */
};

/* the vector of the light source (Local light Matrix） 
   : 光源ベクトル（ローカルライトマトリックス） */
static MATRIX lgtmat = {
	/*          X     Y     Z */
	          ONE,  ONE, ONE,	/* 光源 #0 */
		    0,    0,    0,	/* #1 */
		    0,    0,    0	/* #2 */
};

static int pad_read(int *ncube, 
		MATRIX *world, MATRIX *local, MATRIX *rotlgt);
static void init_attr(CUBEATTR *attr, int nattr);
static init_prim(DB *db);

main()
{
	
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current buffer */
	
	/* attribute of cube: 立方体の属性 */
	CUBEATTR	attr[NCUBE];	
	
	/* world-screen matrix: ワールドスクリーンマトリクス */
	MATRIX		ws;

	/* local-screen: ローカルスクリーンマトリクス */
	MATRIX		ls;

	/* light source matrix: 光源のローカルスクリーンマトリックス */
	MATRIX		lls;		
	
	/* lighint matrix: 最終的な光源マトリックス */
	MATRIX		light;

	/* number of cubes: 表示する立方体の数 */
	int		ncube = NCUBE/2;
	
	int		i;		/* work */
	long		dmy, flg;	/* dummy */

	/* set double buffer environment
	 :  ダブルバッファ用の環境設定（インターレースモード）*/
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* set background color: 背景色の設定 */
	SetBackColor(64, 64, 64);	
	
	/* set local
	   color matrix: ローカルカラーマトリックスの設定 */
	SetColorMatrix(&cmat);		

	/* プリミティブバッファの初期設定 */
	init_prim(&db[0]);
	init_prim(&db[1]);
	init_attr(attr, NCUBE);

	/* display enable: 表示開始 */
	SetDispMask(1);		

	PutDrawEnv(&db[0].draw); /* set DRAWENV: 描画環境の設定 */
	PutDispEnv(&db[0].disp); /* set DISPENV: 表示環境の設定 */

	while (pad_read(&ncube, &ws, &ls, &lls) == 0) {

		cdb = (cdb==db)? db+1: db;	
		ClearOTagR(cdb->ot, OTSIZE);	

		/* Calcurate Matrix for the light source;
		 * Notice that MulMatrix() destroys current matrix.
		 : 光源マトリックスの設定
		 * Mulmatrix() はカレントマトリクスを破壊することに注意
		 */
		PushMatrix();
		MulMatrix0(&lgtmat, &lls, &light);
		PopMatrix();
		SetLightMatrix(&light);
		
		/* put the primitives of cubes int OT: 立方体のＯＴへの登録 */
		limitRange(ncube, 1, NCUBE);
		
		/* add cubes to the OT
		 * note that only translation vector of the local
		 * screen matrix is changed   ncube 個の立方体を登録
		 : OT に登録
		 * ローカルスクリーンマトリクスの移動ベクトル成分だけ
		 * を更新していることに注意
		 */ 
		for (i = 0; i < ncube; i++) {
			RotTrans(&attr[i].trans, (VECTOR *)ls.t, &flg);
			add_cubeF4L(cdb->ot, cdb->c[i].s, &ls, attr[i].col);
		}

		VSync(0);
		ResetGraph(1);

		/* clear background: 背景クリア */
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		DrawOTag(cdb->ot+OTSIZE-1);	/* draw: 描画 */
		FntFlush(-1);			/* for debug */
	}
	/* close controller: コントローラのクローズ */
	PadStop();			
	ResetGraph(1);
	StopCallback();
	return;
}


/* Analyzing PAD and Calcurating Matrix
 : コントローラの解析と、変換マトリックスの計算
 */
/* int	  *ncube	number of cubes: 表示する立方体の数 */
/* MATRIX *ws	 	rottrans matrix for all: 全体の回転マトリックス */
/* MATRIX *ls	 	rottrans matrix for each: 各立方体の回転マトリックス */
/* MATRIX *lls		light local-screen matrix: 光源の回転マトリックス */
static int pad_read(int *ncube, 
		MATRIX *ws, MATRIX *ls, MATRIX *lls)
{
	/* PlayStation treats angle like as follows: 360°= 4096
	   : Play Station では、角度を 360°= 4096 で扱います。*/
	
	/*; 自分が回転するということは自分以外の世界が逆方向に回転する
	   ことと等価なことに注意 */
	 
	/*  rotation angle of world: 全体の回転角度 */
	static SVECTOR	wang    = {0,  0,  0};	
	
	/* rotation angle for each cube: 個々の立方体の回転角度 */
	static SVECTOR	lang   = {0,  0,  0};	
	
	/* lotation angle for light source: 光源の回転角度 */
	static SVECTOR	lgtang = {1024, -512, 1024};	
	
	/* translation vector for all objects: 全体の平行移動ベクトル */
	static VECTOR	vec    = {0,  0,  SCR_Z};

	/* scale of the each cube: 各立方体の拡大・縮小率 */
	static VECTOR	scale  = {1024, 1024, 1024, 0};
	
	SVECTOR	dwang, dlang;
	int	ret = 0;
	u_long	padd = PadRead(0);

	/* quit: 終了 */
	if (padd & PADselect) 	ret = -1;	

	/* rotate all cubes : 立方体（全体）を回転する。*/
	if (padd & PADRup)	wang.vz += 32;
	if (padd & PADRdown)	wang.vz -= 32;
	if (padd & PADRleft) 	wang.vy += 32;
	if (padd & PADRright)	wang.vy -= 32;

	/* rotate each cube: 立方体みを回転する */
	if (padd & PADLup)	lang.vx += 32;
	if (padd & PADLdown)	lang.vx -= 32;
	if (padd & PADLleft) 	lang.vy += 32;
	if (padd & PADLright)	lang.vy -= 32;
	
	/* distance from screen : 原点からの距離 */
	if (padd & PADL1)	vec.vz += 8;
	if (padd & PADL2) 	vec.vz -= 8;

	/* change number of displayed cubes : 表示する立方体の数の変更 */
	if (padd & PADR1)       (*ncube)++;
	if (padd & PADR2)	(*ncube)--;
	limitRange(*ncube, 1, NCUBE-1);

	FntPrint("objects = %d\n", *ncube);
	
	/* calcurate world-screen matrix
	  ワールドスクリーンマトリックスの計算 */
	RotMatrix(&wang, ws);	
	TransMatrix(ws, &vec);

	/* calcurate matrix for each cubes.
	 * In this case, each local-screen matrix is the same because each
	 * cube rotates to the same direction.
	 : 個々の立方体のローカルマトリックスの計算 
	 * この場合は、立方体は同じ方向に回転するのでローカルマトリクスは同じ
	 */
	RotMatrix(&lang, ls);
	
	/* make local-screen matrix:
	 * Notice the difference between  MulMatrix() and MulMatrix2()
	 : ローカルスクリーンマトリックスをつくる 
	 * MulMatrix() と MulMatrix2() の違いに注意
	 */
	MulMatrix2(ws, ls); 
	
	/*: make light matrix:  光源の回転マトリックスの計算 */
	RotMatrix(&lgtang, lls);
	MulMatrix(lls, ls);

	/* scale of the local matrix represents the scale of the ls object.
	 : オブジェクトのスケールはローカルマトリクスのスケールで表現できる */
	ScaleMatrix(ls, &scale);
	
	/*Setting Matrix for all the objects）
	  : マトリックスの設定（全体） */
	SetRotMatrix(ws);	/* rotation: 回転マトリックス */
	SetTransMatrix(ws);	/* translation: 平行移動ベクトル */

	return(ret);
}

#define MIN_D 	64		/* minumus distance between each cube */
#define MAX_D	(SCR_Z/2)	/* maximum distance */
/* CUBEATTR	*attr,	attribute of cube: 立方体の属性 */
/* int		nattr	number of cube: 立方体の数 */
static void init_attr(CUBEATTR *attr, int nattr)
{
	int	i;
	POLY_F4	templ;

	SetPolyF4(&templ);

	for (; nattr; nattr--, attr++) {
		for (i = 0; i < 6; i++) {
			attr->col[i].cd = templ.code;	/* sys code */
			attr->col[i].r  = rand();	/* R */
			attr->col[i].g  = rand();	/* G */
			attr->col[i].b  = rand();	/* B */
		}
		/* Set initial coordinates; スタート位置の設定 */
		attr->trans.vx = (rand()/MIN_D*MIN_D)%MAX_D-(MAX_D/2);
		attr->trans.vy = (rand()/MIN_D*MIN_D)%MAX_D-(MAX_D/2);
		attr->trans.vz = (rand()/MIN_D*MIN_D)%MAX_D-(MAX_D/2);
	}
}
	
/* DB	*db;	primivie buffer: プリミティブバッファ */
static init_prim(DB *db)
{
	int	i, j;

	for (i = 0; i < NCUBE; i++) 
		for (j = 0; j < 6; j++) 
			SetPolyF4(&db->c[i].s[j]);
}

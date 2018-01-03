/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto3: simple cube
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		  	  Draw 3D objects (cube) 
 :		  	  回転する立方体を描画する 
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* libgte functions return 'otz', which indicates the OT linking position.
 * 'otz' is a quater value of the Z in the screen cordinate, and libgte
 * has 15bit (0-0x7fff) z range. Therefore maximum OT size should be 4096
 * (2^14)
 : OTにプリミティブを登録するときに参照する otz の値を返す
 * libgte 関数の多くは、実際の奥行き(z)の値の1/4の値を返すため、
 * OTのサイズも実際の分解能の1/4 (4096) でよい。
 */
#define SCR_Z	(512)		/* screen depth: スクリーンの深さ */
#define	OTSIZE	(4096)		/* OT size: ＯＴのサイズ */

typedef struct {
	DRAWENV		draw;		/* drawing environment: 描画環境 */
	DISPENV		disp;		/* display environment: 表示環境 */
	u_long		ot[OTSIZE];	/* OT: オーダリングテーブル */
	POLY_F4		s[6];		/* cube surface: 立方体の側面 */
} DB;

static int pad_read(MATRIX *rottrans);
static void init_prim(DB *db, CVECTOR *c);

main()
{
	/* double buffer: ダブルバッファ */
	DB		db[2];		
	
	/* current db: 現在のバッファアドレス */
	DB		*cdb;		
	
	/* rotation-translation matrix: 回転・平行移動マトリックス */
	MATRIX		rottrans;	
	
	/* color of cube: 立方体の色 */
	CVECTOR		col[6];		
	
	int		i;		/* work */
	int		dmy, flg;	/* work */

	/* initialize environment for double buffer (interlace)
	 : ダブルバッファ用の環境設定（インターレースモード）*/
	/*	buffer #0	(0,  0)-(640,480)
	 *	buffer #1	(0,  0)-(640,480)
	 */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* set surface colors : 立方体の側面の色を設定する */
	for (i = 0; i < 6; i++) {
		col[i].r = rand();	/* R */
		col[i].g = rand();	/* G */
		col[i].b = rand();	/* B */
	}

	/* set primitive parameters on buffer #0/#1
	   : プリミティブバッファの初期設定 #0/#1 */
	init_prim(&db[0], col);	
	init_prim(&db[1], col);	

	/* enable to display: 表示開始 */
	SetDispMask(1);			

	/*
	 * When using interrace mode, there is no need to changing 
	 * draw/display environment for every frames because the same
	 * same area is used for both drawing and displaying images.
	 * Therefore, the environment should be set only at the first time.
	 * Even in this case, 2 primitive buffers are needed since drawing
	 * process runs parallely with CPU adn GPU.
	 :
	 * インターレースモードの場合、描画／表示ともフレームバッファ中の
	 * 同じ領域を使用しているため、１フレームごとに描画環境／表示環境の
	 * 切替えを行う必要がありません。
	 * したがって環境設定は最初に一度だけ行います。
	 * ただし、描画はプログラムの処理と並列に行われるため、
         *  プリミティブはダブルで持つ必要があります。
	 */
	PutDrawEnv(&db[0].draw);	/*: 描画環境の設定 */
	PutDispEnv(&db[0].disp);	/*: 表示環境の設定 */
	
	/* loop while [select] key: select キーが押されるまでループする */
	while (pad_read(&rottrans) == 0) {	

		/* swap double buffer: ダブルバッファポインタの切り替え */
		cdb = (cdb==db)? db+1: db;	
		
		/* clear OT.
		 * ClearOTagR() clears OT as reversed order. This is natural
		 * for 3D type application, because OT pointer to be linked
		 * is simply related to Z value of the primivie. ClearOTagR()
		 * is faster than ClearOTag because it uses hardware DMA
		 * channel to clear.
		 : OT をクリアする。
		 * ClearOTagR() は逆順の OT を生成する。これは 3D のアプリ
		 * ケーションの場合には自然な順番になる。また ClearOTagR()
		 * はハードウェアで OT をクリアするので高速
		 */
		ClearOTagR(cdb->ot, OTSIZE);	

		/* add cube int OT: 立方体をＯＴに登録する */
		add_cubeF4(cdb->ot, cdb->s, &rottrans);

		/* When using interlaced single buffer, all drawing have to be
		 * finished in 1/60 sec. Therefore we have to reset the drawing
		 * procedure at the timing of VSync by calling ResetGraph(1)
		 * instead of DrawSync(0)
		 : インターレースモードの場合は、すべての描画処理は 1/60sec で
		 * 終了しなくてはならない。そのため、DrawSync(0) の代わりに
		 * ResetGraph(1) を使用してVSync のタイミングで描画を打ち切る
		 */
		VSync(0);	
		ResetGraph(1);	

		/* clear background: 背景のクリア */
		ClearImage(&cdb->draw.clip, 60, 120, 120);
		
		/* Draw Otag.
		 * Since ClearOTagR() clears the OT as reversed order, the top
		 * pointer of the table is ot[OTSIZE-1]. Notice that drawing
		 * start point is not ot[0] but ot[OTSIZE-1].
		 : ClearOTagR() は OT を逆順にクリアするので OT の先頭ポイ
		 * ンタは ot[0] ではなくて ot[OTSIZE-1] になる。そのため、
		 * DrawOTag() は ot[OTSIZE-1] から開始しなくてはならない
		 * ことに注意
		 */
		/*DumpOTag(cdb->ot+OTSIZE-1);	/* for debug */
		DrawOTag(cdb->ot+OTSIZE-1);	
		FntFlush(-1);
	}
        /* close controller: コントローラをクローズ */
	PadStop();	
	ResetGraph(1);
	StopCallback();
	return;
}

/* 
 *  Analyzing PAD and setting Matrix
 : コントローラの解析と、変換マトリックスの設定を行う。
 */
/* MATRIX *rottrans;	回転・平行移動マトリックス */
static int pad_read(MATRIX *rottrans)
{
	
	/* angle of rotation: 回転角度( 360°= 4096 ) */
	static SVECTOR	ang  = { 0, 0, 0};	
	
	/* translation vertex: 平行移動ベクトル */
	static VECTOR	vec  = {0, 0, SCR_Z};	

	/* read from controller: コントローラからデータを読み込む */
	u_long	padd = PadRead(1);	
	
	int	ret = 0;

	/* quit: 終了 */
	if (padd & PADselect) 	ret = -1;	

	/* change rotation angle: 回転角度の変更（z, y, x の順に回転） */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* distance from screen : 原点からの距離 */
	if (padd & PADL1)	vec.vz += 8;
	if (padd & PADR1) 	vec.vz -= 8;

	/* calculate matrix: マトリックスの計算 */
	RotMatrix(&ang, rottrans);	/* rotation: 回転 */
	TransMatrix(rottrans, &vec);	/* translation: 平行移動 */

	/* set matrix: マトリックスの設定 */
	SetRotMatrix(rottrans);		/* rotation: 回転 */
	SetTransMatrix(rottrans);	/* translation: 平行移動 */

	/* print status: 現在のジオメトリ状況をプリント */
	FntPrint("tuto3: simple cube angle=(%d,%d,%d)\n",
		 ang.vx, ang.vy, ang.vz);
		
	return(ret);
}

/*
 *	Initialization of Primitives
 :	プリミティブの初期化
 */
/*DB	*db;	primitive buffer: プリミティブバッファ */
/*CVECTOR *c;	coloer of cube surface: 側面の色 */
static void init_prim(DB *db, CVECTOR *c)
{
	int	i;

	/* initialize for side polygon: 側面の初期設定 */
	for (i = 0; i < 6; i++) {
		/* initialize POLY_FT4: フラット４角形プリミティブの初期化 */
		SetPolyF4(&db->s[i]);	
		setRGB0(&db->s[i], c[i].r, c[i].g, c[i].b);
	}
}

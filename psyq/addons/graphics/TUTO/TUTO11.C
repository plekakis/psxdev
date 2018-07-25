/* $PSLibId: Runtime Library Release 3.6$ */
/*		 tuto11: special effect (mosaic)
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *		               Mosaic
 :			      モザイク
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define NCELL_X	16		/* number of cells: セル数 */
#define	NCELL_Y	16

typedef struct {
	DRAWENV		draw;		/* drawing environment: 描画環境 */
	DISPENV		disp;		/* display environment: 表示環境 */
	u_long		ot;		/* OT: オーダリングテーブル */
	POLY_FT4	bg[NCELL_X*NCELL_Y];

} DB;

static void mozaic(u_long *ot, POLY_FT4 *ft4, int ox, int oy, int dx, 
		int dy, int ou, int ov, int du, int dv,
		int divx, int divy, int rate);
main()
{
	/* BG texture pattern: BG テクスチャパターン(bgtex.c) */
	extern	u_long	bgtex[];	
	
	DB		db[2];		/* primitive double buffer */
	DB		*cdb;		/* current buffer */
	SVECTOR		x0;		/* positoin */
	u_short		tpage;		/* texture page */
	u_short		clut;		/* texture CLUT */
	int		id, i, j;	/* work */
	u_long		padd;
	
	/* mosaic rate (0-7): モザイクのレート（０～７） */
	int		mrate = 0;	

	PadInit(0);		
	ResetGraph(0);		
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));
	
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* load texture page: テクスチャページをロードする。(bgtex.c) */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256,256);
	clut  = LoadClut(bgtex, 0,500);

	for (i = 0; i < 2; i++) {
		/* set backgound color 各バッファの背景色をきめる。*/
		db[i].draw.isbg = 1;
		setRGB0(&db[i].draw, 0, 0, 0);	

		/* initialize POLY_FT4: ポリゴンプリミティブを初期化 */
		for (j = 0; j < NCELL_X*NCELL_Y; j++) {
			SetPolyFT4(&db[i].bg[j]);
			SetShadeTex(&db[i].bg[j], 1);
			db[i].bg[j].tpage = tpage;
			db[i].bg[j].clut  = clut;
		}
	}

	/* set sprite initial postion: スプライトの表示位置の初期値を決定 */
	setVector(&x0, 0,   0, 0);

	/* main loop: メインループ */
	SetDispMask(1);
	while (1) {

		FntPrint("tuto11: mosaic\n");
		
		/* initialize primitive buffer: バッファ初期化 */
		cdb = (cdb==db)? db+1: db;
		ClearOTag(&cdb->ot, 1);

		/* read controller: コントロールパッドを解析 */
		padd = PadRead(1);
		if (padd & PADselect) 	break;
		if (padd & PADRup)	mrate++;
		if (padd & PADRdown)	mrate--;
		if (padd & PADLright)	x0.vx++;
		if (padd & PADLleft)	x0.vx--;
		if (padd & PADLup)	x0.vy--;
		if (padd & PADLdown)	x0.vy++;

		/* mosaic: モザイク */
		limitRange(mrate, 0, 8);
		mozaic(&cdb->ot, cdb->bg,
				x0.vx, x0.vy, 256, 256, 0, 0, 256, 256,
				NCELL_X, NCELL_Y, mrate);

		/* draw: 描画 */
		DrawSync(0);	
		VSync(0);	
		PutDrawEnv(&cdb->draw); 
		PutDispEnv(&cdb->disp); 
		DrawOTag(&cdb->ot);	
		FntFlush(-1);
	}
	/* close controller: コントローラをクローズ */
	PadStop();
	ResetGraph(1);
	StopCallback();
	return;
}

/*
 *	Example for MOSAIC
 :	モザイクの例
 */
/* u_long *ot,			
/* POLY_FT4 *ft4,	primitive: プリミティブ */
/* int ox, oy,		left-upper corner: プリミティブの左上座標値 */
/* int dx, dy		size of primitive: プリミティブサイズ */
/* int ou, ov		left-upper corner of texture: テクスチャの左上座標値 */
/* int du, dv,	 	texture size: テクスチャサイズ */
/* int divx, divy,	number of division: 分割数 */
/* int rate		mosaic rate: モザイクレート */
static void mozaic(u_long *ot, POLY_FT4 *ft4, int ox, int oy, int dx, 
		int dy, int ou, int ov, int du, int dv,
		int divx, int divy, int rate)
{
	
	/* unit for mosic: モザイクをかける単位 */
	int	sx = dx/divx, sy = dy/divx;	
	int	su = du/divy, sv = du/divy; 	
	
	/* loop counter: ループカウンター */
	int	ix, iy;				
	
	int	x, y, u, v;			/* work */
	int	u0, v0, u1, v1;			/* work */

	/* mosic by changing (u,v) value of each primitive
	   : (u, v) の値を変更してモザイクをかける */
	for (v = ov, y = oy, iy = 0; iy < divy; iy++, v += sv, y += sy) {
		for(u = ou, x = ox, ix = 0; ix < divx; ix++, u += su, x += sx){

			/* calcrating (u, v)  
			 * The essense management of mosaic is here.
			 : (u, v) 値の計算 
			 * モザイクの基本はこの２行
			 */
			u0 = u + rate;		v0 = v + rate;
			u1 = u+su - rate;	v1 = v+sv - rate;

			/* check overflow: (u, v) 値のオーバーフローチェック */
			if (u1 >= 256)	u1 = 255;
			if (v1 >= 256)	v1 = 255;

			/* set values for primitive 
			   : プリミティブに値をセットする */
			setUV4(ft4, u0, v0, u1, v0, u0, v1, u1, v1);
			setXYWH(ft4, x, y, sx, sy);

			AddPrim(ot, ft4);	/* add to OT: OT に登録 */
			ft4++;			
		}
	}
}


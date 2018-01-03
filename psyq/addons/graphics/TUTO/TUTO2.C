/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto2: RotTransPers
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		  Draw rotating plate using GTE functions
 :		  回転する１枚の板を GTE を使って描画する
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define SCR_Z	512		/* distant to screen: スクリーンの深さ */

/*#define DEBUG*/

/* primitive buffer: プリミティブバッファ */
typedef struct {
	DRAWENV		draw;		/* drawing environment: 描画環境 */
	DISPENV		disp;		/* display environment: 表示環境 */
	u_long		ot;		/* ordering table: ＯＴ */
	POLY_G4		wall;		/* primitive: ポリゴンプリミティブ */
} DB;

static int pad_read(void);
static void init_prim(DB *db);

main()
{
	SVECTOR	x[4];		/* walls position: 'wall'の座標値 */
	DB	db[2];		/* primitive double buffer
				   : プリミティブダブルバッファ */
	DB	*cdb;		/* current DB
				   : ダブルバッファのうち現在のバッファ */
	long	dmy, flg;	/* work */
	int	i;

	/* initialize wall position: 'wall'のスタート座標値の設定 */
	setVector(&x[0], -128, -128, 0); setVector(&x[1],  128, -128, 0);
	setVector(&x[2], -128,  128, 0); setVector(&x[3],  128,  128, 0);

	/* initialize environment for double buffer 
	   : 描画・表示環境をダブルバッファ用に設定する*/
	/*	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,240)-(320,480) (320x240)
	 */
	init_system(160, 120, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* init primitives: プリミティブの初期設定 */
	init_prim(&db[0]);
	init_prim(&db[1]);

	/* display: 表示開始 */
	SetDispMask(1);

	/* main loop: メインループ */
	while (pad_read() == 0) {

		FntPrint("tuto2: use GTE\n");

		/* swap double buffer ID: ダブルバッファの交換 */
		cdb = (cdb==db)? db+1: db;	

		/* clea ordering table: ＯＴのクリア */
		ClearOTag(&cdb->ot, 1);		

		/* rotate and shift each vertex of the wall
		 : ローカル座標からスクリーン座標へ、座標変換と透視変換を行う。
		 * 変換マトリックスは、pad_read()内で設定。
		 */
                RotTransPers(&x[0], (long *)&cdb->wall.x0, &dmy, &flg);
                RotTransPers(&x[1], (long *)&cdb->wall.x1, &dmy, &flg);
                RotTransPers(&x[2], (long *)&cdb->wall.x2, &dmy, &flg);
                RotTransPers(&x[3], (long *)&cdb->wall.x3, &dmy, &flg);

#ifdef DEBUG
		dumpXY4(&cdb->wall);
#endif
		/*: ＯＴに登録 */
		AddPrim(&cdb->ot, &cdb->wall);		

		DrawSync(0);	/* wait for end of drawing: 描画の終了を待つ */
		VSync(0);	/* wait for the next VBLNK: 垂直同期を待つ */

		/* swap double buffer
		  ; ダブルバッファの実際の切り替えはここで行われる */
		
		PutDrawEnv(&cdb->draw); /* update DRAWENV: 描画環境の更新 */
		PutDispEnv(&cdb->disp); /* update DISPENV: 表示環境の更新 */
#ifdef DEBUG
		DumpOTag(&cdb->ot);
#endif
		DrawOTag(&cdb->ot);	/* draw: 描画する */
		FntFlush(-1);		/* flush print buffer */
	}
        PadStop();	
	ResetGraph(1);
	StopCallback();
	return;
}

/* analyzie controller and set matrix for rotation and translation.
 : コントローラの解析をして、回転・平行移動のマトリックスを設定する。
 */
static int pad_read(void)
{
	/* Because PlayStation treats angles like that :360°= 4096,
	 * you need to set angles as follows:
	 : Play Station では、角度を 360°= 4096 で扱います。
	 * したがって、角度の設定をする場合は、以下のようになる。
	 */
	/*	 45°=  512
	 *	 90°= 1024
	 *	180°= 2048
	 */
	/* rotation angle: 回転角度 */
	static SVECTOR	ang   = {512, 512, 512};	
	
	/* translate vector: 平行移動ベクトル */
	static VECTOR	vec   = {0,     0, SCR_Z};	
	
	/* work matrix */
	static MATRIX	m;				

	int	ret = 0;

	/* get controller status: コントローラからデータを読み込む */
	u_long	padd = PadRead(1);	

	/* quit from program: プログラムの終了 */
	if (padd & PADselect) 	ret = -1;

	/* change value of rotation angle: 回転角度の変更 */
	if (padd & PADRup)	ang.vx += 32;
	if (padd & PADRdown)	ang.vx -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* change scale: 拡大・縮小 */
	if (padd & PADLup)	vec.vz += 8;
	if (padd & PADLdown) 	vec.vz -= 8;

	ang.vx += 32;

	/* rotate: 回転角度（z, y, x の順で回転） */
	RotMatrix(&ang, &m);		
	
	/* translate:  平行移動ベクトル */
	TransMatrix(&m, &vec);		

	/* set rotation: 回転角度 */
	SetRotMatrix(&m);		
	
	/* set translation: 平行移動ベクトル */
	SetTransMatrix(&m);		

	return(ret);
}

/* Initialization of Primitive buffer
 : プリミティブバッファの初期化
 */
/* DB *db;	プリミティブバッファ */
static void init_prim(DB *db)
{
	/* auto background clear mode: 背景自動クリアモード */
	db->draw.isbg = 1;			
	
	/* set background color: 背景色の設定 */
	setRGB0(&db->draw, 60, 120, 120);	

	/* initialize 'wall' primitive: 'wall'の初期設定 */
	SetPolyG4(&db->wall);
	
	/* set colors for each vertex: ４頂点の色を設定 */
	setRGB0(&db->wall, 0xff, 0xff, 0x00);
	setRGB1(&db->wall, 0xff, 0x00, 0xff);
	setRGB2(&db->wall, 0x00, 0xff, 0xff);
	setRGB3(&db->wall, 0xff, 0x00, 0x00);
}


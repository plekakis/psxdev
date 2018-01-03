/* $PSLibId: Runtime Library Release 3.6$ */
/* 			tuto8: 1D scroll
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		    Sample of 1-D BG scroll
 :		   1D スクロール BG の実験
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* depth of OT is 8: オーダリングテーブルの深さは、８ */
#define OTSIZE		8

/* 
 * Sprite primiteives have no texture page member. 
 * We define new primitive type which has a texture pages.
 : 通常のスプライトは、テクスチャページを持たないので、ここで、
 * 新しくテクスチャページ付きスプライトを定義する。
 */
typedef struct {
	DR_MODE	mode;	/* mode primitive: モード設定プリミティブ */
	SPRT	sprt;	/* sprite primitive: スプライト プリミティブ */
} TSPRT;

/* Structures which contains a information about texture location.
   : テクスチャ関係の情報を集めた構造体 */

typedef struct {
	/* texture address on main memory: テクスチャパターンがあるアドレス */
	u_long	*addr;	
	
	/* texture addres on frame buffer: テクスチャパターンをロードする場所*/
	short	tx, ty;	
	
	/* texture address on frame buffer: テクスチャ CLUT をロードする場所 */
	short	cx, cy;	
	
	/* texture page ID: テクスチャページ ID （後から計算する）*/
	u_short	tpage;	
	
	/* texture CLUT ID: テクスチャ CLUT ID （後から計算する）*/
	u_short	clut;	
} TEX;

/* primitive double buffer :プリミティブダブルバッファ */
typedef struct {
	DRAWENV		draw;		/* drawing environment: 描画環境 */
	DISPENV		disp;		/* display environment: 表示環境 */
	u_long		ot[OTSIZE];	/* OT: オーダリングテーブル */
	TSPRT		far0, far1;	/* far mountains: 遠くの山々 */
	TSPRT		near0, near1;	/* near mountains: 近くの山々 */
	TSPRT		window;		/* train window: 電車の窓 */
} DB;

static int update(DB *db,int padd);
static void add_sprite(u_long *ot,TSPRT *tsprt,
			int x0,int y0,int u0,int v0,int w,int h,TEX *tex);
static int SetTSprt(TSPRT *tsprt,int dfe,int dtd,int tpage,RECT *tw);

main()
{
	/* packet double buffer: パケットダブルバッファ */
	DB		db[2];		
							  
	/* current burrer: 現在のバッファアドレス */
	DB		*cdb;		
	
	/* controller data: コントロールパッドのデータ */
	int	padd;		

	/* reset controller: コントローラのリセット */
	PadInit(0);             
	
	/* reset graphic system: 描画・表示環境のリセット */
	ResetGraph(0);		
	
	/* set debug mode: デバッグモードの設定 */
	SetGraphDebug(0);	

	/* テクスチャデータをフレームバッファに転送 */
	load_tex();		

	/* set double buffer environment: ダブルバッファ用の環境設定 */
	/*	buffer #0:	(0,  0)-(255,239) (256x240)
	 *	buffer #1:	(0,240)-(255,479) (256x240)
	*/
	SetDefDrawEnv(&db[0].draw, 0,   0, 256, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 256, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 256, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 256, 240);
	db[0].draw.isbg = db[1].draw.isbg = 1;

	SetDispMask(1);		/* 表示開始 */

	while (((padd = PadRead(1)) & PADselect) == 0) {

		/* swap double buffer ID: db[0], db[1] を交換 */
		cdb = (cdb==db)? db+1: db;	

		/* update coordinates: 座標をアップデート */
		update(cdb, padd);		

		DrawSync(0);	/* wait for end of drawing: 描画の終了を待つ */
		VSync(0);	/* wait for VBLNK: 垂直同期を待つ */

		/* swap double buffer: ダブルバッファの切替え */
		PutDrawEnv(&cdb->draw);	/* update DRAWENV: 描画環境の更新 */
		PutDispEnv(&cdb->disp);	/* update DISPENV: 表示環境の更新 */
		DrawOTag(cdb->ot);	/* draw: 描画 */
	}

	/* quit: 終了 */
	PadStop();	
	ResetGraph(1);
	StopCallback();
	return;
}

/* 
 * 5 8bit texture pages are used here.
 * Both Polygons used for far-landscape and close-landscape use 256x256
 * These pages are used for far landscae, near landescape and train window.
 * 2 pages (512x256) are used for each landscape. 1 page is used for
 * train window.
 * Each landscape is rap-rounded by 512 pixel.
 * Details of the sprite is as follows:
 : テクスチャページは５枚使用（8bit モード)
 * 遠景、近景とも、256x256 を２枚横にならべてその一部を表示する。
 * 景色はラップラウンドする。
 * 各ページの内容は以下の通り
 */
extern u_long far0[];		/* far mountains(left):   遠くの山々(左側)*/
extern u_long far1[];		/* far mountains(right):  遠くの山々(右側)*/
extern u_long near0[];		/* near mountains(left):  近くの山々(左側)*/
extern u_long near1[];		/* near mountains(right): 近くの山々(右側)*/
extern u_long window[];		/* train window: 汽車の窓 */

/* define ID to every textures: 各テクスチャに通し番号を定義する。*/
#define TEX_FAR0	0
#define TEX_FAR1	1
#define TEX_NEAR0	2
#define TEX_NEAR1	3
#define TEX_WINDOW	4

/* Initial value of the texture structure: テクスチャ構造体の初期値 */
static TEX tex[] = {
	/*addr   tx   ty cx   cy tpage clut			*/
	/*--------------------------------------------------------------*/ 
	far0,   512,   0, 0, 481,    0,   0,	/* far0    */ 
	far1,   512, 256, 0, 482,    0,   0,	/* far1    */ 
	near0,  640,   0, 0, 483,    0,   0,	/* near0   */ 
	near1,  640, 256, 0, 484,    0,   0,	/* near1   */ 
	window, 768,   0, 0, 485,    0,   0,	/* window  */ 
	0,					/* terminator: 終端 */
};

/* Load all texture data to V-RAM at once.
   : テクスチャページをまとめてフレームバッファへ転送する。*/
load_tex(void)
{
	int	i;

	/* loop while addr is not 0 : 'addr' が 0 でない限りループ */
	for (i = 0; tex[i].addr; i++) {	

		/* Load texture pattern : テクスチャパターンをロード */
		tex[i].tpage = LoadTPage(tex[i].addr+0x80, 1, 0,
					 tex[i].tx, tex[i].ty,  256, 256);
		
		/* Load texture clut : テクスチャ CLUT をロードする */
		tex[i].clut = LoadClut(tex[i].addr, tex[i].cx, tex[i].cy);
	}
}

/* Read controller and update the scroll parameters*
   : コントロールパッドを読んでスクロールのパラメータを決定する。*/

/* DB	*db,	primitive buffer: プリミティブバッファ */
/* int	padd	controller data:  コントローラのデータ */
static int update(DB *db, int padd)
{
	
	static int 	ofs_x = 0;	/* position: 風景の移動量 */
	static int	v     = 0;	/* verosicy: 移動速度 */
	
	/* texture: テクスチャデータ */
	TEX	*tp[2];			
	
	/* direction 0:right to left, 1:reverse: 移動方向 0:右→左, 1:左→右 */
	int	side;			
	
	/* flag to disaplay near mountains: 近景表示 1:する, else:しない */
	int	isnear   = 1;		
	
	/* flag to display window:  窓の表示 1:する, else:しない */
	int	iswindow = 1;		
	
	int	d;

	/* update speed: コントローラの解析 */
	if (padd & PADLright)	v++;	/* speed up: スピードアップ */
	if (padd & PADLleft)	v--;	/* speed down: スピードダウン */
	ofs_x += v;			/* total amount: 移動量の累積をとる */

	/* if needed, it is surpressed to display landscapes or a window.  
	   : 場合によっては、近景、汽車の窓の表示を禁止する。*/
	if (padd & PADR1)	isnear   = 0;
	if (padd & PADL1)	iswindow = 0;

	/* clear OT: ＯＴのクリア */
	ClearOTag(db->ot, OTSIZE);

	/* display far landscape. (The period of rap-round is 512 pixel) 
	   : 遠くの山々を表示する。景色は横 512 でラップラウンド */
    display_far:

	side = (ofs_x/4)&0x100;	/* decide L->R or R->L: 表示向きを決定 */
	d    = (ofs_x/4)&0x0ff;	/* limited between 0-256:
				   移動量を 256 で正規化 */

	/* Put TSPRT primite into OT.
	 * The scroll is achived by changing (u0,v0) not (x0, y0).
	 * Left primitive draws the area of (0,0)-(256-d,240)
	 * Right primitive draws the area of (256-d,0)-(256,240)
	 * 'd' is valid according to the  scroll position.
	 *
	 : TSPRT を ＯＴに登録する関数をコールする 
	 * x0, y0 ではなく、u0, v0 を動かしてスクロールを行なう
	 * 右の一枚は、区間 (256-d,0)-(256,240) を担当する
	 * 左の一枚は、区間 (0, 0)-(256-d,240) を担当する。
	 */
	tp[0] = &tex[side==0? TEX_FAR0: TEX_FAR1];	/* right hand */
	tp[1] = &tex[side==0? TEX_FAR1: TEX_FAR0];	/* left hand */

	add_sprite(db->ot, &db->far0, 0,     16, d, 0, 256-d, 168, tp[0]);
	add_sprite(db->ot, &db->far1, 256-d, 16, 0, 0, d,     168, tp[1]);

	/* Display near landscape.( The cycle of lap-round is 512 pixel  
	   : 近くの山々を表示する。景色は横 512 でラップラウンド */
    display_near:
	if (isnear == 0) goto display_window;

	side = (ofs_x/2)&0x100;	
	d    = (ofs_x/2)&0x0ff;

	tp[0] = &tex[side==0? TEX_NEAR0: TEX_NEAR1];
	tp[1] = &tex[side==0? TEX_NEAR1: TEX_NEAR0];

	add_sprite(db->ot+1, &db->near0, 0,     32, d, 0, 256-d, 168, tp[0]);
	add_sprite(db->ot+1, &db->near1, 256-d, 32, 0, 0, d,     168, tp[1]);

	/* draw window: 窓を描く */
    display_window:
	if (iswindow == 0) return;

	add_sprite(db->ot+2,
		   &db->window, 0, 0, 0, 0, 256, 200, &tex[TEX_WINDOW]);
}

/* Put TSPR primitive into OT : TSPRT をＯＴに登録する */
/* u_long *ot,	  OT: OT */
/* TSPRT  *tsprt, TSPRT primitive: 登録するテクスチャスプライトプリミティブ */
/* int	  x0,y0	  left-upper corner of TSPRT: スプライトの左上座標値 */
/* int	  u0,v0	  left-upper corner of TSPRT(U): テクスチャの左上座標値（ｕ）*/
/* int	  w,h	  size of texture pattern: テクスチャのサイズ*/
/* TEX	  *tex	  texture attributes: テクスチャデータ */

static void add_sprite(u_long *ot,TSPRT *tsprt,
			int x0,int y0,int u0,int v0,int w,int h,TEX *tex)
{
	/* initialize TSPRT primitive: TSPRT プリミティブを初期化 */
	SetTSprt(tsprt, 1, 1, tex->tpage, 0);

	/* shading off: シェーディングオフ */
	SetShadeTex(&tsprt->sprt, 1);	
	
	/* set left-upper corner point of (x,y): スプライトの左上座標値 */
	setXY0(&tsprt->sprt, x0, y0);	
	
	/* set left-upper corner point of (u,v): テクスチャの左上座標値 */
	setUV0(&tsprt->sprt, u0, v0);	
	
	/* set size of sprite: スプライトのサイズ */
	setWH(&tsprt->sprt, w, h);	
	
	/* set CLUT ID: テクスチャCLUT ID */
	tsprt->sprt.clut = tex->clut;	

	/* add to OT: OT に登録 */
	AddPrim(ot, &tsprt->mode);	
}

/* TSPRT *tsprt, TSPRT pointer: 値をセットする TSPRT 構造体へのポインタ */
/* int	 dfe,	 same as DRAWENV.dfe: 表示領域への描画  0:不可、1:可 */
/* int	 dtd,	 same as DRAWENV.dtd: ディザ  0:なし、1:あり */
/* int	 tpage,	 texture page: テクスチャページ */
/* RECT	 *tw	 texture window: テクスチャウィンドウ */

static int SetTSprt(TSPRT *tsprt,int dfe,int dtd,int tpage,RECT *tw)
{
	/* initialize MODE primitive: MODE プリミティブを初期化 */
 	SetDrawMode(&tsprt->mode, dfe, dtd, tpage, tw);

	/* initialize SPRT primitive: SPRT プリミティブを初期化 */
	SetSprt(&tsprt->sprt);

	/* Merge 2 primitives to one.
	 * Merging primitives should be located on the continuous
	 * memory area.
	 * The total size of 2 primitives must be under 16 words.
	 : ２つのプリミティブをマージする。
	 * マージするプリミティブは、メモリ上の連続した領域に置かれなくて
	 *  はいけない。マージするプリミティブのサイズは 16 ワード以下
	 */
	if (MargePrim(&tsprt->mode, &tsprt->sprt) != 0) {
		printf("Marge failed!\n");
		return(-1);
	}
	return(0);
}


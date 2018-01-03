/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *	rcube for overlay: PlayStation Demonstration program
 *
 *	"rcube.c" Main routine
 *
 *		Version 3.01			Jan, 28, 1994
 *		Version 3.01a	yoshi		Mar, 31, 1995
 *		Version 3.02			Jan, 9, 1995
 *		Version 3.02a	yoshi		Aug, 3, 1995
 *		Version 3.02b	yoshi		Mar, 4, 1996
 *
 *		Copyright (C) 1993,1994,1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *====================================================================
 * This was rewritten as a child process. Compile conditionally with OVERLAY.
 * 子プロセス用に書き直した。OVERLAYで条件コンパイルする。
 *
 */
#include <sys/types.h>
#include <sys/file.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libcd.h>
#include "table.h"
#include "pos.h"

/*
 * texture data : テクスチャ情報
 */
#define TIM_ADDR 0x80108000		/* stored address of the TIM file to be used : 
                                   使用するTIMファイルの格納アドレス */
/*#define TIM_ADDR 0x80020000		/* stored address of the TIM file to be used :
                                       使用するTIMファイルの格納アドレス */
#define TIM_HEADER 0x00000010

/*
 * modeling data : モデリングデータ情報
 */
#define TMD_ADDR 0x80100000		/* stored address of the TMD file to be used : 
                                   使用するTMDファイルの格納アドレス */
/*#define TMD_ADDR 0x80010000		/* stored address of the TMD file to be used :
                                       使用するTMDファイルの格納アドレス */
u_long *TmdBase;			/* address of object within TMD : 
                               TMDのうち、オブジェクト部のアドレス */
int CurrentTmd; 			/* the TMD number for the TMD being used : 使用中のTMD番号 */

/*
 * ordering table (OT) : オーダリングテーブル (OT)
 */
#define OT_LENGTH  7			/* OT resolution (large) : OT解像度（大きさ） */
GsOT WorldOT[2];			/* OT data (double buffer) : OT情報（ダブルバッファ） */
GsOT_TAG OTTags[2][1<<OT_LENGTH];	/* OT tag area (double buffer) : 
                                       OTのタグ領域（ダブルバッファ） */

/*
 * GPU packet creation area : GPUパケット生成領域
 */
#define PACKETMAX 1000			/* maximum packet number in 1 frame : 
                                   1フレームの最大パケット数 */
PACKET GpuPacketArea[2][PACKETMAX*64];	/* packet area (double buffer) : 
                                           パケット領域（ダブルバッファ） */

/*
 *  object (cube) variable : オブジェクト（キューブ）変数
 */
#define NCUBE 44
#define OBJMAX NCUBE
int nobj;				/* number of cubes : キューブ個数 */
GsDOBJ2 object[OBJMAX];			/* 3D object variable : 3Dオブジェクト変数 */
GsCOORDINATE2 objcoord[OBJMAX];		/* local coordinate variable : ローカル座標変数 */
SVECTOR Rot[OBJMAX];			/* rotation angle : 回転角 */
SVECTOR RotV[OBJMAX];			/* rotation speed (angular velocity) : 
                                   回転スピード（角速度） */
VECTOR Trns[OBJMAX];			/* cube position (parallel displacement) : 
                                   キューブ位置（平行移動量） */
VECTOR TrnsV[OBJMAX];			/* displacement speed : 移動スピード */

/*
 *  VIEW (viewpoint) : 視点（VIEW）
 */
GsRVIEW2  View;				/* 視点変数 */
int ViewAngleXZ;			/* 視点の高さ */
int ViewRadial;				/* 視点からの距離 */
#define DISTANCE 600			/* Radialの初期値 */

/*
 *  light source : 光源
 */
GsF_LIGHT pslt[3];			/* light source data variable x 3 : 光源情報変数×3 */

/*
 *  other : その他...
 */
int Bakuhatu;				/* explosion processing flag : 爆発処理フラグ */
u_long PadData;				/* controller pad data : コントロールパッドの情報 */
u_long oldpad;				/* controller pad data from previous frame : 
                               １フレーム前のパッド情報 */
GsFOGPARAM dq;				/* parameter for depth queue (fog) : 
                               デプスキュー(フォグ)用パラメータ */
int dqf;				/* check if fog is ON : フォグがONかどうか */
int back_r, back_g, back_b;		/* background color : バックグラウンド色 */
#define FOG_R 160
#define FOG_G 170
#define FOG_B 180

/*
 *  function prototype declaration : 関数のプロトタイプ宣言
 */
void drawCubes();
int moveCubes();
void initModelingData();
void allocCube();
void initSystem();
void initAll();
void initTexture();
void initView();
void initLight();
void changeFog();
void changeTmd();
int	datafile_search();
int	datafile_read();

/*
 *  file information : ファイル情報
 */
typedef struct{
	char	*fname;
	void	*addr;
	CdlFILE finfo;
} FILE_INFO;

#define DFILENO 2

static FILE_INFO dfile[DFILENO] = {
	{ "\\DATA\\RCUBE.TMD;1",(void *)TMD_ADDR,0 },
	{ "\\DATA\\RCUBE.TIM;1",(void *)TIM_ADDR,0 } 
};

/*
 *  main routine : メインルーチン
 */
#ifdef OVERLAY
child_rcube()
#else
main()
#endif
{
	RECT rct;

#ifdef OVERLAY
	/* some tricks are used to allow smooth screen transitions : 
	   画面切り替えがスムーズに行くように少し工夫している */
	VSync(0);
	SetDispMask(0);
	ResetGraph(1);
	setRECT(&rct,0,0,1024,512);
	ClearImage(&rct,0,0,0);
	DrawSync(0);
#else
	ResetCallback();
	CdInit();
	PadInit(0);
#endif

	datafile_search(dfile,DFILENO);
	datafile_read(dfile,DFILENO);

	/* initialize system : システムの初期化 */
	initSystem();

	/* other intializations : その他の初期化 */
	Bakuhatu = 0;
	PadData = 0;
	CurrentTmd = 0;
	dqf = 0;
	back_r = back_g = back_b = 0;
	initView();
	initLight(0, 0xc0);
	initModelingData(TMD_ADDR);
	initTexture(TIM_ADDR);
	allocCube(NCUBE);
	
	/* main loop : メインループ */
	while(1) {
		if(moveCubes())
			break;
		GsSetRefView2(&View);
		drawCubes();
	}

	return(0);
}


/*
 *  draw 3D object : 3Dオブジェクト（キューブ）の描画
 */
void drawCubes()
{
	int i;
	GsDOBJ2 *objp;
	int activeBuff;
	MATRIX LsMtx;

	/* which of the double buffers are active? : 
	   ダブルバッファのうちどちらがアクティブか？ */
	activeBuff = GsGetActiveBuff();

	/* set GPU packet creation address to the start of the area : 
	   GPUパケット生成アドレスをエリアの先頭に設定 */
	GsSetWorkBase((PACKET*)GpuPacketArea[activeBuff]);

	/* clear OT contents : OTの内容をクリア */
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* enter 3D object (cube) into OT : 
	   3Dオブジェクト（キューブ）のOTへの登録 */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* rotation angle -> set in matrix : 回転角->マトリクスにセット */
		RotMatrix(Rot+i, &(objp->coord2->coord));
		
		/* reset flag since matrix has been updated : 
		   マトリクスを更新したのでフラグをリセット */
                objp->coord2->flg = 0;

		/* translation capacity -> set in matrix : 
		   平行移動量->マトリクスにセット */
                TransMatrix(&(objp->coord2->coord), &Trns[i]);
		
		/* calculate matrix for perspective transformation and set in GTE : 
		   透視変換のためのマトリクスを計算してＧＴＥにセット */
		GsGetLs(objp->coord2, &LsMtx);
		GsSetLsMatrix(&LsMtx);
		GsSetLightMatrix(&LsMtx);

		/* perform perspective transformation and enter in OT : 
		   透視変換してOTに登録 */
		GsSortObject4(objp, &WorldOT[activeBuff], 14-OT_LENGTH,getScratchAddr(0));
		objp++;
	}

    /* include pad data in buffer : 
	   パッドの内容をバッファに取り込む */
	oldpad = PadData;
	PadData = PadRead(1);
	
	/* wait for V-BLNK : 
	   V-BLNKを待つ */
	VSync(0);
	
	/* forcibly stop drawing operation for previous frame : 
	   前のフレームの描画作業を強制終了 */
	ResetGraph(1);

	/* swap double buffer : ダブルバッファを入れ換える */
	GsSwapDispBuff();

	/* insert screen clear command at start of OT : 
	   OTの先頭に画面クリア命令を挿入 */
	GsSortClear(back_r, back_g, back_b, &WorldOT[activeBuff]);

	/* begin drawing OT contents in background : 
	   OTの内容をバックグラウンドで描画開始 */
	GsDrawOt(&WorldOT[activeBuff]);
}

/*
 *  move cube : キューブの移動
 */
int moveCubes()
{
	int i;
	GsDOBJ2   *objp;
	
	/* process according to pad value : パッドの値によって処理 */
	if((PadData & PADLleft)>0) {
		ViewAngleXZ++;
		if(ViewAngleXZ >= 72) {
			ViewAngleXZ = 0;
		}
	}
	if((PadData & PADLright)>0) {
		ViewAngleXZ--;
		if(ViewAngleXZ < 0) {
		  ViewAngleXZ = 71;
		}
	}
	if((PadData & PADLup)>0) View.vpy += 100;
	if((PadData & PADLdown)>0) View.vpy -= 100;
	if((PadData & PADRdown)>0) {
		ViewRadial-=3;
		if(ViewRadial < 8) {
			ViewRadial = 8;
		}
	}
	if((PadData & PADRright)>0) {
		ViewRadial+=3;
		if(ViewRadial > 450) {
			ViewRadial = 450;
		}
	}
	if((PadData & PADk)>0) return(-1);
	if(((PadData & PADRleft)>0)&&((oldpad&PADRleft) == 0)) changeFog();
	if(((PadData & PADRup)>0)&&((oldpad&PADRup) == 0)) changeTmd();
	if(((PadData & PADn)>0)&&((oldpad&PADn) == 0)) Bakuhatu = 1;
	if(((PadData & PADl)>0)&&((oldpad&PADl) == 0)) allocCube(NCUBE);

	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;

	/* update position data for cube : キューブの位置情報更新 */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* begin explosion : 爆発の開始 */
		if(Bakuhatu == 1) {

			/* increase rotation speed : 自転速度 up */
			RotV[i].vx *= 3;
			RotV[i].vy *= 3;
			RotV[i].vz *= 3;

			/* set direction of displacement and velocity : 移動方向&速度設定 */
			TrnsV[i].vx = objp->coord2->coord.t[0]/4+
						(rand()-16384)/200;	
			TrnsV[i].vy = objp->coord2->coord.t[1]/6+
						(rand()-16384)/200-200;
			TrnsV[i].vz = objp->coord2->coord.t[2]/4+
						(rand()-16384)/200;
		}
		/* processing for during explosion : 爆発中の処理 */
		else if(Bakuhatu > 1) {
			if(Trns[i].vy > 3000) {
				Trns[i].vy = 3000-(Trns[i].vy-3000)/2;
				TrnsV[i].vy = -TrnsV[i].vy*6/10;
			}
			else {
				TrnsV[i].vy += 20*2;	/* free fall : 自由落下 */
			}

			if((TrnsV[i].vy < 70)&&(TrnsV[i].vy > -70)&&
			   (Trns[i].vy > 2800)) {
				Trns[i].vy = 3000;
				TrnsV[i].vy = 0;

				RotV[i].vx *= 95/100;
				RotV[i].vy *= 95/100;
				RotV[i].vz *= 95/100;
			}


			TrnsV[i].vx = TrnsV[i].vx*97/100;
			TrnsV[i].vz = TrnsV[i].vz*97/100;
		}

		/* update rotation angle (rotation) : 回転角(Rotation)の更新 */
 		Rot[i].vx += RotV[i].vx;
		Rot[i].vy += RotV[i].vy;
		Rot[i].vz += RotV[i].vz;

		/* update translation (Transfer) : 平行移動量(Transfer)の更新 */
 		Trns[i].vx += TrnsV[i].vx;
		Trns[i].vy += TrnsV[i].vy;
		Trns[i].vz += TrnsV[i].vz;

		objp++;
	}

	if(Bakuhatu == 1)
		Bakuhatu++;

	return(0);
}

/*
 *  set cube to initial position : キューブを初期位置に配置
 */
void allocCube(n)
int n;
{	
	int x, y, z;
	int i;
	int *posp;
	GsDOBJ2 *objp;
	GsCOORDINATE2 *coordp;

	posp = cube_def_pos;
	objp = object;
	coordp = objcoord;
	nobj = 0;
	for(i = 0; i < NCUBE; i++) {

		/* intialize object struture : オブジェクト構造体の初期化 */
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		GsInitCoordinate2(WORLD, coordp);
		objp->coord2 = coordp;
		objp->attribute = 0;
		coordp++;
		objp++;

		/* set initial position (read from pos.h) : 
		   初期位置の設定(pos.hから読む) */
		Trns[i].vx = *posp++;
		Trns[i].vy = *posp++;
		Trns[i].vz = *posp++;
		Rot[i].vx = 0;
		Rot[i].vy = 0;
		Rot[i].vz = 0;

		/* initialize velocity : 速度の初期化 */
		TrnsV[i].vx = 0;
		TrnsV[i].vy = 0;
		TrnsV[i].vz = 0;
		RotV[i].vx = rand()/300;
		RotV[i].vy = rand()/300;
		RotV[i].vz = rand()/300;

		nobj++;
	}
	Bakuhatu = 0;
}

/*
 *  initialize function group : イニシャライズ関数群
 */
void initSystem()
{
	int i;

	/* initialize pad : パッドの初期化 */
	PadInit(0);

	/* initialize graphics : グラフィックの初期化 */
	GsInitGraph(640, 480, 2, 0, 0);
	GsDefDispBuff(0, 0, 0, 0);

	/* initialize OT : OTの初期化 */
	for(i = 0; i < 2; i++) {	
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}	

	/* initialize 3D system : 3Dシステムの初期化 */
	GsInit3D();
}

void initModelingData(tmdp)
u_long *tmdp;
{
	u_long size;
	int i;
	int tmdobjnum;
	
	/* skip header : ヘッダをスキップ */
	tmdp++;

	/* mapping to read address : 実アドレスへマッピング */
	GsMapModelingData(tmdp);

	tmdp++;
	tmdobjnum = *tmdp;
	tmdp++; 		/* point to object at beginning : 
	                   先頭のオブジェクトをポイント */
	TmdBase = tmdp;
}

/*
 *  read in texture (transfer to VRAM) : 
 *  テクスチャの読み込み（VRAMへの転送）
 */
void initTexture(tex_addr)
u_long *tex_addr;
{
	RECT rect1;
	GsIMAGE tim1;
	int i;
	
	while(1) {
		if(*tex_addr != TIM_HEADER) {
			break;
		}
		tex_addr++;	/* skip header (1word) : ヘッダのスキップ(1word) */
		GsGetTimInfo(tex_addr, &tim1);
		tex_addr += tim1.pw*tim1.ph/2+3+1;	/* proceed up to next block : 
		                                       次のブロックまで進める */
		rect1.x=tim1.px;
		rect1.y=tim1.py;
		rect1.w=tim1.pw;
		rect1.h=tim1.ph;
		LoadImage(&rect1,tim1.pixel);
		if((tim1.pmode>>3)&0x01) {	/* if ther is a CLUT, then transfer : CLUTがあれば転送 */
			rect1.x=tim1.cx;
			rect1.y=tim1.cy;
			rect1.w=tim1.cw;
			rect1.h=tim1.ch;
			LoadImage(&rect1,tim1.clut);
			tex_addr += tim1.cw*tim1.ch/2+3;
		}
	}
}

/*
 *  intialize viewpoint : 視点の初期化
 */
void initView()
{
	/* set viewpoint variable as initial position : 初期位置を視点変数にセット */
	ViewAngleXZ = 54;
	ViewRadial = 75;
	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpy = -100;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;
	View.vrx = 0; View.vry = 0; View.vrz = 0;
	View.rz=0;

	/* parent coordinates of viewpoint : 視点の親座標 */
	View.super = WORLD;

	/* set : 設定 */
	GsSetRefView2(&View);

	/* Projection */
	GsSetProjection(1000);

	/* mod = normal lighting : モード = 'normal lighting' */
	GsSetLightMode(0);
}

/*
 *  initialize light source : 光源の初期化
 */
void initLight(c_mode, factor)
int c_mode;	/* if 0, white light; if 1, cocktail lights : 
               ０のとき白色光、１のときカクテルライト */
int factor;	/* brightness factor (0 - 255) : 明るさのファクター(0～255) */
{
	if(c_mode == 0) {
		/* set white light : 白色光のセット */
		pslt[0].vx = 200; pslt[0].vy= 200; pslt[0].vz= 300;
		pslt[0].r = factor; pslt[0].g = factor; pslt[0].b = factor;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -50; pslt[1].vy= -1000; pslt[1].vz= 0;
		pslt[1].r=0x20; pslt[1].g=0x20; pslt[1].b=0x20;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
		pslt[2].r=0x0; pslt[2].g=0x0; pslt[2].b=0x0;
		GsSetFlatLight(2,&pslt[2]);
	}
	else {
		/* cocktail lights (using Gouraud) : 
		   カクテルライト（Gouraudで使用） */
		pslt[0].vx = 200; pslt[0].vy= 100; pslt[0].vz= 0;
		pslt[0].r = factor; pslt[0].g = 0; pslt[0].b = 0;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -200; pslt[1].vy= 100; pslt[1].vz= 0;
		pslt[1].r=0; pslt[1].g=0; pslt[1].b=factor;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = 0; pslt[2].vy= -200; pslt[2].vz= 0;
		pslt[2].r=0; pslt[2].g=factor; pslt[2].b=0;
		GsSetFlatLight(2,&pslt[2]);
	}	

	/* ambient light : アンビエント（周辺光） */
	GsSetAmbient(ONE/2,ONE/2,ONE/2);
}

/*
 * fog ON/OFF : フォグのON/OFF
 */
void changeFog()
{
	if(dqf) {
		/* reset fog : フォグのリセット */
		GsSetLightMode(0);
		dqf = 0;
		back_r = 0;
		back_g = 0;
		back_b = 0;
	}
	else {
		/* set fog : フォグの設定 */
		dq.dqa = -600;
		dq.dqb = 5120*4096;
		dq.rfc = FOG_R;
		dq.gfc = FOG_G;
		dq.bfc = FOG_B;
		GsSetFogParam(&dq);
		GsSetLightMode(1);
		dqf = 1;
		back_r = FOG_R;
		back_g = FOG_G;
		back_b = FOG_B;
	}
}

/*
 *  switch TMD data : TMDデータの切り替え
 */
void changeTmd()
{
	u_long *tmdp;
	GsDOBJ2 *objp;
	int i;

	/* switch TMD : TMDを切り替え */
	CurrentTmd++;
	if(CurrentTmd == 4) {
		CurrentTmd = 0;
	}
	objp = object;
	for(i = 0; i < nobj; i++) {
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		objp++;
	}

	/* switch color/brightness of light source according to TMD type : 
	   TMDの種類にあわせて光源の色/明るさを切り替え */
	switch(CurrentTmd) {
	    case 0:
                /* normal (flat) : ノーマル (flat) */
		initLight(0, 0xc0);
		break;
	    case 1:
	        /* semi-transparent (flat) : 半透明 (flat) */
		initLight(0, 0xc0);
		break;
	    case 2:
	        /* Gouraud */
		initLight(1, 0xff);
		break;
	    case 3:
	        /* with texture : テクスチャ付き */
		initLight(0, 0xff);
		break;
	}
}



int
datafile_search(file,nf)

FILE_INFO *file;
int nf;
{
	short i,j;

	for (j = 0; j < nf; j++){
		for (i = 0; i < 10; i++) {	/* ten retries : リトライは 10 回 */
			if (CdSearchFile( &(file[j].finfo), file[j].fname ) != 0) 
				break;
			else
				printf("%s not find.\n",file[j].fname);
		}
	}
}


int
datafile_read(file,nf)

FILE_INFO *file;
int nf;
{
	int	mode = CdlModeSpeed;	
	int	nsector;
	short i,j;
	long cnt;

	for (j = 0; j < nf; j++){
		for (i = 0; i < 10; i++) {	/* ten retries : リトライは 10 回 */
			nsector = (file[j].finfo.size + 2047) / 2048;
		
			/* set target position : ターゲットポジションを設定 */
			CdControl(CdlSetloc, (u_char *)&(file[j].finfo.pos), 0);

			/* begin read : リード開始 */
			CdRead(nsector, file[j].addr, mode);
	
			/* normal operations can be performed behind the read operation.*/
			/* sector count is monitored here until Read is finished*/
			/* リードの裏で通常の処理は実行できる。*/
			/* ここでは、Read が終了するまで残りのセクタ数を監視する */
			while ((cnt = CdReadSync(1, 0)) > 0 ) {
				VSync(0);
			}
		
			/* break if normal exit : 正常終了ならばブレーク */
			if (cnt == 0)	break;
		}
	}
}


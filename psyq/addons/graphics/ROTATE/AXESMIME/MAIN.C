/* $PSLibId: Runtime Library Release 3.6$ */
/***********************************************
 *		axesmime sample
 *
 *	階層構造をもったオブジェクトを
 *	座標系に対する MIMe でアニメーションするサンプル
 ***********************************************/
/***********************************************
 *	main.c
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

#include "paddef.h"
#include "model.h"

/* resolution of OT */
#define OT_LENGTH 10

/* Background Color : 背景色 */
#define BGR 80
#define BGG 80
#define BGB 80

/* View point parameters: 視点パラメータ */
#define DEFAULT_PROJ 2000
#define DEFAULT_VPZ -6000
#define DEFAULT_VPX 0
#define DEFAULT_VPY 0
#define DEFAULT_VRX 0
#define DEFAULT_VRY 0
#define DEFAULT_VRZ 0
#define DEFAULT_RZ 0

/* Light Sources parameteres: 光源パラメータ */
#define DEFAULT_AMBR_F   3000
#define DEFAULT_AMBG_F   2300
#define DEFAULT_AMBB_F   2300
#define DEFAULT_AMBR_S   3700
#define DEFAULT_AMBG_S   3900
#define DEFAULT_AMBB_S   3900
#define DEFAULT_0VX    20
#define DEFAULT_0VY    100
#define DEFAULT_0VZ    100
#define DEFAULT_0R    150
#define DEFAULT_0G    DEFAULT_0R
#define DEFAULT_0B    DEFAULT_0R
#define DEFAULT_1VX    20
#define DEFAULT_1VY    (-50)
#define DEFAULT_1VZ    (-100)
#define DEFAULT_1R    0x80
#define DEFAULT_1G    DEFAULT_1R
#define DEFAULT_1B    DEFAULT_1R
#define DEFAULT_2VX    (-100)
#define DEFAULT_2VY    (-20)
#define DEFAULT_2VZ    -50
#define DEFAULT_2R    0x60
#define DEFAULT_2G    DEFAULT_2R
#define DEFAULT_2B    DEFAULT_2R

static GsOT            Wot[2];			/* Handler of OT*/

int main(void);
void init_all(void);
void finish_all(void);
int obj_interactive(void);
int draw_init(void);
void fnt_init(void);
void view_init(void);
void light_init(void);
static void set_lights(short *ambc, GsF_LIGHT *pslt);
void set_strong_light(void);
void set_faint_light(void);

int main(void)
{
    u_long fn;
    int outbuf_idx;				/* double buffer index */
    GsOT *cwot;					/* current Wot */
    int v0, v1;

    init_all();					/* call initialize functions */

    for (fn=0; ; fn++){				/* main loop */
	outbuf_idx=GsGetActiveBuff();		/* Get double buffer index*/
	cwot= &Wot[outbuf_idx];			/* Current OT */
	GsClearOt(0,0,cwot);			/* Clear OT for using buffer*/

	v0=VSync(1);
	if (obj_interactive()<0) break;		/* pad operation */
	v1=VSync(1);
	FntPrint("time: %d\n", v1-v0);

	model(cwot, OT_LENGTH);			/* add models' entry to OT */

	DrawSync(0);				/* 描画終了待ち */
	VSync(0);
	GsSwapDispBuff();			/* Swap double buffer*/
	GsSortClear(BGR,BGG,BGB,cwot);
	GsDrawOt(cwot);				/* Drawing OT*/
	FntFlush(-1);
    }

    finish_all();				/* call functions for finish */

    return 0;
}

/* initialize functions */
void init_all(void)
{
    ResetCallback();
    PadInit(0);
    ResetGraph(0);		/* Reset GPU : GPUリセット */

    draw_init();
    fnt_init();    
    light_init();
    model_init();				/* in "model.c" */
    view_init();
}

/* functions for finish : プログラムの終了処理 */
void finish_all(void)
{
    PadStop();
    ResetGraph(3);
    StopCallback();
}

/* pad operations : パッド操作 */
int obj_interactive(void)
{
    u_long padd;

    padd=PadRead(0);

    /* finish this program: プログラム終了 */
    if (((padd & (A_PADstart|A_PADselect))== (A_PADstart|A_PADselect))
	|| ((padd & (B_PADstart|B_PADselect))== (B_PADstart|B_PADselect))){
	return -1;
    }

    /* operations of model/MIMe : モデル／MIMe の操作 */
    if (model_move(padd)<0){
	return -1;
    }

    return 0;
}

/***********************************************
 *	draw functions
 ***********************************************/
static GsOT_TAG	zsorttable[2][1<<OT_LENGTH];	/* Area of OT*/
/* initialize for drawing : 描画系初期化 */
int draw_init(void)
{
    
    /* 解像度設定（インターレースモード） */
    GsInitGraph(640,240,GsOFSGPU|GsINTER,1,0);
    GsDefDispBuff(0,0,0,240);	/* Double buffer setting */

    GsInit3D();			/* Init 3D system : ３Dシステム初期化 */

    Wot[0].length=OT_LENGTH;	/* Set bit length of OT handler */
    Wot[0].org=zsorttable[0];	/* Set Top address of OT Area to OT handler */

    /* same setting for anoter OT handler*/
    Wot[1].length=OT_LENGTH;
    Wot[1].org=zsorttable[1];
}

/* initialize for FntPrint(): フォントの初期化 */
void fnt_init(void)
{
    FntLoad(960, 256);
    SetDumpFnt(FntOpen(-300,-100,200,200,0,512));
}

/* Setting view point : 視点設定 */
void view_init(void)
{
    GsRVIEW2  view;			/* View Point Handler*/

    /*---- Set projection,view ----*/
    GsSetProjection(DEFAULT_PROJ); /* Set projection : プロジェクション設定 */
    /* Setting view point location : 視点パラメータ設定 */
    view.vpx = DEFAULT_VPX; view.vpy = DEFAULT_VPY; view.vpz = DEFAULT_VPZ;
  
    /* Setting focus point location : 注視点パラメータ設定 */
    view.vrx = DEFAULT_VRX; view.vry = DEFAULT_VRY; view.vrz = DEFAULT_VRZ;
  
    /* Setting bank of SCREEN : 視点の捻りパラメータ設定 */
    view.rz=DEFAULT_RZ;

    /* Setting parent of viewing coordinate : 視点座標パラメータ設定 */
    view.super = WORLD;
  
    /* Calculate World-Screen Matrix from viewing paramter*/
    /* 視点パラメータを群から視点を設定する*/
    /*ワールドスクリーンマトリックスを計算する */
    GsSetRefView2(&view);
  
    GsSetNearClip(100);           /* Set Near Clip : ニアクリップ設定 */
}

/***********************************************
 *	set lights
 *	光源設定
 ***********************************************/
static void set_lights(short *ambc, GsF_LIGHT *pslt);
static GsF_LIGHT pslt_strong[3];
static GsF_LIGHT pslt_faint[3];
static short ambc_strong[3];
static short ambc_faint[3];

/* initialize lights : 光源初期化 */
void light_init(void)
{
    pslt_strong[0].vx=pslt_faint[0].vx=DEFAULT_0VX;
    pslt_strong[0].vy=pslt_faint[0].vy=DEFAULT_0VY;
    pslt_strong[0].vz=pslt_faint[0].vz=DEFAULT_0VZ;
    pslt_strong[1].vx=pslt_faint[1].vx=DEFAULT_1VX;
    pslt_strong[1].vy=pslt_faint[1].vy=DEFAULT_1VY;
    pslt_strong[1].vz=pslt_faint[1].vz=DEFAULT_1VZ;
    pslt_strong[2].vx=pslt_faint[2].vx=DEFAULT_2VX;
    pslt_strong[2].vy=pslt_faint[2].vy=DEFAULT_2VY;
    pslt_strong[2].vz=pslt_faint[2].vz=DEFAULT_2VZ;

    pslt_strong[0].b=DEFAULT_0B;
    pslt_strong[0].g=DEFAULT_0G;
    pslt_strong[0].r=DEFAULT_0R;
    pslt_strong[1].b=DEFAULT_1B;
    pslt_strong[1].g=DEFAULT_1G;
    pslt_strong[1].r=DEFAULT_1R;
    pslt_strong[2].b=DEFAULT_2B;
    pslt_strong[2].g=DEFAULT_2G;
    pslt_strong[2].r=DEFAULT_2R;

    pslt_faint[0].b=DEFAULT_0B;
    pslt_faint[0].g=DEFAULT_0G;
    pslt_faint[0].r=DEFAULT_0R;
    pslt_faint[1].b=DEFAULT_1B;
    pslt_faint[1].g=DEFAULT_1G;
    pslt_faint[1].r=DEFAULT_1R;
    pslt_faint[2].b=DEFAULT_2B;
    pslt_faint[2].g=DEFAULT_2G;
    pslt_faint[2].r=DEFAULT_2R;

    ambc_strong[0]=DEFAULT_AMBR_S;
    ambc_strong[1]=DEFAULT_AMBG_S;
    ambc_strong[2]=DEFAULT_AMBB_S;

    ambc_faint[0]=DEFAULT_AMBR_F;
    ambc_faint[1]=DEFAULT_AMBG_F;
    ambc_faint[2]=DEFAULT_AMBB_F;

    /* Setting default light mode : 光源計算のデフォルトの方式設定 */
    GsSetLightMode(0);
}

/* set light parameters : 光源設定 */
static void set_lights(short *ambc, GsF_LIGHT *pslt)
{
    int i;

    GsSetAmbient(ambc[0], ambc[1], ambc[2]);
    for (i=0; i<3; i++){
	GsSetFlatLight(i,&pslt[i]);
    }
}

void set_strong_light(void)
{
    set_lights(ambc_strong, pslt_strong);
}


void set_faint_light(void)
{
    set_lights(ambc_faint, pslt_faint);
}

/* $PSLibId: Runtime Library Release 3.6$ */
/***********************************************
 *
 *	"main.c" main routine
 *
 *		Version 1.00	May. 2, 1995
 *
 *	Copyright (C) 1995 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 *
 ***********************************************/
/*	sample program for polygon subdivision 
 :	ポリゴン分割サンプルプログラム
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>


#define OT_LENGTH  10	/* resolution of OT: オーダリングテーブルの解像度 */

#define BGR 0		/* BG color Red */
#define BGG 0		/* BG color Green */
#define BGB 0		/* BG color Blue */

void draw_poly(GsCOORDINATE2 *co, int idx, GsOT_TAG *org);
int obj_interactive(u_long fn, u_long padd);


GsOT            Wot[2];	/* OT handler:  オーダリングテーブルハンドラ */
GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* OT: オーダリングテーブル実体 */

POLY_FT4 ft4[2];		/* divided polygon */
SVECTOR fpos[4];		/* polygon vertex positions */
CVECTOR fcol;			/* polygon color */

GsCOORDINATE2   DWorld;		/* object coordinate: オブジェクトの座標系 */
SVECTOR PWorld;			/* rotation vector:
				   座標系を作るためのローテーションベクター */
extern MATRIX GsIDMATRIX;	/* Unit Matrix : 単位行列 */

struct {
    char u;
    char v;
    short cd;
} uv0, uv1, uv2, uv3;		/* texture parameters for divide */

/*int GsTON;*/
DIVPOLYGON4 divp;		/* divide buffer */
LINE_F2 indicator[4];		/* picture area indicator line */

/************* MAIN START ******************************************/
main()
{
    u_long fn;
    int  outbuf_idx=0;
    u_long padd=0;		
    static int divnum[]={1, 2, 4, 8, 16, 32};	/* divde depth=>number */

    ResetCallback();
    PadInit(0);			/* initialize controller: コントローラ初期化 */

    init_all();

    /* main loop */
    for (fn=0;;fn++){
	outbuf_idx=GsGetActiveBuff();		/* get one of buffers */

	if (obj_interactive(fn, padd)<0) return; /* pad data handling */

	FntPrint("divp.ndiv=%d (%2dx%2d)         : START + direction\n",
		divp.ndiv, divnum[divp.ndiv], divnum[divp.ndiv]);
	FntPrint("picture area (w,h)=(%3d,%3d) : SELECT + direction\n",
		divp.pih, divp.piv);
	FntPrint("exit from this sample        : SELECT + L2 + R2\n");


	GsClearOt(0,0,&Wot[outbuf_idx]);	/* clear ordering table */

	/*draw polygon(s) & indicator*/
	draw_poly(&DWorld, outbuf_idx, Wot[outbuf_idx].org);

	padd=PadRead(1);			/* read pad */

	DrawSync(0);				/* wait drawing done */
	VSync(0);				/* wait Vertical Sync */

	GsSwapDispBuff();			/* switch double buffers */
	
	/* BG auto clear enable:
	   画面のクリアをオーダリングテーブルの最初に登録する */
	GsSortClear(BGR,BGG,BGB,&Wot[outbuf_idx]); 

	/* draw primitives on OT:
	   オーダリングテーブルに登録されているパケットの描画を開始する */
	GsDrawOt(&Wot[outbuf_idx]);
	FntFlush(-1);				/* draw print-stream */
    }
}

/* draw polygon(s) */
void draw_poly(GsCOORDINATE2 *co, int idx, GsOT_TAG *org)
{
    int i;
    static POLY_FT4 fbuf[2][1024];		/* draw buffer for divide */
    long otz, p, flag;
    MATRIX tmpls;
    POLY_FT4 *f;
    long nclip;

    /* update coordinate and set to GTE: 座標を計算して GTE にセット */
    GsGetLs(co, &tmpls);
    GsSetLightMatrix(&tmpls);
    GsSetLsMatrix(&tmpls);

    /* set indicator position */
    setXY2(&indicator[0], -divp.pih/2, -120, -divp.pih/2, 120);
    setXY2(&indicator[1], divp.pih/2, -120, divp.pih/2, 120);
    setXY2(&indicator[2], -320, -divp.piv/2, 320, -divp.piv/2);
    setXY2(&indicator[3], -320, divp.piv/2, 320, divp.piv/2);
    for (i=0; i<4; i++){
	AddPrim(org+20,&indicator[i]);	/* draw indicator */
    }

    f= &ft4[idx];
    nclip=RotAverageNclip4(&fpos[0], &fpos[1], &fpos[2], &fpos[3],
			   (long *)(&f->x0),
			   (long *)(&f->x1),
			   (long *)(&f->x2),
			   (long *)(&f->x3),
			   &p, &otz, &flag
			   );

    if (divp.ndiv==0){				/* without using division */
	if (flag >=0				/* error OK */
	    && nclip>=0				/* normal clip OK */
	    && otz>0)				/* otz OK */
	    AddPrim(org+(otz>>(14-OT_LENGTH)), f); /* draw */
    } else{					/* using division */
	f= (POLY_FT4 *)DivideFT4(&fpos[0], &fpos[1], &fpos[2], &fpos[3],
				 (u_long *)&(uv0.u),
				 (u_long *)&(uv1.u),
				 (u_long *)&(uv2.u),
				 (u_long *)&(uv3.u),
				 &fcol,
				 &fbuf[idx][0],
				 (u_long *)(org+(otz>>(14-OT_LENGTH))),
				 &divp);
    }
}

/* pad data handling */
int obj_interactive(u_long fn, u_long padd)
{
#define DT 30
#define VT 30
#define ZT 30
    static u_long oldpadd=0;

    if (padd & PADk){			/* change PICTURE AREA with PADk */
	if (padd & PADLleft) divp.pih++;
	if (padd & PADLright) divp.pih--;
	if (padd & PADLdown) divp.piv--;
	if (padd & PADLup) divp.piv++;
	if (divp.pih<0) divp.pih=0;
	if (divp.pih>640) divp.pih=640;
	if (divp.piv<0) divp.piv=0;
	if (divp.piv>240) divp.piv=240;
    } else if (padd & PADh){		/* change DIVIDE DEPTH with PADh */
	if (padd & PADLup & ~oldpadd) divp.ndiv++;
	if (padd & PADLright & ~oldpadd) divp.ndiv++;
	if (divp.ndiv<1){
	    if (padd & PADLdown & ~oldpadd) divp.ndiv=5;
	    if (padd & PADLleft & ~oldpadd) divp.ndiv=5;
	} else if (divp.ndiv>5){
	    divp.ndiv=0;
	} else{
	    if (padd & PADLdown & ~oldpadd) divp.ndiv--;
	    if (padd & PADLleft & ~oldpadd) divp.ndiv--;
	}
    } else{				/* translate and rotate */
	if (padd & PADLleft) DWorld.coord.t[0]+=DT;
	else if (padd & PADLright) DWorld.coord.t[0]-=DT;
	
	if (padd & PADLup) DWorld.coord.t[1]-=DT;
	else if (padd & PADLdown) DWorld.coord.t[1]+=DT;
	
	if (padd & PADo) DWorld.coord.t[2]-=ZT;
	else if (padd & PADn) DWorld.coord.t[2]+=ZT;
	
	if (padd & PADRleft) PWorld.vy-=VT;
	else if (padd & PADRright) PWorld.vy+=VT;
	
	if (padd & PADRup) PWorld.vx+=VT;
	else if (padd & PADRdown) PWorld.vx-=VT;
	
	if (padd & PADm) PWorld.vz-=VT;
	else if (padd & PADl) PWorld.vz+=VT;
    }


    if ((padd & (PADm|PADo|PADk))==(PADm|PADo|PADk)){
	/* quit; プログラム終了 */
	PadStop();
	ResetGraph(3);
	StopCallback();
	return -1;
    }

    set_coordinate(&PWorld,&DWorld);
    oldpadd=padd;
    return 0;
}

init_all()			/* initialize: 初期化ルーチン群 */
{
    ResetGraph(0);		/* initialize GPU: GPUリセット */

    draw_init();
    coord_init();
    view_init();
    fnt_init();
    poly_init();
    div_init();
    texture_init((u_long *)0x80100000);
}

poly_init()
{
    /* divided polygon */
    setPolyFT4(&ft4[0]);	/* initialize primitive */
    fcol.r=128;			/* set color */
    fcol.g=128;
    fcol.b=128;
    fcol.cd=ft4[0].code;
    
    /* set polygon color */
    setRGB0(&ft4[0], fcol.r, fcol.g, fcol.b);	
    
    /* set position */
    setXY4(&ft4[0], -100, -100, 100, -100, -100, 100, 100, 100); 
    
    /* set texture position */
    setUV4(&ft4[0], 0,0,255,0, 0, 255, 255, 255); 
    
    ft4[0].clut=GetClut(0, 480);		/* clut */
    ft4[0].tpage=GetTPage(1,0,640,0);		/* texture page */
    ft4[1]=ft4[0];				/* copy data to the other ft4*/

    /* picture area indicator */
    SetLineF2(&indicator[0]);			/* initialize primitive */
    setRGB0(&indicator[0], 255,255,255);	/* set indicator color */
    indicator[3]=indicator[2]=indicator[1]=indicator[0]; /* copy data */
    
    /* set position */
    setXY2(&indicator[0], -divp.pih/2, -120, -divp.pih/2, 120);
    setXY2(&indicator[1], divp.pih/2, -120, divp.pih/2, 120);
    setXY2(&indicator[2], -320, -divp.piv/2, 320, -divp.piv/2);
    setXY2(&indicator[3], -320, divp.piv/2, 320, divp.piv/2);
}

/* initialize for divide */
div_init()
{
#define SIDELONG 8000
    /* set vertex positions */
    fpos[0].vx= SIDELONG;
    fpos[0].vy= -SIDELONG;
    fpos[0].vz= 0;
    fpos[1].vx= -SIDELONG;
    fpos[1].vy= -SIDELONG;
    fpos[1].vz= 0;
    fpos[2].vx= SIDELONG;
    fpos[2].vy= SIDELONG;
    fpos[2].vz= 0;
    fpos[3].vx= -SIDELONG;
    fpos[3].vy= SIDELONG;
    fpos[3].vz= 0;

    /* set texture parameters */
    uv0.u=0;
    uv0.v=0;
    uv0.cd=GetClut(0, 480);
    uv1.u=255;
    uv1.v=0;
    uv1.cd=GetTPage(1,0,640,0);
    uv2.u=0;
    uv2.v=255;
    uv3.u=255;
    uv3.v=255;

    /* set divide parameters */
    divp.ndiv=5;			/* divide depth */
    divp.pih=540;			/* horizontal resolution */
    divp.piv=170;			/* vertical resolution */
}

draw_init()
{
    /* set resolution (interlace mode): 解像度設定（インターレースモード） */
    GsInitGraph(640,240,GsOFSGPU|GsINTER,1,0);	
    
    /* initialize double buffer: ダブルバッファ指定 */
    GsDefDispBuff(0,0,0,240);	
    
    /* reset 3D system: ３Dシステム初期化 */
    GsInit3D();			

    /* set OT resolution: オーダリングテーブルハンドラに解像度設定 */
    Wot[0].length=OT_LENGTH;	
    Wot[1].length=OT_LENGTH;
    
    /* set OT on OT handler(0): ＯＴハンドラにＯＴの実体設定 */
    Wot[0].org=zsorttable[0];	
    Wot[1].org=zsorttable[1];
}

fnt_init()
{
    FntLoad(960, 256);				/* font load */
    SetDumpFnt(FntOpen(-290,-100,400,100,0,200)); /* stream open & define */
}

int view_init()
{
    GsRVIEW2  view;		/* View Point Handler*/

    /* Set projection : プロジェクション設定 */
    GsSetProjection(300);	
  
    /* Setting view point location : 視点パラメータ設定 */
    view.vpx = 0; view.vpy = 0; view.vpz = 2000;
  
    /* Setting focus point location : 注視点パラメータ設定 */
    view.vrx = 0; view.vry = 0; view.vrz = 0;
  
    /* Setting bank of SCREEN : 視点の捻りパラメータ設定 */
    view.rz=0;

    /* Setting parent of viewing coordinate : 視点座標パラメータ設定 */
    view.super = WORLD;
  
    /* Calculate World-Screen Matrix from viewing paramter
       : 視点パラメータを群から視点を設定する*/
    GsSetRefView2(&view);
  
    /* Set Near Clip: ニアクリップ設定 */
    GsSetNearClip(100);           
}


int coord_init()
{
    /* initialize coordinates: 座標の定義 */
    GsInitCoordinate2(WORLD,&DWorld);
    DWorld.coord.t[2]= -10000;

    /* set the rotation vector for rot-trans matrix:
       マトリックス計算ワークのローテーションベクター初期化 */
    PWorld.vx=PWorld.vy=PWorld.vz=0;
    PWorld.vx=896;
    set_coordinate(&PWorld,&DWorld);
}

/* make rot-trans matrix from the rotation vector:
   ローテションベクタからマトリックスを作成し座標系にセットする */
int set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor)
{
    MATRIX tmp1;
    SVECTOR v1;

    /* start from th unit matrix: 単位行列から出発する */
    tmp1   = GsIDMATRIX;	
    
    /* set translation vectro: 平行移動をセットする */
    tmp1.t[0] = coor->coord.t[0];
    tmp1.t[1] = coor->coord.t[1];
    tmp1.t[2] = coor->coord.t[2];

    /* make rotaion matrix and hook to object handler
     : マトリックスにローテーションベクタを作用させる 
     * 求めたマトリックスを座標系にセットする
     */
    v1 = *pos;
    RotMatrix(&v1,&tmp1);
    coor->coord = tmp1;
    
    /* flush matrix cache
     : マトリックスキャッシュをフラッシュする
     */
    coor->flg = 0;
}


texture_init(u_long *addr)
{
    RECT rect1;
    GsIMAGE tim1;

    /* Get texture information of TIM FORMAT/
       : TIMデータのヘッダからテクスチャのデータタイプの情報を得る */  
    GsGetTimInfo(addr+1,&tim1);
  
    rect1.x=tim1.px;		/* X point of image data on VRAM
				   : テクスチャ左上のVRAMでのX座標 */
    rect1.y=tim1.py;		/* Y point of image data on VRAM
				   : テクスチャ左上のVRAMでのY座標 */
    rect1.w=tim1.pw;		/* Width of image : テクスチャ幅 */
    rect1.h=tim1.ph;		/* Height of image : テクスチャ高さ */
  
    /* Load texture to VRAM : VRAMにテクスチャをロードする */
    LoadImage(&rect1,tim1.pixel);
  
    /* Exist Color Lookup Table : カラールックアップテーブルが存在する */  
    if ((tim1.pmode>>3)&0x01){
	rect1.x=tim1.cx;		/* X point of CLUT data on VRAM
					   : クラット左上のVRAMでのX座標 */
	rect1.y=tim1.cy;		/* Y point of CLUT data on VRAM
					   : クラット左上のVRAMでのY座標 */
	rect1.w=tim1.cw;		/* Width of CLUT : クラットの幅 */
	rect1.h=tim1.ch;		/* Height of CLUT : クラットの高さ */

        /* Load CULT data to VRAM : VRAMにクラットをロードする */
	LoadImage(&rect1,tim1.clut);
    }
}

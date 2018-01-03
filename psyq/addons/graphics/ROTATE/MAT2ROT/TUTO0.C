/* $PSLibId: Runtime Library Release 3.6$ */
/***********************************************
 *	
 *	Inverse RotMatrix function sample program
 *
 *	"tuto0.c" main routine
 *
 *		Version 1.00
 *
 *	Copyright (C) 1995,1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>


#define OT_LENGTH  1	/* resolution of Wot : オーダリングテーブルの解像度 */
#define PROJECTION 1000

#define BGR 100					/* BG color Red */
#define BGG 100					/* BG color Green */
#define BGB 100					/* BG color Blue */
void draw_poly(MATRIX *m, POLY_FT4 *f, GsOT_TAG *org, int offset_x);
int obj_interactive(u_long fn, u_long padd);
void main();
short get_angle(DVECTOR *d0);



GsOT            Wot[2];	/* OT handler : オーダリングテーブルハンドラ */
GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* substance of OT : OT実体 */


POLY_FT4 ft4_org[2];	/* polygon for original (left) */
POLY_FT4 ft4_trans[2];	/* polygon for reconstructed angles (right) */
SVECTOR fpos[4];	/* polygon vertex positions */
CVECTOR fcol;		/* polygon color */

GsCOORDINATE2   DWorld;	/* Coordinate for original (left) :オリジナル座標系 */
GsCOORDINATE2   Dnew;	/* Coordinate for reconstructed (right): 再構成座標系 */
SVECTOR PWorld;		/* work short vector for making Coordinate parameter : */
			/* 座標系を作るためのローテーションベクター */

int  outbuf_idx=0;

#define ORGOFFSETX -160
#define TRANSOFFSETX 160

LINE_F2 lf2[2];		/* vertical center line */

/************* MAIN START ******************************************/
void main()
{
    u_long fn;
    u_long padd=0;	/* to keep pad data:コントローラのデータを保持する */
    int i,j;
    MATRIX mat0, mat1;
    SVECTOR v;


    ResetCallback();
    PadInit(0);		/* initilize pad controller : コントローラ初期化 */
    init_all();

    /* main loop */
    for (fn=0;;fn++){
	outbuf_idx=GsGetActiveBuff();		/* get one of buffers */

	if (obj_interactive(fn, padd)<0) break; /* pad data handling */


	GsClearOt(0,0,&Wot[outbuf_idx]);	/* clear ordering table */

	GsGetLs(&DWorld, &mat0);

	/* draw original polygon */
	draw_poly(&mat0, &ft4_org[outbuf_idx], Wot[outbuf_idx].org, ORGOFFSETX);

	/* calculate back to Euler's angles from rotation matrix */
	InvRotMatrixZYX(&DWorld.coord, &v);

	/* recreate rotation matrix */
	set_coordinate(&v, &Dnew);

	Dnew.coord.t[0]=DWorld.coord.t[0];
	Dnew.coord.t[1]=DWorld.coord.t[1];
	Dnew.coord.t[2]=DWorld.coord.t[2];
	GsGetLs(&Dnew, &mat0);

	/* draw new polygon */
	draw_poly(&mat0, &ft4_trans[outbuf_idx], Wot[outbuf_idx].org, TRANSOFFSETX);

	FntPrint("<LEFT>  original Euler's angles=      <%4d,%4d,%4d>\n",
		(20480+PWorld.vx)&0xfff,(20480+PWorld.vy)&0xfff,(20480+PWorld.vz)&0xfff);
	FntPrint("<RIGHT> reconstructed Euler's angles= <%4d,%4d,%4d>\n",
		 (20480+v.vx)&0xfff,(20480+v.vy)&0xfff,(20480+v.vz)&0xfff);

	/* draw vertical center line */
	AddPrim(Wot[outbuf_idx].org, &lf2[outbuf_idx]);

	padd=PadRead(1);			/* read pad data */

	DrawSync(0);				/* wait drawing done */

	VSync(0);				/* wait Vertical Sync */

	GsSwapDispBuff();			/* switch double buffers */

	/* Set SCREEN CLESR PACKET to top of OT : */
	/*  画面のクリアをオーダリングテーブルの最初に登録する */
	GsSortClear(BGR,BGG,BGB,&Wot[outbuf_idx]); 


	/* Start Drawing : */
	/* オーダリングテーブルに登録されているパケットの描画を開始する */
	GsDrawOt(&Wot[outbuf_idx]);
	FntFlush(-1);				/* draw print-stream */
    }
    return;
}

/* draw polygon(s) */
void draw_poly(MATRIX *m, POLY_FT4 *f, GsOT_TAG *org, int offset_x)
{
    long otz, flag, p;

    /* set the LS matrix to gte : GTEに LS matrix をセット */
    GsSetLsMatrix(m);

    otz=RotAverage4(&fpos[0], &fpos[1], &fpos[2], &fpos[3],
			   (long *)(&f->x0),
			   (long *)(&f->x1),
			   (long *)(&f->x2),
			   (long *)(&f->x3),
			   &p, &flag
			   );
    if (flag >=0			/* error OK */
	&& otz>0){			/* otz OK */
	f->x0+=offset_x;
	f->x1+=offset_x;
	f->x2+=offset_x;
	f->x3+=offset_x;
	AddPrim(org+(otz>>(14-OT_LENGTH)), f); /* draw */
    }
}


/* pad data handling */
int obj_interactive(u_long fn, u_long padd)
{
#define DT 30
#define VT 37
#define ZT 30
    static u_long oldpadd=0;

    if (padd & PADLleft) PWorld.vy-=VT;
    else if (padd & PADLright) PWorld.vy+=VT;
    
    if (padd & PADLup) PWorld.vx+=VT;
    else if (padd & PADLdown) PWorld.vx-=VT;
    
    if (padd & PADL1) PWorld.vz-=VT;
    else if (padd & PADL2) PWorld.vz+=VT;

    if ((padd & (PADm|PADo|PADk))==(PADm|PADo|PADk)){
	/* finish : プログラム終了 */
	PadStop();
	ResetGraph(3);
	StopCallback();
	return -1;
    }

    set_coordinate(&PWorld,&DWorld);
    oldpadd=padd;
    return 0;
}

init_all()			/* initialize routines : 初期化ルーチン群 */
{
    ResetGraph(0);		/* reset GPU : GPUリセット */

    draw_init();
    coord_init();
    view_init();
    fnt_init();
    poly_init();
    texture_init((u_long *)0x80100000);
}

poly_init()
{
    /* original (left) polygon : 回転させるポリゴン */
    setPolyFT4(&ft4_org[0]);			/* initialize primitive */
    SetShadeTex(&ft4_org[0],1);			/* shading off */
    fcol.r=128;					/* set color */
    fcol.g=128;
    fcol.b=128;
    fcol.cd=ft4_org[0].code;
    setRGB0(&ft4_org[0], fcol.r, fcol.g, fcol.b); /* set polygon color */

						/* set position */
    setXY4(&ft4_org[0], -100, -100, 100, -100, -100, 100, 100, 100);
    setUV4(&ft4_org[0], 0,0,255,0, 0, 255, 255, 255); /* set texture position */
    ft4_org[0].clut=GetClut(0, 480);		/* clut */
    ft4_org[0].tpage=GetTPage(1,0,640,0);	/* texture page */
    ft4_org[1]=ft4_org[0];		/* copy data to the other ft4_org*/


 /* copy data to reconstructed (right) polygon */
    ft4_trans[1]= ft4_trans[0]= ft4_org[0];

#define SIDELENGTH 150
    /* set vertex positions */
    fpos[0].vx= -SIDELENGTH;
    fpos[0].vy= -SIDELENGTH;
    fpos[0].vz= 0;
    fpos[1].vx= SIDELENGTH;
    fpos[1].vy= -SIDELENGTH;
    fpos[1].vz= 0;
    fpos[2].vx= -SIDELENGTH;
    fpos[2].vy= SIDELENGTH;
    fpos[2].vz= 0;
    fpos[3].vx= SIDELENGTH;
    fpos[3].vy= SIDELENGTH;
    fpos[3].vz= 0;


    /* vertical center line: 中央の縦の線 */
    setLineF2(&lf2[0]);
    setRGB0(&lf2[0], 255,255,255);
    setXY2(&lf2[0], 0, -80, 0, 120);
    lf2[1]=lf2[0];
}


draw_init()
{
    /* set the resolution of screen (on interrace mode) :*/
    /* 解像度設定（ノンインターレースモード） */
    GsInitGraph(640,240,GsNONINTER|GsOFSGPU,1,0);
    GsDefDispBuff(0,0,0,240); /* set the double buffers : ダブルバッファ指定 */
    GsInit3D();		/* init 3d part of libgs : ３Dシステム初期化 */
  
    Wot[0].length=OT_LENGTH; /* set the length of OT1 : OT１に解像度設定 */
    Wot[0].org=zsorttable[0]; /* set the top address of OT1 tags : */
			/* OT１にオーダリングテーブルの実体設定 */
    /* set anoter OT for double buffer : ダブルバッファのためもう一方にも同じ設定 */
    Wot[1].length=OT_LENGTH;
    Wot[1].org=zsorttable[1];
}

fnt_init()
{
    FntLoad(960, 256);				/* font load */
    SetDumpFnt(FntOpen(-290,-100,580,100,0,255)); /* stream open & define */
}

int view_init()
{
    GsRVIEW2  view;		/* View Point Handler*/

    /*---- Set projection,view ----*/
    GsSetProjection(PROJECTION); /* Set projection : プロジェクション設定 */
 
    /* Setting view point location : 視点パラメータ設定 */
    view.vpx = 0; view.vpy = 0; view.vpz = -2000;
 
    /* Setting focus point location : 注視点パラメータ設定 */
    view.vrx = 0; view.vry = 0; view.vrz = 0;
 
    /* Setting bank of SCREEN : 視点の捻りパラメータ設定 */
    view.rz=0;

    /* Setting parent of viewing coordinate : 視点座標パラメータ設定 */
    view.super = WORLD;
 
    /* Calculate World-Screen Matrix from viewing paramter*/
    /* 視点パラメータを群から視点を設定する*/
    /*ワールドスクリーンマトリックスを計算する */
    GsSetRefView2(&view);
 
    GsSetNearClip(100);           /* Set Near Clip : ニアクリップ設定 */
}


int coord_init()
{
    /* 座標の定義 */
    GsInitCoordinate2(WORLD,&DWorld);
    DWorld.coord.t[2]= 0;
    GsInitCoordinate2(WORLD,&Dnew);
    Dnew.coord.t[2]= 0;

    /* マトリックス計算ワークのローテーションベクター初期化 */
    PWorld.vx=PWorld.vy=PWorld.vz=0;

    set_coordinate(&PWorld,&DWorld);
}

/* Set coordinte parameter from work vector :*/
/*   ローテションベクタからマトリックスを作成し座標系にセットする */
int set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor)
{
  MATRIX tmp1;
  
  /* Set translation : 平行移動をセットする */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  
  /* Rotate Matrix : ローテーションベクタから回転マトリックスを作成する */
  RotMatrix(pos,&tmp1);
  
  /* Set Matrix to Coordinate : 求めたマトリックスを座標系にセットする */
  coor->coord = tmp1;

  /* Clear flag becase of changing parameter : マトリックスキャッシュをフラッシュする */
  coor->flg = 0;
}


texture_init(u_long *addr)
{
    int i;
    GsIMAGE tim1;
    unsigned short *clut;
    RECT rect1;

    /* Get texture information of TIM FORMAT*/
       /* TIMデータのヘッダからテクスチャのデータタイプの情報を得る */  
    GsGetTimInfo(addr+1,&tim1);
 
    rect1.x=tim1.px;		/* X point of image data on VRAM*/
				   /* テクスチャ左上のVRAMでのX座標 */
    rect1.y=tim1.py;		/* Y point of image data on VRAM*/
				   /* テクスチャ左上のVRAMでのY座標 */
    rect1.w=tim1.pw;		/* Width of image /* テクスチャ幅 */
    rect1.h=tim1.ph;		/* Height of image /* テクスチャ高さ */
 
    /* Load texture to VRAM /* VRAMにテクスチャをロードする */
    LoadImage(&rect1,tim1.pixel);
 
    /* Exist Color Lookup Table /* カラールックアップテーブルが存在する */  
    if ((tim1.pmode>>3)&0x01){
	rect1.x=tim1.cx;		/* X point of CLUT data on VRAM*/
					   /* クラット左上のVRAMでのX座標 */
	rect1.y=tim1.cy;		/* Y point of CLUT data on VRAM*/
					   /* クラット左上のVRAMでのY座標 */
	rect1.w=tim1.cw;		/* Width of CLUT : クラットの幅 */
	rect1.h=tim1.ch;		/* Height of CLUT : クラットの高さ */

	    /* Load CULT data to VRAM /* VRAMにクラットをロードする */
	LoadImage(&rect1,tim1.clut);
    }
}

/***********************************************
 *	rotatino matrix => Euler's angles
 ***********************************************/
#define VALUEONE (4096)

InvRotMatrixZYX(MATRIX *rotm, SVECTOR *vector)
{
    SVECTOR sv0, sv1;
    DVECTOR dv0;	
    MATRIX m;
    short c;

    /* ignore translation : 平行移動をしない回転だけのマトリクスにする */
    m= *rotm;
    m.t[0]=m.t[1]=m.t[2]=0;

    /* rotate point A (={0,0,1}) : 基準点 A (={0,0,1})を回転させてみる */
    sv0.vx=0; sv0.vy=0; sv0.vz=VALUEONE;
    ApplyMatrixSV(&m, &sv0, &sv1);
    
    /* determine angle around X-axis (rotate on XZ-plane) : */
    /* X 軸回転分を決める (XZ平面上まで回転させる) */
    vector->vx= -ratan2(sv1.vy, sv1.vz);

    /* determine angle around Y-axis (rotate on Z-axis) : */
    /* Y 軸回転分を決める (Z 軸上まで回転させる) */
    c=SquareRoot12(((sv1.vz*(long)sv1.vz) + (sv1.vy*(long)sv1.vy))>>12);

    vector->vy= ratan2(sv1.vx, c);

    /* rotate point B (={0,1,0}) : 基準点B (={0,1,0})を回転させてみる */
    sv0.vx=0; sv0.vy=VALUEONE; sv0.vz=0;
    ApplyMatrixSV(&m, &sv0, &sv1);

    /* rotate B back for angles around X- and Y-axes : */
    /* B を X, Y 軸回転分だけ戻す*/
    sv0.vx= -vector->vx; sv0.vy= -vector->vy; sv0.vz= 0;
    RotMatrixZYX(&sv0, &m);
    ApplyMatrixSV(&m, &sv1, &sv0);

    /* get angle around Z-axis : Z 軸回転分を計算 */
    vector->vz= -ratan2(sv0.vx, sv0.vy);
}

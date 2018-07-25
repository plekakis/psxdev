/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *	mime (from tmdview5: GsDOBJ5 object viewing rotine)
 *
 *	"tuto0.c" ******** simple GsDOBJ5 Viewing routine
 *
 *		Version 1.00	Jul,  14, 1994
 *
 *		Copyright (C) 1993,1994,1995 by Sony Computer Entertainment
 *		All rights Reserved
 */

#include <sys/types.h>

#include <libetc.h>		/* Must be included when using control pad :
				   PADを使うためにインクルードする必要あり */
#include <libgte.h>		/* LIBGS uses libgte (Must be included when using LIBGS
				/* Must be included when using libgs :
				   LIGSを使うためにインクルードする必要あり*/
#include <libgpu.h>		/* LIBGS uses libgpu (Must be included when using LIBGS
				/*L Must be included when using libgs :
				  IGSを使うためにインクルードする必要あり */
#include <libgs.h>		/* Structures, etc. are defined for using
				   graphics library (LIBGS) :
				   グラフィックライブラリ を使うための
				   構造体などが定義されている */

#include "addr.h"
#include "nmime.h"  
#include "control.h"  

int obj_interactive();

#define OBJECTMAX 100		/* Define the maximum number of logical objects
				   into which a 3D model can be devided :
				   ３Dのモデルは論理的なオブジェクトに
                                   分けられるこの最大数 を定義する */
   
#define OT_LENGTH  12		/* Resolution of OT :
				   オーダリングテーブルの解像度 */


GsOT            Wot[2];		/* Two handlers are required for OT double
				   buffer :
				   オーダリングテーブルハンドラ
				   ダブルバッファのため２つ必要 */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Ordering table (OT) entity :
						オーダリングテーブル実体 */

GsDOBJ5		object[OBJECTMAX]; /* Requres as many object handlers as
				      there are objects :
				      オブジェクトハンドラ
				      オブジェクトの数だけ必要 */

u_long          Objnum;		/* Retain number of modeling data objects :
				   モデリングデータのオブジェクトの数を
				   保持する */

GsCOORDINATE2   DWorld;  /* Coordinate system for each object :
			    オブジェクトごとの座標系 */

SVECTOR         PWorld; /* work short vector for making Coordinate parmeter :
			   座標系を作るためのローテーションベクター */

extern MATRIX GsIDMATRIX;	/* Unit Matrix : 単位行列 */

GsRVIEW2  view;			/* View Point Handler :
				   視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler :
				   平行光源を設定するための構造体 */
u_long padd;			/* Controler data :
				   コントローラのデータを保持する */

/* Work area for making packet data double size is required for double buffer:
   パケットデータを作成するためのワークダブルバッファのため2倍必要 */
/* u_long PacketArea[0x10000]; */


u_long *PacketArea;

/* Transfer Function for smooth control */

extern int cntrlarry[CTLMAX][CTLTIME]; 
extern CTLFUNC ctlfc[CTLMAX];

/* Data table for Transfer Function */

static int cnv0[16] = {
	  39,  79, 118, 197, 315, 394, 433, 473,
	 473, 433, 394, 315, 197, 118,  79,  39
};

static int cnv1[24] = {
	 200, 325, 450, 475, 500, 475, 450, 375,
	 300, 250, 200, 150, 100,  80,  55,   0,
	 -50, -75,-100, -70, -40, -25, -10,  -5
};

static int cnv2[32] = {
	  20,  30,  40,  50,  59,  79,  98, 128,
	 157, 177, 197, 206, 216, 226, 236, 238,
	 236, 226, 216, 206, 197, 177, 157, 128,
	  98,  79,  59,  50,  40,  30,  20,  10
};

u_long *SCRATCH  = (u_long *)0x1f800000;

/************* MAIN START ******************************************/
main()
{
  u_long  fn;			/* frame (field) No. */
  int     i;
  GsDOBJ5 *op;			/* pointer of Object Handler :
				   オブジェクトハンドラへのポインタ */
  int     outbuf_idx;		/* double buffer index */
  MATRIX  tmpls;
  
  ResetCallback();
  PadInit(0);
  PacketArea = (u_long *)PACKETBUF;
  init_all();
  /*== initialize Transfer Function  ====*/

  init_cntrlarry(cnv1,24);
  /* initialize MIMe function */
  
  init_mime_data(0, MODEL_ADDR, MDFDATAVTX, MDFDATANRM, ORGVTXBUF, ORGNRMBUF);

  /* Initialize object position */
  DWorld.coord.t[2] = -30000; 
  PWorld.vy = ONE/2;
  PWorld.vx = -ONE/12;

  for(fn=0;;fn++)
    {
      	if (obj_interactive()<0) return;/* set motion data by pad data */

		set_cntrl(fn);	/* generate control wave by transfer function */

		PWorld.vy += ctlfc[4].out;  /* Rotate around the Y axis */
		PWorld.vx += ctlfc[5].out;  /* Rotate around the X axis */
		DWorld.coord.t[2] += ctlfc[8].out; /* Move along the Z axis */
		DWorld.coord.t[0] += ctlfc[6].out; /* Move along the X axis */
		DWorld.coord.t[1] += ctlfc[7].out; /* Move along the Y axis */

		/*==MIME Function=======*/
		mimepr[0][0] = ctlfc[0].out; /* MIMe Weight (Control) value 0 */
		mimepr[0][1] = ctlfc[1].out; /* MIMe Weight (Control) value 1 */
		mimepr[0][2] = ctlfc[2].out; /* MIMe Weight (Control) value 2 */
		mimepr[0][3] = ctlfc[3].out; /* MIMe Weight (Control) value 3 */
		vertex_mime(0);			/* MIMe operation (Vertex) */
		normal_mime(0);			/* MIMe operation (Normal) */

      GsSetRefView2(&view);	/* Calculate World-Screen Matrix :
				   ワールドスクリーンマトリックス計算 */
      outbuf_idx=GsGetActiveBuff();/* Get double buffer index :
				      ダブルバッファのどちらかを得る */

      GsClearOt(0,0,&Wot[outbuf_idx]); /* Clear OT for using buffer :
					  オーダリングテーブルをクリアする */

      for(i=0,op=object;i<Objnum;i++)
	{
	  /* Calculate Local-World Matrix :
	     ワールド／ローカルマトリックスを計算する */
	  GsGetLw(op->coord2,&tmpls);
	  
	  /* Set LWMATRIX to GTE Lighting Registers :
	     ライトマトリックスをGTEにセットする */
	  GsSetLightMatrix(&tmpls);
	  
	  /* Calculate Local-Screen Matrix :
	     スクリーン／ローカルマトリックスを計算する */
	  GsGetLs(op->coord2,&tmpls);

	  /* Set LSAMTRIX to GTE Registers :
	     スクリーン／ローカルマトリックスをGTEにセットする */
	  GsSetLsMatrix(&tmpls);
	  
	  /* Perspective Translate Object and Set OT :
	     オブジェクトを透視変換しオーダリングテーブルに登録する */
	  GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,SCRATCH);
	  op++;
	}
	
      VSync(0);			/* Wait VSYNC : Vブランクを待つ */
      padd=PadRead(1);		/* Readint Control Pad data :
				   パッドのデータを読み込む */
      GsSwapDispBuff();		/* Swap double buffer :
				   ダブルバッファを切替える */
      
      /* Set SCREEN CLESR PACKET to top of OT :
	 画面のクリアをオーダリングテーブルの最初に登録する */
      GsSortClear(0x0,0x0,0x0,&Wot[outbuf_idx]);
      
      /* Drawing OT :
	 オーダリングテーブルに登録されているパケットの描画を開始する */
      GsDrawOt(&Wot[outbuf_idx]);
    }
}


int obj_interactive()
{
  SVECTOR v1;
  MATRIX  tmp1;

/*===========================================================================*/ 
		/* set control data0 for MIMe value0 */
		if((padd & PADl)>0) ctlfc[0].in = ONE; else ctlfc[0].in = 0;
		/* set control data1 for MIMe value1 */
		if((padd & PADm)>0) ctlfc[1].in = ONE; else ctlfc[1].in = 0;
		/* set control data2 for MIMe value2 */
		if((padd & PADn)>0) ctlfc[2].in = ONE; else ctlfc[2].in = 0;
		/* set control data3 for MIMe value3 */
		if((padd & PADo)>0) ctlfc[3].in = ONE; else ctlfc[3].in = 0;
  		/* set control data4 for rotation of Y axis */
		if((padd & PADRleft)>0) 	 ctlfc[4].in =  ONE/64;
		else {  if((padd & PADRright)>0) ctlfc[4].in = -ONE/64; 
			else 			 ctlfc[4].in =      0; 
		}
  		/* set control data5 for rotation of X axis */
		if((padd & PADRup)>0) 		 ctlfc[5].in =  ONE/64;
		else { 	if((padd & PADRdown)>0)  ctlfc[5].in = -ONE/64;
			else			 ctlfc[5].in =      0; 
		}
  		/* set control data8 for motion along Z axis */
		if((padd & PADh)>0)		 ctlfc[8].in = -240;
		else {  if((padd & PADk)>0)	 ctlfc[8].in =  240; 
			else			 ctlfc[8].in =   0; 
		}
  		/* set control data6 for motion along X axis */
		if((padd & PADLleft)>0) 	 ctlfc[6].in =  180;
		else {  if((padd & PADLright)>0) ctlfc[6].in = -180; 
			else			 ctlfc[6].in =   0; 
		}
  		/* set control data7 for motion along Y axis */
		if((padd & PADLup)>0)		 ctlfc[7].in = -180;
		else {  if((padd & PADLdown)>0)	 ctlfc[7].in =  180;
			else			 ctlfc[7].in =   0; 
		}
/*===========================================================================*/ 
  if (((padd & PADk)>0)&&((padd & PADo)>0)&&((padd & PADm)>0)){
      PadStop();
      ResetGraph(3);
      StopCallback();
      return -1;
  }
 
  /* set matrix in the coordinate system  */
  set_coordinate(&PWorld,&DWorld);
  
  /* clear flag for recalcuration */
  DWorld.flg = 0;

  return 0;
}



/* Initialize routine : 初期化ルーチン群 */
init_all()
{
  ResetGraph(0);		/* Reset GPU : GPUリセット */
  padd=0;			/* Initialize controller value :
				   コントローラ値初期化 */

  GsInitGraph(640,480,GsOFSGPU|GsINTER,1,0);	/* rezolution set ,
						   interrace mode :
						   解像度設定
						   （インターレースモード） */
    

  GsDefDispBuff(0,0,0,0);	/* Double buffer setting :
				   ダブルバッファ指定 */
#if 0    
  GsInitGraph(320,240,1,0,0);	/* rezolution set , non interrace mode :
				   解像度設定（インターレースモード） */
  GsDefDispBuff(0,0,0,240);	/* Double buffer setting :
				   ダブルバッファ指定 */
#endif

  GsInit3D();			/* Init 3D system : ３Dシステム初期化 */

  Wot[0].length=OT_LENGTH;	/* Set bit length of OT handler :
				   オーダリングテーブルハンドラに解像度設定 */

  Wot[0].org=zsorttable[0];	/* Set Top address of OT Area to OT handler :
				   オーダリングテーブルハンドラに
				   オーダリングテーブルの実体設定 */

  /* same setting for anoter OT handler :
     ブルバッファのためもう一方にも同じ設定 */
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  coord_init();			/* Init coordinate : 座標定義 */
  model_init();			/* Reading modeling data :
				   モデリングデータ読み込み */
  view_init();			/* Setting view point : 視点設定 */
  light_init();			/* Setting Flat Light : 平行光源設定 */
}


view_init()			/* Setting view point : 視点設定 */
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/* Set projection : プロジェクション設定 */
  
  /* Setting view point location : 視点パラメータ設定 */
  view.vpx = 0; view.vpy = 0; view.vpz = 2000;
  
  /* Setting focus point location : 注視点パラメータ設定 */
  view.vrx = 0; view.vry = 0; view.vrz = 0;
  
  /* Setting bank of SCREEN : 視点の捻りパラメータ設定 */
  view.rz=0;

  /* Setting parent of viewing coordinate : 視点座標パラメータ設定 */
  view.super = WORLD;
  
  /* Calculate World-Screen Matrix from viewing paramter :
     視点パラメータを群から視点を設定する
     ワールドスクリーンマトリックスを計算する */
  GsSetRefView2(&view);
}


light_init()			/* init Flat light : 平行光源設定 */
{
  /* Setting Light ID 0 : ライトID０ 設定 */
  /* Setting direction vector of Light0 :
     平行光源方向パラメータ設定 */
  pslt[0].vx = 20; pslt[0].vy= -100; pslt[0].vz= -100;
  
  /* Setting color of Light0 :
     平行光源色パラメータ設定 */
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  
  /* Set Light0 from parameters :
     光源パラメータから光源設定 */
  GsSetFlatLight(0,&pslt[0]);

  
  /* Setting Light ID 1 : ライトID１ 設定 */
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= 100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /* Setting Light ID 2 : ライトID２ 設定 */
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= -100;
  pslt[2].r=0x60; pslt[2].g=0x60; pslt[2].b=0x60;
  GsSetFlatLight(2,&pslt[2]);
  
  /* Setting Ambient : アンビエント設定 */
  GsSetAmbient(0,0,0);

  /* Setting default light mode : 光源計算のデフォルトの方式設定 */
  GsSetLightMode(0);
}


coord_init()			/* Setting coordinate : 座標系設定 */
{
  /* Setting hierarchy of Coordinate : 座標の定義 */
  GsInitCoordinate2(WORLD,&DWorld);
  
  /* Init work vector :
     マトリックス計算ワークのローテーションベクター初期化 */
  PWorld.vx=PWorld.vy=PWorld.vz=0;
  
  /* the org point of DWold is set to Z = -40000 :
     オブジェクトの原点をワールドのZ = -4000に設定 */
  DWorld.coord.t[2] = -4000;
}


/* Set coordinte parameter from work vector :
   ローテションベクタからマトリックスを作成し座標系にセットする */
set_coordinate(pos,coor)
SVECTOR *pos;			/* work vector : ローテションベクタ */
GsCOORDINATE2 *coor;		/* Coordinate : 座標系 */
{
  MATRIX tmp1;
  
  /* Set translation : 平行移動をセットする */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  
  /* Rotate Matrix :
     マトリックスにローテーションベクタを作用させる */
  RotMatrix(pos,&tmp1);
  
  /* Set Matrix to Coordinate :
     求めたマトリックスを座標系にセットする */
  coor->coord = tmp1;

  /* Clear flag becase of changing parameter :
     マトリックスキャッシュをフラッシュする */
  coor->flg = 0;
}

/* Load texture to VRAM :
   テクスチャデータをVRAMにロードする */

texture_init(addr)
u_long addr;
{
  RECT rect1;
  GsIMAGE tim1;

  /* Get texture data type information from header of TIM data :
     TIMデータのヘッダからテクスチャのデータタイプの情報を得る */  
  GsGetTimInfo((u_long *)(addr+4),&tim1);
  
  rect1.x=tim1.px;		/* X coordinate (in VRAM) of upper-left of
				   texture :
				   テクスチャ左上のVRAMでのX座標 */
  rect1.y=tim1.py;		/* Y coordinate (in VRAM) of upper-left of
				   texture :
				   テクスチャ左上のVRAMでのY座標 */
  rect1.w=tim1.pw;		/* Width of texture : テクスチャ幅 */
  rect1.h=tim1.ph;		/* Height of texture : テクスチャ高さ */
  
  /* Load texture to VRAM : VRAMにテクスチャをロードする */
  LoadImage(&rect1,tim1.pixel);
  
  /* Exist Color Lookup Table : カラールックアップテーブルが存在する */  
  if((tim1.pmode>>3)&0x01)
    {
      rect1.x=tim1.cx;		/* X coordinate (in VRAM) of upper-left
				   of CLUT :
				   クラット左上のVRAMでのX座標 */
      rect1.y=tim1.cy;		/* Y coordinate (in VRAM) of upper-left
				   of CLUT :
				   クラット左上のVRAMでのY座標 */
      rect1.w=tim1.cw;		/* Width of CLUT : クラットの幅 */
      rect1.h=tim1.ch;		/* Height of CLUT : クラットの高さ */

      /* Load CULT data to VRAM : VRAMにクラットをロードする */
      LoadImage(&rect1,tim1.clut);
    }
}


/* Read modeling data (TMD FORMAT) : モデリングデータの読み込み */
model_init()
{
  u_long *dop;
  GsDOBJ5 *objp;		/* handler of object :
				   モデリングデータハンドラ */
  int i;
  u_long *oppp;			/* packet area pointer */
  
  dop=(u_long *)MODEL_ADDR;	/* Top Address of MODELING DATA(TMD FORMAT) :
				   モデリングデータが格納されているアドレス */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* Map modeling data (TMD format) to real
				   address :
				   モデリングデータ（TMDフォーマット）を
				   実アドレスにマップする*/

  dop++;
  Objnum = *dop;		/* Get number of Objects :
				   オブジェクト数をTMDのヘッダから得る */

  dop++;			/* Bring to TMD object header in order to
				   link with GsLink Ojbect2 :
				   GsLinkObject2でリンクするためにTMDの
				   オブジェクトの先頭にもってくる */

  /* Link ObjectHandler and TMD FORMAT MODELING DATA :
     TMDデータとオブジェクトハンドラを接続する */
  for(i=0;i<Objnum;i++)
    GsLinkObject5((u_long)dop,&object[i],i);
  /* Top address of packet area :
     packetの枠組を作るアドレス */
  oppp = PacketArea;
  
  for(i=0,objp=object;i<Objnum;i++)
    {	
      /* Set Coordinate of Object Handler :
	 フォルトのオブジェクトの座標系の設定 */
      objp->coord2 =  &DWorld;
      
      objp->attribute = 0;	/* init attribute */
      
      /* The packet framework must be build beforhand for GsOBJ5-type objects :
	 GsDOBJ5型のオブジェクトは予めパケットの枠組を作って
	 おかなかれば使えない */      
      oppp = GsPresetObject(objp,oppp);	
      
      objp++;
    }
}

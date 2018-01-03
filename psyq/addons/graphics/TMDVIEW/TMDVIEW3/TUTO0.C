/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *	PMD VIEWING ROUTINE
 *
 *
 *		Version 1.00	Jul,  26, 1994
 *
 *		Copyright (C) 1994  Sony Computer Entertainment
 *		All rights Reserved
 */

#include <sys/types.h>

#include <libetc.h>		/* for Control Pad :
				   PADを使うためにインクルードする必要あり */
#include <libgte.h>		/* LIBGS uses libgte :
				   LIGSを使うためにインクルードする必要あり*/
#include <libgpu.h>		/* LIBGS uses libgpu :
				   LIGSを使うためにインクルードする必要あり */
#include <libgs.h>		/* for LIBGS :
				   グラフィックライブラリ を使うための
				   構造体などが定義されている */


#define OBJECTMAX 100		/* Max Objects :
				   ３Dのモデルは論理的なオブジェクトに
                                   分けられるこの最大数 を定義する */

#define TEX_ADDR   0xa0010000	/* Top Address of texture data1 (TIM FORMAT) :
				   テクスチャデータ（TIMフォーマット）
				   がおかれるアドレス */

#define TIM_HEADER 0x00000010

#define MODEL_ADDR 0xa0040000	/* Top Address of modeling data (TMD FORMAT) :
				   モデリングデータ（TMDフォーマット）
				   がおかれるアドレス */

#define OT_LENGTH  10		/* bit length of OT :
				   オーダリングテーブルの解像度 */


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer :
				   オーダリングテーブルハンドラ
				   ダブルバッファのため２つ必要 */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Area of OT:
						オーダリングテーブル実体 */

GsDOBJ3		object[OBJECTMAX]; /* Array of Object Handler :
				      オブジェクトハンドラ
				      オブジェクトの数だけ必要 */

u_long          Objnum;		/* valibable of number of Objects :
				   モデリングデータのオブジェクトの数を
				   保持する */

/* object coordinate : オブジェクト座標系 */
GsCOORDINATE2   DWorld;


SVECTOR         PWorld; /* work short vector for making Coordinate parmeter :
			   座標系を作るためのローテーションベクター */

extern MATRIX GsIDMATRIX;	/* Unit Matrix : 単位行列 */

GsRVIEW2  view;			/* View Point Handler :
				   視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler :
				   平行光源を設定するための構造体 */
u_long padd;			/* Controler data :
				   コントローラのデータを保持する */

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ3 *op;			/* pointer of Object Handler :
				   オブジェクトハンドラへのポインタ */
  int     outbuf_idx;		/* double buffer index */
  MATRIX  tmpls;
  
  ResetCallback();
  init_all();
  
  while(1)
    {
      if(obj_interactive()==0)
          return 0;	/* interactive parameter get :
			   パッドデータから動きのパラメータを入れる */
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
	  GsSortObject3(op,&Wot[outbuf_idx],14-OT_LENGTH);
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


obj_interactive()
{
  SVECTOR v1;
  MATRIX  tmp1;
  
  /* Rotate Y  :
     オブジェクトをY軸回転させる */
  if((padd & PADRleft)>0) PWorld.vy -=5*ONE/360;

  /* Rotate Y :
     オブジェクトをY軸回転させる */
  if((padd & PADRright)>0) PWorld.vy +=5*ONE/360;

  /* Rotate X :
     オブジェクトをX軸回転させる */
  if((padd & PADRup)>0) PWorld.vx+=5*ONE/360;

  /* Rotate X :
     オブジェクトをX軸回転させる */
  if((padd & PADRdown)>0) PWorld.vx-=5*ONE/360;
  
  /* Translate Z :
     オブジェクトをZ軸にそって動かす */
  if((padd & PADm)>0) DWorld.coord.t[2]-=100;
  
  /* Translate Z :
     オブジェクトをZ軸にそって動かす */
  if((padd & PADl)>0) DWorld.coord.t[2]+=100;

  /* Translate X :
     オブジェクトをX軸にそって動かす */
  if((padd & PADLleft)>0) DWorld.coord.t[0] +=10;

  /* Translate X :
     オブジェクトをX軸にそって動かす */
  if((padd & PADLright)>0) DWorld.coord.t[0] -=10;

  /* Translate Y :
     オブジェクトをY軸にそって動かす */
  if((padd & PADLdown)>0) DWorld.coord.t[1]+=10;

  /* Translate Y :
     オブジェクトをY軸にそって動かす */
  if((padd & PADLup)>0) DWorld.coord.t[1]-=10;

  /* exit program :
     プログラムを終了してモニタに戻る */
  if((padd & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}
  
  /* Calculate Matrix from Object Parameter and Set Coordinate :
     オブジェクトのパラメータからマトリックスを計算し座標系にセット */
  set_coordinate(&PWorld,&DWorld);
  
  /* Clear flag of Coordinate for recalculation :
     再計算のためのフラグをクリアする */
  DWorld.flg = 0;
  return 1;
}

/* Initialize routine : 初期化ルーチン群 */
init_all()
{
  ResetGraph(0);		/* Reset GPU /* GPUリセット */
  PadInit(0);			/* Reset Controller /* コントローラ初期化 */
  padd=0;			/*/* コントローラ値初期化 */
  
  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  /* rezolution set , interrace mode :
     解像度設定（インターレースモード） */
    
  GsDefDispBuff(0,0,0,0);	/* Double buffer setting
                                /* ダブルバッファ指定 */
#if 0    
  GsInitGraph(320,240,GsNONINTER|GsOFSGPU,0,0);
  /* rezolution set , non interrace mode :
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
     ダブルバッファのためもう一方にも同じ設定 */
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  
  coord_init();			/* Init coordinate : 座標定義 */
  model_init();			/* Reading modeling data :
				   モデリングデータ読み込み */  
  view_init();			/* Setting view point : 視点設定 */
  light_init();			/* Setting Flat Light : 平行光源設定 */
  
  initTexture1();	        /* texture load of TEX_ADDR */
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


initTexture1()
{
        RECT rect1;
        GsIMAGE tim1;
        u_long *tex_addr;
        int i;

        tex_addr = (u_long *)TEX_ADDR;          /* Top of TIM data address */
        while(1) {
                if(*tex_addr != TIM_HEADER) {
                        break;
                }
                tex_addr++;     /* Skip TIM file header (1word) */
                GsGetTimInfo(tex_addr, &tim1);
                tex_addr += tim1.pw*tim1.ph/2+3+1;    /* Next Texture address*/
                rect1.x=tim1.px;
                rect1.y=tim1.py;
                rect1.w=tim1.pw;
                rect1.h=tim1.ph;
                printf("XY:(%d,%d) WH:(%d,%d)\n",tim1.px,tim1.py,
                                                tim1.pw,tim1.ph);
                LoadImage(&rect1,tim1.pixel);
                DrawSync(0);
                if((tim1.pmode>>3)&0x01) {      /* if clut exist */
                        rect1.x=tim1.cx;
                        rect1.y=tim1.cy;
                        rect1.w=tim1.cw;
                        rect1.h=tim1.ch;
                        LoadImage(&rect1,tim1.clut);
                        DrawSync(0);
                        tex_addr += tim1.cw*tim1.ch/2+3;
                printf("CXY:(%d,%d) CWH:(%d,%d)\n",tim1.cx,tim1.cy,
                                                tim1.cw,tim1.ch);
                }
        }
}


/* Read modeling data (TMD FORMAT) : モデリングデータの読み込み */
model_init()
{
  u_long *dop;
  GsDOBJ3 *objp;		/* handler of object :
				   モデリングデータハンドラ */
  int i;
  
  dop=(u_long *)MODEL_ADDR;	/* Top Address of MODELING DATA(TMD FORMAT) :
				   モデリングデータが格納されているアドレス */

  /* Link ObjectHandler and TMD FORMAT MODELING DATA :
     TMDデータとオブジェクトハンドラを接続する */
  Objnum = GsLinkObject3((unsigned long)dop,object);

  for(i=0,objp=object;i<Objnum;i++)
    {	
      /* Set Coordinate of Object Handler :
	 デフォルトのオブジェクトの座標系の設定 */
      objp->coord2 =  &DWorld;
      
      objp->attribute = 0;	/* init attribute */
      objp++;
    }
}

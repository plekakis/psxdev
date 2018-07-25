/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *	spaceshuttle: sample program
 *
 *	"tuto0.c" main routine
 *
 *		Version 1.00	Mar,  31, 1994
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

#define TEX_ADDR   0x80010000	/* Top Address of texture data1 (TIM FORMAT) :
                                   テクスチャデータ（TIMフォーマット）
				   がおかれるアドレス */

#define MODEL_ADDR 0x80040000	/* Top Address of modeling data (TMD FORMAT) :
				   モデリングデータ（TMDフォーマット）
				   がおかれるアドレス */


#define OT_LENGTH  10		/* Area of OT : オーダリングテーブルの解像度 */


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer :
				   オーダリングテーブルハンドラ
				   ダブルバッファのため２つ必要 */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Area of OT :
						オーダリングテーブル実体 */

GsDOBJ5		object[OBJECTMAX]; /* Array of Object Handler :
				      オブジェクトハンドラ
				      オブジェクトの数だけ必要 */

unsigned long   Objnum;		/* valibable of number of Objects :
				   モデリングデータのオブジェクトの数を
				   保持する */


GsCOORDINATE2   DSpaceShattle,DSpaceHatchL,DSpaceHatchR,DSatt;
/* Coordinate for GsDOBJ2 : オブジェクトごとの座標系 */

SVECTOR         PWorld,PSpaceShattle,PSpaceHatchL,PSpaceHatchR,PSatt;
/* work short vector for making Coordinate parmeter :
   オブジェクトごとの座標系を作るための元データ */

GsRVIEW2  view;			/* View Point Handler :
				   視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler :
				   平行光源を設定するための構造体 */
unsigned long padd;		/* Controler data :
				   コントローラのデータを保持する */

u_long          preset_p[0x10000];

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ5 *op;			/* pointer of Object Handler :
				   オブジェクトハンドラへのポインタ */
  int     outbuf_idx;
  MATRIX  tmpls;
  
  ResetCallback();
  init_all();

  GsInitVcount();
  while(1)
    {
      if(obj_interactive()==0)
        return 0;	/* interactive parameter get ;
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
	     スクリーン／ローカルマトリックスを計算する */
	  GsGetLs(op->coord2,&tmpls);
	  /* Set LWMATRIX to GTE Lighting Registers :
	     ライトマトリックスをGTEにセットする */
	  GsSetLightMatrix(&tmpls);
	  /* Set LSAMTRIX to GTE Registers :
	     スクリーン／ローカルマトリックスをGTEにセットする */
	  GsSetLsMatrix(&tmpls);
	  /* Perspective Translate Object and Set OT :
	     オブジェクトを透視変換しオーダリングテーブルに登録する */
	  GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,getScratchAddr(0));
  	  op++;
	}
      padd=PadRead(0);		/* Readint Control Pad data :
				   パッドのデータを読み込む */
      VSync(0);			/* Wait for VSYNC : Vブランクを待つ */
      ResetGraph(1);		/* Reset GPU */
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
  
  /* rotate Y axis : シャトルをY軸回転させる */
  if((padd & PADRleft)>0) PSpaceShattle.vy +=5*ONE/360;
  /* rotate Y axis : シャトルをY軸回転させる */
  if((padd & PADRright)>0) PSpaceShattle.vy -=5*ONE/360;
  /* rotate X axis :シャトルをX軸回転させる */
  if((padd & PADRup)>0) PSpaceShattle.vx+=5*ONE/360;
  /* rotate X axis : シャトルをX軸回転させる */
  if((padd & PADRdown)>0) PSpaceShattle.vx-=5*ONE/360;
  
  /* transfer Z axis : シャトルをZ軸にそって動かす */
  if((padd & PADh)>0)      DSpaceShattle.coord.t[2]+=100;
  
  /* transfer Z axis : シャトルをZ軸にそって動かす */
  /* if((padd & PADk)>0)      DSpaceShattle.coord.t[2]-=100; */
  
  /* exit program :
     プログラムを終了してモニタに戻る */
  if((padd & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}
  
  
  /* transfer X axis : シャトルをX軸にそって動かす */
  if((padd & PADLleft)>0) DSpaceShattle.coord.t[0] -=10;
  /* transfer X axis : シャトルをX軸にそって動かす */
  if((padd & PADLright)>0) DSpaceShattle.coord.t[0] +=10;

  /* transfer Y axis : シャトルをY軸にそって動かす */
  if((padd & PADLdown)>0) DSpaceShattle.coord.t[1]+=10;
  /* transfer Y axis : シャトルをY軸にそって動かす */
  if((padd & PADLup)>0) DSpaceShattle.coord.t[1]-=10;
  
  if((padd & PADl)>0)
    {				/* open the hatch : ハッチを開く */
      object[3].attribute &= 0x7fffffff;	/* exist the satellite :
						   衛星を存在させる */
      /* set the rotate parameter of the right hatch along z axis :
	 右ハッチをZ軸に回転させるためのパラメータをセットする */
      PSpaceHatchR.vz -= 2*ONE/360;
      /* set the rotate parameter of the left hatch along z axis :
	 左ハッチをZ軸に回転させるためのパラメータをセットする */      
      PSpaceHatchL.vz += 2*ONE/360;
      /* caliculate the matrix and set :
	 右ハッチをZ軸に回転させるためにパラメータからマトリックスを計算し
	 右ハッチの座標系にセットする */
      set_coordinate(&PSpaceHatchR,&DSpaceHatchR);
      /* caliculate the matrix and set :
	 左ハッチをZ軸に回転させるためにパラメータからマトリックスを計算し
	 左ハッチの座標系にセットする */
      set_coordinate(&PSpaceHatchL,&DSpaceHatchL);
    }
  if((padd & PADm)>0)
    {				/* close the hatch : ハッチをとじる */
      PSpaceHatchR.vz += 2*ONE/360;
      PSpaceHatchL.vz -= 2*ONE/360;
      set_coordinate(&PSpaceHatchR,&DSpaceHatchR);
      set_coordinate(&PSpaceHatchL,&DSpaceHatchL);
    }

  if((padd & PADn)>0 && (object[3].attribute & 0x80000000) == 0)
    {				/* transfer the satellite : 衛星を送出する */
      DSatt.coord.t[1] -= 10;	/* translation parameter set :
				   衛星をX軸に沿って動かすパラメータセット */
      /* rotation parameter set :
	 衛星をY軸に沿って回転させるためのパラメータセット */
      PSatt.vy += 2*ONE/360;
      /* set the matrix to the coordinate from parameters :
	 パラメータからマトリックスを計算し座標系にセット */
      set_coordinate(&PSatt,&DSatt);
    }
  if((padd & PADo)>0 && (object[3].attribute & 0x80000000) == 0)
    {				/* transfer the satellite : 衛星を回収する */
      DSatt.coord.t[1] += 10;
      PSatt.vy -= 2*ONE/360;
      set_coordinate(&PSatt,&DSatt);
    }
  /* set the matrix to the coordinate from the parameters of the shuttle :
     シャトルのパラメータからマトリックスを計算し座標系にセット */
  set_coordinate(&PSpaceShattle,&DSpaceShattle);
  return 1;
}


init_all()			/* Initialize routine : 初期化ルーチン群 */
{
  ResetGraph(0);		/* reset GPU */
  PadInit(0);			/* Reset Controller : コントローラ初期化 */
  padd=0;			/* controller value initialize :
				   コントローラ値初期化 */
  GsInitGraph(640,480,2,1,0);	/* rezolution set , non interrace mode :
				   解像度設定（インターレースモード） */
  GsDefDispBuff(0,0,0,0);	/* Double buffer setting :
				   ダブルバッファ指定 */

/*  GsInitGraph(640,240,1,1,0);	/* rezolution set , non-interrace mode :
    解像度設定（ノンインターレースモード） */
/*  GsDefDispBuff(0,0,0,240);	/* Double buffer setting :ダブルバッファ指定 */

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
  /*
  texture_init(TEX_ADDR);
  */
}

view_init()			/* Setting view point : 視点設定 */
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/* Set projection : プロジェクション設定 */
  
  /* Setting view point location : 視点パラメータ設定 */
  view.vpx = 0; view.vpy = 0; view.vpz = -2000;
  /* Setting focus point location : 注視点パラメータ設定 */
  view.vrx = 0; view.vry = 0; view.vrz = 0;
  /* Setting bank of SCREEN : 視点の捻りパラメータ設定 */
  view.rz=0;
  /* Setting parent of viewing coordinate : 視点座標パラメータ設定 */  
  view.super = WORLD;
  /* view.super = &DSatt; */
  
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
  pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
  /* Setting color of Light0 : 平行光源色パラメータ設定 */
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  /* Set Light0 from parameters : 光源パラメータから光源設定 */
  GsSetFlatLight(0,&pslt[0]);
  
  /* Setting Light ID 1 : ライトID１ 設定 */
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /* Setting Light ID 2 : ライトID２ 設定 */
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
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
  /* SpaceShuttle's coordinate connect the WORLD coordinate directly :
     SpaceShattle座標系はワールドに直にぶら下がる */
  GsInitCoordinate2(WORLD,&DSpaceShattle);
  /* the right hatch's coordinate connects the shuttle's :
     ハッチ右の座標系はスペースシャトルにぶら下がる */
  GsInitCoordinate2(&DSpaceShattle,&DSpaceHatchL);
  /* the left hatch's coordinate connects the shuttle's :
     ハッチ左の座標系はスペースシャトルにぶら下がる */
  GsInitCoordinate2(&DSpaceShattle,&DSpaceHatchR);
  /* the satellite's coordinate connects the shuttle's :
     衛星の座標系はスペースシャトルにぶら下がる */
  GsInitCoordinate2(&DSpaceShattle,&DSatt);
  
  /* offset the hatch's orign point to the edge of the shuttle :
     ハッチの原点をスペースシャトルのへりに持ってくる */
  DSpaceHatchL.coord.t[0] =  356;
  DSpaceHatchR.coord.t[0] = -356;
  DSpaceHatchL.coord.t[1] = 34;
  DSpaceHatchR.coord.t[1] = 34;
  
  /* offset the satellite's orign point 20 from the orign point of the shuttle:
     衛星の原点をスペースシャトルの原点からY軸に２０ずらす */
  DSatt.coord.t[1] = 20;
  
  /* Init work vector : マトリックス計算ワークのローテーションベクター初期化 */
  PWorld.vx=PWorld.vy=PWorld.vz=0;
  PSatt = PSpaceHatchR = PSpaceHatchL = PSpaceShattle = PWorld;
  /* the org point of DWold is set to Z = 4000 :
     スペースシャトルの原点をワールドのZ = 4000に設定 */
  DSpaceShattle.coord.t[2] = 4000;
}

/* Set coordinte parameter from work vector :
   マトリックス計算ワークからマトリックスを作成し座標系にセットする */
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


/* Load texture to VRAM : テクスチャデータをVRAMにロードする */
texture_init(addr)
unsigned long addr;
{
  RECT rect1;
  GsIMAGE tim1;
  
  /* Get texture information of TIM FORMAT :
     TIMデータのヘッダからテクスチャのデータタイプの情報を得る */  
  GsGetTimInfo((unsigned long *)(addr+4),&tim1);
  rect1.x=tim1.px;		/* X point of image data on VRAM :
				   テクスチャ左上のVRAMでのX座標 */
  rect1.y=tim1.py;		/* Y point of image data on VRAM :
				   テクスチャ左上のVRAMでのY座標 */
  rect1.w=tim1.pw;		/* Width of image : テクスチャ幅 */
  rect1.h=tim1.ph;		/* Height of image : テクスチャ高さ */
  
  /* Load texture to VRAM : VRAMにテクスチャをロードする */
  LoadImage(&rect1,tim1.pixel);
  
  /* Exist Color Lookup Table : カラールックアップテーブルが存在する */  
  if((tim1.pmode>>3)&0x01)
    {
      rect1.x=tim1.cx;		/* X point of CLUT data on VRAM :
				   クラット左上のVRAMでのX座標 */
      rect1.y=tim1.cy;		/* Y point of CLUT data on VRAM :
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
  unsigned long *dop;
  GsDOBJ5 *objp;		/* handler of object :
				   モデリングデータハンドラ */
  int i;
  u_long *oppp;
  
  dop=(u_long *)MODEL_ADDR;	/* Top Address of MODELING DATA(TMD FORMAT) :
				   モデリングデータが格納されているアドレス */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* Mappipng real address :
				   モデリングデータ（TMDフォーマット）を
				   実アドレスにマップする */
  dop++;
  Objnum = *dop;		/* Get number of Objects :
				   オブジェクト数をTMDのヘッダから得る */
  dop++;			/* Adjusting for GsLinkObject4 :
				   GsLinkObject2でリンクするためにTMDの
				   オブジェクトの先頭にもってくる */
  /* Link ObjectHandler and TMD FORMAT MODELING DATA :
     TMDデータとオブジェクトハンドラを接続する */
  for(i=0;i<Objnum;i++)		
    GsLinkObject5((unsigned long)dop,&object[i],i);
  
  oppp = preset_p;
  for(i=0,objp=object;i<Objnum;i++)
    {
      /* Set Coordinate of Object Handler :
	 デフォルトのオブジェクトの座標系の設定 */
      objp->coord2 =  &DSpaceShattle;
				/* default object attribute (not display):
				   デフォルトのオブジェクトのアトリビュート
				   の設定（表示しない） */
      objp->attribute = GsDOFF;
      oppp = GsPresetObject(objp,oppp);
      objp++;
    }
  
  object[0].attribute &= ~GsDOFF;
  
  object[0].attribute &= ~GsDOFF;	/* display on : 表示オン */
  object[0].coord2    = &DSpaceShattle;	/* set the shuttle coordinate :
					   スペースシャトルの座標に設定 */
  object[1].attribute &= ~GsDOFF;	/* display on : 表示オン */
  object[1].attribute |= GsALON;	/* semi-transparent : 半透明 */
  object[1].coord2    = &DSpaceHatchR;  /* set the right hatch coordinate :
					   ハッチ右の座標に設定 */
  object[2].attribute &= ~GsDOFF;	/* display on :表示オン */
  object[2].attribute |= GsALON;	/* semi-transparent : 半透明 */
  object[2].coord2    = &DSpaceHatchL;  /* set the left hatch coordiante :
					   ハッチ左の座標に設定 */
  
  object[3].coord2    = &DSatt;	/* set the satellite coordinate :
				   衛星の座標に設定 */
}

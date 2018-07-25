/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 * tmdview4: GsDOBJ2 object viewing rotine(multi OT) 
 * 
 * "tuto0.c" ******** simple GsDOBJ5 Viewing routine 
 * 
 * Version 1.00	Jul,  14, 1994 
 * 
 * Copyright (C) 1993 by Sony Computer Entertainment All rights Reserved 
 */

#include <sys/types.h>

#include <libetc.h>		/* for Control Pad :
				   PADを使うためにインクルードする必要あり */
#include <libgte.h>		/* LIBGS uses libgte :
				   LIGSを使うためにインクルードする必要あり */
#include <libgpu.h>		/* LIBGS uses libgpu :
				   LIGSを使うためにインクルードする必要あり */
#include <libgs.h>		/* for LIBGS : グラフィックライブラリ を使うための
				   構造体などが定義されている */

#define OBJECTMAX 100		/* Max Objects :
				   ３Dのモデルは論理的なオブジェクトに 分けられるこの最大数
				   を定義する */

#define TEX_ADDR   0x80010000	/* Top Address of texture data1 (TIM FORMAT)
				   : テクスチャデータ（TIMフォーマット）がおかれるアドレス */

#define TEX_ADDR1   0x80020000	/* Top Address of texture data1 (TIM FORMAT) */
#define TEX_ADDR2   0x80030000	/* Top Address of texture data2 (TIM FORMAT) */


#define MODEL_ADDR 0x80040000	/* Top Address of modeling data (TMD FORMAT)
				   : モデリングデータ（TMDフォーマット）がおかれるアドレス */

#define OT_LENGTH  4		/* bit length of OT :
				   オーダリングテーブルの解像度 */


GsOT    Wot1[2];		/* OT handler for double buffers :
				   オーダリングテーブルハンドラダブルバッファのため２つ必要 */

GsOT    Wot2[2];		/* OT handler for double buffers :
				   オーダリングテーブルハンドラダブルバッファのため２つ必要 */

GsOT_TAG zsorttable1[2][1 << OT_LENGTH];	/* substance of OT :
						   オーダリングテーブル実体 */
GsOT_TAG zsorttable2[2][1 << OT_LENGTH];	/* substance of OT :
						   オーダリングテーブル実体 */

GsDOBJ2 object[OBJECTMAX];	/* Array of Object Handler :
				   オブジェクトハンドラオブジェクトの数だけ必要 */

u_long  Objnum;			/* valibable of number of Objects :
				   モデリングデータのオブジェクトの数を 保持する */


GsCOORDINATE2 DWorld, DLocal1, DLocal2;	/* coordinates for objects :
					   オブジェクトごとの座標系 */

/* work short vector for making Coordinate parameter :
   座標系を作るためのローテーションベクター */
SVECTOR PWorld, PLocal1, PLocal2;


GsRVIEW2 view;			/* View Point Handler : 視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler : 平行光源を設定するための構造体 */
u_long  padd;			/* Controler data : コントローラのデータを保持する */

u_long  out_packet[2][0x14000]; /* GPU PACKETS AREA */

/************* MAIN START ******************************************/
main()
{
	int     i;
	GsDOBJ2 *op;		/* the pointer of Object handler :
				   オブジェクトハンドラへのポインタ */
	GsOT_TAG *p;
	int     outbuf_idx;
	MATRIX  tmpls;

	ResetCallback();
	init_all();

	while (1) {
		if (obj_interactive() == 0)
			return 0;	/* interactive parameter get :
					   パッドデータから動きのパラメータを入れる */
		GsSetRefView2(&view);	/* caliculate World/Screen matrix :
					   ワールドスクリーンマトリックス計算 */
		outbuf_idx = GsGetActiveBuff();	/* get the double buffer
						   index : ダブルバッファのどちらかを得る */

		/* auto divide packet work setting : 自動分割用パケットワーク設定 */
		GsSetWorkBase((PACKET *) out_packet[outbuf_idx]);
		GsClearOt(0, 0, &Wot1[outbuf_idx]);	/* Clear OT1 : OT1をクリアする */
		/* Clear OT2, object2 always exists in front of object1
		   because OT2's point value is 0 : OT2をクリアする pointは0
		   よってオブジェクト２は、必ずオブジェクト１ の手前になる */
		GsClearOt(0, 12, &Wot2[outbuf_idx]);

		/* Clear OT2, object2 always lays behaind object1 because
		   OT2's point value is set to 1<<OT_LENGTH-1 : OT2をクリアする
		   pointは1<<OT_LENGTH-1 よってオブジェクト２は
		   必ずオブジェクト１の後ろになる
		   GsClearOt(0,1<<OT_LENGTH-1,&Wot2[outbuf_idx]); */

		for (i = 0, op = object; i < Objnum; i++) {	/* object1 caliculation
								   : オブジェクト１計算 */
			/* Calculate Local-World Matrix :
			   ワールド／ローカルマトリックスを計算する */
			GsGetLw(op->coord2, &tmpls);
			/* Set LWMATRIX to GTE Lighting Registers :
			   ライトマトリックスをGTEにセットする */
			GsSetLightMatrix(&tmpls);
			/* Calculate Local-Screen Matrix :
			   スクリーン／ローカルマトリックスを計算する */
			GsGetLs(op->coord2, &tmpls);
			/* Set LSAMTRIX to GTE Registers :
			   スクリーン／ローカルマトリックスをGTEにセットする */
			GsSetLsMatrix(&tmpls);
			/* オブジェクトを透視変換しオーダリングテーブルに登録する */
			GsSortObject4(op, &Wot1[outbuf_idx], 14 - OT_LENGTH, getScratchAddr(0));
			op++;
		}
		for (i = 0, op = &object[Objnum]; i < Objnum; i++) {	/* object2 caliculation
									   : オブジェクト２計算 */
			/* Calculate Local-World Matrix :
			   ワールド／ローカルマトリックスを計算する */
			GsGetLw(op->coord2, &tmpls);
			/* Set LWMATRIX to GTE Lighting Registers :
			   ライトマトリックスをGTEにセットする */
			GsSetLightMatrix(&tmpls);
			/* Calculate Local-Screen Matrix :
			   スクリーン／ローカルマトリックスを計算する */
			GsGetLs(op->coord2, &tmpls);
			/* Set LSAMTRIX to GTE Registers :
			   スクリーン／ローカルマトリックスをGTEにセットする */
			GsSetLsMatrix(&tmpls);
			/* Perspective Translate Object and Set OT :
			   オブジェクトを透視変換しオーダリングテーブルに登録する */
			GsSortObject4(op, &Wot2[outbuf_idx], 14 - OT_LENGTH, getScratchAddr(0));
			op++;
		}
		padd = PadRead(1);	/* Readint Control Pad data :
					   パッドのデータを読み込む */
		VSync(0);	/* Wait VSYNC : Vブランクを待つ */

		ResetGraph(1);	/* Reset GPU : GPUをリセットする */
		GsSwapDispBuff();	/* Swap double buffer : ダブルバッファを切替える */
		SetDispMask(1);
		/* Set SCREEN CLESR PACKET to top of OT :
		   画面のクリアをオーダリングテーブルの最初に登録する */
		GsSortClear(0x0, 0x0, 0x0, &Wot1[outbuf_idx]);
		/* Sort OT each other , Wot1 is represent sorted OTS :
		   オーダリングテーブル同士をソートする ソートされたOTは Wot1に統合される */
		GsSortOt(&Wot2[outbuf_idx], &Wot1[outbuf_idx]);
		/* Start Drawing :
		   オーダリングテーブルに登録されているパケットの描画を開始する */
		GsDrawOt(&Wot1[outbuf_idx]);
	}
}


obj_interactive()
{
	SVECTOR v1;
	MATRIX  tmp1;

	/* Rotate object1 around Y : オブジェクト1をY軸回転させる */
	if ((padd & PADRleft) > 0)
		PLocal1.vy -= 5 * ONE / 360;
	/* Rotate object1 around Y : オブジェクト1をY軸回転させる */
	if ((padd & PADRright) > 0)
		PLocal1.vy += 5 * ONE / 360;
	/* Rotate object1 around X : オブジェクト1をX軸回転させる */
	if ((padd & PADRup) > 0)
		PLocal1.vx += 5 * ONE / 360;
	/* Rotate object1 around X : オブジェクト1をX軸回転させる */
	if ((padd & PADRdown) > 0)
		PLocal1.vx -= 5 * ONE / 360;
	/* Transfer object1 along Z : オブジェクト1をZ軸にそって動かす */
	if ((padd & PADl) > 0)
		DLocal1.coord.t[2] -= 100;
	/* Transfer object1 along Z : オブジェクト1をZ軸にそって動かす */
	if ((padd & PADm) > 0)
		DLocal1.coord.t[2] += 100;
	/* Calculate Matrix from Object Parameter and Set Coordinate :
	   オブジェクトのパラメータからマトリックスを計算し座標系にセット */
	set_coordinate(&PLocal1, &DLocal1);

	/* Rotate object2 around Y : オブジェクト2をY軸回転させる */
	if ((padd & PADLleft) > 0)
		PLocal2.vy -= 5 * ONE / 360;
	/* Rotate object2 around Y : オブジェクト2をY軸回転させる */
	if ((padd & PADLright) > 0)
		PLocal2.vy += 5 * ONE / 360;
	/* Rotate object2 around X : オブジェクト2をX軸回転させる */
	if ((padd & PADLup) > 0)
		PLocal2.vx += 5 * ONE / 360;
	/* Rotate object2 around X : オブジェクト2をX軸回転させる */
	if ((padd & PADLdown) > 0)
		PLocal2.vx -= 5 * ONE / 360;
	/* Transfer object2 along Z : オブジェクト2をZ軸にそって動かす */
	if ((padd & PADn) > 0)
		DLocal2.coord.t[2] -= 100;
	/* Transfer object2 along Z : オブジェクト2をZ軸にそって動かす */
	if ((padd & PADo) > 0)
		DLocal2.coord.t[2] += 100;
	/* Calculate Matrix from Object Parameter and Set Coordinate :
	   オブジェクトのパラメータからマトリックスを計算し座標系にセット */

	/* exit program : プログラムを終了してモニタに戻る */
	if ((padd & PADk) > 0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	set_coordinate(&PLocal2, &DLocal2);
	return 1;
}


init_all()
{				/* initialize rotines : 初期化ルーチン群 */
	ResetGraph(0);		/* reset GPU : GPUリセット */
	PadInit(0);		/* init controler : コントローラ初期化 */
	padd = 0;		/* init contorler value : コントローラ値初期化 */

#if 0
	GsInitGraph(640, 480, GsINTER | GsOFSGPU, 1, 0);
	/* set the resolution of screen (interrace mode) :
	   解像度設定（インターレースモード） */
	GsDefDispBuff(0, 0, 0, 0);	/* set the double buffers :
					   ダブルバッファ指定 */
#endif

	GsInitGraph(640, 240, GsNONINTER | GsOFSGPU, 1, 0);
	/* set the resolution of screen (on interrace mode) :
	   解像度設定（インターレースモード）ディザオン GPUオフセット */
	GsDefDispBuff(0, 0, 0, 240);	/* set the double buffers :
					   ダブルバッファ指定 */
	GsInit3D();		/* init 3d part of libgs : ３Dシステム初期化 */

	Wot1[0].length = OT_LENGTH;	/* set the length of OT :
					   オーダリングテーブルハンドラに解像度設定 */
	Wot1[0].org = zsorttable1[0];	/* set the top address of OT1 tags :
					   オーダリングテーブルハンドラに
					   オーダリングテーブルの実体設定 */
	/* set anoter OT for double buffer : ダブルバッファのためもう一方にも同じ設定 */
	Wot1[1].length = OT_LENGTH;
	Wot1[1].org = zsorttable1[1];

	Wot2[0].length = OT_LENGTH;
	Wot2[0].org = zsorttable2[0];

	Wot2[1].length = OT_LENGTH;
	Wot2[1].org = zsorttable2[1];

	coord_init();		/* initialize the coordinate system : 座標定義 */
	model_init();		/* set up the modeling data :
				   モデリングデータ読み込み */
	view_init();		/* set the viewpoint : 視点設定 */
	light_init();		/* set the flat light : 平行光源設定 */

	texture_init(TEX_ADDR);	/* 16bit texture load */
	texture_init(TEX_ADDR1);/* 8bit  texture load */
	texture_init(TEX_ADDR2);/* 4bit  texture load */
}


view_init()
{				/* set the viewpoint : 視点設定 */
	/*---- Set projection,view ----*/
	GsSetProjection(1000);	/* set the projection : プロジェクション設定 */

	/* set the viewpoint parameter : 視点パラメータ設定 */
	view.vpx = 0;
	view.vpy = 0;
	view.vpz = 2000;
	/* set the refarence point parameter : 注視点パラメータ設定 */
	view.vrx = 0;
	view.vry = 0;
	view.vrz = -4000;
	/* set the roll pameter of viewpoint : 視点の捻りパラメータ設定 */
	view.rz = 0;
	view.super = WORLD;	/* set the view coordinate : 視点座標パラメータ設定 */

	/* set the view point from parameters (libgs caliculate World-Screen
	   Matrix) : 視点パラメータを群から視点を設定する
	   ワールドスクリーンマトリックスを計算する */
	GsSetRefView2(&view);
}


light_init()
{				/* init Flat light : 平行光源設定 */
	/* Setting Light ID 0 : ライトID０ 設定 */
	/* Setting direction vector of Light0 : 平行光源方向パラメータ設定 */
	pslt[0].vx = 20;
	pslt[0].vy = -100;
	pslt[0].vz = -100;

	/* Setting color of Light0 : 平行光源色パラメータ設定 */
	pslt[0].r = 0xd0;
	pslt[0].g = 0xd0;
	pslt[0].b = 0xd0;

	/* Set Light0 from parameters : 光源パラメータから光源設定 */
	GsSetFlatLight(0, &pslt[0]);

	/* Setting Light ID 1 : ライトID１ 設定 */
	pslt[1].vx = 20;
	pslt[1].vy = -50;
	pslt[1].vz = 100;
	pslt[1].r = 0x80;
	pslt[1].g = 0x80;
	pslt[1].b = 0x80;
	GsSetFlatLight(1, &pslt[1]);

	/* Setting Light ID 2 : ライトID２ 設定 */
	pslt[2].vx = -20;
	pslt[2].vy = 20;
	pslt[2].vz = -100;
	pslt[2].r = 0x60;
	pslt[2].g = 0x60;
	pslt[2].b = 0x60;
	GsSetFlatLight(2, &pslt[2]);

	/* Setting Ambient : アンビエント設定 */
	GsSetAmbient(0, 0, 0);

	/* Setting default light mode : 光源計算のデフォルトの方式設定 */
	GsSetLightMode(0);
}

coord_init()
{				/* Setting coordinate : 座標系設定 */
	/* init coordinates : 座標の定義 */
	GsInitCoordinate2(WORLD, &DWorld);
	GsInitCoordinate2(&DWorld, &DLocal1);
	GsInitCoordinate2(&DWorld, &DLocal2);
	/* Init work vector : マトリックス計算ワークのローテーションベクター初期化 */
	PWorld.vx = PWorld.vy = PWorld.vz = 0;
	PLocal1 = PLocal2 = PWorld;
	/* the org point of DWold is set to Z = -40000 :
	   オブジェクトの原点をワールドのZ = -4000に設定 */
	DWorld.coord.t[2] = -4000;
}


/* Set coordinte parameter from work vector :
   ローテションベクタからマトリックスを作成し座標系にセットする */
set_coordinate(pos, coor)
	SVECTOR *pos;		/* work vector : ローテションベクタ */
	GsCOORDINATE2 *coor;	/* Coordinate : 座標系 */
{
	MATRIX  tmp1;

	/* Set translation : 平行移動をセットする */
	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	/* Rotate Matrix : ローテーションベクタから回転マトリックスを作成する */
	RotMatrix(pos, &tmp1);

	/* Set Matrix to Coordinate : 求めたマトリックスを座標系にセットする */
	coor->coord = tmp1;

	/* Clear flag becase of changing parameter :
	   マトリックスキャッシュをフラッシュする */
	coor->flg = 0;
}

/* Load texture to VRAM : テクスチャデータをVRAMにロードする */
texture_init(addr)
	u_long  addr;
{
	RECT    rect1;
	GsIMAGE tim1;

	/* Get texture information of TIM FORMAT :
	   TIMデータのヘッダからテクスチャのデータタイプの情報を得る */
	GsGetTimInfo((u_long *) (addr + 4), &tim1);

	rect1.x = tim1.px;	/* X point of image data on VRAM :
				   テクスチャ左上のVRAMでのX座標 */
	rect1.y = tim1.py;	/* Y point of image data on VRAM :
				   テクスチャ左上のVRAMでのY座標 */
	rect1.w = tim1.pw;	/* Width of image : テクスチャ幅 */
	rect1.h = tim1.ph;	/* Height of image : テクスチャ高さ */

	/* Load texture to VRAM : VRAMにテクスチャをロードする */
	LoadImage(&rect1, tim1.pixel);

	/* Exist Color Lookup Table : カラールックアップテーブルが存在する */
	if ((tim1.pmode >> 3) & 0x01) {
		rect1.x = tim1.cx;	/* X point of CLUT data on VRAM :
					   クラット左上のVRAMでのX座標 */
		rect1.y = tim1.cy;	/* Y point of CLUT data on VRAM :
					   クラット左上のVRAMでのY座標 */
		rect1.w = tim1.cw;	/* Width of CLUT : クラットの幅 */
		rect1.h = tim1.ch;	/* Height of CLUT : クラットの高さ */

		/* Load CULT data to VRAM : VRAMにクラットをロードする */
		LoadImage(&rect1, tim1.clut);
	}
}

model_init()
{				/* set up the modeling data :
				   モデリングデータの読み込み */
	u_long *dop;
	GsDOBJ2 *objp;		/* the handler or modeling data :
				   モデリングデータハンドラ */
	int     i;

	dop = (u_long *) MODEL_ADDR;	/* the top address of modeling data :
					   モデリングデータが格納されているアド
					   レス */
	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* map the modeling data to real address :
				   モデリングデータ（TMDフォーマット）を
				   実アドレスにマップする */
	dop++;
	Objnum = *dop;		/* get the number of objects :
				   オブジェクト数をTMDのヘッダから得る */
	dop++;			/* inc the address to link to the handler :
				   GsLinkObject5でリンクするためにTMDの
				   オブジェクトの先頭にもってくる */

	for (i = 0; i < Objnum; i++)	/* Link TMD data and Object Handler :
					   TMDデータとオブジェクトハンドラを接続する */
		GsLinkObject4((u_long) dop, &object[i], i);

	/* Link TMD data and Object Handler (reuse same modeling with
	   diffrent object handler) :
	   TMDデータとオブジェクトハンドラを接続する（object使い回し） */
	for (i = 0; i < Objnum; i++)
		GsLinkObject4((u_long) dop, &object[i + Objnum], i);

	for (i = 0, objp = object; i < Objnum; i++) {	
		/* default object coordinate : デフォルトのオブジェクトの座標系の設定 */
		objp->coord2 = &DLocal1;
		objp->attribute = 0;	/* init attributes */
		objp++;
	}

	for (i = 0, objp = &object[Objnum]; i < Objnum; i++) {
		/* default object coordinate : デフォルトのオブジェクトの座標系の設定 */
		objp->coord2 = &DLocal2;
		objp->attribute = 0;	/* init attributes */
		objp++;
	}
}

/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *	BG 描画サンプルプログラム
 *
 *		Version 1.10	Jan, 11, 1995
 *
 *		Copyright (C) 1994,1995 by Sony Computer Entertainment
 *			All rights Reserved
 */

#include	<sys/types.h>
#include	<libetc.h>
#include	<libgte.h>
#include	<libgpu.h>
#include	<libgs.h>

/* オーダリングテーブル(OT)の定義 */
#define	OT_LENGTH 4
GsOT WorldOT[2];
GsOT_TAG OTTags[2][1<<OT_LENGTH];

/* GPUパケット領域の定義 */
#define	PACKETMAX 6000*24
PACKET	GpuPacketArea[2][PACKETMAX];

/* テクスチャ情報 */
#define TEX_ADDR 0x80180000		/* テクスチャ先頭アドレス */
#define	TIM_HEADER 0x00000010		/* TIMヘッダの値 */
GsIMAGE TexInfo;			/* TIMデータの情報 */

/*
 *  BG情報
 */
#define BGD_ADDR 0x80100000		/* BGDデータの格納アドレス */
#define	BGD_HEADER 0x23			/* BGDヘッダの値 */
#define CEL_ADDR 0x80140000		/* CELデータの格納アドレス */
#define	CEL_HEADER 0x22			/* CELヘッダの値 */
GsBG	BGData;				/* BG情報 */
GsMAP	BGMap;				/* マップ情報 */

/*
 *  その他...
 */
u_long padd;			/* コントロールパッドデータ */

/*
 *  関数のプロトタイプ宣言
 */
void drawAll();
int  moveCharacter();
void initSystem();
void initTexture();
void initBG();

/*
 *  メインルーチン
 */
main(void)
{
	ResetCallback();
	initSystem();

	padd = 0;

	initTexture();
	initBG();

	while(1) {
		if(moveCharacter()) break;
		drawAll();
	}

	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
}

/*
 *  描画
 */
void drawAll()
{
	int activeBuff;		/* アクティブなバッファへのポインタ */
	int i;
	
	activeBuff = GsGetActiveBuff();
	GsSetWorkBase((PACKET *)GpuPacketArea[activeBuff]);
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* BGのOTへの登録 */
	GsSortBg(&BGData, &WorldOT[activeBuff], 15);

	/* 回転しない場合は高速描画関数が使える */
	/* GsSortFastBg(&BGData, &WorldOT[activeBuff], 15); */

	padd = PadRead(0);

	VSync(0);

	ResetGraph(1);
	GsSwapDispBuff();
	GsSortClear(0, 0, 0, &WorldOT[activeBuff]);
	GsDrawOt(&WorldOT[activeBuff]);
}

/*
 *  コントロールパッドでＢＧを動かす
 */
int moveCharacter()
{
	/* ＢＧサイズの変更 */
	if(((padd & PADRup)>0)&&(BGData.h < 480)) {
		BGData.scrolly -= 1;
		BGData.h += 2;
		BGData.my += 1;
	}
	if(((padd & PADRdown)>0)&&(BGData.h > 2)) {
		BGData.scrolly += 1;
		BGData.h -= 2;
		BGData.my -= 1;
	}
	if(((padd & PADRleft)>0)&&(BGData.w < 640)) {
		BGData.scrollx -= 1;
		BGData.w += 2;
		BGData.mx += 1;
	}
	if(((padd & PADRright)>0)&&(BGData.w > 2)) {
		BGData.scrollx += 1;
		BGData.w -= 2;
		BGData.mx -= 1;
	}

	/* ＢＧ回転角の変更 */
	if((padd & PADl)>0) {
		BGData.rotate += ONE*4;
	}
	if((padd & PADn)>0) {
		BGData.rotate -= ONE*4;
	}

	/* ＢＧ拡大率の変更 */
	if(((padd & PADm)>0)&&(BGData.scalex<28000)) {
		BGData.scalex += BGData.scalex/8;
		BGData.scaley += BGData.scaley/8;
	}
	if(((padd & PADo)>0)&&(BGData.scalex>512)) {
		BGData.scalex -= BGData.scalex/8;
		BGData.scaley -= BGData.scaley/8;
	}

	/* スクロール */
	if((padd & PADLup)>0) {
		BGData.scrolly -= 2;
	}
	if((padd & PADLdown)>0) {
		BGData.scrolly += 2;
	}
	if((padd & PADLleft)>0) {
		BGData.scrollx -= 2;
	}
	if((padd & PADLright)>0) {
		BGData.scrollx += 2;
	}

	/* Kボタンで終了 */
	if((padd & PADk)>0) return -1;

	return 0;
}

/*
 *  システムのイニシャライズ
 */
void initSystem()
{
	int i;

	/* パッドの初期化 */
	PadInit(0);

	/* パッドの初期化 */
	GsInitGraph(320, 240, 0, 0, 0);
	GsDefDispBuff(0, 0, 0, 240);

	/* OTの初期化 */
	for(i = 0; i < 2; i++) {
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}

	/* 画面座標系を3Dと同じにする */	
	GsInit3D();
}

/*
 *  スプライトパターンの読み込み
 *  （複数のTIMデータをVRAMへ転送）
 */
void initTexture()
{
	u_long *timP;

	timP = (u_long *)TEX_ADDR;
	while(1) {
		/* TIMデータがあるかどうか */
		if(*timP != TIM_HEADER)	{
			break;
		}

		/* ヘッダ良みとばし */
		timP++;

		/* TIMデータの位置情報を得る */
		GsGetTimInfo( timP, &TexInfo );

		/* PIXELデータをVRAMへ転送 */
		timP += TexInfo.pw * TexInfo.ph/2+3+1;	/* ポインタを進める */
		LoadImage((RECT *)&TexInfo.px, TexInfo.pixel);
		
		/* CLUTがあればVRAMへ転送 */
		if((TexInfo.pmode>>3)&0x01) {
			LoadImage( (RECT *)&TexInfo.cx, TexInfo.clut );
			timP += TexInfo.cw*TexInfo.ch/2+3;	/* ポインタを進める */
		}
	}
}

/*
 *  BGの初期化
 */
void initBG()
{
        u_char *cel;
        u_char *bgd;
	u_char celflag;
	int ncell;
	u_char bgdflag;

        cel = (u_char *)CEL_ADDR;        
	cel += 3;
	celflag = *cel++;
	ncell = *(u_short *)cel;	
	cel += 4;

       	bgd = (u_char *)BGD_ADDR;
	bgd += 3;
	bgdflag = *bgd++;

	BGMap.ncellw = *bgd++;
	BGMap.ncellh = *bgd++;
	BGMap.cellw = *bgd++;
	BGMap.cellh = *bgd++;
	BGMap.base = (GsCELL *)cel;
	BGMap.index = (u_short *)bgd;

	BGData.attribute = ((TexInfo.pmode&0x03)<<24);
	BGData.x = 0;
	BGData.y = 0;
	BGData.scrollx = BGData.scrolly = 0;
	BGData.r = BGData.g = BGData.b = 128;
	BGData.map = &BGMap;
	BGData.mx = 320/2;
	BGData.my = 224/2;
	BGData.scalex = BGData.scaley = ONE;
	BGData.rotate = 0;
	BGData.w = 320;
	BGData.h = 224;

	cel += ncell*8;
	if(celflag&0xc0 == 0x80) {
		cel += ncell;		/* Skip ATTR (8bit) */
	}
	if(celflag&0xc0 == 0xc0) {
		cel += ncell*2;		/* Skip ATTR (16bit) */
	}

	bgd += BGMap.ncellw*BGMap.ncellh*2;
	if(bgdflag&0xc0 == 0x80) {
		bgd += BGMap.ncellw*BGMap.ncellh;
	}
	if(bgdflag&0xc0 == 0xc0) {
		bgd += BGMap.ncellw*BGMap.ncellh*2;
	}
}

/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *	高速BG描画サンプル
 *		(GsSortFixBG32()使用)
 *
 *		Version 1.00	Mar, 7, 1995
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *			All rights Reserved
 */

#include	<sys/types.h>
#include	<libetc.h>
#include	<libgte.h>
#include	<libgpu.h>
#include	<libgs.h>

/* GPU Info. */
#define	OT_LENGTH 4
GsOT WorldOT[2];
GsOT_TAG OTTags[2][1<<OT_LENGTH];

/* テクスチャ（パターン）情報 */
#define TEX_ADDR 0x80180000	/* テクスチャ先頭アドレス */
#define	TIM_HEADER 0x00000010	/* TIMヘッダの値 */
GsIMAGE TimInfo;

/*
 *  BG情報
 */
#define N_BG 8			/* 用意するBGの枚数 */
GsBG	BGData[N_BG];		/* BG情報 */
GsMAP	BGMap[N_BG];		/* マップ情報 */
#define CEL_ADDR  0x80140000	/* セルデータ先頭アドレス（各面共通） */

/* GsSortFixBg32()のための作業領域の確保（８面分） */
#define BGWSIZE (((320/32+1)*(240/32+1+1)*6+4)*2+2)
u_long BGPacket[BGWSIZE*N_BG];

/* 各面の作業領域へのポインタ */
u_long *BGWork[N_BG];

/* 4面分のマップデータ(BGDファイル) */
#define BGD0_ADDR 0x80100000	/* マップ#0先頭アドレス */
#define BGD1_ADDR 0x80101000	/* マップ#1先頭アドレス */
#define BGD2_ADDR 0x80102000	/* マップ#2先頭アドレス */
#define BGD3_ADDR 0x80103000	/* マップ#3先頭アドレス */
unsigned long *BgdAddr[N_BG] = {
	(unsigned long *)BGD0_ADDR,
	(unsigned long *)BGD1_ADDR,
	(unsigned long *)BGD2_ADDR,
	(unsigned long *)BGD3_ADDR
};

/* BGの各面がどのマップを使うかのテーブル */
int MapIndex[8] = {
	0, 1, 2, 3, 1, 2, 3, 1	
};
int NumBG;			/* 表示中のBG面数 */

/*
 *  その他...
 */
u_long PadData;			/* コントロールパッドデータ */
u_long OldPad;			/* 前のフレームのデータ */

/*
 *  メインルーチン
 */
main(void)
{
	/* システムの初期化 */
	ResetCallback();
	initSystem();

	/* その他の初期化 */
	PadData = 0;
	NumBG = 1;
	initTexture();
	initBG();
	GsInitVcount();

	while(1) {
		if(moveBG()) break;
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
drawAll()
{
	int activeBuff;
	int i;
	
	/* ダブルバッファのどちらがアクティブかを得る */
	activeBuff = GsGetActiveBuff();

	/* OTのクリア */
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* BGの描画 */
	for(i = 0; i < NumBG; i++) {	
		GsSortFixBg32(BGData+i, BGWork[i], &WorldOT[activeBuff], N_BG-i);
	}

	/* パッドの読み込み */
	OldPad = PadData;
	PadData = PadRead(1);

	/* V-Sync 同期 */
	VSync(0);

	ResetGraph(1);
	GsSwapDispBuff();

	GsSortClear(0, 0, 0, &WorldOT[activeBuff]);
	GsDrawOt(&WorldOT[activeBuff]);
}

/*
 *  ＢＧを動かす
 */
moveBG()
{
	int i;

	/* 上矢印 = BG表示枚数減らす */
	if(((PadData&PADLup)>0)&&
		((OldPad&PADLup)==0)&&
		(NumBG < N_BG)) NumBG++;

	/* 下矢印 = BG表示枚数減らす */
	if(((PadData&PADLdown)>0)&&
		((OldPad&PADLdown)==0)&&
		(NumBG > 1)) NumBG--;

	/* BG表示位置 */
	BGData[0].scrolly += 1;
	BGData[1].scrollx -= 1;
	BGData[1].scrolly -= 1;
	BGData[3].scrollx += 1;
	BGData[4].scrollx -= 1;
	BGData[4].scrolly += 2;
	BGData[5].scrollx += 2;
	BGData[6].scrolly -= 1;

	/* Kボタンで終了 */
	if((PadData & PADk)>0) return -1;

	return 0;
}

/*
 *  イニシャライズ
 */
initSystem()
{
	int i;
		
	/* パッドの初期化 */
	PadInit(0);
	
	/* GPUの初期化 */
	GsInitGraph(320,240,0,0,0);
	GsDefDispBuff(0,0,0,240);

	/* OT領域の初期化 */
	WorldOT[0].length=OT_LENGTH;
	WorldOT[0].org=OTTags[0];
	WorldOT[1].length=OT_LENGTH;
	WorldOT[1].org=OTTags[1];
	
	/* 3D環境の初期化 */
	GsInit3D();
}

/*
 *  セルパターン(Texture pattern)の読み込み
 *  （あらかじめメモリ上に置かれたTIMデータをVRAMへ転送）
 */
initTexture()
{
	RECT rect;
	u_long *timP;
	int i;

	/* TIMデータの先頭アドレス */	
	timP = (u_long *)TEX_ADDR;

	/* ヘッダ良みとばし */
	timP++;

	/* TIMデータの位置情報を得る */
	GsGetTimInfo( timP, &TimInfo );

	/* PIXELデータをVRAMへ転送 */
	timP += TimInfo.pw * TimInfo.ph/2+3+1;
	LoadImage((RECT *)&TimInfo.px, TimInfo.pixel);
		
	/* CLUTがあればVRAMへ転送 */
	if((TimInfo.pmode>>3)&0x01) {
		LoadImage( (RECT *)&TimInfo.cx, TimInfo.clut );
		timP += TimInfo.cw*TimInfo.ch/2+3;
	}
}

/*
 *  BGの初期化
 */
initBG()
{
	int i;
	GsCELL *cellP;
        u_char *cel;
        u_char *bgd;
	int ncell;

	/* CELデータからセル情報を得る */
        cel = (u_char *)CEL_ADDR;        
	cel += 4;
	ncell = *(u_short *)cel;	
	cel += 4;

	/* BGの各面のマップとBG構造体を初期化 */
	for(i = 0; i < N_BG; i++) {

		/* BGDデータからマップ情報を得る */
	       	bgd = (u_char *)BgdAddr[MapIndex[i]];
		bgd += 4;

		BGMap[i].ncellw = *bgd++;		/* MAPの大きさ */
		BGMap[i].ncellh = *bgd++;
		BGMap[i].cellw = *bgd++;		/* セルの大きさ */
		BGMap[i].cellh = *bgd++;
		BGMap[i].index = (unsigned short *)bgd; /* マップ本体 */
		BGMap[i].base = (GsCELL *)cel;		/* セル配列へのポインタ */
		
		/* 作業領域(Primitiveエリア)の確保＆初期化 */
		BGWork[i] = BGPacket+i*BGWSIZE;
		GsInitFixBg32(&BGData[i], BGWork[i]);

		/* その他(attribute類) */
		BGData[i].attribute = (0<<24)|GsROTOFF;
		BGData[i].scrollx = BGData[i].scrolly = 0;
		BGData[i].r = BGData[i].g = BGData[i].b = 128;
		BGData[i].map = &(BGMap[i]);
	}
}

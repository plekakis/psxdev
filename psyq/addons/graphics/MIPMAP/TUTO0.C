/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 * tuto0: mip-map sample program
 *
 * 	Copyright (C) 1996 by Sony Computer Entertainment Inc.
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>	
#include <libgpu.h>	
#include <libgs.h>

#define PACKETMAX 2048	
#define OBJECTMAX 1	
#define PACKETMAX2 (PACKETMAX*20)

#define MODEL_ADDR	0x80100000	
#define TEX_ADDR	0x80120000	

#define OT_LENGTH  7		

GsOT    Wot[2]; 
GsOT_TAG zsorttable[2][1 << OT_LENGTH];

GsDOBJ2 object[OBJECTMAX];	
u_long  Objnum;		

GsCOORDINATE2 DWorld;

SVECTOR PWorld;	

GsRVIEW2 view;	
GsF_LIGHT pslt[3];	
u_long  padd;			

PACKET  out_packet[2][PACKETMAX2];

int	ismip;	

extern _GsFCALL GsFCALL4;	/* GsSortObject4J Func Table */
PACKET *GsTMDfastTF4LM();
PACKET *GsTMDfastTF4L();

/************* MAIN START ******************************************/
main()
{
	int     i;
	GsDOBJ2 *op;
	int     outbuf_idx;
	MATRIX  tmpls, tmplw;
	u_long	vcount[2];

	ResetCallback();
	init_all();
	jt_init4();

	while (1) {
		if (obj_interactive() == 0) return 0;	

		GsSetRefView2(&view);	
		outbuf_idx = GsGetActiveBuff();	
		GsSetWorkBase((PACKET *) out_packet[outbuf_idx]);
		GsClearOt(0, 0, &Wot[outbuf_idx]);

		for (i = 0, op = object; i < Objnum; i++) {
			GsGetLws(op->coord2, &tmplw, &tmpls);

			GsSetLightMatrix(&tmplw);
			GsSetLsMatrix(&tmpls);

			GsClearVcount();
			GsSortObject4J(op, &Wot[outbuf_idx], 14 - OT_LENGTH,
					getScratchAddr(0));
			vcount[0] = GsGetVcount();
			op++;
		}

		VSync(0);	
		padd = PadRead(1);	
		GsSwapDispBuff();

		GsSortClear(0x0, 0x0, 0xff, &Wot[outbuf_idx]);

		GsClearVcount();
		GsDrawOt(&Wot[outbuf_idx]);
		DrawSync(0);
		vcount[1] = GsGetVcount();
		if(ismip)
			KanjiFntPrint("(mip-map)  %d %d\n",
					vcount[0], vcount[1]);
		else
			KanjiFntPrint("(original) %d %d\n",
					vcount[0], vcount[1]);
		KanjiFntFlush(-1);
	}
}

obj_interactive()
{
	SVECTOR v1;
	MATRIX  tmp1;

	static u_long oldpad = -1;

	if ((padd & PADRleft) > 0)
		PWorld.vz -= 5 * ONE / 360;
	if ((padd & PADRright) > 0)
		PWorld.vz += 5 * ONE / 360;

	if ((padd & PADLup) > 0)
		PWorld.vx -= 5 * ONE / 360;
	if ((padd & PADLdown) > 0)
		PWorld.vx += 5 * ONE / 360;

	if ((padd & PADLleft) > 0)
		PWorld.vy -= 5 * ONE / 360;
	if ((padd & PADLright) > 0)
		PWorld.vy += 5 * ONE / 360;

	if ((padd & PADL1) > 0) {
		PWorld.vx = 70*ONE/360; PWorld.vy = PWorld.vz = 0;
	}

	if((padd & PADstart)>0){
		if(oldpad != padd) ismip = (ismip?0:1);
		if (ismip)
			GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_NORMAL] = 
				GsTMDfastTF4LM;
		else
			GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_NORMAL] = 
				GsTMDfastTF4L;
	}
	oldpad = padd;

	if ((padd & PADk) > 0) {
		PadStop();
		StopCallback();
		return 0;
	}

	/* オブジェクトのパラメータからマトリックスを計算し座標系にセット */
	set_coordinate(&PWorld, &DWorld);
	return 1;
}

init_all()
{
	u_long *dop;
	GsDOBJ2 *objp;	
	int     i;

	ResetGraph(0);	
	PadInit(0);	

	GsInitVcount();
	padd = 0;
	ismip = 0;

	GsInitGraph(320, 240, GsNONINTER | GsOFSGPU, 1, 0);
	GsDefDispBuff(0, 0, 0, 240);	
	GsInit3D();	

	Wot[0].length = OT_LENGTH;
	Wot[0].org = zsorttable[0];	

	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	/*  座標定義 */
	GsInitCoordinate2(WORLD, &DWorld);
	PWorld.vx = 70*ONE/360; PWorld.vy = PWorld.vz = 0;
	DWorld.coord.t[2] = -400;

	/*  モデリングデータ読み込み */
	dop = (u_long *) MODEL_ADDR;
	dop++;
	GsMapModelingData(dop);
	dop++;
	Objnum = *dop;
	dop++; 
	for (i = 0; i < Objnum; i++)
		GsLinkObject4((u_long) dop, &object[i], i);
	for (i = 0, objp = object; i < Objnum; i++, objp++) {
		objp->coord2 = &DWorld;
		objp->attribute = 0;
	}

	/*  視点設定 */
	GsSetProjection(250);
	view.vpx = 0; view.vpy = 000; view.vpz = 400;
	view.vrx = 0; view.vry = 0; view.vrz = 0;
	view.rz = 0;
	view.super = WORLD;
	GsSetRefView2(&view);

	/*  平行光源設定 */
	pslt[0].vx = 0; pslt[0].vy = 0; pslt[0].vz = -100;
	pslt[0].r = 0xf0; pslt[0].g = 0xf0; pslt[0].b = 0xf0;
	GsSetFlatLight(0, &pslt[0]);

	pslt[1].vx = 0; pslt[1].vy = 100; pslt[1].vz = -100;
	pslt[1].r = 0xf0; pslt[1].g = 0xf0; pslt[1].b = 0xf0;
	GsSetFlatLight(1, &pslt[1]);

	pslt[2].vx = 0; pslt[2].vy = 100; pslt[2].vz = 0;
	pslt[2].r = 0x60; pslt[2].g = 0x60; pslt[2].b = 0x60;
	GsSetFlatLight(2, &pslt[2]);

	GsSetAmbient(0, 0, 0);
	GsSetLightMode(0);

	/* テクスチャデータの読み込み */
	texture_init(TEX_ADDR);

	/* デバッグプリントの準備 */
	KanjiFntOpen(-150,-100,300,200,896,0,768,256,0,512);
}

set_coordinate(pos, coor)
	SVECTOR *pos;		/* ローテションベクタ */
	GsCOORDINATE2 *coor;	/* 座標系 */
{
	MATRIX  tmp1;

	/* 平行移動をセットする */
	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	/* マトリックスにローテーションベクタを作用させる */
	RotMatrix(pos, &tmp1);

	/* 求めたマトリックスを座標系にセットする */
	coor->coord = tmp1;

	/* マトリックスキャッシュをフラッシュする */
	coor->flg = 0;
}

texture_init(addr)
	u_long  addr;
{
	RECT    rect1;
	GsIMAGE tim1;

	GsGetTimInfo((u_long *) (addr + 4), &tim1);

	rect1.x = tim1.px;	rect1.y = tim1.py;
	rect1.w = tim1.pw; rect1.h = tim1.ph;

	LoadImage(&rect1, tim1.pixel);

	if ((tim1.pmode >> 3) & 0x01) {
		rect1.x = tim1.cx; rect1.y = tim1.cy;
		rect1.w = tim1.cw; rect1.h = tim1.ch;	

		LoadImage(&rect1, tim1.clut);
	}
}

/* hook only functions to use*/
jt_init4()
{
	/* texture flat quad */
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTF4L;
}

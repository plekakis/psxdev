/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			tmdview4: GsDOBJ4 object viewing rotine 
 *
 * "tuto7.c" ******** simple GsDOBJ4 Viewing routine using jump table 
 * 							(material attenuation supported) 
 * 
 * Version 1.00	Jul,  14, 1994 
 * 
 * Copyright (C) 1993 by Sony Computer Entertainment All rights Reserved 
 */

#include <sys/types.h>

#include <libetc.h>		/* for Control Pad */
#include <libgte.h>		/* LIBGS uses libgte */
#include <libgpu.h>		/* LIBGS uses libgpu */
#include <libgs.h>		/* for LIBGS */

#define PACKETMAX 10000		/* Max GPU packets */

#define OBJECTMAX 100		/* Max Objects */

#define PACKETMAX2 (PACKETMAX*24)	/* size of PACKETMAX (byte) paket
					   size may be 24 byte(6 word) */

#define MODEL_ADDR 0x80040000	/* Top Address of modeling data (TMD FORMAT) */

#define OT_LENGTH  10		/* bit length of OT */


GsOT    Wot[2];			/* Handler of OT Uses 2 Hander for Dowble buffer */

GsOT_TAG zsorttable[2][1 << OT_LENGTH];	/* Area of OT */

GsDOBJ2 object[OBJECTMAX];	/* Array of Object Handler */

u_long  Objnum;			/* valibable of number of Objects */


GsCOORDINATE2 DWorld;		/* Coordinate for GsDOBJ2 */

SVECTOR PWorld;			/* work short vector for making Coordinate parmeter */

GsRVIEW2 view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting
				   Handler*/
u_long  padd;			/* Controler data */

PACKET  out_packet[2][PACKETMAX2];	/* GPU PACKETS AREA */

/************* MAIN START ******************************************/
main()
{
	int     i;
	GsDOBJ2 *op;		/* pointer of Object Handler */
	int     outbuf_idx;	/* double buffer index */
	MATRIX  tmpls;

	ResetCallback();

	init_all();

	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 256, 200, 0, 512));

	while (1) {
		FntPrint("z = %d\n", DWorld.coord.t[2]);
		if (obj_interactive() == 0)
			return 0;	/* interactive parameter get */
		GsSetRefView2(&view);	/* Calculate World-Screen Matrix */
		outbuf_idx = GsGetActiveBuff();	/* Get double buffer index */

		/* Set top address of Packet Area for output of GPU PACKETS */
		GsSetWorkBase((PACKET *) out_packet[outbuf_idx]);

		GsClearOt(0, 0, &Wot[outbuf_idx]);	/* Clear OT for using buffer */

		for (i = 0, op = object; i < Objnum; i++) {
			/* Calculate Local-World Matrix */
			GsGetLw(op->coord2, &tmpls);

			/* Set LWMATRIX to GTE Lighting Registers */
			GsSetLightMatrix(&tmpls);

			/* Calculate Local-Screen Matrix */
			GsGetLs(op->coord2, &tmpls);

			/* Set LSAMTRIX to GTE Registers */
			GsSetLsMatrix(&tmpls);

			/* Perspective Translate Object and Set OT */
			GsSortObject4J(op, &Wot[outbuf_idx], 14 - OT_LENGTH, getScratchAddr(0));
			op++;
		}
		VSync(0);	/* Wait VSYNC */
		DrawSync(0);
		padd = PadRead(1);	/* Readint Control Pad data */
		GsSwapDispBuff();	/* Swap double buffer */

		/* Set SCREEN CLESR PACKET to top of OT */
		GsSortClear(0x0, 0x0, 0x0, &Wot[outbuf_idx]);

		/* Drawing OT */
		GsDrawOt(&Wot[outbuf_idx]);
		FntFlush(-1);
	}
}


obj_interactive()
{
	SVECTOR v1;
	MATRIX  tmp1;

	/* Rotate Y  */
	if ((padd & PADRleft) > 0)
		PWorld.vy -= 5 * ONE / 360;

	/* Rotate Y */
	if ((padd & PADRright) > 0)
		PWorld.vy += 5 * ONE / 360;

	/* Rotate X */
	if ((padd & PADRup) > 0)
		PWorld.vx += 5 * ONE / 360;

	/* Rotate X */
	if ((padd & PADRdown) > 0)
		PWorld.vx -= 5 * ONE / 360;

	/* Translate Z */
	if ((padd & PADm) > 0)
		DWorld.coord.t[2] -= 100;

	/* Translate Z */
	if ((padd & PADl) > 0)
		DWorld.coord.t[2] += 100;

	/* Translate X */
	/* if((padd & PADLleft)>0) DWorld.coord.t[0] +=10; */
	if ((padd & PADLleft) > 0)
		view.vrx += 10;

	/* Translate X */
	/* if((padd & PADLright)>0) DWorld.coord.t[0] -=10; */
	if ((padd & PADLright) > 0)
		view.vrx -= 10;

	/* Translate Y */
	/* if((padd & PADLdown)>0) DWorld.coord.t[1]+=10; */
	if ((padd & PADLdown) > 0)
		view.vry += 10;

	/* Translate Y */
	/* if((padd & PADLup)>0) DWorld.coord.t[1]-=10; */
	if ((padd & PADLup) > 0)
		view.vry -= 10;

	/* exit program */
	if ((padd & PADk) > 0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	/* Calculate Matrix from Object Parameter and Set Coordinate */
	set_coordinate(&PWorld, &DWorld);
	return 1;
}


/* Initialize routine */
init_all()
{
	GsFOGPARAM fgp;
	ResetGraph(0);		/* Reset GPU */
	PadInit(0);		/* Reset Controller */
	padd = 0;		/* init value of controller */

#if 0
	GsInitGraph(640, 480, GsINTER | GsOFSGPU, 1, 0);
	/* rezolution set , interrace mode */

	GsDefDispBuff(0, 0, 0, 0);/* Double buffer setting */
#endif

	GsInitGraph(640, 240, GsINTER | GsOFSGPU, 0, 0);
	/* rezolution set , non interrace mode */
	GsDefDispBuff(0, 0, 0, 240);/* Double buffer setting */


	GsInit3D();		/* Init 3D system */

	Wot[0].length = OT_LENGTH;	/* Set bit length of OT handler */

	Wot[0].org = zsorttable[0];	/* Set Top address of OT Area to OT handler */

	/* same setting for anoter OT handler */
	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	coord_init();		/* Init coordinate */
	model_init();		/* Reading modeling data */
	view_init();		/* Setting view point */
	light_init();		/* Setting Flat Light */

	/* setting FOG parameters */
	fgp.dqa = -10000 * ONE / 64 / 1000;
	fgp.dqb = 5 / 4 * ONE * ONE;
	fgp.rfc = fgp.gfc = fgp.bfc = 0;
	GsSetFogParam(&fgp);

	/* setting jumptable for GsSortObject4J() */
	jt_init4();
}


view_init()
{				/* Setting view point */
	/*---- Set projection,view ----*/
	GsSetProjection(1000);	/* Set projection */

	/* Setting view point location */
	view.vpx = 0;
	view.vpy = 0;
	view.vpz = 2000;

	/* Setting focus point location */
	view.vrx = 0;
	view.vry = 0;
	view.vrz = 0;

	/* Setting bank of SCREEN */
	view.rz = 0;

	/* Setting parent of viewing coordinate */
	view.super = WORLD;

	/* Calculate World-Screen Matrix from viewing paramter */
	GsSetRefView2(&view);
}


light_init()
{				/* init Flat light */
	/* Setting Light ID 0 */
	/* Setting direction vector of Light0 */
	pslt[0].vx = 30;
	pslt[0].vy = 0;
	pslt[0].vz = -100;

	/* Setting color of Light0 */
	pslt[0].r = 0xf0;
	pslt[0].g = 0;
	pslt[0].b = 0;

	/* Set Light0 from parameters */
	GsSetFlatLight(0, &pslt[0]);


	/* Setting Light ID 1 */
	pslt[1].vx = 0;
	pslt[1].vy = 30;
	pslt[1].vz = -100;
	pslt[1].r = 0;
	pslt[1].g = 0xf0;
	pslt[1].b = 0;
	GsSetFlatLight(1, &pslt[1]);

	/* Setting Light ID 2 */
	pslt[2].vx = -30;
	pslt[2].vy = 0;
	pslt[2].vz = -100;
	pslt[2].r = 0;
	pslt[2].g = 0;
	pslt[2].b = 0xf0;
	GsSetFlatLight(2, &pslt[2]);

	/* Setting Ambient */
	GsSetAmbient(0, 0, 0);

	/* Setting default light mode */
	GsSetLightMode(0);
}

coord_init()
{				/* Setting coordinate */
	/* Setting hierarchy of Coordinate */
	GsInitCoordinate2(WORLD, &DWorld);

	/* Init work vector */
	PWorld.vx = PWorld.vy = PWorld.vz = 0;

	/* the org point of DWold is set to Z = -4000 */
	DWorld.coord.t[2] = -4000;
}

/* Set coordinte parameter from work vector */
set_coordinate(pos, coor)
	SVECTOR *pos;		/* work vector */
	GsCOORDINATE2 *coor;	/* Coordinate */
{
	MATRIX  tmp1;

	/* Set translation */
	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	/* Rotate Matrix */
	RotMatrix(pos, &tmp1);

	/* Set Matrix to Coordinate */
	coor->coord = tmp1;

	/* Clear flag becase of changing parameter */
	coor->flg = 0;
}


/* Load texture to VRAM */
texture_init(addr)
	u_long  addr;
{
	RECT    rect1;
	GsIMAGE tim1;

	/* Get texture information of TIM FORMAT */
	GsGetTimInfo((u_long *) (addr + 4), &tim1);

	rect1.x = tim1.px;	/* X point of image data on VRAM */
	rect1.y = tim1.py;	/* Y point of image data on VRAM */
	rect1.w = tim1.pw;	/* Width of image */
	rect1.h = tim1.ph;	/* Height of image */

	/* Load texture to VRAM */
	LoadImage(&rect1, tim1.pixel);

	/* Exist Color Lookup Table */
	if ((tim1.pmode >> 3) & 0x01) {
		rect1.x = tim1.cx;	/* X point of CLUT data on VRAM */
		rect1.y = tim1.cy;	/* Y point of CLUT data on VRAM */
		rect1.w = tim1.cw;	/* Width of CLUT */
		rect1.h = tim1.ch;	/* Height of CLUT */

		/* Load CULT data to VRAM */
		LoadImage(&rect1, tim1.clut);
	}
}


/* Read modeling data (TMD FORMAT) */
model_init()
{
	u_long *dop;
	GsDOBJ2 *objp;		/* handler of object */
	int     i;

	dop = (u_long *) MODEL_ADDR;/* Top Address of MODELING DATA(TMD FORMAT) */

	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* Mapping real address */

	dop++;
	Objnum = *dop;		/* Get number of Objects */

	dop++;			/* Adjusting for GsLinkObject4J */

	/* Link ObjectHandler and TMD FORMAT MODELING DATA */
	for (i = 0; i < Objnum; i++)
		GsLinkObject4((u_long) dop, &object[i], i);

	for (i = 0, objp = object; i < Objnum; i++) {
		/* Set Coordinate of Object Handler */
		objp->coord2 = &DWorld;

		/* material attenuation setting */
		objp->attribute =  GsLLMOD | GsMATE | GsLDIM4;

		objp++;
	}
}

extern _GsFCALL GsFCALL4;	/* GsSortObject4J Func Table */
jt_init4()
{				/* Gs SortObject4J Fook Func (for material
				   attenuation) */
	PACKET *GsTMDfastF3NL(), *GsTMDfastF3MFG(), *GsTMDfastM3L(), *GsTMDfastNF3();
	PACKET *GsTMDdivF3NL(), *GsTMDdivF3LFG(), *GsTMDdivF3L(), *GsTMDdivNF3();
	PACKET *GsTMDfastG3NL(), *GsTMDfastG3MFG(), *GsTMDfastG3M(), *GsTMDfastNG3();
	PACKET *GsTMDdivG3NL(), *GsTMDdivG3LFG(), *GsTMDdivG3L(), *GsTMDdivNG3();
	PACKET *GsTMDfastTF3NL(), *GsTMDfastTF3MFG(), *GsTMDfastTF3M(), *GsTMDfastTNF3();
	PACKET *GsTMDdivTF3NL(), *GsTMDdivTF3LFG(), *GsTMDdivTF3L(), *GsTMDdivTNF3();
	PACKET *GsTMDfastTG3NL(), *GsTMDfastTG3MFG(), *GsTMDfastTG3M(), *GsTMDfastTNG3();
	PACKET *GsTMDdivTG3NL(), *GsTMDdivTG3LFG(), *GsTMDdivTG3L(), *GsTMDdivTNG3();
	PACKET *GsTMDfastF4NL(), *GsTMDfastF4MFG(), *GsTMDfastF4M(), *GsTMDfastNF4();
	PACKET *GsTMDdivF4NL(), *GsTMDdivF4LFG(), *GsTMDdivF4L(), *GsTMDdivNF4();
	PACKET *GsTMDfastG4NL(), *GsTMDfastG4MFG(), *GsTMDfastG4M(), *GsTMDfastNG4();
	PACKET *GsTMDdivG4NL(), *GsTMDdivG4LFG(), *GsTMDdivG4L(), *GsTMDdivNG4();
	PACKET *GsTMDfastTF4NL(), *GsTMDfastTF4MFG(), *GsTMDfastTF4M(), *GsTMDfastTNF4();
	PACKET *GsTMDdivTF4NL(), *GsTMDdivTF4LFG(), *GsTMDdivTF4L(), *GsTMDdivTNF4();
	PACKET *GsTMDfastTG4NL(), *GsTMDfastTG4MFG(), *GsTMDfastTG4M(), *GsTMDfastTNG4();
	PACKET *GsTMDdivTG4NL(), *GsTMDdivTG4LFG(), *GsTMDdivTG4L(), *GsTMDdivTNG4();
	PACKET *GsTMDfastF3GNL(), *GsTMDfastF3GLFG(), *GsTMDfastF3GL();
	PACKET *GsTMDfastG3GNL(), *GsTMDfastG3GLFG(), *GsTMDfastG3GL();

	/* flat triangle */
	GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastF3M;
	GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastF3MFG;
	GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastF3NL;
	GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivF3L;
	GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivF3LFG;
	GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivF3NL;
	GsFCALL4.nf3[GsDivMODE_NDIV] = GsTMDfastNF3;
	GsFCALL4.nf3[GsDivMODE_DIV] = GsTMDdivNF3;
	/* gour triangle */
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastG3M;
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastG3MFG;
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastG3NL;
	GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivG3L;
	GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivG3LFG;
	GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivG3NL;
	GsFCALL4.ng3[GsDivMODE_NDIV] = GsTMDfastNG3;
	GsFCALL4.ng3[GsDivMODE_DIV] = GsTMDdivNG3;
	/* texture flat triangle */
	GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTF3M;
	GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTF3MFG;
	GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTF3NL;
	GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTF3L;
	GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTF3LFG;
	GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTF3NL;
	GsFCALL4.ntf3[GsDivMODE_NDIV] = GsTMDfastTNF3;
	GsFCALL4.ntf3[GsDivMODE_DIV] = GsTMDdivTNF3;
	/* texture gour triangle */
	GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTG3M;
	GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTG3MFG;
	GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTG3NL;
	GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTG3L;
	GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTG3LFG;
	GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTG3NL;
	GsFCALL4.ntg3[GsDivMODE_NDIV] = GsTMDfastTNG3;
	GsFCALL4.ntg3[GsDivMODE_DIV] = GsTMDdivTNG3;
	/* flat quad */
	GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastF4M;
	GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastF4MFG;
	GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastF4NL;
	GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivF4L;
	GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivF4LFG;
	GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivF4NL;
	GsFCALL4.nf4[GsDivMODE_NDIV] = GsTMDfastNF4;
	GsFCALL4.nf4[GsDivMODE_DIV] = GsTMDdivNF4;
	/* gour quad */
	GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastG4M;
	GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastG4MFG;
	GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastG4NL;
	GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivG4L;
	GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivG4LFG;
	GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivG4NL;
	GsFCALL4.ng4[GsDivMODE_NDIV] = GsTMDfastNG4;
	GsFCALL4.ng4[GsDivMODE_DIV] = GsTMDdivNG4;
	/* texture flat quad */
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTF4M;
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTF4MFG;
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTF4NL;
	GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTF4L;
	GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTF4LFG;
	GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTF4NL;
	GsFCALL4.ntf4[GsDivMODE_NDIV] = GsTMDfastTNF4;
	GsFCALL4.ntf4[GsDivMODE_DIV] = GsTMDdivTNF4;
	/* texture gour quad */
	GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTG4M;
	GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTG4MFG;
	GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTG4NL;
	GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTG4L;
	GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTG4LFG;
	GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTG4NL;
	GsFCALL4.ntg4[GsDivMODE_NDIV] = GsTMDfastTNG4;
	GsFCALL4.ntg4[GsDivMODE_DIV] = GsTMDdivTNG4;
	/* gradation  triangle */
	GsFCALL4.f3g[GsLMODE_NORMAL] = GsTMDfastF3GL;
	GsFCALL4.f3g[GsLMODE_FOG] = GsTMDfastF3GLFG;
	GsFCALL4.f3g[GsLMODE_LOFF] = GsTMDfastF3GNL;
	GsFCALL4.g3g[GsLMODE_NORMAL] = GsTMDfastG3GL;
	GsFCALL4.g3g[GsLMODE_FOG] = GsTMDfastG3GLFG;
	GsFCALL4.g3g[GsLMODE_LOFF] = GsTMDfastG3GNL;
}


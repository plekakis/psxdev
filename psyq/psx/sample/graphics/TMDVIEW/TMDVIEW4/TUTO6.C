/* $PSLibId: Run-time Library Release 4.4$ */
/* tmdview4: GsDOBJ2 object viewing rotine 
 * 
 * "tuto6.c" ******** GsDOBJ2 Viewing routine (cocpit view) 
 * 
 * Version 1.00	Jul,  14, 1995 
 * 
 * Copyright (C) 1995 by Sony Computer Entertainment All rights Reserved 
 */

#include <sys/types.h>

#include <libetc.h>		/* needs to be included to use PAD */
#include <libgte.h>		/* LIBGS uses libgte */
#include <libgpu.h>		/* needs to be included to use LIBGS */
#include <libgs.h>		/* for LIBGS */

#define OBJECTMAX 100		/* Max Objects 
				    defines the maximum number of logical objects
a 3D model can be divided into*/

#define TEX_ADDR   0x80010000	/* Top Address of texture data1 (TIM FORMAT) */

#define TEX_ADDR1   0x80020000	/* Top Address of texture data1 (TIM FORMAT) */
#define TEX_ADDR2   0x80030000	/* Top Address of texture data2 (TIM FORMAT) */

#define MODEL_ADDR 0x80040000	/* Top Address of modeling data (TMD FORMAT) */

#define OT_LENGTH  12		/* bit length of OT */


GsOT    Wot[2];			/* OT handler for double buffers */

GsOT_TAG zsorttable[2][1 << OT_LENGTH];	/* substance of OT */

GsDOBJ2 object[OBJECTMAX];	/* Array of Object Handler */

u_long  Objnum;			/* valibable of number of Objects */

GsCOORDINATE2 DWorld;	/* Coordinate for GsDOBJ2*/

GsCOORDINATE2 DView;		/* Coordinate for View */

SVECTOR PWorld;			/* work short vector for making Coordinate parameter */

GsRVIEW2 view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler */
u_long  padd;			/* Controler data */

/* work area of PACKET DATA used double size for packet double buffer */
u_long  out_packet[2][0x10000];

/************* MAIN START ******************************************/
main()
{
	int     i;
	GsDOBJ2 *op;		/* the pointer of Object handler */
	int     outbuf_idx;
	MATRIX  tmpls, tmplw;

	ResetCallback();
	init_all();

	while (1) {
		if (obj_interactive() == 0)
			return 0;	/* interactive parameter get */
		GsSetRefView2(&view);	/* caliculate World/Screen matrix */

		outbuf_idx = GsGetActiveBuff();	/* get the double buffer index */

		GsSetWorkBase((PACKET *)out_packet[outbuf_idx]);
		GsClearOt(0, 0, &Wot[outbuf_idx]);	/* clear ordering table */

		for (i = 0, op = object; i < Objnum; i++) {
			/* Calculate Local-World Matrix & Local-Screen Matrix */
			GsGetLws(op->coord2, &tmplw, &tmpls);
			/* Set LWMATRIX to GTE Lighting Registers */
			GsSetLightMatrix(&tmplw);
			/* Set LSAMTRIX to GTE Registers */
			GsSetLsMatrix(&tmpls);
			/* Perspective Translate Object and Set OT */
			GsSortObject4(op, &Wot[outbuf_idx], 14 - OT_LENGTH, getScratchAddr(0));
			op++;
		}
		padd = PadRead(1);	/* Readint Control Pad data */
		VSync(0);	/* Wait VSYNC */
		ResetGraph(1);	/* Reset GPU */
		GsSwapDispBuff();	/* Swap double buffer */
		SetDispMask(1);

		/* Set SCREEN CLESR PACKET to top of OT */
		GsSortClear(0x0, 0x0, 0x0, &Wot[outbuf_idx]);

		/* Start Drawing */
		GsDrawOt(&Wot[outbuf_idx]);
	}
}

obj_interactive()
{
	SVECTOR dd;
	SVECTOR ax;		/* work short vector for making Coordinate parmeter */
	MATRIX  tmp1;
	VECTOR  tmpv;
	static int count = 0;

	count++;

	dd.vx = dd.vy = dd.vz = 0;
	ax.vx = ax.vy = ax.vz = 0;

	/* Rotate Y */
	if ((padd & PADRleft) > 0)
		ax.vy -= 1 * ONE / 360;

	/* Rotate Y */
	if ((padd & PADRright) > 0)
		ax.vy += 1 * ONE / 360;

	/* Rotate X */
	if ((padd & PADRup) > 0)
		ax.vx += 1 * ONE / 360;

	/* Rotate X */
	if ((padd & PADRdown) > 0)
		ax.vx -= 1 * ONE / 360;

	/* Rotate Z */
	if ((padd & PADo) > 0)
		ax.vz += 1 * ONE / 360 / 2;

	/* Rotate Z */
	if ((padd & PADn) > 0)
		ax.vz -= 1 * ONE / 360 / 2;

	/* Translate Z */
	if ((padd & PADm) > 0)
		dd.vz = 100;

	/* Translate Z */
	if ((padd & PADl) > 0)
		dd.vz = -100;

	/* Translate X */
	if ((padd & PADLright) > 0)
		dd.vx = 20;

	/* Translate X */
	if ((padd & PADLleft) > 0)
		dd.vx = -20;

	/* Translate Y */
	if ((padd & PADLdown) > 0)
		dd.vy = 20;

	/* Translate Y */
	if ((padd & PADLup) > 0)
		dd.vy = -20;

	/* convert subjective displacement to World displacement*/
	ApplyMatrix(&DView.coord, &dd, &tmpv);
	DView.coord.t[0] += tmpv.vx;
	DView.coord.t[1] += tmpv.vy;
	DView.coord.t[2] += tmpv.vz;

	/* exit program */
	if ((padd & PADk) > 0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	/* Calculate Matrix from Object Parameter and Set Coordinate */
	set_shukan_coordinate(&ax, &DView);
	return 1;
}

/* Set coordinte parameter from work vector */
set_shukan_coordinate(pos, coor)
	SVECTOR *pos;		/* work vector */
	GsCOORDINATE2 *coor;	/* Coordinate */
{
	MATRIX  tmp1;

	/* Clear flag becase of changing parameter */
	coor->flg = 0;

	/* exit without doing anything if no rotation*/
	if (pos->vx == 0 && pos->vy == 0 && pos->vz == 0)
		return;

	/* Rotate Matrix */
	RotMatrix(pos, &tmp1);

	/* apply rotation matrix*/
	MulMatrix(&coor->coord, &tmp1);

	/* correct for deformation resulting from accumulation of errors*/
	MatrixNormal(&coor->coord, &coor->coord);
}

init_all()
{				/* initialize rotines */
	ResetGraph(0);		/* reset GPU */
	PadInit(0);		/* init controler */
	padd = 0;		/* init contorler value */

#if 0
	GsInitGraph(640, 480, GsINTER | GsOFSGPU, 1, 0);
	/* set the resolution of screen (interrace mode) */

	GsDefDispBuff(0, 0, 0, 0);	/* set the double buffers */
#endif
	GsInitGraph(640, 240, GsNONINTER | GsOFSGPU, 1, 0);
	/* set the resolution of screen (on interrace mode) */
	GsDefDispBuff(0, 0, 0, 240);	/* set the double buffers */
	GsInit3D();		/* init 3d part of libgs */

	Wot[0].length = OT_LENGTH;	/* set the length of OT1 */
	Wot[0].org = zsorttable[0];	/* set the top address of OT1 tags */
	/* set anoter OT for double buffer */
	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	coord_init();		/* initialize the coordinate system */
	model_init();		/* set up the modeling data */
	view_init();		/* set the viewpoint */
	light_init();		/* set the flat light */

	texture_init(TEX_ADDR);	/* 16bit texture load */
	texture_init(TEX_ADDR1);/* 8bit  texture load */
	texture_init(TEX_ADDR2);/* 4bit  texture load */
}



view_init()
{				/* set the viewpoint */
	/*---- Set projection,view ----*/
	GsSetProjection(1000);	/* set the projection */

	/* set the viewpoint parameter */
	view.vpx = 0;
	view.vpy = 0;
	view.vpz = 2000;
	/* set the refarence point parameter */
	view.vrx = 0;
	view.vry = 0;
	view.vrz = -4000;
	/* set the roll pameter of viewpoint */
	view.rz = 0;
	GsInitCoordinate2(WORLD, &DView);	/* init the view coordinate */
	view.super = &DView;	/* set the view coordinte */

	/* set the view point from parameters (libgs caliculate World-Screen
	   Matrix) */
	GsSetRefView2(&view);
}


light_init()
{				/* init Flat light */
	/* Setting Light ID 0 */
	/* Setting direction vector of Light0 */
	pslt[0].vx = 20;
	pslt[0].vy = -100;
	pslt[0].vz = -100;

	/* Setting color of Light0 */
	pslt[0].r = 0xd0;
	pslt[0].g = 0xd0;
	pslt[0].b = 0xd0;

	/* Set Light0 from parameters */
	GsSetFlatLight(0, &pslt[0]);

	/* Setting Light ID 1 */
	pslt[1].vx = 20;
	pslt[1].vy = -50;
	pslt[1].vz = 100;
	pslt[1].r = 0x80;
	pslt[1].g = 0x80;
	pslt[1].b = 0x80;
	GsSetFlatLight(1, &pslt[1]);

	/* Setting Light ID 2 */
	pslt[2].vx = -20;
	pslt[2].vy = 20;
	pslt[2].vz = -100;
	pslt[2].r = 0x60;
	pslt[2].g = 0x60;
	pslt[2].b = 0x60;
	GsSetFlatLight(2, &pslt[2]);

	/* Setting Ambient */
	GsSetAmbient(ONE / 4, ONE / 4, ONE / 4);

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


model_init()
{				/* set up the modeling data */
	u_long *dop;
	GsDOBJ2 *objp;		/* the handler or modeling data */
	int     i;

	dop = (u_long *) MODEL_ADDR;	/* the top address of modeling data */
	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* map the modeling data to real address */
	dop++;
	Objnum = *dop;		/* get the number of objects */
	dop++;			/* inc the address to link to the handler */

	for (i = 0; i < Objnum; i++)	/* Link TMD data and Object Handler */
		GsLinkObject4((u_long) dop, &object[i], i);

	for (i = 0, objp = object; i < Objnum; i++) {	
	/* default object coordinate */
		objp->coord2 = &DWorld;
		objp->attribute = 0;	/* attribute init */
		objp++;
	}
}

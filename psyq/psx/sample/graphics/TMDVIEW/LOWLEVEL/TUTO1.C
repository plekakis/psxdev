/* $PSLibId: Run-time Library Release 4.4$ */
/*
 * low_level: GsDOBJ4 object viewing rotine with low_level access 
 *
 * "tuto1.c" ******** simple GsDOBJ4 Viewing routine using GsTMDfast only for
 * gouraud polygon model 
 *
 * Version 1.00	Nov,  12, 1995 
 *
 * Copyright (C) 1995 by Sony Computer Entertainment All rights Reserved 
 */

#include <sys/types.h>

#include <libetc.h>		/* for Control Pad */
#include <libgte.h>		/* LIBGS uses libgte */
#include <libgpu.h>		/* LIBGS uses libgpu */
#include <libgs.h>		/* for LIBGS */

#define PACKETMAX 1000		/* Max GPU packets */

#define OBJECTMAX 100		/* Max Objects */

#define PACKETMAX2 (PACKETMAX*24)	/* size of PACKETMAX (byte) 
										packet size may be 24 byte(6 word) */

#define TEX_ADDR   0x80120000	/* Top Address of texture data1 (TIM FORMAT)
				 * */

#define TEX_ADDR1   0x80140000	/* Top Address of texture data1 (TIM FORMAT) */
#define TEX_ADDR2   0x80160000	/* Top Address of texture data2 (TIM FORMAT) */


#define MODEL_ADDR 0x80100000	/* Top Address of modeling data (TMD FORMAT)
				 * */

#define OT_LENGTH  7		/* bit length of OT */


GsOT    Wot[2];			/* Handler of OT Uses 2 Hander for Double
				 * buffer */

GsOT_TAG zsorttable[2][1 << OT_LENGTH];	/* Area of OT */

GsDOBJ2 object[OBJECTMAX];	/* Array of Object Handler */

u_long  Objnum;			/* variable of number of Objects */


GsCOORDINATE2 DWorld;		/* Coordinate for GsDOBJ2 */

SVECTOR PWorld;			/* work short vector for making Coordinate
				 * parmeter */

extern MATRIX GsIDMATRIX;	/* Unit Matrix */

GsRVIEW2 view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler */
u_long  padd;			/* Controler data */

PACKET  out_packet[2][PACKETMAX2];	/* GPU PACKETS AREA */

/************* MAIN START ******************************************/
main()
{
	int     i;
	GsDOBJ2 *op;		/* pointer of Object Handler */
	int     outbuf_idx;	/* double buffer index */
	MATRIX  tmpls, tmplw;
	int     vcount;
	int     p;

	ResetCallback();
	GsInitVcount();

	init_all();
	GsSetFarClip(1000);

	FntLoad(960, 256);
	SetDumpFnt(FntOpen(0, -64, 256, 200, 0, 512));

	while (1) {
		if (obj_interactive() == 0)
			return 0;	/* interactive parameter get */
		GsSetRefView2(&view);	/* Calculate World-Screen Matrix */
		outbuf_idx = GsGetActiveBuff();	/* Get double buffer index */

		/* Set top address of Packet Area for output of GPU PACKETS */
		GsSetWorkBase((PACKET *) out_packet[outbuf_idx]);

		GsClearOt(0, 0, &Wot[outbuf_idx]);	/* Clear OT for using
							 * buffer */

		for (i = 0, op = object; i < Objnum; i++) {
			/*
			 * Calculate Local-World Matrix */
			GsGetLws(op->coord2, &tmplw, &tmpls);

			/*
			 * Set LWMATRIX to GTE Lighting Registers */
			GsSetLightMatrix(&tmplw);

			/*
			 * Set LSAMTRIX to GTE Registers */

			GsSetLsMatrix(&tmpls);

			/*
			 * Perspective Translate Object and Set OT */
			SortTMDobject(op, &Wot[outbuf_idx], 14 - OT_LENGTH);
			op++;
		}

		/* printf("OUT_PACKET_P = %x\n",GsOUT_PACKET_P); */

		DrawSync(0);
		VSync(0);	/* Wait VSYNC */
		/* ResetGraph(1); */
		padd = PadRead(1);	/* Readint Control Pad data */
		GsSwapDispBuff();	/* Swap double buffer */

		/*
		 * Set SCREEN CLESR PACKET to top of OT */
		GsSortClear(0x0, 0x0, 0x0, &Wot[outbuf_idx]);

		/*
		 * Drawing OT */
		GsDrawOt(&Wot[outbuf_idx]);
		FntFlush(-1);
	}
}

obj_interactive()
{
	SVECTOR v1;
	MATRIX  tmp1;

	/* Rotate Y */
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
	/* Translate Z */
	if ((padd & PADLleft) > 0)
		DWorld.coord.t[0] += 10;
	/* Translate Z */
	if ((padd & PADLright) > 0)
		DWorld.coord.t[0] -= 10;
	/* Translate Z */
	if ((padd & PADLdown) > 0)
		DWorld.coord.t[1] += 10;
	/* Translate Z */
	if ((padd & PADLup) > 0)
		DWorld.coord.t[1] -= 10;
	/*
	 * exit program */
	if ((padd & PADk) > 0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	/* Set the matrix of Coordinate 2 */
	set_coordinate(&PWorld, &DWorld);
	return 1;
}

/*
 * Initialize routine */
init_all()
{
	GsFOGPARAM  fgp;

	ResetGraph(0);		/* Reset GPU */
	PadInit(0);		/* Reset Controller */
	padd = 0;		/* init controller value*/

	GsInitGraph(640, 480, GsINTER | GsOFSGPU, 1, 0);
	/*
	 * resolution set , interlace mode */

	GsDefDispBuff(0, 0, 0, 0);	/* Double buffer setting */

#if 0
	GsInitGraph(640, 240, GsINTER | GsOFSGPU, 1, 0);
	/*
	 * resolution set , non interlace mode */
	GsDefDispBuff(0, 0, 0, 240);	/* Double buffer setting */
#endif

	GsInit3D();		/* Init 3D system */

	Wot[0].length = OT_LENGTH;	/* Set bit length of OT handler */
	Wot[0].offset = 0x7ff;

	Wot[0].org = zsorttable[0];	/* Set Top address of OT Area to OT
					 * handler */

	/*
	 * same setting for anoter OT handler */
	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	Wot[1].offset = 0x7ff;

	coord_init();		/* Init coordinate */
	model_init();		/* Reading modeling data */
	view_init();		/* Setting view point */
	light_init();		/* Setting Flat Light */

	texture_init(TEX_ADDR);	/* texture load of TEX_ADDR */
	texture_init(TEX_ADDR1);/* texture load of TEX_ADDR1 */
	texture_init(TEX_ADDR2);/* texture load of TEX_ADDR2 */

	/* set FOG paramater */
	fgp.dqa = -10000*ONE/64/1000;
	fgp.dqb = 5/4*ONE*ONE;
	fgp.rfc = fgp.gfc = fgp.bfc = 0;
	GsSetFogParam(&fgp);
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

	/*
	 * Calculate World-Screen Matrix from viewing paramter */
	GsSetRefView2(&view);

	GsSetNearClip(100);	/* Set Near Clip */

}


light_init()
{				/* init Flat light */
	/* Setting Light ID 0 */
	/*
	 * Setting direction vector of Light0 */
	pslt[0].vx = 20;
	pslt[0].vy = -100;
	pslt[0].vz = -100;

	/*
	 * Setting color of Light0 */
	pslt[0].r = 0xd0;
	pslt[0].g = 0xd0;
	pslt[0].b = 0xd0;

	/*
	 * Set Light0 from parameters */
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
	GsSetAmbient(0, 0, 0);

	/* Setting default light mode */
	GsSetLightMode(0);
}

coord_init()
{				/* Setting coordinate */
	/* Setting hierarchy of Coordinate */
	GsInitCoordinate2(WORLD, &DWorld);

	/*
	 * Init work vector */
	PWorld.vx = PWorld.vy = PWorld.vz = 0;

	/*
	 * the org point of DWold is set to Z = -40000 */
	DWorld.coord.t[2] = -4000;
}


/*
 * Set coordinte parameter from work vector */
set_coordinate(pos, coor)
	SVECTOR *pos;		/* work vector */
	GsCOORDINATE2 *coor;	/* Coordinate */
{
	MATRIX  tmp1;

	/* Set translation */
	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	/*
	 * Rotate Matrix */
	RotMatrix(pos, &tmp1);

	/*
	 * Set Matrix to Coordinate */
	coor->coord = tmp1;

	/*
	 * Clear flag becase of changing parameter */
	coor->flg = 0;
}

/*
 * Load texture to VRAM */
texture_init(addr)
	u_long  addr;
{
	RECT    rect1;
	GsIMAGE tim1;

	/*
	 * Get texture information of TIM FORMAT */
	GsGetTimInfo((u_long *) (addr + 4), &tim1);

	rect1.x = tim1.px;	/* X point of image data on VRAM  */
	rect1.y = tim1.py;	/* Y point of image data on VRAM  */
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

	dop = (u_long *) MODEL_ADDR;	/* Top Address of MODELING DATA(TMD
					 * FORMAT) */
	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* Mapping real address */

	dop++;
	Objnum = *dop;		/* Get number of Objects */

	dop++;			/* Adjusting for GsLinkObject4 */

	/*
	 * Link ObjectHandler and TMD FORMAT MODELING DATA */
	for (i = 0; i < Objnum; i++)
		GsLinkObject4((u_long) dop, &object[i], i);

	for (i = 0, objp = object; i < Objnum; i++) {
		/*
		 * Set Coordinate of Object Handler
		 * */
		objp->coord2 = &DWorld;
		objp->attribute = 0;	/* Normal Light */
/*		objp->attribute = GsFOG | GsLLMOD;	/* FOG on */
/*		objp->attribute = GsLOFF;	/* LIGHT OFF */
		objp++;
	}
}

extern PACKET *FastG4L();	/* see g4l.c */
extern PACKET *FastG4LFG(); /* see g4lfg.c */
extern PACKET *FastG4NL(); /* see g4nl.c */
extern PACKET *FastTG4L(); /* see tg4l.c */
extern PACKET *FastTG4LFG(); /* see tg4lfg.c */
extern PACKET *FastTG4NL(); /* see tg4nl.c */

/************************* SAMPLE for GsTMDobject *****************/
SortTMDobject(objp, otp, shift)
	GsDOBJ2 *objp;
	GsOT   *otp;
	int     shift;
{
	u_long *vertop, *nortop, *primtop, primn;
	int     code;		/* polygon type */
	int		light_mode;

	/* get various informations from TMD foramt */
	vertop = ((struct TMD_STRUCT *) (objp->tmd))->vertop;
	nortop = ((struct TMD_STRUCT *) (objp->tmd))->nortop;
	primtop = ((struct TMD_STRUCT *) (objp->tmd))->primtop;
	primn = ((struct TMD_STRUCT *) (objp->tmd))->primn;

	/* attribute decoding */
	/* GsMATE_C=objp->attribute&0x07; not use */
	GsLMODE = (objp->attribute >> 3) & 0x03;
	GsLIGNR = (objp->attribute >> 5) & 0x01;
	GsLIOFF = (objp->attribute >> 6) & 0x01;
	GsNDIV = (objp->attribute >> 9) & 0x07;
	GsTON = (objp->attribute >> 30) & 0x01;

	if(GsLIOFF == 1)
		light_mode = GsLMODE_LOFF;
	else
		if((GsLIGNR == 0 && GsLIGHT_MODE == 1) ||
			(GsLIGNR == 1 && GsLMODE == 1))
			light_mode = GsLMODE_FOG;
		else
			light_mode = GsLMODE_NORMAL;

	/* primn > 0 then loop (making packets) */
	switch (light_mode){
		case GsLMODE_NORMAL:
			while (primn) {
				code = ((*primtop) >> 24 & 0xfd);	/* pure polygon type */
				switch (code) {
					case GPU_COM_G4:
						GsOUT_PACKET_P = (PACKET *) FastG4L
							((TMD_P_G4 *)primtop,(VERT *)vertop,(VERT *)nortop,
							GsOUT_PACKET_P,*((u_short *) primtop),shift,otp);
						primn -= *((u_short *) primtop);
						primtop += sizeof(TMD_P_G4)/4 * *((u_short *)primtop);
						break;
					case GPU_COM_TG4:	
						GsOUT_PACKET_P = (PACKET *) FastTG4L
							((TMD_P_TG4 *)primtop,(VERT *)vertop,(VERT *)nortop,
							GsOUT_PACKET_P,*((u_short *) primtop),shift,otp);
						primn -= *((u_short *) primtop);
						primtop += sizeof(TMD_P_TG4)/4 * *((u_short *)primtop);
						break;
					default:
						printf("This program supports only gouraud polygon.\n");
						printf("<%x,%x,%x>\n", code, GPU_COM_G4, GPU_COM_TG4);
						break;
				}
			}
			break;
		case GsLMODE_FOG:
			while (primn) {
				code = ((*primtop) >> 24 & 0xfd);	/* pure polygon type */
				switch (code) {
					case GPU_COM_G4:
						GsOUT_PACKET_P = (PACKET *) FastG4LFG
							((TMD_P_G4 *)primtop,(VERT *)vertop,(VERT *)nortop,
							GsOUT_PACKET_P,*((u_short *) primtop),shift,otp);
						primn -= *((u_short *) primtop);
						primtop += sizeof(TMD_P_G4)/4 * *((u_short *)primtop);
						break;
					case GPU_COM_TG4:	
						GsOUT_PACKET_P = (PACKET *) FastTG4LFG
							((TMD_P_TG4 *)primtop,(VERT *)vertop,(VERT *)nortop,
							GsOUT_PACKET_P,*((u_short *) primtop),shift,otp);
						primn -= *((u_short *) primtop);
						primtop += sizeof(TMD_P_TG4)/4 * *((u_short *)primtop);
						break;
					default:
						printf("This program supports only gouraud polygon.\n");
						printf("<%x,%x,%x>\n", code, GPU_COM_G4, GPU_COM_TG4);
						break;
				}
			}
			break;
		case GsLMODE_LOFF:
			while (primn) {
				code = ((*primtop) >> 24 & 0xfd);	/* pure polygon type */
				switch (code) {
					case GPU_COM_G4:
						GsOUT_PACKET_P = (PACKET *) FastG4NL
							((TMD_P_G4 *)primtop,(VERT *)vertop,(VERT *)nortop,
							GsOUT_PACKET_P,*((u_short *) primtop),shift,otp);
						primn -= *((u_short *) primtop);
						primtop += sizeof(TMD_P_G4)/4 * *((u_short *)primtop);
						break;
					case GPU_COM_TG4:	
						GsOUT_PACKET_P = (PACKET *) FastTG4NL
							((TMD_P_TG4 *)primtop,(VERT *)vertop,(VERT *)nortop,
							GsOUT_PACKET_P,*((u_short *) primtop),shift,otp);
						primn -= *((u_short *) primtop);
						primtop += sizeof(TMD_P_TG4)/4 * *((u_short *)primtop);
						break;
					default:
						printf("This program supports only gouraud polygon.\n");
						printf("<%x,%x,%x>\n", code, GPU_COM_G4, GPU_COM_TG4);
						break;
				}
			}
			break;
	}
}

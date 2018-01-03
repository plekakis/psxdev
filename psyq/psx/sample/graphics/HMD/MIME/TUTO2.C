#define DOPROF	0

/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	"tuto1.c" HMD viewer with MIMe
 *
 *		Basically, as same as tuto0.c but this sample performs
 *		with up to two analog controlers.
 *
 *		In this sample program, "MIMeID" is used as controler ID.
 *		MIMeID 0 corresponds to 1st controler, MIMeID 1 is for
 *		2dn controler.
 *
 *		Copyright (C) 1997  Sony Computer Entertainment
 *		All rights Reserved
 */

/*#define DEBUG		/**/

/*
#define INTER		/* Interlace mode */

#include <sys/types.h>

#include <libetc.h>		/* needs to be included to use PAD */
#include <libapi.h>		/* for analog controler */
#include <libgte.h>		/* needs to be included to use LIGS */
#include <libgpu.h>		/* needs to be included to use LIBGS */
#include <libgs.h>		/* definitions of structures for using
                    the graphics library, etc.   for LIBGS */
#include <libhmd.h>             /* for LIBHMD */
#include <stdio.h>
#include <stdlib.h>
#include "../common/scan.h"

#if DOPROF
#include <libapi.h>	/* for RCnt */
#endif /* DOPROF */

#include "control.h"		/* ctlfc calculation routine*/

#if DOPROF
/*#define RCNT	RCntCNT1	/* Hcount */
#define RCNT	RCntCNT2	/* 1/8 */
#endif /* DOPROF */

static u_char	padbuf[2][34];

#define MAXMIME	32
#define MAXPR	2
static long	mimepr[MAXPR][MAXMIME];	/* MIMe weight coefficient*/
static u_long	mimenum = 0;
static int	prnum = 0;
static int	cntrlarry[MAXPR][CTLMAX][CTLTIME]; /* ring buffer for convolution */
static CTLFUNC	ctlfc[MAXPR][CTLMAX];	/* ring buffer for convolution */

static int cnv0[24] = {		/* transfer function used for ctlfc calculation*/
    -40, -50, 0,
    200, 325, 450, 475, 500, 475, 450, 375,
    300, 250, 200, 150, 100,  80,  55,   0,
    -20, -50, -40, -25,
};



#define OBJECTMAX 100		/* Max Objects  defines the maximum number of logical objects
                    a 3D model can be divided into */

#define MODEL_ADDR ((u_long *)0x80010000)	/* Top Address of modeling data (HMD FORMAT) */

#define OT_LENGTH 14		/* Area of OT */

#define PACKETMAX (10000*24)


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH];
				/* Area of OT */

GsUNIT		object[OBJECTMAX];
				/* Array of Object Handler */

u_long		bnum;		/* valibable of number of Objects */

GsCOORDUNIT	*DModel = NULL;	/* Coordinate for GsUNIT */

/* work short vector for making Coordinate parmeter */

GsRVIEW2	view;		/* View Point Handler */
GsF_LIGHT	pslt[3];	/* Flat Lighting Handler */
u_long		padd = 0;		/* Controler data */

PACKET          out_packet[2][PACKETMAX];
				/* GPU PACKETS AREA */

/* 
 * prototype
 */
void init_all(void);
int  obj_interactive(void);
void mime_interactive(u_long padd);
void set_coordinate(GsCOORDUNIT *coor);
void model_init(void);
void view_init(void);
void light_init(void);

/************* MAIN START ******************************************/
main()
{
	GsUNIT	*op;		/* pointer of Object Handler */
	int	outbuf_idx;
	MATRIX	tmpls;
	int	i;
	
	ResetCallback();
	init_all();

	FntLoad(960, 256);
#ifdef INTER
	FntOpen(-300, -200, 256, 200, 0, 512); 
#else /* INTER */
	FntOpen(-150, -100, 256, 200, 0, 512); 
#endif /* INTER */

	GsInitVcount();

#if DOPROF
	SetRCnt(RCNT, 65535, RCntMdSP);
#endif /* DOPROF */

	while(1) {
#if DOPROF
		static long	min_cnt = -1;
		static long	max_cnt = 0;

		ResetRCnt(RCNT);
#endif /* DOPROF */

		/* interactive parameter get */
		if(obj_interactive() == 0) {
			PadStopCom();
			ResetGraph(3);
			StopCallback();
			return 0;
		}
		GsSetRefView2(&view);	/* Calculate World-Screen Matrix */
		outbuf_idx=GsGetActiveBuff();
					/* Get double buffer index */
		GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);
			
		GsClearOt(0, 0, &Wot[outbuf_idx]);
					/* Clear OT for using buffer */
			
		for(i=0,op=object; i<bnum; i++,op++) {
			if (op->primtop == NULL)
				continue;
			if (op->coord) {
				/* Calculate Local-World Matrix */
				GsGetLwUnit(op->coord, &tmpls);
	
				/* Set LWMATRIX to GTE Lighting Registers */
				GsSetLightMatrix(&tmpls);
	
				/* Set LSAMTRIX to GTE Registers */
				GsGetLsUnit(op->coord, &tmpls);
	
				/* Set LSMATRIX to GTE Lighting Registers */
				GsSetLsMatrix(&tmpls);
			}

			/* Perspective Translate Object and Set OT */
			GsSortUnit(op, &Wot[outbuf_idx], getScratchAddr(0));
		}
			
		padd = 0;
		if (padbuf[0][0] == 0) {
			padd |= ((~((padbuf[0][2]<<8)|(padbuf[0][3])))&0xffff);
		}
		if (padbuf[1][0] == 0) {
			padd |= ((~((padbuf[1][2]<<8)|(padbuf[1][3])))<<16);
		}

#if DOPROF
		{
			long	cnt = GetRCnt(RCNT);

			if (min_cnt == -1 || cnt < min_cnt) {
				min_cnt = cnt;
			}
			if (cnt > max_cnt) {
				max_cnt = cnt;
			}
			FntPrint("cnt = %d\n", cnt);
			FntPrint("min = %d\n", min_cnt);
			FntPrint("max = %d\n", max_cnt);
		}
#endif /* DOPROF */

		FntPrint("Hcount = %d\n", GsGetVcount()); /**/
#ifndef INTER
		DrawSync(0);
#endif /* !INTER */
		VSync(0);		/* Wait for VSYNC */
		GsClearVcount();
#ifdef INTER
		ResetGraph(1);		/* Reset GPU */
#endif /* INTER */
		GsSwapDispBuff();	/* Swap double buffer */
		/* Set SCREEN CLESR PACKET to top of OT */
		GsSortClear(0x0, 0x0, 0x0, &Wot[outbuf_idx]);

		/* Drawing OT */
		GsDrawOt(&Wot[outbuf_idx]);

		FntFlush(-1);
	}

	return 0;
}

/* convert pad data to mimepr */
static void
pad_mimepr(int fn, unsigned long padd, int k)
{
	int	i;
	unsigned char	*buf;
	long	*pr;
	CTLFUNC	*fc;

	if (k == 0) {
		buf = padbuf[0];
		pr = mimepr[k];
		fc = ctlfc[k];
	} else if (k == 1) {
		buf = padbuf[1];
		pr = mimepr[k];
		fc = ctlfc[k];
	} else {
		printf("%s(%d): I can not do this!\n", __FILE__, __LINE__);
		exit(1);
	}

	for (i = 0; i < mimenum; i++) {
		fc[i].in=0;
	}

	if (padd & PADL1)	fc[0].in+=4096;
	if (padd & PADL2)	fc[1].in+=4096;
	if (padd & PADR1)	fc[2].in+=4096;
	if (padd & PADR2)	fc[3].in+=4096;
	if (padd & PADLleft)	fc[4].in+=4096;
	if (padd & PADLright)	fc[5].in+=4096;
	if (padd & PADLup)	fc[6].in+=4096;
	if (padd & PADLdown)	fc[7].in+=4096;

	if ((buf[1]>>4) == 7) {
		int	rx, ry, lx, ly;

		rx = 4096 * (-128 + (int)buf[4]) / 128;
		ry = 4096 * ( 128 - (int)buf[5]) / 128;
		lx = 4096 * (-128 + (int)buf[6]) / 128;
		ly = 4096 * ( 128 - (int)buf[7]) / 128;

		fc[0].in += rx;
		fc[1].in += ry;
		fc[2].in += lx;
		fc[3].in += ly;
	}

	for (i = 0; i < prnum; i++) {
		set_cntrl(fn, ctlfc[i], cntrlarry[i]);
	}

	for (i = 0; i < mimenum; i++) {
		pr[i]=fc[i].out;
	}
}

void
mime_interactive(u_long padd)
{
	int i, j;
	static u_long	fn = 0;

	pad_mimepr(fn, padd,       0);
	pad_mimepr(fn, padd >> 16, 1);

	for (i = 0; i < mimenum; i++) {
		for (j = 0; j < prnum; j++) {
			FntPrint("MIMe[%d]:%5d  ", i, mimepr[j][i]);
		}
		FntPrint("\n");
	}

	fn++;
}

int obj_interactive()
{
	int num = 0;
	static u_long opadd = 0;

		
	for (num = 0; num < prnum; num++) {
		u_long	p = padd >> (16 * num);

		if (p & PADRleft)	DModel[num].rot.vy += 5*ONE/360;
		if (p & PADRright)	DModel[num].rot.vy -= 5*ONE/360;
		if (p & PADRup)	DModel[num].rot.vx += 5*ONE/360;
		if (p & PADRdown)	DModel[num].rot.vx -= 5*ONE/360;
		
#if 0		
		if (p & PADLleft)	DModel[num].matrix.t[0] -= 10;
		if (p & PADLright)	DModel[num].matrix.t[0] += 10;

		if (p & PADLdown)	DModel[num].matrix.t[1] += 10;
		if (p & PADLup)	DModel[num].matrix.t[1] -= 10;
#endif /* 0 */

		if (p & PADstart)	DModel[num].matrix.t[2] += 100;
		if (p & PADselect)	DModel[num].matrix.t[2] -= 100;

		if ((p & PADselect) && (padd & PADstart)) {
			return(0);
		}
		
		set_coordinate(&DModel[num]);
	}

	mime_interactive(padd);

	opadd = padd;
	return(1);
}

void init_all(void)		/* Initialize routine */
{
	ResetGraph(0);		/* reset GPU */
	PadInitDirect(padbuf[0], padbuf[1]);
	PadStartCom();
	padd = 0;		/* controller value initialize */
#ifdef INTER
	GsInitGraph(640, 480, 5, 1, 0);
				/* rezolution set , interrace mode */
	GsDefDispBuff(0, 0, 0, 0);
				/* Double buffer setting */
#else /* INTER */
	GsInitGraph(320, 240, 4, 1, 0);
				/* rezolution set , non interrace mode */
	GsDefDispBuff(0, 0, 0, 240);
				/* Double buffer setting */
#endif /* INTER */

	GsInit3D();		/* Init 3D system */
	
	Wot[0].length = OT_LENGTH;/* Set bit length of OT handler */
	Wot[0].org = zsorttable[0];
				/* Set Top address of OT Area to OT handler */
	/* same setting for anoter OT handler */
	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	model_init();		/* Reading modeling data */
	view_init();		/* Setting view point */
	light_init();		/* Setting Flat Light */

	{
		int	i;

		for (i = 0; i < prnum; i++) {
			init_cntrlarry(cnv0, sizeof(cnv0) / sizeof(cnv0[0]),
				ctlfc[i], cntrlarry[i]);
		}
	}
}

void view_init(void)		/* Setting view point */
{
	/*---- Set projection,view ----*/
	GsSetProjection(1000);	/* Set projection */
	
	/* Setting view point location */
	view.vpx = 0; view.vpy = 0; view.vpz = -2000;

	/* Setting focus point location */
	view.vrx = 0; view.vry = 0; view.vrz = 0;

	/* Setting bank of SCREEN */
	view.rz=0;

	/* Setting parent of viewing coordinate */	
	view.super = WORLD;

	/* Calculate World-Screen Matrix from viewing paramter */
	GsSetRefView2(&view);
}


void light_init(void)		/* init Flat light */
{
	/* Setting Light ID 0 */	
	/* Setting direction vector of Light0 */
	pslt[0].vx = 100; pslt[0].vy = 100; pslt[0].vz = 100;

	/* Setting color of Light0 */
	pslt[0].r = 0xd0; pslt[0].g = 0xd0; pslt[0].b = 0xd0;

	/* Set Light0 from parameters */
	GsSetFlatLight(0, &pslt[0]);
	
	/* Setting Light ID 1 */
	pslt[1].vx = 20; pslt[1].vy = -50; pslt[1].vz = -100;
	pslt[1].r = 0x80; pslt[1].g = 0x80; pslt[1].b = 0x80;
	GsSetFlatLight(1, &pslt[1]);
	
	/* Setting Light ID 2 */
	pslt[2].vx = -20; pslt[2].vy = 20; pslt[2].vz = 100;
	pslt[2].r = 0x60; pslt[2].g = 0x60; pslt[2].b = 0x60;
	GsSetFlatLight(2, &pslt[2]);
	
	/* Setting Ambient */
	GsSetAmbient(0, 0, 0);
	
	/* Setting default light mode */	
	GsSetLightMode(0);
}

/* Set coordinte parameter from work vector */
void set_coordinate(GsCOORDUNIT *coor)
{
	/* Rotate Matrix */
	RotMatrix(&coor->rot, &coor->matrix);

	/* Clear flag becase of changing parameter */
	coor->flg = 0;
}

static void
set_mimepr(u_long type, u_long *hp)
{
	long	**qMIMePr;
	u_long	nMIMeNum;
	u_short	nMIMeID;

	switch (type) {
	case GsVtxMIMe:
	case GsNrmMIMe:
		qMIMePr = (long **)&hp[1];
		nMIMeNum = hp[2];
		nMIMeID = hp[3] & 0x0ffff;
		break;
	case GsJntAxesMIMe:
	case GsJntRPYMIMe:
		qMIMePr = (long **)&hp[2];
		nMIMeNum = hp[3];
		nMIMeID = hp[4] & 0x0ffff;
		break;
	default:
		printf("%s(%d): can't happen!\n", __FILE__, __LINE__);
		exit(1);
	}

	if (nMIMeID >= MAXPR) {
		printf("%s(%d): too large MIMeID (%d). Ignored.\n",
			__FILE__, __LINE__, nMIMeID);
		nMIMeID = 0;
	}
	if (*qMIMePr == 0) {
		*qMIMePr = mimepr[nMIMeID];
	}
	if (nMIMeNum > mimenum) {
		mimenum = nMIMeNum;
	}
	if ((int)nMIMeID + 1 > prnum) {
		prnum = nMIMeID + 1;
	}
}

/* Read modeling data (HMD FORMAT) */
void model_init(void)
{
	u_long	*dop;
	int	i;

	dop = MODEL_ADDR;
				/* Top Address of MODELING DATA(HMD FORMAT) */
	GsMapUnit(dop);		/* Mappipng real address */
	dop++;			/* ID skip */
	dop++;			/* flag skip */
	dop++;			/* headder top skip */
	bnum = *dop;		/* Get number of Blocks */
	dop++;			/* skip block number */

	for (i = 0; i < bnum; i++) {
		GsTYPEUNIT	ut;

		object[i].primtop = (u_long *)dop[i];
		if (object[i].primtop == NULL)
			continue;

		GsScanUnit(object[i].primtop, 0, 0, 0);
		while (GsScanUnit(0, &ut, &Wot[0], getScratchAddr(0))) {
			if (((ut.type >> 24 == 0x00)	/* CTG 0: POLY */
			|| (ut.type >> 24 == 0x01)	/* CTG 1: SHARED */
			|| (ut.type >> 24 == 0x05)	/* CTG 5: GROUND */
			|| (ut.type >> 24 == 0x06)	/* CTG 6: ENVMAP(beta) */
			) && (ut.type & 0x00800000)) {	/* check INI bit */
				DModel = GsMapCoordUnit(MODEL_ADDR, ut.ptr);
				/* clear INI bit */
				ut.type &= (~0x00800000);
			}

			*ut.ptr = NULL;
			switch (ut.type >> 24) {
			case 0x00:	/* CTG 0: POLY or NULL */
				*ut.ptr = scan_poly(ut.type);
				break;
			case 0x01:	/* CTG 1: SHARED */
				*ut.ptr = scan_shared(ut.type);
				break;
			case 0x02:	/* CTG 2: IMAGE */
				*ut.ptr = scan_image(ut.type);
				/*
					The image is loaded only one time
					in this application.
				*/
				((GsUNIT_Funcp)(*ut.ptr))
					((GsARGUNIT *)getScratchAddr(0));
				*ut.ptr = (u_long)GsU_00000000;
				break;
#ifdef notdef
			case 0x03:	/* CTG 3: ANIMATION */
				*ut.ptr = scan_anim(ut.type);
				if (*ut.ptr != NULL
				&& *ut.ptr != (u_long)GsU_00000000) {
					anum = GsLinkAnim(seq, ut.ptr);
					if (GsScanAnim(ut.ptr, 0) == 0) {
						printf("ScanAnim failed!!\n\n");
					} else {
						scan_interp();
					}
				}
				break;
#endif /* notdef */
			case 0x04:	/* CTG 4: MIMe */
				*ut.ptr = scan_mime(ut.type);
				switch (ut.type) {
				case GsVtxMIMe:
				case GsNrmMIMe:
				case GsJntAxesMIMe:
				case GsJntRPYMIMe:
					/*
						This application has
						area for MIMePr.
					*/
					set_mimepr(ut.type, GsGetHeadpUnit());
					break;
				case GsRstVtxMIMe:
					GsInitRstVtxMIMe(
						ut.ptr, GsGetHeadpUnit());
					break;
				case GsRstNrmMIMe:
					GsInitRstNrmMIMe(
						ut.ptr, GsGetHeadpUnit());
					break;
				default:
					/* do nothing */;
				}
				break;
			case 0x05:	/* CTG 5: GROUND */
				*ut.ptr = scan_gnd(ut.type);
				break;
			case 0x06:	/* CTG 6: ENVMAP(beta) */
				*ut.ptr = scan_envmap(ut.type);
				break;
			default:
				/* do nothing */;
			}
			if (*ut.ptr == NULL) {
				printf("unsupported type 0x%08x\n", ut.type);
				*ut.ptr = (u_long)GsU_00000000;
			}

#ifdef DEBUG
			printf("DEBUG:block:%d, Code:0x%08x\n", i, ut.type);
#endif /* DEBUG */
		}
		object[i].coord = NULL;
	}

	if (DModel != NULL) {
		for(i = 1; i < bnum - 1; i++) {
			object[i].coord = &DModel[i - 1];
		}
	}
}

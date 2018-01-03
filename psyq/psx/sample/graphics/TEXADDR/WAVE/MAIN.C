/* $PSLibId: Run-time Library Release 4.4$ */
/*
 * "main.c" - texture address modulation sample
 * 
 * Version 1.00 July 18, 1997
 * 
 * Copyright (C) 1993-97 Sony Computer Entertainment Inc.
 * All rights Reserved 
 */

#include <sys/types.h>
#include <libetc.h>		/* for Control Pad */
#include <libgte.h>		/* LIBGS uses libgte */
#include <libgpu.h>		/* LIBGS uses libgpu */
#include <libgs.h>		/* for LIBGS */
#include <libcd.h>		/* for LIBCD */
#include <inline_c.h>
#include <gtemac.h>
#include "spadstk.h"		/* for Scratch Pad Stack */

/*#define CDROM /* Define if exec from CD-ROM */

extern _GsFCALL GsFCALL4;	/* GsSortObject4J Func Table */
extern PACKET *FastG3L(TMD_P_G3*, VERT*, VERT*, PACKET*, int, int, GsOT*);
extern PACKET *FastG3L2(TMD_P_G3*, VERT*, VERT*, PACKET*, int, int, GsOT*);
extern PACKET *FastG3L2L3(TMD_P_G3*, VERT*, VERT*, PACKET*, int, int, GsOT*);

/***********************************************************/ 
/* Ordering Table (OT) */
/***********************************************************/ 
/* Address of texture data TIM FORMAT) */
#define TEX0_ADDR   0x80018000
#define TEX1_ADDR   0x80010000
#define TEX2_ADDR   0x80068000
#define TEX3_ADDR   0x80040000
#define TEX4_ADDR   0x80090000

/* Top Address of modeling data (TMD FORMAT) */
#define MODEL1_ADDR 0x800D0000
#define MODEL2_ADDR 0x800C8000

/***********************************************************/
/* Ordering Table (OT) */
/***********************************************************/
GsOT    Wot0[2];		/* Handler of OT Uses 2 Hander for Dowble buffer */
/*#define OT_LENGTH  8		/* bit length of OT */
#define OT_LENGTH  11		/* bit length of OT */
GsOT_TAG zsorttable0[2][1<<OT_LENGTH];	/* Area of OT */

GsOT    *ot2;			/* another OT pointer */
GsOT    Wot00[2];		/* Handler of OT Uses 2 Hander for Dowble buffer */
GsOT_TAG zsorttable00[2][1<<OT_LENGTH];	/* Area of OT */

/***********************************************************/
/* 3D Objects */
/***********************************************************/
#define OBJECTMAX 2		/* Max Objects */
GsDOBJ2 object[OBJECTMAX];	/* Array of Object Handler */

u_long  Objnum;			/* valibable of number of Objects */

GsCOORDINATE2 DWorld,DWorld2;	/* Coordinate for GsDOBJ2 */
SVECTOR Rotation,Rotation2;	/* rotation vector for each object */
VECTOR Scale,Scale2;		/* scaling vector for each object */

/***********************************************************/ 
/* Other variables */
/***********************************************************/ 
GsRVIEW2 view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting
				   Handler*/
u_long padd;			/* Controler data */
u_long padd2;

extern DRAWENV GsDRAWENV;	/* Original drawenv */

/***********************************************************/ 
/* GPU packet area */
/***********************************************************/ 
#define PACKETMAX 10000		/* Max GPU packets */
#define SIZEOFPACKETAREA (PACKETMAX*24)	/* size of PACKETMAX (byte) paket
					   size may be 24 byte(6 word) */
PACKET  out_packet[2][SIZEOFPACKETAREA];	/* GPU PACKETS AREA */


/***********************************************************/ 
/* Work for Environment mapping (Set1 for refraction) */
/***********************************************************/ 
GsOT	Wot1[2];		/* Handler of OT */
GsOT_TAG zsorttable1[2][1<<OT_LENGTH];	/* Area of OT */
DRAWENV drenv1;			/* Drawenv to create Env-map pattern  */
RECT	envrect1;
DR_STP  pr0, pr1;		/* To use stp flag */
int	refract;		/* Refraction rate */

/***********************************************************/ 
/* Work for Environment mapping (Set2 for reflection) */
/***********************************************************/ 
GsOT	Wot2[2];		/* Handler of OT */
GsOT_TAG zsorttable2[2][1<<OT_LENGTH];	/* Area of OT */
DRAWENV	drenv2;			/* Drawenv to create Env-map pattern  */
RECT	envrect2;
GsRVIEW2 view2;			/* View Point Handler */
int	reflect;		/* Reflection rate */

/*****************  Type Definitions  ***************************/

/* struct of object */
typedef struct  {
  u_long *vert_top;
  u_long n_vert;
  u_long *normal_top;
  u_long n_normal;
  u_long *primitive_top;
  u_long n_primitive;
  long scale;
} Object;

/* flags */
int f_Back  = 0;
int f_Wave  = 0;
int f_Auto  = 1;
int f_Black = 1;
int f_Mode  = 0;
int f_Ripple = 0;

/*****************  Global variables  ***************************/

/* The values of sin( ) from 0 to 90 degrees.
   divided into 64 parts.
   Each entry is in fixed point format. */
static short G_sinT[] =
{    0,   100,   200,   301,   401,   501,   601,   700,
   799,   897,   995,  1092,  1189,  1284,  1379,  1474,
  1567,  1659,  1751,  1841,  1930,  2018,  2105,  2191,
  2275,  2358,  2439,  2519,  2598,  2675,  2750,  2824,
  2896,  2966,  3034,  3101,  3166,  3229,  3289,  3348,
  3405,  3460,  3513,  3563,  3612,  3658,  3702,  3744,
  3784,  3821,  3856,  3889,  3919,  3947,  3973,  3996,
  4017,  4035,  4051,  4065,  4076,  4084,  4091,  4094
};

Object  G_obj;             /* struct of object */

/* 
   sin
      n : from 0 to 255 */   
short m_sin(unsigned char n)
{
  if (n <= 63)
    return G_sinT[n];

  else if (n <= 127)
    return G_sinT[127 - n];

  else if (n <= 191)
    return -G_sinT[n-128];

  else
    return -G_sinT[255-n];
}

/*********************************************************************
	RIPPLE
 *********************************************************************/
#include "link14.c"		/* include additional data */
#define NRING 128
int SetTmdVertexY(int n, short y);

/* define wave type */
typedef struct {
	int h;				/* height */
	int aa, a1, a2;			/* amplitude */
	int ws;				/* speed */
	int a0, a;
	int top;
	short h_ring[NRING];		/* ring height buffer */
	unsigned char d_tbl[NVERT];	/* distance table */
} RIPPLE;

RIPPLE r0 = {12, 5, 256*6,  256*12, 3, 0,0,0};
RIPPLE r1 = { 8, 8, 256*12, 256*12, 3, 256*4,0,0};

/***********************************************************/ 
/* Main routine */
/***********************************************************/ 
main()
{
	int     i;
	GsDOBJ2 *op;		/* pointer of Object Handler */
	int     outbuf_idx;	/* double buffer index */
	MATRIX  tmpls;
	VECTOR  tmpv;

	ResetCallback();

	init_all();

	SetDrawStp(&pr0, 0);
	SetDrawStp(&pr1, 1);
	reflect = 70;
	refract = 150;

	f_Back = 1;
	f_Wave = 1;
	reflect = 140;
	refract = 55;
	Scale.vx = Scale.vy = Scale.vz = 4096;
	DWorld.coord.t[0] = 0;
	DWorld.coord.t[1] = 0;
	DWorld.coord.t[2] = -2800;
	Rotation.vx = -524;
	Rotation.vy = 3072;
	Rotation.vz = 2072;
	Scale2.vx = Scale2.vy = Scale2.vz = 3196;
	DWorld2.coord.t[0] = 0;
	DWorld2.coord.t[1] = -200;
	DWorld2.coord.t[2] = 400;

	while (1) {
		if(f_Auto) autoDemo();

		/* interactive parameter get */
#if TUNE > 1
		SetSpadStack(SPAD_STACK_ADDR);
#endif
		if (moveObjectByPad() == 0) {
#if TUNE > 1
			ResetSpadStack();
#endif
			return 0;
		}
#if TUNE > 1
		ResetSpadStack();
#endif

		/* Update camera information */
		GsSetProjection(1000);	/* Set projection */
		GsSetRefView2(&view);	/* Calculate World-Screen Matrix */

		/* Get double buffer index */
		outbuf_idx = GsGetActiveBuff();
		ot2 = &Wot00[outbuf_idx];

		/* Set top address of Packet Area for output of GPU PACKETS */
		GsSetWorkBase((PACKET *) out_packet[outbuf_idx]);

		/* Clear OT for using buffer */
		GsClearOt(0, 0, &Wot0[outbuf_idx]);
		GsClearOt(0, 0, &Wot00[outbuf_idx]);
		GsClearOt(0, (1<<OT_LENGTH)-1, &Wot1[outbuf_idx]);
		GsClearOt(0, (1<<OT_LENGTH)-1, &Wot2[outbuf_idx]);

		if(!f_Black)
		for (i = 0, op = object; i < OBJECTMAX; i++) {
			/* Calculate Local-World Matrix */
			GsGetLw(op->coord2, &tmpls);

			/* Set LWMATRIX to GTE Lighting Registers */
			GsSetLightMatrix(&tmpls);

			/* Calculate Local-Screen Matrix */
			GsGetLs(op->coord2, &tmpls);

			/* Set LSAMTRIX to GTE Registers */
			GsSetLsMatrix(&tmpls);

			/* Perspective Translate Object and Set OT */

			switch(i) {
				case 0:
#if TUNE > 1
					SetSpadStack(SPAD_STACK_ADDR);
#endif
					GsSortObject4J(op,&Wot0[outbuf_idx],
						14-OT_LENGTH,getScratchAddr(0));
#if TUNE > 1
					ResetSpadStack();
#endif
					break;
				case 1:
				case 2:
#if TUNE > 1
					SetSpadStack(SPAD_STACK_ADDR);
#endif
					GsSortObject4(op,&Wot0[outbuf_idx],
						14-OT_LENGTH,getScratchAddr(0));
#if TUNE > 1
					ResetSpadStack();
#endif
					break;
			}
			op++;
		}

		/* Draw environment pattern */
		GsSetRefView2(&view);
		op = object+1;
		GsGetLw(op->coord2, &tmpls);
		GsSetLightMatrix(&tmpls);
		GsGetLs(op->coord2, &tmpls);
		tmpv.vx = ONE/2;
		tmpv.vy = ONE;
		tmpv.vz = ONE;
		ScaleMatrixL(&tmpls, &tmpv);
		tmpls.t[0] = tmpls.t[0]/2;
		GsSetLsMatrix(&tmpls);
#if TUNE > 1
		SetSpadStack(SPAD_STACK_ADDR);
#endif
		if(DWorld2.coord.t[2] < DWorld.coord.t[2]+750) { 
			GsSortObject4(op, &Wot1[outbuf_idx],
				14 - OT_LENGTH, getScratchAddr(0));
		}
#if TUNE > 1
		ResetSpadStack();
#endif

		/* Draw environment pattern */
		view2_update();
		GsSetProjection(1000);
		GsSetRefView2(&view2);
		op = object+1;
		GsGetLw(op->coord2, &tmpls);
		GsSetLightMatrix(&tmpls);
		GsGetLs(op->coord2, &tmpls);
		GsSetLsMatrix(&tmpls);
#if TUNE > 1
		SetSpadStack(SPAD_STACK_ADDR);
#endif
		GsSortObject4(op, &Wot2[outbuf_idx], 14-OT_LENGTH, getScratchAddr(0));
#if TUNE > 1
		ResetSpadStack();
#endif

		AddPrim(Wot2[outbuf_idx].tag, &pr1);

		/* wait for V-BLNK*/
		VSync(0);
		/* Wait until GPU finishes drawing */
		DrawSync(0);

		/* Readint Control Pad data */
		padd = PadRead(1);
		padd2 = padd>>16;

		/* Swap double buffer */
		GsSwapDispBuff();

		AddPrim(Wot0[outbuf_idx].tag, &pr0);

		/* Set SCREEN CLEAR PACKET to top of OT */
		GsSortClear(0x0, 0x0, 0x0, &Wot0[outbuf_idx]);

		/* Send Env-map image */
		switch(f_Back) {
			case 0:
				LoadImage(&envrect1, (u_long *)(TEX2_ADDR+12));
				break;
			case 1:
				LoadImage(&envrect1, (u_long *)(TEX4_ADDR+12));
				break;
		}
		LoadImage(&envrect2, (u_long *)(TEX0_ADDR+12));

		/* Drawing OT */
		PutDrawEnv(&drenv1);
		GsDrawOt(&Wot1[outbuf_idx]);
		PutDrawEnv(&drenv2);
		GsDrawOt(&Wot2[outbuf_idx]);
		GsSetDrawBuffClip();
		GsDrawOt(&Wot0[outbuf_idx]);
		GsDrawOt(&Wot00[outbuf_idx]);

#if TUNE > 1
		SetSpadStack(SPAD_STACK_ADDR);
#endif
		if(f_Auto | f_Ripple) {
			ripple();
			g3_normal();
		}
#if TUNE > 1
		ResetSpadStack();
#endif
	}
}

/**********************************************************
	Auto Demo
 **********************************************************/

typedef struct {
	int f_Back;
	int f_Wave;
	int r0_h;
	int r1_h;
	PACKET *(*Render)();
	int reflect;
	int refract;
	long s1;
	long t1[3];
	SVECTOR Rotation1;
	long s2;
	long t2[3];
	SVECTOR Rotation2;
} MODE;

MODE mode[3] = {
	{
		1,1,12,0,FastG3L,140,55,
		4096,{0,0,-2800},{-524,3072,2072},
		3196,{0,-200,400},{0,0,0}
	},
	{
		0,0,12,8,FastG3L2,35,55,
		4096,{0,0,-2800},{-524,3072,2072},
		10896,{0,100,-4200},{0,0,0}
	},
	{
		1,0,12,8,FastG3L2,25,70,
		4096,{0,0,-2800},{-524,3072,2072},
		10896,{0,100,-4200},{0,0,0}
	}
};

static long v_cnt = 0;
#define FRAME 30
#define TIME0 (FRAME*2)
#define TIME1 40

long cnt_mode[] = {
	0,
	(TIME0+(FRAME*TIME1)*0),
	(TIME0+(FRAME*TIME1)*1),
	(TIME0+(FRAME*TIME1)*2),
	(TIME0+(FRAME*TIME1)*3)
};

autoDemo()
{
	if(v_cnt == cnt_mode[0] ) {
		f_Mode = 0;
		f_Black = 1;
		r0.h  = 0;
		r1.h  = 0;
	} else if(v_cnt == cnt_mode[1] ) {
		f_Mode = 1;
		f_Black = 0;
		changeMode(0);
	} else if(v_cnt <  cnt_mode[2] ) {
		changeParam(0, v_cnt - cnt_mode[1]);
	} else if(v_cnt == cnt_mode[2] ) {
		f_Mode = 2;
		changeMode(1);
	} else if(v_cnt <  cnt_mode[3] ) {
		changeParam(1, v_cnt - cnt_mode[2]);
	} else if(v_cnt == cnt_mode[3] ) {
		f_Mode = 3;
		changeMode(2);
	} else if(v_cnt <  cnt_mode[4] ) {
		changeParam(2, v_cnt - cnt_mode[3]);
	} else if(v_cnt == cnt_mode[4] ) {
		f_Mode = 4;
		v_cnt = -1;
	}

	v_cnt++;
}

changeParam(int n, long cnt)
{
#define TIME5 10
	if(n == 0) {
		       if(cnt <  FRAME*(TIME5+0)) {
		} else if(cnt == FRAME*(TIME5+3)) {
			r0.h  = 12;
			r0.aa = 5;
			r0.a1 = 256*8;
			r0.a2 = 256*12;
			r0.ws = 4;
			r0.a0 = r0.a = 0;
		} else if(cnt == FRAME*(TIME5+17)) {
			f_Wave = 0;
			r0.h  = 0;
			r1.h  = 8;
		} else if(cnt == FRAME*(TIME5+21)) {
			r0.h  = 12;
			r0.aa = 5;
			r0.a1 = 256*6;
			r0.a2 = 256*12;
			r0.ws = 3;
			r0.a0 = r0.a = 0;
		} else if(cnt < FRAME*TIME1) {
		}
	} else if(n == 1) {
		       if(cnt < FRAME*(TIME5+ 0)) {
		} else if(cnt < FRAME*(TIME5+ 3)) {
			if(reflect != 0) reflect-=1;
		} else if(cnt < FRAME*(TIME5+ 6)) {
			if(refract != 0) refract-=1;
		} else if(cnt < FRAME*(TIME5+ 9)) {
			if(refract < mode[n].refract) refract+=1;
		} else if(cnt < FRAME*(TIME5+12)) {
			if(reflect < mode[n].reflect) reflect+=1;
		} else if(cnt < FRAME*TIME1) {
		}
	} else {
		       if(cnt < FRAME*(TIME5+ 0)) {
		} else if(cnt < FRAME*(TIME5+ 3)) {
			if(reflect != 0) reflect-=1;
		} else if(cnt < FRAME*(TIME5+ 6)) {
			if(refract != 0) refract-=1;
		} else if(cnt < FRAME*(TIME5+ 9)) {
			if(refract < mode[n].refract) refract+=1;
		} else if(cnt < FRAME*(TIME5+12)) {
			if(reflect < mode[n].reflect) reflect+=1;
		} else if(cnt < FRAME*(TIME5+12) + (4096/16)) {
			Rotation.vy -= 16;
		} else if(cnt < FRAME*TIME1) {
		}
	}
}

changeMode(int n)
{
	r0.a0 = r0.a = 0;
	r1.a0 = r1.a = 0;
	f_Back             = mode[n].f_Back;
	f_Wave             = mode[n].f_Wave;
	r0.h               = mode[n].r0_h;
	r1.h               = mode[n].r1_h;
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = mode[n].Render;
	reflect            = mode[n].reflect;
	refract            = mode[n].refract;
	Scale.vx = Scale.vy = Scale.vz = mode[n].s1;
	DWorld.coord.t[0]  = mode[n].t1[0] ;
	DWorld.coord.t[1]  = mode[n].t1[1] ;
	DWorld.coord.t[2]  = mode[n].t1[2] ;
	Rotation.vx        = mode[n].Rotation1.vx ;
	Rotation.vy        = mode[n].Rotation1.vy ;
	Rotation.vz        = mode[n].Rotation1.vz ;
	Scale2.vx = Scale2.vy = Scale2.vz = mode[n].s2;
	DWorld2.coord.t[0] = mode[n].t2[0] ;
	DWorld2.coord.t[1] = mode[n].t2[1] ;
	DWorld2.coord.t[2] = mode[n].t2[2] ;
	Rotation2.vx       = mode[n].Rotation2.vx ;
	Rotation2.vy       = mode[n].Rotation2.vy ;
	Rotation2.vz       = mode[n].Rotation2.vz ;
}

/***********************************************************/
/* Move Objects */
/***********************************************************/
moveObjectByPad()
{
	SVECTOR v1;
	MATRIX  tmp1;
	static GsCOORDINATE2 *pDWorld = &DWorld2;
	static VECTOR *pScale = &Scale2;
	static int f_OnStart = 0;
	static int f_Obj = 0;
	static int f_OnSelect  = 0;
	static int f_OnSelect2 = 0;
	static int f_Render = 0;
	static int f_OnRleft  = 0;

	/* Rotate X */
	if ((padd & PADLup) > 0)
		Rotation.vx += 5 * ONE / 360;
	if ((padd & PADLdown) > 0)
		Rotation.vx -= 5 * ONE / 360;

	/* Rotate Y */
	if ((padd & PADLleft) > 0)
		Rotation.vy -= 5 * ONE / 360;
	if ((padd & PADLright) > 0)
		Rotation.vy += 5 * ONE / 360;

	if ((padd & PADRleft) > 0)	{
		if(!f_OnRleft) {
			r0.h = 12;
			r1.h = 8;
			f_Wave++;
			if(f_Wave > 2) {
				f_Wave = 0;
			}
		}
		f_OnRleft = 1;
	} else {
		f_OnRleft = 0;
	}

	if ((padd & PADRup) > 0)	{
		if(f_Auto == 0) {
			f_Ripple = 1;
		}
	}

	if ((padd & PADRdown) > 0) {
		f_Auto = 0;
		f_Ripple = 0;
	}
	if ((padd & PADRright) > 0) {
		f_Auto = 1;
	}

	/* Refrection rate */
	if ((padd & PADR1) > 0)
		if(reflect < 255) reflect += 5;
	if ((padd & PADR2) > 0)
		if(reflect > 0) reflect -= 5;

	/* Translate X */
	if ((padd2 & PADLleft) > 0)
		pDWorld->coord.t[0] += 100;
	if ((padd2 & PADLright) > 0)
		pDWorld->coord.t[0] -= 100;

	/* Translate Y */
	if ((padd2 & PADLdown) > 0)
		pDWorld->coord.t[1] += 100;
	if ((padd2 & PADLup) > 0)
		pDWorld->coord.t[1] -= 100;

	/* Translate Z */
	if ((padd2 & PADm) > 0)
		pDWorld->coord.t[2] -= 200;
	if ((padd2 & PADl) > 0)
		pDWorld->coord.t[2] += 200;

	/* Rotate X */
	if ((padd2 & PADRup) > 0) {
		pScale->vx += 100;
		pScale->vy += 100;
		pScale->vz += 100;
	}
	if ((padd2 & PADRdown) > 0) {
		pScale->vx -= 100;
		pScale->vy -= 100;
		pScale->vz -= 100;
	}

	if ((padd & PADstart) > 0) {
		if(!f_OnStart) {
#if 1
			/* select scene */
			f_Mode++;
			if(f_Mode > 3) {
				f_Mode = 1;
			}
			v_cnt = cnt_mode[f_Mode];
			if(f_Auto == 0 && f_Mode != 0) {
				changeMode(f_Mode - 1);
			}
#else
			/* restart demo */
			f_Auto ^= 1;
			if(f_Auto) {	/* initialize */
				v_cnt = cnt_mode[1];
			} else {
				f_Black = 1;
				ripple_init();
				f_Ripple = 0;
			}
#endif
		}
		f_OnStart = 1;
	} else {
		f_OnStart = 0;
	}

	/* exit program */
	if ((padd & PADselect) > 0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}

	if ((padd2 & PADselect) > 0) {
		if(!f_OnSelect2) {
			f_Render ^= 1;
			if(f_Render) {
				GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L2;
			} else {
#if TUNE == 3
				GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L;
#else
				GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L;
#endif
			}
		}
		f_OnSelect2 = 1;
	} else {
		f_OnSelect2 = 0;
	}

	/* Rotate 2nd Object */
	Rotation2.vx -= 2 * ONE / 360;
	Rotation2.vy -= 3 * ONE / 360;
	Rotation2.vz -= 4 * ONE / 360;

	/* Refrection rate */
	if ((padd & PADL1) > 0)
		if(refract < 1023)	refract += 5;
	if ((padd & PADL2) > 0)
		if(refract > 0)		refract -= 5;

#if 0
	if ((padd2 & PADstart) > 0) {
		printf("reflect:%d refract:%d\n", reflect, refract);
		printf("OBJ1(%d,%d,%d)(%d,%d,%d)(%d,%d,%d)\n", 
				Scale.vx, Scale.vy, Scale.vz, 
				DWorld.coord.t[0], DWorld.coord.t[1], DWorld.coord.t[2],
				Rotation.vx, Rotation.vy, Rotation.vz);
		printf("OBJ2(%d,%d,%d)(%d,%d,%d)(%d,%d,%d)\n", 
				Scale2.vx, Scale2.vy, Scale2.vz, 
				DWorld2.coord.t[0], DWorld2.coord.t[1], DWorld2.coord.t[2],
				Rotation2.vx, Rotation2.vy, Rotation2.vz);
	}
#endif

	/* Calculate Matrix from Object Parameter and Set Coordinate */
	set_coordinate(&Rotation,  &Scale, &DWorld);
	set_coordinate(&Rotation2, &Scale2, &DWorld2);
	return 1;
}

/***********************************************************/ 
/* Initialize routines */
/***********************************************************/ 
init_all()
{
	GsFOGPARAM fgp;

#ifdef CDROM
	SsInit();
	CdInit();
#endif CDROM

	ResetGraph(0);		/* Reset GPU */

	/* Reset Controller */
	PadInit(0);

	padd = 0;		/* init value of controller */

	GsInitGraph(640, 240, GsOFSGPU, 1, 0);
	GsDefDispBuff(0, 0, 0, 240);

	GsInit3D();		/* Init 3D system */

	Wot0[0].length = OT_LENGTH;	/* Set bit length of OT handler */
	Wot0[0].org = zsorttable0[0];	/* Set Top address of OT */
	Wot0[1].length = OT_LENGTH;
	Wot0[1].org = zsorttable0[1];

	Wot00[0].length = OT_LENGTH;	/* Set bit length of OT handler */
	Wot00[0].org = zsorttable00[0];	/* Set Top address of OT */
	Wot00[1].length = OT_LENGTH;
	Wot00[1].org = zsorttable00[1];

	Wot1[0].length = OT_LENGTH;
	Wot1[0].org = zsorttable1[0];
	Wot1[1].length = OT_LENGTH;
	Wot1[1].org = zsorttable1[1];

	Wot2[0].length = OT_LENGTH;
	Wot2[0].org = zsorttable2[0];
	Wot2[1].length = OT_LENGTH;
	Wot2[1].org = zsorttable2[1];

	coord_init();		/* Init coordinate */

#ifdef CDROM
	/* Read data file from CD-ROM */
	readFile("\\DATA\\SKY.TIM;1",     TEX0_ADDR);
	readFile("\\DATA\\TILE.TIM;1",    TEX2_ADDR);
	readFile("\\DATA\\BLACK.TIM;1",   TEX4_ADDR);
	readFile("\\DATA\\GWAVE14.TMD;1", MODEL1_ADDR);
	readFile("\\DATA\\CLOCK.TMD;1",   MODEL2_ADDR);
	CdControl(CdlStop, (void *)0, 0);
#endif CDROM

	model_init();		/* Reading modeling data */
	view_init();		/* Setting view point */
	light_init();		/* Setting Flat Light */

	texture_init(TEX0_ADDR); /* texture load of TEX0_ADDR */
	texture_init(TEX1_ADDR); /* texture load of TEX1_ADDR */
	texture_init(TEX2_ADDR); /* texture load of TEX2_ADDR */

	ripple_init();

	/* setting FOG parameters */
	fgp.dqa = -10000 * ONE / 64 / 1000;
	fgp.dqb = 5 / 4 * ONE * ONE;
	fgp.rfc = fgp.gfc = fgp.bfc = 0;
	GsSetFogParam(&fgp);

	/* init environment mapping variable */
	env1_init();
	env2_init();

	/* setting jumptable for GsSortObject4J() */
	jt_init4();
}

/*--- Setting view point */
view_init()
{
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
}

/*--- Setting view point */
view2_update()
{
	/* Setting view point location */
	view2.vpx = DWorld.coord.t[0];
	view2.vpy = DWorld.coord.t[1];
	view2.vpz = DWorld.coord.t[2];

	/* Setting focus point location */
	view2.vrx = 0;
	view2.vry = 0;
	view2.vrz = 0;

	/* Setting bank of SCREEN */
	view2.rz = 0;

	/* Setting parent of viewing coordinate */
	view2.super = WORLD;
}

/*--- init light */
light_init()
{
	/* Setting Light ID 0 */
	/* Setting direction vector of Light0 */
	pslt[0].vx = 0;
	pslt[0].vy = 100;
	pslt[0].vz = -100;

	/* Setting color of Light0 */
	pslt[0].r = 0xf0;
	pslt[0].g = 0xf0;
	pslt[0].b = 0xf0;

	/* Set Light0 from parameters */
	GsSetFlatLight(0, &pslt[0]);


	/* Setting Light ID 1 */
	pslt[1].vx = 0;
	pslt[1].vy = 100;
	pslt[1].vz = 100;
	pslt[1].r = 0xf0;
	pslt[1].g = 0xf0;
	pslt[1].b = 0xf0;
	GsSetFlatLight(1, &pslt[1]);

	/* Setting Light ID 2 */
	pslt[2].vx = -30;
	pslt[2].vy = 0;
	pslt[2].vz = -100;
	pslt[2].r = 0;
	pslt[2].g = 0;
	pslt[2].b = 0;
	GsSetFlatLight(2, &pslt[2]);

	/* Setting Ambient */
	GsSetAmbient(1800, 1800, 1800);

	/* Setting default light mode */
	GsSetLightMode(0);
}

/*--- Setting coordinate for each object */
coord_init()
{
	/* Setting hierarchy of Coordinate */
	GsInitCoordinate2(WORLD, &DWorld);
	GsInitCoordinate2(WORLD, &DWorld2);

	/* Init work vector */
	Rotation.vx = -ONE/4; Rotation.vy = ONE/2; Rotation.vz = 0;
	Rotation2.vx = Rotation2.vy = Rotation2.vz = 0;

	/* set initial position for each objects */
	DWorld.coord.t[2] = -12000;
	DWorld2.coord.t[0] = -2000;
	DWorld2.coord.t[2] = -8000;

	Scale.vx = Scale.vy = Scale.vz = ONE;
	Scale2.vx = Scale2.vy = Scale2.vz = ONE;
}

/*
--- Set coordinte parameter from work vector */
set_coordinate(rot, scale, coor)
	SVECTOR *rot;		/* rotation work vector */
	VECTOR *scale;		/* scale work vector */
	GsCOORDINATE2 *coor;	/* Coordinate */
{
	MATRIX  tmp1;

	/* Set translation */
	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	/* Rotate Matrix */
	RotMatrix(rot, &tmp1);
	ScaleMatrix(&tmp1, scale);

	/* Set Matrix to Coordinate */
	coor->coord = tmp1;

	/* Clear flag becase of changing parameter */
	coor->flg = 0;
}


/*--- Load texture to VRAM */
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


/*--- Read modeling data (TMD FORMAT) */
model_init()
{
	u_long *dop;
	GsDOBJ2 *objp;		/* handler of object */
	int     i;

	dop = (u_long *) MODEL1_ADDR;/* Top Address of MODELING DATA(TMD FORMAT) */

	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* Mapping real address */

	dop++;
	Objnum = *dop;		/* Get number of Objects */

	dop++;			/* Adjusting for GsLinkObject4J */

	G_obj = *((Object*)dop); /**/

	/* Link ObjectHandler and TMD FORMAT MODELING DATA */

	GsLinkObject4((u_long) dop, &object[0], 0);

	objp = object;

	/* Set Coordinate of Object Handler */
	objp->coord2 = &DWorld;

	/* material attenuation setting */
	objp->attribute =  GsLLMOD | GsMATE | GsLDIM4;


	/********************************/
	/*	for second TMD		*/
	/********************************/

	dop = (u_long *) MODEL2_ADDR;/* Top Address of MODELING DATA(TMD FORMAT) */

	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* Mapping real address */

	dop++;
	Objnum = *dop;		/* Get number of Objects */

	dop++;			/* Adjusting for GsLinkObject4J */

	/* Link ObjectHandler and TMD FORMAT MODELING DATA */

	GsLinkObject4((u_long) dop, &object[1], 0);

	objp = object+1;

	/* Set Coordinate of Object Handler */
	objp->coord2 = &DWorld2;

	/* material attenuation setting */
	objp->attribute =  0;
}

env1_init()
{
	drenv1 = GsDRAWENV;
	drenv1.clip.x = 768;
	drenv1.clip.y = 256;
	drenv1.clip.w = 256;
	drenv1.clip.h = 256;
	drenv1.ofs[0] = 768+128;
	drenv1.ofs[1] = 256+128;

	envrect1.x = 768;
	envrect1.y = 256;
	envrect1.w = 256;
	envrect1.h = 256;
}

env2_init()
{
	drenv2 = GsDRAWENV;
	drenv2.clip.x = 768;
	drenv2.clip.y = 0;
	drenv2.clip.w = 256;
	drenv2.clip.h = 256;
	drenv2.ofs[0] = 768+128;
	drenv2.ofs[1] = 0+128;

	envrect2.x = 768;
	envrect2.y = 0;
	envrect2.w = 256;
	envrect2.h = 256;
}

/**********************************************************
	Init jump table for GsSortObj4J()
 **********************************************************/
jt_init4()
{
	/* Gs SortObject4J Fook Func (for material attenuation) */
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
#if TUNE == 3
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L;
#else
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L;
#endif
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

/*
 *      read data file from CD-ROM
 */
readFile(char *fname, u_long addr)
{
	CdlFILE finfo;
	int r;
	int nsector;

	/* Search file on the CD-ROM */
	r = (int)CdSearchFile(&finfo, fname);

	/* Read data */
	nsector = (finfo.size+2047)/2048;
	CdControl(CdlSetloc, (void *)&(finfo.pos), 0);
	CdRead(nsector, (void *)addr, CdlModeSpeed);
	while(CdReadSync(1,0) > 0) {
		VSync(0);
	}
}

/*********************************************************************
	RIPPLE
 *********************************************************************/

ripple_init()
{
	int i;
	long r;
	SVECTOR v;
	long max;

	/* initialize parameters */
	r0.h  = 12;
	r0.aa = 5;
	r0.a1 = 256*6;
	r0.a2 = 256*12;
	r0.ws = 3;
	r0.a0 = r0.a = 0;
	r0.top = 0;

	/* initialize height buffer */
	for(i = 0; i < NRING; i++) {
		r0.h_ring[i] = 0;
		r1.h_ring[i] = 0;
	}

	/* initialize distance table */
#define R  (1024+30)
#define R2 (2048+60)

	/* wave 1 */
	max = (long)((float)R*1.41421356)+1;
	for(i=0; i< G_obj.n_vert; i++) {
		GetTmdVertex(i, &v);
		r = v.vx*v.vx+v.vz*v.vz;
		r = SquareRoot0(r);
		r = (unsigned char)((float)r/max*NRING);
		r0.d_tbl[i] = r;
	}

	/* wave 2 */
	max = (long)((float)R2*1.41421356)+1;
	for(i=0; i< G_obj.n_vert; i++) {
		GetTmdVertex(i, &v);
		r = (v.vx+R)*(v.vx+R)+(v.vz+R)*(v.vz+R);
		r = SquareRoot0(r);
		r = (unsigned char)((float)r/max*NRING);
		r1.d_tbl[i] = r;
	}
}

ripple()
{
	RIPPLE *r;
	int j0, j1, j2;
	int i;
	int n;
	short z;

	r = &r0;
	r->top -= r->ws; if(r->top < 0) r->top += NRING;
	for(i=0; i< r->ws; i++) {		/* r->ws : speed of wave */
		n = r->top - i;
		if(n < 0) n += NRING;
		r->h_ring[n] = (short)((float)(m_sin(r->a)/(32*16))*r->h);
		r->a0 += r->aa;			/* speed of amplitude's */
		if(r->a0 < r->a1) {		r->a = r->a0%256;
		} else if (r->a0 < r->a2) {	r->a = 0;
		} else {			r->a0 = r->a = 0;
		}
	}

	r = &r1;
	r->top -= r->ws; if(r->top < 0) r->top += NRING;
	for(i=0; i< r->ws; i++) {		/* r->ws : speed of wave */
		n = r->top - i;
		if(n < 0) n += NRING;
		r->h_ring[n] = (short)((float)(m_sin(r->a)/(32*16))*r->h);
		r->a0 += r->aa;			/* speed of amplitude's */
		if(r->a0 < r->a1) {		r->a = r->a0%256;
		} else if (r->a0 < r->a2) {	r->a = 0;
		} else {			r->a0 = r->a = 0;
		}
	}

	/* mix waves */
	if(f_Wave == 0) {
		for(i=0; i< G_obj.n_vert; i++) {
			j0 = r0.d_tbl[i] + r0.top; if(j0 >= NRING) j0 -= NRING;
			j1 = r1.d_tbl[i] + r1.top; if(j1 >= NRING) j1 -= NRING;
			z = r0.h_ring[j0] + r1.h_ring[j1];
			SetTmdVertexY(i, z);
		}
	} else if(f_Wave == 1) {
		for(i=0; i< G_obj.n_vert; i++) {
			j0 = r0.d_tbl[i] + r0.top; if(j0 >= NRING) j0 -= NRING;
			j1 = r1.d_tbl[i] + r1.top; if(j1 >= NRING) j1 -= NRING;
			z = r0.h_ring[j0];
			SetTmdVertexY(i, z);
		}
	} else if(f_Wave == 2) {
		for(i=0; i< G_obj.n_vert; i++) {
			j0 = r0.d_tbl[i] + r0.top; if(j0 >= NRING) j0 -= NRING;
			j1 = r1.d_tbl[i] + r1.top; if(j1 >= NRING) j1 -= NRING;
			z = r1.h_ring[j1];
			SetTmdVertexY(i, z);
		}
	}
}

/**********************************************************************/

g3_normal()
{
	int i, j, k;
	SVECTOR vn;
	SVECTOR vn2;

	/* calculate normals of polygons */
	for(i=0;  i< G_obj.n_primitive; i++) {
		CalcTmdNormal(i, &Normal[i]);
	}

	/* calculate normals of vertices */
	for(i=0;  i< G_obj.n_normal; i++) {
		vn.vx = 0;
		vn.vy = 0;
		vn.vz = 0;
		for(j = 1; j < 7; j++) {
			k = G_link[i][j];
			if(k) {
				vn.vx += Normal[k-1].vx;
				vn.vy += Normal[k-1].vy;
				vn.vz += Normal[k-1].vz;
			}
		}
		VectorNormalSS(&vn, &vn2);
		SetTmdNormal(i, (SVECTOR *)&vn2);
	}
}

#define SHIFT 4
CalcTmdNormal(int n, SVECTOR *vn1)
{
	int n0, n1, n2;
	int nn;
	int nn0, nn1, nn2;

	SVECTOR v0, v1, v2;
	VECTOR a0, a1, vn0;

#if 0
	/* check PolyG3 */
	if( (long)(*(G_obj.primitive_top + n*5)) !=  0x30000406) {
		vn1->vx = 0;
		vn1->vy = 4096;
		vn1->vz = 0;
		return 0;
	}
#endif

	n0 = (0xffff0000 & (long)(*(G_obj.primitive_top + n*5+2))) >> 16;
	n1 = (0xffff0000 & (long)(*(G_obj.primitive_top + n*5+3))) >> 16;
	n2 = (0xffff0000 & (long)(*(G_obj.primitive_top + n*5+4))) >> 16;

	GetTmdVertex(n0, &v0);
	GetTmdVertex(n1, &v1);
	GetTmdVertex(n2, &v2);

	a0.vx = (v1.vx - v0.vx) >> SHIFT;
	a0.vy = (v1.vy - v0.vy) >> SHIFT;
	a0.vz = (v1.vz - v0.vz) >> SHIFT;
	a1.vx = (v2.vx - v0.vx) >> SHIFT;
	a1.vy = (v2.vy - v0.vy) >> SHIFT;
	a1.vz = (v2.vz - v0.vz) >> SHIFT;

#if TUNE > 1
	gte_OuterProduct0(&a0, &a1, &vn0);
#else
	OuterProduct0(&a0, &a1, &vn0);
#endif
	VectorNormalS(&vn0, vn1);

	vn1->vx = - vn1->vx;
	vn1->vy = - vn1->vy;
	vn1->vz = - vn1->vz;
}

SetTmdVertexY(int n, short y)
{
	*((short *)(G_obj.vert_top + n*2)+1) = y;
}

GetTmdVertex(int n, SVECTOR *v)
{
	*v = *(SVECTOR *)(G_obj.vert_top + n*2);
}

SetTmdNormal(int n, SVECTOR *v)
{
	*(SVECTOR *)(G_obj.normal_top + n*2) = *v;
}


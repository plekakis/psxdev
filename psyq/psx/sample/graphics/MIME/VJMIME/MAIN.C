/* $PSLibId: Run-time Library Release 4.4$ */
#if defined(S0) | defined(L0)
	static char	*progname="graphics/mime/vjmime/vjmime0.cpe";
#endif
#if defined(S1) | defined(L1)
	static char	*progname="graphics/mime/vjmime/vjmime1.cpe";
#endif
#if defined(S0) | defined(S1)
#define	APD_SAVE
#endif
#if defined(L0) | defined(L1)
#define	APD_LOAD
#endif

/***********************************************
 *		vdf-axesmime sample
 *
 * sample program of vdf MIMe and MIMe animation of an object having a
 * hierarchical structure with respect to a coordinate system */
/***********************************************
 *	main.c
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

#include "paddef.h"
#include "common.h"
#include "model.h"

/* resolution of OT */
#define OT_LENGTH 8
#define WOT_LEN 2

/* Background Color */
#define BGR 80
#define BGG 80
#define BGB 80

/* View point parameters*/
#define DEFAULT_PROJ 1000
#define DEFAULT_VPZ (-6000)
#define DEFAULT_VPX 0
#define DEFAULT_VPY 0
#define DEFAULT_VRX 0
#define DEFAULT_VRY 0
#define DEFAULT_VRZ 0
#define DEFAULT_RZ 0

/* Light Sources parameteres*/
#define DEFAULT_AMBR   3000
#define DEFAULT_AMBG   2300
#define DEFAULT_AMBB   2300
#define DEFAULT_0VX    20
#define DEFAULT_0VY    100
#define DEFAULT_0VZ    100
#define DEFAULT_0R    150
#define DEFAULT_0G    DEFAULT_0R
#define DEFAULT_0B    DEFAULT_0R
#define DEFAULT_1VX    20
#define DEFAULT_1VY    (-50)
#define DEFAULT_1VZ    (-100)
#define DEFAULT_1R    0x80
#define DEFAULT_1G    DEFAULT_1R
#define DEFAULT_1B    DEFAULT_1R
#define DEFAULT_2VX    (-100)
#define DEFAULT_2VY    (-20)
#define DEFAULT_2VZ    (-50)
#define DEFAULT_2R    0x60
#define DEFAULT_2G    DEFAULT_2R
#define DEFAULT_2B    DEFAULT_2R

#define PACKETMAX (3*24)
static PACKET out_packet[2][PACKETMAX];
static GsOT            Wot[2];			/* Handler of OT*/
static GsOT            gsot[2][DISPNUM];		/* Handler of OT*/

int main(void);
int init_all(void);
void finish_all(void);
int obj_interactive(void);
int draw_init(void);
void fnt_init(void);
void view_init(void);
void light_init(void);
static void set_lights(short *a, GsF_LIGHT *l);

int main(void)
{
    u_long fn;
    int outbuf_idx;				/* double buffer index */
    GsOT *cwot;					/* current Wot */
    int v0, v1;
    int i;

    if (init_all()<0) return 0;			/* call initialize functions */


    for (fn=0; ; fn++){				/* main loop */
	outbuf_idx=GsGetActiveBuff();		/* Get double buffer index*/
	cwot= &Wot[outbuf_idx];			/* Current OT */
	GsSetWorkBase(&out_packet[outbuf_idx][0]);
	GsClearOt(0,0,cwot);			/* Clear OT for using buffer*/
	for (i=0; i<DISPNUM; i++){
	    GsClearOt(0,0,&gsot[outbuf_idx][i]); /* Clear OT for using buffer*/
	}
	v0=VSync(1);
	if (obj_interactive()<0) break;		/* pad operation */
	v1=VSync(1);

	model(&gsot[outbuf_idx][0], OT_LENGTH); /* add models' entry to OT */


	FntPrint("proceed time:%d\n", v1-v0);
	for (i=0; i<DISPNUM; i++){
	    GsSortOt(&gsot[outbuf_idx][i], cwot);
	}
	DrawSync(0);				/* wait for end of drawing*/
	VSync(0);
	GsSwapDispBuff();			/* Swap double buffer*/
	GsSortClear(BGR,BGG,BGB,cwot);
/*	DumpOTag((u_long *)cwot->tag);*/
	GsDrawOt(cwot);				/* Drawing OT*/
	FntFlush(-1);
    }

    finish_all();				/* call functions for finish */

    return 0;
}

/* initialize functions */
int init_all(void)
{
    ResetCallback();
    PadInit(0);
    ResetGraph(0);		/* Reset GPU */

    draw_init();
    fnt_init();    
    light_init();
    if (model_init()<0) return -1;			/* in "tuto?.c" */
    view_init();

    return 0;
}

/* functions for finish */
void finish_all(void)
{
   PadStop();
    ResetGraph(3);
    StopCallback();
}

/* pad operations */
int obj_interactive(void)
{
    u_long padd;

    padd=PadRead(0);

    /* finish this program*/
    if (((padd & (A_PADstart|A_PADselect))== (A_PADstart|A_PADselect))
	|| ((padd & (B_PADstart|B_PADselect))== (B_PADstart|B_PADselect))){
	return -1;
    }

    /* operations of model/MIMe */
    if (model_move(padd)<0){			/* in tuto?.c */
	return -1;
    }

    return 0;
}

/***********************************************
 *	draw functions
 ***********************************************/
static GsOT_TAG	zsorttable[2][DISPNUM][1<<OT_LENGTH];	/* Area of OT*/
static GsOT_TAG	wzsorttable[2][1<<WOT_LEN];	/* Area of OT*/
/* initialize for drawing */
int draw_init(void)
{
    int i;
    
    /* set resolution (interlace mode)*/
    GsInitGraph(640,240,GsOFSGPU|GsINTER,1,0);
    GsDefDispBuff(0,0,0,240);	/* Double buffer setting */

    GsInit3D();			/* Init 3D system */

    Wot[0].length=WOT_LEN;	/* Set bit length of OT handler */
    Wot[0].org=wzsorttable[0];	/* Set Top address of OT Area to OT handler */

    /* same setting for anoter OT handler*/
    Wot[1].length=WOT_LEN;
    Wot[1].org=wzsorttable[1];

    for (i=0; i<DISPNUM; i++){
	gsot[0][i].length=OT_LENGTH;
	gsot[0][i].org=zsorttable[0][i];
	gsot[0][i].point=2;
	gsot[1][i].length=OT_LENGTH;
	gsot[1][i].org=zsorttable[1][i];
	gsot[1][i].point=2;
    }

}

/* initialize for FntPrint()*/
void fnt_init(void)
{
    FntLoad(960, 256);
    SetDumpFnt(FntOpen(-300,-100,0,200,0,512));
}

/* Setting view point */
void view_init(void)
{
    GsRVIEW2  view;			/* View Point Handler*/

    /*---- Set projection,view ----*/
    GsSetProjection(DEFAULT_PROJ); /* Set projection */
    /* Setting view point location */
    view.vpx = DEFAULT_VPX; view.vpy = DEFAULT_VPY; view.vpz = DEFAULT_VPZ;
  
    /* Setting focus point location */
    view.vrx = DEFAULT_VRX; view.vry = DEFAULT_VRY; view.vrz = DEFAULT_VRZ;
  
    /* Setting bank of SCREEN */
    view.rz=DEFAULT_RZ;

    /* Setting parent of viewing coordinate */
    view.super = WORLD;
  
    /* Calculate World-Screen Matrix from viewing parameter
       set up viewpoint from viewpointer parameter */
    GsSetRefView2(&view);
  
    GsSetNearClip(100);           /* Set Near Clip */
}

/***********************************************
 *	set lights */
static void set_lights(short *a, GsF_LIGHT *l);

/* initialize lights */
void light_init(void)
{
    static GsF_LIGHT pslt[3];
    static short ambc[3];

    pslt[0].vx=DEFAULT_0VX;
    pslt[0].vy=DEFAULT_0VY;
    pslt[0].vz=DEFAULT_0VZ;
    pslt[1].vx=DEFAULT_1VX;
    pslt[1].vy=DEFAULT_1VY;
    pslt[1].vz=DEFAULT_1VZ;
    pslt[2].vx=DEFAULT_2VX;
    pslt[2].vy=DEFAULT_2VY;
    pslt[2].vz=DEFAULT_2VZ;

    pslt[0].b=DEFAULT_0B;
    pslt[0].g=DEFAULT_0G;
    pslt[0].r=DEFAULT_0R;
    pslt[1].b=DEFAULT_1B;
    pslt[1].g=DEFAULT_1G;
    pslt[1].r=DEFAULT_1R;
    pslt[2].b=DEFAULT_2B;
    pslt[2].g=DEFAULT_2G;
    pslt[2].r=DEFAULT_2R;

    ambc[0]=DEFAULT_AMBR;
    ambc[1]=DEFAULT_AMBG;
    ambc[2]=DEFAULT_AMBB;

    /* Setting default light mode */
    GsSetLightMode(0);

    set_lights(ambc, pslt);
}

/* set light parameters */
static void set_lights(short *a, GsF_LIGHT *l)
{
    int i;

    GsSetAmbient(a[0], a[1], a[2]);
    for (i=0; i<3; i++){
	GsSetFlatLight(i,&l[i]);
    }
}

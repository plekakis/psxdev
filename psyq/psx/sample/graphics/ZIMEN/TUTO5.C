/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *
 *
 *	"tuto0.c" loop 3DBG (FOG MAP DATA)
 *
 *		Version 1.00	Jul,  14, 1994
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>	

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#include "struct.h"

/***************************************************************************
 * 
 *	BG Handler*/
/* BG-CELL structure
 * This example uses only one clut. Therefore "clut" member does not used here.
 * This example does not use "attr" member. See bgmap.h */
extern	u_long	bgtex[];	/* BG texture, CLUT (4bit) */
extern	u_long	bgtex8[];	/* BG texture, CLUT (8bit) */

#define PACKETMAX 4000		/* Max GPU packets */

#define OBJECTMAX 100		/* Max Objects */

#define PACKETMAX2 (PACKETMAX*24) /* size of PACKETMAX (byte)
                                     paket size may be 24 byte(6 word) */

#define OT_LENGTH  4		/* bit length of OT */


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Area of OT */

#include "bgmap.h"		/* 2D array about the cell pattern */
Gs3DBG0		gobj = {32, 32, 10, _map, _ctype};
GsDIVCOND0      cond;
GsDPCLUT0       ClutHandle;

u_long divz[6] = {3,2,2,2,1,1}; /* shift length */

#define CBIT 5
#define CNUM (1<<CBIT)
#define BGR 200
#define BGG 200
#define BGB 200

GsCOORDINATE2   DJimen;  /* Coordinate for Gs3DBG */

SVECTOR         PJimen; /* work short vector for making Coordinate parmeter */
SVECTOR         PView; /* work short vector for making Coordinate parmeter */

GsCOORDINATE2   DLocal;		/* local coordinate for setting viewpoint */
SVECTOR         PLocal;		/* rotation work vector */


extern MATRIX GsIDMATRIX,GsIDMATRIX2;	/* Unit Matrix */
extern PACKET *GsOUT_PACKET_P;

GsVIEW2  view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler */
u_long padd;			/* Controler data */

PACKET		out_packet[2][PACKETMAX2];  /* GPU PACKETS AREA */

u_long Projection = 800;


#define DEBUG

/************* MAIN START ******************************************/
tuto5()
{
  int     i;

  int     outbuf_idx;		/* double buffer index */
  MATRIX  tmpls,tmplw;
#ifdef DEBUG
  int ratch_vcount;
#endif
  ResetCallback();
  init_all();
  SetGraphDebug(0);
  
/*  KanjiFntOpen(-320+32, -240+32, 640-64, 200, 704, 0, 768, 256, 0, 512); */
  FntLoad(960, 256);
/*  SetDumpFnt(FntOpen(-320+32, -240+32, 640-64, 200, 0, 512)); */
  FntOpen(-320+32, -240+32, 640-64, 200, 0, 512);
  
  while((PadRead(1)&PADselect) == 0) 
    {
      obj_interactive2();	/* interactive parameter get */

      GsSetView2(&view);	/* Calculate World-Screen Matrix */

      outbuf_idx=GsGetActiveBuff();/* Get double buffer index */

      /* Set top address of Packet Area for output of GPU PACKETS */
      GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);

      GsClearOt(0,0,&Wot[outbuf_idx]); /* Clear OT for using buffer */

      /* Calculate Local-World Matrix */
	  GsGetLw(gobj.coord2,&tmplw);
      
      /* Set LWMATRIX to GTE Lighting Registers */
	GsSetLightMatrix(&tmplw);
      
      /* Calculate Local-Screen Matrix */
	GsGetLs(gobj.coord2,&tmpls);
      
#ifdef DEBUG
      ratch_vcount = VSync(1);
#endif
      GsSort3DBG0_DPQ(&gobj,&Wot[outbuf_idx],14-OT_LENGTH,&tmpls,&ClutHandle);
#ifdef DEBUG
      FntPrint("load = %d\n%d (%d %d %d)\n",VSync(1)-ratch_vcount,
		    PLocal.vy,
		    DLocal.coord.t[0],DLocal.coord.t[1],DLocal.coord.t[2]);
#endif
      VSync(0);			/* Wait VSYNC */

/*      DrawSync(0);*/
      ResetGraph(1);
      padd=PadRead(1);		/* Readint Control Pad data */
      GsSwapDispBuff();		/* Swap double buffer */

      /* Set SCREEN CLESR PACKET to top of OT */
      GsSortClear(BGR, BGG, BGB, &Wot[outbuf_idx]);

      
      /* Drawing OT */
      GsDrawOt(&Wot[outbuf_idx]);
      FntFlush(-1);
    }
    DrawSync(0);
    return;
}


obj_interactive2()
{
  MATRIX  tmpm;
  SVECTOR dd;
  VECTOR  tmpv;

  dd.vx = dd.vy = dd.vz = 0;
  
  if((padd & PADRright)>0)
    PLocal.vy+=ONE/360/2;
  
  if((padd & PADRleft)>0)
    PLocal.vy-=ONE/360/2;

  set_coordinate(&PLocal,&DLocal);
  
  /* Translate Z */
  if((padd & PADR1)>0)
    dd.vz = -30;
  
  /* Translate Z */
  if((padd & PADR2)>0)
    dd.vz = 30;
  
  /* Translate X */
  if((padd & PADLleft)>0)
    dd.vx = -30;
  
  /* Translate X */
  if((padd & PADLright)>0)
    dd.vx = 30;

  /* Translate Y */
  if((padd & PADLdown)>0)
    dd.vy = 10;
  
  /* Translate Y */
  if((padd & PADLup)>0)
    dd.vy = -10;

  /* translate the translation in local to world */
  ApplyMatrix(&DLocal.coord,&dd,&tmpv);
  DLocal.coord.t[0]+=tmpv.vx;
  DLocal.coord.t[1]+=tmpv.vy;
  DLocal.coord.t[2]+=tmpv.vz;

  /* change effective area */ 
  if((padd & PADo)>0)
    {
      gobj.nw +=1;
      gobj.nh +=1;
    }
  if((padd & PADn)>0 && gobj.nw>1)
    {
      gobj.nw -=1;
      gobj.nh -=1;
    }
  
  /* activate FOG around the edge of the area */
  SetFogFar(gobj.ch*gobj.nw/2,Projection);
}


/* Initialize routine */
init_all()
{
  padd=0;			/* controler value initialize */
  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  /* rezolution set , interrace mode */

  GsDefDispBuff(0,0,0,0);	/* Double buffer setting */

#if 0
  GsInitGraph(320,240,GsINTER|GsOFSGPU,1,0);
  /* rezolution set , non interrace mode */
  GsDefDispBuff(0,0,0,240);	/* Double buffer setting */
#endif

  GsInit3D();			/* Init 3D system */
  
  Wot[0].length=OT_LENGTH;	/* Set bit length of OT handler */

  Wot[0].org=zsorttable[0];	/* Set Top address of OT Area to OT handler */
  
  /* same setting for anoter OT handler */
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  
  coord_init();			/* Init coordinate */
  model_init();			/* Reading modeling data */  
  view_init();			/* Setting view point */
  light_init();			/* Setting Flat Light */
  texture_init();		/* load texture */
}


view_init()			/* Setting view point */
{
  /*---- Set projection,view ----*/
  GsSetProjection(Projection);	/* Set projection */
  
  view.view  = GsIDMATRIX2;
  view.super = &DLocal;
  view.view.t[2] = -100;
  view.view.t[1] = 0;
  view.view.t[0] = 0;
  PView.vx=PView.vy=PView.vz=0;
  /* Calculate World-Screen Matrix from viewing paramter */
  GsSetView2(&view);
}


light_init()			/* init Flat light */
{
  /* Setting Light ID 0 */
  /* Setting direction vector of Light0 */
  pslt[0].vx = 20; pslt[0].vy= -100; pslt[0].vz= -100;
  
  /* Setting color of Light0 */
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  
  /* Set Light0 from parameters */
  GsSetFlatLight(0,&pslt[0]);

  
  /* Setting Light ID 1 */
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= 100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /* Setting Light ID 2 */
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= -100;
  pslt[2].r=0x60; pslt[2].g=0x60; pslt[2].b=0x60;
  GsSetFlatLight(2,&pslt[2]);
  
  /* Setting Ambient */
  GsSetAmbient(0,0,0);

  /* Setting default light mode */
  GsSetLightMode(0);

  /* set fog parmaeter */
  SetFogNear(8000,Projection);	/* default */
}

coord_init()			/* Setting coordinate */
{
  /* Setting hierarchy of Coordinate */
  GsInitCoordinate2(WORLD,&DJimen);
  /* Init work vector */
  PJimen.vx= -90*ONE/360;	/* rotate the gournd 90 degree around x */
  PJimen.vy= PJimen.vz=0;
  set_coordinate(&PJimen,&DJimen);

  GsInitCoordinate2(WORLD,&DLocal);
  DLocal.coord.t[1] = -200;
  PLocal.vx=PLocal.vy=PLocal.vz=0;
}

/* Set coordinte parameter from work vector */
set_coordinate(pos,coor)
SVECTOR *pos;			/* work vector */
GsCOORDINATE2 *coor;		/* Coordinate */
{
  MATRIX tmp1;
  
  /* Set translation */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  
  /* Rotate Matrix */
  RotMatrix(pos,&tmp1);
  
  /* Set Matrix to Coordinate */
  coor->coord = tmp1;

  /* Clear flag becase of changing parameter */
  coor->flg = 0;
}


/* Load texture to VRAM */
texture_init()
{
	u_short		clut, tpage;
	int i;
  /* load texture data and texture clut */

#ifdef BG_4bit	/* 4bit mode */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 480);
#else		/* 8bit mode */
	tpage = LoadTPage(bgtex8+0x80, 1, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex8, 0, 480);
#endif

	ClutHandle.cbit   = CBIT;
	ClutHandle.clut   = (u_short *)bgtex8;
	ClutHandle.rectc.x = 640;
	ClutHandle.rectc.y = 480;
	ClutHandle.rectc.w = 256;
	ClutHandle.rectc.h = 1<<CBIT;
	ClutHandle.bgc.r=BGR;
	ClutHandle.bgc.g=BGG;
	ClutHandle.bgc.b=BGB;
	GsMakeDPClut0(&ClutHandle);
	
	for (i = 0; i < gobj.nctype; i++) {
		gobj.ctype[i].clut = &clut;
		gobj.ctype[i].tpage = tpage;
	}
}


/* Read modeling data (TMD FORMAT) */
model_init()
{
  /* Set Coordinate of Object Handler */
    gobj.coord2 =  &DJimen;
    gobj.cw = 1024;		/* widthe of the cell */
    gobj.ch = 1024;		/* height of the cell */
    gobj.nw = 16;		/* the number of the cell of the width of
				   the map */
    gobj.nh = 16;		/* the number of the cell of the height of
				   the map */
    gobj.iw = 0x1f;		/* repeat unit of the map (x) */
    gobj.ih = 0x1f;		/* repeat unit of the map (y) */
    gobj.cond = &cond;
    
				/* the condition of active sub divide */
    cond.nearz = Projection/4;	/* z point of satart dividing */
    cond.shift = 7;		/* shift */
    cond.nz    = 6;		/* maximum divide number */
    cond.cond  = divz;		/* pointer of the table of divide number */
}

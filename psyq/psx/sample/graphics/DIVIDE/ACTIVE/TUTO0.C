/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	Active sub-divide simplest sample
 *
 *	"tuto0.c" ******** for texture gouror polygon
 *
 *		Version 1.00	Sep,  1, 1995
 *		Version 1.01	Mar,  5, 1997
 *
 *		Copyright (C) 1995  Sony Computer Entertainment
 *		All rights Reserved
 */

#include <sys/types.h>

#include <libetc.h>		/* needs to be included to use PAD */
#include <libgte.h>		/* needs to be included to use LIGS */
#include <libgpu.h>		/* needs to be included to use LIBGS */
#include <libgs.h>		/* definitions of structures for using
                    the graphics library, etc.   for LIBGS */

#define PACKETMAX 10000		/* Max GPU packets */

#define OBJECTMAX 100		/* Max Objects defines maximum number of logical
                    objects a 3D model can be divided into*/

#define PACKETMAX2 (PACKETMAX*24) /* size of PACKETMAX (byte)
                                     paket size may be 24 byte(6 word) */

#define TEX_ADDR   0x80100000	/* Top Address of texture data1 (TIM FORMAT) */

#define OT_LENGTH  8		/* resolution of ordering table */


GsOT            Wot[2];		/* ordering table handler, two are needed for double buffering */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Area of OT */

GsDOBJ2		object[OBJECTMAX]; /* Array of Object Handler */

u_long          Objnum;		/* valibable of number of Objects */


GsCOORDINATE2   DWorld;  /* coordinate system for each object */

SVECTOR         PWorld; /* work short vector for making Coordinate parmeter */

GsRVIEW2  view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler */
u_long padd;			/* Controler data */

PACKET		out_packet[2][PACKETMAX2];  /* GPU PACKETS AREA */

int Projection = 1000;

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ2 *op;			/* pointer to object handler */
  int     outbuf_idx;		/* double buffer index */
  MATRIX  tmpls;
  int vcount;
  
  ResetCallback();
  GsInitVcount();

  init_all();
  
  FntLoad(960, 256);
  SetDumpFnt(FntOpen(32-320, 32-120, 256, 200, 0, 512));
  
  while(1)
    {
				/* get motion parameters from pad data */
      if(obj_interactive()) break;

      GsSetRefView2(&view);	/* calculate world screen matrix */
      outbuf_idx=GsGetActiveBuff();/* Get double buffer index */

      /* Set top address of Packet Area for output of GPU PACKETS */
      GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);
      
      GsClearOt(0,0,&Wot[outbuf_idx]); /* Clear OT for using buffer */
	
	/* Calculate Local-World Matrix */
	GsGetLw(&DWorld,&tmpls);

      /* Set LWMATRIX to GTE Lighting Registers */
	GsSetLightMatrix(&tmpls);
      
      /* Calculate Local-Screen Matrix */
      GsGetLs(&DWorld,&tmpls);
      
      /* Set LSAMTRIX to GTE Registers */
      GsSetLsMatrix(&tmpls);
      
      /* Perspective Translate Object and Set OT */
      vcount = VSync(1);
      sort_tg_plane(out_packet[outbuf_idx],14-OT_LENGTH,
		    &Wot[outbuf_idx],getScratchAddr(0));
/*      DrawSync(0);*/
      FntPrint("Load = %d\n",VSync(1)-vcount);
      VSync(0);			/* Wait VSYNC */
      ResetGraph(1);
      padd=PadRead(1);		/* Readint Control Pad data */
      GsSwapDispBuff();		/* Swap double buffer */
      
      /* Set SCREEN CLESR PACKET to top of OT */
      GsSortClear(0x0,0x0,0x40,&Wot[outbuf_idx]);
      /* Drawing OT */
      GsDrawOt(&Wot[outbuf_idx]);
      FntFlush(-1);
    }
    PadStop();
    ResetGraph(3);
    StopCallback();
    return 0;
}


obj_interactive()
{
  SVECTOR v1;
  MATRIX  tmp1;
  
  /* Rotate Y  */
  if((padd & PADo)>0) PWorld.vy -=2*ONE/360;

  /* Rotate Y */
  if((padd & PADn)>0) PWorld.vy +=2*ONE/360;

  /* Rotate X */
  if((padd & PADRup)>0) PWorld.vx+=1*ONE/360;

  /* Rotate X */
  if((padd & PADRdown)>0) PWorld.vx-=1*ONE/360;
  
  /* Rotate Z */
  if((padd & PADRright)>0) PWorld.vz+=1*ONE/360;
  
  /* Rotate Z */
  if((padd & PADRleft)>0) PWorld.vz-=1*ONE/360;
  
  /* Translate Z */
  if((padd & PADm)>0) DWorld.coord.t[2]-=100;
  
  /* Translate Z */
  if((padd & PADl)>0) DWorld.coord.t[2]+=100;

  /* Translate X */
  if((padd & PADLleft)>0) DWorld.coord.t[0] +=10;

  /* Translate X */
  if((padd & PADLright)>0) DWorld.coord.t[0] -=10;

  /* Translate Y */
  if((padd & PADLdown)>0) DWorld.coord.t[1]+=10;

  /* Translate Y */
  if((padd & PADLup)>0) DWorld.coord.t[1]-=10;

  /* exit program */
  if((padd & PADk)>0) return -1;

  if((padd & PADh)>0)
    if(object[0].attribute == 0)
      object[0].attribute = GsDIV4;
    else
      object[0].attribute = 0;
    
  /* Calculate Matrix from Object Parameter and Set Coordinate */
  set_coordinate(&PWorld,&DWorld);

  return 0;
}


/* Initialize routine */
init_all()
{
  ResetGraph(0);		/* Reset GPU */
  PadInit(0);			/* Reset Controller */
  padd=0;			/* init controller value */

#if 0
  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  /* rezolution set , interrace mode */

  GsDefDispBuff(0,0,0,0);	/* Double buffer setting */
#endif

  GsInitGraph(640,240,GsINTER|GsOFSGPU,0,0);
  /* rezolution set , non interrace mode */
  GsDefDispBuff(0,0,0,240);	/* Double buffer setting */


  GsInit3D();			/* Init 3D system */
  
  Wot[0].length=OT_LENGTH;	/* Set bit length of OT handler */

  Wot[0].org=zsorttable[0];	/* Set Top address of OT Area to OT handler */
    
  /* same setting for anoter OT handler */
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  
  coord_init();			/* Init coordinate */
  view_init();			/* Setting view point */
  light_init();			/* Setting Flat Light */
  texture_init(TEX_ADDR);
}


view_init()			/* Setting view point */
{
  /*---- Set projection,view ----*/
  GsSetProjection(Projection);	/* Set projection */
  
  /* Setting view point location */
  view.vpx = 0; view.vpy = 0; view.vpz = 2000;
  
  /* Setting focus point location */
  view.vrx = 0; view.vry = 0; view.vrz = 0;
  
  /* Setting bank of SCREEN */
  view.rz=0;

  /* Setting parent of viewing coordinate */
  view.super = WORLD;
  
  /* Calculate World-Screen Matrix from viewing paramter */
  GsSetRefView2(&view);
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

  /* Setting Light ID 3 */
  pslt[2].vx = 0; pslt[2].vy= 100; pslt[2].vz= 0;
  pslt[2].r=0xa0; pslt[2].g=0xa0; pslt[2].b=0xa0;
  GsSetFlatLight(2,&pslt[2]);
  
  /* Setting Ambient */
  GsSetAmbient(0,0,0);

  /* Setting default light mode */
  GsSetLightMode(0);
}

coord_init()			/* Setting coordinate */
{
  VECTOR v;
  
  /* Setting hierarchy of Coordinate */
  GsInitCoordinate2(WORLD,&DWorld);
  
  /* Init work vector */
  PWorld.vx=PWorld.vy=PWorld.vz=0;
  
  /* the org point of DWold is set to Z = -40000 */
  DWorld.coord.t[2] = -4000;
}


/* Set coordinte parameter from work vector */
set_coordinate(pos,coor)
SVECTOR *pos;			/* work vector */
GsCOORDINATE2 *coor;		/* Coordinate */
{
  MATRIX tmp1;
  VECTOR v1;
  
  /* Set translation */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  
  /* set rotation in work matrix to work vector */
  
  /* Rotate Matrix */
  RotMatrix(pos,&tmp1);
  
  /* Set Matrix to Coordinate */
  coor->coord = tmp1;

  /* Clear flag becase of changing parameter */
  coor->flg = 0;
}

u_short		clut, tpage;

/* Load texture to VRAM */
texture_init(u_long *addr)
{
  GsIMAGE tim1;

  /* Get texture information of TIM FORMAT */
  GsGetTimInfo(addr+1,&tim1);
  
  /* Load texture to VRAM */
  tpage = LoadTPage(tim1.pixel,1,0,tim1.px,tim1.py,tim1.pw*2,tim1.ph);
  
  /* Exist Color Lookup Table */  
  if((tim1.pmode>>3)&0x01)
    {
      clut = LoadClut(tim1.clut,tim1.cx,tim1.cy);
    }
}

#ifdef GCC
__main()
{
  ;
}
#endif

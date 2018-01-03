/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	PMD VIEWING ROUTINE
 *
 *
 *		Version 1.00	Jul,  26, 1994
 *
 *		Copyright (C) 1994  Sony Computer Entertainment
 *		All rights Reserved
 */

#include <sys/types.h>

#include <libetc.h>		/* needs to be included to use PAD */
#include <libgte.h>		/* needs to be included to use LIGS */
#include <libgpu.h>		/* needs to be included to use LIBGS */
#include <libgs.h>		/* definitions of structures for using
                    the graphics library, etc.   for LIBGS */


#define OBJECTMAX 100		/* Max Objects defines maximum number of logical
                    objects a 3D model can be divided into*/

#define TEX_ADDR   0xa0010000	/* Top Address of texture data1 (TIM FORMAT) */

#define TIM_HEADER 0x00000010

#define MODEL_ADDR 0xa0040000	/* Top Address of modeling data (TMD FORMAT) */

#define OT_LENGTH  10		/* resolution of ordering table */


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Area of OT*/

GsDOBJ3		object[OBJECTMAX]; /* Array of Object Handler */

u_long          Objnum;		/* valibable of number of Objects */

/* object coordinate */
GsCOORDINATE2   DWorld;


SVECTOR         PWorld; /* work short vector for making Coordinate parmeter */

extern MATRIX GsIDMATRIX;	/* Unit Matrix */

GsRVIEW2  view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler */
u_long padd;			/* Controler data */

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ3 *op;			/* pointer of Object Handler */
  int     outbuf_idx;		/* double buffer index */
  MATRIX  tmpls;
  
  ResetCallback();
  init_all();
  
  while(1)
    {
      if(obj_interactive()==0)
          return 0;	/* interactive parameter get */
      GsSetRefView2(&view);	/* calculate world screen matrix*/
      outbuf_idx=GsGetActiveBuff();/* Get double buffer index */

      GsClearOt(0,0,&Wot[outbuf_idx]); /* Clear OT for using buffer */

      for(i=0,op=object;i<Objnum;i++)
	{
	  /* Calculate Local-World Matrix */
	  GsGetLw(op->coord2,&tmpls);
	  
	  /* Set LWMATRIX to GTE Lighting Registers */
	  GsSetLightMatrix(&tmpls);
	  
	  /* Calculate Local-Screen Matrix */
	  GsGetLs(op->coord2,&tmpls);

	  /* Set LSAMTRIX to GTE Registers */
	  GsSetLsMatrix(&tmpls);
	  
	  /* Perspective Translate Object and Set OT */
	  GsSortObject3(op,&Wot[outbuf_idx],14-OT_LENGTH);
	  op++;
	}

      VSync(0);			/* Wait VSYNC */
      padd=PadRead(1);		/* Readint Control Pad data */
      GsSwapDispBuff();		/* Swap double buffer */
      
      /* Set SCREEN CLESR PACKET to top of OT */
      GsSortClear(0x0,0x0,0x0,&Wot[outbuf_idx]);
      
      /* Drawing OT */
      GsDrawOt(&Wot[outbuf_idx]);
    }
}


obj_interactive()
{
  SVECTOR v1;
  MATRIX  tmp1;
  
  /* Rotate Y  */
  if((padd & PADRleft)>0) PWorld.vy -=5*ONE/360;

  /* Rotate Y */
  if((padd & PADRright)>0) PWorld.vy +=5*ONE/360;

  /* Rotate X */
  if((padd & PADRup)>0) PWorld.vx+=5*ONE/360;

  /* Rotate X */
  if((padd & PADRdown)>0) PWorld.vx-=5*ONE/360;
  
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
/*  if((padd & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}*/
  if((padd & PADk)>0) {
	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
  }
  
  /* Calculate Matrix from Object Parameter and Set Coordinate */
  set_coordinate(&PWorld,&DWorld);
  
  /* Clear flag of Coordinate for recalculation */
  DWorld.flg = 0;
  return 1;
}

/* Initialize routine */
init_all()
{
  ResetGraph(0);		/* Reset GPU */
  PadInit(0);			/* Reset Controller */
  padd=0;			/* init controller value*/
  
  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  /* rezolution set , interrace mode */
    
  GsDefDispBuff(0,0,0,0);	/* Double buffer setting */
#if 0    
  GsInitGraph(320,240,GsNONINTER|GsOFSGPU,0,0);
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
  
  initTexture1();	        /* texture load of TEX_ADDR */
}

view_init()			/* Setting view point */
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/* Set projection */
  
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
  
  /* Setting Ambient */
  GsSetAmbient(0,0,0);

  /* Setting default light mode */
  GsSetLightMode(0);
}

coord_init()			/* Setting coordinate */
{
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


initTexture1()
{
        RECT rect1;
        GsIMAGE tim1;
        u_long *tex_addr;
        int i;

        tex_addr = (u_long *)TEX_ADDR;          /* Top of TIM data address */
        while(1) {
                if(*tex_addr != TIM_HEADER) {
                        break;
                }
                tex_addr++;     /* Skip TIM file header (1word) */
                GsGetTimInfo(tex_addr, &tim1);
                tex_addr += tim1.pw*tim1.ph/2+3+1;    /* Next Texture address*/
                rect1.x=tim1.px;
                rect1.y=tim1.py;
                rect1.w=tim1.pw;
                rect1.h=tim1.ph;
                printf("XY:(%d,%d) WH:(%d,%d)\n",tim1.px,tim1.py,
                                                tim1.pw,tim1.ph);
                LoadImage(&rect1,tim1.pixel);
                DrawSync(0);
                if((tim1.pmode>>3)&0x01) {      /* if clut exist */
                        rect1.x=tim1.cx;
                        rect1.y=tim1.cy;
                        rect1.w=tim1.cw;
                        rect1.h=tim1.ch;
                        LoadImage(&rect1,tim1.clut);
                        DrawSync(0);
                        tex_addr += tim1.cw*tim1.ch/2+3;
                printf("CXY:(%d,%d) CWH:(%d,%d)\n",tim1.cx,tim1.cy,
                                                tim1.cw,tim1.ch);
                }
        }
}


/* Read modeling data (TMD FORMAT) */
model_init()
{
  u_long *dop;
  GsDOBJ3 *objp;		/* handler of object */
  int i;
  
  dop=(u_long *)MODEL_ADDR;	/* Top Address of MODELING DATA(TMD FORMAT) */

  /* Link ObjectHandler and TMD FORMAT MODELING DATA */
  Objnum = GsLinkObject3((unsigned long)dop,object);

  for(i=0,objp=object;i<Objnum;i++)
    {	
      /* Set Coordinate of Object Handler */
      objp->coord2 =  &DWorld;
      
      objp->attribute = 0;	/* init attribute */
      objp++;
    }
}

/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	spaceshuttle: sample program
 *
 *	"tuto0.c" main routine
 *
 *		Version 1.00	Mar,  31, 1994
 *
 *		Copyright (C) 1994  Sony Computer Entertainment
 *		All rights Reserved
 */

#include <sys/types.h>

#include <libetc.h>		/* for Control Pad */
#include <libgte.h>		/* LIBGS uses libgte */
#include <libgpu.h>		/* LIBGS uses libgpu */
#include <libgs.h>		/* for LIBGS */

#define OBJECTMAX 100		/* Max Objects */

#define TEX_ADDR   0x80010000	/* Top Address of texture data1 (TIM FORMAT) */

#define MODEL_ADDR 0x80040000	/* Top Address of modeling data (TMD FORMAT) */


#define OT_LENGTH  10		/* Area of OT */


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Area of OT */

GsDOBJ5		object[OBJECTMAX]; /* Array of Object Handler */

unsigned long   Objnum;		/* valibable of number of Objects */


GsCOORDINATE2   DSpaceShattle,DSpaceHatchL,DSpaceHatchR,DSatt;
/* Coordinate for GsDOBJ2 */

SVECTOR         PWorld,PSpaceShattle,PSpaceHatchL,PSpaceHatchR,PSatt;
/* work short vector for making Coordinate parmeter */

GsRVIEW2  view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler */
unsigned long padd;		/* Controler data */

u_long          preset_p[0x10000];

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ5 *op;			/* pointer of Object Handler */
  int     outbuf_idx;
  MATRIX  tmpls;
  
  ResetCallback();
  init_all();

  GsInitVcount();
  while(1)
    {
      if(obj_interactive()==0)
        return 0;	/* interactive parameter get */
      GsSetRefView2(&view);	/* Calculate World-Screen Matrix */
      outbuf_idx=GsGetActiveBuff();/* Get double buffer index */
      
      GsClearOt(0,0,&Wot[outbuf_idx]); /* Clear OT for using buffer */
      
      for(i=0,op=object;i<Objnum;i++)
	{
	  /* Calculate Local-World Matrix */
	  GsGetLs(op->coord2,&tmpls);
	  /* Set LWMATRIX to GTE Lighting Registers */
	  GsSetLightMatrix(&tmpls);
	  /* Set LSAMTRIX to GTE Registers */
	  GsSetLsMatrix(&tmpls);
	  /* Perspective Translate Object and Set OT */
	  GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,getScratchAddr(0));
  	  op++;
	}
      padd=PadRead(0);		/* Readint Control Pad data */
      VSync(0);			/* Wait for VSYNC */
      ResetGraph(1);		/* Reset GPU */
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
  
  /* rotate Y axis */
  if((padd & PADRleft)>0) PSpaceShattle.vy +=5*ONE/360;
  /* rotate Y axis */
  if((padd & PADRright)>0) PSpaceShattle.vy -=5*ONE/360;
  /* rotate X axis */
  if((padd & PADRup)>0) PSpaceShattle.vx+=5*ONE/360;
  /* rotate X axis */
  if((padd & PADRdown)>0) PSpaceShattle.vx-=5*ONE/360;
  
  /* transfer Z axis */
  if((padd & PADh)>0)      DSpaceShattle.coord.t[2]+=100;
  
  /* transfer Z axis */
  /* if((padd & PADk)>0)      DSpaceShattle.coord.t[2]-=100; */
  
  /* exit program */
/*  if((padd & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}*/
  if((padd & PADk)>0) {
	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
  }
  
  
  /* transfer X axis */
  if((padd & PADLleft)>0) DSpaceShattle.coord.t[0] -=10;
  /* transfer X axis */
  if((padd & PADLright)>0) DSpaceShattle.coord.t[0] +=10;

  /* transfer Y axis */
  if((padd & PADLdown)>0) DSpaceShattle.coord.t[1]+=10;
  /* transfer Y axis */
  if((padd & PADLup)>0) DSpaceShattle.coord.t[1]-=10;
  
  if((padd & PADl)>0)
    {				/* open the hatch */
      object[3].attribute &= 0x7fffffff;	/* exist the satellite */
      /* set the rotate parameter of the right hatch along z axis */
      PSpaceHatchR.vz -= 2*ONE/360;
      /* set the rotate parameter of the left hatch along z axis */      
      PSpaceHatchL.vz += 2*ONE/360;
      /* caliculate the matrix and set */
      set_coordinate(&PSpaceHatchR,&DSpaceHatchR);
      /* caliculate the matrix and set */
      set_coordinate(&PSpaceHatchL,&DSpaceHatchL);
    }
  if((padd & PADm)>0)
    {				/* close the hatch */
      PSpaceHatchR.vz += 2*ONE/360;
      PSpaceHatchL.vz -= 2*ONE/360;
      set_coordinate(&PSpaceHatchR,&DSpaceHatchR);
      set_coordinate(&PSpaceHatchL,&DSpaceHatchL);
    }

  if((padd & PADn)>0 && (object[3].attribute & 0x80000000) == 0)
    {				/* transfer the satellite */
      DSatt.coord.t[1] -= 10;	/* translation parameter set */
      /* rotation parameter set */
      PSatt.vy += 2*ONE/360;
      /* set the matrix to the coordinate from parameters */
      set_coordinate(&PSatt,&DSatt);
    }
  if((padd & PADo)>0 && (object[3].attribute & 0x80000000) == 0)
    {				/* transfer the satellite */
      DSatt.coord.t[1] += 10;
      PSatt.vy -= 2*ONE/360;
      set_coordinate(&PSatt,&DSatt);
    }
  /* set the matrix to the coordinate from the parameters of the shuttle */
  set_coordinate(&PSpaceShattle,&DSpaceShattle);
  return 1;
}


init_all()			/* Initialize routine */
{
  ResetGraph(0);		/* reset GPU */
  PadInit(0);			/* Reset Controller */
  padd=0;			/* controller value initialize */
  GsInitGraph(640,480,2,1,0);	/* rezolution set , non interrace mode */
  GsDefDispBuff(0,0,0,0);	/* Double buffer setting */

/*  GsInitGraph(640,240,1,1,0);	/* rezolution set , non-interrace mode */
/*  GsDefDispBuff(0,0,0,240);	/* Double buffer setting */

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
  /*
  texture_init(TEX_ADDR);
  */
}

view_init()			/* Setting view point */
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
  /* view.super = &DSatt; */
  
  /* Calculate World-Screen Matrix from viewing paramter */
  GsSetRefView2(&view);
}


light_init()			/* init Flat light */
{
  /* Setting Light ID 0 */  
  /* Setting direction vector of Light0 */
  pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
  /* Setting color of Light0 */
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  /* Set Light0 from parameters */
  GsSetFlatLight(0,&pslt[0]);
  
  /* Setting Light ID 1 */
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /* Setting Light ID 2 */
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
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
  /* SpaceShuttle's coordinate connect the WORLD coordinate directly */
  GsInitCoordinate2(WORLD,&DSpaceShattle);
  /* the right hatch's coordinate connects the shuttle's */
  GsInitCoordinate2(&DSpaceShattle,&DSpaceHatchL);
  /* the left hatch's coordinate connects the shuttle's */
  GsInitCoordinate2(&DSpaceShattle,&DSpaceHatchR);
  /* the satellite's coordinate connects the shuttle's */
  GsInitCoordinate2(&DSpaceShattle,&DSatt);
  
  /* offset the hatch's orign point to the edge of the shuttle */
  DSpaceHatchL.coord.t[0] =  356;
  DSpaceHatchR.coord.t[0] = -356;
  DSpaceHatchL.coord.t[1] = 34;
  DSpaceHatchR.coord.t[1] = 34;
  
  /* offset the satellite's orign point 20 from the orign point of the shuttle*/
  DSatt.coord.t[1] = 20;
  
  /* Init work vector */
  PWorld.vx=PWorld.vy=PWorld.vz=0;
  PSatt = PSpaceHatchR = PSpaceHatchL = PSpaceShattle = PWorld;
  /* the org point of DWold is set to Z = 4000 */
  DSpaceShattle.coord.t[2] = 4000;
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
texture_init(addr)
unsigned long addr;
{
  RECT rect1;
  GsIMAGE tim1;
  
  /* Get texture information of TIM FORMAT */  
  GsGetTimInfo((unsigned long *)(addr+4),&tim1);
  rect1.x=tim1.px;		/* X point of image data on VRAM */
  rect1.y=tim1.py;		/* Y point of image data on VRAM */
  rect1.w=tim1.pw;		/* Width of image */
  rect1.h=tim1.ph;		/* Height of image */
  
  /* Load texture to VRAM */
  LoadImage(&rect1,tim1.pixel);
  
  /* Exist Color Lookup Table */  
  if((tim1.pmode>>3)&0x01)
    {
      rect1.x=tim1.cx;		/* X point of CLUT data on VRAM */
      rect1.y=tim1.cy;		/* Y point of CLUT data on VRAM */
      rect1.w=tim1.cw;		/* Width of CLUT */
      rect1.h=tim1.ch;		/* Height of CLUT */

      /* Load CULT data to VRAM */
      LoadImage(&rect1,tim1.clut);
    }
}

/* Read modeling data (TMD FORMAT) */
model_init()
{
  unsigned long *dop;
  GsDOBJ5 *objp;		/* handler of object */
  int i;
  u_long *oppp;
  
  dop=(u_long *)MODEL_ADDR;	/* Top Address of MODELING DATA(TMD FORMAT) */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* Mappipng real address */
  dop++;
  Objnum = *dop;		/* Get number of Objects */
  dop++;			/* Adjusting for GsLinkObject4 */
  /* Link ObjectHandler and TMD FORMAT MODELING DATA */
  for(i=0;i<Objnum;i++)		
    GsLinkObject5((unsigned long)dop,&object[i],i);
  
  oppp = preset_p;
  for(i=0,objp=object;i<Objnum;i++)
    {
      /* Set Coordinate of Object Handler */
      objp->coord2 =  &DSpaceShattle;
				/* default object attribute (not display)*/
      objp->attribute = GsDOFF;
      oppp = GsPresetObject(objp,oppp);
      objp++;
    }
  
  object[0].attribute &= ~GsDOFF;
  
  object[0].attribute &= ~GsDOFF;	/* display on */
  object[0].coord2    = &DSpaceShattle;	/* set the shuttle coordinate */
  object[1].attribute &= ~GsDOFF;	/* display on */
  object[1].attribute |= GsALON;	/* semi-transparent */
  object[1].coord2    = &DSpaceHatchR;  /* set the right hatch coordinate */
  object[2].attribute &= ~GsDOFF;	/* display on */
  object[2].attribute |= GsALON;	/* semi-transparent */
  object[2].coord2    = &DSpaceHatchL;  /* set the left hatch coordiante */
  
  object[3].coord2    = &DSatt;	/* set the satellite coordinate */
}

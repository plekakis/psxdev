/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	tmdview4: GsDOBJ4 object viewing rotine
 *
 *	"tuto2.c" ******** use Active Sub Divide
 *
 *		Version 1.00	Jul,  14, 1994
 *
 *		Copyright (C) 1993 by Sony Computer Entertainment
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

#define TEX_ADDR   0x80010000	/* Top Address of texture data1 (TIM FORMAT) */

#define TEX_ADDR1   0x80020000  /* Top Address of texture data1 (TIM FORMAT) */
#define TEX_ADDR2   0x80030000  /* Top Address of texture data2 (TIM FORMAT) */


#define MODEL_ADDR 0x80040000	/* Top Address of modeling data (TMD FORMAT)  */

#define OT_LENGTH  8		/* resolution of ordering table */


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Area of OT */

GsDOBJ2		object[OBJECTMAX]; /* Array of Object Handler */

u_long          Objnum;		/* valibable of number of Objects */


GsCOORDINATE2   DWorld;  /* Coordinate for GsDOBJ2 */

SVECTOR         PWorld; /* work short vector for making Coordinate parmeter */

GsRVIEW2  view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler */
u_long padd;			/* Controler data */

PACKET		out_packet[2][PACKETMAX2];  /* GPU PACKETS AREA */
static u_long   nop_packet[2][2] = {0x01000000,0,0x01000000,0};
  
/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ2 *op;			/* pointer of Object Handler */
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
      if(obj_interactive()==0)
          return 0;	/* interactive parameter get */
      GsSetRefView2(&view);	/* calculate world screen matrix*/
      outbuf_idx=GsGetActiveBuff();/* Get double buffer index */

      /* Set top address of Packet Area for output of GPU PACKETS */
      GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);

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
	  GsSetAzwh(2000,1023,511);
	  vcount = VSync(1);
	  GsSortObject4J(op,&Wot[outbuf_idx],14-OT_LENGTH,getScratchAddr(0));
	  FntPrint("Load = %d\n",VSync(1)-vcount);
	  op++;
	}
      VSync(0);			/* Wait VSYNC */
      DrawSync(0);
      ResetGraph(1);
      padd=PadRead(1);		/* Readint Control Pad data */
      GsSwapDispBuff();		/* Swap double buffer */
      
      /* Set SCREEN CLESR PACKET to top of OT */
      GsSortClear(0x0,0x0,0x40,&Wot[outbuf_idx]);

      addPrim(Wot[outbuf_idx].org+1,nop_packet[outbuf_idx]);
      /*
      printf("address = %x\n\n",Wot[outbuf_idx].org);
      while(1);
      */
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
  if((padd & PADRright)>0) PWorld.vy -=5*ONE/360;

  /* Rotate Y */
  if((padd & PADRleft)>0) PWorld.vy +=5*ONE/360;

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
  return 1;

}


/* Initialize routine */
init_all()
{
  ResetGraph(0);		/* Reset GPU */
  PadInit(0);			/* Reset Controller */
  padd=0;			/* init controller value*/

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
  model_init();			/* Reading modeling data */  
  view_init();			/* Setting view point */
  light_init();			/* Setting Flat Light */
  
  texture_init(TEX_ADDR);	/* texture load of TEX_ADDR */
  texture_init(TEX_ADDR1);	/* texture load of TEX_ADDR1 */
  texture_init(TEX_ADDR2);	/* texture load of TEX_ADDR2 */
  jt_init4();
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
  
  /* set translation to the work matrix */
  
  /* Rotate Matrix */
  RotMatrix(pos,&tmp1);
  
  /*
  v1.vz=v1.vy=v1.vx = 7*ONE;
  ScaleMatrix(&tmp1,&v1);
  */
  
  /* Set Matrix to Coordinate */
  coor->coord = tmp1;

  /* Clear flag becase of changing parameter */
  coor->flg = 0;
}

/* Load texture to VRAM */
texture_init(addr)
u_long addr;
{
  RECT rect1;
  GsIMAGE tim1;

  /* Get texture information of TIM FORMAT */  
  GsGetTimInfo((u_long *)(addr+4),&tim1);
  
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
  u_long *dop;
  GsDOBJ2 *objp;		/* handler of object */
  int i;
  
  dop=(u_long *)MODEL_ADDR;	/* Top Address of MODELING DATA(TMD FORMAT) */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* Mapping real address  */

  dop++;
  Objnum = *dop;		/* Get number of Objects */

  dop++;			/* Adjusting for GsLinkObject4 */
    
/* Link ObjectHandler and TMD FORMAT MODELING DATA */
  for(i=0;i<Objnum;i++)
    GsLinkObject4((u_long)dop,&object[i],i);
  
  for(i=0,objp=object;i<Objnum;i++)
    {
      /* Set Coordinate of Object Handler */
      objp->coord2 =  &DWorld;
      objp->attribute = GsDIV4;
      objp++;
    }
}

/*
__main()
{
  ;
}
*/

extern _GsFCALL GsFCALL4;

jt_init4()			/* Gs SortObject4J Fook Func */
{
  PACKET *GsTMDfastF3NL(),*GsTMDfastF3LFG(),*GsTMDfastF3L(),*GsTMDfastNF3();
  PACKET *GsA4divF3NL(),*GsA4divF3LFG(),*GsA4divF3L(),*GsA4divNF3();
  PACKET *GsTMDfastG3NL(),*GsTMDfastG3LFG(),*GsTMDfastG3L(),*GsTMDfastNG3();
  PACKET *GsA4divG3NL(),*GsA4divG3LFG(),*GsA4divG3L(),*GsA4divNG3();
  PACKET *GsTMDfastTF3NL(),*GsTMDfastTF3LFG(),*GsTMDfastTF3L(),*GsTMDfastTNF3();
  PACKET *GsA4divTF3NL(),*GsA4divTF3LFG(),*GsA4divTF3L(),*GsA4divTNF3();
  PACKET *GsTMDfastTG3NL(),*GsTMDfastTG3LFG(),*GsTMDfastTG3L(),*GsTMDfastTNG3();
  PACKET *GsA4divTG3NL(),*GsA4divTG3LFG(),*GsA4divTG3L(),*GsA4divTNG3();
  PACKET *GsTMDfastF4NL(),*GsTMDfastF4LFG(),*GsTMDfastF4L(),*GsTMDfastNF4();
  PACKET *GsA4divF4NL(),*GsA4divF4LFG(),*GsA4divF4L(),*GsA4divNF4();
  PACKET *GsTMDfastG4NL(),*GsTMDfastG4LFG(),*GsTMDfastG4L(),*GsTMDfastNG4();
  PACKET *GsA4divG4NL(),*GsA4divG4LFG(),*GsA4divG4L(),*GsA4divNG4();
  PACKET *GsTMDfastTF4NL(),*GsTMDfastTF4LFG(),*GsTMDfastTF4L(),*GsTMDfastTNF4();
  PACKET *GsA4divTF4NL(),*GsA4divTF4LFG(),*GsA4divTF4L(),*GsA4divTNF4();
  PACKET *GsTMDfastTG4NL(),*GsTMDfastTG4LFG(),*GsTMDfastTG4L(),*GsTMDfastTNG4();
  PACKET *GsA4divTG4NL(),*GsA4divTG4LFG(),*GsA4divTG4L(),*GsA4divTNG4();
  PACKET *GsA4divTF4L();
  
  /* flat triangle */
  GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastF3L;
  GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_FOG]    = GsTMDfastF3LFG;
  GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_LOFF]   = GsTMDfastF3NL;
  GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_NORMAL]  = GsA4divF3L;
  GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_FOG]     = GsA4divF3LFG;
  GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_LOFF]    = GsA4divF3NL;
  GsFCALL4.nf3[GsDivMODE_NDIV]                = GsTMDfastNF3;
  GsFCALL4.nf3[GsDivMODE_DIV]                 = GsA4divNF3;
  /* gour triangle */
  GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastG3L;
  GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_FOG]    = GsTMDfastG3LFG;
  GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_LOFF]   = GsTMDfastG3NL;
  GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_NORMAL]  = GsA4divG3L;
  GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_FOG]     = GsA4divG3LFG;
  GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_LOFF]    = GsA4divG3NL;
  GsFCALL4.ng3[GsDivMODE_NDIV]                = GsTMDfastNG3;
  GsFCALL4.ng3[GsDivMODE_DIV]                 = GsA4divNG3;
  /* texture flat triangle */
  GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTF3L;
  GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_FOG]    = GsTMDfastTF3LFG;
  GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_LOFF]   = GsTMDfastTF3NL;
  GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_NORMAL]  = GsA4divTF3L;
  GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_FOG]     = GsA4divTF3LFG;
  GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_LOFF]    = GsA4divTF3NL;
  GsFCALL4.ntf3[GsDivMODE_NDIV]                = GsTMDfastTNF3;
  GsFCALL4.ntf3[GsDivMODE_DIV]                 = GsA4divTNF3;
  /* texture gour triangle */
  GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTG3L;
  GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_FOG]    = GsTMDfastTG3LFG;
  GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_LOFF]   = GsTMDfastTG3NL;
  GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_NORMAL]  = GsA4divTG3L;
  GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_FOG]     = GsA4divTG3LFG;
  GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_LOFF]    = GsA4divTG3NL;
  GsFCALL4.ntg3[GsDivMODE_NDIV]                = GsTMDfastTNG3;
  GsFCALL4.ntg3[GsDivMODE_DIV]                 = GsA4divTNG3;
  /* flat quad */
  GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastF4L;
  GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_FOG]    = GsTMDfastF4LFG;
  GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_LOFF]   = GsTMDfastF4NL;
  GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_NORMAL]  = GsA4divF4L;
  GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_FOG]     = GsA4divF4LFG;
  GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_LOFF]    = GsA4divF4NL;
  GsFCALL4.nf4[GsDivMODE_NDIV]                = GsTMDfastNF4;
  GsFCALL4.nf4[GsDivMODE_DIV]                 = GsA4divNF4;
  /* gour quad */
  GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastG4L;
  GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_FOG]    = GsTMDfastG4LFG;
  GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_LOFF]   = GsTMDfastG4NL;
  GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_NORMAL]  = GsA4divG4L;
  GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_FOG]     = GsA4divG4LFG;
  GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_LOFF]    = GsA4divG4NL;
  GsFCALL4.ng4[GsDivMODE_NDIV]                = GsTMDfastNG4;
  GsFCALL4.ng4[GsDivMODE_DIV]                 = GsA4divNG4;
  /* texture flat quad */
  GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTF4L;
  GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_FOG]    = GsTMDfastTF4LFG;
  GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_LOFF]   = GsTMDfastTF4NL;
  GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_NORMAL]  = GsA4divTF4L;
  GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_FOG]     = GsA4divTF4LFG;
  GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_LOFF]    = GsA4divTF4NL;
  GsFCALL4.ntf4[GsDivMODE_NDIV]                = GsTMDfastTNF4;
  GsFCALL4.ntf4[GsDivMODE_DIV]                 = GsA4divTNF4;
  /* texture gour quad */
  GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTG4L;
  GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_FOG]    = GsTMDfastTG4LFG;
  GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_LOFF]   = GsTMDfastTG4NL;
  GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_NORMAL]  = GsA4divTG4L;
  GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_FOG]     = GsA4divTG4LFG;
  GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_LOFF]    = GsA4divTG4NL;
  GsFCALL4.ntg4[GsDivMODE_NDIV]                = GsTMDfastTNG4;
  GsFCALL4.ntg4[GsDivMODE_DIV]                 = GsA4divTNG4;
}

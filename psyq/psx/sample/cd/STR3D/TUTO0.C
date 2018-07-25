/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *   anim_tex: streaming & graphics combination
 *
 *   Version 1.00   Feb,  23, 1995
 *
 *        Copyright  (C)  1995 by Sony Computer Entertainment
 *        All rights Reserved
 *
 *      If TEXTURE16 is defined, the animation is pasted as a texture.
 *      If BG is defined, the animation is pasted as a BG
 *
 *
 *      The following functions are added for standard graphics programs:
 * 
 *      init_anim ()   initialization routine for streaming section
 *      start_anim ()  begins streaming
 *      poll_anim ()   activates streaming routine with polling (should be
*      called at least once per frame)
 *      load_image_for_mdec_data ()  transfers 1 frame's worth of data to VRAM at once */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libcd.h>

/* #define TEXTURE16 */
#define BG

#define OBJECTMAX 100		/* Max Objects
                                   defines the maximum number of logical objects
a 3D model can be divided into */


#define TEX_ADDR   0x80010000	/* Top Address of texture data1 (TIM FORMAT) */

#define TEX_ADDR1   0x80020000  /* Top Address of texture data1 (TIM FORMAT) */
#define TEX_ADDR2   0x80030000  /* Top Address of texture data2 (TIM FORMAT) */

   
#define MODEL_ADDR 0x80040000	/* Top Address of modeling data (TMD FORMAT)  */

#define OT_LENGTH  12		/* bit length of OT */


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Area of OT */

GsDOBJ5		object[OBJECTMAX]; /* Array of Object Handler */

u_long          Objnum;		/* valibable of number of Objects */


GsCOORDINATE2   DWorld;  /* Coordinate for GsDOBJ5 */

SVECTOR         PWorld; /* work short vector for making Coordinate parmeter */

extern MATRIX GsIDMATRIX;	/* Unit Matrix */

GsRVIEW2  view;			/* View Point Handler */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler */
u_long padd;			/* Controler data */

/*  work area of PACKET DATA must be double size for packet double buffer */
u_long PacketArea[0x1000];

/*
 *	playback streaming data */
typedef struct {
    char *fileName;	 /*  filename */
    u_long endFrame; /* last frame no. */
} FileData;

static FileData fileData[] = {
	{"\\XDATA\\STR\\MOV.STR;1", 800},
	{NULL, 0 }
};

/************* MAIN START ******************************************/
int main( void )
{
  CdlFILE file;
  int     i;
  GsDOBJ5 *op;			/* pointer of Object Handler */
  int     outbuf_idx;		/* double buffer index */
  MATRIX  tmpls;
  
  ResetCallback();
  CdInit();
  
  init_all();
  init_anim();
  
  /* search file */
  if (CdSearchFile(&file,fileData[0].fileName) == 0) {
    printf("file not found\n");
    StopCallback();
    PadStop();
    exit();
    }
  
  start_anim(&file,fileData[0].endFrame);
  
  while(1)
    {
      if(obj_interactive()==0)
         return 0;	/* interactive parameter get */
      GsSetRefView2(&view);	/* Calculate World-Screen Matrix */
      outbuf_idx=GsGetActiveBuff();/* Get double buffer index */

      GsClearOt(0,0,&Wot[outbuf_idx]); /* Clear OT for using buffer */

      poll_anim(&file);		/*  transfer control to streaming routine */
      
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
	  GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,getScratchAddr(0));
	  op++;
	}
      padd=PadRead(1);		/* Readint Control Pad data */
      poll_anim(&file);		/*  transfer control to streaming routine */
      VSync(0);			/* Wait VSYNC */
      GsSwapDispBuff();		/* Swap double buffer */

#ifdef BG
      /* no need to clear if animation data updates entire BG */
      GsClearDispArea(0,0,0);
      /* transfer animation data to display area */      
      load_image_for_mdec_data(0,outbuf_idx?0:240);
#endif
      
#ifdef TEXTURE16		/* transfer animation data to texture area */
      load_image_for_mdec_data(640,64);
      GsSortClear(0,0,0,&Wot[outbuf_idx]); /* display area must be cleared */
#endif

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
  if((padd & PADk)>0)
    {
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
  padd=0;			/* init controller value */
  
  GsInitGraph(256,240,GsNONINTER|GsOFSGPU,1,0);
  
                                /* set resolution (interlace mode) */
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
  
  GsSetNearClip(100);           /* Set Near Clip */
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
  SVECTOR v1;
  
  tmp1   = GsIDMATRIX;		/* start from unit matrix */
    
  /* Set translation */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  
  /* set rotation in work matrix to work vector */
  v1 = *pos;
  
  /* Rotate Matrix */
  RotMatrix(&v1,&tmp1);
  
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
  GsDOBJ5 *objp;		/* handler of object */
  int i;
  u_long *oppp;			/* packet area pointer */
  
  dop=(u_long *)MODEL_ADDR;	/* Top Address of MODELING DATA(TMD FORMAT) */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* Mapping real address  */

  dop++;
  Objnum = *dop;		/* Get number of Objects */

  dop++;			/* Adjusting for GsLinkObject2 */

  /* Link ObjectHandler and TMD FORMAT MODELING DATA */
  for(i=0;i<Objnum;i++)
    GsLinkObject5((u_long)dop,&object[i],i);

  /* Top address of packet area */
  oppp = PacketArea;
  
  for(i=0,objp=object;i<Objnum;i++)
    {	
      /* Set Coordinate of Object Handler */
      objp->coord2 =  &DWorld;
      
      objp->attribute = 0;	/* init attribute */
      
      /* for using GsDOBJ5,you must preset PACKETS */      
      oppp = GsPresetObject(objp,oppp);	
      objp++;
    }

}

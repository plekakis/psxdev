/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	Cube : 	Rotation Matrix Interpolation sample
 *
 *	"main.c" main routine
 *
 *		Version 1.00	April,  27, 1994
 *		Version 1.01	March,  07, 1997  (added autopad)
 *
 *		Copyright (C) 1994  Sony Computer Entertainment
 *		All rights Reserved
 **/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

#define PACKETMAX 2400	
#define OBJECTMAX 100	
#define PACKETMAX2 (PACKETMAX*24) 
#define TEX_ADDR   0x80010000
#define MODEL_ADDR 0x80040000
#define OT_LENGTH  8

#define PNUM	6
#define R	200	
#define NINTP	100

GsOT            Wot[2];
GsOT_TAG	zsorttable[2][1<<OT_LENGTH];
SVECTOR         PWorld;

GsDOBJ2		object;
GsCOORDINATE2   DCube;
SVECTOR         PCube;

extern MATRIX GsIDMATRIX;	

GsRVIEW2  view;		
GsF_LIGHT pslt[3];
u_long padd;	

PACKET		out_packet[2][PACKETMAX2];

typedef struct {
	POLY_F4		surf[PNUM]; 
} PBUF;


SVECTOR vert[PNUM][4]=
	       {{{-R,-R,-R,0}, { R,-R,-R,0}, {-R, R,-R,0}, { R, R,-R,0}},
		{{ R,-R,-R,0}, { R,-R, R,0}, { R, R,-R,0}, { R, R, R,0}},
		{{ R,-R, R,0}, {-R,-R, R,0}, { R, R, R,0}, {-R, R, R,0}},
		{{-R,-R, R,0}, {-R,-R,-R,0}, {-R, R, R,0}, {-R, R,-R,0}},
		{{-R,-R, R,0}, { R,-R, R,0}, {-R,-R,-R,0}, { R,-R,-R,0}},
		{{-R, R,-R,0}, { R, R,-R,0}, {-R, R, R,0}, { R, R, R,0}}
};

static init_prim();

/************* MAIN START ******************************************/
main()
{
  int     i,j;
  int     outbuf_idx;
  MATRIX  tmpls;
  PBUF	  pb[2];		
  long	  p,otz,flag,nclip;
  int	  ret=0;

  init_all();
  GsInitVcount();

  init_prim(&pb[0]);
  init_prim(&pb[1]);

  /*Color genration for GPU packet*/
  for(i=0;i<PNUM;i++){
  	pb[0].surf[i].r0=(i*100)%155;
  	pb[0].surf[i].g0=((i+1)*100)%155;
  	pb[0].surf[i].b0=((i+2)*100)%155;
  }

  for(i=0;i<PNUM;i++){
  	pb[1].surf[i].r0=(i*100)%155;
  	pb[1].surf[i].g0=((i+1)*100)%155;
  	pb[1].surf[i].b0=((i+2)*100)%155;
  }

FntLoad(960, 256);
SetDumpFnt(FntOpen(64, 64, 256, 200, 0, 512));

  while(ret==0)
    {
      ret=obj_interactive();
      GsSetRefView2(&view);
      outbuf_idx=GsGetActiveBuff();
      GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);
      GsClearOt(0,0,&Wot[outbuf_idx]); 
      
      GsGetLs(object.coord2,&tmpls);
      GsSetLightMatrix(&tmpls);
      GsSetLsMatrix(&tmpls);

      for(i=0;i<PNUM;i++){
	nclip=RotAverageNclip4(
			&vert[i][0],&vert[i][1],&vert[i][2],&vert[i][3],
			(long *)&pb[outbuf_idx].surf[i].x0,
			(long *)&pb[outbuf_idx].surf[i].x1,
			(long *)&pb[outbuf_idx].surf[i].x2,
			(long *)&pb[outbuf_idx].surf[i].x3,
			&p,&otz,&flag);

	if(nclip>0&&flag>0){
		otz >>= (14-OT_LENGTH);
		AddPrim(&zsorttable[outbuf_idx][0]+otz,&pb[outbuf_idx].surf[i]);
	}
      }
      padd=PadRead(0);	
      VSync(0);	

      ResetGraph(1);		
      GsSwapDispBuff();	
      GsSortClear(0x0,0x0,0x0,&Wot[outbuf_idx]);
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
  static flag=0;
  static ite;
  static MATRIX eig;
  static MATRIX ieig;
  static MATRIX rot;
  MATRIX r1;
  static short theta;
  long tan;
  short delta;


  if(flag==0){
  	/* rotate Y*/
  	if((padd & PADRleft)>0) PCube.vy -=5*ONE/360;
  	/* rotate Y*/
  	if((padd & PADRright)>0) PCube.vy +=5*ONE/360;
  	/* rotate X*/
  	if((padd & PADRup)>0) PCube.vx+=5*ONE/360;
  	/* rotate X*/
  	if((padd & PADRdown)>0) PCube.vx-=5*ONE/360;

  	/* move X*/
  	if((padd & PADLleft)>0) DCube.coord.t[0]-=10;
  	/* move X*/
  	if((padd & PADLright)>0) DCube.coord.t[0]+=10;
  	/* move Y*/
  	if((padd & PADLdown)>0) DCube.coord.t[1]+=10;
  	/* move Y*/
  	if((padd & PADLup)>0) DCube.coord.t[1]-=10;

  	/* return*/
  	if((padd & PADk)>0) return(1);

  	if((padd & PADl)>0) {ite=0; flag=1;}
  	set_coordinate(&PCube,&DCube);
  }else{
	if(ite==0){
		if(IsIdMatrix(&DCube.coord)==1){
			printf("Id Matrix!\n");
			flag=0;
		}else{
			EigenMatrix(&DCube.coord,&eig);
			TransposeMatrix(&eig,&ieig);
			MulMatrix0(&ieig,&DCube.coord,&r1);
			MulMatrix0(&r1,&eig,&rot);
			tan = rot.m[1][2]*4096/rot.m[1][1];
			theta = catan(tan);
			if(rot.m[1][1]<0){
				if(rot.m[1][2]>=0){
					theta +=2048;
				}else{
					theta -=2048;
				}
			}
		}
	}
	delta = (theta*ite)/NINTP;		/*angle interpolation*/
	rot.m[1][1]=rcos(delta);
	rot.m[1][2]=rsin(delta);
	rot.m[2][1]= -rot.m[1][2];
	rot.m[2][2]=rot.m[1][1];
	MulMatrix0(&eig,&rot,&r1);
	MulMatrix0(&r1,&ieig,&DCube.coord);
	ite++;
	if(ite==NINTP)flag=0;
	DCube.flg=0;
  }
  return(0);
}


init_all()			/* initialize*/
{
  ResetCallback();
  ResetGraph(0);		/* GPU reset*/
  PadInit(0);			/* initialize pad*/
  padd=0;			/* init pad value*/
  
  GsInitGraph(640,480,2,0,0);	/* interlace*/
  GsDefDispBuff(0,0,0,0);	/* double buffer*/

/*  GsInitGraph(640,240,0,0,0);	/* non-interlace*/
/*  GsDefDispBuff(0,0,0,240);	/* double buffer*/

  GsInit3D();			/* init 3D system*/
  
  Wot[0].length=OT_LENGTH;	/* set OT length*/
  Wot[0].org=zsorttable[0];	/* set OT*/
  				/* double buffer*/
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  
  coord_init();			/* init coordinate*/
  model_init();
  view_init();			/* set viewpoint*/
  light_init();			/* set light source */
  
}

model_init()
{
	object.coord2 = &DCube;
}

view_init()			/* set view point*/
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/* set projection*/
  
  /* viewpoint*/
  view.vpx = 0; view.vpy = 0; view.vpz = -2000;
  /* target*/
  view.vrx = 0; view.vry = 0; view.vrz = 0;
  /* twist*/
  view.rz=0;
  view.super = WORLD;		/* set view param*/
  /* view.super = &DSatt;	/* set viewpoint parameter*/
  
  /* calculate World/Screen Matrix from viewing param. */
  GsSetRefView2(&view);
  GsSetNearClip(100);           /* set near clip*/
}


light_init()			/* lighting*/
{
  /* Light 0*/
  /* lighting direction*/
  pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
  /* lighting color*/
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  /* set light param.*/
  GsSetFlatLight(0,&pslt[0]);

  /* Light 1*/
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /* Light 2*/
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
  pslt[2].r=0x60; pslt[2].g=0x60; pslt[2].b=0x60;
  GsSetFlatLight(2,&pslt[2]);
  
  /* Ambient*/
  GsSetAmbient(0,0,0);

  /* set lighting mode*/
  GsSetLightMode(0);
}

coord_init()			/* coordinates*/
{
  /* DCube directly connected to World*/
  GsInitCoordinate2(WORLD,&DCube);
  PWorld.vx=PWorld.vy=PWorld.vz=0;

  PCube = PWorld;

  /* Dcube origin to World(0,0,0)*/
  DCube.coord.t[0] = 0;
  DCube.coord.t[1] = 0;
  DCube.coord.t[2] = 0;
}

set_coordinate(pos,coor)        /* generate matrix from matrix work and set */
SVECTOR *pos;                   /* matrix work*/
GsCOORDINATE2 *coor;            /* coordinate*/
{
  MATRIX tmp1;
  SVECTOR v1;

  tmp1   = GsIDMATRIX;          /* unit matrix*/
  				/* set transfer*/
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
		/*set rotation to work vector */
  v1 = *pos;
		/* calculate matrix from rotation vector */
  RotMatrix(&v1,&tmp1);
		/* set matrix to coordinate */
  coor->coord = tmp1;
		/* flush cache */
  coor->flg = 0;
}

static init_prim(pb)
PBUF      *pb;
{
        long    i;

	for(i=0;i<PNUM;i++) {
		SetPolyF4(&pb->surf[i]);
	}
}

prmatrix(m)
MATRIX *m;
{
        printf("mat=[%8d,%8d,%8d]\n",m->m[0][0],m->m[0][1],m->m[0][2]);
        printf("    [%8d,%8d,%8d]\n",m->m[1][0],m->m[1][1],m->m[1][2]);
        printf("    [%8d,%8d,%8d]\n",m->m[2][0],m->m[2][1],m->m[2][2]);
        printf("    [%8d,%8d,%8d]\n\n",m->t[0],m->t[1],m->t[2]);
}


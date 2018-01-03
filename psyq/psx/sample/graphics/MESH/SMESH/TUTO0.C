/* $PSLibId: Run-time Library Release 4.4$ */
/*			main: 
 *
 *		RotMeshS... test program
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 *
 *
 *		   PSX screen coordinate system 
 *
 *                           Z+
 *                          /
 *                         /
 *                        +------X+
 *                       /|
 *                      / |
 *                     /  Y+
 *                   eye		
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

#define HOP

#define G3_X		(0)
#define G3_Y		(-200)

#define F3_X		(0)
#define F3_Y		(-200)

#define FT3_X		(0)
#define FT3_Y		(-200)

#define FC3_X		(0)
#define FC3_Y		(-200)

#define GC3_X		(0)
#define GC3_Y		(-200)

#define GT3_X		(0)
#define GT3_Y		(-200)

#define FCT3_X		(0)
#define FCT3_Y		(-200)

#define GCT3_X		(0)
#define GCT3_Y		(-200)

#define T3_X		(0)
#define T3_Y		(-200)

#define TIM_ADDR 	0x80070000
#define TIM_HEADER 	0x00000010

#define PIH		640
#define PIV		480
#define BGR		20			/* BG color R */
#define BGG		20			/* BG color G */
#define BGB		20			/* BG color B */
#define SCR_Z		512			/* distant to screen */
#define RBK		0			/* Back color R */
#define GBK		0			/* Back color G */
#define BBK		0			/* Back color B */
#define RFC		BGR			/* Far color R */
#define GFC		BGG			/* Far color G */
#define BFC		BGB			/* Far color B */
#define OFX		(PIH>>1)		/* screen offset X */
#define OFY		(PIV>>1)		/* screen offset Y */
#define FOGNEAR	 	1000			/* Fog near point */
#define OTLEN		10
#define OTSIZE		(1<<OTLEN)

#define RECTH           25
#define RECTV           20
#define RECTD           12

#define RECTH1           24
#define RECTV1           17
#define RECTD1          4

#define EDGE            20


#define MESH_LEN	1252			/* number of vertex */
#define PRIM_LEN	(MESH_LEN-2)		/* number of primitive */
#define DPQ		0			/* 0:dpq off 1:on */
#define BACKC		0			/* 0:nclip off 1:on */

#define NMODE		9

#define MODE_FC3	0
#define MODE_F3		1
#define MODE_G3		2
#define MODE_FT3	3
#define MODE_GC3	4
#define MODE_GT3	5
#define MODE_T3		6
#define MODE_FCT3	7
#define MODE_GCT3	8

#define MLEN_FC3	1252
#define MLEN_F3		952
#define MLEN_G3		602
#define MLEN_FT3	802
#define MLEN_GC3	602
#define MLEN_GT3	402
#define MLEN_T3		802
#define MLEN_FCT3	802
#define MLEN_GCT3	402

#define PLEN_FC3	(MLEN_FC3-2)
#define PLEN_F3		(MLEN_F3-2)
#define PLEN_G3		(MLEN_G3-2)
#define PLEN_FT3	(MLEN_FT3-2)
#define PLEN_GC3	(MLEN_GC3-2)
#define PLEN_GT3	(MLEN_GT3-2)
#define PLEN_T3		(MLEN_T3-2)
#define PLEN_FCT3	(MLEN_FCT3-2)
#define PLEN_GCT3	(MLEN_GCT3-2)

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_G3		surf_G3[4][PLEN_G3];
	POLY_F3		surf_F3[4][PLEN_F3];
	POLY_FT3	surf_FT3[4][PLEN_FT3];	
	POLY_F3		surf_FC3[4][PLEN_FC3];	
	POLY_G3		surf_GC3[4][PLEN_GC3];	
	POLY_GT3	surf_GT3[4][PLEN_GT3];	
	POLY_FT3	surf_T3[4][PLEN_T3];	
	POLY_FT3	surf_FCT3[4][PLEN_FCT3];	
	POLY_GT3	surf_GCT3[4][PLEN_GCT3];	
} DB;


/*-G3------------------------------------------------------------------*/
SVECTOR p_G3[MLEN_G3];		/* vertex */
CVECTOR c_G3[MLEN_G3];		/* original color */
SVECTOR	n_G3[MLEN_G3];		/* normal*/
/*--------------------------------------------------------------------*/
/*-F3------------------------------------------------------------------*/
SVECTOR p_F3[MLEN_F3];		/* vertex*/
CVECTOR c_F3[MLEN_F3];		/* original color */
SVECTOR n_F3[MLEN_F3];		/* normal*/
/*--------------------------------------------------------------------*/
/*-FT3------------------------------------------------------------------*/
SVECTOR p_FT3[MLEN_FT3];	/* vertex */
CVECTOR c_FT3[MLEN_FT3];	/* original color */
SVECTOR n_FT3[MLEN_FT3];	/* normal */
/*--------------------------------------------------------------------*/
/*-FC3------------------------------------------------------------------*/
SVECTOR p_FC3[MLEN_FC3];	/* vertex */
CVECTOR c_FC3[MLEN_FC3];	/* original color */
/*--------------------------------------------------------------------*/
/*-GC3------------------------------------------------------------------*/
SVECTOR p_GC3[MLEN_GC3];	/* vertex */
CVECTOR c_GC3[MLEN_GC3];	/* original color */
/*--------------------------------------------------------------------*/
/*-GT3------------------------------------------------------------------*/
SVECTOR p_GT3[MLEN_GT3];	/* vertex */
CVECTOR c_GT3[MLEN_GT3];	/* original color */
SVECTOR n_GT3[MLEN_GT3];	/* normal */
/*--------------------------------------------------------------------*/
/*-FCT3------------------------------------------------------------------*/
SVECTOR p_FCT3[MLEN_FCT3];	/* vertex */
CVECTOR c_FCT3[MLEN_FCT3];	/* original color */
/*--------------------------------------------------------------------*/
/*-GCT3------------------------------------------------------------------*/
SVECTOR p_GCT3[MLEN_GCT3];	/* vertex */
CVECTOR c_GCT3[MLEN_GCT3];	/* original color */
/*--------------------------------------------------------------------*/
/*-T3------------------------------------------------------------------*/
SVECTOR p_T3[MLEN_T3];		/* vertex */
CVECTOR c_T3[MLEN_T3];		/* original color */
SVECTOR n_T3[MLEN_T3];		/* normal */
/*--------------------------------------------------------------------*/


MATRIX lo ={0,0,4096,		/* light direction 1 (eye to screen)*/
	    0,4096,0,		/* light direction 2 (up to down)*/
	    -4096,0,0,		/* light direction 3 (right to left)*/
	    0,0,0};			

MATRIX lc = {4096,0,0,				/* light color 1(red)*/
	     0,4096,0,				/* light color 2(green)*/
	     0,0,4096,				/* light color 3(blue) */
	     0,0,0};				/* light color matrix*/

MATRIX lct = {4096,4096,4096,
	      4096,4096,4096,
	      4096,4096,4096,
	      0,0,0};				/* light color matrix*/

MATRIX	l;					/* LocalLightMatrix */

u_short tpage;
u_short clut;
u_long	backc=BACKC;
u_long	dpq=DPQ;
int		mlen=MLEN_FC3;
int		mode=0;
int		hopstop;

static pad_read();
static init_prim();

main()
{
	DB		db[2];		/* packet double buffer */
	DB		*cdb;		/* current db */
	int		dmy, flg1,flg2;	/* dummy */
	long		opz;
	long		otz[PRIM_LEN];
	long		nclip;

	TMESH		msh_G3;
	TMESH		msh_F3;
	TMESH		msh_FT3;
	TMESH		msh_FC3;
	TMESH		msh_GC3;
	TMESH		msh_GT3;
	TMESH		msh_FCT3;
	TMESH		msh_GCT3;
	TMESH		msh_T3;

	long		flag[PRIM_LEN];
	int		i,j;
	long		sxy0,sxy1,sxy2;
	VECTOR		v0,v1,v2;
	VECTOR		nml[PRIM_LEN];	/*primitive normal*/
	long		a,b;
	int		frame;
	VECTOR		vect;
	MATRIX		WLmat[4];
	MATRIX		SWmat;
	MATRIX		SLmat[4];
	int		hopY[64];
	int		shrink[64];
	SVECTOR		rotang;
	static int		savet;
	long	ir1,ir2,ir3;


	rotang.vx=0;
	rotang.vy=0;
	rotang.vz=0;

	for(i=0;i<32;i++){
#ifdef HOP
		hopY[i]=hopY[63-i]= -300 + i*i/3;
#else
		hopY[i]=hopY[63-i]= -300 ;
#endif
	}
	for(i=0;i<64;i++){
#ifdef HOP
		shrink[i]= 4096 + rcos(64*i)/8;
#else
		shrink[i]= 4096 ;
#endif
	}

	hopstop=0;

	ResetCallback();
	PadInit(0);
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(OFX, OFY);	/* set geometry origin as (OFX, OFY) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */
	SetBackColor(RBK,GBK,BBK);	/* set background(ambient) color*/
	SetFarColor(RFC,GFC,BFC);	/* set far color */
	SetColorMatrix(&lc);		/* set light color matrix*/
	SetFogNear(FOGNEAR,SCR_Z);	/* set fog parameter*/
	texture_init(TIM_ADDR);

	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, PIH, PIV);	
	SetDefDrawEnv(&db[1].draw, 0,   0, PIH, PIV);
	SetDefDispEnv(&db[0].disp, 0,   0, PIH, PIV);
	SetDefDispEnv(&db[1].disp, 0,   0, PIH, PIV);
	
        FntLoad(960, 256);
        SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	/* init primitives */
	init_prim(&db[0]);
	init_prim(&db[1]);
	

	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	/*genarate vertex*/
	for(i=0;i<MLEN_G3;i++){
		p_G3[i].vx= rcos(-64*i)/20 + G3_X;
		p_G3[i].vy= i+ (i%2)*40  - 200 + G3_Y;
		p_G3[i].vz= rsin(-64*i)/20 ;
	}
	for(i=0;i<MLEN_F3;i++){
		p_F3[i].vx= rcos(-64*i)/20 + F3_X;
		p_F3[i].vy= i + (i%2)*40 - 200 + F3_Y;
		p_F3[i].vz= rsin(-64*i)/20 ;
	}
	for(i=0;i<MLEN_FT3;i++){
		p_FT3[i].vx= rcos(-64*i)/20 + FT3_X;
		p_FT3[i].vy= i + (i%2)*40 - 200 + FT3_Y;
		p_FT3[i].vz= rsin(-64*i)/20 ;
	}
	for(i=0;i<MLEN_FC3;i++){
		p_FC3[i].vx= (rcos(-64*i)/20) + FC3_X;
		p_FC3[i].vy= i +(i%2)*40 - 200  +FC3_Y;
		p_FC3[i].vz= (rsin(-64*i)/20) ;
	}
	for(i=0;i<MLEN_GC3;i++){
		p_GC3[i].vx= (rcos(-64*i)/20) + GC3_X;
		p_GC3[i].vy= i +(i%2)*40 - 200  +GC3_Y;
		p_GC3[i].vz= (rsin(-64*i)/20) ;
	}
	for(i=0;i<MLEN_GT3;i++){
		p_GT3[i].vx= (rcos(-64*i)/20) + GT3_X;
		p_GT3[i].vy= i +(i%2)*40 - 200  +GT3_Y;
		p_GT3[i].vz= (rsin(-64*i)/20) ;
	}
	for(i=0;i<MLEN_FCT3;i++){
		p_FCT3[i].vx= (rcos(-64*i)/20) + FCT3_X;
		p_FCT3[i].vy= i +(i%2)*40 - 200  +FCT3_Y;
		p_FCT3[i].vz= (rsin(-64*i)/20) ;
	}
	for(i=0;i<MLEN_GCT3;i++){
		p_GCT3[i].vx= (rcos(-64*i)/20) + GCT3_X;
		p_GCT3[i].vy= i +(i%2)*40 - 200  +GCT3_Y;
		p_GCT3[i].vz= (rsin(-64*i)/20) ;
	}
	for(i=0;i<MLEN_T3;i++){
		p_T3[i].vx= (rcos(-64*i)/20) + T3_X;
		p_T3[i].vy= i +(i%2)*40 - 200  +T3_Y;
		p_T3[i].vz= (rsin(-64*i)/20) ;
	}

	/*generate color*/
	for(i=0;i<MLEN_G3;i++){
		c_G3[i].r=255;
		c_G3[i].g=255;
		c_G3[i].b=255;
		c_G3[i].cd=db[0].surf_G3[0][0].code;	
	}
	for(i=0;i<MLEN_F3;i++){
		c_F3[i].r=255;
		c_F3[i].g=255;
		c_F3[i].b=255;
		c_F3[i].cd=db[0].surf_F3[0][0].code;
	}
	for(i=0;i<MLEN_FT3;i++){
		c_FT3[i].r=255;
		c_FT3[i].g=255;
		c_FT3[i].b=255;
		c_FT3[i].cd=db[0].surf_FT3[0][0].code;
	}
	for(i=0;i<MLEN_FC3;i++){
		c_FC3[i].r= rand(i)%255;
		c_FC3[i].g= rand(i)%255;
		c_FC3[i].b= rand(i)%255;
		c_FC3[i].cd=db[0].surf_FC3[0][0].code;	/* GPU code copy */
	}
	for(i=0;i<MLEN_GC3;i++){
		c_GC3[i].r= rand(i)%255;
		c_GC3[i].g= rand(i)%255;
		c_GC3[i].b= rand(i)%255;
		c_GC3[i].cd=db[0].surf_GC3[0][0].code;	/* GPU code copy */
	}
	for(i=0;i<MLEN_GT3;i++){
		c_GT3[i].r=255;
		c_GT3[i].g=255;
		c_GT3[i].b=255;
		c_GT3[i].cd=db[0].surf_GT3[0][0].code;
	}
	for(i=0;i<MLEN_FCT3;i++){
		c_FCT3[i].r= i%255;
		c_FCT3[i].g= i%255;
		c_FCT3[i].b= i%255;
		c_FCT3[i].cd=db[0].surf_FCT3[0][0].code; /* GPU code copy */
	}
	for(i=0;i<MLEN_GCT3;i++){
		c_GCT3[i].r= i%255;
		c_GCT3[i].g= i%255;
		c_GCT3[i].b= i%255;
		c_GCT3[i].cd=db[0].surf_GCT3[0][0].code; /* GPU code copy */
	}
	for(i=0;i<MLEN_T3;i++){
		c_T3[i].r=255;
		c_T3[i].g=255;
		c_T3[i].b=255;
		c_T3[i].cd=db[0].surf_T3[0][0].code;
	}

	/*generate normals (use G3 normal for all)*/
	for(i=0;i<PLEN_FC3;i++){
		if(i%2==0){
			v0.vx= p_FC3[i+1].vx - p_FC3[i].vx;
			v0.vy= p_FC3[i+1].vy - p_FC3[i].vy;
			v0.vz= p_FC3[i+1].vz - p_FC3[i].vz;
			v1.vx= p_FC3[i+2].vx - p_FC3[i].vx;
			v1.vy= p_FC3[i+2].vy - p_FC3[i].vy;
			v1.vz= p_FC3[i+2].vz - p_FC3[i].vz;
		}else{
			v0.vx= p_FC3[i].vx - p_FC3[i+1].vx;
			v0.vy= p_FC3[i].vy - p_FC3[i+1].vy;
			v0.vz= p_FC3[i].vz - p_FC3[i+1].vz;
			v1.vx= p_FC3[i+2].vx - p_FC3[i+1].vx;
			v1.vy= p_FC3[i+2].vy - p_FC3[i+1].vy;
			v1.vz= p_FC3[i+2].vz - p_FC3[i+1].vz;
		}	

		OuterProduct0(&v0,&v1,&v2);

		a=absmax3(v2.vx,v2.vy,v2.vz);
		if(a<0) a= -a;
		b= SquareRoot0(a);
		v2.vx/=b;
		v2.vy/=b;
		v2.vz/=b;

                VectorNormal(&v2,&nml[i]);
	}
	/*generate G3 normals*/
	for(i=0;i<MLEN_G3-2;i++){
		n_G3[i].vx= nml[i].vx;
		n_G3[i].vy= nml[i].vy;
		n_G3[i].vz= nml[i].vz;
	}
	n_G3[MLEN_G3-2].vx= nml[PLEN_G3-1].vx;
	n_G3[MLEN_G3-2].vy= nml[PLEN_G3-1].vy;
	n_G3[MLEN_G3-2].vz= nml[PLEN_G3-1].vz;
	n_G3[MLEN_G3-1].vx= nml[PLEN_G3-1].vx;
	n_G3[MLEN_G3-1].vy= nml[PLEN_G3-1].vy;
	n_G3[MLEN_G3-1].vz= nml[PLEN_G3-1].vz;

	/*generate F3 normals*/
	for(i=0;i<MLEN_F3;i++){
		n_F3[i].vx= nml[i].vx;
		n_F3[i].vy= nml[i].vy;
		n_F3[i].vz= nml[i].vz;
	}
	/*generate FT3 normals*/
	for(i=0;i<MLEN_FT3;i++){
		n_FT3[i].vx= nml[i].vx;
		n_FT3[i].vy= nml[i].vy;
		n_FT3[i].vz= nml[i].vz;
	}
	/*generate GT3 normals*/
	for(i=0;i<MLEN_GT3;i++){
		n_GT3[i].vx= nml[i].vx;
		n_GT3[i].vy= nml[i].vy;
		n_GT3[i].vz= nml[i].vz;
	}
	n_GT3[MLEN_GT3-2].vx= nml[PLEN_GT3-1].vx;
	n_GT3[MLEN_GT3-2].vy= nml[PLEN_GT3-1].vy;
	n_GT3[MLEN_GT3-2].vz= nml[PLEN_GT3-1].vz;
	n_GT3[MLEN_GT3-1].vx= nml[PLEN_GT3-1].vx;
	n_GT3[MLEN_GT3-1].vy= nml[PLEN_GT3-1].vy;
	n_GT3[MLEN_GT3-1].vz= nml[PLEN_GT3-1].vz;

	/*set MESH*/
	msh_G3.v=p_G3;
	msh_G3.n=n_G3;
	msh_G3.c=c_G3;
	msh_G3.len=MLEN_G3;

	msh_F3.v=p_F3;
	msh_F3.n=n_F3;
	msh_F3.c=c_F3;
	msh_F3.len=MLEN_F3;

	msh_FT3.v=p_FT3;
	msh_FT3.n=n_FT3;
	msh_FT3.c=c_FT3;
	msh_FT3.len=MLEN_FT3;

	msh_FC3.v=p_FC3;
	msh_FC3.c=c_FC3;
	msh_FC3.len=MLEN_FC3;

	msh_GC3.v=p_GC3;
	msh_GC3.c=c_GC3;
	msh_GC3.len=MLEN_GC3;

	msh_GT3.v=p_GT3;
	msh_GT3.n=n_GT3;
	msh_GT3.c=c_GT3;
	msh_GT3.len=MLEN_GT3;

	msh_FCT3.v=p_FCT3;
	msh_FCT3.c=c_FCT3;
	msh_FCT3.len=MLEN_FCT3;

	msh_GCT3.v=p_GCT3;
	msh_GCT3.c=c_GCT3;
	msh_GCT3.len=MLEN_GCT3;

	msh_T3.v=p_T3;
	msh_T3.n=n_T3;
	msh_T3.c=c_T3;
	msh_T3.len=MLEN_T3;

	frame=0;
	while (pad_read(&SWmat) == 0) {
		
	    cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
	    ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */
/*
	    rotang.vy += 10;
*/
	    /* rotate & Transfer & Perspect & AverageZ & NormalClip &
				 lighting & DepthQuing of 3 vertices */
	    for(j=0;j<4;j++){
		RotMatrix(&rotang,&WLmat[j]);
		if(hopstop==0){
	    	    vect.vx=4096;
	    	    vect.vy=shrink[(frame+10*j)%64];
		    vect.vz=4096;
		    ScaleMatrix(&WLmat[j],&vect);
		    WLmat[j].t[0]= -700+450*j;
		    WLmat[j].t[1]= hopY[(frame+10*j)%64];
		    WLmat[j].t[2]= 0;
		}
		CompMatrix(&SWmat,&WLmat[j],&SLmat[j]);

		MulMatrix0(&lo,&SLmat[j],&l);
		SetLightMatrix(&l);		
		SetColorMatrix(&lc);	

		SetRotMatrix(&SLmat[j]);
		SetTransMatrix(&SLmat[j]);

	    	switch(mode){
	    	case MODE_G3:
			msh_G3.len= mlen;
			RotMeshPrimS_G3(&msh_G3,cdb->surf_G3[j],cdb->ot,
					OTLEN,dpq,backc);	
			break;

	    	case MODE_F3:
			msh_F3.len= mlen;
			RotMeshPrimS_F3(&msh_F3,cdb->surf_F3[j],cdb->ot,
					OTLEN,dpq,backc);	
			break;

	    	case MODE_FT3:
			msh_FT3.len= mlen;
			RotMeshPrimS_FT3(&msh_FT3,cdb->surf_FT3[j],cdb->ot,
					OTLEN,dpq,backc);	
			break;

	    	case MODE_FC3:
			msh_FC3.len= mlen;
			RotMeshPrimS_FC3(&msh_FC3,cdb->surf_FC3[j],cdb->ot,
					OTLEN,dpq,backc);
			break;

	    	case MODE_GC3:
			msh_GC3.len= mlen;
			RotMeshPrimS_GC3(&msh_GC3,cdb->surf_GC3[j],cdb->ot,
					OTLEN,dpq,backc);
			break;

	    	case MODE_GT3:
			msh_GT3.len= mlen;
			RotMeshPrimS_GT3(&msh_GT3,cdb->surf_GT3[j],cdb->ot,
					OTLEN,dpq,backc);	
			break;

	    	case MODE_FCT3:
			msh_FCT3.len= mlen;
			RotMeshPrimS_FCT3(&msh_FCT3,cdb->surf_FCT3[j],cdb->ot,
					OTLEN,dpq,backc);	
			break;

	    	case MODE_GCT3:
			msh_GCT3.len= mlen;
			RotMeshPrimS_GCT3(&msh_GCT3,cdb->surf_GCT3[j],cdb->ot,
					OTLEN,dpq,backc);	
			break;

	    	case MODE_T3:
			msh_T3.len= mlen;
			RotMeshPrimS_T3(&msh_T3,cdb->surf_T3[j],cdb->ot,
					OTLEN,dpq,backc);
			break;
	    	}

	    }
	    frame++;

	    /* swap buffer */
	    DrawSync(0);	/* wait for end of drawing */
	    VSync(0);	/* wait for the next V-BLNK */
	
	    PutDrawEnv(&cdb->draw); /* update drawing environment */
	    PutDispEnv(&cdb->disp); /* update display environment */

	    DrawOTag(cdb->ot+OTSIZE-1);	/* draw */
	    switch(mode){
	    case MODE_F3:
                FntPrint("\n\nF3 %3dK/sec\n",mlen*4*60/1000);
                FntFlush(-1);
		break;
	    case MODE_G3:
                FntPrint("\n\nG3 %3dK/sec\n",mlen*4*60/1000);
                FntFlush(-1);
		break;
	    case MODE_FT3:
                FntPrint("\n\nFT3 %3dK/sec\n",mlen*4*60/1000);
                FntFlush(-1);
		break;
	    case MODE_FC3:
                FntPrint("\n\nFC3 %3dK/sec\n",mlen*4*60/1000);
                FntFlush(-1);
		break;
	    case MODE_GC3:
                FntPrint("\n\nGC3 %3dK/sec\n",mlen*4*60/1000);
                FntFlush(-1);
		break;
	    case MODE_GT3:
                FntPrint("\n\nGT3 %3dK/sec\n",mlen*4*60/1000);
                FntFlush(-1);
		break;
	    case MODE_FCT3:
                FntPrint("\n\nFCT3 %3dK/sec\n",mlen*4*60/1000);
                FntFlush(-1);
		break;
	    case MODE_GCT3:
                FntPrint("\n\nGCT3 %3dK/sec\n",mlen*4*60/1000);
                FntFlush(-1);
		break;
	    case MODE_T3:
                FntPrint("\n\nT3 %3dK/sec\n",mlen*4*60/1000);
                FntFlush(-1);
		break;
	    }
	}
	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
}

static pad_read(m)
MATRIX	*m;
{
	static SVECTOR	ang   = {0, 0, 0};	/* rotate angle */
	static VECTOR	vec   = {0, 0, 1800};	/* translate vector */
	static VECTOR	vec1   = {0, 0, 5000};	/* translate vector */
	static VECTOR	vec2   = {0, 0, 4000};	/* translate vector */
	static modec=0;
	static oldmode=0;
	
	int	ret = 0;	
	u_long	padd;
/*	u_long	padd = PadRead(0); */
	
	padd = PadRead(0);

	if (padd & PADRup)	ang.vx -= 32;
	if (padd & PADRdown)	ang.vx += 32;
	if (padd & PADRleft) 	ang.vy -= 32;
	if (padd & PADRright)	ang.vy += 32;
	
      if((padd & PADLup)>0) modec++;
      if((padd & PADLdown)>0) modec--;

      if((padd & PADLleft)>0) mlen+=10;
      if((padd & PADLright)>0) mlen-=10;

	mode= (modec/10)%NMODE;
	if(mode<0) mode +=NMODE;

	if(mode!=oldmode){ 
		switch(mode){
		case MODE_F3:
			mlen= MLEN_F3;
			break;
		case MODE_G3:
			mlen= MLEN_G3;
			break;
		case MODE_FT3:
			mlen= MLEN_FT3;
			break;
		case MODE_FC3:
			mlen= MLEN_FC3;
			break;
		case MODE_GC3:
			mlen= MLEN_GC3;
			break;
		case MODE_GT3:
			mlen= MLEN_GT3;
			break;
		case MODE_FCT3:
			mlen= MLEN_FCT3;
			break;
		case MODE_GCT3:
			mlen= MLEN_GCT3;
			break;
		case MODE_T3:
			mlen= MLEN_T3;
			break;
		}
	}

	oldmode= mode;

	switch(mode){
	case MODE_F3:
		if(mlen>MLEN_F3) mlen=MLEN_F3;
		if(mlen<0) mlen=0;
		break;
	case MODE_G3:
		if(mlen>MLEN_G3) mlen=MLEN_G3;
		if(mlen<0) mlen=0;
		break;
	case MODE_FT3:
		if(mlen>MLEN_FT3) mlen=MLEN_FT3;
		if(mlen<0) mlen=0;
		break;
	case MODE_FC3:
		if(mlen>MLEN_FC3) mlen=MLEN_FC3;
		if(mlen<0) mlen=0;
		break;
	case MODE_GC3:
		if(mlen>MLEN_GC3) mlen=MLEN_GC3;
		if(mlen<0) mlen=0;
		break;
	case MODE_GT3:
		if(mlen>MLEN_GT3) mlen=MLEN_GT3;
		if(mlen<0) mlen=0;
		break;
	case MODE_FCT3:
		if(mlen>MLEN_FCT3) mlen=MLEN_FCT3;
		if(mlen<0) mlen=0;
		break;
	case MODE_GCT3:
		if(mlen>MLEN_GCT3) mlen=MLEN_GCT3;
		if(mlen<0) mlen=0;
		break;
	case MODE_T3:
		if(mlen>MLEN_T3) mlen=MLEN_T3;
		if(mlen<0) mlen=0;
		break;
	}

	if (padd & PADl) vec.vz += 8;

	if (padd & PADn) 	vec.vz -= 8;

	if(padd & PADm)		dpq=1;
	if(padd & PADo)		dpq=0;

	if(padd & PADh)		backc=1;
	if(padd & PADk) 	backc=0;

	if(((padd & PADm)!=0)&&((padd & PADo)!=0)&&((padd & PADk)!=0)){
		ret= -1;
	}
	if(((padd & PADn)!=0)&&((padd & PADl)!=0)&&((padd & PADh)!=0)){
		hopstop= 1-hopstop;
	}

	RotMatrix(&ang, m);	
	TransMatrix(m, &vec);				

	return(ret);
}		

static init_prim(db)
DB	*db;
{
	int i,j,k;
	int im,in;
	POLY_F3 *sf;

	db->draw.isbg=1;
	setRGB0( &db->draw, BGR, BGG, BGB);	/* (r,g,b) = (BGR,BGG,BGB) */
	
	/* initialize surf */
	for(i=0;i<PLEN_G3;i++){
	    for(j=0;j<4;j++){
		SetPolyG3(&db->surf_G3[j][i]);	/* set POLY_G3 primitve ID */
	    }
	}
	for(i=0;i<PLEN_F3;i++){
	    for(j=0;j<4;j++){
		SetPolyF3(&db->surf_F3[j][i]);	/* set POLY_F3 primitve ID */
	    }
	}
	for(i=0;i<PLEN_FT3;i++){
	    for(j=0;j<4;j++){
		SetPolyFT3(&db->surf_FT3[j][i]);/* set POLY_FT3 primitve ID */
		im=i%64;
		in=i/64;
		if(i%2==0){
			db->surf_FT3[j][i].u0= (im*8);
			db->surf_FT3[j][i].v0= (in*8);
			db->surf_FT3[j][i].u1= (im*8);
			db->surf_FT3[j][i].v1= ((in+1)*8);
			db->surf_FT3[j][i].u2= ((im+1)*8);
			db->surf_FT3[j][i].v2= (in*8);
			db->surf_FT3[j][i].tpage= tpage;
			db->surf_FT3[j][i].clut= clut;		
		}
		if(i%2==1){
			db->surf_FT3[j][i].u0= ((im-1)*8);
			db->surf_FT3[j][i].v0= ((in+1)*8);
			db->surf_FT3[j][i].u1= ((im-1+1)*8);
			db->surf_FT3[j][i].v1= (in*8);
			db->surf_FT3[j][i].u2= ((im-1+1)*8);
			db->surf_FT3[j][i].v2= ((in+1)*8);
			db->surf_FT3[j][i].tpage= tpage;
			db->surf_FT3[j][i].clut= clut;		
		}
	    }
	}
	for(i=0;i<PLEN_FC3;i++){
	    for(j=0;j<4;j++){
		SetPolyF3(&db->surf_FC3[j][i]);	/* set POLY_F3 primitve ID */
	    }
	}
	for(i=0;i<PLEN_GC3;i++){
	    for(j=0;j<4;j++){
		SetPolyG3(&db->surf_GC3[j][i]);	/* set POLY_F3 primitve ID */
	    }
	}
	for(i=0;i<PLEN_GT3;i++){
	    for(j=0;j<4;j++){
		SetPolyGT3(&db->surf_GT3[j][i]);/* set POLY_GT3 primitve ID */
		im=i%64;
		in=i/64;
		if(i%2==0){
			db->surf_GT3[j][i].u0= (im*8);
			db->surf_GT3[j][i].v0= (in*8);
			db->surf_GT3[j][i].u1= (im*8);
			db->surf_GT3[j][i].v1= ((in+1)*8);
			db->surf_GT3[j][i].u2= ((im+1)*8);
			db->surf_GT3[j][i].v2= (in*8);
			db->surf_GT3[j][i].tpage= tpage;
			db->surf_GT3[j][i].clut= clut;		
		}
		if(i%2==1){
			db->surf_GT3[j][i].u0= ((im-1)*8);
			db->surf_GT3[j][i].v0= ((in+1)*8);
			db->surf_GT3[j][i].u1= ((im-1+1)*8);
			db->surf_GT3[j][i].v1= (in*8);
			db->surf_GT3[j][i].u2= ((im-1+1)*8);
			db->surf_GT3[j][i].v2= ((in+1)*8);
			db->surf_GT3[j][i].tpage= tpage;
			db->surf_GT3[j][i].clut= clut;		
		}
	    }
	}
	for(i=0;i<PLEN_FCT3;i++){
	    for(j=0;j<4;j++){
		SetPolyFT3(&db->surf_FCT3[j][i]);/* set POLY_FT3 primitve ID */
		im=i%64;
		in=i/64;
		if(i%2==0){
			db->surf_FCT3[j][i].u0= (im*8);
			db->surf_FCT3[j][i].v0= (in*8);
			db->surf_FCT3[j][i].u1= (im*8);
			db->surf_FCT3[j][i].v1= ((in+1)*8);
			db->surf_FCT3[j][i].u2= ((im+1)*8);
			db->surf_FCT3[j][i].v2= (in*8);
			db->surf_FCT3[j][i].tpage= tpage;
			db->surf_FCT3[j][i].clut= clut;		
		}
		if(i%2==1){
			db->surf_FCT3[j][i].u0= ((im-1)*8);
			db->surf_FCT3[j][i].v0= ((in+1)*8);
			db->surf_FCT3[j][i].u1= ((im-1+1)*8);
			db->surf_FCT3[j][i].v1= (in*8);
			db->surf_FCT3[j][i].u2= ((im-1+1)*8);
			db->surf_FCT3[j][i].v2= ((in+1)*8);
			db->surf_FCT3[j][i].tpage= tpage;
			db->surf_FCT3[j][i].clut= clut;		
		}
		db->surf_FCT3[j][i].r0= i%255;
		db->surf_FCT3[j][i].g0= i%255;
		db->surf_FCT3[j][i].b0= i%255;
	    }
	}
	for(i=0;i<PLEN_GCT3;i++){
	    for(j=0;j<4;j++){
		SetPolyGT3(&db->surf_GCT3[j][i]);/* set POLY_GT3 primitve ID */
		im=i%64;
		in=i/64;
		if(i%2==0){
			db->surf_GCT3[j][i].u0= (im*8);
			db->surf_GCT3[j][i].v0= (in*8);
			db->surf_GCT3[j][i].u1= (im*8);
			db->surf_GCT3[j][i].v1= ((in+1)*8);
			db->surf_GCT3[j][i].u2= ((im+1)*8);
			db->surf_GCT3[j][i].v2= (in*8);
			db->surf_GCT3[j][i].tpage= tpage;
			db->surf_GCT3[j][i].clut= clut;		
		}
		if(i%2==1){
			db->surf_GCT3[j][i].u0= ((im-1)*8);
			db->surf_GCT3[j][i].v0= ((in+1)*8);
			db->surf_GCT3[j][i].u1= ((im-1+1)*8);
			db->surf_GCT3[j][i].v1= (in*8);
			db->surf_GCT3[j][i].u2= ((im-1+1)*8);
			db->surf_GCT3[j][i].v2= ((in+1)*8);
			db->surf_GCT3[j][i].tpage= tpage;
			db->surf_GCT3[j][i].clut= clut;		
		}
		db->surf_GCT3[j][i].r0= i%255;
		db->surf_GCT3[j][i].g0= i%255;
		db->surf_GCT3[j][i].b0= i%255;
		db->surf_GCT3[j][i].r1= (i+1)%255;
		db->surf_GCT3[j][i].g1= (i+1)%255;
		db->surf_GCT3[j][i].b1= (i+1)%255;
		db->surf_GCT3[j][i].r2= (i+2)%255;
		db->surf_GCT3[j][i].g2= (i+2)%255;
		db->surf_GCT3[j][i].b2= (i+2)%255;
	    }
	}
	for(i=0;i<PLEN_T3;i++){
	    for(j=0;j<4;j++){
		SetPolyFT3(&db->surf_T3[j][i]);/* set POLY_FT3 primitve ID */
		im=i%64;
		in=i/64;
		if(i%2==0){
			db->surf_T3[j][i].u0= (im*8);
			db->surf_T3[j][i].v0= (in*8);
			db->surf_T3[j][i].u1= (im*8);
			db->surf_T3[j][i].v1= ((in+1)*8);
			db->surf_T3[j][i].u2= ((im+1)*8);
			db->surf_T3[j][i].v2= (in*8);
			db->surf_T3[j][i].tpage= tpage;
			db->surf_T3[j][i].clut= clut;		
		}
		if(i%2==1){
			db->surf_T3[j][i].u0= ((im-1)*8);
			db->surf_T3[j][i].v0= ((in+1)*8);
			db->surf_T3[j][i].u1= ((im-1+1)*8);
			db->surf_T3[j][i].v1= (in*8);
			db->surf_T3[j][i].u2= ((im-1+1)*8);
			db->surf_T3[j][i].v2= ((in+1)*8);
			db->surf_T3[j][i].tpage= tpage;
			db->surf_T3[j][i].clut= clut;		
		}
	    }
	}
}
/* load textures to the frame buffer*/
texture_init(addr) 
u_long addr;
{
  RECT rect1;
  GsIMAGE tim1;

  /* get texture tyep information from TIM header*/
  GsGetTimInfo((u_long *)(addr+4),&tim1); 
  
  rect1.x=tim1.px;	/* x*/
  rect1.y=tim1.py;	/* y*/
  rect1.w=tim1.pw;	/* width*/
  rect1.h=tim1.ph;	/* height*/

  /* load texture data to the frame buffer*/
  LoadImage(&rect1,tim1.pixel); 
  tpage=GetTPage(tim1.pmode,0,640,0);
  tpage= tpage&0xfe7f;          /*for GetTPage bug*/

  /* if there is some CLUTs, load them*/
  if((tim1.pmode>>3)&0x01)     
    {
      rect1.x=tim1.cx;          /* x*/
      rect1.y=tim1.cy;          /* y*/
      rect1.w=tim1.cw;          /* width*/
      rect1.h=tim1.ch;          /* height*/
      
      /* load CLUT to the frame buffer*/
      LoadImage(&rect1,tim1.clut); 
    }
  clut=GetClut(tim1.cx,tim1.cy);
}

/* get the largest absolute value */
absmax3(a,b,c)
long a,b,c;
{
        long    p,q,r;

        p=a; q=b; r=c;

        if(p<0) p= -p;
        if(q<0) q= -q;
        if(r<0) r= -r;

        if((p>=q)&&(p>=r)) return(a);
        if((q>=p)&&(q>=r)) return(b);
        if((r>=p)&&(r>=q)) return(c);
}

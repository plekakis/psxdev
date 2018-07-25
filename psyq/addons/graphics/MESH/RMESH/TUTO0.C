/* $PSLibId: Runtime Library Release 3.6$ */
/*			main: 
 *
 *		RotMeshR... test program
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


#define G3_X		(-500)
#define G3_Y		(-200)

#define F3_X		(-250)
#define F3_Y		(-200)

#define FT3_X		(0)
#define FT3_Y		(-200)

#define GT3_X		(250)
#define GT3_Y		(-200)

#define GC3_X		(-500)
#define GC3_Y		(200)

#define FC3_X		(-250)
#define FC3_Y		(200)

#define FCT3_X          (0)
#define FCT3_Y          (200)

#define GCT3_X          (250)
#define GCT3_Y          (200)

#define T3_X            (500)
#define T3_Y            (200)


#define TIM_ADDR 	0x80020000
#define TIM_HEADER 	0x00000010

#define PIH		640
#define PIV		480
#define BGR		60			/* BG color R */
#define BGG		120			/* BG color G */
#define BGB		120			/* BG color B */
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

#define MESH_LEN	50			/* number of vertex */
#define PRIM_LEN	(MESH_LEN-2)		/* number of primitive */
#define DPQ		1			/* 0:dpq off 1:on */
#define BACKC		0			/* 0:nclip off 1:on */

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_G3		surf_G3[PRIM_LEN];
	POLY_F3		surf_F3[PRIM_LEN];
	POLY_FT3	surf_FT3[PRIM_LEN];
	POLY_GT3	surf_GT3[PRIM_LEN];
	POLY_G3		surf_GC3[PRIM_LEN];
	POLY_F3		surf_FC3[PRIM_LEN];
        POLY_FT3        surf_FCT3[PRIM_LEN];
        POLY_GT3        surf_GCT3[PRIM_LEN];
	POLY_FT3	surf_T3[PRIM_LEN];
} DB;

/*-G3------------------------------------------------------------------*/
SVECTOR p_G3[MESH_LEN];		/* vertex */
CVECTOR c_G3[MESH_LEN];		/* original color */
SVECTOR	n_G3[MESH_LEN];		/* normal */
/*--------------------------------------------------------------------*/
/*-F3------------------------------------------------------------------*/
SVECTOR p_F3[MESH_LEN];		/* vertex */
CVECTOR c_F3[MESH_LEN];		/* original color */
SVECTOR	n_F3[MESH_LEN];		/* normal */
/*--------------------------------------------------------------------*/
/*-FT3------------------------------------------------------------------*/
SVECTOR p_FT3[MESH_LEN];	/* vertex */
CVECTOR c_FT3[MESH_LEN];	/* original color */
SVECTOR	n_FT3[MESH_LEN];	/* normal */
/*--------------------------------------------------------------------*/
/*-GT3------------------------------------------------------------------*/
SVECTOR p_GT3[MESH_LEN];	/* vertex */
CVECTOR c_GT3[MESH_LEN];	/* original color */
SVECTOR	n_GT3[MESH_LEN];	/* normal */
/*--------------------------------------------------------------------*/
/*-GC3------------------------------------------------------------------*/
SVECTOR p_GC3[MESH_LEN];	/* vertex */
CVECTOR c_GC3[MESH_LEN];	/* original color */
SVECTOR	n_GC3[MESH_LEN];	/* normal */
/*--------------------------------------------------------------------*/
/*-FC3------------------------------------------------------------------*/
SVECTOR p_FC3[MESH_LEN];	/* vertex */
CVECTOR c_FC3[MESH_LEN];	/* original color */
SVECTOR	n_FC3[MESH_LEN];	/* normal */
/*--------------------------------------------------------------------*/
/*-FCT3------------------------------------------------------------------*/
SVECTOR p_FCT3[MESH_LEN];       /* vertex */
CVECTOR c_FCT3[MESH_LEN];       /* original color */
SVECTOR n_FCT3[MESH_LEN];       /* normal */
/*--------------------------------------------------------------------*/
/*-GCT3------------------------------------------------------------------*/
SVECTOR p_GCT3[MESH_LEN];       /* vertex */
CVECTOR c_GCT3[MESH_LEN];       /* original color */
SVECTOR n_GCT3[MESH_LEN];       /* normal */
/*--------------------------------------------------------------------*/
/*-T3------------------------------------------------------------------*/
SVECTOR p_T3[MESH_LEN];		/* vertex */
CVECTOR c_T3[MESH_LEN];		/* original color */
SVECTOR	n_T3[MESH_LEN];		/* normal */
/*--------------------------------------------------------------------*/


MATRIX lo ={0,0,4096,		/* light direction 1 (eye to screen)*/
	    0,4096,0,		/* light direction 2 (up to down)*/
	    -4096,0,0,		/* light direction 3 (right to left)*/
	    0,0,0};			

MATRIX lc = {4096,0,0,				/* light color 1(red)*/
	     0,4096,0,				/* light color 2(green)*/
	     0,0,4096,				/* light color 3(blue) */
	     0,0,0};				/* light color matrix*/

MATRIX	l;					/* LocalLightMatrix */

u_short tpage;
u_short clut;
u_long  backc=BACKC;
u_long  dpq=DPQ;

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
	TMESH		msh_GT3;
	TMESH		msh_GC3;
	TMESH		msh_FC3;
        TMESH           msh_FCT3;
        TMESH           msh_GCT3;
	TMESH		msh_T3;

	long		flag[PRIM_LEN];
	int		i;
	long		sxy0,sxy1,sxy2;
	VECTOR		v0,v1,v2;
	VECTOR		nml[PRIM_LEN];	/*primitive normal*/
	long		a,b;

	
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
	

	/* init primitives */
	init_prim(&db[0]);
	init_prim(&db[1]);
	
	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	/*genarate vertex*/
	p_G3[0].vx= 0+G3_X;
	p_G3[0].vy= 0+G3_Y;
	p_G3[0].vz= 0;
	
	p_F3[0].vx= 0+F3_X;
	p_F3[0].vy= 0+F3_Y;
	p_F3[0].vz= 0;
	
	p_FT3[0].vx= 0+FT3_X;
	p_FT3[0].vy= 0+FT3_Y;
	p_FT3[0].vz= 0;
	
	p_GT3[0].vx= 0+GT3_X;
	p_GT3[0].vy= 0+GT3_Y;
	p_GT3[0].vz= 0;
	
	p_GC3[0].vx= 0+GC3_X;
	p_GC3[0].vy= 0+GC3_Y;
	p_GC3[0].vz= 0;
	
	p_FC3[0].vx= 0+FC3_X;
	p_FC3[0].vy= 0+FC3_Y;
	p_FC3[0].vz= 0;
	
	p_GCT3[0].vx= 0+GCT3_X;
	p_GCT3[0].vy= 0+GCT3_Y;
	p_GCT3[0].vz= 0;
	
	p_FCT3[0].vx= 0+FCT3_X;
	p_FCT3[0].vy= 0+FCT3_Y;
	p_FCT3[0].vz= 0;
	
	p_T3[0].vx= 0+T3_X;
	p_T3[0].vy= 0+T3_Y;
	p_T3[0].vz= 0;
	
	for(i=1;i<MESH_LEN;i++){
		p_G3[i].vx= rcos(-100*i)/40 +rand()%20 + G3_X;
		p_G3[i].vy= (i%2)*30 +i*2 - 200 +rand()%20 + G3_Y;
		p_G3[i].vz= rsin(-100*i)/40 +rand()%20;

		p_F3[i].vx= rcos(-100*i)/40 +rand()%20 + F3_X;
		p_F3[i].vy= (i%2)*30 +i*2 - 200 +rand()%20 + F3_Y;
		p_F3[i].vz= rsin(-100*i)/40 +rand()%20;

		p_FT3[i].vx= rcos(-100*i)/40 +rand()%20 + FT3_X;
		p_FT3[i].vy= (i%2)*30 +i*2 - 200 +rand()%20 + FT3_Y;
		p_FT3[i].vz= rsin(-100*i)/40 +rand()%20;

		p_GT3[i].vx= rcos(-100*i)/40 +rand()%20 + GT3_X;
		p_GT3[i].vy= (i%2)*30 +i*2 - 200 +rand()%20 + GT3_Y;
		p_GT3[i].vz= rsin(-100*i)/40 +rand()%20;

		p_GC3[i].vx= rcos(-100*i)/40 +rand()%20 + GC3_X;
		p_GC3[i].vy= (i%2)*30 +i*2 - 200 +rand()%20 + GC3_Y;
		p_GC3[i].vz= rsin(-100*i)/40 +rand()%20;

		p_FC3[i].vx= rcos(-100*i)/40 +rand()%20 + FC3_X;
		p_FC3[i].vy= (i%2)*30 +i*2 - 200 +rand()%20 + FC3_Y;
		p_FC3[i].vz= rsin(-100*i)/40 +rand()%20;

		p_GCT3[i].vx= rcos(-100*i)/40 +rand()%20 + GCT3_X;
		p_GCT3[i].vy= (i%2)*30 +i*2 - 200 +rand()%20 + GCT3_Y;
		p_GCT3[i].vz= rsin(-100*i)/40 +rand()%20;

		p_FCT3[i].vx= rcos(-100*i)/40 +rand()%20 + FCT3_X;
		p_FCT3[i].vy= (i%2)*30 +i*2 - 200 +rand()%20 + FCT3_Y;
		p_FCT3[i].vz= rsin(-100*i)/40 +rand()%20;

		p_T3[i].vx= rcos(-100*i)/40 +rand()%20 + T3_X;
		p_T3[i].vy= (i%2)*30 +i*2 - 200 +rand()%20 + T3_Y;
		p_T3[i].vz= rsin(-100*i)/40 +rand()%20;
	}

	/*generate color*/
	for(i=0;i<MESH_LEN;i++){
		c_G3[i].r=255;
		c_G3[i].g=255;
		c_G3[i].b=255;
		c_G3[i].cd=db[0].surf_G3[0].code;	/* GPU code copy */

		c_F3[i].r=255;
		c_F3[i].g=255;
		c_F3[i].b=255;
		c_F3[i].cd=db[0].surf_F3[0].code;	/* GPU code copy */

		c_FT3[i].r=255;
		c_FT3[i].g=255;
		c_FT3[i].b=255;
		c_FT3[i].cd=db[0].surf_FT3[0].code;	/* GPU code copy */

		c_GT3[i].r=255;
		c_GT3[i].g=255;
		c_GT3[i].b=255;
		c_GT3[i].cd=db[0].surf_GT3[0].code;	/* GPU code copy */

                c_GC3[i].r= (20*i)%255;
                c_GC3[i].g= (20*i)%255;
                c_GC3[i].b= (20*i)%255;
                c_GC3[i].cd=db[0].surf_GC3[0].code;     /* GPU code copy */

                c_FC3[i].r= (20*i)%255;
                c_FC3[i].g= (20*i)%255;
                c_FC3[i].b= (20*i)%255;
                c_FC3[i].cd=db[0].surf_FC3[0].code;     /* GPU code copy */

                c_GCT3[i].r= (20*i)%255;
                c_GCT3[i].g= (20*i)%255;
                c_GCT3[i].b= (20*i)%255;
                c_GCT3[i].cd=db[0].surf_GCT3[0].code;     /* GPU code copy */

                c_FCT3[i].r= (20*i)%255;
                c_FCT3[i].g= (20*i)%255;
                c_FCT3[i].b= (20*i)%255;
                c_FCT3[i].cd=db[0].surf_FCT3[0].code;     /* GPU code copy */

                c_T3[i].r= 128;
                c_T3[i].g= 128;
                c_T3[i].b= 128;
                c_T3[i].cd=db[0].surf_T3[0].code;       /* GPU code copy */

	}

        /*set color in packet*/
        for(i=0;i<PRIM_LEN;i++){
                db[0].surf_GC3[i].r0= c_GC3[i].r;
                db[0].surf_GC3[i].g0= c_GC3[i].g;
                db[0].surf_GC3[i].b0= c_GC3[i].b;
                db[0].surf_GC3[i].r1= c_GC3[i+1].r;
                db[0].surf_GC3[i].g1= c_GC3[i+1].g;
                db[0].surf_GC3[i].b1= c_GC3[i+1].b;
                db[0].surf_GC3[i].r2= c_GC3[i+2].r;
                db[0].surf_GC3[i].g2= c_GC3[i+2].g;
                db[0].surf_GC3[i].b2= c_GC3[i+2].b;
                db[1].surf_GC3[i].r0= c_GC3[i].r;
                db[1].surf_GC3[i].g0= c_GC3[i].g;
                db[1].surf_GC3[i].b0= c_GC3[i].b;
                db[1].surf_GC3[i].r1= c_GC3[i+1].r;
                db[1].surf_GC3[i].g1= c_GC3[i+1].g;
                db[1].surf_GC3[i].b1= c_GC3[i+1].b;
                db[1].surf_GC3[i].r2= c_GC3[i+2].r;
                db[1].surf_GC3[i].g2= c_GC3[i+2].g;
                db[1].surf_GC3[i].b2= c_GC3[i+2].b;

                db[0].surf_FC3[i].r0= c_FC3[i].r;
                db[0].surf_FC3[i].g0= c_FC3[i].g;
                db[0].surf_FC3[i].b0= c_FC3[i].b;
                db[1].surf_FC3[i].r0= c_FC3[i].r;
                db[1].surf_FC3[i].g0= c_FC3[i].g;
                db[1].surf_FC3[i].b0= c_FC3[i].b;

                db[0].surf_GCT3[i].r0= c_GCT3[i].r;
                db[0].surf_GCT3[i].g0= c_GCT3[i].g;
                db[0].surf_GCT3[i].b0= c_GCT3[i].b;
                db[0].surf_GCT3[i].r1= c_GCT3[i+1].r;
                db[0].surf_GCT3[i].g1= c_GCT3[i+1].g;
                db[0].surf_GCT3[i].b1= c_GCT3[i+1].b;
                db[0].surf_GCT3[i].r2= c_GCT3[i+2].r;
                db[0].surf_GCT3[i].g2= c_GCT3[i+2].g;
                db[0].surf_GCT3[i].b2= c_GCT3[i+2].b;
                db[1].surf_GCT3[i].r0= c_GCT3[i].r;
                db[1].surf_GCT3[i].g0= c_GCT3[i].g;
                db[1].surf_GCT3[i].b0= c_GCT3[i].b;
                db[1].surf_GCT3[i].r1= c_GCT3[i+1].r;
                db[1].surf_GCT3[i].g1= c_GCT3[i+1].g;
                db[1].surf_GCT3[i].b1= c_GCT3[i+1].b;
                db[1].surf_GCT3[i].r2= c_GCT3[i+2].r;
                db[1].surf_GCT3[i].g2= c_GCT3[i+2].g;
                db[1].surf_GCT3[i].b2= c_GCT3[i+2].b;

                db[0].surf_FCT3[i].r0= c_FCT3[i].r;
                db[0].surf_FCT3[i].g0= c_FCT3[i].g;
                db[0].surf_FCT3[i].b0= c_FCT3[i].b;
                db[1].surf_FCT3[i].r0= c_FCT3[i].r;
                db[1].surf_FCT3[i].g0= c_FCT3[i].g;
                db[1].surf_FCT3[i].b0= c_FCT3[i].b;

                db[0].surf_T3[i].r0= c_T3[i].r;
                db[0].surf_T3[i].g0= c_T3[i].g;
                db[0].surf_T3[i].b0= c_T3[i].b;
                db[1].surf_T3[i].r0= c_T3[i].r;
                db[1].surf_T3[i].g0= c_T3[i].g;
                db[1].surf_T3[i].b0= c_T3[i].b;
        }


	/*generate normals*/
	for(i=0;i<PRIM_LEN;i++){
		v0.vx= p_G3[i+1].vx - p_G3[0].vx;
		v0.vy= p_G3[i+1].vy - p_G3[0].vy;
		v0.vz= p_G3[i+1].vz - p_G3[0].vz;
		v1.vx= p_G3[i+2].vx - p_G3[0].vx;
		v1.vy= p_G3[i+2].vy - p_G3[0].vy;
		v1.vz= p_G3[i+2].vz - p_G3[0].vz;

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
	for(i=0;i<PRIM_LEN;i++){
		n_G3[i].vx= nml[i].vx;
		n_G3[i].vy= nml[i].vy;
		n_G3[i].vz= nml[i].vz;
	}
	n_G3[MESH_LEN-2].vx= nml[PRIM_LEN-1].vx;
	n_G3[MESH_LEN-2].vy= nml[PRIM_LEN-1].vy;
	n_G3[MESH_LEN-2].vz= nml[PRIM_LEN-1].vz;
	n_G3[MESH_LEN-1].vx= nml[PRIM_LEN-1].vx;
	n_G3[MESH_LEN-1].vy= nml[PRIM_LEN-1].vy;
	n_G3[MESH_LEN-1].vz= nml[PRIM_LEN-1].vz;

	/*generate F3 normals*/
	for(i=0;i<PRIM_LEN;i++){
		n_F3[i].vx= nml[i].vx;
		n_F3[i].vy= nml[i].vy;
		n_F3[i].vz= nml[i].vz;
	}

	/*generate FT3 normals*/
	for(i=0;i<PRIM_LEN;i++){
		n_FT3[i].vx= nml[i].vx;
		n_FT3[i].vy= nml[i].vy;
		n_FT3[i].vz= nml[i].vz;
	}

	/*generate GT3 normals*/
	for(i=0;i<PRIM_LEN;i++){
		n_GT3[i].vx= nml[i].vx;
		n_GT3[i].vy= nml[i].vy;
		n_GT3[i].vz= nml[i].vz;
	}
	n_GT3[MESH_LEN-2].vx= nml[PRIM_LEN-1].vx;
	n_GT3[MESH_LEN-2].vy= nml[PRIM_LEN-1].vy;
	n_GT3[MESH_LEN-2].vz= nml[PRIM_LEN-1].vz;
	n_GT3[MESH_LEN-1].vx= nml[PRIM_LEN-1].vx;
	n_GT3[MESH_LEN-1].vy= nml[PRIM_LEN-1].vy;
	n_GT3[MESH_LEN-1].vz= nml[PRIM_LEN-1].vz;

	/*set MESH*/
	msh_G3.v=p_G3;
	msh_G3.n=n_G3;
	msh_G3.c=c_G3;
	msh_G3.len=MESH_LEN;

	msh_F3.v=p_F3;
	msh_F3.n=n_F3;
	msh_F3.c=c_F3;
	msh_F3.len=MESH_LEN;

	msh_FT3.v=p_FT3;
	msh_FT3.n=n_FT3;
	msh_FT3.c=c_FT3;
	msh_FT3.len=MESH_LEN;

	msh_GT3.v=p_GT3;
	msh_GT3.n=n_GT3;
	msh_GT3.c=c_GT3;
	msh_GT3.len=MESH_LEN;

	msh_GC3.v=p_GC3;
	msh_GC3.c=c_GC3;
	msh_GC3.len=MESH_LEN;

	msh_FC3.v=p_FC3;
	msh_FC3.c=c_FC3;
	msh_FC3.len=MESH_LEN;

	msh_GCT3.v=p_GCT3;
	msh_GCT3.c=c_GCT3;
	msh_GCT3.len=MESH_LEN;

	msh_FCT3.v=p_FCT3;
	msh_FCT3.c=c_FCT3;
	msh_FCT3.len=MESH_LEN;

	msh_T3.v=p_T3;
	msh_T3.c=c_T3;
	msh_T3.len=MESH_LEN;


	while (pad_read() == 0) {
		
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */


		/* rotate & Transfer & Perspect & AverageZ & NormalClip &
				 lighting & DepthQuing of 3 vertices */
		RotMeshPrimR_G3(&msh_G3,cdb->surf_G3,cdb->ot,OTLEN,dpq,backc);
		RotMeshPrimR_F3(&msh_F3,cdb->surf_F3,cdb->ot,OTLEN,dpq,backc);
		RotMeshPrimR_FT3(
			&msh_FT3,cdb->surf_FT3,cdb->ot,OTLEN,dpq,backc);
		RotMeshPrimR_GT3(
			&msh_GT3,cdb->surf_GT3,cdb->ot,OTLEN,dpq,backc);
		RotMeshPrimR_GC3(
			&msh_GC3,cdb->surf_GC3,cdb->ot,OTLEN,dpq,backc);
		RotMeshPrimR_FC3(
			&msh_FC3,cdb->surf_FC3,cdb->ot,OTLEN,dpq,backc);
		RotMeshPrimR_GCT3(
			&msh_GCT3,cdb->surf_GCT3,cdb->ot,OTLEN,dpq,backc);
		RotMeshPrimR_FCT3(
			&msh_FCT3,cdb->surf_FCT3,cdb->ot,OTLEN,dpq,backc);
		RotMeshPrimR_T3(&msh_T3,cdb->surf_T3,cdb->ot,OTLEN,dpq,backc);

		/* swap buffer */
		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */

		DrawOTag(cdb->ot+OTSIZE-1);	/* draw */
	
	}
	ResetGraph(3);
	StopCallback();
	return 0;
}

static pad_read()
{
	static SVECTOR	ang   = {0, 0, 0};	/* rotate angle */
	static VECTOR	vec   = {0, 0, 2*SCR_Z};	/* translate vector */
	static MATRIX	m;				/* matrix */
	
	int	ret = 0;	
	u_long	padd = PadRead(0);
	
	if (padd & PADRup)	ang.vx -= 32;
	if (padd & PADRdown)	ang.vx += 32;
	if (padd & PADRleft) 	ang.vy -= 32;
	if (padd & PADRright)	ang.vy += 32;
	if (padd & PADm) 	ang.vz -= 32;
	if (padd & PADo)	ang.vz += 32;
	
      if((padd & PADLleft)>0) vec.vx -=10;
      if((padd & PADLright)>0) vec.vx +=10;
      if((padd & PADLdown)>0) vec.vy+=10;
      if((padd & PADLup)>0) vec.vy-=10;

	if (padd & PADl)	vec.vz += 8;
	if (padd & PADn) 	vec.vz -= 8;

        if(padd & PADm)         dpq=1;
        if(padd & PADo)         dpq=0;

        if(padd & PADh)         backc=1;
        if(padd & PADk)         backc=0;

        if(((padd & PADm)!=0)&&((padd & PADo)!=0)&&((padd & PADk)!=0)){
                ret= -1;
        }


	RotMatrix(&ang, &m);	
	TransMatrix(&m, &vec);	

	MulMatrix0(&lo,&m,&l);		/* MulMatrix destroys RotMatrix */
					/* don't use after SetRotMatrix */
	SetLightMatrix(&l);		/* LocalLightMatrix must be changed 
						 as Object coordinate */

	SetRotMatrix(&m);
	SetTransMatrix(&m);

	return(ret);
}		

static init_prim(db)
DB	*db;
{
	int i;

	db->draw.isbg=1;
	setRGB0( &db->draw, BGR, BGG, BGB);	/* (r,g,b) = (BGR,BGG,BGB) */
	
	/* initialize surf */
	for(i=0;i<PRIM_LEN;i++){
		SetPolyG3(&db->surf_G3[i]);	/* set POLY_G3 primitve ID */
		SetPolyF3(&db->surf_F3[i]);	/* set POLY_F3 primitve ID */
		SetPolyFT3(&db->surf_FT3[i]);	/* set POLY_FT3 primitve ID */
		SetPolyGT3(&db->surf_GT3[i]);	/* set POLY_FT3 primitve ID */
		SetPolyG3(&db->surf_GC3[i]);	/* set POLY_G3 primitve ID */
		SetPolyF3(&db->surf_FC3[i]);	/* set POLY_F3 primitve ID */
		SetPolyGT3(&db->surf_GCT3[i]);	/* set POLY_GT3 primitve ID */
		SetPolyFT3(&db->surf_FCT3[i]);	/* set POLY_FT3 primitve ID */
		SetPolyFT3(&db->surf_T3[i]);	/* set POLY_FT3 primitve ID */

		db->surf_FT3[i].u0=0;
		db->surf_FT3[i].v0=0;
		db->surf_FT3[i].u1=63;
		db->surf_FT3[i].v1=0;
		db->surf_FT3[i].u2=63;
		db->surf_FT3[i].v2=63;
		db->surf_FT3[i].tpage= tpage;
		db->surf_FT3[i].clut= clut;		

		db->surf_GT3[i].u0=0;
		db->surf_GT3[i].v0=0;
		db->surf_GT3[i].u1=63;
		db->surf_GT3[i].v1=0;
		db->surf_GT3[i].u2=63;
		db->surf_GT3[i].v2=63;
		db->surf_GT3[i].tpage= tpage;
		db->surf_GT3[i].clut= clut;		

		db->surf_FCT3[i].u0=0;
		db->surf_FCT3[i].v0=0;
		db->surf_FCT3[i].u1=63;
		db->surf_FCT3[i].v1=0;
		db->surf_FCT3[i].u2=63;
		db->surf_FCT3[i].v2=63;
		db->surf_FCT3[i].tpage= tpage;
		db->surf_FCT3[i].clut= clut;		

		db->surf_GCT3[i].u0=0;
		db->surf_GCT3[i].v0=0;
		db->surf_GCT3[i].u1=63;
		db->surf_GCT3[i].v1=0;
		db->surf_GCT3[i].u2=63;
		db->surf_GCT3[i].v2=63;
		db->surf_GCT3[i].tpage= tpage;
		db->surf_GCT3[i].clut= clut;		

		db->surf_T3[i].u0=0;
		db->surf_T3[i].v0=0;
		db->surf_T3[i].u1=63;
		db->surf_T3[i].v1=0;
		db->surf_T3[i].u2=63;
		db->surf_T3[i].v2=63;
		db->surf_T3[i].tpage= tpage;
		db->surf_T3[i].clut= clut;		
	}
}

/* load textures to the frame buffer: テクスチャデータをVRAMにロードする */
texture_init(addr) 
u_long addr;
{
  RECT rect1;
  GsIMAGE tim1;

  /* get texture tyep information from TIM header:
     TIMデータのヘッダからテクスチャのデータタイプの情報を得る */
  GsGetTimInfo((u_long *)(addr+4),&tim1); 
  
  rect1.x=tim1.px;	/* x: テクスチャ左上のVRAMでのX座標 */
  rect1.y=tim1.py;	/* y: テクスチャ左上のVRAMでのY座標 */
  rect1.w=tim1.pw;	/* width: テクスチャ幅 */
  rect1.h=tim1.ph;	/* height: テクスチャ高さ */

  /* load texture data to the frame buffer: VRAMにテクスチャをロードする */
  LoadImage(&rect1,tim1.pixel); 
  tpage=GetTPage(tim1.pmode,0,640,0);
  tpage= tpage&0xfe7f;          /*for GetTPage bug*/

  /* if there is some CLUTs, load them: カラールックアップテーブルが存在する */
  if((tim1.pmode>>3)&0x01)     
    {
      rect1.x=tim1.cx;          /* x: クラット左上のVRAMでのX座標 */
      rect1.y=tim1.cy;          /* y: クラット左上のVRAMでのY座標 */
      rect1.w=tim1.cw;          /* width: クラットの幅 */
      rect1.h=tim1.ch;          /* height: クラットの高さ */
      
      /* load CLUT to the frame buffer: VRAMにクラットをロードする */
      LoadImage(&rect1,tim1.clut); 
    }
  clut=GetClut(tim1.cx,tim1.cy);
}

/* get the largest absolute value :  絶対値最大を返す*/
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


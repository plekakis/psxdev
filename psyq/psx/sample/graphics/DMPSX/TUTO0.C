/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*				
 *
 *		libgte low level function vs DMPSX macro 
 *			Speed Comparing Program
 *
 *
 *		Copyright (C) 1993/1994/1995/1996 by Sony Corporation
 *			All rights Reserved
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <inline_c.h>
#include <gtemac.h>


/*
#define DMPSX_MACRO
*/


#define SCR_Z	(512)			/* screen depth (h) */
#define OTLEN	10
#define	OTSIZE	(1<<OTLEN)		/* ordering table size */
#define CUBESIZ	196			/* cube size */
#define TEX_EDGE	16

#define NCLIP		64

#ifdef DMPSX_MACRO
#define NUMX           	30		/*DMPSX: 30x30*/
#define NUMY           	30		
#else
#define NUMX           	20		/*libgte low level: 20x20*/
#define NUMY           	20		
#endif

#define EDGE           	30		/* edge of cube */
#define SPACE		50

#define STH		(-SPACE*(NUMX-1)/2)
#define STV		(-SPACE*(NUMY-1)/2)

#define CENTX		(NUMX>>1)
#define CENTY		(NUMY>>1)

#define	AA		300
#define LL		10

#define PIH             640
#define PIV             480
#define OFX             (PIH/2)                 /* screen offset X */
#define OFY             (PIV/2)                 /* screen offset Y */
#define BGR             255                      /* BG color R */
#define BGG             255                     /* BG color G */
#define BGB             255                     /* BG color B */
#define RBK             0                       /* Back color R */
#define GBK             0                       /* Back color G */
#define BBK             0                       /* Back color B */
#define RFC             BGR                     /* Far color R */
#define GFC             BGG                     /* Far color G */
#define BFC             BGB                     /* Far color B */

typedef struct {
	POLY_F4	        surf[NUMY][NUMX][6];
} PBUF;						/* GPU pachet buffer */


typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	PBUF		pb;
} DB;

typedef struct {
        SVECTOR         v[8];                   /*shared vertices*/
        SVECTOR         n[6];                   /*surface normals*/
        SVECTOR         u[14];                  /*shared texture addresses*/
        CVECTOR         c[6];                   /*surface colors*/
} CUBE;


SVECTOR	move[NUMY][NUMX];
long	ite;
long	DPQ=0;
long	FOGNEAR=700;

CVECTOR	SCE[NUMY][NUMX];

typedef struct {
	CUBE	cube;
	POLY_F4	top,bot;
	long	nclip[6];
	long	otz;
	u_long*	otzz;
	MATRIX	SLmat;
	MATRIX	SWmat;
	int	t,s;
	SVECTOR	sv0;
	VECTOR	v0;
	MATRIX	TSmat;
	SVECTOR	moveV;
	SVECTOR tmp;
}WORK;

static pad_read();
static init_prim();

main()
{
	DB		db[2];		/* packet double buffer */
	DB		*cdb;		/* current db */
        int             i,j,k;
        long            ret;
	MATRIX		ll;		/* local light Matrix */
	MATRIX		lc;		/* local color Matrix */
	CVECTOR		bk;		/* ambient color */
	POLY_F4		*pij6;
	long		R;
	WORK		*W;
	int		c,c2;

	W= (WORK*)getScratchAddr(0);

	ResetCallback();
	PadInit(0);		/* reset graphic environment */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	gte_SetGeomOffset(OFX, OFY);	/* set geometry origin as (160, 120) */
	gte_SetGeomScreen(SCR_Z);		/* distance to viewing-screen */
        gte_SetBackColor(RBK,GBK,BBK);      /* set background(ambient) color*/
        gte_SetFarColor(RFC,GFC,BFC);       /* set far color */
        SetFogNear(FOGNEAR,SCR_Z);      /* set fog parameter*/

	/*interlace mode*/	
	SetDefDrawEnv(&db[0].draw, 0,   0, PIH, PIV);	
	SetDefDrawEnv(&db[1].draw, 0,   0, PIH, PIV);	
	SetDefDispEnv(&db[0].disp, 0,   0, PIH, PIV);	
	SetDefDispEnv(&db[1].disp, 0,   0, PIH, PIV);
	db[0].draw.dfe = db[1].draw.dfe = 0;

	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	/*SCE LOGO generate*/
	gen_color_sce(SCE,NUMX,NUMY);

	init_prim(&db[0]);	/* set primitive parameters on buffer #0 */
	init_prim(&db[1]);	/* set primitive parameters on buffer #1 */
	
        /* generate vertex coordinates of cube */
        gen_cube(W->cube.v,EDGE);

	/* generate normals */
	gen_cube_normal(W->cube.n);

	/* use normals for light-source calculations and apply colors */
		ll.m[0][0]= 1800; ll.m[0][1]= 2500; ll.m[0][2]= 3200;
		ll.m[1][0]= -1800; ll.m[1][1]= -2500; ll.m[1][2]= -3200;
		ll.m[2][0]= 0; ll.m[2][1]= 0; ll.m[2][2]= 0;
		lc.m[0][0]= 1800; lc.m[0][1]= 1500; lc.m[0][2]= 0;
		lc.m[1][0]= 1800; lc.m[1][1]= 1500; lc.m[1][2]= 0;
		lc.m[2][0]= 0; lc.m[2][1]= 500; lc.m[2][2]= 0;
		bk.r=60; bk.g=60; bk.b=60;
	gen_cube_color_normal(W->cube.c,W->cube.n,&ll,&lc,&bk);

	for(i=0;i<NUMY;i++)
	for(j=0;j<NUMX;j++){
		R=SquareRoot0((i-CENTY)*(i-CENTY)+(j-CENTX)*(j-CENTX));
		move[i][j].vx=  AA*rcos(R*4096/LL)/4096/(R+1);
		move[i][j].vy=  AA*rsin(R*4096/LL)/4096/(R+1);
	}

	ret=0;

	FntLoad(960,256);
	SetDumpFnt(FntOpen(64, 64, 64, 10, 1, 512));
	GsInitVcount();

	while (pad_read() == 0) {

		gte_ReadRotMatrix(&W->SWmat);

        	SetFogNear(FOGNEAR,SCR_Z);      /* set fog parameter*/

		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */

		ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */


		pij6= cdb->pb.surf[0][0];


		W->sv0.vx= STH;
		W->sv0.vy= STV;

		W->tmp.vx =  rsin(ite);
		W->tmp.vy = -rcos(ite);
		W->tmp.vz = 0;
		ite+=64;

		gte_ldbkdir(W->SWmat.t[0],W->SWmat.t[1],W->SWmat.t[2]);

		gte_ldsvllrow0(&W->tmp);	/*load tmp to [ll] row 0*/

	  	for(i=0;i<NUMY;i++){
	    	  for(j=0;j<NUMX;j++){
#ifdef DMPSX_MACRO
		    /*inner product of tmp and move[i][j]*/
		    gte_ldsv(&move[i][j]);	/*load move[i][j] to [sv]*/
		    gte_llir();			/*[lv]=[ll]*[sv]*/
		    gte_stlvnl0(&W->sv0.vz);	/*store scalar 0 of [lv]*/

		    gte_ldsv(&W->sv0);		/*load local transfer to [sv]*/
		    gte_rtirbk();		/*[lv]=[bk]+[rt]*[sv]*/
		      gte_ldv3c(&W->cube.v[0]);	/*delay slot: moved from (1)*/
						/*     load v[0],v[1],v[2]  */
		    gte_mvlvtr();		/*move [lv] to [tr]*/

		    /*RotTransPers of 8 vertices*/
		    /* (1) */
		    gte_rtpt();			/* RotTransPers3(v0,v1,v2) */
		      W->cube.c[1]= SCE[i][j];
		    gte_stsxy3( 		/* store sxy0,sxy1,sxy2	   */
				(long*)&W->top.x0,
				(long*)&W->top.x1,
				(long*)&W->top.x3);
		    gte_stszotz(&W->otz);   /* store otz(1 otz for 1 cube) */
		    gte_nclip();	    	/* normal clip */
		      gte_ldv3(			/*delay slot: moved from (2)*/
				&W->cube.v[4],	/*     load v[4],v[3],v[7]  */
				&W->cube.v[3],
				&W->cube.v[7]);
		    gte_stopz(&W->nclip[0]);	/* store opz */

		    /* (2) */
		    gte_rtpt();			/* RotTransPers3(v4,v3,v7) */
		      W->otzz = cdb->ot+ W->otz; /*delay slot: moved from (3)*/
		    gte_stsxy3(			/* store sxy4,sxy3,sxy7 */ 
				(long*)&W->bot.x0,
				(long*)&W->top.x2,
				(long*)&W->bot.x1);
		    gte_nclip();		/* normal clip */
		      gte_ldv3c(&W->cube.v[5]);	/*delay slot: moved from (4)*/
						/*     load v[5],v[6],v[7]  */
		    gte_stopz(&W->nclip[1]);	/* store opz */

		    /* (4) */
		    gte_rtpt();			/* RotTransPers3(v5,v6,v7) */
		      gte_ldrgb3c(		/*delay slot: moved from (5)*/
				&W->cube.c[0]);	/* load rgb0,rgb1,rgb2 */
		    gte_stsxy3( 		/* store sxy5,sxy6,sxy7	 */
				(long*)&W->bot.x2,
				(long*)&W->bot.x3,
				(long*)&W->bot.x1);

		    gte_ldsxy0(*(long*)&W->top.x3);  /*load sxy2*/
		    gte_nclip();		/* normal clip */
		    gte_stopz(&W->nclip[2]);	/* load opz */

	 	    if(DPQ==1){			/* Fog ON */
			/* (5) */
			gte_dpct();		/* depth quing */
			gte_strgb3(		/* store rgb0,rgb1,rgb2 */
				(CVECTOR*)&pij6[0].r0,
				(CVECTOR*)&pij6[1].r0,
				(CVECTOR*)&pij6[2].r0);
			gte_ldrgb3c(&W->cube.c[3]); /* load rgb3,rgb4,rgb5 */
			gte_dpct();		/* depth quing */
			gte_strgb3(		/* store rgb3,rgb4,rgb5 */
				(CVECTOR*)&pij6[3].r0,
				(CVECTOR*)&pij6[4].r0,
				(CVECTOR*)&pij6[5].r0);

		    }else{			/* Fog OFF */
						/* copy cube color to packet */
			*(long *)&pij6[0].r0= *(long *)&W->cube.c[0];
			*(long *)&pij6[1].r0= *(long *)&W->cube.c[1];
			*(long *)&pij6[2].r0= *(long *)&W->cube.c[2];
			*(long *)&pij6[3].r0= *(long *)&W->cube.c[3];
			*(long *)&pij6[4].r0= *(long *)&W->cube.c[4];
			*(long *)&pij6[5].r0= *(long *)&W->cube.c[5];
		    }
#else
                    W->sv0.vz = 
			(W->tmp.vx*move[i][j].vx-W->tmp.vy*move[i][j].vy)>>12;

                    ApplyRotMatrix(&W->sv0,&W->v0);

                    W->SLmat.t[0] = W->SWmat.t[0]+ W->v0.vx;
                    W->SLmat.t[1] = W->SWmat.t[1]+ W->v0.vy;
                    W->SLmat.t[2] = W->SWmat.t[2]+ W->v0.vz;

                    SetTransMatrix(&W->SLmat);

		    RotCubeFog(&W->cube,pij6,W->nclip,&W->otz,&SCE[i][j]);
#endif
		    if(W->otz>=NCLIP && W->otz<OTSIZE){ 
#ifdef DMPSX_MACRO
			/* (3) */
			if(W->nclip[0]>0){	/* surface 0 is visible */
			    *(long *)(&pij6[0].x0) = *(long *)(&W->top.x0);
			    *(long *)(&pij6[0].x1) = *(long *)(&W->top.x1);
			    *(long *)(&pij6[0].x2) = *(long *)(&W->top.x2);
			    *(long *)(&pij6[0].x3) = *(long *)(&W->top.x3);
               		    addPrim(W->otzz,&pij6[0]);
			}else{			/* surface 5 is visible */
			    *(long *)(&pij6[5].x0) = *(long *)(&W->bot.x0);
			    *(long *)(&pij6[5].x1) = *(long *)(&W->bot.x1);
			    *(long *)(&pij6[5].x2) = *(long *)(&W->bot.x2);
			    *(long *)(&pij6[5].x3) = *(long *)(&W->bot.x3);
               		    addPrim(W->otzz,&pij6[5]);
			}

		  	if(W->nclip[1]>0){	/* surface 1 is visible */
			    *(long *)(&pij6[1].x0) = *(long *)(&W->top.x0);
			    *(long *)(&pij6[1].x1) = *(long *)(&W->top.x2);
			    *(long *)(&pij6[1].x2) = *(long *)(&W->bot.x0);
			    *(long *)(&pij6[1].x3) = *(long *)(&W->bot.x1);
               		    addPrim(W->otzz,&pij6[1]);
			}else{			/* surface 3 is visible */
			    *(long *)(&pij6[3].x0) = *(long *)(&W->top.x3);
			    *(long *)(&pij6[3].x1) = *(long *)(&W->top.x1);
			    *(long *)(&pij6[3].x2) = *(long *)(&W->bot.x3);
			    *(long *)(&pij6[3].x3) = *(long *)(&W->bot.x2);
               		    addPrim(W->otzz,&pij6[3]);
			}

			if(W->nclip[2]>0){	/* surface 2 is visible */
			    *(long *)(&pij6[2].x0) = *(long *)(&W->top.x2);
			    *(long *)(&pij6[2].x1) = *(long *)(&W->top.x3);
			    *(long *)(&pij6[2].x2) = *(long *)(&W->bot.x1);
			    *(long *)(&pij6[2].x3) = *(long *)(&W->bot.x3);
               		    addPrim(W->otzz,&pij6[2]);
			}else{			/* surface 4 is visible */
			    *(long *)(&pij6[4].x0) = *(long *)(&W->top.x1);
			    *(long *)(&pij6[4].x1) = *(long *)(&W->top.x0);
			    *(long *)(&pij6[4].x2) = *(long *)(&W->bot.x2);
			    *(long *)(&pij6[4].x3) = *(long *)(&W->bot.x0);
               		    addPrim(W->otzz,&pij6[4]);
			}
#else
			AddPrimFogF(pij6,W->nclip,cdb->ot,W->otz);
#endif
		    }
		    pij6 += 6;
		    W->sv0.vx += SPACE;

		  }
		  W->sv0.vx = STH;
		  W->sv0.vy += SPACE;
		}

		/* swap buffer */
		DrawSync(0);	/* wait for end of drawing */
		c=GsGetVcount();
		FntPrint("c=%d\n",c);

		VSync(0);	/* wait for the next V-BLNK */

		GsClearVcount();
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */
		DrawOTag(cdb->ot+OTSIZE-1);	/* draw */
		FntFlush(-1);
	}
	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
}

/*
 *	set matrix according to user's input
 */
#define p(x)	printf("\t%s\n", x)
static usage()
{
	p("            Moving a Cube                 ");
	p("Right Cross Key:     rotate cube.          ");
	p("I-key, J-key:        move cube position    ");
	p("K-key:		end                   ");
}
#undef p

static pad_read()
{
	static SVECTOR	ang  = { 0, 0, -512};	/* rotate angle */
	static VECTOR	vec  = {0, 0, 3*SCR_Z};	/* rottranslate vector */

	int	ret = 0;	
	MATRIX	mat;

/*	u_long	padd = PadRead(0);*/
	u_long	padd;

	padd = PadRead(1);

	/* rotate light source and cube */
	if (padd & PADRup)	ang.vx += 32;
	if (padd & PADRdown)	ang.vx -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* change distance */
	if (padd & PADm)	vec.vz += 8;
	if (padd & PADo) 	vec.vz -= 8;

	if (padd & PADl)	ang.vz +=32;
	if (padd & PADn)	ang.vz -=32;

	if (padd & PADLup)	DPQ=1;
	if (padd & PADLdown)	DPQ=0;
	if (padd & PADLleft)	FOGNEAR += 10;
	if (padd & PADLright)	FOGNEAR -= 10;

	if (padd & PADk) 	ret = -1;

	RotMatrixZYX(&ang, &mat);
	
	/* set matrix */
	TransMatrix(&mat, &vec);	

	gte_SetRotMatrix(&mat);
	gte_SetTransMatrix(&mat);

	return(ret);
}		

/*
 *	initialize primitive parameters
 */
static init_prim(db)
DB	*db;
{
	long	i,j,k;

	db->draw.isbg=1;
	setRGB0(&db->draw, BGR, BGG, BGB);	/* (r,g,b) = (60,120,120) */

	for(j=0;j<NUMY;j++){
		for(i=0;i<NUMX;i++) {
			for(k=0;k<6;k++){
				SetPolyF4(&db->pb.surf[j][i][k]);
			}
		}
	}
}



gen_cube(p,e)
SVECTOR p[8];
long e;
{
	p[0].vx= -e/2; p[0].vy= -e/2; p[0].vz= -e;
	p[1].vx= -e/2; p[1].vy= -e/2; p[1].vz=  e;
	p[2].vx=  e/2; p[2].vy= -e/2; p[2].vz=  e;
	p[3].vx=  e/2; p[3].vy= -e/2; p[3].vz= -e;
	p[4].vx= -e/2; p[4].vy=  e/2; p[4].vz= -e;
	p[5].vx= -e/2; p[5].vy=  e/2; p[5].vz=  e;
	p[6].vx=  e/2; p[6].vy=  e/2; p[6].vz=  e;
	p[7].vx=  e/2; p[7].vy=  e/2; p[7].vz= -e;
}

gen_cube_normal(n)
SVECTOR n[6];
{
	n[0].vx= 0; n[0].vy= 4096; n[0].vz= 0;
	n[1].vx= 0; n[1].vy= 0; n[1].vz= 4096;
	n[2].vx= -4096; n[2].vy= 0; n[2].vz= 0;
	n[3].vx= 0; n[3].vy= 0; n[3].vz= -4096;
	n[4].vx= 4096; n[4].vy= 0; n[4].vz= 0;
	n[5].vx= 0; n[5].vy= -4096; n[5].vz= 0;

}

gen_cube_color_normal(c,n,ll,lc,bk)
CVECTOR c[6];
SVECTOR n[6];
MATRIX *ll,*lc;
CVECTOR *bk;
{
	int k;
	CVECTOR oc;

	oc.r=255; oc.g=255; oc.b=255; oc.cd=40;	/*GPUcode FLAT4*/

	gte_SetLightMatrix(ll);
	gte_SetColorMatrix(lc);
	gte_SetBackColor(bk->r,bk->g,bk->b);
	
	for(k=0;k<6;k++) gte_NormalColorCol(&n[k],&oc,&c[k]);

}


/******************************Cube function**************************
*		    (3)						*
*		1-----------2					*
*	       /|  (0top)   |\					*
*	      / |           | \					*
*	     0-----------------3				*
*       (4)  |  |   (1)     |  |(2)				*
*	     |  |           |  |				*
*	     |  5-----------6  |				*
*	     | /             \ |				*
*	     |/	   (5bottom)  \|				*
*	     4-----------------7				*
*								*
****************************************************************/
RotCubeFog(cube,prim,nclip,otz,col)
CUBE *cube;
POLY_F4 prim[6];
long nclip[3];
long *otz;
CVECTOR	*col;
{
	long p;
	long flg;
	long	dmy;


	/* Convert coordinates */

	*otz= RotTransPers3(
			&cube->v[0],&cube->v[1],&cube->v[2],
			(long*)&prim[0].x0,
			(long*)&prim[0].x1,
			(long*)&prim[0].x3,
			&p,&flg);
	RotTransPers(&cube->v[3],(long*)&prim[0].x2,&p,&flg);
	RotTransPers3(
			&cube->v[4],&cube->v[7],&cube->v[6],
			(long*)&prim[5].x0,
			(long*)&prim[5].x1,
			(long*)&prim[5].x3,
			&p,&flg);
	RotTransPers(&cube->v[5],(long*)&prim[5].x2,&p,&flg);

	if(DPQ==1){
		DpqColor3(&cube->c[0],&cube->c[1],&cube->c[2],
			p,(CVECTOR*)&prim[0].r0,(CVECTOR*)&prim[1].r0,
			(CVECTOR*)&prim[2].r0);
		DpqColor3(&cube->c[3],&cube->c[4],&cube->c[5],
			p,(CVECTOR*)&prim[3].r0,(CVECTOR*)&prim[4].r0,
			(CVECTOR*)&prim[5].r0);
		DpqColor(col,p,(CVECTOR*)&prim[1].r0);

	}else{
		*(long *)&prim[0].r0= *(long *)&cube->c[0];
		*(long *)&prim[1].r0= *(long *)col;
		*(long *)&prim[2].r0= *(long *)&cube->c[2];
		*(long *)&prim[3].r0= *(long *)&cube->c[3];
		*(long *)&prim[4].r0= *(long *)&cube->c[4];
		*(long *)&prim[5].r0= *(long *)&cube->c[5];
	}
	nclip[0]= NormalClip(
			*(long *)(&prim[0].x0),
			*(long *)(&prim[0].x1),
			*(long *)(&prim[0].x3));
	nclip[1]= NormalClip(
			*(long *)(&prim[5].x0),
			*(long *)(&prim[0].x0),
			*(long *)(&prim[0].x2));
	nclip[2]= NormalClip(
			*(long *)(&prim[0].x2),
			*(long *)(&prim[0].x3),
			*(long *)(&prim[5].x3));
}


AddPrimFogF(prim,nclip,ot,otz)
POLY_F4 prim[6];
long nclip[6];
u_long *ot;
long otz;
{
	u_long *otzz;
	
	otzz = ot+otz;

	if(nclip[0]>0){
               	addPrim(otzz,&prim[0]);
	}else{	
               	addPrim(otzz,&prim[5]);
	}

	if(nclip[1]>0){
		*(long *)(&prim[1].x0) = *(long *)(&prim[0].x0);
		*(long *)(&prim[1].x1) = *(long *)(&prim[0].x2);
		*(long *)(&prim[1].x2) = *(long *)(&prim[5].x0);
		*(long *)(&prim[1].x3) = *(long *)(&prim[5].x1);

               	addPrim(otzz,&prim[1]);
	}else{
		*(long *)(&prim[3].x0) = *(long *)(&prim[0].x3);
		*(long *)(&prim[3].x1) = *(long *)(&prim[0].x1);
		*(long *)(&prim[3].x2) = *(long *)(&prim[5].x3);
		*(long *)(&prim[3].x3) = *(long *)(&prim[5].x2);

               	addPrim(otzz,&prim[3]);
	}

	if(nclip[2]>0){
		*(long *)(&prim[2].x0) = *(long *)(&prim[0].x2);
		*(long *)(&prim[2].x1) = *(long *)(&prim[0].x3);
		*(long *)(&prim[2].x2) = *(long *)(&prim[5].x1);
		*(long *)(&prim[2].x3) = *(long *)(&prim[5].x3);

               	addPrim(otzz,&prim[2]);
	}else{	
		*(long *)(&prim[4].x0) = *(long *)(&prim[0].x1);
		*(long *)(&prim[4].x1) = *(long *)(&prim[0].x0);
		*(long *)(&prim[4].x2) = *(long *)(&prim[5].x2);
		*(long *)(&prim[4].x3) = *(long *)(&prim[5].x0);

               	addPrim(otzz,&prim[4]);
	}
}









gen_color_sce(col,numx,numy)
CVECTOR	*col;
int	numx,numy;
{
	int i,j;
	int	pc;

	pc= (256-55)/numx;

	for(i=0;i<numy;i++){
	  for(j=0;j<numx;j++){
		col[i*numx+j].r=255;	
		if((i+j)<numx) 	col[i*numx+j].g= 55 + pc*(i+j);
		else 		col[i*numx+j].g= 255- pc*(i+j-numx);
		col[i*numx+j].b=0;
		col[i*numx+j].cd=40;
	  }
	}

	pc= (210-55)/(numx/2-2);

	for(i=2;i<=numy/2;i++){
	  for(j=numx/2;j<numx-2;j++){
		col[i*numx+j].r=255;	
		if((i+j)<numx+1) 	col[i*numx+j].g= 55+pc*(i+j-numx/2-2);
		col[i*numx+j].b=0;
		col[i*numx+j].cd=40;
	  }
	}

	for(i=numy/2;i<numy-2;i++){
	  for(j=2;j<=numx/2;j++){
		col[i*numx+j].r=255;	
		if((i+j)>=numx) 	col[i*numx+j].g= 210-pc*(i+j-numx);
		col[i*numx+j].b=0;
		col[i*numx+j].cd=40;
	  }
	}
}

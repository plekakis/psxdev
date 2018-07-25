/* $PSLibId: Run-time Library Release 4.4$ */
/*			
 *			Sample of Shadow
 *		  
 *
 *		Copyright (C) 1995 by Sony Corporation
 *			All rights Reserved
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define SCR_Z	512				/* distant to screen */
#define OTLEN	8
#define OTSIZE	(1<<OTLEN)

#define copySVECTOR(sv0,sv1)    (*(long*)&(sv0)->vx = *(long*)&(sv1)->vx), \
                                (*(long*)&(sv0)->vz = *(long*)&(sv1)->vz)

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_F3		lght;			/* light source */	
	POLY_F3		wall[6][2];		/* wall */
	POLY_G3		plane;			/* object */
	POLY_F3		shdw[6][2][4];		/* partial shadow */
} DB;

typedef struct {
	SVECTOR		vert[7]; 		/* intersection of tetra and */
	int             npol;			/*  triangle is max 7-hedron*/
} POLYGON_3D;

typedef struct {				/* tetrahedron */
        SVECTOR         vert[4];
} VOLUME;

typedef struct {
        SVECTOR         vert[3];		/* triangle */
} TRIANGLE_3D;

typedef struct {
        SVECTOR         vert[3];		/* plane */
} PLANE;

static int pad_read();
static void init_prim(DB *db);
static MATRIX	ll= {-2000,-2200,-2500, 0,0,0, 0,0,0, 0,0,0};
static MATRIX	lc= {4096,0,0, 4096,0,0, 4096,0,0, 0,0,0};
static SVECTOR	objang= {0,0,0};
static SVECTOR	lgtang= {0,0,0};

/*initial light position*/
static SVECTOR	l_init[3]= {{150, -210, 0},{160, -210, 0},{160, -200, 10}};

/*wall position*/
static SVECTOR	x[6][2][3]= {
			{{{-256, 256, 256},{256, 256, 256},{-256, 256,-256}},
			 {{ 256, 256, 256},{256, 256,-256},{-256, 256,-256}}},

			{{{-256,-256, 256},{256,-256, 256},{-256, 256, 256}},
			 {{ 256,-256, 256},{256, 256, 256},{-256, 256, 256}}},

			{{{-256,-256,-256},{256,-256,-256},{-256,-256, 256}}, 
			 {{ 256,-256,-256},{256,-256, 256},{-256,-256, 256}}},

			{{{-256, 256,-256},{256, 256,-256},{-256,-256,-256}}, 
			 {{ 256, 256,-256},{256,-256,-256},{-256,-256,-256}}},

			{{{-256,-256,-256},{-256,-256, 256},{-256, 256,-256}}, 
			 {{-256,-256, 256},{-256, 256, 256},{-256, 256,-256}}}, 

			{{{ 256,-256, 256},{ 256,-256,-256},{ 256, 256, 256}},
			 {{ 256,-256,-256},{ 256, 256,-256},{ 256, 256, 256}}}
			}; 

/*wall's normal*/
static SVECTOR	n[6][2]= {
			{{    0,-4096,    0},{    0, -4096,     0}},
			{{    0,    0,-4096},{    0,     0, -4096}},
			{{    0, 4096,    0},{    0,  4096,     0}},
			{{    0,    0, 4096},{    0,     0,  4096}},
			{{ 4096,    0,    0},{ 4096,     0,     0}},
			{{-4096,    0,    0},{-4096,     0,     0}}
			};

/*wall's color*/
static CVECTOR c= {128,128,128,0x20};

/*plane's initial position*/
static SVECTOR y_init[3]= {{0,100,200},{50,200,150},{-50,150,150}}; 

main()
{
	SVECTOR		lgt[3];		/* light source's position */
	SVECTOR		ypl[3];		/* plane's position */

	POLYGON_3D	pol_3d[6][2];

	DB	db[2];		/* packet double buffer */
	DB	*cdb;		/* current db */
	long	dmy, flg;	/* dummy */
	long	otz;
	long	min_otz;
	long	opz[6][2];
	
	int	i,j,k;

	MATRIX	objmat;
	MATRIX  lgtmat;
	MATRIX	rotmat;

	VOLUME		vlm;
	TRIANGLE_3D	gn;

	ResetCallback();
	PadInit(0);             /* initialize PAD */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(1);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(160, 120);	/* set geometry origin as (160, 120) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */

	/* initialize environment for double buffer 
	 *	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,240)-(320,480) (320x240)
	 */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);	
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);
	
	/* init primitives */
	init_prim(&db[0]);
	init_prim(&db[1]);
	
	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	SetColorMatrix(&lc);
	SetBackColor(100,100,100);

	while (pad_read() == 0) {

		PushMatrix();
		RotMatrix(&lgtang,&lgtmat);
		for(i=0;i<3;i++){ 
			ApplyMatrixSV(&lgtmat,&l_init[i],&lgt[i]);
		}
		RotMatrix(&objang,&objmat);
		for(i=0;i<3;i++){ 
			ApplyMatrixSV(&objmat,&y_init[i],&ypl[i]);
		}
		PopMatrix();

		for(i=0;i<6;i++)
		for(j=0;j<2;j++){
			copySVECTOR(&vlm.vert[0],&lgt[0]);
			copySVECTOR(&vlm.vert[1],&ypl[0]);	
			copySVECTOR(&vlm.vert[2],&ypl[1]);	
			copySVECTOR(&vlm.vert[3],&ypl[2]);	
			copySVECTOR(&gn.vert[0],&x[i][j][0]);
			copySVECTOR(&gn.vert[1],&x[i][j][1]);
			copySVECTOR(&gn.vert[2],&x[i][j][2]);

			volume_and_poly(&vlm,&gn,&pol_3d[i][j]);
		}


		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		
		ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */
		
		/*add light source*/
		otz=RotAverage3(
				&lgt[0],&lgt[1],&lgt[2],
				(long *)&cdb->lght.x0,
				(long *)&cdb->lght.x1,
				(long *)&cdb->lght.x2,
				&dmy, &flg);
		otz>>=(14-OTLEN);
		AddPrim(cdb->ot, &cdb->lght);	

		/*add plane*/
		otz=RotAverage3(
				&ypl[0],&ypl[1],&ypl[2],
				(long *)&cdb->plane.x0,
				(long *)&cdb->plane.x1,
				(long *)&cdb->plane.x2,
				&dmy, &flg);
		otz>>=(14-OTLEN);
		AddPrim(cdb->ot, &cdb->plane);	

		/*add wall*/
		min_otz= OTSIZE;
		for(i=0;i<6;i++){
		  for(j=0;j<2;j++){
		    opz[i][j]=RotAverageNclip3(
			&x[i][j][0],&x[i][j][1],&x[i][j][2],
			(long *)&cdb->wall[i][j].x0,
			(long *)&cdb->wall[i][j].x1,
			(long *)&cdb->wall[i][j].x2,
			&dmy, &otz, &flg);
		    if(opz[i][j]<=0) continue;
		    otz>>=(14-OTLEN);
		    if(otz<min_otz) min_otz= otz;
		    NormalColorCol(
				&n[i][j],
				&c,
				(CVECTOR*)&cdb->wall[i][j].r0);

		    if(opz[i][j]>0) AddPrim(cdb->ot+otz, &cdb->wall[i][j]);
		  }
		}

		/*add shadow*/
		for(i=0;i<6;i++){
		  for(j=0;j<2;j++){
		    for(k=0;(k<pol_3d[i][j].npol-2)&&(opz[i][j]>0);k++){
			RotTransPers3(
				&pol_3d[i][j].vert[0],
				&pol_3d[i][j].vert[k+1],
				&pol_3d[i][j].vert[k+2],
				(long *)&cdb->shdw[i][j][k].x0,
				(long *)&cdb->shdw[i][j][k].x1,
				(long *)&cdb->shdw[i][j][k].x2,
				&dmy,&flg);
			AddPrim(cdb->ot+min_otz-1, &cdb->shdw[i][j][k]);
		    }
		  }
		}
		/* swap buffer */
		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */
		/*DumpOTag(cdb->ot);	/* for debug */
		DrawOTag(cdb->ot+OTSIZE-1);	/* draw */
	}
        PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
}

static int pad_read()
{
	static SVECTOR	ang   = {0, 0, 0};	/* rotate angle */
	static VECTOR	vec   = {0,     0, 3*SCR_Z};	/* translate vector */
	static MATRIX	m,tmp;				/* matrix */
	static int	objmove=0;
	
	int	ret = 0;	
/*	u_long	padd = PadRead(1);*/
	u_long	padd;
	
	padd = PadRead(1);

	if (padd & PADRup)	ang.vx += 32;
	if (padd & PADRdown)	ang.vx -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;
	if (padd & PADm) 	ang.vz += 32;
	if (padd & PADo)	ang.vz -= 32;

	if (padd & PADn)	objmove = 1;
	if (padd & PADl)	objmove = 0;


	if (padd & PADk) 	ret = -1;

	if(objmove==1){
		objang.vx +=32;
		objang.vy +=20;
	}else{
		lgtang.vx +=32;
		lgtang.vy +=20;
	}

/*	ang.vx += 32;	*/
	RotMatrix(&ang, &m);	
	TransMatrix(&m, &vec);	

	MulMatrix0(&ll,&m,&tmp);
	SetLightMatrix(&tmp);
	SetRotMatrix(&m);
	SetTransMatrix(&m);

	return(ret);
}		

static void init_prim(DB *db)
{
	int	i,j,k;

	/* set background color */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);	/* (r,g,b) = (60,120,120) */
	
	/* initialize packet */
	for(i=0;i<6;i++)
	for(j=0;j<2;j++)
	for(k=0;k<4;k++){
	    SetPolyF3(&db->shdw[i][j][k]);
	    setRGB0(&db->shdw[i][j][k], 0x0, 0x0, 0x00);
	    SetSemiTrans(&db->shdw[i][j][k],1);
	}
	SetPolyF3(&db->lght);			/* set POLY_G3 primitve ID */
	setRGB0(&db->lght, 0xff, 0xff, 0x00);	/* set colors */

	for(i=0;i<6;i++)
	for(j=0;j<2;j++){
	    SetPolyF3(&db->wall[i][j]);		/* set POLY_G3 primitve ID */
	}

	SetPolyG3(&db->plane);			/* set POLY_G3 primitve ID */
	setRGB0(&db->plane, 0x00, 0xff, 0x00);	/* set colors */
	setRGB1(&db->plane, 0xff, 0x00, 0x00);
	setRGB2(&db->plane, 0x00, 0x00, 0xff);
}



/************************************************
* vlm->vert[0]:light source			*
* vlm->vert[1],vert[2],vert[3]: wall		*
* when vert[1][2][3] is clockwise from inside	*
* 021,032,031 is clockwise from inside		*
************************************************/
volume_and_poly(vlm,tri,pol)
VOLUME		*vlm;
TRIANGLE_3D	*tri;
POLYGON_3D	*pol;
{
	PLANE		bnd;
	POLYGON_3D	p1,p2;
	long	pl[4];			/*plane equation*/
	VECTOR	V01,V02;
	VECTOR	v;
	long	volume;
	int	flg;

	V01.vx= vlm->vert[2].vx - vlm->vert[1].vx;
	V01.vy= vlm->vert[2].vy - vlm->vert[1].vy;
	V01.vz= vlm->vert[2].vz - vlm->vert[1].vz;

	V02.vx= vlm->vert[3].vx - vlm->vert[1].vx;
	V02.vy= vlm->vert[3].vy - vlm->vert[1].vy;
	V02.vz= vlm->vert[3].vz - vlm->vert[1].vz;

	OuterProduct0(&V01,&V02,&v);
	v.vx /= 256;
	v.vy /= 256;
	v.vz /= 256;
	VectorNormal(&v,&v);

	pl[0]= v.vx;
	pl[1]= v.vy;
	pl[2]= v.vz;
	pl[3]= -(pl[0]*vlm->vert[1].vx 
		+ pl[1]*vlm->vert[1].vy + pl[2]*vlm->vert[1].vz);

	volume= pl[0]*vlm->vert[0].vx
		+ pl[1]*vlm->vert[0].vy
		+ pl[2]*vlm->vert[0].vz
		+ pl[3];

	/*if tetra is plane return*/
	if(volume<20000&&volume>-20000){
		pol->npol=0;
		return;
	}

	flg= 1- in_or_out(pl,&vlm->vert[0]);

	copySVECTOR(&bnd.vert[0],&vlm->vert[1]);
	copySVECTOR(&bnd.vert[1],&vlm->vert[3]);
	copySVECTOR(&bnd.vert[2],&vlm->vert[2]);

	copySVECTOR(&p1.vert[0],&tri->vert[0]);
	copySVECTOR(&p1.vert[1],&tri->vert[1]);
	copySVECTOR(&p1.vert[2],&tri->vert[2]);

	p1.npol= 3;

	plane_and_poly(&bnd,&p1,&p2,flg);

	copySVECTOR(&bnd.vert[0],&vlm->vert[0]);
	copySVECTOR(&bnd.vert[1],&vlm->vert[2]);
	copySVECTOR(&bnd.vert[2],&vlm->vert[1]);

	plane_and_poly(&bnd,&p2,&p1,flg);

	copySVECTOR(&bnd.vert[0],&vlm->vert[0]);
	copySVECTOR(&bnd.vert[1],&vlm->vert[3]);
	copySVECTOR(&bnd.vert[2],&vlm->vert[2]);

	plane_and_poly(&bnd,&p1,&p2,flg);

	copySVECTOR(&bnd.vert[0],&vlm->vert[0]);
	copySVECTOR(&bnd.vert[1],&vlm->vert[1]);
	copySVECTOR(&bnd.vert[2],&vlm->vert[3]);

	plane_and_poly(&bnd,&p2,pol,flg);
}


/****************************************************************
* cut polygon 'pol1' by plane 'pln' according to flag 'flg'	*
* flg=0: remain part of polygon in right side of plane		*
* flg=1: remain part of polygon in left side of plane		*
* right/left of plane is determined by right screw's low	*
*	right screw's direction is LEFT				*
****************************************************************/
plane_and_poly(pln,pol1,pol2,flg)
PLANE		*pln;
POLYGON_3D	*pol1,*pol2;
int		flg;
{
	int	i;
	long	pl[4];			/*plane equation*/
	VECTOR	V01,V02;
	VECTOR	v;
	int	pflag;			/*previous in-out flag*/
	int	flag;			/*in-out flag*/
	long	pvolume;
	VECTOR	absv;
	long	max;

	/*make plane's eauation from plane's vertices*/
	V01.vx= pln->vert[1].vx - pln->vert[0].vx;
	V01.vy= pln->vert[1].vy - pln->vert[0].vy;
	V01.vz= pln->vert[1].vz - pln->vert[0].vz;

	V02.vx= pln->vert[2].vx - pln->vert[0].vx;
	V02.vy= pln->vert[2].vy - pln->vert[0].vy;
	V02.vz= pln->vert[2].vz - pln->vert[0].vz;

	/*right screw'slow*/
	if(flg==0) OuterProduct0(&V01,&V02,&v);
	else	   OuterProduct0(&V02,&V01,&v);

	/*if 'v' is too large VectorNormal overflow*/
	if(v.vx<0) absv.vx= -v.vx;
	else	absv.vx= v.vx;
	if(v.vy<0) absv.vy= -v.vy;
	else	absv.vy= v.vy;
	if(v.vz<0) absv.vz= -v.vz;
	else	absv.vz= v.vz;

	if(absv.vx>absv.vy) max= absv.vx;
	else	max= absv.vy;
	if(absv.vz>max) max= absv.vz;

	if(max>=26754){			/*26754=sqrt(1/6)*65536*/
		v.vx /= 256;
		v.vy /= 256;
		v.vz /= 256;
	}
	pvolume=VectorNormal(&v,&v);

	/*if plane is line don't calculate*/
	if(pvolume<=20){
		pol2->npol=0;
		return;
	}

	/*plane's equation*/
	pl[0]= v.vx;
	pl[1]= v.vy;
	pl[2]= v.vz;
	pl[3]= -(pl[0]*pln->vert[0].vx 
		+ pl[1]*pln->vert[0].vy + pl[2]*pln->vert[0].vz);

	pol2->npol=0;

	pflag= in_or_out(pl,&pol1->vert[pol1->npol-1]);

	for(i=0;i<pol1->npol;i++){
		flag= in_or_out(pl,&pol1->vert[i]);
		if(flag==1){
			if(pflag==1){
			   copySVECTOR(&pol2->vert[pol2->npol],&pol1->vert[i]);
			   pol2->npol++;
			}else{
			   plane_and_line(
			     pl,
			     &pol1->vert[((i-1)+pol1->npol)%pol1->npol],
			     &pol1->vert[i],
			     &pol2->vert[pol2->npol]);
			   pol2->npol++;
			   copySVECTOR(&pol2->vert[pol2->npol],&pol1->vert[i]);
			   pol2->npol++;
			}
			pflag= 1;
		}else{
			if(pflag==1){
			   plane_and_line(
			     pl,
			     &pol1->vert[((i-1)+pol1->npol)%pol1->npol],
			     &pol1->vert[i],
			     &pol2->vert[pol2->npol]);
			   pol2->npol++;
			}else{
			}
			pflag= 0;
		}
	}
}

/*****************************************************************************
* when plane's equation is made from vertices according to right screw's low *
* if(return=0) vertex 'v' is same direction as (pln[0],pln[1],pln[2])	     *
* if(return=1) vertex 'v' is anti direction as (pln[0],pln[1],pln[2])	     *
*****************************************************************************/
in_or_out(pln,v)
long	pln[4];
SVECTOR *v;
{
	int	p;

	p= pln[0]*v->vx+pln[1]*v->vy+pln[2]*v->vz+pln[3];
	if(p>=0) return(0);	/*out:0*/
	else	return(1);	/*in:1*/
}

/*************************************
* get intersection of plane and line *
*************************************/
plane_and_line(pln,v0,v1,v)
long	pln[4];
SVECTOR	*v0,*v1;
SVECTOR *v;
{
	int	p[2];	
	int	q[2];
	int	max;

	p[0]= pln[0]*v0->vx+pln[1]*v0->vy+pln[2]*v0->vz+pln[3];
	p[1]= pln[0]*v1->vx+pln[1]*v1->vy+pln[2]*v1->vz+pln[3];

	if((p[0]>0&&p[1]>0)||(p[0]<0&&p[1]<0)){
		printf("Error: intersection check error\n");
		exit();
	}

	/*get absolute*/
	if(p[0]<0) p[0]= -p[0];
	if(p[1]<0) p[1]= -p[1];

	/*get max*/
	if(p[0]>p[1])max=p[0];
	else	max=p[1];

	/*if max is to large divide both by 256*/
	if(max>65536){ 
		p[0] /= 256;
		p[1] /= 256;
	}
	
	/*interpolation weights by (1,3,12) format*/
	q[0]= p[0]*4096/(p[0]+p[1]);
	q[1]= p[1]*4096/(p[0]+p[1]);

	LoadAverageShort12(v0,v1,q[1],q[0],v);
}

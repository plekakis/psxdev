/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *   Sample program showing motion along a polygon
 *
 *   This program shows how to move an object along a curved surface composed
 *   of polygons. The curved surface can be either a contoured surface or a boundary,
 *   but if a boundary is used, special handling such as wrap-around needs to be 
 *   performed. The following operations are performed.
 *
 *    (1) The space is divided into zones by the polygons that make up the curved surface.
 *    (2) Determine which polygon-dependent zone the object is in.
 *    (3) The object is bound to the closest of the polygons containing the object.
 *    (4) The direction of the object is matched to the direction of the polygon normal.
 *    (5) When the object goes beyond the boundary of the polygon, it is rotated according to the corresponding normal.
 *
 *   Among these steps, (1) is performed once at the start of the program, and the
 *   remaining steps are performed for each frame.
 *
 *        Version 0.01   June,  3, 1994
 *
 *        Copyright  (C)  1994  Sony Computer Entertainment
 *        All rights Reserved */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>	


#define TEX_ADDR   	0x80010000
#define PMD_ADDR  	0x80040000
#define PMD_ADDR2   	0x80080000
#define SV_ADDR0	0x80090000
#define SV_ADDR1	0x800a0000
#define SV_ADDR2	0x800b0000
#define SV_ADDR3	0x800c0000
#define OT_LENGTH  	14
#define OTSIZE		(1<<OT_LENGTH)
#define PMDMAX		1000
#define MAX_PP		2000	/*max # of poygons constituting surface*/
#define MAX_VV		2000	/*max # of vertices constituting surface*/
#define MAX_VP		10	/*max # of poygons sharing one vertex*/


GsOT            Wot[2];		/* OT handler */
GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* OT */
u_long          Objnum;		/* #object*/

GsCOORDINATE2   DFace;		/* coordinate of surface */
GsCOORDINATE2   DChar;		/* coordinate of object */
SVECTOR         PWorld; 	/* source data for World coordinate */
SVECTOR		PFace;		/* source data for surface coordinate */
SVECTOR		PChar;		/* source data for object coordinate */

extern MATRIX GsIDMATRIX;	/* unit matrix */
GsRVIEW2  view;			/* viewing structure */
GsF_LIGHT pslt[3];		/* light source structure */
u_long padd;			/* controller data */

GsDOBJ2		face;		/* surface object */



typedef struct {		/*PMD primitive*/
        POLY_G3 p[2];
        SVECTOR v1;
        SVECTOR v2;
        SVECTOR v3;
} polyG3;



u_short tpage;
u_short clut;

polyG3		pb;			/* packet buffer for mark */
LINE_F2		Normal[2][MAX_PP];
LINE_F2		worm[2];
LINE_F2		NWALL[2][MAX_PP][3];
int		WSTART=0;		/*start/stop flag*/
long		NEARLEN;

int		vnormal_disp=0;
int		normal_disp=0;
int		locus_disp=0;
int		pn;		/* PMD file primitive number*/
polyG3		*prim;		/* pointer to polygons constituting surface */
int		saveR[MAX_PP][3];	/* save area for colors of surface */

/************* MAIN START ******************************************/
main()
{
  int     i,j;
  GsDOBJ2 *op;			/* pointer to obejct handler */
  int     outbuf_idx;
  MATRIX  tmpls;
  MATRIX  tmpls1;

  int 		ret;
  long		otz,p,flag;


  VECTOR	GO;		/* gravity center of object */

  VECTOR	nml[MAX_PP];	/*normals of polygons constituting surface */
  VECTOR 	a01,a12,a20;
  VECTOR	a,b,c;
  long		otz0,otz1;

  SVECTOR	w0;		/* object center */
  SVECTOR	w1;
  VECTOR	w2;
  MATRIX	ROT;
  VECTOR	vw0;
  long 		nw0;

  int		PnP;		/*previous OnP*/
  int		OnP;		/*nearest polygon #*/
  MATRIX	Rmat,Tmat;	/*rotation matrix & transpose*/
  VECTOR	Vel,Vel0;
  long		Vabs;
  int		found;
  long		ang;		/*angle between two normals*/
  MATRIX	rot;
  MATRIX	Rrot,RrotT;
  SVECTOR	svel;

  int		Econ[MAX_PP][3];	/*edge connection of each polygon*/
  long		NEARPOL;		/*nearest On polygon distance*/
  VECTOR	nwall[MAX_PP][3];	/*normal of walls of polygon*/
  VECTOR	nml0;
  MATRIX	m;
  int		edge;			/*crossing edge*/

  VECTOR	Vnml[MAX_PP][3];	/* shared normals of vertices */
  VECTOR	Epol[MAX_PP][3];	/* edge of polygon */

  SVECTOR	n0,n1;
  int		iter=0;
  VECTOR	Enml,Enml0;
  long		amax;
  VECTOR	Ege;
  
  SVECTOR	sv;
  VECTOR	lv;
  VECTOR	Rhand;			/* right hand vector */
  
  int		nofound=0;
  int		reverse=0;

  int walkcount;
  u_long *walkp;


  ResetCallback();

  init_all();
  GsInitVcount();


  prim= (polyG3 *)(PMD_ADDR+4);
  pn= *(short *)(PMD_ADDR);


  /* save colors of polygons constituting surface */
  for(i=0;i<pn;i++){
	saveR[i][0]= prim[i].p[0].r0;
	saveR[i][1]= prim[i].p[0].r1;
	saveR[i][2]= prim[i].p[0].r2;
  }

  /* get normals of polygons constituting surface */
  /* prim,pn -> nml,Epol			  */
  PolyNormal(prim,nml,Epol,pn);

  /* get shared normals at vertices */
  /* prim,pn,nml -> Vnml 	    */
  VertexNormal(prim,nml,Vnml,pn);

  /* get normals of 'WALL' between polygons */
  /* Vnml,Epol,pn -> nwall		    */
  WallNormal(Vnml,Epol,nwall,pn);

  
  /* initialize moving object's position & velocity 	*/
  /* object starts from center of polygon #0 	 	*/
  /*	speed=1.0, direction=edge #0 of polygon #0 	*/ 
  w0.vx= (prim[0].v1.vx + prim[0].v2.vx + prim[0].v2.vx)/3;
  w0.vy= (prim[0].v1.vy + prim[0].v2.vy + prim[0].v2.vy)/3;
  w0.vz= (prim[0].v1.vz + prim[0].v2.vz + prim[0].v2.vz)/3;
  OuterProduct12(&Epol[0][0],&nml[0],&Vel0);
  VectorNormal(&Vel0,&Vel);

  /*initial coordinate of moving object */
  DChar.coord.t[0] = w0.vx + nml[0].vx/100;
  DChar.coord.t[1] = w0.vy + nml[0].vy/100;
  DChar.coord.t[2] = w0.vz + nml[0].vz/100;

  ret=0;
  Vabs= 5;
  PnP= 0;
  OnP= 0;
  walkcount=0;

  while(ret==0)
    {
      	ret=obj_interactive();		/* read controller */
      	outbuf_idx=GsGetActiveBuff();	/* get double buffer ID */

      	GsClearOt(0,0,&Wot[outbuf_idx]); 	/* clear OT */


	/* find polygon object belongs to */
	NEARPOL=100000000;
	for(i=0;i<pn;i++){
		/*test w0 if belongs to polygon prim[i]*/
		found= OnTriangle(&w0,&prim[i].v1,nwall[i],&edge);
		if(found>0&&found<NEARPOL){ 
			OnP=i;
			NEARPOL=found;
		}
	}

	/* if found constraint object on polygon #OnP */
	if(NEARPOL<100000000){
		ConstraintOnPolygon(&w0,&prim[OnP].v1,&nml[OnP]);
		nofound=0;
	}else{
		nofound++;
	}

	/* if 10 times no found then wrap round */
	if(nofound>=10){
		if(w0.vx<0)w0.vx+=800;
		if(w0.vx>800)w0.vx-=800;
		if(w0.vz<0)w0.vz+=800;
		if(w0.vz>800)w0.vz-=800;
		nofound=0;
	}

      	/*if flag=ON move object&object coordinate*/
	/*direction=Vel,speed=Vabs*/
	if(WSTART==1){
     		w0.vx += ((Vel.vx*Vabs)>>12);
      		w0.vy += ((Vel.vy*Vabs)>>12);
      		w0.vz += ((Vel.vz*Vabs)>>12);
		DChar.coord.t[0]= w0.vx + nml[OnP].vx/100;
		DChar.coord.t[1]= w0.vy + nml[OnP].vy/100;
		DChar.coord.t[2]= w0.vz + nml[OnP].vz/100;
	}

	/*if object cross polygon's edge change velocity*/
	if(OnP!=PnP){
		/*rotate Vel to Vel0*/
		RotVectorByUnit(&nml[PnP],&nml[OnP],&Vel,&Vel0,&m);

		/*constraint Vel0 on polygon #OnP*/
      		nw0= (nml[OnP].vx*Vel0.vx + 
			nml[OnP].vy*Vel0.vy + nml[OnP].vz*Vel0.vz)>>12;

		Vel0.vx -= (nw0*nml[OnP].vx)>>12;
		Vel0.vy -= (nw0*nml[OnP].vy)>>12;
		Vel0.vz -= (nw0*nml[OnP].vz)>>12;

		/*normalize*/
		VectorNormal(&Vel0,&Vel);

		/*new object coordinate*/
		DChar.coord.m[0][2]= -Vel.vx;
		DChar.coord.m[1][2]= -Vel.vy;
		DChar.coord.m[2][2]= -Vel.vz;
		DChar.coord.m[0][1]= -nml[OnP].vx;
		DChar.coord.m[1][1]= -nml[OnP].vy;
		DChar.coord.m[2][1]= -nml[OnP].vz;

		OuterProduct12(&nml[OnP],&Vel,&Rhand);

		DChar.coord.m[0][0]= Rhand.vx;
		DChar.coord.m[1][0]= Rhand.vy;
		DChar.coord.m[2][0]= Rhand.vz;
	}

	GsSetRefView2(&view);		/*set World/Screen Matrix*/
	GsGetLs(face.coord2,&tmpls);	/*get Local/Screen Matrix*/
	GsSetLsMatrix(&tmpls);		/*set LS Matrix for surface to GTE*/

      	/*coordinate transformation & make OT*/
      	RotPMD_G3(
		(long*)PMD_ADDR,
		(u_long*)&zsorttable[outbuf_idx][0],
		OT_LENGTH,
		outbuf_idx,0);


	/*change color of locus*/
	if(locus_disp==1){
		prim[OnP].p[outbuf_idx].r0=255;
		prim[OnP].p[outbuf_idx].r1=255;
		prim[OnP].p[outbuf_idx].r2=255;
		prim[OnP].p[1-outbuf_idx].r0=255;
		prim[OnP].p[1-outbuf_idx].r1=255;
		prim[OnP].p[1-outbuf_idx].r2=255;
	}

      	/* load character matrix*/
      	CompMatrix(&tmpls,&DChar.coord,&tmpls1);
	if(normal_disp==0){
      		GsSetLsMatrix(&tmpls1);	/*set LS Matrix for object to GTE*/
	}

	/*select pose*/
	switch(walkcount/2) {
		case 0:	walkp=(u_long*)SV_ADDR0;break;
		case 5:
		case 1:	walkp=(u_long*)SV_ADDR1;break;
		case 4:
		case 2:	walkp=(u_long*)SV_ADDR2;break;
		case 3:	walkp=(u_long*)SV_ADDR3;break;
	}
	walkcount=(walkcount+1)%12;

	/*coordinate transformation & make OT*/
	if(normal_disp==0){
	    /*draw boy*/
      	    RotPMD_SV_G3(
		(long*)PMD_ADDR2,
		(long*)walkp,
		(u_long*)&zsorttable[outbuf_idx][0],
		OT_LENGTH,
		outbuf_idx,0);
	}else{
	    /*draw normal vector*/
     	    w1.vx= w0.vx + nml[OnP].vx/10;
      	    w1.vy= w0.vy + nml[OnP].vy/10;
      	    w1.vz= w0.vz + nml[OnP].vz/10;
	    otz0=RotTransPers( &w0,(long*)&worm[outbuf_idx].x0,&p,&flag);
      	    otz1=RotTransPers( &w1,(long*)&worm[outbuf_idx].x1,&p,&flag);
      	    otz= (otz0+otz1)/2;
      	    otz >>= (14-OT_LENGTH);
      	    AddPrim(&zsorttable[outbuf_idx][0]+otz,&worm[outbuf_idx]);
	}

	/*draw Vertex Normals*/
	if(vnormal_disp==1){
      	    GsSetLsMatrix(&tmpls);	/*set LS Matrix for surface to GTE*/
	    for(i=0;i<pn;i++){
		n0.vx= prim[i].v1.vx;
		n0.vy= prim[i].v1.vy;
		n0.vz= prim[i].v1.vz;
		n1.vx= n0.vx + Vnml[i][0].vx/100;
		n1.vy= n0.vy + Vnml[i][0].vy/100;
		n1.vz= n0.vz + Vnml[i][0].vz/100;
		otz0=RotTransPers(&n0,(long*)&Normal[outbuf_idx][i].x0,&p,&flag);
		otz1=RotTransPers(&n1,(long*)&Normal[outbuf_idx][i].x1,&p,&flag);
      		otz= (otz0+otz1)/2;
   		otz >>= (14-OT_LENGTH);
   		AddPrim(&zsorttable[outbuf_idx][0]+otz,&Normal[outbuf_idx][i]);
	    }
	}

	/*draw Wall Normals*/
/*
	for(i=0;i<pn;i++){
		n0.vx= (prim[i].v1.vx+prim[i].v2.vx)/2;
		n0.vy= (prim[i].v1.vy+prim[i].v2.vy)/2;
		n0.vz= (prim[i].v1.vz+prim[i].v2.vz)/2;
		n1.vx= n0.vx + nwall[i][0].vx/10;
		n1.vy= n0.vy + nwall[i][0].vy/10;
		n1.vz= n0.vz + nwall[i][0].vz/10;
		otz0=RotTransPers( &n0,&NWALL[outbuf_idx][i][0].x0,&p,&flag);
		otz1=RotTransPers( &n1,&NWALL[outbuf_idx][i][0].x1,&p,&flag);
	      	otz= (otz0+otz1)/2;
      		otz >>= (14-OT_LENGTH);
      		AddPrim(&zsorttable[outbuf_idx][0]+otz,&NWALL[outbuf_idx][i][0]);
	}
*/
	/*copy OnP to PnP*/
	PnP= OnP;

      	padd=PadRead(0);		/*read pad*/
      	VSync(0);			/*wait controller*/

      	ResetGraph(1);			/*GPU reset*/
      	GsSwapDispBuff();		/*swap double buffer*/

      	/*add display clear to the head of OT*/
      	GsSortClear(0x0,0x0,0x0,&Wot[outbuf_idx]);
      	/*start drawing*/
      	GsDrawOt(&Wot[outbuf_idx]);
    }
    PadStop();
    ResetGraph(3);
    StopCallback();

    return 0;
}


obj_interactive()
{
  int 	i;
  SVECTOR v1;
  MATRIX  tmp1;

  if((padd & PADRleft)>0) PFace.vy -=5*ONE/360;	/*rotate surface around Y-axis*/
  if((padd & PADRright)>0) PFace.vy +=5*ONE/360;/*rotate surface around Y-axis*/
  if((padd & PADRup)>0) PFace.vx+=5*ONE/360;	/*rotate surface around X-axis*/
  if((padd & PADRdown)>0) PFace.vx-=5*ONE/360;	/*rotate surface around X-axis*/

  if((padd & PADm)>0) DFace.coord.t[2]+=100;	/*move surface along Z-axis*/
  if((padd & PADo)>0) DFace.coord.t[2]-=100;	/*move surface along Z-axis*/

  if((padd & PADLup)>0) normal_disp=1;		/*display moving normal*/
  if((padd & PADLdown)>0) normal_disp=0;	/*display moving boy*/

  if((padd & PADLleft)>0) locus_disp=1;		/*remain locus*/	
  if((padd & PADLright)>0){
	locus_disp=0;				/*elase locus*/	
	for(i=0;i<pn;i++){
		prim[i].p[0].r0= saveR[i][0];
		prim[i].p[0].r1= saveR[i][1];
		prim[i].p[0].r2= saveR[i][2];
		prim[i].p[1].r0= saveR[i][0];
		prim[i].p[1].r1= saveR[i][1];
		prim[i].p[1].r2= saveR[i][2];
	}
  }

  if(((padd & PADh)<=0)&&((padd & PADl)>0)) WSTART=1;	/* walk start */
  if(((padd & PADh)<=0)&&((padd & PADn)>0)) WSTART=0;	/* walk stop */

  if(((padd & PADh)>0)&&((padd & PADl)>0)) vnormal_disp=1;	
						/*display vertex normals*/
  if(((padd & PADh)>0)&&((padd & PADn)>0)) vnormal_disp=0;
						/*elase vertex normals*/

  if((padd & PADk)>0) return(-1); 		/*end*/
  
  set_coordinate(&PFace,&DFace);		/*get rotation matrix*/
  return(0);
}


init_all()			/*initialize*/
{
  int i;

  ResetGraph(0);		/*GPU reset*/
  PadInit(0);			/*controller reset*/
  padd=0;			/*controller data reset*/
  
/*  GsInitGraph(640,480,2,0,0);	/*set resolution(interlace mode)*/
/*  GsDefDispBuff(0,0,0,0);	/*double buffer*/

  GsInitGraph(640,240,0,0,0);	/*set resolution(noninterlace mode)*/
  GsDefDispBuff(0,0,0,240);	/*double buffer*/

  GsInit3D();			/*GTE reset*/
  
  Wot[0].length=OT_LENGTH;	/*set OT length*/
  Wot[0].org=zsorttable[0];	/*set OT to OT handler*/

  Wot[1].length=OT_LENGTH;	/*set OT length*/
  Wot[1].org=zsorttable[1];	/*set OT to OT handler*/
  
  coord_init();			/*coordinate initialize*/
  view_init();			/*view initialize*/
  light_init();			/*light source initialize*/
  model_init();			/*model initialize*/

  /*vertex normal & wall normal initilaize*/
  for(i=0;i<MAX_PP; i++){
	SetLineF2(&Normal[0][i]);
	SetLineF2(&Normal[1][i]);
	SetLineF2(&NWALL[0][i][0]);
	SetLineF2(&NWALL[0][i][1]);
	SetLineF2(&NWALL[0][i][2]);
	SetLineF2(&NWALL[1][i][0]);
	SetLineF2(&NWALL[1][i][1]);
	SetLineF2(&NWALL[1][i][2]);
	Normal[0][i].r0= 0; Normal[0][i].g0= 255; Normal[0][i].b0= 0;
	Normal[1][i].r0= 0; Normal[1][i].g0= 255; Normal[1][i].b0= 0;
	NWALL[0][i][0].r0= 0; NWALL[0][i][0].g0= 255; NWALL[0][i][0].b0= 0;
	NWALL[0][i][1].r0= 0; NWALL[0][i][1].g0= 255; NWALL[0][i][1].b0= 0;
	NWALL[0][i][2].r0= 0; NWALL[0][i][2].g0= 255; NWALL[0][i][2].b0= 0;
	NWALL[1][i][0].r0= 0; NWALL[1][i][0].g0= 255; NWALL[1][i][0].b0= 0;
	NWALL[1][i][1].r0= 0; NWALL[1][i][1].g0= 255; NWALL[1][i][1].b0= 0;
	NWALL[1][i][2].r0= 0; NWALL[1][i][2].g0= 255; NWALL[1][i][2].b0= 0;
  }

  /*moving normal initialize*/
  SetLineF2(&worm[0]);
  SetLineF2(&worm[1]);
  worm[0].r0= 255; worm[0].g0= 0; worm[0].b0= 255;
  worm[1].r0= 255; worm[1].g0= 0; worm[1].b0= 255;
}


view_init()			/*view initialize*/
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/*set projection*/
  
  /*set eye position*/
  view.vpx = 0; view.vpy = 0; view.vpz = -2000;
  /*set watching position*/
  view.vrx = 0; view.vry = 0; view.vrz = 0;
  /*set view twist*/
  view.rz=0;
  /*set view coordinate*/
  view.super = WORLD;
  
  /*get World/Screen Matrix from view parameters*/
  GsSetRefView2(&view);

  /*set near clip position*/
  GsSetNearClip(100);
}


light_init()			/*light source initialize*/
{
  /*set light source #0*/
  /*set direction*/
  pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
  /*set color*/
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  /*set light source*/
  GsSetFlatLight(0,&pslt[0]);

  /*set light source #1*/
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /*set light source #2*/
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
  pslt[2].r=0x60; pslt[2].g=0x60; pslt[2].b=0x60;
  GsSetFlatLight(2,&pslt[2]);
  
  /*set ambient*/
  GsSetAmbient(0,0,0);

  /*set default lighting mode*/
  GsSetLightMode(0);
}

coord_init()			/*coordinate initialize*/
{
  /*set hierarchy of coordinates */
  /*surface coord. directly subject to Wolrd coord.*/
  GsInitCoordinate2(WORLD,&DFace);
  GsInitCoordinate2(&DFace,&DChar);
  
  /*rotation vector initialize*/
  PWorld.vx=1024;
  PWorld.vy=PWorld.vz=0;
  PFace = PWorld;
  PChar = PFace;

  /*set origin of surface (0,0,4000) of World */
  DFace.coord.t[2] = 4000;
}

set_coordinate(pos,coor)	/*get coordinate Matrix from rotation vector*/
SVECTOR *pos;			/*rotation vector*/
GsCOORDINATE2 *coor;		/*coordinate*/
{
  MATRIX tmp1;
  SVECTOR v1;
  
  tmp1   = GsIDMATRIX;		/*start form unit Matrix*/

  tmp1.t[0] = coor->coord.t[0];	/*set transposition*/
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];

  v1 = *pos;

  RotMatrix(&v1,&tmp1);		/*get rotation Matrix from rotation vector*/

  coor->coord = tmp1;		/*get coordinate*/
  coor->flg = 0;		/*cache flush for speed up*/
}

model_init()
{                               /*read modeling data*/
  face.coord2 = &DFace;
}



Veql(v1,v2)			/*vector comparator*/	
SVECTOR *v1,*v2;
{
	if(v1->vx==v2->vx&&v1->vy==v2->vy&&v1->vz==v2->vz)
		return(1);
	else
		return(0);
}

/* get edges and normals from polygon vertices */
PolyNormal(prim,nml,Epol,pn)
polyG3	*prim;			/* PMD data */
VECTOR	nml[MAX_PP];		/* normals */
VECTOR	Epol[MAX_PP][3];	/* Edge */
int	pn;			/* #polygon */
{
	int	i;
	VECTOR	c;
	long	a,b;

  	for(i=0;i<pn;i++){
		Epol[i][0].vx= prim[i].v2.vx - prim[i].v1.vx;
		Epol[i][0].vy= prim[i].v2.vy - prim[i].v1.vy;
		Epol[i][0].vz= prim[i].v2.vz - prim[i].v1.vz;
		Epol[i][1].vx= prim[i].v3.vx - prim[i].v2.vx;
		Epol[i][1].vy= prim[i].v3.vy - prim[i].v2.vy;
		Epol[i][1].vz= prim[i].v3.vz - prim[i].v2.vz;
		Epol[i][2].vx= prim[i].v1.vx - prim[i].v3.vx;
		Epol[i][2].vy= prim[i].v1.vy - prim[i].v3.vy;
		Epol[i][2].vz= prim[i].v1.vz - prim[i].v3.vz;

		OuterProduct0(&Epol[i][1],&Epol[i][0],&c);

		/*pre-normlization for VectorNormal*/
		a= absmax3(c.vx,c.vy,c.vz);
		if(a<0) a= -a;
		b= SquareRoot0(a);
		c.vx /= b;
		c.vy /= b;
		c.vz /= b;

		VectorNormal(&c,&nml[i]);
  	}
}


/* get shared normals from polygon vertices */
VertexNormal(prim,nml,vnml,pn)
polyG3	*prim;			/* PMD data */
VECTOR	nml[MAX_PP];		/* normals */
VECTOR	vnml[MAX_PP][3];	/* shared normals at vertices*/
int	pn;			/* #polygon */
{
	int 	i,j;
	int	pc;		/* #polygons sharing one vertex */
	VECTOR	nbuf[MAX_VP];	/* normals of polygons sharing one vertex */
	int	k;

	for(i=0;i<pn;i++){
		pc=0;
		for(j=0;j<pn;j++){
			if(Veql(&prim[i].v1,&prim[j].v1)==1||
			   Veql(&prim[i].v1,&prim[j].v2)==1||
			   Veql(&prim[i].v1,&prim[j].v3)==1){
				nbuf[pc].vx= nml[j].vx;
				nbuf[pc].vy= nml[j].vy;
				nbuf[pc].vz= nml[j].vz;
				pc++;
			}
		}
		AverageNormal(nbuf,pc,&vnml[i][0]);

		pc=0;
		for(j=0;j<pn;j++){
			if(Veql(&prim[i].v2,&prim[j].v1)==1||
			   Veql(&prim[i].v2,&prim[j].v2)==1||
			   Veql(&prim[i].v2,&prim[j].v3)==1){
				nbuf[pc].vx= nml[j].vx;
				nbuf[pc].vy= nml[j].vy;
				nbuf[pc].vz= nml[j].vz;
				pc++;
			}
		}
		AverageNormal(nbuf,pc,&vnml[i][1]);

		pc=0;
		for(j=0;j<pn;j++){
			if(Veql(&prim[i].v3,&prim[j].v1)==1||
			   Veql(&prim[i].v3,&prim[j].v2)==1||
			   Veql(&prim[i].v3,&prim[j].v3)==1){
				nbuf[pc].vx= nml[j].vx;
				nbuf[pc].vy= nml[j].vy;
				nbuf[pc].vz= nml[j].vz;
				pc++;
			}
		}
		AverageNormal(nbuf,pc,&vnml[i][2]);
	}
}


AverageNormal(nbuf,pc,vnml)	/* get average of normals */
VECTOR 	*nbuf;
int	pc;
VECTOR	*vnml;
{
	int i;
	VECTOR	v0;

	vnml->vx=0;
	vnml->vy=0;
	vnml->vz=0;

	for(i=0;i<pc;i++){

		vnml->vx += nbuf[i].vx;
		vnml->vy += nbuf[i].vy;
		vnml->vz += nbuf[i].vz;
	}
	v0.vx= vnml->vx/pc;
	v0.vy= vnml->vy/pc;
	v0.vz= vnml->vz/pc;

	VectorNormal(&v0,vnml);
}


WallNormal(Vnml,Epol,nwall,pn)		/*get normals of polygon's wall*/
VECTOR	Vnml[MAX_PP][3];		/*shared normals of vertices*/
VECTOR	Epol[MAX_PP][3];		/*3 edges of polygon*/
VECTOR	nwall[MAX_PP][3];		/*3 normals of polygon wall*/
int	pn;				/*#polygon*/
{
	int	i;
	VECTOR	Enml,Enml0;		/*average fo two shared normals*/
	int	amax;
	VECTOR	Ege;			/*same directed edge*/
	VECTOR	nml0;

  	for(i=0;i<pn;i++){
		/*average of two shared normals at the end of edge #01*/
		Enml0.vx= (Vnml[i][0].vx + Vnml[i][1].vx)/2;
		Enml0.vy= (Vnml[i][0].vy + Vnml[i][1].vy)/2;
		Enml0.vz= (Vnml[i][0].vz + Vnml[i][1].vz)/2;
		VectorNormal(&Enml0,&Enml);

		/*small error between Epol & -Epol causes small error
		  between nwall & -nwall. this means there is small gap
		  of area division*/ 
		amax= absmax3(Epol[i][0].vx,Epol[i][0].vy,Epol[i][0].vz);
		if(amax<0){
			Ege.vx= -Epol[i][0].vx;
			Ege.vy= -Epol[i][0].vy;
			Ege.vz= -Epol[i][0].vz;
		}else{
			Ege.vx= Epol[i][0].vx;
			Ege.vy= Epol[i][0].vy;
			Ege.vz= Epol[i][0].vz;
		}
		/*get normal of polygon wall #01*/
		OuterProduct12(&Ege,&Enml,&nml0);
		VectorNormal(&nml0,&nwall[i][0]);
		if(amax<0){
			nwall[i][0].vx= -nwall[i][0].vx;
			nwall[i][0].vy= -nwall[i][0].vy;
			nwall[i][0].vz= -nwall[i][0].vz;
		}

		/*average of two shared normals at the end of edge #12*/
		Enml0.vx= (Vnml[i][1].vx + Vnml[i][2].vx)/2;
		Enml0.vy= (Vnml[i][1].vy + Vnml[i][2].vy)/2;
		Enml0.vz= (Vnml[i][1].vz + Vnml[i][2].vz)/2;
		VectorNormal(&Enml0,&Enml);
		amax= absmax3(Epol[i][1].vx,Epol[i][1].vy,Epol[i][1].vz);
		if(amax<0){
			Ege.vx= -Epol[i][1].vx;
			Ege.vy= -Epol[i][1].vy;
			Ege.vz= -Epol[i][1].vz;
		}else{
			Ege.vx= Epol[i][1].vx;
			Ege.vy= Epol[i][1].vy;
			Ege.vz= Epol[i][1].vz;
		}
		/*get normal of polygon wall #12*/
		OuterProduct12(&Ege,&Enml,&nml0);
		VectorNormal(&nml0,&nwall[i][1]);
		if(amax<0){
			nwall[i][1].vx= -nwall[i][1].vx;
			nwall[i][1].vy= -nwall[i][1].vy;
			nwall[i][1].vz= -nwall[i][1].vz;
		}

		/*average of two shared normals at the end of edge #20*/
		Enml0.vx= (Vnml[i][2].vx + Vnml[i][0].vx)/2;
		Enml0.vy= (Vnml[i][2].vy + Vnml[i][0].vy)/2;
		Enml0.vz= (Vnml[i][2].vz + Vnml[i][0].vz)/2;
		VectorNormal(&Enml0,&Enml);
		amax= absmax3(Epol[i][2].vx,Epol[i][2].vy,Epol[i][2].vz);
		if(amax<0){
			Ege.vx= -Epol[i][2].vx;
			Ege.vy= -Epol[i][2].vy;
			Ege.vz= -Epol[i][2].vz;
		}else{
			Ege.vx= Epol[i][2].vx;
			Ege.vy= Epol[i][2].vy;
			Ege.vz= Epol[i][2].vz;
		}
		/*get normal of polygon wall #20*/
		OuterProduct12(&Ege,&Enml,&nml0);
		VectorNormal(&nml0,&nwall[i][2]);
		if(amax<0){
			nwall[i][2].vx= -nwall[i][2].vx;
			nwall[i][2].vy= -nwall[i][2].vy;
			nwall[i][2].vz= -nwall[i][2].vz;
		}
	}
}

absmax3(a,b,c)			/*return max of 3 values*/
long a,b,c;
{
	long	p,q,r;

	p=a; q=b; r=c;

	if(p<0) p= -p;
	if(q<0) q= -q;
	if(r<0) r= -r;

	if((p>=q)&&(p>=r)) return(a);
	if((q>=p)&&(q>=r)) return(b);
	if((r>=p)&&(r>=q)) return(c);
}

/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *
 *        Phong shading sample program
 *
 *   This sample program performs Phong shading on the PlayStation.
 *   Since the PlayStation is not equipped with Phong shading hardware,
 *   calculations must be performed in software. Thus, compared to Gouraud 
 *   shading, calculations take at least ten times as long. Phong shading of polygons 
 *   should only performed where necessary or most effective. Also, similar effects can 
 *   be achieved faster by increasing the number of polygons and using Gouraud 
 *   shading. In this sample program, Phong shading is performed only at the corner 
 *   surfaces of two octagonal pillars. At the beginning, the left side is Phong-shaded, 
 *   and the right side is Gouraud. Shading switches to Gouraud if the corner area is 
 *   small. The results of Phong shading are first rendered in the texture area, and 
 *   then rendered again in the display area as a textured polygon.
 *
 *        Copyright  (C)  1995 by Sony Computer Entertainment
 *             All rights Reserved
 * */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>


#define SCR_Z	(1024)			/* screen depth (h) */
#define OTLEN	10
#define	OTSIZE	(1<<OTLEN)		/* ordering table size */
#define CUBESIZ	200			/* cube size */


#define PIH             320
#define PIV             240

#define OFX             (PIH/2)                 /* screen offset X */
#define OFY             (PIV/2)                 /* screen offset Y */
#define BGR             60                      /* BG color R */
#define BGG             120                     /* BG color G */
#define BGB             120                     /* BG color B */
#define RBK             0                       /* Back color R */
#define GBK             0                       /* Back color G */
#define BBK             0                       /* Back color B */
#define RFC             BGR                     /* Far color R */
#define GFC             BGG                     /* Far color G */
#define BFC             BGB                     /* Far color B */
#define FOGNEAR         1000                    /* Fog near point */
#define DPQ             0                       /*1:ON,0:OFF*/



typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	/*right object*/
	POLY_F3		top[6];			/* Flat */
	POLY_F3		bot[6];			/* Flat */
	POLY_F3		sid[8];			/* Flat */
	POLY_G3		ege[8];			/* Gouraud */

	/*left object*/
	POLY_F3		topP[6];		/* Flat */
	POLY_F3		botP[6];		/* Flat */
	POLY_F3		sidP[8];		/* Flat */
	POLY_FT3	egeP[8];		/* Flat texture(Phong) */
	POLY_G3		egeG[8];		/* Gouraud */

	/*right object otz save area*/
	int		otz_top[6];		/* Flat */
	int		otz_bot[6];		/* Flat */
	int		otz_sid[8];		/* Flat */
	int		otz_ege[8];		/* Gouraud */

	/*left object otz save area*/
	int		otz_topP[6];		/* Flat */
	int		otz_botP[6];		/* Flat */
	int		otz_sidP[8];		/* Flat */
	int		otz_egeP[8];		/* Flat texture(Phong) */
	int		otz_egeG[8];		/* Gouraud */
} DB;



u_short tpage;

int	PG= 100;		/*Phong/Gouraud border line opz*/
int	TBX= 8;			/* Texture Base address X*/

MATRIX	WLmat;			/*2 objects*/
MATRIX	WL1mat;			/*Phong object*/
MATRIX	WL2mat;			/*Gouraud object*/

MATRIX	lmat;			/*light vector matrix*/
MATRIX	cmat;			/*light color matrix*/
CVECTOR	object_col;		/*object color*/

static pad_read();
static init_prim();


main()
{
	DB		db[2];		/* packet double buffer */
	DB		*cdb;		/* current db */
        int             i,j,k;
        long            ret;

	SVECTOR		QCOL[8][2];	/*3D coordinates*/
	int		center[3];
	int		hight;
	int		width;
	int		depth;
	int		cut;		/*corner cut*/
	short		SV_top[8][2];	/*screen coordinate of top*/
	short		SV_bot[8][2];	/*screen coordinate of bottom*/
	int		OTZ_top[8];	/*OTZ of top vertex*/
	int		OTZ_bot[8];	/*OTZ of bottom vertex*/
	SVECTOR		NML_TB[2];	/*normal vector of top&bottom face*/
	SVECTOR		NML_SD[4];	/*normal vector of side face*/
	CVECTOR		COL_TB[2];	/*color of top/bottom face*/
	CVECTOR		COL_SD[4];	/*color of side face*/

	ResetCallback();
	PadInit(0);		/* reset graphic environment */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(OFX, OFY);	/* set geometry origin as (160, 120) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */
        SetBackColor(RBK,GBK,BBK);      /* set background(ambient) color*/
        SetFarColor(RFC,GFC,BFC);       /* set far color */
        SetFogNear(FOGNEAR,SCR_Z);      /* set fog parameter*/

	/*interlace mode*/	
	SetDefDrawEnv(&db[0].draw, 0,   0, PIH, PIV);	
	SetDefDrawEnv(&db[1].draw, 0,   PIV, PIH, PIV);	
	SetDefDispEnv(&db[0].disp, 0,   PIV, PIH, PIV);	
	SetDefDispEnv(&db[1].disp, 0,   0, PIH, PIV);


	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	init_prim(&db[0]);
	init_prim(&db[1]);


	center[0]= 0;
	center[1]= 0;
	center[2]= 0;
	hight= 500;
	width= 500;
	depth= 500;
	cut= 50;
	object_col.r= 255;
	object_col.g= 255;
	object_col.b= 0;

	/*set lighting constants*/
	set_light_color(&lmat,&cmat);
	SetColorMatrix(&cmat);

	/*set vertex coordinates&normals*/
	set_column_vertex(QCOL,center,hight,width,depth,cut);
	set_column_normal_4096(NML_TB,NML_SD);


	ret=0;
	while(pad_read()==0){
	   	cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */

	   	ClearOTagR(cdb->ot, OTSIZE);
	   	DrawSync(0);		/* wait for end of BG drawing */

	    	/*LEFT(phong)*/
		  /*set LEFT object coordinate*/
		  SetRotMatrix(&WL1mat);
		  SetTransMatrix(&WL1mat);

		  /*rotate&transfer&perspective of LEFT object*/
		  rot_column(QCOL,SV_top,SV_bot,OTZ_top,OTZ_bot);

		  /*lighting of LEFT object*/
		  col_column(NML_TB,NML_SD,COL_TB,COL_SD);

		  /*make GPU packet for LEFT object*/
		  set_primP(SV_top,SV_bot,COL_TB,COL_SD,cdb,OTZ_top,OTZ_bot);

		  /*add prim for LEFT object*/
		  add_primP(cdb);

		  /*phong shade & draw in texture area for LEFT object*/
		  phong_column_edge(SV_top,SV_bot,NML_TB,NML_SD);

	    	/*RIGHT(Gouraud)*/
		  /*set RIGHT object coordinate*/
		  SetRotMatrix(&WL2mat);
		  SetTransMatrix(&WL2mat);

		  /*rotate&transfer&perspective of RIGHT object*/
		  rot_column(QCOL,SV_top,SV_bot,OTZ_top,OTZ_bot);

		  /*lighting of RIGHT object*/
		  col_column(NML_TB,NML_SD,COL_TB,COL_SD);

		  /*make GPU packet for LEFT object*/
		  set_prim(SV_top,SV_bot,COL_TB,COL_SD,cdb,OTZ_top,OTZ_bot);

		  /*add prim for RIGHT object*/
		  add_prim(cdb);

	   	DrawSync(0);		/* wait for end of BG drawing */

	   	VSync(0);	/* wait for the next V-BLNK */
	
	   	PutDrawEnv(&cdb->draw); /* update drawing environment */
	   	PutDispEnv(&cdb->disp); /* update display environment */

	   	DrawOTag(cdb->ot+OTSIZE-1);	/* draw BG*/
	}
	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
}


static pad_read()
{
	static SVECTOR	ang  = { 0, 0, 0};	/* rotate angle */
	static SVECTOR	angL  = { 0, 0, 0};	/* rotate angle */
	static VECTOR	vec  = {0, 0, 10000};	/* rottranslate vector */
	static VECTOR	vec1  = {-500, 0, 0};/* rottranslate vector */
	static VECTOR	vec2  = {500, 0, 0};/* rottranslate vector */
	static MATRIX	LL1mat;
	static MATRIX	LL2mat;

	int	ret = 0;	

/*	u_long	padd = PadRead(0);*/
	u_long	padd;

	padd = PadRead(0);

	/* rotate L1,L2 */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* rotate L */
	if (padd & PADLup)	angL.vz += 32;
	if (padd & PADLdown)	angL.vz -= 32;
	if (padd & PADLleft)	angL.vy += 32;
	if (padd & PADLright)	angL.vy -= 32;

	if (padd & PADk) 	ret = -1;
/*
	if(padd & PADl) PG += 10;
	if(padd & PADn) PG -= 10;
*/

	/* set matrix */
	RotMatrix(&ang, &LL1mat);
	RotMatrix(&ang, &LL2mat);
	RotMatrix(&angL, &WLmat);
	
	TransMatrix(&WLmat, &vec);	
	TransMatrix(&LL1mat, &vec1);	
	TransMatrix(&LL2mat, &vec2);	

	CompMatrix(&WLmat,&LL1mat,&WL1mat);
	CompMatrix(&WLmat,&LL2mat,&WL2mat);

	return(ret);
}		

/*
 *	initialize primitive parameters
 */
static init_prim(db)
DB	*db;
{
	long	i,j;

	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);	/* (r,g,b) = (60,120,120) */

	for(i=0;i<6;i++){
		SetPolyF3(&db->top[i]);
		SetPolyF3(&db->bot[i]);
		SetPolyF3(&db->topP[i]);
		SetPolyF3(&db->botP[i]);
	}
	for(i=0;i<8;i++){
		SetPolyF3(&db->sid[i]);
		SetPolyG3(&db->ege[i]);
		SetPolyF3(&db->sidP[i]);
		SetPolyFT3(&db->egeP[i]);
		SetShadeTex(&db->egeP[i],1);	/*No Shading*/
		db->egeP[i].tpage= 0x0100+TBX;	/*16bit texture 64*8 offset*/
		SetPolyG3(&db->egeG[i]);	/*alternate Gouraud */
	}
}


/*set light vector & light color*/
set_light_color(lmat,cmat)
MATRIX	*lmat,*cmat;
{
	lmat->m[0][0]= 1024; lmat->m[0][1]= 1024; lmat->m[0][2]= 3831;
	lmat->m[1][0]= 0; lmat->m[1][1]= 0; lmat->m[1][2]= 0;
	lmat->m[2][0]= 0; lmat->m[2][1]= 0; lmat->m[2][2]= 0;

	cmat->m[0][0]= 4096; cmat->m[0][1]= 0; cmat->m[0][2]= 0;
	cmat->m[1][0]= 4096; cmat->m[1][1]= 0; cmat->m[1][2]= 0;
	cmat->m[2][0]= 4096; cmat->m[2][1]= 0; cmat->m[2][2]= 0;
}

/*set vertex 3D coordinates*/
set_column_vertex(QCOL,center,hight,width,depth,cut)
SVECTOR	QCOL[8][2];
int	center[3];
int	hight;
int	width;
int	depth;
int	cut;
{
	int	i;

	for(i=0;i<2;i++){
		QCOL[0][i].vx= center[0]-width/2;
		QCOL[0][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[0][i].vz= center[2]-depth/2+cut;

		QCOL[1][i].vx= center[0]-width/2+cut;
		QCOL[1][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[1][i].vz= center[2]-depth/2;

		QCOL[2][i].vx= center[0]+width/2-cut;
		QCOL[2][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[2][i].vz= center[2]-depth/2;

		QCOL[3][i].vx= center[0]+width/2;
		QCOL[3][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[3][i].vz= center[2]-depth/2+cut;

		QCOL[4][i].vx= center[0]+width/2;
		QCOL[4][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[4][i].vz= center[2]+depth/2-cut;

		QCOL[5][i].vx= center[0]+width/2-cut;
		QCOL[5][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[5][i].vz= center[2]+depth/2;

		QCOL[6][i].vx= center[0]-width/2+cut;
		QCOL[6][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[6][i].vz= center[2]+depth/2;

		QCOL[7][i].vx= center[0]-width/2;
		QCOL[7][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[7][i].vz= center[2]+depth/2-cut;
	}
}

/*set normal vector of face*/
set_column_normal_4096(NML_TB,NML_SD)
SVECTOR *NML_TB;
SVECTOR *NML_SD;
{
	/*top&bottom*/
	NML_TB[0].vx= 0; NML_TB[0].vy= 4096; NML_TB[0].vz= 0;
	NML_TB[1].vx= 0; NML_TB[1].vy= -4096; NML_TB[1].vz= 0;

	/*side*/
	NML_SD[0].vx= 0; NML_SD[0].vy= 0; NML_SD[0].vz= 4096;
	NML_SD[1].vx= -4096; NML_SD[1].vy= 0; NML_SD[1].vz= 0;
	NML_SD[2].vx= 0; NML_SD[2].vy= 0; NML_SD[2].vz= -4096;
	NML_SD[3].vx= 4096; NML_SD[3].vy= 0; NML_SD[3].vz= 0;
}

/*rotate&transfer&perspective of 3D coordinate*/
rot_column(QCOL,SV_top,SV_bot,OTZ_top,OTZ_bot)
SVECTOR		QCOL[8][2];		/*3D coordinate*/
short		SV_top[8][2];		/*top 2D coordinate*/
short		SV_bot[8][2];		/*bottom 2D coordinate*/
int		OTZ_top[8];		/*OTZ of vertex*/
int		OTZ_bot[8];		/*OTZ of vertex*/
{
	int	i;
	long	p,flag;

	for(i=0;i<8;i++){
		OTZ_top[i]=RotTransPers(&QCOL[i][0],(long*)SV_top[i],&p,&flag);
		OTZ_bot[i]=RotTransPers(&QCOL[i][1],(long*)SV_bot[i],&p,&flag);
	}
}


/*lighting*/
col_column(NML_TB,NML_SD,COL_TB,COL_SD)
SVECTOR	*NML_TB;
SVECTOR	*NML_SD;
CVECTOR	*COL_TB;
CVECTOR	*COL_SD;
{
        int     i;
	MATRIX	WLmat;
        MATRIX  lm;

	/*rotate local light vector according to rotation of local coordinates*/
	ReadRotMatrix(&WLmat);		/*read local coodinates*/
	MulMatrix0(&lmat,&WLmat,&lm);	/*rotate local light*/
	SetRotMatrix(&WLmat);		/*MulMatrix destroies GTE RotMatrix*/
	SetLightMatrix(&lm);		/*set LightMatrix*/

	/*top&bottom face */
        for(i=0;i<2;i++) NormalColorCol(&NML_TB[i],&object_col,&COL_TB[i]);

	/*side face*/
        for(i=0;i<4;i++) NormalColorCol(&NML_SD[i],&object_col,&COL_SD[i]);
}

/*make GPU packet from vertex 2D coordinates & colors GOURAUD*/
set_prim(SV_top,SV_bot,COL_TB,COL_SD,db,OTZ_top,OTZ_bot)
short		SV_top[8][2];
short		SV_bot[8][2];
CVECTOR		COL_TB[2];
CVECTOR		COL_SD[4];
DB		*db;
int		OTZ_top[8];
int		OTZ_bot[8];
{
	int	i,j;

	/*coordinate: top&bottom vertices' screen coordinates-> 
	  top&bottom&side&edge triangles' vertices' coordinates */
	/*top*/
	db->top[0].x0= SV_top[0][0];	db->top[0].y0= SV_top[0][1];
	db->top[0].x1= SV_top[7][0];	db->top[0].y1= SV_top[7][1];
	db->top[0].x2= SV_top[6][0];	db->top[0].y2= SV_top[6][1];

	db->top[1].x0= SV_top[0][0];	db->top[1].y0= SV_top[0][1];
	db->top[1].x1= SV_top[6][0];	db->top[1].y1= SV_top[6][1];
	db->top[1].x2= SV_top[1][0];	db->top[1].y2= SV_top[1][1];

	db->top[2].x0= SV_top[1][0];	db->top[2].y0= SV_top[1][1];
	db->top[2].x1= SV_top[6][0];	db->top[2].y1= SV_top[6][1];
	db->top[2].x2= SV_top[5][0];	db->top[2].y2= SV_top[5][1];

	db->top[3].x0= SV_top[1][0];	db->top[3].y0= SV_top[1][1];
	db->top[3].x1= SV_top[5][0];	db->top[3].y1= SV_top[5][1];
	db->top[3].x2= SV_top[2][0];	db->top[3].y2= SV_top[2][1];

	db->top[4].x0= SV_top[2][0];	db->top[4].y0= SV_top[2][1];
	db->top[4].x1= SV_top[5][0];	db->top[4].y1= SV_top[5][1];
	db->top[4].x2= SV_top[4][0];	db->top[4].y2= SV_top[4][1];

	db->top[5].x0= SV_top[2][0];	db->top[5].y0= SV_top[2][1];
	db->top[5].x1= SV_top[4][0];	db->top[5].y1= SV_top[4][1];
	db->top[5].x2= SV_top[3][0];	db->top[5].y2= SV_top[3][1];

	/*bottom*/
	db->bot[0].x0= SV_bot[0][0];	db->bot[0].y0= SV_bot[0][1];
	db->bot[0].x1= SV_bot[6][0];	db->bot[0].y1= SV_bot[6][1];
	db->bot[0].x2= SV_bot[7][0];	db->bot[0].y2= SV_bot[7][1];

	db->bot[1].x0= SV_bot[0][0];	db->bot[1].y0= SV_bot[0][1];	
	db->bot[1].x1= SV_bot[1][0];	db->bot[1].y1= SV_bot[1][1];
	db->bot[1].x2= SV_bot[6][0];	db->bot[1].y2= SV_bot[6][1];

	db->bot[2].x0= SV_bot[1][0];	db->bot[2].y0= SV_bot[1][1];
	db->bot[2].x1= SV_bot[5][0];	db->bot[2].y1= SV_bot[5][1];
	db->bot[2].x2= SV_bot[6][0];	db->bot[2].y2= SV_bot[6][1];

	db->bot[3].x0= SV_bot[1][0];	db->bot[3].y0= SV_bot[1][1];
	db->bot[3].x1= SV_bot[2][0];	db->bot[3].y1= SV_bot[2][1];
	db->bot[3].x2= SV_bot[5][0];	db->bot[3].y2= SV_bot[5][1];

	db->bot[4].x0= SV_bot[2][0];	db->bot[4].y0= SV_bot[2][1];
	db->bot[4].x1= SV_bot[4][0];	db->bot[4].y1= SV_bot[4][1];
	db->bot[4].x2= SV_bot[5][0];	db->bot[4].y2= SV_bot[5][1];

	db->bot[5].x0= SV_bot[2][0];	db->bot[5].y0= SV_bot[2][1];
	db->bot[5].x1= SV_bot[3][0];	db->bot[5].y1= SV_bot[3][1];
	db->bot[5].x2= SV_bot[4][0];	db->bot[5].y2= SV_bot[4][1];

	/*side*/
	db->sid[0].x0= SV_top[1][0];	db->sid[0].y0= SV_top[1][1];
	db->sid[0].x1= SV_top[2][0];	db->sid[0].y1= SV_top[2][1];
	db->sid[0].x2= SV_bot[1][0];	db->sid[0].y2= SV_bot[1][1];

	db->sid[1].x0= SV_bot[1][0];	db->sid[1].y0= SV_bot[1][1];
	db->sid[1].x1= SV_top[2][0];	db->sid[1].y1= SV_top[2][1];
	db->sid[1].x2= SV_bot[2][0];	db->sid[1].y2= SV_bot[2][1];

	db->sid[2].x0= SV_top[3][0];	db->sid[2].y0= SV_top[3][1];
	db->sid[2].x1= SV_top[4][0];	db->sid[2].y1= SV_top[4][1];
	db->sid[2].x2= SV_bot[3][0];	db->sid[2].y2= SV_bot[3][1];

	db->sid[3].x0= SV_bot[3][0];	db->sid[3].y0= SV_bot[3][1];
	db->sid[3].x1= SV_top[4][0];	db->sid[3].y1= SV_top[4][1];
	db->sid[3].x2= SV_bot[4][0];	db->sid[3].y2= SV_bot[4][1];

	db->sid[4].x0= SV_top[5][0];	db->sid[4].y0= SV_top[5][1];
	db->sid[4].x1= SV_top[6][0];	db->sid[4].y1= SV_top[6][1];
	db->sid[4].x2= SV_bot[5][0];	db->sid[4].y2= SV_bot[5][1];

	db->sid[5].x0= SV_bot[5][0];	db->sid[5].y0= SV_bot[5][1];
	db->sid[5].x1= SV_top[6][0];	db->sid[5].y1= SV_top[6][1];
	db->sid[5].x2= SV_bot[6][0];	db->sid[5].y2= SV_bot[6][1];

	db->sid[6].x0= SV_top[7][0];	db->sid[6].y0= SV_top[7][1];
	db->sid[6].x1= SV_top[0][0];	db->sid[6].y1= SV_top[0][1];
	db->sid[6].x2= SV_bot[7][0];	db->sid[6].y2= SV_bot[7][1];

	db->sid[7].x0= SV_bot[7][0];	db->sid[7].y0= SV_bot[7][1];
	db->sid[7].x1= SV_top[0][0];	db->sid[7].y1= SV_top[0][1];
	db->sid[7].x2= SV_bot[0][0];	db->sid[7].y2= SV_bot[0][1];

	/*edge*/
	db->ege[0].x0= SV_top[0][0];	db->ege[0].y0= SV_top[0][1];
	db->ege[0].x1= SV_top[1][0];	db->ege[0].y1= SV_top[1][1];
	db->ege[0].x2= SV_bot[0][0];	db->ege[0].y2= SV_bot[0][1];

	db->ege[1].x0= SV_bot[0][0];	db->ege[1].y0= SV_bot[0][1];
	db->ege[1].x1= SV_top[1][0];	db->ege[1].y1= SV_top[1][1];
	db->ege[1].x2= SV_bot[1][0];	db->ege[1].y2= SV_bot[1][1];

	db->ege[2].x0= SV_top[2][0];	db->ege[2].y0= SV_top[2][1];
	db->ege[2].x1= SV_top[3][0];	db->ege[2].y1= SV_top[3][1];
	db->ege[2].x2= SV_bot[2][0];	db->ege[2].y2= SV_bot[2][1];

	db->ege[3].x0= SV_bot[2][0];	db->ege[3].y0= SV_bot[2][1];
	db->ege[3].x1= SV_top[3][0];	db->ege[3].y1= SV_top[3][1];
	db->ege[3].x2= SV_bot[3][0];	db->ege[3].y2= SV_bot[3][1];

	db->ege[4].x0= SV_top[4][0];	db->ege[4].y0= SV_top[4][1];
	db->ege[4].x1= SV_top[5][0];	db->ege[4].y1= SV_top[5][1];
	db->ege[4].x2= SV_bot[4][0];	db->ege[4].y2= SV_bot[4][1];

	db->ege[5].x0= SV_bot[4][0];	db->ege[5].y0= SV_bot[4][1];
	db->ege[5].x1= SV_top[5][0];	db->ege[5].y1= SV_top[5][1];
	db->ege[5].x2= SV_bot[5][0];	db->ege[5].y2= SV_bot[5][1];

	db->ege[6].x0= SV_top[6][0];	db->ege[6].y0= SV_top[6][1];
	db->ege[6].x1= SV_top[7][0];	db->ege[6].y1= SV_top[7][1];
	db->ege[6].x2= SV_bot[6][0];	db->ege[6].y2= SV_bot[6][1];

	db->ege[7].x0= SV_bot[6][0];	db->ege[7].y0= SV_bot[6][1];
	db->ege[7].x1= SV_top[7][0];	db->ege[7].y1= SV_top[7][1];
	db->ege[7].x2= SV_bot[7][0];	db->ege[7].y2= SV_bot[7][1];

	/*color: top&bottom&side faces' color -> 
		top&bottom&side faces' color and edge vertices' color*/
	/*top,bottom*/
	for(i=0;i<6;i++){
		db->top[i].r0= COL_TB[0].r;	
		db->top[i].g0= COL_TB[0].g;
		db->top[i].b0= COL_TB[0].b;
		db->bot[i].r0= COL_TB[1].r;	
		db->bot[i].g0= COL_TB[1].g;
		db->bot[i].b0= COL_TB[1].b;
	}
	/*side*/
	for(i=0;i<4;i++){
		db->sid[2*i].r0= COL_SD[i].r;	
		db->sid[2*i].g0= COL_SD[i].g;
		db->sid[2*i].b0= COL_SD[i].b;
		db->sid[2*i+1].r0= COL_SD[i].r;	
		db->sid[2*i+1].g0= COL_SD[i].g;
		db->sid[2*i+1].b0= COL_SD[i].b;
	}
	/*edge*/
	db->ege[0].r0= COL_SD[3].r;
	db->ege[0].g0= COL_SD[3].g;
	db->ege[0].b0= COL_SD[3].b;
	db->ege[0].r1= COL_SD[0].r;
	db->ege[0].g1= COL_SD[0].g;
	db->ege[0].b1= COL_SD[0].b;
	db->ege[0].r2= COL_SD[3].r;
	db->ege[0].g2= COL_SD[3].g;
	db->ege[0].b2= COL_SD[3].b;

	db->ege[1].r0= COL_SD[3].r;
	db->ege[1].g0= COL_SD[3].g;
	db->ege[1].b0= COL_SD[3].b;
	db->ege[1].r1= COL_SD[0].r;
	db->ege[1].g1= COL_SD[0].g;
	db->ege[1].b1= COL_SD[0].b;
	db->ege[1].r2= COL_SD[0].r;
	db->ege[1].g2= COL_SD[0].g;
	db->ege[1].b2= COL_SD[0].b;

	db->ege[2].r0= COL_SD[0].r;
	db->ege[2].g0= COL_SD[0].g;
	db->ege[2].b0= COL_SD[0].b;
	db->ege[2].r1= COL_SD[1].r;
	db->ege[2].g1= COL_SD[1].g;
	db->ege[2].b1= COL_SD[1].b;
	db->ege[2].r2= COL_SD[0].r;
	db->ege[2].g2= COL_SD[0].g;
	db->ege[2].b2= COL_SD[0].b;

	db->ege[3].r0= COL_SD[0].r;
	db->ege[3].g0= COL_SD[0].g;
	db->ege[3].b0= COL_SD[0].b;
	db->ege[3].r1= COL_SD[1].r;
	db->ege[3].g1= COL_SD[1].g;
	db->ege[3].b1= COL_SD[1].b;
	db->ege[3].r2= COL_SD[1].r;
	db->ege[3].g2= COL_SD[1].g;
	db->ege[3].b2= COL_SD[1].b;

	db->ege[4].r0= COL_SD[1].r;
	db->ege[4].g0= COL_SD[1].g;
	db->ege[4].b0= COL_SD[1].b;
	db->ege[4].r1= COL_SD[2].r;
	db->ege[4].g1= COL_SD[2].g;
	db->ege[4].b1= COL_SD[2].b;
	db->ege[4].r2= COL_SD[1].r;
	db->ege[4].g2= COL_SD[1].g;
	db->ege[4].b2= COL_SD[1].b;

	db->ege[5].r0= COL_SD[1].r;
	db->ege[5].g0= COL_SD[1].g;
	db->ege[5].b0= COL_SD[1].b;
	db->ege[5].r1= COL_SD[2].r;
	db->ege[5].g1= COL_SD[2].g;
	db->ege[5].b1= COL_SD[2].b;
	db->ege[5].r2= COL_SD[2].r;
	db->ege[5].g2= COL_SD[2].g;
	db->ege[5].b2= COL_SD[2].b;

	db->ege[6].r0= COL_SD[2].r;
	db->ege[6].g0= COL_SD[2].g;
	db->ege[6].b0= COL_SD[2].b;
	db->ege[6].r1= COL_SD[3].r;
	db->ege[6].g1= COL_SD[3].g;
	db->ege[6].b1= COL_SD[3].b;
	db->ege[6].r2= COL_SD[2].r;
	db->ege[6].g2= COL_SD[2].g;
	db->ege[6].b2= COL_SD[2].b;

	db->ege[7].r0= COL_SD[2].r;
	db->ege[7].g0= COL_SD[2].g;
	db->ege[7].b0= COL_SD[2].b;
	db->ege[7].r1= COL_SD[3].r;
	db->ege[7].g1= COL_SD[3].g;
	db->ege[7].b1= COL_SD[3].b;
	db->ege[7].r2= COL_SD[3].r;
	db->ege[7].g2= COL_SD[3].g;
	db->ege[7].b2= COL_SD[3].b;

	/*OTZ: top&bottom vertices' OTZ-> top&bottom&side&edge faces' OTZ*/
	/*top*/
	db->otz_top[0]= OTZ_top[0]>>(14-OTLEN);
	db->otz_top[1]= OTZ_top[0]>>(14-OTLEN);
	db->otz_top[2]= OTZ_top[1]>>(14-OTLEN);
	db->otz_top[3]= OTZ_top[1]>>(14-OTLEN);
	db->otz_top[4]= OTZ_top[2]>>(14-OTLEN);
	db->otz_top[5]= OTZ_top[2]>>(14-OTLEN);

	/*bot*/
	db->otz_bot[0]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_bot[1]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_bot[2]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_bot[3]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_bot[4]= OTZ_bot[2]>>(14-OTLEN);
	db->otz_bot[5]= OTZ_bot[2]>>(14-OTLEN);

	/*side*/
	db->otz_sid[0]= OTZ_top[1]>>(14-OTLEN);
	db->otz_sid[1]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_sid[2]= OTZ_top[3]>>(14-OTLEN);
	db->otz_sid[3]= OTZ_bot[3]>>(14-OTLEN);
	db->otz_sid[4]= OTZ_top[5]>>(14-OTLEN);
	db->otz_sid[5]= OTZ_bot[5]>>(14-OTLEN);
	db->otz_sid[6]= OTZ_top[7]>>(14-OTLEN);
	db->otz_sid[7]= OTZ_bot[7]>>(14-OTLEN);

	/*edge*/
	db->otz_ege[0]= OTZ_top[0]>>(14-OTLEN);
	db->otz_ege[1]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_ege[2]= OTZ_top[2]>>(14-OTLEN);
	db->otz_ege[3]= OTZ_bot[2]>>(14-OTLEN);
	db->otz_ege[4]= OTZ_top[4]>>(14-OTLEN);
	db->otz_ege[5]= OTZ_bot[4]>>(14-OTLEN);
	db->otz_ege[6]= OTZ_top[6]>>(14-OTLEN);
	db->otz_ege[7]= OTZ_bot[6]>>(14-OTLEN);
}

/*add GPU packet to OT*/
add_prim(db)
DB	*db;
{
	int	i,j;
	long	opz;

	for(i=0;i<6;i++){
                opz= NormalClip(*(long*)&db->top[i].x0,
                                *(long*)&db->top[i].x1,
                                *(long*)&db->top[i].x2);

		if(opz>0) AddPrim(db->ot+db->otz_top[i],&db->top[i]);

                opz= NormalClip(*(long*)&db->bot[i].x0,
                                *(long*)&db->bot[i].x1,
                                *(long*)&db->bot[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_bot[i],&db->bot[i]);

	}

	for(i=0;i<8;i++){
                opz= NormalClip(*(long*)&db->sid[i].x0,
                                *(long*)&db->sid[i].x1,
                                *(long*)&db->sid[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_sid[i],&db->sid[i]);

                opz= NormalClip(*(long*)&db->ege[i].x0,
                                *(long*)&db->ege[i].x1,
                                *(long*)&db->ege[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_ege[i],&db->ege[i]);

	}
}
	
/*make GPU packet from vertex 2D coordinates & colors PHONG*/
set_primP(SV_topP,SV_botP,COL_TB,COL_SD,db,OTZ_top,OTZ_bot)
short		SV_topP[8][2];
short		SV_botP[8][2];
CVECTOR		COL_TB[2];
CVECTOR		COL_SD[4];
DB		*db;
int		OTZ_top[8];
int		OTZ_bot[8];
{
	int	i,j;

	/*coordinate: top&bottom vertices' screen coordinates-> 
	  top&bottom&side&edge triangles' vertices' coordinates 
	  and edge triangles' vertices' texture address*/
	/*top*/
	db->topP[0].x0= SV_topP[0][0];	db->topP[0].y0= SV_topP[0][1];
	db->topP[0].x1= SV_topP[7][0];	db->topP[0].y1= SV_topP[7][1];
	db->topP[0].x2= SV_topP[6][0];	db->topP[0].y2= SV_topP[6][1];

	db->topP[1].x0= SV_topP[0][0];	db->topP[1].y0= SV_topP[0][1];
	db->topP[1].x1= SV_topP[6][0];	db->topP[1].y1= SV_topP[6][1];
	db->topP[1].x2= SV_topP[1][0];	db->topP[1].y2= SV_topP[1][1];

	db->topP[2].x0= SV_topP[1][0];	db->topP[2].y0= SV_topP[1][1];
	db->topP[2].x1= SV_topP[6][0];	db->topP[2].y1= SV_topP[6][1];
	db->topP[2].x2= SV_topP[5][0];	db->topP[2].y2= SV_topP[5][1];

	db->topP[3].x0= SV_topP[1][0];	db->topP[3].y0= SV_topP[1][1];
	db->topP[3].x1= SV_topP[5][0];	db->topP[3].y1= SV_topP[5][1];
	db->topP[3].x2= SV_topP[2][0];	db->topP[3].y2= SV_topP[2][1];

	db->topP[4].x0= SV_topP[2][0];	db->topP[4].y0= SV_topP[2][1];
	db->topP[4].x1= SV_topP[5][0];	db->topP[4].y1= SV_topP[5][1];
	db->topP[4].x2= SV_topP[4][0];	db->topP[4].y2= SV_topP[4][1];

	db->topP[5].x0= SV_topP[2][0];	db->topP[5].y0= SV_topP[2][1];
	db->topP[5].x1= SV_topP[4][0];	db->topP[5].y1= SV_topP[4][1];
	db->topP[5].x2= SV_topP[3][0];	db->topP[5].y2= SV_topP[3][1];

	/*bottom*/
	db->botP[0].x0= SV_botP[0][0];	db->botP[0].y0= SV_botP[0][1];
	db->botP[0].x1= SV_botP[6][0];	db->botP[0].y1= SV_botP[6][1];
	db->botP[0].x2= SV_botP[7][0];	db->botP[0].y2= SV_botP[7][1];

	db->botP[1].x0= SV_botP[0][0];	db->botP[1].y0= SV_botP[0][1];	
	db->botP[1].x1= SV_botP[1][0];	db->botP[1].y1= SV_botP[1][1];
	db->botP[1].x2= SV_botP[6][0];	db->botP[1].y2= SV_botP[6][1];

	db->botP[2].x0= SV_botP[1][0];	db->botP[2].y0= SV_botP[1][1];
	db->botP[2].x1= SV_botP[5][0];	db->botP[2].y1= SV_botP[5][1];
	db->botP[2].x2= SV_botP[6][0];	db->botP[2].y2= SV_botP[6][1];

	db->botP[3].x0= SV_botP[1][0];	db->botP[3].y0= SV_botP[1][1];
	db->botP[3].x1= SV_botP[2][0];	db->botP[3].y1= SV_botP[2][1];
	db->botP[3].x2= SV_botP[5][0];	db->botP[3].y2= SV_botP[5][1];

	db->botP[4].x0= SV_botP[2][0];	db->botP[4].y0= SV_botP[2][1];
	db->botP[4].x1= SV_botP[4][0];	db->botP[4].y1= SV_botP[4][1];
	db->botP[4].x2= SV_botP[5][0];	db->botP[4].y2= SV_botP[5][1];

	db->botP[5].x0= SV_botP[2][0];	db->botP[5].y0= SV_botP[2][1];
	db->botP[5].x1= SV_botP[3][0];	db->botP[5].y1= SV_botP[3][1];
	db->botP[5].x2= SV_botP[4][0];	db->botP[5].y2= SV_botP[4][1];

	/*side*/
	db->sidP[0].x0= SV_topP[1][0];	db->sidP[0].y0= SV_topP[1][1];
	db->sidP[0].x1= SV_topP[2][0];	db->sidP[0].y1= SV_topP[2][1];
	db->sidP[0].x2= SV_botP[1][0];	db->sidP[0].y2= SV_botP[1][1];

	db->sidP[1].x0= SV_botP[1][0];	db->sidP[1].y0= SV_botP[1][1];
	db->sidP[1].x1= SV_topP[2][0];	db->sidP[1].y1= SV_topP[2][1];
	db->sidP[1].x2= SV_botP[2][0];	db->sidP[1].y2= SV_botP[2][1];

	db->sidP[2].x0= SV_topP[3][0];	db->sidP[2].y0= SV_topP[3][1];
	db->sidP[2].x1= SV_topP[4][0];	db->sidP[2].y1= SV_topP[4][1];
	db->sidP[2].x2= SV_botP[3][0];	db->sidP[2].y2= SV_botP[3][1];

	db->sidP[3].x0= SV_botP[3][0];	db->sidP[3].y0= SV_botP[3][1];
	db->sidP[3].x1= SV_topP[4][0];	db->sidP[3].y1= SV_topP[4][1];
	db->sidP[3].x2= SV_botP[4][0];	db->sidP[3].y2= SV_botP[4][1];

	db->sidP[4].x0= SV_topP[5][0];	db->sidP[4].y0= SV_topP[5][1];
	db->sidP[4].x1= SV_topP[6][0];	db->sidP[4].y1= SV_topP[6][1];
	db->sidP[4].x2= SV_botP[5][0];	db->sidP[4].y2= SV_botP[5][1];

	db->sidP[5].x0= SV_botP[5][0];	db->sidP[5].y0= SV_botP[5][1];
	db->sidP[5].x1= SV_topP[6][0];	db->sidP[5].y1= SV_topP[6][1];
	db->sidP[5].x2= SV_botP[6][0];	db->sidP[5].y2= SV_botP[6][1];

	db->sidP[6].x0= SV_topP[7][0];	db->sidP[6].y0= SV_topP[7][1];
	db->sidP[6].x1= SV_topP[0][0];	db->sidP[6].y1= SV_topP[0][1];
	db->sidP[6].x2= SV_botP[7][0];	db->sidP[6].y2= SV_botP[7][1];

	db->sidP[7].x0= SV_botP[7][0];	db->sidP[7].y0= SV_botP[7][1];
	db->sidP[7].x1= SV_topP[0][0];	db->sidP[7].y1= SV_topP[0][1];
	db->sidP[7].x2= SV_botP[0][0];	db->sidP[7].y2= SV_botP[0][1];

	/*edge*/
	db->egeP[0].x0= SV_topP[0][0];	db->egeP[0].y0= SV_topP[0][1];
	db->egeP[0].x1= SV_topP[1][0];	db->egeP[0].y1= SV_topP[1][1];
	db->egeP[0].x2= SV_botP[0][0];	db->egeP[0].y2= SV_botP[0][1];

	db->egeP[1].x0= SV_botP[0][0];	db->egeP[1].y0= SV_botP[0][1];
	db->egeP[1].x1= SV_topP[1][0];	db->egeP[1].y1= SV_topP[1][1];
	db->egeP[1].x2= SV_botP[1][0];	db->egeP[1].y2= SV_botP[1][1];

	db->egeP[2].x0= SV_topP[2][0];	db->egeP[2].y0= SV_topP[2][1];
	db->egeP[2].x1= SV_topP[3][0];	db->egeP[2].y1= SV_topP[3][1];
	db->egeP[2].x2= SV_botP[2][0];	db->egeP[2].y2= SV_botP[2][1];

	db->egeP[3].x0= SV_botP[2][0];	db->egeP[3].y0= SV_botP[2][1];
	db->egeP[3].x1= SV_topP[3][0];	db->egeP[3].y1= SV_topP[3][1];
	db->egeP[3].x2= SV_botP[3][0];	db->egeP[3].y2= SV_botP[3][1];

	db->egeP[4].x0= SV_topP[4][0];	db->egeP[4].y0= SV_topP[4][1];
	db->egeP[4].x1= SV_topP[5][0];	db->egeP[4].y1= SV_topP[5][1];
	db->egeP[4].x2= SV_botP[4][0];	db->egeP[4].y2= SV_botP[4][1];

	db->egeP[5].x0= SV_botP[4][0];	db->egeP[5].y0= SV_botP[4][1];
	db->egeP[5].x1= SV_topP[5][0];	db->egeP[5].y1= SV_topP[5][1];
	db->egeP[5].x2= SV_botP[5][0];	db->egeP[5].y2= SV_botP[5][1];

	db->egeP[6].x0= SV_topP[6][0];	db->egeP[6].y0= SV_topP[6][1];
	db->egeP[6].x1= SV_topP[7][0];	db->egeP[6].y1= SV_topP[7][1];
	db->egeP[6].x2= SV_botP[6][0];	db->egeP[6].y2= SV_botP[6][1];

	db->egeP[7].x0= SV_botP[6][0];	db->egeP[7].y0= SV_botP[6][1];
	db->egeP[7].x1= SV_topP[7][0];	db->egeP[7].y1= SV_topP[7][1];
	db->egeP[7].x2= SV_botP[7][0];	db->egeP[7].y2= SV_botP[7][1];

	/*edge GOURAUD*/
	db->egeG[0].x0= SV_topP[0][0];	db->egeG[0].y0= SV_topP[0][1];
	db->egeG[0].x1= SV_topP[1][0];	db->egeG[0].y1= SV_topP[1][1];
	db->egeG[0].x2= SV_botP[0][0];	db->egeG[0].y2= SV_botP[0][1];

	db->egeG[1].x0= SV_botP[0][0];	db->egeG[1].y0= SV_botP[0][1];
	db->egeG[1].x1= SV_topP[1][0];	db->egeG[1].y1= SV_topP[1][1];
	db->egeG[1].x2= SV_botP[1][0];	db->egeG[1].y2= SV_botP[1][1];

	db->egeG[2].x0= SV_topP[2][0];	db->egeG[2].y0= SV_topP[2][1];
	db->egeG[2].x1= SV_topP[3][0];	db->egeG[2].y1= SV_topP[3][1];
	db->egeG[2].x2= SV_botP[2][0];	db->egeG[2].y2= SV_botP[2][1];

	db->egeG[3].x0= SV_botP[2][0];	db->egeG[3].y0= SV_botP[2][1];
	db->egeG[3].x1= SV_topP[3][0];	db->egeG[3].y1= SV_topP[3][1];
	db->egeG[3].x2= SV_botP[3][0];	db->egeG[3].y2= SV_botP[3][1];

	db->egeG[4].x0= SV_topP[4][0];	db->egeG[4].y0= SV_topP[4][1];
	db->egeG[4].x1= SV_topP[5][0];	db->egeG[4].y1= SV_topP[5][1];
	db->egeG[4].x2= SV_botP[4][0];	db->egeG[4].y2= SV_botP[4][1];

	db->egeG[5].x0= SV_botP[4][0];	db->egeG[5].y0= SV_botP[4][1];
	db->egeG[5].x1= SV_topP[5][0];	db->egeG[5].y1= SV_topP[5][1];
	db->egeG[5].x2= SV_botP[5][0];	db->egeG[5].y2= SV_botP[5][1];

	db->egeG[6].x0= SV_topP[6][0];	db->egeG[6].y0= SV_topP[6][1];
	db->egeG[6].x1= SV_topP[7][0];	db->egeG[6].y1= SV_topP[7][1];
	db->egeG[6].x2= SV_botP[6][0];	db->egeG[6].y2= SV_botP[6][1];

	db->egeG[7].x0= SV_botP[6][0];	db->egeG[7].y0= SV_botP[6][1];
	db->egeG[7].x1= SV_topP[7][0];	db->egeG[7].y1= SV_topP[7][1];
	db->egeG[7].x2= SV_botP[7][0];	db->egeG[7].y2= SV_botP[7][1];

	/*edgeUV*/
	db->egeP[0].u0= SV_topP[0][0];	db->egeP[0].v0= SV_topP[0][1];
	db->egeP[0].u1= SV_topP[1][0];	db->egeP[0].v1= SV_topP[1][1];
	db->egeP[0].u2= SV_botP[0][0];	db->egeP[0].v2= SV_botP[0][1];

	db->egeP[1].u0= SV_botP[0][0];	db->egeP[1].v0= SV_botP[0][1];
	db->egeP[1].u1= SV_topP[1][0];	db->egeP[1].v1= SV_topP[1][1];
	db->egeP[1].u2= SV_botP[1][0];	db->egeP[1].v2= SV_botP[1][1];

	db->egeP[2].u0= SV_topP[2][0];	db->egeP[2].v0= SV_topP[2][1];
	db->egeP[2].u1= SV_topP[3][0];	db->egeP[2].v1= SV_topP[3][1];
	db->egeP[2].u2= SV_botP[2][0];	db->egeP[2].v2= SV_botP[2][1];

	db->egeP[3].u0= SV_botP[2][0];	db->egeP[3].v0= SV_botP[2][1];
	db->egeP[3].u1= SV_topP[3][0];	db->egeP[3].v1= SV_topP[3][1];
	db->egeP[3].u2= SV_botP[3][0];	db->egeP[3].v2= SV_botP[3][1];

	db->egeP[4].u0= SV_topP[4][0];	db->egeP[4].v0= SV_topP[4][1];
	db->egeP[4].u1= SV_topP[5][0];	db->egeP[4].v1= SV_topP[5][1];
	db->egeP[4].u2= SV_botP[4][0];	db->egeP[4].v2= SV_botP[4][1];

	db->egeP[5].u0= SV_botP[4][0];	db->egeP[5].v0= SV_botP[4][1];
	db->egeP[5].u1= SV_topP[5][0];	db->egeP[5].v1= SV_topP[5][1];
	db->egeP[5].u2= SV_botP[5][0];	db->egeP[5].v2= SV_botP[5][1];

	db->egeP[6].u0= SV_topP[6][0];	db->egeP[6].v0= SV_topP[6][1];
	db->egeP[6].u1= SV_topP[7][0];	db->egeP[6].v1= SV_topP[7][1];
	db->egeP[6].u2= SV_botP[6][0];	db->egeP[6].v2= SV_botP[6][1];

	db->egeP[7].u0= SV_botP[6][0];	db->egeP[7].v0= SV_botP[6][1];
	db->egeP[7].u1= SV_topP[7][0];	db->egeP[7].v1= SV_topP[7][1];
	db->egeP[7].u2= SV_botP[7][0];	db->egeP[7].v2= SV_botP[7][1];

	/*color: top&bottom&side faces' color -> 
		top&bottom&side faces' color and edge vertices' color*/
	/*topP,botPtom*/
	for(i=0;i<6;i++){
		db->topP[i].r0= COL_TB[0].r;	
		db->topP[i].g0= COL_TB[0].g;
		db->topP[i].b0= COL_TB[0].b;
		db->botP[i].r0= COL_TB[1].r;	
		db->botP[i].g0= COL_TB[1].g;
		db->botP[i].b0= COL_TB[1].b;
	}
	/*sidPe*/
	for(i=0;i<4;i++){
		db->sidP[2*i].r0= COL_SD[i].r;	
		db->sidP[2*i].g0= COL_SD[i].g;
		db->sidP[2*i].b0= COL_SD[i].b;
		db->sidP[2*i+1].r0= COL_SD[i].r;	
		db->sidP[2*i+1].g0= COL_SD[i].g;
		db->sidP[2*i+1].b0= COL_SD[i].b;
	}
	/*edge*/
	db->egeG[0].r0= COL_SD[3].r;
	db->egeG[0].g0= COL_SD[3].g;
	db->egeG[0].b0= COL_SD[3].b;
	db->egeG[0].r1= COL_SD[0].r;
	db->egeG[0].g1= COL_SD[0].g;
	db->egeG[0].b1= COL_SD[0].b;
	db->egeG[0].r2= COL_SD[3].r;
	db->egeG[0].g2= COL_SD[3].g;
	db->egeG[0].b2= COL_SD[3].b;

	db->egeG[1].r0= COL_SD[3].r;
	db->egeG[1].g0= COL_SD[3].g;
	db->egeG[1].b0= COL_SD[3].b;
	db->egeG[1].r1= COL_SD[0].r;
	db->egeG[1].g1= COL_SD[0].g;
	db->egeG[1].b1= COL_SD[0].b;
	db->egeG[1].r2= COL_SD[0].r;
	db->egeG[1].g2= COL_SD[0].g;
	db->egeG[1].b2= COL_SD[0].b;

	db->egeG[2].r0= COL_SD[0].r;
	db->egeG[2].g0= COL_SD[0].g;
	db->egeG[2].b0= COL_SD[0].b;
	db->egeG[2].r1= COL_SD[1].r;
	db->egeG[2].g1= COL_SD[1].g;
	db->egeG[2].b1= COL_SD[1].b;
	db->egeG[2].r2= COL_SD[0].r;
	db->egeG[2].g2= COL_SD[0].g;
	db->egeG[2].b2= COL_SD[0].b;

	db->egeG[3].r0= COL_SD[0].r;
	db->egeG[3].g0= COL_SD[0].g;
	db->egeG[3].b0= COL_SD[0].b;
	db->egeG[3].r1= COL_SD[1].r;
	db->egeG[3].g1= COL_SD[1].g;
	db->egeG[3].b1= COL_SD[1].b;
	db->egeG[3].r2= COL_SD[1].r;
	db->egeG[3].g2= COL_SD[1].g;
	db->egeG[3].b2= COL_SD[1].b;

	db->egeG[4].r0= COL_SD[1].r;
	db->egeG[4].g0= COL_SD[1].g;
	db->egeG[4].b0= COL_SD[1].b;
	db->egeG[4].r1= COL_SD[2].r;
	db->egeG[4].g1= COL_SD[2].g;
	db->egeG[4].b1= COL_SD[2].b;
	db->egeG[4].r2= COL_SD[1].r;
	db->egeG[4].g2= COL_SD[1].g;
	db->egeG[4].b2= COL_SD[1].b;

	db->egeG[5].r0= COL_SD[1].r;
	db->egeG[5].g0= COL_SD[1].g;
	db->egeG[5].b0= COL_SD[1].b;
	db->egeG[5].r1= COL_SD[2].r;
	db->egeG[5].g1= COL_SD[2].g;
	db->egeG[5].b1= COL_SD[2].b;
	db->egeG[5].r2= COL_SD[2].r;
	db->egeG[5].g2= COL_SD[2].g;
	db->egeG[5].b2= COL_SD[2].b;

	db->egeG[6].r0= COL_SD[2].r;
	db->egeG[6].g0= COL_SD[2].g;
	db->egeG[6].b0= COL_SD[2].b;
	db->egeG[6].r1= COL_SD[3].r;
	db->egeG[6].g1= COL_SD[3].g;
	db->egeG[6].b1= COL_SD[3].b;
	db->egeG[6].r2= COL_SD[2].r;
	db->egeG[6].g2= COL_SD[2].g;
	db->egeG[6].b2= COL_SD[2].b;

	db->egeG[7].r0= COL_SD[2].r;
	db->egeG[7].g0= COL_SD[2].g;
	db->egeG[7].b0= COL_SD[2].b;
	db->egeG[7].r1= COL_SD[3].r;
	db->egeG[7].g1= COL_SD[3].g;
	db->egeG[7].b1= COL_SD[3].b;
	db->egeG[7].r2= COL_SD[3].r;
	db->egeG[7].g2= COL_SD[3].g;
	db->egeG[7].b2= COL_SD[3].b;

	/*OTZ: top&bottom vertices' OTZ-> top&bottom&side&edge faces' OTZ*/
	/*top*/
	db->otz_topP[0]= OTZ_top[0]>>(14-OTLEN);
	db->otz_topP[1]= OTZ_top[0]>>(14-OTLEN);
	db->otz_topP[2]= OTZ_top[1]>>(14-OTLEN);
	db->otz_topP[3]= OTZ_top[1]>>(14-OTLEN);
	db->otz_topP[4]= OTZ_top[2]>>(14-OTLEN);
	db->otz_topP[5]= OTZ_top[2]>>(14-OTLEN);

	/*bot*/
	db->otz_botP[0]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_botP[1]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_botP[2]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_botP[3]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_botP[4]= OTZ_bot[2]>>(14-OTLEN);
	db->otz_botP[5]= OTZ_bot[2]>>(14-OTLEN);

	/*side*/
	db->otz_sidP[0]= OTZ_top[1]>>(14-OTLEN);
	db->otz_sidP[1]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_sidP[2]= OTZ_top[3]>>(14-OTLEN);
	db->otz_sidP[3]= OTZ_bot[3]>>(14-OTLEN);
	db->otz_sidP[4]= OTZ_top[5]>>(14-OTLEN);
	db->otz_sidP[5]= OTZ_bot[5]>>(14-OTLEN);
	db->otz_sidP[6]= OTZ_top[7]>>(14-OTLEN);
	db->otz_sidP[7]= OTZ_bot[7]>>(14-OTLEN);

	/*edge*/
	db->otz_egeP[0]= OTZ_top[0]>>(14-OTLEN);
	db->otz_egeP[1]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_egeP[2]= OTZ_top[2]>>(14-OTLEN);
	db->otz_egeP[3]= OTZ_bot[2]>>(14-OTLEN);
	db->otz_egeP[4]= OTZ_top[4]>>(14-OTLEN);
	db->otz_egeP[5]= OTZ_bot[4]>>(14-OTLEN);
	db->otz_egeP[6]= OTZ_top[6]>>(14-OTLEN);
	db->otz_egeP[7]= OTZ_bot[6]>>(14-OTLEN);
}

/*add GPU packet to OT*/
add_primP(db)
DB	*db;
{
	int	i,j;
	long	opz;

	for(i=0;i<6;i++){
                opz= NormalClip(*(long*)&db->topP[i].x0,
                                *(long*)&db->topP[i].x1,
                                *(long*)&db->topP[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_topP[i],&db->topP[i]);

                opz= NormalClip(*(long*)&db->botP[i].x0,
                                *(long*)&db->botP[i].x1,
                                *(long*)&db->botP[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_botP[i],&db->botP[i]);

	}

	for(i=0;i<8;i++){
                opz= NormalClip(*(long*)&db->sidP[i].x0,
                                *(long*)&db->sidP[i].x1,
                                *(long*)&db->sidP[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_sidP[i],&db->sidP[i]);

                opz= NormalClip(*(long*)&db->egeP[i].x0,
                                *(long*)&db->egeP[i].x1,
                                *(long*)&db->egeP[i].x2);

                if(opz>PG) AddPrim(db->ot+db->otz_egeP[i],&db->egeP[i]);
		if(opz>0&&opz<=PG) AddPrim(db->ot+db->otz_egeP[i],&db->egeG[i]);
	}
}
	
/*Phong Shading Column's 4 Edge faces(8 triangles)*/
phong_column_edge(SV_top,SV_bot,NML_TB,NML_SD)
short		SV_top[8][2];		/*top vertices' screen coordinates*/
short		SV_bot[8][2];		/*bottom vertices' screen coordinates*/
SVECTOR		NML_TB[2];		/*top&bottom normals(not used)*/
SVECTOR		NML_SD[4];		/*side normals*/
{
	SVECTOR nml[3];
	short   v[3][2];
	int	opz;


        opz= NormalClip(*(long*)SV_top[0],*(long*)SV_top[1],*(long*)SV_bot[0]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[3].vx;
		nml[0].vy= NML_SD[3].vy;
		nml[0].vz= NML_SD[3].vz;
		nml[1].vx= NML_SD[0].vx;
		nml[1].vy= NML_SD[0].vy;
		nml[1].vz= NML_SD[0].vz;
		nml[2].vx= NML_SD[3].vx;
		nml[2].vy= NML_SD[3].vy;
		nml[2].vz= NML_SD[3].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_top[0][0]+TBX*64;	v[0][1]= SV_top[0][1];
		v[1][0]= SV_top[1][0]+TBX*64;	v[1][1]= SV_top[1][1];
		v[2][0]= SV_bot[0][0]+TBX*64;	v[2][1]= SV_bot[0][1];

		/*phong shaded image generation*/
	        phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_bot[0],*(long*)SV_top[1],*(long*)SV_bot[1]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[3].vx;
		nml[0].vy= NML_SD[3].vy;
		nml[0].vz= NML_SD[3].vz;
		nml[1].vx= NML_SD[0].vx;
		nml[1].vy= NML_SD[0].vy;
		nml[1].vz= NML_SD[0].vz;
		nml[2].vx= NML_SD[0].vx;
		nml[2].vy= NML_SD[0].vy;
		nml[2].vz= NML_SD[0].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_bot[0][0]+TBX*64;	v[0][1]= SV_bot[0][1];
		v[1][0]= SV_top[1][0]+TBX*64;	v[1][1]= SV_top[1][1];
		v[2][0]= SV_bot[1][0]+TBX*64;	v[2][1]= SV_bot[1][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_top[2],*(long*)SV_top[3],*(long*)SV_bot[2]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[0].vx;
		nml[0].vy= NML_SD[0].vy;
		nml[0].vz= NML_SD[0].vz;
		nml[1].vx= NML_SD[1].vx;
		nml[1].vy= NML_SD[1].vy;
		nml[1].vz= NML_SD[1].vz;
		nml[2].vx= NML_SD[0].vx;
		nml[2].vy= NML_SD[0].vy;
		nml[2].vz= NML_SD[0].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_top[2][0]+TBX*64;	v[0][1]= SV_top[2][1];
		v[1][0]= SV_top[3][0]+TBX*64;	v[1][1]= SV_top[3][1];
		v[2][0]= SV_bot[2][0]+TBX*64;	v[2][1]= SV_bot[2][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_bot[2],*(long*)SV_top[3],*(long*)SV_bot[3]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[0].vx;
		nml[0].vy= NML_SD[0].vy;
		nml[0].vz= NML_SD[0].vz;
		nml[1].vx= NML_SD[1].vx;
		nml[1].vy= NML_SD[1].vy;
		nml[1].vz= NML_SD[1].vz;
		nml[2].vx= NML_SD[1].vx;
		nml[2].vy= NML_SD[1].vy;
		nml[2].vz= NML_SD[1].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_bot[2][0]+TBX*64;	v[0][1]= SV_bot[2][1];
		v[1][0]= SV_top[3][0]+TBX*64;	v[1][1]= SV_top[3][1];
		v[2][0]= SV_bot[3][0]+TBX*64;	v[2][1]= SV_bot[3][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_top[4],*(long*)SV_top[5],*(long*)SV_bot[4]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[1].vx;
		nml[0].vy= NML_SD[1].vy;
		nml[0].vz= NML_SD[1].vz;
		nml[1].vx= NML_SD[2].vx;
		nml[1].vy= NML_SD[2].vy;
		nml[1].vz= NML_SD[2].vz;
		nml[2].vx= NML_SD[1].vx;
		nml[2].vy= NML_SD[1].vy;
		nml[2].vz= NML_SD[1].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_top[4][0]+TBX*64;	v[0][1]= SV_top[4][1];
		v[1][0]= SV_top[5][0]+TBX*64;	v[1][1]= SV_top[5][1];
		v[2][0]= SV_bot[4][0]+TBX*64;	v[2][1]= SV_bot[4][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_bot[4],*(long*)SV_top[5],*(long*)SV_bot[5]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[1].vx;
		nml[0].vy= NML_SD[1].vy;
		nml[0].vz= NML_SD[1].vz;
		nml[1].vx= NML_SD[2].vx;
		nml[1].vy= NML_SD[2].vy;
		nml[1].vz= NML_SD[2].vz;
		nml[2].vx= NML_SD[2].vx;
		nml[2].vy= NML_SD[2].vy;
		nml[2].vz= NML_SD[2].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_bot[4][0]+TBX*64;	v[0][1]= SV_bot[4][1];
		v[1][0]= SV_top[5][0]+TBX*64;	v[1][1]= SV_top[5][1];
		v[2][0]= SV_bot[5][0]+TBX*64;	v[2][1]= SV_bot[5][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_top[6],*(long*)SV_top[7],*(long*)SV_bot[6]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[2].vx;
		nml[0].vy= NML_SD[2].vy;
		nml[0].vz= NML_SD[2].vz;
		nml[1].vx= NML_SD[3].vx;
		nml[1].vy= NML_SD[3].vy;
		nml[1].vz= NML_SD[3].vz;
		nml[2].vx= NML_SD[2].vx;
		nml[2].vy= NML_SD[2].vy;
		nml[2].vz= NML_SD[2].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_top[6][0]+TBX*64;	v[0][1]= SV_top[6][1];
		v[1][0]= SV_top[7][0]+TBX*64;	v[1][1]= SV_top[7][1];
		v[2][0]= SV_bot[6][0]+TBX*64;	v[2][1]= SV_bot[6][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_bot[6],*(long*)SV_top[7],*(long*)SV_bot[7]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[2].vx;
		nml[0].vy= NML_SD[2].vy;
		nml[0].vz= NML_SD[2].vz;
		nml[1].vx= NML_SD[3].vx;
		nml[1].vy= NML_SD[3].vy;
		nml[1].vz= NML_SD[3].vz;
		nml[2].vx= NML_SD[3].vx;
		nml[2].vy= NML_SD[3].vy;
		nml[2].vz= NML_SD[3].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_bot[6][0]+TBX*64;	v[0][1]= SV_bot[6][1];
		v[1][0]= SV_top[7][0]+TBX*64;	v[1][1]= SV_top[7][1];
		v[2][0]= SV_bot[7][0]+TBX*64;	v[2][1]= SV_bot[7][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}
}

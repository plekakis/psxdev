/* $PSLibId: Runtime Library Release 3.6$ */
/*				
 *	TransRot... sample program
 *
 *	このプログラムは隣接したポリゴンが異なる座標系に属するとき
 *	に発生するスキマを解消するための関数TransRot...のサンプルです。
 *	ここでは、９つの正方形が辺の長さだけずらした座標系に置かれていると
 *	します。全体を含む座標系（ワールド）が回転していないとき、隣接する
 *	頂点の座標は一致するので９つの正方形の間にスキマは発生しません。
 *	しかし、ワールドが回転すると通常のRotTransPersなどでは、
 *	平行移動量を回転したものが別個に足されるため計算誤差により
 *	頂点が一致せずスキマが発生する場合があります。
 *	TransRot...関数は最初に平行移動量を足しておいて次に回転を
 *	くわえるためスキマが発生しません。
 *
 *	
 *	Lupボタン...	RotTransPersによるスキマあり
 *	Lleftボタン...	TransRotPersによるスキマなし
 *	Ldownボタン...	TransRotPers3によるスキマなし
 *	Lrightボタン...	TransRot_32によるスキマなし（32bit空間まで計算可）
 *
 * TransRot... sample program
 *
 * This program is a sample of the TransRot... function, which eliminates gaps
 * generated when adjacent polygons are in different coordinate systems.
 * In this case, nine cubes are placed in coordinate systems that are offset by the
 * length of the side of a cube. When the overall coordinate system (world) is not
 * rotating, the coordinates of adjacent vertices match up so that there are no
 * gaps between the cubes. However, if the world is rotated, RotTransPers will add
 * the rotated translation separately. The margin of error in
 * calculations can lead to vertices not matching up and gaps being created.
 * In the TransRot... function, translation is added first, and then
 * rotation is applied, thus preventing gaps from being generated.
 *
 *
 * Lup button... gaps created by RotTransPers 
 * Lleft button... no gaps from TransRotPers
 * Ldown button... no gaps from TransRotPers3
 * Lright button... no gaps from TransRot_32(can calculate up to a 32bit space)
 *
 *  Copyright (C) 1995 by Sony Computer Entertainment
 *   All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define SCR_Z	(512)			/* screen depth (h) */
#define	OTSIZE		(1024)		/* ordering table size */
#define OTLENGTH	10
#define CUBESIZ	5000			/* cube size */
#define ONE3	2364			/* ONE/sqrt(3) */
#define CODE_G3	(0x30)			/* G3 GPU code */

#define copySVECTOR(sv0,sv1)	(*(long*)&(sv0)->vx = *(long*)&(sv1)->vx), \
				(*(long*)&(sv0)->vz = *(long*)&(sv1)->vz)


typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_G3		s[9][2];			/* cube surface */
} DB;

static MATRIX	LLM = {
	-ONE3,-ONE3,-ONE3,
	-ONE3,ONE3,-ONE3,
	-ONE3,-ONE3,ONE3
};

static MATRIX	LCM = {
	ONE,0,0,
	0,ONE,0,
	0,0,ONE
};

static VECTOR	BK = {
	0x40,0x40,0x40
};

static VECTOR	FC = {
	60,120,120
};
int	FUNCTION=0;
	
/*
 *	
 *	Simple Tile Handler
 */	
static	SVECTOR	C0= {	0,	0,	0,0};
static	SVECTOR	C1= {	CUBESIZ,0,	0,0};
static	SVECTOR	C2= {	CUBESIZ,CUBESIZ,0,0};
static	SVECTOR	C3= {	0,	CUBESIZ,0,0};
static	SVECTOR	C4= {  -CUBESIZ,CUBESIZ,0,0};
static	SVECTOR	C5= {  -CUBESIZ,0,	0,0};
static	SVECTOR	C6= {  -CUBESIZ,-CUBESIZ,0,0};
static	SVECTOR	C7= {	0,	-CUBESIZ,0,0};
static	SVECTOR	C8= {	CUBESIZ,-CUBESIZ,0,0};

static	SVECTOR	*cen[9] = {&C0,&C1,&C2,&C3,&C4,&C5,&C6,&C7,&C8};
		
static SVECTOR P0 = {-CUBESIZ/2,-CUBESIZ/2,0,0};
static SVECTOR P1 = { CUBESIZ/2,-CUBESIZ/2,0,0};
static SVECTOR P2 = { CUBESIZ/2, CUBESIZ/2,0,0};
static SVECTOR P3 = {-CUBESIZ/2, CUBESIZ/2,0,0};

static pad_read(MATRIX*);
static init_prim(DB*);

main()
{
	DB		db[2];		/* packet double buffer */
	DB		*cdb;		/* current db */
	MATRIX		rottrans;	/* rot-trans matrix */
	int		i;		/* work */
	int		dmy, flg;	/* dummy */
	CVECTOR		col[12];	/* cube color */
	
	ResetCallback();
	PadInit(0);             /* initialize PAD */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(320, 240);	/* set geometry origin as (160, 120) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */

	SetLightMatrix(&LLM);
	SetColorMatrix(&LCM);
	SetBackColor(BK.vx,BK.vy,BK.vz);
	SetFarColor(FC.vx,FC.vy,FC.vz);
	SetFogNear(1*SCR_Z,SCR_Z);
	
	/* initialize environment for double buffer (interlace)
	 *	buffer ID	VRAM address 
	 *-------------------------------------------------------
	 *	buffer #0	(0,  0)-(640,480)
	 *	buffer #1	(0,  0)-(640,480)
	 */
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);	
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);	
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);	
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);
	
	FntLoad(960,256);
	SetDumpFnt(FntOpen(64, 64, 256, 200, 0, 512));

	/* set surface colors */
	for (i = 0; i < 12; i+=2) {
		col[i].r = col[i+1].r = 0xff/*rand()*/;	/* R */
		col[i].g = col[i+1].g = 0xff/*rand()*/;	/* G */
		col[i].b = col[i+1].b = 0xff/*rand()*/;	/* B */
		col[i].cd = col[i+1].cd = CODE_G3;	/* cd */
	}
	
	init_prim(&db[0]);	/* set primitive parameters on buffer #0 */
	init_prim(&db[1]);	/* set primitive parameters on buffer #1 */
	
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	while (pad_read(&rottrans) == 0) {
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */

		/* add tile */
		add_tile(cdb->ot, cdb->s, &rottrans,col);

		/* swap buffer */
		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
	
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

static pad_read(rottrans)
MATRIX	*rottrans;		/* rot-trans matrix, light matrix */
{
	static SVECTOR	ang  = { 0, 0, 0};	/* rotate angle */
	static VECTOR	vec  = {0, 0, 50*SCR_Z};/* rottranslate vector */
	MATRIX	llm;				/* Local Light Matrix */

	int	ret = 0;	
	u_long	padd = PadRead(1);
	
	/* rotate light source and cube */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* change distance */
	if (padd & PADl)	vec.vz += 128;
	if (padd & PADn) 	if(vec.vz>1000) vec.vz -= 128;

	if (padd & PADk) 	ret = -1;

	if (padd & PADLup)	FUNCTION=0;	/*RotTransPers*/
	if (padd & PADLleft) 	FUNCTION=1;	/*TransRotPers*/
	if (padd & PADLdown)	FUNCTION=2;	/*TransRotPers3*/
	if (padd & PADLright)	FUNCTION=3;	/*TransRot_32*/

	/* make matrix */
	RotMatrix(&ang, rottrans);	/* make rot-trans matrix */
	TransMatrix(rottrans, &vec);	

	/*set Local Light Matrix*/
	MulMatrix0(&LLM,rottrans,&llm);	/*destroy Rot Matrix*/
	SetLightMatrix(&llm);

	/* set Rotation Transfer Matrix */
	SetRotMatrix(rottrans);
	SetTransMatrix(rottrans);

	return(ret);
}		

/*
 *	initialize primitive parameters
 */
static init_prim(db)
DB	*db;
{
	int	i;
	
	db->draw.isbg = 1;
	setRGB0(&db->draw, 0, 0, 120);	/* (r,g,b) = (60,120,120) */

	for (i = 0; i < 9; i++) {
		SetPolyG3(&db->s[i][0]);
		SetPolyG3(&db->s[i][1]);
		db->s[i][0].r0= 200; db->s[i][0].g0= 200; db->s[i][0].b0= 200;
		db->s[i][0].r1= 150; db->s[i][0].g1= 150; db->s[i][0].b1= 150;
		db->s[i][0].r2= 100; db->s[i][0].g2= 100; db->s[i][0].b2= 100;
		db->s[i][1].r0= 200; db->s[i][1].g0= 200; db->s[i][1].b0= 200;
		db->s[i][1].r1= 150; db->s[i][1].g1= 150; db->s[i][1].b1= 150;
		db->s[i][1].r2= 100; db->s[i][1].g2= 100; db->s[i][1].b2= 100;
	}
}


add_tile(ot, s, rottrans, c)
u_long	*ot;
POLY_G3	*s;
MATRIX	*rottrans;
CVECTOR *c;
{
	int	i;
	int	otz, opz, clip;
	long	p,flg;
	SVECTOR	**vp;
	SVECTOR **np;
	MATRIX	m;
	int	p0,p1,p2;
	SVECTOR	sv[4];
	MATRIX	mat;
	MATRIX	irot;
	VECTOR	v0;
	VECTOR	lv[4];
	VECTOR	va[4];
	VECTOR	vv[4];
	long	sz;

	switch(FUNCTION){

	case 3:
	    FntPrint("TransRot_32\n");

	    copySVECTOR(&sv[0],&P0);
	    copySVECTOR(&sv[1],&P1);
	    copySVECTOR(&sv[2],&P2);
	    copySVECTOR(&sv[3],&P3);

	    /*copy to long VECTOR*/
	    va[0].vx= sv[0].vx; va[0].vy= sv[0].vy; va[0].vz= sv[0].vz;
	    va[1].vx= sv[1].vx; va[1].vy= sv[1].vy; va[1].vz= sv[1].vz;
	    va[2].vx= sv[2].vx; va[2].vy= sv[2].vy; va[2].vz= sv[2].vz;
	    va[3].vx= sv[3].vx; va[3].vy= sv[3].vy; va[3].vz= sv[3].vz;

	    /*inverse rotation of World transfer vector*/
	    ApplyTransposeMatrixLV(rottrans,(VECTOR*)rottrans->t,&v0);
	    SetRotMatrix(rottrans);

	    for (i=0; i<9; i++) {
		/*add inverse rotated World transfer vector*/
		mat.t[0]= v0.vx+cen[i]->vx;
		mat.t[1]= v0.vy+cen[i]->vy;
		mat.t[2]= v0.vz+cen[i]->vz;

		/*set Local transfer vector*/
		SetTransMatrix(&mat);

		/*Transfer&Rotate of 32bit vertices*/
		TransRot_32(&va[0],&lv[0],&flg);
		TransRot_32(&va[1],&lv[1],&flg);
		TransRot_32(&va[2],&lv[2],&flg);

		/*average screen Z of 3 vertices*/
		sz= (lv[0].vz+lv[1].vz+lv[2].vz)/3;

		if(sz>65536)FntPrint("SZ= %d\n",sz);

		/*32bit Perspective*/
		s->x0= 320+lv[0].vx*SCR_Z/lv[0].vz;
		s->y0= 240+lv[0].vy*SCR_Z/lv[0].vz;
		s->x1= 320+lv[1].vx*SCR_Z/lv[1].vz;
		s->y1= 240+lv[1].vy*SCR_Z/lv[1].vz;
		s->x2= 320+lv[2].vx*SCR_Z/lv[2].vz;
		s->y2= 240+lv[2].vy*SCR_Z/lv[2].vz;

		if((flg&0x00060000)==0){ 	
			sz >>=(18-OTLENGTH);
			if(sz>0&&sz<OTSIZE) AddPrim(ot+sz, s);
		}
		s++;

		/*Transfer&Rotate of 32bit vertices*/
		TransRot_32(&va[0],&lv[0],&flg);
		TransRot_32(&va[2],&lv[2],&flg);
		TransRot_32(&va[3],&lv[3],&flg);

		/*average screen Z of 3 vertices*/
		sz= (lv[0].vz+lv[2].vz+lv[3].vz)/3;

		/*32bit Perspective*/
		s->x0= 320+lv[0].vx*SCR_Z/lv[0].vz;
		s->y0= 240+lv[0].vy*SCR_Z/lv[0].vz;
		s->x1= 320+lv[2].vx*SCR_Z/lv[2].vz;
		s->y1= 240+lv[2].vy*SCR_Z/lv[2].vz;
		s->x2= 320+lv[3].vx*SCR_Z/lv[3].vz;
		s->y2= 240+lv[3].vy*SCR_Z/lv[3].vz;

		if((flg&0x00060000)==0){ 	
			sz>>=(18-OTLENGTH);
			if(sz>0&&sz<OTSIZE) AddPrim(ot+sz, s);
		}
		s++;
	    }
	    break;

	case 2:
	    FntPrint("TransRotPers3\n");

	    copySVECTOR(&sv[0],&P0);
	    copySVECTOR(&sv[1],&P1);
	    copySVECTOR(&sv[2],&P2);
	    copySVECTOR(&sv[3],&P3);

	    /*inverse rotation of World transfer vector*/
	    ApplyTransposeMatrixLV(rottrans,(VECTOR*)rottrans->t,&v0);
	    SetRotMatrix(rottrans);

	    for (i=0; i<9; i++) {
		/*add inverse rotated World transfer vector*/
		mat.t[0]= v0.vx+cen[i]->vx;
		mat.t[1]= v0.vy+cen[i]->vy;
		mat.t[2]= v0.vz+cen[i]->vz;

		/*set Local transfer vector*/
		SetTransMatrix(&mat);

		/*Transfer&Rotate 3 vertices*/ 
		otz=
		TransRotPers3(&sv[0],&sv[1],&sv[2],&s->x0,&s->x1,&s->x2,&p,&flg);

		if((flg&0x00060000)==0){ 	
			otz>>=(14-OTLENGTH);
			AddPrim(ot+otz, s);
		}
		s++;

		/*Transfer&Rotate 3 vertices*/ 
		otz=
		TransRotPers3(&sv[0],&sv[2],&sv[3],&s->x0,&s->x1,&s->x2,&p,&flg);

		if((flg&0x00060000)==0){ 	
			otz>>=(14-OTLENGTH);
			AddPrim(ot+otz, s);
		}
		s++;
	    }
	    break;

	case 1:
	    FntPrint("TransRotPers\n");

	    copySVECTOR(&sv[0],&P0);
	    copySVECTOR(&sv[1],&P1);
	    copySVECTOR(&sv[2],&P2);
	    copySVECTOR(&sv[3],&P3);
				
	    /*inverse rotation of World transfer vector*/
	    ApplyTransposeMatrixLV(rottrans,(VECTOR*)rottrans->t,&v0);
	    SetRotMatrix(rottrans);

	    for (i=0; i<9; i++) {
		/*add inverse rotated World transfer vector*/
		mat.t[0]= v0.vx+cen[i]->vx;
		mat.t[1]= v0.vy+cen[i]->vy;
		mat.t[2]= v0.vz+cen[i]->vz;

		/*set Local transfer vector*/
		SetTransMatrix(&mat);

		/*Transfer&Rotate 3 vertices*/ 
		otz=
		TransRotPers(&sv[0],&s->x0,&p,&flg);
		TransRotPers(&sv[1],&s->x1,&p,&flg);
		TransRotPers(&sv[2],&s->x2,&p,&flg);

		if((flg&0x00060000)==0){ 	
			otz>>=(14-OTLENGTH);
			AddPrim(ot+otz, s);
		}
		s++;

		/*Transfer&Rotate 3 vertices*/ 
		otz=
		TransRotPers(&sv[0],&s->x0,&p,&flg);
		TransRotPers(&sv[2],&s->x1,&p,&flg);
		TransRotPers(&sv[3],&s->x2,&p,&flg);

		if((flg&0x00060000)==0){ 	
			otz>>=(14-OTLENGTH);
			AddPrim(ot+otz, s);
		}
		s++;
	    }
	    break;

	case 0:
	    FntPrint("RotTransPers\n");

	    copySVECTOR(&sv[0],&P0);
	    copySVECTOR(&sv[1],&P1);
	    copySVECTOR(&sv[2],&P2);
	    copySVECTOR(&sv[3],&P3);

	    for (i=0; i<9; i++) {
		/*set World transfer vector*/
	        SetTransMatrix(rottrans);

		/*calculate Local transfer vector*/
		RotTrans(cen[i],(VECTOR*)mat.t,&flg); 

		/*set Local transfer vector*/
		SetTransMatrix(&mat);

		/*Rotate&Transfer 3 vertices*/
		otz=			
		RotTransPers(&sv[0],(long*)&s->x0,&p,&flg);
		RotTransPers(&sv[1],(long*)&s->x1,&p,&flg);
		RotTransPers(&sv[2],(long*)&s->x2,&p,&flg);

		if((flg&0x00060000)==0){ 	
			otz>>=(14-OTLENGTH);
			AddPrim(ot+otz, s);
		}
		s++;

		/*Rotate&Transfer 3 vertices*/
		otz=
		RotTransPers(&sv[0],(long*)&s->x0,&p,&flg);
		RotTransPers(&sv[2],(long*)&s->x1,&p,&flg);
		RotTransPers(&sv[3],(long*)&s->x2,&p,&flg);

		if((flg&0x00060000)==0){ 	
			otz>>=(14-OTLENGTH);
			AddPrim(ot+otz, s);
		}
		s++;
	    }
	    break;
	}
}

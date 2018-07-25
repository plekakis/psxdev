/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	high level objects in low level programs */
/*	 Version	Date		Design
 *	-----------------------------------------
 *	2.00		Aug,31,1993	masa	(original)
 *	2.10		Mar,25,1994	suzu	(added addPrimitive())
 *      2.20            Dec,25,1994     suzu	(chaned GsDOBJ4)
 *      2.30            Mar,05,1997     sachiko	(added autopad)
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define MODEL_ADDR	(u_long *)0x80100000	/* modeling data info. */
#define TEX_ADDR	(u_long *)0x80180000	/* texture info. */
	
#define SCR_Z		1000		/* projection */
#define OT_LENGTH	12		/* OT resolution */
#define OTSIZE		(1<<OT_LENGTH)	/* OT tag size */
#define PACKETMAX	4000		/* max number of packets per frame */
#define PACKETMAX2	(PACKETMAX*24)	/* average packet size is 24 */

PACKET	GpuPacketArea[2][PACKETMAX2];	/* packet area (double buffer) */
GsOT	WorldOT[2];			/* OT info */
SVECTOR	PWorld;			 	/* vector for making Coordinates */

GsOT_TAG	OTTags[2][OTSIZE];	/* OT tag */
GsDOBJ2		object;			/* object substance */
GsRVIEW2	view;			/* view point */
GsF_LIGHT	pslt[3];		/* lighting point */
u_long		PadData;		/* controller info. */
GsCOORDINATE2   DWorld;			/* Coordinate for GsDOBJ2 */

extern MATRIX GsIDMATRIX;

/*
 *  double buffer environment*/
typedef struct {
	DRAWENV		draw;			/* draw*/
	DISPENV		disp;			/* dislay*/
	GsOT 		gsot;			/* OT*/
	u_long		ot[OTSIZE];		/* OT tags*/
	PACKET		gsprim[PACKETMAX2];	/* primitive buffer */
} DB;

static void initSystem(DB *db);
static void initPrimitives();
static void drawObject(DB *db);
static int  moveObject(void);

/*
 *  main routine*/
void tuto1(void)
{
	DB	db[2];
	DB	*cdb;

	/* initialize*/
	initSystem(db);			/* grobal variables */
	initView();			/* position matrix */
	initLight();			/* light matrix */
	initModelingData(MODEL_ADDR);	/* load model data */
	initTexture(TEX_ADDR);		/* load texture pattern */
	initPrimitives();		/* GPU primitives */
	
	/* load font pattern*/
	FntLoad(960, 256);		
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	
	/* main loop*/	
	while(1) {
		cdb  = (cdb==db)? db+1: db;	/* swap double buffer ID */
		if ( moveObject() ) break;
		drawObject(cdb);
	}
	DrawSync(0);
	return;
}

/*
 *  3D object drawing procedure*/
static void drawObject(DB *cdb)
{
	MATRIX tmpls;
	
	/* Set GPU packet generation address to start of area */
	GsSetWorkBase(cdb->gsprim);
	
	/* clear contents of OT */
	ClearOTagR(cdb->ot, OTSIZE);
	
	/* register 3D object to OT*/
	GsGetLw(object.coord2,&tmpls);		
	GsSetLightMatrix(&tmpls);
	GsGetLs(object.coord2,&tmpls);
	GsSetLsMatrix(&tmpls);
	GsSortObject4(&object, &cdb->gsot,14-OT_LENGTH,getScratchAddr(0));
	
	/* append primitives*/
	addPrimitives(cdb->ot);
	
	/* read controler*/
	PadData = PadRead(0);

	/* wait for V-BLNK*/
	VSync(0);

	/* swap double buffer*/
	PutDispEnv(&cdb->disp);
	PutDrawEnv(&cdb->draw);

	/* If you use Gs functions without GsSwapDispBuff() , you must
	   Call GsIncFrame() */
	GsIncFrame();
	
	/* start drawing contents of OT as background */
	DrawOTag(cdb->ot+OTSIZE-1);
	FntFlush(-1);
	
	SetDispMask(1);
}

/*
 *  translate object using control pad */
static int moveObject(void)
{
	/* update local coordinate systems among object veriables */
	if(PadData & PADRleft)	PWorld.vy += 5*ONE/360;
	if(PadData & PADRright)	PWorld.vy -= 5*ONE/360;
	if(PadData & PADRup)	PWorld.vx -= 5*ONE/360;
	if(PadData & PADRdown)	PWorld.vx += 5*ONE/360;
	
	if(PadData & PADR1) DWorld.coord.t[2] += 200;
	if(PadData & PADR2) DWorld.coord.t[2] -= 200;
	
	/* Calculate Matrix from Object Parameter and Set Coordinate */
	set_coordinate(&PWorld,&DWorld);
	
	/* Clear flag of Coordinate for recalculation */
	DWorld.flg = 0;
	
	/* quit */
	if(PadData & PADselect) 
		return(-1);
	return(0);
}

/*
 *  initialization routines*/
static void initSystem(DB *db)
{
	int	i;
	
	/* control pad initialization*/
	PadData = 0;
	
	/* initialize environment*/
	GsInitGraph(320, 240, 0, 0, 0);	
	GsDefDispBuff(0, 0, 0, 0);
	
	/* initialize double buffer*/
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* initialize OT*/
	for (i = 0; i < 2; i++) {
		
		db[i].draw.isbg = 1;
		setRGB0(&db[i].draw, 0, 0, 0);

		db[i].gsot.length = OT_LENGTH;
		db[i].gsot.point  = 0;
		db[i].gsot.offset = 0;
		db[i].gsot.org    = (GsOT_TAG *)db[i].ot;
		db[i].gsot.tag    = (GsOT_TAG *)db[i].ot + OTSIZE - 1;
	}
	
	/* init 3D libs*/
	GsInit3D();
	
	/* move geometry origin to the center of the screen
	 * note that GPU offset is used.   */
	SetGeomOffset(160, 120);
}

/*
 *  set viewing position*/
initView()
{
	/* set projection*/
	GsSetProjection(SCR_Z);

	/* set viewing point*/
	view.vpx = 0; view.vpy = 0; view.vpz = -1000;
	view.vrx = 0; view.vry = 0; view.vrz = 0;
	view.rz = 0;
	view.super = WORLD;
	GsSetRefView2(&view);

	/* set Zclip value*/
	GsSetNearClip(100);
}

/*
 *  set light source (lighting direction and color) */
initLight()
{
	/* lighting in direction of light source #0(100,100,100) */
	pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
	pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
	GsSetFlatLight(0,&pslt[0]);
	
	/* light source #1 (not used)*/
	pslt[1].vx = 100; pslt[1].vy= 100; pslt[1].vz= 100;
	pslt[1].r=0; pslt[1].g=0; pslt[1].b=0;
	GsSetFlatLight(1,&pslt[1]);
	
	/* light source #2 (not used)*/
	pslt[2].vx = 100; pslt[2].vy= 100; pslt[2].vz= 100;
	pslt[2].r=0; pslt[2].g=0; pslt[2].b=0;
	GsSetFlatLight(2,&pslt[2]);
	
	/* set ambient*/
	GsSetAmbient(ONE/2,ONE/2,ONE/2);

	/* set light mode*/
	GsSetLightMode(0);
}

/*
 * read TMD data from memory and initialize object */
initModelingData(addr)
u_long *addr;
{
	u_long *tmdp;
	
	/* top address of TMD data*/
	tmdp = addr;			
	
	/* skip fie header*/
	tmdp++;				
	
	/* map to real address*/
	GsMapModelingData(tmdp);	
	
	tmdp++;		/* skip flag*/
	tmdp++;		/* skip number of objects*/
	
	GsLinkObject4((u_long)tmdp,&object,0);
	
	/* Init work vector */
        PWorld.vx=PWorld.vy=PWorld.vz=0;
	GsInitCoordinate2(WORLD, &DWorld);
	
	/* initialize 3D objects*/
	object.coord2 =  &DWorld;
	object.coord2->coord.t[2] = 4000;
	object.tmd = tmdp;		
	object.attribute = 0;
}

/*
 *  read texture data from memory*/
initTexture(addr)
u_long *addr;
{
	RECT rect1;
	GsIMAGE tim1;

	/* get TIM data info. */	
	/* skip and pass file header*/
	GsGetTimInfo(addr+1, &tim1);	

	/* transfer pixel data to VRAM*/	
	rect1.x=tim1.px;
	rect1.y=tim1.py;
	rect1.w=tim1.pw;
	rect1.h=tim1.ph;
	LoadImage(&rect1,tim1.pixel);

	/* if CLUT exists, transfer it to VRAM*/
	if((tim1.pmode>>3)&0x01) {
		rect1.x=tim1.cx;
		rect1.y=tim1.cy;
		rect1.w=tim1.cw;
		rect1.h=tim1.ch;
		LoadImage(&rect1,tim1.clut);
	}
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
  v1 = *pos;
  
  /* Rotate Matrix */
  RotMatrix(&v1,&tmp1);
  
  /* Set Matrix to Coordinate */
  coor->coord = tmp1;
  
  /* Clear flag becase of changing parameter */
  coor->flg = 0;
}

/*
 * initialize primitives*/
#include "balltex.h"
/* number of balls*/
#define NBALL	256		

/* ball scattering range*/
#define DIST	SCR_Z/4		

/* primitive buffer*/
POLY_FT4	ballprm[2][NBALL];	

/* position of ball*/
SVECTOR		ballpos[NBALL];		
	
static void initPrimitives()
{

	int		i, j;
	u_short		tpage, clut[32];
	POLY_FT4	*bp;	
		
	/* load ball's texture page */
	tpage = LoadTPage(ball16x16, 0, 0, 640, 256, 16, 16);

	/* load CLUT for balls*/
	for (i = 0; i < 32; i++)
		clut[i] = LoadClut(ballcolor[i], 256, 480+i);
	
	/* initialize primiteves*/
	for (i = 0; i < 2; i++)
		for (j = 0; j < NBALL; j++) {
			bp = &ballprm[i][j];
			SetPolyFT4(bp);
			SetShadeTex(bp, 1);
			bp->tpage = tpage;
			bp->clut = clut[j%32];
			setUV4(bp, 0, 0, 16, 0, 0, 16, 16, 16);
		}
	
	/* initialize positio*/
	for (i = 0; i < NBALL; i++) {
		ballpos[i].vx = (rand()%DIST)-DIST/2;
		ballpos[i].vy = (rand()%DIST)-DIST/2;
		ballpos[i].vz = (rand()%DIST)-DIST/2;
	}
}

/*
 * register primitives to OT*/
addPrimitives(ot)
u_long	*ot;
{
	static int	id    = 0;		/* buffer ID */
	static VECTOR	trans = {0, 0, SCR_Z};	/* world-screen vector */
	static SVECTOR	angle = {0, 0, 0};	/* world-screen angle */
	static MATRIX	rottrans;		/* world-screen matrix */
	int		i, padd;
	long		dmy, flg, otz;
	POLY_FT4	*bp;
	SVECTOR		*sp;
	SVECTOR		dp;
	
	
	id = (id+1)&0x01;	/* swap ID*/
	
	/* push current GTE matrix*/
	PushMatrix();		
	
	/* read controler and update world-screen matrix */
	padd = PadRead(1);

	if(padd & PADLup)    angle.vx -= 10;
	if(padd & PADLdown)  angle.vx += 10;
	if(padd & PADLright) angle.vy -= 10;
	if(padd & PADLleft)  angle.vy += 10;
	if(padd & PADL1) trans.vz += 50;
	if(padd & PADL2) trans.vz -= 50;
	
	RotMatrix(&angle, &rottrans);		/* rotate*/
	TransMatrix(&rottrans, &trans);		/* translate*/
	
	/* copy world-screen matrix ('rottrans') to current matrix */
	SetTransMatrix(&rottrans);	
	SetRotMatrix(&rottrans);
	
	/* update primitive members*/
	bp = ballprm[id];
	sp = ballpos;
	for (i = 0; i < NBALL; i++, bp++, sp++) {
		otz = RotTransPers(sp, (long *)&dp, &dmy, &flg);
		if (otz > 0 && otz < OTSIZE) {
			setXY4(bp, dp.vx, dp.vy,    dp.vx+16, dp.vy,
			           dp.vx, dp.vy+16, dp.vx+16, dp.vy+16);

			AddPrim(ot+otz, bp);
		}
	}

	/* recover old GTE matrix*/
	PopMatrix();
}




/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	rcube for overlay: PlayStation Demonstration program
 *
 *	"rcube.c" Main routine
 *
 *		Version 3.01			Jan, 28, 1994
 *		Version 3.01a	yoshi		Mar, 31, 1995
 *		Version 3.02			Jan, 9, 1995
 *		Version 3.02a	yoshi		Aug, 3, 1995
 *		Version 3.02b	yoshi		Mar, 4, 1996
 *
 *		Copyright (C) 1993,1994,1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *====================================================================
 */
/* This was rewritten as a child process. Compile conditionally with OVERLAY. */
#include <sys/types.h>
#include <sys/file.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libcd.h>
#include "table.h"
#include "pos.h"

/*
 * texture data */
#define TIM_ADDR 0x80108000		/* stored address of the TIM file to be used */
/*#define TIM_ADDR 0x80020000		/* stored address of the TIM file to be used */
#define TIM_HEADER 0x00000010

/*
 * modeling data */
#define TMD_ADDR 0x80100000		/* stored address of the TMD file to be used */
/*#define TMD_ADDR 0x80010000		/* stored address of the TMD file to be used */
u_long *TmdBase;			/* address of object within TMD */
int CurrentTmd; 			/* the TMD number for the TMD being used */

/*
 * ordering table (OT) */
#define OT_LENGTH  7			/* OT resolution (large) */
GsOT WorldOT[2];			/* OT data (double buffer) */
GsOT_TAG OTTags[2][1<<OT_LENGTH];	/* OT tag area (double buffer) */

/*
 * GPU packet creation area */
#define PACKETMAX 1000			/* maximum packet number in 1 frame */
PACKET GpuPacketArea[2][PACKETMAX*64];	/* packet area (double buffer) */

/*
 *  object (cube) variable */
#define NCUBE 44
#define OBJMAX NCUBE
int nobj;				/* number of cubes */
GsDOBJ2 object[OBJMAX];			/* 3D object variable */
GsCOORDINATE2 objcoord[OBJMAX];		/* local coordinate variable */
SVECTOR Rot[OBJMAX];			/* rotation angle */
SVECTOR RotV[OBJMAX];			/* rotation speed (angular velocity) */
VECTOR Trns[OBJMAX];			/* cube position (parallel displacement) */
VECTOR TrnsV[OBJMAX];			/* displacement speed */

/*
 *  VIEW (viewpoint) */
GsRVIEW2  View;				/* view point*/
int ViewAngleXZ;			/* height of view point*/
int ViewRadial;				/* distance from view point*/
#define DISTANCE 600			/* initial value of Radial*/

/*
 *  light source */
GsF_LIGHT pslt[3];			/* light source data variable x 3 */

/*
 *  other */
int Bakuhatu;				/* explosion processing flag */
u_long PadData;				/* controller pad data */
u_long oldpad;				/* controller pad data from previous frame */
GsFOGPARAM dq;				/* parameter for depth queue (fog) */
int dqf;				/* check if fog is ON */
int back_r, back_g, back_b;		/* background color */
#define FOG_R 160
#define FOG_G 170
#define FOG_B 180

/*
 *  function prototype declaration */
void drawCubes();
int moveCubes();
void initModelingData();
void allocCube();
void initSystem();
void initAll();
void initTexture();
void initView();
void initLight();
void changeFog();
void changeTmd();
int	datafile_search();
int	datafile_read();

/*
 *  file information */
typedef struct{
	char	*fname;
	void	*addr;
	CdlFILE finfo;
} FILE_INFO;

#define DFILENO 2

static FILE_INFO dfile[DFILENO] = {
	{ "\\DATA\\RCUBE.TMD;1",(void *)TMD_ADDR,0 },
	{ "\\DATA\\RCUBE.TIM;1",(void *)TIM_ADDR,0 } 
};

/*
 *  main routine */
#ifdef OVERLAY
child_rcube()
#else
main()
#endif
{
	RECT rct;

#ifdef OVERLAY
	/* some tricks are used to allow smooth screen transitions */
	VSync(0);
	SetDispMask(0);
	ResetGraph(1);
	setRECT(&rct,0,0,1024,512);
	ClearImage(&rct,0,0,0);
	DrawSync(0);
#else
	ResetCallback();
	CdInit();
	PadInit(0);
#endif

	datafile_search(dfile,DFILENO);
	datafile_read(dfile,DFILENO);

	/* initialize system */
	initSystem();

	/* other intializations */
	Bakuhatu = 0;
	PadData = 0;
	CurrentTmd = 0;
	dqf = 0;
	back_r = back_g = back_b = 0;
	initView();
	initLight(0, 0xc0);
	initModelingData(TMD_ADDR);
	initTexture(TIM_ADDR);
	allocCube(NCUBE);
	
	/* main loop */
	while(1) {
		if(moveCubes())
			break;
		GsSetRefView2(&View);
		drawCubes();
	}

	return(0);
}


/*
 *  draw 3D object */
void drawCubes()
{
	int i;
	GsDOBJ2 *objp;
	int activeBuff;
	MATRIX LsMtx;

	/* which of the double buffers are active? */
	activeBuff = GsGetActiveBuff();

	/* set GPU packet creation address to the start of the area */
	GsSetWorkBase((PACKET*)GpuPacketArea[activeBuff]);

	/* clear OT contents */
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* enter 3D object (cube) into OT */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* rotation angle -> set in matrix */
		RotMatrix(Rot+i, &(objp->coord2->coord));
		
		/* reset flag since matrix has been updated */
                objp->coord2->flg = 0;

		/* translation capacity -> set in matrix */
                TransMatrix(&(objp->coord2->coord), &Trns[i]);
		
		/* calculate matrix for perspective transformation and set in GTE */
		GsGetLs(objp->coord2, &LsMtx);
		GsSetLsMatrix(&LsMtx);
		GsSetLightMatrix(&LsMtx);

		/* perform perspective transformation and enter in OT */
		GsSortObject4(objp, &WorldOT[activeBuff], 14-OT_LENGTH,getScratchAddr(0));
		objp++;
	}

    /* include pad data in buffer */
	oldpad = PadData;
	PadData = PadRead(1);
	
	/* wait for V-BLNK */
	VSync(0);
	
	/* forcibly stop drawing operation for previous frame */
	ResetGraph(1);

	/* swap double buffer */
	GsSwapDispBuff();

	/* insert screen clear command at start of OT */
	GsSortClear(back_r, back_g, back_b, &WorldOT[activeBuff]);

	/* begin drawing OT contents in background */
	GsDrawOt(&WorldOT[activeBuff]);
}

/*
 *  move cube */
int moveCubes()
{
	int i;
	GsDOBJ2   *objp;
	
	/* process according to pad value */
	if((PadData & PADLleft)>0) {
		ViewAngleXZ++;
		if(ViewAngleXZ >= 72) {
			ViewAngleXZ = 0;
		}
	}
	if((PadData & PADLright)>0) {
		ViewAngleXZ--;
		if(ViewAngleXZ < 0) {
		  ViewAngleXZ = 71;
		}
	}
	if((PadData & PADLup)>0) View.vpy += 100;
	if((PadData & PADLdown)>0) View.vpy -= 100;
	if((PadData & PADRdown)>0) {
		ViewRadial-=3;
		if(ViewRadial < 8) {
			ViewRadial = 8;
		}
	}
	if((PadData & PADRright)>0) {
		ViewRadial+=3;
		if(ViewRadial > 450) {
			ViewRadial = 450;
		}
	}
	if((PadData & PADk)>0) return(-1);
	if(((PadData & PADRleft)>0)&&((oldpad&PADRleft) == 0)) changeFog();
	if(((PadData & PADRup)>0)&&((oldpad&PADRup) == 0)) changeTmd();
	if(((PadData & PADn)>0)&&((oldpad&PADn) == 0)) Bakuhatu = 1;
	if(((PadData & PADl)>0)&&((oldpad&PADl) == 0)) allocCube(NCUBE);

	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;

	/* update position data for cube */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* begin explosion */
		if(Bakuhatu == 1) {

			/* increase rotation speed */
			RotV[i].vx *= 3;
			RotV[i].vy *= 3;
			RotV[i].vz *= 3;

			/* set direction of displacement and velocity */
			TrnsV[i].vx = objp->coord2->coord.t[0]/4+
						(rand()-16384)/200;	
			TrnsV[i].vy = objp->coord2->coord.t[1]/6+
						(rand()-16384)/200-200;
			TrnsV[i].vz = objp->coord2->coord.t[2]/4+
						(rand()-16384)/200;
		}
		/* processing for during explosion */
		else if(Bakuhatu > 1) {
			if(Trns[i].vy > 3000) {
				Trns[i].vy = 3000-(Trns[i].vy-3000)/2;
				TrnsV[i].vy = -TrnsV[i].vy*6/10;
			}
			else {
				TrnsV[i].vy += 20*2;	/* free fall */
			}

			if((TrnsV[i].vy < 70)&&(TrnsV[i].vy > -70)&&
			   (Trns[i].vy > 2800)) {
				Trns[i].vy = 3000;
				TrnsV[i].vy = 0;

				RotV[i].vx *= 95/100;
				RotV[i].vy *= 95/100;
				RotV[i].vz *= 95/100;
			}


			TrnsV[i].vx = TrnsV[i].vx*97/100;
			TrnsV[i].vz = TrnsV[i].vz*97/100;
		}

		/* update rotation angle (rotation) */
 		Rot[i].vx += RotV[i].vx;
		Rot[i].vy += RotV[i].vy;
		Rot[i].vz += RotV[i].vz;

		/* update translation (Transfer) */
 		Trns[i].vx += TrnsV[i].vx;
		Trns[i].vy += TrnsV[i].vy;
		Trns[i].vz += TrnsV[i].vz;

		objp++;
	}

	if(Bakuhatu == 1)
		Bakuhatu++;

	return(0);
}

/*
 *  set cube to initial position */
void allocCube(n)
int n;
{	
	int x, y, z;
	int i;
	int *posp;
	GsDOBJ2 *objp;
	GsCOORDINATE2 *coordp;

	posp = cube_def_pos;
	objp = object;
	coordp = objcoord;
	nobj = 0;
	for(i = 0; i < NCUBE; i++) {

		/* intialize object struture */
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		GsInitCoordinate2(WORLD, coordp);
		objp->coord2 = coordp;
		objp->attribute = 0;
		coordp++;
		objp++;

		/* set initial position (read from pos.h) */
		Trns[i].vx = *posp++;
		Trns[i].vy = *posp++;
		Trns[i].vz = *posp++;
		Rot[i].vx = 0;
		Rot[i].vy = 0;
		Rot[i].vz = 0;

		/* initialize velocity */
		TrnsV[i].vx = 0;
		TrnsV[i].vy = 0;
		TrnsV[i].vz = 0;
		RotV[i].vx = rand()/300;
		RotV[i].vy = rand()/300;
		RotV[i].vz = rand()/300;

		nobj++;
	}
	Bakuhatu = 0;
}

/*
 *  initialize function group */
void initSystem()
{
	int i;

	/* initialize pad */
	PadInit(0);

	/* initialize graphics */
	GsInitGraph(640, 480, 2, 0, 0);
	GsDefDispBuff(0, 0, 0, 0);

	/* initialize OT */
	for(i = 0; i < 2; i++) {	
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}	

	/* initialize 3D system */
	GsInit3D();
}

void initModelingData(tmdp)
u_long *tmdp;
{
	u_long size;
	int i;
	int tmdobjnum;
	
	/* skip header */
	tmdp++;

	/* mapping to read address */
	GsMapModelingData(tmdp);

	tmdp++;
	tmdobjnum = *tmdp;
	tmdp++; 		/* point to object at beginning */
	TmdBase = tmdp;
}

/*
 *  read in texture (transfer to VRAM) */
void initTexture(tex_addr)
u_long *tex_addr;
{
	RECT rect1;
	GsIMAGE tim1;
	int i;
	
	while(1) {
		if(*tex_addr != TIM_HEADER) {
			break;
		}
		tex_addr++;	/* skip header (1word) */
		GsGetTimInfo(tex_addr, &tim1);
		tex_addr += tim1.pw*tim1.ph/2+3+1;	/* proceed up to next block */
		rect1.x=tim1.px;
		rect1.y=tim1.py;
		rect1.w=tim1.pw;
		rect1.h=tim1.ph;
		LoadImage(&rect1,tim1.pixel);
		if((tim1.pmode>>3)&0x01) {	/* if ther is a CLUT, then transfer */
			rect1.x=tim1.cx;
			rect1.y=tim1.cy;
			rect1.w=tim1.cw;
			rect1.h=tim1.ch;
			LoadImage(&rect1,tim1.clut);
			tex_addr += tim1.cw*tim1.ch/2+3;
		}
	}
}

/*
 *  intialize viewpoint */
void initView()
{
	/* set viewpoint variable as initial position */
	ViewAngleXZ = 54;
	ViewRadial = 75;
	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpy = -100;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;
	View.vrx = 0; View.vry = 0; View.vrz = 0;
	View.rz=0;

	/* parent coordinates of viewpoint */
	View.super = WORLD;

	/* set */
	GsSetRefView2(&View);

	/* Projection */
	GsSetProjection(1000);

	/* mod = normal lighting */
	GsSetLightMode(0);
}

/*
 *  initialize light source */
void initLight(c_mode, factor)
int c_mode;	/* if 0, white light; if 1, cocktail lights */
int factor;	/* brightness factor (0 - 255) */
{
	if(c_mode == 0) {
		/* set white light */
		pslt[0].vx = 200; pslt[0].vy= 200; pslt[0].vz= 300;
		pslt[0].r = factor; pslt[0].g = factor; pslt[0].b = factor;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -50; pslt[1].vy= -1000; pslt[1].vz= 0;
		pslt[1].r=0x20; pslt[1].g=0x20; pslt[1].b=0x20;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
		pslt[2].r=0x0; pslt[2].g=0x0; pslt[2].b=0x0;
		GsSetFlatLight(2,&pslt[2]);
	}
	else {
		/* cocktail lights (using Gouraud) */
		pslt[0].vx = 200; pslt[0].vy= 100; pslt[0].vz= 0;
		pslt[0].r = factor; pslt[0].g = 0; pslt[0].b = 0;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -200; pslt[1].vy= 100; pslt[1].vz= 0;
		pslt[1].r=0; pslt[1].g=0; pslt[1].b=factor;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = 0; pslt[2].vy= -200; pslt[2].vz= 0;
		pslt[2].r=0; pslt[2].g=factor; pslt[2].b=0;
		GsSetFlatLight(2,&pslt[2]);
	}	

	/* ambient light */
	GsSetAmbient(ONE/2,ONE/2,ONE/2);
}

/*
 * fog ON/OFF */
void changeFog()
{
	if(dqf) {
		/* reset fog */
		GsSetLightMode(0);
		dqf = 0;
		back_r = 0;
		back_g = 0;
		back_b = 0;
	}
	else {
		/* set fog */
		dq.dqa = -600;
		dq.dqb = 5120*4096;
		dq.rfc = FOG_R;
		dq.gfc = FOG_G;
		dq.bfc = FOG_B;
		GsSetFogParam(&dq);
		GsSetLightMode(1);
		dqf = 1;
		back_r = FOG_R;
		back_g = FOG_G;
		back_b = FOG_B;
	}
}

/*
 *  switch TMD data */
void changeTmd()
{
	u_long *tmdp;
	GsDOBJ2 *objp;
	int i;

	/* switch TMD */
	CurrentTmd++;
	if(CurrentTmd == 4) {
		CurrentTmd = 0;
	}
	objp = object;
	for(i = 0; i < nobj; i++) {
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		objp++;
	}

	/* switch color/brightness of light source according to TMD type */
	switch(CurrentTmd) {
	    case 0:
                /* normal (flat) */
		initLight(0, 0xc0);
		break;
	    case 1:
	        /* semi-transparent (flat) */
		initLight(0, 0xc0);
		break;
	    case 2:
	        /* Gouraud */
		initLight(1, 0xff);
		break;
	    case 3:
	        /* with texture */
		initLight(0, 0xff);
		break;
	}
}



int
datafile_search(file,nf)

FILE_INFO *file;
int nf;
{
	short i,j;

	for (j = 0; j < nf; j++){
		for (i = 0; i < 10; i++) {	/* ten retries */
			if (CdSearchFile( &(file[j].finfo), file[j].fname ) != 0) 
				break;
			else
				printf("%s not find.\n",file[j].fname);
		}
	}
}


int
datafile_read(file,nf)

FILE_INFO *file;
int nf;
{
	int	mode = CdlModeSpeed;	
	int	nsector;
	short i,j;
	long cnt;
	unsigned char com;

	for (j = 0; j < nf; j++){
		for (i = 0; i < 10; i++) {	/* ten retries */
			nsector = (file[j].finfo.size + 2047) / 2048;
		
			/* set target position */
			CdControl(CdlSetloc, (u_char *)&(file[j].finfo.pos), 0);

			com = CdlModeSpeed;
			CdControlB( CdlSetmode, &com, 0 );
			VSync( 3 );

			/* begin read */
			CdRead(nsector, file[j].addr, mode);
	
			/* normal operations can be performed behind the read operation.
			   sector count is monitored here until Read is finished */
			while ((cnt = CdReadSync(1, 0)) > 0 ) {
				VSync(0);
			}
		
			/* break if normal exit */
			if (cnt == 0)	break;
		}
	}
}

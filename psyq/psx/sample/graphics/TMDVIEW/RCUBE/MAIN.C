/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	rcube: PS-X Demonstration program
 *
 *	"main.c" Main routine
 *
 *		Version 3.02	Jan, 9, 1995
 *
 *		Copyright (C) 1993,1994,1995 by Sony Computer Entertainment
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "table.h"
#include "pos.h"

/* Texture information  */
#define TIM_ADDR 0x80020000		/* Address where target TIM file is stored  */

#define TIM_HEADER 0x00000010

/* Modeling data information  */
#define TMD_ADDR 0x80010000		/* Address where target TMD file is stored  */

u_long *TmdBase;			/* Address of object in TMD */

int CurrentTmd; 			/* TMD no. in use */

/* Ordering table (OT) */
#define OT_LENGTH  7			/* OT resolution (size) */
GsOT WorldOT[2];			/* OT info (double buffer) */
GsOT_TAG OTTags[2][1<<OT_LENGTH];	/* OT's tag area (double buffer) */

/* GPU packet generation area  */
#define PACKETMAX 1000			/* Maximum number of packets per frame */

PACKET GpuPacketArea[2][PACKETMAX*64];	/* Packet area (double buffer)  */

/*  Object (cube) variable */
#define NCUBE 44
#define OBJMAX NCUBE
int nobj;				/* Number of cubes */
GsDOBJ2 object[OBJMAX];			/* 3D object variable */
GsCOORDINATE2 objcoord[OBJMAX];		/* Local coordinate variable  */

SVECTOR Rot[OBJMAX];			/* Rotation angle */
SVECTOR RotV[OBJMAX];			/* Rotation speed (angular velocity) */

VECTOR Trns[OBJMAX];			/* Cube position (translatio distance) */

VECTOR TrnsV[OBJMAX];			/* Translation speed */

/*  Viewpoint (VIEW) */
GsRVIEW2  View;			/* Viewpoint variable */
int ViewAngleXZ;		/* Viewpoint height */
int ViewRadial;			/* Distance from viewpoint */
#define DISTANCE 600		/* Initial value of radial */

/*  Light source */
GsF_LIGHT pslt[3];			/* Light source information variable x3 */

/*  Other */
int Bakuhatu;				/* Explosion processing flag */
u_long PadData;				/* Control pad information */
u_long oldpad;				/* Pad information from previous frame */
GsFOGPARAM dq;				/* Parameter for depth cueing (FOG) */
int dqf;				/* FOG ON/OFF */
int back_r, back_g, back_b;		/* Background color */

#define FOG_R 160
#define FOG_G 170
#define FOG_B 180

/*  Function prototype declaration  */
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

/*  Main routine */
main()
{
	/* System initialization */
	ResetCallback();
	initSystem();

	/* Other initialization */
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
	
	/* Main loop */
	while(1) {
		if(moveCubes()==0)
		  return 0;
		GsSetRefView2(&View);
		drawCubes();
	}
}


/*  Draw 3D object (cube) */
void drawCubes()
{
	int i;
	GsDOBJ2 *objp;
	int activeBuff;
	MATRIX LsMtx;

	/* Which double buffer is active */
	activeBuff = GsGetActiveBuff();

	/* Set GPU packet generation address to start of area */
	GsSetWorkBase((PACKET*)GpuPacketArea[activeBuff]);

	/* Clear contents of OT */
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* Register 3D object (cube to OT) */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* Set Rotation angle to matrix */
		RotMatrix(Rot+i, &(objp->coord2->coord));
		
		/* Reset flag after matrix has been updated */
		objp->coord2->flg = 0;

		/* Set Translation distance to matrix */
		TransMatrix(&(objp->coord2->coord), &Trns[i]);
		
		/* Calculate matrix for perspective transformation and set to GTE */
		GsGetLs(objp->coord2, &LsMtx);
		GsSetLsMatrix(&LsMtx);
		GsSetLightMatrix(&LsMtx);

		/* Perform perspective transformation and register to OT */
		GsSortObject4(objp, &WorldOT[activeBuff], 14-OT_LENGTH,getScratchAddr(0));
		objp++;
	}

	/* Fetch pad contents for buffer */
	oldpad = PadData;
	PadData = PadRead(1);
	
	/* Wait for V-BLANK */
 	VSync(0);
	
	/* Force termination of previous frame's drawing operation */
	ResetGraph(1);

	/* Replace double buffer */
	GsSwapDispBuff();

	/* Insert screen clear command at start of OT */
	GsSortClear(back_r, back_g, back_b, &WorldOT[activeBuff]);

	/* Start drawing contents of OT as background */
	GsDrawOt(&WorldOT[activeBuff]);
}

/* Cube translation */
int moveCubes()
{
	int i;
	GsDOBJ2   *objp;
	
	/* exit program */
/*	if((PadData & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}*/
	if((PadData & PADk)>0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	
	/* Process according to pad value */
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

	/* Update cube's position information */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* Start explosion */
		if(Bakuhatu == 1) {

			/* Auto-rotation speed */
			RotV[i].vx *= 3;
			RotV[i].vy *= 3;
			RotV[i].vz *= 3;

			/* Set translation direction and speed */
			TrnsV[i].vx = objp->coord2->coord.t[0]/4+
						(rand()-16384)/200;	
			TrnsV[i].vy = objp->coord2->coord.t[1]/6+
						(rand()-16384)/200-200;
			TrnsV[i].vz = objp->coord2->coord.t[2]/4+
						(rand()-16384)/200;
		}
		/* Processing during explosion */
		else if(Bakuhatu > 1) {
			if(Trns[i].vy > 3000) {
				Trns[i].vy = 3000-(Trns[i].vy-3000)/2;
				TrnsV[i].vy = -TrnsV[i].vy*6/10;
			}
			else {
				TrnsV[i].vy += 20*2;	/* Free fall */
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

		/* Update rotation angle */
 		Rot[i].vx += RotV[i].vx;
		Rot[i].vy += RotV[i].vy;
		Rot[i].vz += RotV[i].vz;

		/* Upadte translation distance */
 		Trns[i].vx += TrnsV[i].vx;
		Trns[i].vy += TrnsV[i].vy;
		Trns[i].vz += TrnsV[i].vz;

		objp++;
	}

	if(Bakuhatu == 1)
		Bakuhatu++;

	return(1);
}

/*	Map cube to initial position */
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

		/* Initialize object structure */
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		GsInitCoordinate2(WORLD, coordp);
		objp->coord2 = coordp;
		objp->attribute = 0;
		coordp++;
		objp++;

		/* Set initial position (read from pos.h) */
		Trns[i].vx = *posp++;
		Trns[i].vy = *posp++;
		Trns[i].vz = *posp++;
		Rot[i].vx = 0;
		Rot[i].vy = 0;
		Rot[i].vz = 0;

		/* Initialize speed */
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

/*	Initialization functions */
void initSystem()
{
	int i;

	/* Initialize pad */
	PadInit(0);

	/* Initialize graphic */
	GsInitGraph(640, 480, 2, 0, 0);
	GsDefDispBuff(0, 0, 0, 0);

	/* Initialize OT */
	for(i = 0; i < 2; i++) {	
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}	

	/* Initialize 3D system */
	GsInit3D();
}

void initModelingData(tmdp)
u_long *tmdp;
{
	u_long size;
	int i;
	int tmdobjnum;
	
	/* Skip header */
	tmdp++;

	/* Mapping to real address */
	GsMapModelingData(tmdp);

	tmdp++;
	tmdobjnum = *tmdp;
	tmdp++; 		/* Point to top object */
	TmdBase = tmdp;
}

/*	Read texture (transfer to VRAM) */
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
		tex_addr++;	/* Skip header (1word) */
		GsGetTimInfo(tex_addr, &tim1);
		tex_addr += tim1.pw*tim1.ph/2+3+1;	/* Go to next block  */

		rect1.x=tim1.px;
		rect1.y=tim1.py;
		rect1.w=tim1.pw;
		rect1.h=tim1.ph;
		LoadImage(&rect1,tim1.pixel);
		if((tim1.pmode>>3)&0x01) {	/* Transfer CLUT (if there are any)  */

			rect1.x=tim1.cx;
			rect1.y=tim1.cy;
			rect1.w=tim1.cw;
			rect1.h=tim1.ch;
			LoadImage(&rect1,tim1.clut);
			tex_addr += tim1.cw*tim1.ch/2+3;
		}
	}
}

/*	Initialize viewpoint */
void initView()
{
	/* Set initial position as viewpoint variable */
	ViewAngleXZ = 54;
	ViewRadial = 75;
	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpy = -100;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;
	View.vrx = 0; View.vry = 0; View.vrz = 0;
	View.rz=0;

	/* Super coordinates of viewpoint */
	View.super = WORLD;

	/* Setting */
	GsSetRefView2(&View);

	/* Projection */
	GsSetProjection(1000);

	/* Mode = 'normal lighting' */
	GsSetLightMode(0);
}

/*	Initialize light source  */
void initLight(c_mode, factor)
int c_mode;	/* Light color = wight when 0, cocktail light when 1 */
int factor;	/* Intensity factor (0 to 255) */
{
	if(c_mode == 0) {
		/* Set White light color */
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
		/* Cocktail light (used for Gouraud)  */
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

	/* Ambient (peripheral) light */
	GsSetAmbient(ONE/2,ONE/2,ONE/2);
}

/* FOG on/off */
void changeFog()
{
	if(dqf) {
		/* Reset FOG */
		GsSetLightMode(0);
		dqf = 0;
		back_r = 0;
		back_g = 0;
		back_b = 0;
	}
	else {
		/* Set Fog */
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

/*	Switch TMD data */
void changeTmd()
{
	u_long *tmdp;
	GsDOBJ2 *objp;
	int i;

	/* Switch TMD */
	CurrentTmd++;
	if(CurrentTmd == 4) {
		CurrentTmd = 0;
	}
	objp = object;
	for(i = 0; i < nobj; i++) {
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		objp++;
	}

	/* Switch light source color/intensity according to type of TMD */
	switch(CurrentTmd) {
	    case 0:
                /* Normal (flat) */
		initLight(0, 0xc0);
		break;
	    case 1:
	        /* Opaque (flat) */
		initLight(0, 0xc0);
		break;
	    case 2:
	        /* Gouraud */
		initLight(1, 0xff);
		break;
	    case 3:
	        /* Textured */
		initLight(0, 0xff);
		break;
	}
}


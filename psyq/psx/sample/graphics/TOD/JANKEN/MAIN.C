/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* 
 * 	janken: sample program using TOD data
 * 
 *	"main.c" main routine
 *
 * 		Version 2.00	Feb, 3, 1995
 * 		Version 2.01	Mar, 8, 1997
 * 
 * 		Copyright (C) 1994, 1995 by Sony Computer Entertainment
 * 		All rights Reserved
 */

/*****************/
/* Include files */
/*****************/
#include <sys/types.h>
#include <libetc.h>		/* needs to be included to use PAD*/
#include <libgte.h>		/* needs to be included to use LIGS*/
#include <libgpu.h>		/* needs to be included to use LIGS*/
#include <libgs.h>		/* definitions of structures, etc. for use of graphics library*/
#include "tod.h"		/* declaration section for TOD handling functions */
#include "te.h"			/* TMD-ID list (created with rsdlink) */

/**********/
/* For OT */
/**********/
#define OT_LENGTH	12		 /* 12-bit OT resolution (size) */
GsOT		WorldOT[2];		 /* OT data (double buffering) */
GsOT_TAG	OTTags[2][1<<OT_LENGTH]; /* OT tag area (double buffering) */

/***************/
/* For Objects */
/***************/
#define OBJMAX	50			/* maximum number of objects*/
GsDOBJ2		obj_area[OBJMAX];	/* object variable area*/
GsCOORDINATE2	obj_coord[OBJMAX];	/* local coordinate variable area*/
GsCOORD2PARAM	obj_cparam[OBJMAX];	/* local coordinate variable area*/
TodOBJTABLE	objs;			/* object table*/

/***************/
/* For Packets */
/***************/
#define PACKETMAX	6000		/* maximum packet size for one frame */
#define PACKETMAX2	(PACKETMAX*24)	/* average size of one packet is assumed to be to 24 bytes*/
PACKET	GpuPacketArea[2][PACKETMAX2];	/* packet region (double buffer) */

/***************/
/* For Viewing */
/***************/
#define VPX	-8000
#define VPY	-2000
#define VPZ	-5000
#define VRX	-900
#define VRY	-1500
#define VRZ	0
GsRVIEW2	view;			/* viewpoint information*/

/************************/
/* For Lights ( three ) */
/************************/
GsF_LIGHT	pslt[3];		/* light source x 3*/

/*********************/
/* For Modeling data */
/*********************/
#define MODEL_ADDR	0x800c0000
u_long		*TmdP;			/* modeling data pointer*/

/**********************/
/* For Animation data */
/**********************/
#define TOD_ADDR0	0x800e0000
#define TOD_ADDR1	0x80100000
#define TOD_ADDR2	0x80120000
#define TOD_ADDR3	0x80140000
#define TOD_ADDR4	0x80160000
#define TOD_ADDR5	0x80180000
#define TOD_ADDR6	0x801a0000
#define TOD_ADDR7	0x801c0000
#define TOD_ADDR8	0x801e0000
#define NUMTOD		9
u_long		*TodP[NUMTOD];		/* animation data pointer matrix*/
int		NumFrame[NUMTOD];	/* animation data frame count matrix*/
int		StartFrameNo[NUMTOD];	/* animation starting frame number matrix*/

/******************/
/* For Controller */
/******************/
u_long		padd;

/*******************/
/* For Janken type */
/*******************/
int		pose;

/*************************/
/* Prototype Definitions */
/*************************/
int	exitProgram();
void	drawObject();
void	initSystem();
void	initView();
void	initLight();
void	initModelingData();
void	initTod();
void	initPose();
int	obj_interactive();

/*
 * Main routine */
main()
{
    /* Initialize  */
    ResetCallback();
    initSystem();
    initView();
    initLight();
    initModelingData();
    initTod();
    initPose();
    
    FntLoad(960, 256);
    SetDumpFnt(FntOpen(32-320, 32-120, 256, 200, 0, 512));

    while( 1 ) {

	/* Read the pad data */
	padd = PadRead( 1 );
	/* play TOD data according to the pad data */
	if(obj_interactive()) break;
    }

    PadStop();
    ResetGraph(3);
    StopCallback();
    return 0;
}

/*
 * Draw 3D objects */
void
drawObject()
{
    int		i;
    int		activeBuff;
    GsDOBJ2	*objp;
    MATRIX	LsMtx;
    int         load_count1,load_count2,load_count3,load_org;

    activeBuff = GsGetActiveBuff();

    /* Set the address at which drawing command will be put */
    GsSetWorkBase( (PACKET*)GpuPacketArea[activeBuff] );

    /* Initialize OT  */
    GsClearOt( 0, 0, &WorldOT[activeBuff] );

    /* Set the pointer to the object array  */
    objp = objs.top;

    load_count1=load_count2=load_count3=0;
    for( i = 0; i < objs.nobj; i++ ) { 

	/* flag whether the coord has changed or not */
	objp->coord2->flg = 0;

	if ( ( objp->id != TOD_OBJ_UNDEF ) && ( objp->tmd != 0 ) ) {

	    /* Calculate the local screen matrix  */
	    load_org = VSync(1);
	    GsGetLs( objp->coord2, &LsMtx );
	    load_count1+= VSync(1)-load_org;
	    /* Set the local screen matrix to GTE  */
	    GsSetLsMatrix( &LsMtx );
	    
	    /* Set the light matrix to GTE  */
	    GsSetLightMatrix( &LsMtx );
	    
	    /* Transform the object perspectively and assign it to the OT */
	    load_org = VSync(1);
	    GsSortObject4( objp, 		/* Pointer to the object */
			   &WorldOT[activeBuff],/* Pointer to the OT */
			   14-OT_LENGTH,getScratchAddr(0) );	/* number of bits to be shifted*/
	    load_count2+= VSync(1)-load_org;
	}

	objp++;
    }
    FntPrint("%d %d\n",load_count1,load_count2);

    VSync( 0 );				/* Wait for V-BLNK */
    ResetGraph( 1 );			/* Cancel the current display */
    GsSwapDispBuff();			/* Switch the double buffer */

    /* Resister the clear command to OT  */
    GsSortClear( 0x0,				/* R of the background */
		 0x0,				/* G of the background */
		 0x0,				/* B of the background */
		 &WorldOT[activeBuff] );	/* Pointer to the OT */

    /* Do the drawing commands assigned to the OT  */
    GsDrawOt( &WorldOT[activeBuff] );
    FntFlush(-1);
}

/*
 * Initialize system */
void
initSystem()
{
  /* Initialize the controll pad */
  PadInit( 0 );
  padd = 0;
	
  /* Initialize the GPU  */
  GsInitGraph( 640, 240, GsOFSGPU|GsNONINTER, 1, 0 );
  GsDefDispBuff( 0, 0, 0, 240 );
	
  /* Initialize the OT  */
  WorldOT[0].length = OT_LENGTH;
  WorldOT[0].org = OTTags[0];
  WorldOT[0].offset = 0;
  WorldOT[1].length = OT_LENGTH;
  WorldOT[1].org = OTTags[1];
  WorldOT[1].offset = 0;
	
  /* Initialize the 3D library  */
  GsInit3D();
}

/*
 * Set the location of the view point */
void
initView()
{
    /* Set the view angle  */
    GsSetProjection( 1000 );

    /* Set the view point and the reference point  */
    view.vpx = VPX; view.vpy = VPY; view.vpz = VPZ;
    view.vrx = VRX; view.vry = VRY; view.vrz = VRZ;
    view.rz = 0;
    view.super = WORLD;
    GsSetRefView2( &view );

    /* Set the value of Z clip  */
    GsSetNearClip( 100 );
}

/*
 * Set the light ( the direction and the color ) */
void
initLight()
{
    /* Light #0 , direction (100,100,100)  */
    pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
    pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
    GsSetFlatLight( 0,&pslt[0] );
	
    /* Light #1 ( not used )  */
    pslt[1].vx = 100; pslt[1].vy= 100; pslt[1].vz= 100;
    pslt[1].r=0; pslt[1].g=0; pslt[1].b=0;
    GsSetFlatLight( 1,&pslt[1] );
	
    /* Light #2 ( not used )  */
    pslt[2].vx = 100; pslt[2].vy= 100; pslt[2].vz= 100;
    pslt[2].r=0; pslt[2].g=0; pslt[2].b=0;
    GsSetFlatLight( 2,&pslt[2] );
	
    /* Set the ambient light  */
    GsSetAmbient( ONE/2,ONE/2,ONE/2 );

    /* Set the light mode  */
    GsSetLightMode( 0 );
}

/*
 * Get TMD data from the memory ( use only the top one ) */
void
initModelingData()
{
    /* Top address of the TMD data  */
    TmdP = ( u_long * )MODEL_ADDR;

    /* Skip the file header  */
    TmdP++;

    /* Map to the real address  */
    GsMapModelingData( TmdP );

    /* Initialize the object table  */
    TodInitObjTable( &objs, obj_area, obj_coord, obj_cparam, OBJMAX );
}


/*
 * Set TOD data from the memory */
void
initTod()
{
	TodP[0] = ( u_long * )TOD_ADDR0;
	TodP[0]++;
	NumFrame[0] = *TodP[0]++;
	StartFrameNo[0] = *( TodP[0] + 1 );

	TodP[1] = ( u_long * )TOD_ADDR1;
	TodP[1]++;
	NumFrame[1] = *TodP[1]++;
	StartFrameNo[1] = *( TodP[1] + 1 );

	TodP[2] = ( u_long * )TOD_ADDR2;
	TodP[2]++;
	NumFrame[2] = *TodP[2]++;
	StartFrameNo[2] = *( TodP[2] + 1 );

	TodP[3] = ( u_long * )TOD_ADDR3;
	TodP[3]++;
	NumFrame[3] = *TodP[3]++;
	StartFrameNo[3] = *( TodP[3] + 1 );

	TodP[4] = ( u_long * )TOD_ADDR4;
	TodP[4]++;
	NumFrame[4] = *TodP[4]++;
	StartFrameNo[4] = *( TodP[4] + 1 );

	TodP[5] = ( u_long * )TOD_ADDR5;
	TodP[5]++;
	NumFrame[5] = *TodP[5]++;
	StartFrameNo[5] = *( TodP[5] + 1 );

	TodP[6] = ( u_long * )TOD_ADDR6;
	TodP[6]++;
	NumFrame[6] = *TodP[6]++;
	StartFrameNo[6] = *( TodP[6] + 1 );

	TodP[7] = ( u_long * )TOD_ADDR7;
	TodP[7]++;
	NumFrame[7] = *TodP[7]++;
	StartFrameNo[7] = *( TodP[7] + 1 );

	TodP[8] = ( u_long * )TOD_ADDR8;
	TodP[8]++;
	NumFrame[8] = *TodP[8]++;
	StartFrameNo[8] = *( TodP[8] + 1 );
}

/*
 * Draw the initial pose */
void
initPose()
{
    int		i;
    int		j;
    int		frameNo;
    int		oldFrameNo;

    /* Process the first frame of TOD data*/
    TodP[0] = TodSetFrame( StartFrameNo[0], TodP[0], &objs, te_list, 
			   TmdP, TOD_CREATE );
    drawObject();
    oldFrameNo = StartFrameNo[0];

    /* Process following frames of TOD data*/
    for ( i = 1 ; i < NumFrame[0] ; i++ ) {

	frameNo = *( TodP[0] + 1 );

	for ( j = 0 ; j < frameNo - oldFrameNo - 1 ; j++ ) {
	    drawObject();
	}

	/* Process 1 frame of TOD data*/
	TodP[0] = TodSetFrame( frameNo, TodP[0], &objs, te_list, 
			       TmdP, TOD_CREATE );
	drawObject();
	oldFrameNo = frameNo;
    }
    drawObject();

    pose = -1;
}

/*
 * play TOD data according to the pad data  */
int obj_interactive()
{
    int		i;
    u_long	*TodPtmp;		/* pointer to TOD data  */
    int		step;
    
    /* "rock" () */
    if ( ( padd & PADRright ) > 0 ) {

	/* go from current pose to initial fixed position*/
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* move fist up and down */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* go from fixed position to "rock" */
	TodPtmp = TodP[2];
	for ( i = StartFrameNo[2] ; i < NumFrame[2] + StartFrameNo[2] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	pose = 2;
    }

    /* "paper" */
    if ( ( padd & PADRleft ) > 0 ) {

	/* go from current pose to initial fixed position*/
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* move fist up and down */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* go from fixed position to "paper" */
	TodPtmp = TodP[4];
	for ( i = StartFrameNo[4] ; i < NumFrame[4] + StartFrameNo[4] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	pose = 4;
    }

    /* "scissors" */
    if ( ( padd & PADRup ) > 0 ) {

	/* go from current pose to initial fixed position*/
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* move fist up and down */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* go from fixed position to "scissors" */
	TodPtmp = TodP[3];
	for ( i = StartFrameNo[3] ; i < NumFrame[3] + StartFrameNo[3] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	pose = 3;
    }

    /* randomly choose from "rock", "paper", and "scissors" */
    if ( ( padd & PADRdown ) > 0 ) {

	/* go from current pose to initial fixed position*/
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* move fist up and down */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* go from fixed position to "rock", "paper", or "scissors" */
	pose = rand()%3 + 2;
	TodPtmp = TodP[pose];
	for ( i = StartFrameNo[pose] 
	      ; i < NumFrame[pose] + StartFrameNo[pose] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();
    }

    /* change viewpoint*/
    if ( ( padd & PADn ) > 0 ) {
	view.vpz += 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADo ) > 0 ) {
	view.vpz -= 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLleft ) > 0 ) {
	view.vpx -= 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLright ) > 0 ) {
	view.vpx += 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLdown ) > 0 ) {
	view.vpy += 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLup ) > 0 ) {
	view.vpy -= 100;
	GsSetRefView2( &view );
	drawObject();
    }
    
    /*  restore viewpoint  Move the view point to the initial place */
    if ( ( padd & PADh ) > 0 ) {
	initView();
	drawObject();
    }

    /* Exit the program */
    if ( ( padd & PADk ) > 0 ) {

	/* return to initial pose*/
	if ( pose > 0 ) {

	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* restore viewpoint to initial position*/
	step = ( ( VPX - view.vpx ) * ( VPX - view.vpx )
		 + ( VPY - view.vpy ) * ( VPY - view.vpy )
		 + ( VPZ - view.vpz ) * ( VPZ - view.vpz ) ) / 500000;
	if ( step > 50 ) {
	    step = 50;
	}
	for ( i = 1 ; i <= step ; i++ ) {
	    view.vpx = view.vpx + ( i * ( VPX - view.vpx ) ) / step;
	    view.vpy = view.vpy + ( i * ( VPY - view.vpy ) ) / step;
	    view.vpz = view.vpz + ( i * ( VPZ - view.vpz ) ) / step;
	    GsSetRefView2( &view );
	    drawObject();
	}

	/* exit*/
	TodPtmp = TodP[8];
	for ( i = StartFrameNo[8] ; i < NumFrame[8] + StartFrameNo[8] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* end of program*/
	return -1;
    }

    return 0;
}

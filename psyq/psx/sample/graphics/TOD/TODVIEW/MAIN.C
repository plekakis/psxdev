/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* 
 * 	todview: sample program to view TOD data
 * 
 *	"main.c" simple TOD viewing routine
 *
 * 		Version 2.10	Apr, 17, 1996
 * 		Version 2.20	Mar, 08, 1997
 * 
 * 		Copyright (C) 1994, 1995 by Sony Computer Entertainment
 * 		All rights Reserved
 */

/*****************/
/* Include files */
/*****************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>	

#include "tod.h"		/* definition for TOD functions */
#include "te.h"			/* TMD ID list */

/**********/
/* For OT */
/**********/
#define OT_LENGTH	12		 /* 12-bit OT resolution (size) */
GsOT		WorldOT[2];		 /* OT data (double buffering) */
GsOT_TAG	OTTags[2][1<<OT_LENGTH]; /* OT tag area (double buffering) */

/***************/
/* For Objects */
/***************/
#define OBJMAX	50			/* Maximum number of object */
GsDOBJ2		obj_area[OBJMAX];	/* DOBJ area for object table */
GsCOORDINATE2	obj_coord[OBJMAX];	/* local coordinate area */
GsCOORD2PARAM	obj_cparam[OBJMAX];	/* RST parameter for coordinate */
TodOBJTABLE	objs;			/* object table */

/***************/
/* For Packets */
/***************/
#define PACKETMAX	6000		/* Maximum number of packet */
#define PACKETMAX2	(PACKETMAX*24)	/* average packet size */
PACKET	GpuPacketArea[2][PACKETMAX2];	/* Packet area for libgs */

/***************/
/* For Viewing */
/***************/
#define VPX	-8000
#define VPY	-2000
#define VPZ	-5000
#define VRX	-900
#define VRY	-1500
#define VRZ	0
GsRVIEW2	view;

/************************/
/* For Lights ( three ) */
/************************/
GsF_LIGHT	pslt[3];

/*********************/
/* For Modeling data */
/*********************/
#define MODEL_ADDR	0x800c0000
u_long		*TmdP;

/**********************/
/* For Animation data */
/**********************/
#define TOD_ADDR	0x800e0000 /* addr for TOD data */
u_long	*TodP;		/* animation data pointer matrix*/
int	NumFrame;	/* animation data frame count matrix*/
int 	StartFrameNo;	/* animation starting frame number array*/

/******************/
/* For Controller */
/******************/
u_long		padd;

/*
 * Main routine */
main()
{
    /* Initialize  */
    ResetCallback();
    initSystem();
    initView();
    initLight();
    initModelingData( MODEL_ADDR );
    initTod();

    /* Execute first TOD frame */
    drawTod();

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
drawObject()
{
    int		i;
    int		activeBuff;
    GsDOBJ2	*objp;
    MATRIX	LsMtx;	

    activeBuff = GsGetActiveBuff();

    /* Set the address at which drawing command will be put */
    GsSetWorkBase( (PACKET*)GpuPacketArea[activeBuff] );

    /* Initialize OT  */
    GsClearOt( 0, 0, &WorldOT[activeBuff] );

    /* Set the pointer to the object array  */
    objp = objs.top;

    for( i = 0; i < objs.nobj; i++ ) { 

	/* flag whether the coord has changed or not */
	objp->coord2->flg = 0;

	if ( ( objp->id  != TOD_OBJ_UNDEF ) && ( objp->tmd != 0 ) ) {

	    /* Calculate the local screen matrix  */
	    GsGetLs( objp->coord2, &LsMtx );

	    /* Set the local screen matrix to GTE  */
	    GsSetLsMatrix( &LsMtx );

	    /* Set the light matrix to GTE  */
	    GsSetLightMatrix( &LsMtx );

	    /* Transform the object perspectively and assign it to the OT */
	    GsSortObject4( objp, 		/* Pointer to the object */
			   &WorldOT[activeBuff],/* Pointer to the OT */
			   14-OT_LENGTH,	/* number of bits to be shifted*/
			   getScratchAddr(0));
	}

	objp++;
    }

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
}

/*
 * Initialize system */
initSystem()
{
  /* Initialize the controll pad*/
  PadInit( 0 );
  padd = 0;
	
  /* Initialize the GPU*/
  GsInitGraph( 640, 240, 0, 1, 0 );
  GsDefDispBuff( 0, 0, 0, 240 );
	
  /* Initialize the OT*/
  WorldOT[0].length = OT_LENGTH;
  WorldOT[0].org = OTTags[0];
  WorldOT[0].offset = 0;
  WorldOT[1].length = OT_LENGTH;
  WorldOT[1].org = OTTags[1];
  WorldOT[1].offset = 0;
	
  /* Initialize the 3D library*/
  GsInit3D();
}

/*
 * Set the location of the view point */
initView()
{
    /* Set the view angle*/
    GsSetProjection( 1000 );

    /* Set the view point and the reference point*/
    view.vpx = VPX; view.vpy = VPY; view.vpz = VPZ;
    view.vrx = VRX; view.vry = VRY; view.vrz = VRZ;
    view.rz = 0;
    view.super = WORLD;
    GsSetRefView2( &view );

    /* Set the value of Z clip*/
    GsSetNearClip( 100 );
}

/*
 * Set the light ( the direction and the color ) */
initLight()
{
    /* Light #0 , direction (100,100,100)*/
    pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
    pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
    GsSetFlatLight( 0,&pslt[0] );
	
    /* Light #1 ( not used ) */
    pslt[1].vx = 100; pslt[1].vy= 100; pslt[1].vz= 100;
    pslt[1].r=0; pslt[1].g=0; pslt[1].b=0;
    GsSetFlatLight( 1,&pslt[1] );
	
    /* Light #2 ( not used ) */
    pslt[2].vx = 100; pslt[2].vy= 100; pslt[2].vz= 100;
    pslt[2].r=0; pslt[2].g=0; pslt[2].b=0;
    GsSetFlatLight( 2,&pslt[2] );
	
    /* Set the ambient light */
    GsSetAmbient( ONE/2,ONE/2,ONE/2 );

    /* Set the light mode */
    GsSetLightMode( 0 );
}

/*
 * Get TMD data from the memory ( use only the top one ) */
initModelingData( addr )
u_long	*addr;
{
    /* Top address of the TMD data */
    TmdP = addr;

    /* Skip the file header */
    TmdP++;

    /* Map to the real address */
    GsMapModelingData( TmdP );

    /* Initialize the object table */
    TodInitObjTable( &objs, obj_area, obj_coord, obj_cparam, OBJMAX );
}


/*
 * Set TOD data from the memory */
initTod()
{
    TodP = ( u_long * )TOD_ADDR;
    TodP++;
    NumFrame = *TodP++;
    StartFrameNo = *( TodP + 1 );
}

/*
 * play TOD data according to the pad data  */
obj_interactive()
{
    int		i;

    if ( ( ( padd & PADRright ) > 0 ) ||
	 ( ( padd & PADRleft )  > 0 ) || 
	 ( ( padd & PADRup )    > 0 ) ||
	 ( ( padd & PADRdown )  > 0 ) ) {

	drawTod();
    }

    /* Change viewpoint*/
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
    
    /* Move the view point to the initial place */
    if ( ( padd & PADh ) > 0 ) {
	initView();
	drawObject();
    }

    /* Exit the program */
    if ( ( padd & PADk ) > 0 ) {
	return -1;
    }

    return 0;
}


drawTod()
{
    int		i;
    int		j;
    u_long	*TodPtmp;
    int		frameNo;
    int		oldFrameNo;

    TodPtmp = TodP;
    TodPtmp = TodSetFrame( StartFrameNo, TodPtmp, &objs, te_list,
			   TmdP, TOD_CREATE );
    drawObject();

    oldFrameNo = StartFrameNo;

    for ( i = StartFrameNo + 1 ; i < NumFrame + StartFrameNo ; i++ ) {
	    frameNo = *( TodPtmp + 1 );

        for ( j = 0 ; j < frameNo - oldFrameNo - 1 ; j++ ) {
            drawObject();
        }

        TodPtmp = TodSetFrame( frameNo, TodPtmp, &objs, te_list,
			       TmdP, TOD_CREATE );
        drawObject();

	oldFrameNo = frameNo;
    }
    drawObject();
}


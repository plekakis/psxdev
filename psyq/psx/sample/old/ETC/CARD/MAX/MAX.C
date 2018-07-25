/* $PSLibId: Run-time Library Release 4.4$ */
/*****************************************************************
 *
 * file: max.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include "libps.h"

#include "maxtypes.h"

#include "psxload.h"
#include "spritex.h"
#include "menux.h"

#ifdef EMOUSE
#include "emouse.h"
#endif /* EMOUSE */

extern void SetMenuMovement();
extern void funcCardS(long pad);

extern  int	IconCount[];
extern  SPRTX	IconSprites[2][15];
extern  int	ExitFlag;

#include "maxproto.h"

u_long	padd, opadd = 0;

#include "maxstat.h"
MAXSTAT	maxstat;

DB	db[2];		/* packet double buffer */
DB	*cdb=0L;	/* current db */

int   fot;
int   MESH_PATTERN;
int   JAPAN_R;

void main( void )
{
        int i;

	InitHeap( 0x80100000, 0x80000 ); /**/

	if( malloc_superx() < 0 ) {
		printf("Can't malloc in superx.lib\n");
	}
	if( malloc_turtle() < 0 ) {
		printf("Can't malloc in turtle.lib\n");
	}

	SetVideoMode(MODE_NTSC);

	ResetCallback();

#ifdef MESH
	MESH_PATTERN = 1;
#else
	MESH_PATTERN = 0;
#endif /* MESH */

#ifdef JAPAN
	JAPAN_R = 1;
#else
	JAPAN_R = 0;
#endif /* JAPAN */

	ResetGraph(0);
	SetGraphDebug(0);       /* set debug mode (0:off, 1:monitor, 2:dump) */

/******************************************************
  Initialize
******************************************************/

	/************************************
	  Initialize Devices
	************************************/
	padd = _PADi;
	opadd = _PADj;

#ifdef EMOUSE
	EMouseInit();
#else
	PadInit(0);
#endif /* EMOUSE */

	/* root counter for randomize proc */
	SetRCnt( RCntCNT2, 4096, RCntMdINTR );
	StartRCnt( RCntCNT2 );

	/************************************
	  Initialize Graphics
	************************************/
	InitGeom();		/* initialize geometry subsystem */


/******************************************************
  OSD
******************************************************/
	max();

/******************************************************
  Ending
******************************************************/
#ifndef EMOUSE
	PadStop();
#endif /* EMOUSE */

	free_superx();
	free_turtle();

	PadStop();
	VSync(0);
	ResetGraph(3);
	StopCallback();

	return;
}

/******************************************************
  OSD Sub Routine
******************************************************/
void max( void )
{
	int	i, j;

#ifdef EMOUSE
	ClearTexture();
#endif /* EMOUSE */    

	{ /* randomize proc */
		int r;
		r = GetRCnt(RCntCNT2);
#ifdef DEBUG
		printf( "random seed is %d\n", r );
#endif /* DEBUG */
		while ( r-- ) rand();
	}
	SetRCnt( RCntCNT2, 0, RCntMdNOINTR );
	StopRCnt( RCntCNT2 );

	max_init( db );

	/************************************
	  Initialize Memory Cards
	************************************/
	IconCount[0] = 0;
	IconCount[1] = 0;
	funcCardS( _PADRright );

	SetDispMask(1);	/* enable to display (0:inhibit, 1:enable) */

	VSNopen();  /* background drawing */

	/******************************************************
	  Main Loop
	******************************************************/
	while ( pad_read() == 0 ) {
		extern ScreenMode;

		/********************************************
		  Setup Image Datas
		********************************************/

#ifdef EMOUSE
		if ( maxstat.pageNumber > -1 && EMouse_get_exitanime() == 0) {
#else
		if ( maxstat.pageNumber > -1 ) {
#endif /* EMOUSE */
		do_max_cube( db, &cdb, 0 );
		DrawDB( cdb );
		do_max_cube( db, &cdb, 0 );
		DrawDB( cdb );
		SetUpPageItem( maxstat.pageNumber );
		maxstat.pageNumber = -1;
		}
		/********************************************
		  Drawing Image Datas
		********************************************/
		else {
#ifdef EMOUSE
		/**********************************
        	  Mouse / PAD
		**********************************/
		EMouseIN( cdb ) ;
#endif /* EMOUSE */
		/**********************************
		  GCube
		**********************************/
		do_max_cube( db, &cdb, 1 );

		funcMemCard( padd );
		CopyMemCard();

		/**********************************
		  Menu Item
		**********************************/
		MENUdrawItemAll( cdb );

		/**********************************
		  Other Sprites
		**********************************/
		for ( i = 0; i < maxstat.spriteCount; i++ ) {
			if( maxstat.Sprites[fot][i].priority == (OTSIZE-2) ) continue;
			AddSprite2( cdb, &maxstat.Sprites[fot][i] );
		}

#ifdef EMOUSE
		/**********************************
		  Mouse / PAD
		**********************************/
		EMouseDISP( cdb ) ;
#endif /* EMOUSE */

		/**********************************
		  Drawing All
		**********************************/
		DrawDB( cdb );
		}
	}

	return;
}

void mmemcpy(char *dst, char *src, int size)
{
	while (size--) {
		*dst++ = *src++;
	}
}

/* $PSLibId: Run-time Library Release 4.4$ */
/*****************************************************************
 *
 * file: maxinit.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
#include "libps.h"

#include "maxtypes.h"
#include "maxproto.h"

#include "psxload.h"
#include "spritex.h"

#include "maxstat.h"
extern MAXSTAT	maxstat;

#define SCR_Z		(512)			/* screen depth (h) */

#define TW              108
#define TH              35

#define SPW             120
#define SPH             120
#define SPX		60
#define SPY		60
#define SPR		59

#define SetColor(C,R,G,B)	(((C)->r=(R)),((C)->g=(G)),((C)->b=(B)))
static CVECTOR  ocolor = { BGR, BGG, BGB };     /* oval outer color */
static CVECTOR  ccolor = { RFC, GFC, BFC };     /* oval center color */

void max_init( DB *db )
{
	int	i;
	CVECTOR	col1, col2;

	SetGeomOffset( OFX, OFY );	/* set geometry origin as (160, 120) */
	SetGeomScreen( SCR_Z );		/* distance to viewing-screen */
	SetBackColor( RBK, GBK, BBK );	/* set background(ambient) color*/
	SetFarColor( RFC, GFC, BFC );	/* set far color */

	PSXsetInterlace( 1 );
	SetDefDrawEnv( &db[0].draw, 0,   0, PIH, PIV);
	SetDefDrawEnv( &db[1].draw, 0,   0, PIH, PIV);
	SetDefDispEnv( &db[0].disp, 0,   ENVOFFSETY, PIH, PIV-ENVOFFSETY);
	SetDefDispEnv( &db[1].disp, 0,   ENVOFFSETY, PIH, PIV-ENVOFFSETY);

	if( GetVideoMode() ) {
	    db[0].disp.screen.x = db[1].disp.screen.x = PALOFFSETX;
	    db[0].disp.screen.y = db[1].disp.screen.y = PALOFFSETY;
	    db[0].disp.screen.w = db[1].disp.screen.w = 0;
	    db[0].disp.pad0 = db[1].disp.pad0 = 1;
	}
	
	db[0].disp.screen.h = db[1].disp.screen.h = (PIV-ENVOFFSETY)/2;

	db[0].draw.dfe = db[1].draw.dfe = 1;

	PutDrawEnv( &db[0].draw );
	PutDrawEnv( &db[1].draw ); /**/

	PutDispEnv( &db[0].disp );
	PutDispEnv( &db[1].disp ); /**/

	/* big cow pattern */
	make_cow_pattern((u_long*)( _COW0 ), TW, TH, TW*TH/16 );
	make_cow_pattern((u_long*)( _COW1 ), TW, TH, TW*TH/16 );
	make_cow_pattern((u_long*)( _COW2 ), TW, TH, TW*TH/16 );
	make_cow_pattern((u_long*)( _COW3 ), TW, TH, TW*TH/16 );
	make_cow_pattern((u_long*)( _COW4 ), TW, TH, TW*TH/16 );
	make_cow_pattern((u_long*)( _COW5 ), TW, TH, TW*TH/16 );

	/* background blue oval */
	make_sphere((u_long*)( _SPH0 ), OVAL_ROT32,
	            SPW, SPH, SPX, SPY, SPR, -12, -12, &ocolor, &ccolor );

	/* green ball */
	SetColor( &col1, 1, 26, 13 );
	SetColor( &col2, 2, 255, 143 );
	make_sphere((u_long*)( _SPH1 ), OVAL_ROT16,
	            56, 56, 28, 28, 26, -10, -10, &col1, &col2 );

	/* blue ball */
	SetColor( &col1, 1, 1, 140 );
	SetColor( &col2, 1, 1, 255 );
	make_sphere((u_long*)( _SPH2 ), OVAL_ROT16,
	            56, 56, 28, 28, 26, -10, -10, &ocolor, &ccolor );
	/* red ball */
	SetColor( &col1, 45, 1, 1 );
	SetColor( &col2, 255, 1, 1 );
	make_sphere((u_long*)( _SPH3 ), OVAL_ROT16,
	            56, 56, 28, 28, 26, -10, -10, &col1, &col2 );
	/* orange ball */
	SetColor( &col1, 45, 30, 1 );
	SetColor( &col2, 255, 204, 1 );
	make_sphere((u_long*)( _SPH4 ), OVAL_ROT16,
	            56, 56, 28, 28, 26, -10, -10, &col1, &col2 );
	/* yellow ball */
	SetColor( &col1, 45, 45, 1 );
	SetColor( &col2, 255, 255, 1 );
	make_sphere((u_long*)( _SPH5 ), OVAL_ROT16,
	            56, 56, 28, 28, 26, -10, -10, &col1, &col2 );

	/* purple ball */
	SetColor( &col1, 23, 5, 45 );
	SetColor( &col2, 153, 37, 255 );
	make_sphere((u_long*)( _SPH6 ), OVAL_ROT32,
	            88, 88, 44, 44, 43, -12, -12, &col1, &col2 );

	/* plate for strings */
	SetColor( &col1, 62, 62, 140 );
	make_square((u_long*)( _SPH8 ), 140, 28, &col1 );

	init_prim( &db[0] );	/* set primitive parameters on buffer #0 */
	init_prim( &db[1] );	/* set primitive parameters on buffer #1 */

	/* clear screen */
	PutDrawEnv( &db[0].draw );
	DrawPrim( (u_long *)&db[0].bg );
	PutDrawEnv( &db[1].draw );
	DrawPrim( (u_long *)&db[1].bg );

	db[0].draw.dfe = db[1].draw.dfe = 0;
	PutDrawEnv( &db[0].draw );
	PutDrawEnv( &db[1].draw ); /**/

}

/*
 *	initialize primitive parameters
 */
void init_prim( DB *db )
{
	long	i,j,k;

	SetPolyF4(&db->bg);
	setXYWH( &db->bg, db->draw.clip.x, db->draw.clip.y, PIH, PIV );
	setRGB0(&db->bg, BGR, BGG, BGB);

	make_oval_prim(
	  &db->oval, OVAL_ROT16,
	  PIH/2, PIV/2, PIH/2-30, PIH/2-30, 0, 0, &ocolor, &ccolor );

}

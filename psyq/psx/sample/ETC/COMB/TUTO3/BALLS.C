/* $PSLibId: Run-time Library Release 4.4$ */
#include <stdio.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <rand.h>
#include "balltex.h"

#define	PIH		320
#define	PIV		240
#define BALLMAX		32
/************************************************************** memoryes ***/
static SPRT_16 sprtBalls[2][BALLMAX];
static short vx[BALLMAX], vy[BALLMAX];
static short tpage;
static DR_MODE drmode[2];
/************************************************************* procedure ***/
void _make_balls_data( void )
{
	int	i;
	unsigned short clut[32];	/* CLUT entry */

	/* load texture pattern and CLUT */
	tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i );
	}
	DrawSync(0);

	/* init sprite */
	SetDrawMode( &drmode[0], 0, 0, tpage, 0 );
	SetDrawMode( &drmode[1], 0, 0, tpage, 0 );
	for( i=0; i<BALLMAX; i++ ) {
		SetSprt16(&sprtBalls[0][i]);			/* set SPT_16 primitve ID */
		SetSemiTrans(&sprtBalls[0][i], 0);		/* semi-amibient is OFF */
		SetShadeTex(&sprtBalls[0][i], 1);		/* shading&texture is OFF */
		setUV0(&sprtBalls[0][i], 0, 0);			/* texture point is (0,0) */
		sprtBalls[0][i].clut = clut[i%32];		/* set CLUT */
		setXY0( &sprtBalls[0][i], rand()%PIH, rand()%PIV );	/* update vertex */
		sprtBalls[1][i] = sprtBalls[0][i];
		vx[i] = (rand()%2 ? 1:-1)*(rand()%3+1);
		vy[i] = (rand()%2 ? 1:-1)*(rand()%3+1);
	}

}

void _draw_balls_data( int side, long* ot )
{
	int	i;
	SPRT_16* sprt;

	sprt = sprtBalls[side];
	for( i=0; i<BALLMAX; i++ ) {
		*sprt = sprtBalls[side^1][i];
		sprt->x0 += vx[i];
		sprt->y0 += vy[i];
		if( sprt->x0>=PIH || sprt->x0<=0 ) vx[i] = -vx[i];
		if( sprt->y0>=PIV || sprt->y0<=0 ) vy[i] = -vy[i];
		AddPrim( ot, sprt );
		sprt++;
	}
	AddPrim( ot, &drmode[side] );
}

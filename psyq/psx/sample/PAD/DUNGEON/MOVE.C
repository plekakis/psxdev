/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *      Motion algorithm  */
#include <stdio.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "main.h"
#include "sin.h"
#include "maps.h"
#include "move.h"


#define	sign(a)	((a<0)?-1:1)

#define	WEST		(144*32)
#define	POSY		0

GsRVIEW2 view;			/* Structure for setting view point */
SVECTOR	PVect;			/* View point vector */
static long vpx = 0;
static long vpz = 0;

static int collision( int vx, int vz );
#define	SPEED	16
void make_view_data( void )
{
  	/* Reference point parameter setting */
	PVect.vx = PVect.vy = PVect.vz = 0;
	view.vpx = 128; view.vpy = POSY; view.vpz = 128;
	view.vrx = 0; view.vry = POSY; view.vrz = 0;
	/* View point twist parameter setting */
	view.rz = 0;
	view.super = WORLD;
	GsSetRefView2(&view);
	vpx = view.vpx*32;
	vpz = view.vpz*32;
}

int move_user_char( unsigned char* pad )
{
	int n;
	int bcount;
	long range;
	long vx=0, vz=0;
	static long ofx=0, ofy=0;


	/* Turning left/right */
	n = 0;
	if(pad[2]<=0x60 ) n = -(0x60-pad[2]);
	if(pad[2]>=0xa0 ) n = -(0xa0-pad[2]);
	PVect.vy = (PVect.vy+n/2)&4095;


	/* Moving forth and back */
	n = 0;
	if(pad[3]<=0x60 ) n = (0x60-pad[3]);
	if(pad[3]>=0xa0 ) n = (0xa0-pad[3]);
	if(n!=0) {
		vz += (rcos( PVect.vy )*n/128/4);
		vx += (rsin( PVect.vy )*n/128/4);
	}

	/* Impact judgement */
	bcount = 0;
	if( vz>0 ) {
		if (collision(0, vz+WEST)||
			collision(-WEST, vz+WEST)||
				collision( WEST, vz+WEST))
		{ vz = -vz; bcount++; }
	}
	if( vz<0 ) {
		if (collision(0, vz-WEST)||
			collision(-WEST, vz-WEST)||
				collision( WEST, vz-WEST))
		{ vz = -vz; bcount++; }

	}
	if( vx>0 ) {
		if (collision( vx+WEST,0)||
			collision( vx+WEST,WEST)||
				collision( vx+WEST, -WEST))
		{ vx = -vx; bcount++; }
	}
	if( vx<0 ) {
		if (collision( vx-WEST,0)||
			collision( vx-WEST,  WEST)||
				collision( vx-WEST, -WEST))
		{ vx = -vx; bcount++; }
	}
	if( vz || vx ) {
		vpz += vz;
		vpx += vx;
		view.vpz = vpz/32;
		view.vpx = vpx/32;
	}

	range = rcos( PVect.vx );
	view.vrz = (view.vpz+rcos( PVect.vy+ofy )*range/ONE);
	view.vrx = (view.vpx+rsin( PVect.vy+ofy )*range/ONE);
	view.vry = rsin( PVect.vx+ofx ) + POSY;

	return(bcount);
}

int collision( int vx, int vz )
{
	int rslt;
	int wx, wz;

	wx = (vpx+vx)/32+FACT/2;
	wz = (vpz+vz)/32+FACT/2;

	if( wx<0 || wz<0 ) return 1;

	wx = wx/FACT; wz = wz/FACT;

	rslt = get_world_maps( wx, wz );

	if (rslt)
		set_world_maps( wx, wz, 0 );

	return(rslt);
}

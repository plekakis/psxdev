/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *      Map Data Administration */
#include <stdio.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "sin.h"
#include "main.h"
#include "maze.h"
#include "maps.h"

static int make_dir_maps( void );
static void _set_dir_maps( int dir, int wx, int wy );
static void _clr_dir_maps( int dir, int wx, int wy );
static int _get_dir_maps( int dir, int wx, int wy );

#define OBJMAX		192
#define WD		8
#define DV		32

static char worldmaps[WLDX_MAX][WLDY_MAX];
static GsDOBJ2	object[OBJMAX];
static GsCOORDINATE2 World[WLDX_MAX][WLDY_MAX];
static short nobj;
static int wldxmax, wldymax;
/****************************************************************************

****************************************************************************/
void make_world_maps( int wldx, int wldy )
{
	int	x;
	int	y;
	int 	n;
	int	fact;

	wldxmax = wldx;
	wldymax = wldy;
	make_dir_maps();
	make_maze_data( wldxmax/3, wldymax/3 );

	fact = FACT;
	for( x=0; x<wldxmax; x+=3 ) {
		for( y=0; y<wldymax; y+=3 ) {
			n = worldmaps[x][y] = get_maze_data( x/3, y/3 );
			worldmaps[x][y+1] = n;
			worldmaps[x][y+2] = n;
			worldmaps[x+1][y] = n;
			worldmaps[x+2][y] = n;
			worldmaps[x+1][y+1] = n;
			worldmaps[x+1][y+2] = n;
			worldmaps[x+2][y+1] = n;
			worldmaps[x+2][y+2] = n;
		}
	}

	for( x=0; x<wldxmax; x++ ) {
		for( y=0; y<wldymax; y++ ) {
			GsInitCoordinate2(WORLD, &World[x][y]);
			World[x][y].coord.t[0] = x*fact;
			World[x][y].coord.t[1] = 0;
			World[x][y].coord.t[2] = y*fact;
		}
	}
}

int calc_world_data( GsRVIEW2* pview, SVECTOR* pvect )
{
	int	n;
	int	x, y;
	int	px, py;
	int	fact;

	fact = FACT;

	px = (pview->vpx+fact/2)/fact;
	py = (pview->vpz+fact/2)/fact;
	n = 0;
	for( x=-WD; x<=WD; x++ ) {
		for( y=-WD; y<=WD; y++ ) {
			if( px+x<0 || px+x>=wldxmax ) continue;
			if( py+y<0 || py+y>=wldymax ) continue;
			if( _get_dir_maps( pvect->vy, x, y )==0 ) continue;
			if( abs(x)<3 && abs(y)<3 ) {
				n = calc_model_data( n, px+x, py+y, fact, GsDIV1 );
			} else	{
				n = calc_model_data( n, px+x, py+y, fact, 0 );
			}
		}
	}
	nobj = n;
	return( n );
}

int calc_model_data( int n, int px, int py, int fact, int atrb )
{
	if( worldmaps[px][py] != 0) return(n);

	/* Left wall */
	if(( worldmaps[px-1][py] != 0 )|| px==0 ) {
		GsLinkObject4((long)dop4, &object[n],0);
		object[n].coord2 = &World[px][py];
		object[n].attribute = atrb;
		n++;
	}
	/* Right wall */
	if(( worldmaps[px+1][py] != 0 )|| px==wldxmax-1 ) {
		GsLinkObject4((long)dop3, &object[n],0);
		object[n].coord2 = &World[px][py];
		object[n].attribute = atrb;
		n++;
	}

	/* Back wall */
	if(( worldmaps[px][py-1] != 0 ) || py==0 ) {
		GsLinkObject4((long)dop1, &object[n],0);
		object[n].coord2 = &World[px][py];
		object[n].attribute = atrb;
		n++;
	}
	/* Front wall */
	if(( worldmaps[px][py+1] != 0) || py==wldymax-1 ) {
		GsLinkObject4((long)dop2, &object[n],0);
		object[n].coord2 = &World[px][py];
		object[n].attribute = atrb;
		n++;
	}

	/* Floor */
	GsLinkObject4((long)dop6, &object[n],0);
	object[n].coord2 = &World[px][py];
	object[n].attribute = atrb;
	n++;

	return( n );
}

void draw_maps_data( GsOT* ot )
{
	int i;
	GsDOBJ2 *op;
	MATRIX	tmpls, tmplw;

	op = object;
	for( i=0; i<nobj; i++ ) {
		GsGetLws(op->coord2,&tmplw, &tmpls);	/* Setting light matrix to GTE */
		GsSetLightMatrix(&tmplw);		/* Calculationg screen/local 
                                         matrix */
		GsSetLsMatrix(&tmpls);			/* Registering object after 
                        perspective translation to OT */
		GsSortObject4(op, ot, 3, getScratchAddr(0));
		op++;
	}
}

int get_world_maps( int wx, int wz )
{
	if( wx<0 || wz<0 ||
	wx>=wldxmax || wz>=wldymax ) return(1);

	return( worldmaps[wx][wz] );
}
int set_world_maps( int wx, int wz, int val )
{
	if( wx<0 || wz<0 ||
	wx>=wldxmax || wz>=wldymax ) return 1;

	worldmaps[wx][wz] = val;
	return val;
}
/****************************************************************************

****************************************************************************/
static char dirmaps[4096/DV][(WD*2+1)*4];

void _set_dir_maps( int dir, int wx, int wy )
{
	dirmaps[(dir&4095)/DV][(WD+wy)*4+(WD+wx)/8] |= 1<<((WD+wx)%8);
}

void _clr_dir_maps( int dir, int wx, int wy )
{
	dirmaps[(dir&4095)/DV][(WD+wy)*4+(WD+wx)/8] &= (~1<<((WD+wx)%8));
}

int _get_dir_maps( int dir, int wx, int wy )
{
	if( dirmaps[dir/DV][(WD+wy)*4+(WD+wx)/8]==0 ) return(0);
	return((int)( dirmaps[dir/DV][(WD+wy)*4+(WD+wx)/8] & (1<<((WD+wx)%8)) ? 1:0 ));
}
void make_dir_maps_s( int ofs )
{
	int o;
	int d;
	int wx, wy;

	for( wx=-WD; wx<=WD; wx++ ) {
		for( wy=-WD; wy<=WD; wy++ ) {
			_clr_dir_maps( ofs, wx, wy );
		}
	}

	for( o=-480; o<=480; o+=8  ) {
		for( d=0; rcos(o)*d/ONE<=WD; d++ ) {
			wx = rsin( ofs+o )*d/ONE;
			wy = rcos( ofs+o )*d/ONE;
			_set_dir_maps( ofs, wx, wy );
		}
	}

	for( o=-544; o<=544; o+=8  ) {
		for( d=0; rcos(o)*d/ONE<=5; d++ ) {
			wx = rsin( ofs+o )*d/ONE;
			wy = rcos( ofs+o )*d/ONE;
			_set_dir_maps( ofs, wx, wy );
		}
	}

	for( o=-768; o<=768; o+=8  ) {
		for( d=0; rcos(o)*d/ONE<=3; d++ ) {
			wx = rsin( ofs+o )*d/ONE;
			wy = rcos( ofs+o )*d/ONE;
			_set_dir_maps( ofs, wx, wy );
		}
	}

	for( o=0; o<4096; o+=8  ) {
		for( d=0; d<3; d++ ) {
			wx = rsin( o )*d/ONE;
			wy = rcos( o )*d/ONE;
			_set_dir_maps( ofs, wx, wy );
		}
	}
}

int make_dir_maps( void )
{
	int	i;
	int	cnt;
	int	wx, wy;

	for( i=0; i<4096; i+=DV ) {
		cnt = 0;
		make_dir_maps_s( i );
		for( wy=WD; wy>=-WD; wy-- ) {
			for( wx=-WD; wx<=WD; wx++ ) {
				cnt += _get_dir_maps(i,wx,wy);
			}
		}
	}
	return( cnt );
}

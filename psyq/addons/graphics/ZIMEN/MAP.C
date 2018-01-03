/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *				map
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved;
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Jun,19,1995	suzu	
 *
 *			initialize map
 */

#include "sys.h"

/*
 * Cell charactor type.
 : マップで使用するキャラクタマップ
 */
typedef struct {
	u_char	u0,v0;		/* cell texture uv */
	u_char	du,dv;		/* texture width, height */
	u_short	tpage;		/* texture page */
	u_short	nclut;		/* number of CLUTs (for fog)  */
	u_short	*clut;		/* cell clut */
	u_long	attr;		/* attribute (reserved) */
} CTYPE;

#include "bgmap.h"

MAP	map[BG_MAPY][BG_MAPX];

static void add_height(void);
void initMap(MESH *mesh)
{
	int x, y, u;
	int x0, x1;
	VECTOR	v0, v1, v2, v3, v4;
	CTYPE	*cp;
	
	mesh->map   = &map[0][0];
	mesh->mx    = BG_MAPX;
	mesh->my    = BG_MAPY;
	mesh->msk_x = BG_MAPX-1;
	mesh->msk_y = BG_MAPY-1;

	for (y = 0; y < BG_MAPY; y++)
		for (x = 0; x < BG_MAPX; x++) {
			setVector(&map[y][x].v, 0, 0, 0);
			cp = &_ctype[(_map[y])[x]-'0'];
			map[y][x].c.r  = map[y][x].c.g = map[y][x].c.b = 128;
			map[y][x].u0   = cp->u0;
			map[y][x].v0   = cp->v0;
			map[y][x].du   = cp->du-1;
			map[y][x].dv   = cp->dv-1;
			map[y][x].attr = cp->attr;
		}
#ifdef DEBUG	
	add_height();
#endif	
}

/*
 * Added height to the glid point of the map. This is used for debug
 * for ground with height distortion.   
 : マップの各格子点に適当に高さをつける。高さつき地面のデバッグに使用する
 */
static void add_height(void)
{
	int i, d, h;
	int ox, oy, x, y;

	for (i = 8; i > 0; i--) {
		for (oy = 0; oy < BG_MAPY; oy += i)
			for (ox = 0; ox < BG_MAPX; ox += i) {
				h = rand()%512 - 512/2;
				for (y = oy; y < oy+i; y++)
					for (x = ox; x < ox+i; x++) 
						map[y][x].v.vz += h;
			}
	}
}
				

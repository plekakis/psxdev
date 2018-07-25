/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *				main
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved;
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Jun,19,1995	suzu	
 *	2.00		Feg,15,1996	suzu	
 */
/*			the ground */

#include "sys.h"

/*
 * mesh environment
 */
#define OTSIZE	4096			/* ordering table size */
#define MAXHEAP	1024			/* primitive buffer */
#define CL_UX	(1<<11)			/* cell size (widht) */	
#define CL_UY	(1<<11)			/* cell size (height) */

/*
 * GEOMENV: data base about camera position and angle
 * Local-screen matrix is derived from 'angle' by geomUpdate()
 * The current postion of the camera (home) is described in the world
 * coordinate. The moving vector of the camera (dvs) is described in the
 * screen coordinate. Moving vector (dvs) is translated to the vector
 * in the world coodinate and added to 'home'. In this addition,
 * differential of height (z) is eliminated for natural camera movement.
 *	
 * The ground is represented by two dimentional POLY_FT4 array (cell).
 * When the position of camera (home) exceeds cell size (CL_UX,
 * CL_UY), map index is updated to the opposite direction, and the
 * position is adjusted among (0,0)-(CL_UX,CL_UY).
 *	 */
static GEOMENV	genv = {
	0,		0,	0,   0,	/* moving vector */
	1024,		0,	512, 0,	/* local-screen angle */
	1024,		1024, 	2048,0,	/* current position */
	
	CL_UX,		CL_UY,		/* geometry wrap-round unit */
	16000,		16000*3/5,	/* fog near and fog end */
};


/*
 * Mesh data structure expression of the groud
 *	
 * The ground is handled by 2 dimentional POLY_FT4 array. Each array
 * element is called 'cell'. At first each cell is clipped by the clip
 * area in the world coordinate (clipw) before perspective tranlation.
 * Then cells which is out of the screen clip area (clips) are removed
 * and acturally only few cells are added to OT.
 *
 * Cells which is very near the eye-point are divided into subpolygons
 * to avoid texture distortion.
 *	
 * The cell attribute such as texture address and distortion of the
 * glid vertex are stored in another 2 dimentional array 'map'.
 * The size of 'map' is usually larger than total cell size. Each cell
 * selects the proper map according to the position of the camera.
 * This strategy is similar to the 2D cell type array representation.
 * */
static RECT	_clips;		/* screen clip window */
static RECT32	_clipw;		/* world clip window */

static MESH	mesh = {
	0,				/* map (set later) */
	&_clipw,			/* clip window (world) */
	&_clips,			/* clip window (screen) */
	CL_UX,          CL_UY,		/* cell unit size */
	0,		0,		/* map size (set later) */
	0,		0,		/* map mask (set later) */
	0,	        0,		/* messh offset */
	SCR_Z*2,			/* sub-dividison start point */
	3,				/* max subdivision point */
	128*128,			/* division threshold */
	0,				/* debug mode */
};
	
/*
 * double buffer
 */
typedef struct {			
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_FT4	heap[MAXHEAP];		/* heap */
} DB;

void FNAME (void)
{
	extern 	u_long	mudtex[];	/* MUD (64x64: 4bit) */
	extern 	u_long	bgtex[];	/* BG cell texture pattern */
	static DB	db[2];		/* double buffer */
	DB	*cdb;			/* current double buffer */
	u_long	*ot;			/* current OT */
	u_long	padd;
	int	cnt;
	int	ox, oy;
	
	POLY_FT4	*heap;
	
	/* initialize double buffer */
	db_init(SCR_W, SCR_H, SCR_Z, 0, 0, 0);

	/* initialize debug menu */
	menu_init();

	/* initialize fog parameters */
	SetFarColor(0, 0, 0);
	SetFogNearFar(genv.fog_near, genv.fog_far, SCR_Z);	

	/* load texture */
	mesh.tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	mesh.clut  = LoadClut(bgtex, 0, 480);
	
	/* initialize MAP. MAP has information about texture point or
	 * vertex distortion of each mesh. */
	initMap(&mesh);
	
	/* Adaptive subdivision starts when the distance of target
	 * primitive is smaller than mesh.divz. If no subdivision is
	 * required, set 0 at this field. */
#ifdef NO_DIV
	mesh.divz = 0;
#endif
	
	/* if rounded BG required, set huge value to the map size  */
#ifdef RAP_ROUND
	mesh.mx *= 256;
	mesh.my *= 256;
#endif
	
	/* set viewing volume  */
#ifdef VIEW_CLIP
	setRECT(mesh.clips, -SCR_X/2, -SCR_Y/2, SCR_W/2, SCR_H/2);
#else
	setRECT(mesh.clips, -SCR_X, -SCR_Y, SCR_W, SCR_H);
#endif	
	
	/* notify clip paraters to divPolyFT4() */
	divPolyClip(mesh.clips, mesh.size, mesh.ndiv);

	/* display start */
	SetDispMask(1);		
	
	/* main loop */
	while (((padd = PadRead(1))&PADselect) == 0) {
		
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */
		
		cnt = VSync(1);
		
		/* get world clip window */
		areaClipZ(mesh.clips, mesh.clipw, genv.fog_far);

		/*set map offset */
		mesh.ox = genv.home.vx&~(genv.rx-1);
		mesh.oy = genv.home.vy&~(genv.ry-1);

		/* rot-trans-pers MESH with subdivision */
		heap = meshRotTransPers(cdb->ot, OTSIZE, &mesh, cdb->heap);

		FntPrint("t=%d,poly=%d\n", VSync(1)-cnt, heap-cdb->heap);
		
		/* read controller and set parameters to genv */
		padRead(&genv);

		/* flush geometry */
		geomUpdate(&genv);

		/* limit home position (z > 0) */
		if (genv.home.vz < SCR_Y) genv.home.vz = SCR_Y;
		
		/* change config */
		padd = PadRead(1);
		
		if (padd&PADstart) 
			setMeshConfig(&mesh, &genv);
		
		/* debug */
		if (mesh.clips->w != SCR_W) {
			disp_clips(mesh.clips);
			disp_clipw(mesh.clipw);
		}

		/* swap double buffer */
		db_swap(cdb->ot+OTSIZE-1);
	}
	
	/* end */
	DrawSync(0);
	return;
}


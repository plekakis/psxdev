/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *				tuto0
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved;
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Jun,19,1995	suzu	
 *
 *			primitive subdivision
 :		       プリミティブの分割
 */

#include "sys.h"

/*#define DEBUG*/
#define OTSIZE	4096			/* ordering table size */
#define MAXHEAP	4096			/* primitive buffer */
#define FOGNEAR 16000
#define FOGFAR 	(16000*3/5)
#define MIN_SIZ	(128*128)
#define WW	1024
#define NDIV	4

typedef struct {
	u_long		ot[OTSIZE];		
	POLY_FT4	wall;		
	u_long		heap[MAXHEAP];
} DB;

static int pad_read(void);
static void init_prim(DB *db);


static GEOMENV	genv = {
	0,		0,	0,   0,	/* moving vector */
	1024,		0,	512, 0,	/* local-screen angle */
	-1024,		-1024, 	256,0,	/* current position */
	0,		0,		/* geometry wrap-round unit */
	16000,		16000*3/5,	/* fog near and fog end */
};

int		is_triangle = 0;
main()
{
	static CVECTOR	col = {0x80,0x80,0x80};
	
	SVECTOR		x[4];		
	DB		db[2];		
	DB		*cdb;		
	long		p, flg, otz;
	POLY_FT4	*hp;
	int		i;
	u_long		padd, opadd = 0;
	RECT		rect;
	int		t;
	
	setVector(&x[0], -WW, -WW, 0); setVector(&x[1], WW, -WW, 0);
	setVector(&x[2], -WW,  WW, 0); setVector(&x[3], WW,  WW, 0);

	/* inititalize double buffer */
	db_init(SCR_W, SCR_H, SCR_Z, 0, 0, 0);
	
	/* initialize fog depth */
	SetFarColor(0, 0, 0);
	SetFogNearFar(FOGNEAR, FOGFAR, SCR_Z);	

	/* set polygon clipping area */
#ifdef DEBUG	        
	setRECT(&rect, -SCR_X+96, -SCR_Y+96, SCR_W-256, SCR_H-256);
#else	
	setRECT(&rect, -SCR_X, -SCR_Y, SCR_W, SCR_H);
#endif	
	divPolyClip(&rect, MIN_SIZ, NDIV);

	init_prim(&db[0]);
	init_prim(&db[1]);
	col.cd = db[0].wall.code;
	
	SetDispMask(1);

	while (((padd = pad_read())&PADselect) == 0) {

		cdb = (cdb==db)? db+1: db;	
		ClearOTagR(cdb->ot, OTSIZE);		
		
		otz = RotTransPers4(&x[0], &x[1], &x[2], &x[3],
			    (long *)&cdb->wall.x0, (long *)&cdb->wall.x1,
			    (long *)&cdb->wall.x2, (long *)&cdb->wall.x3,
			    &p, &flg);

		DpqColor(&col, p, (CVECTOR *)&cdb->wall.r0);
		
		if (is_triangle)
			SetPolyFT3((POLY_FT3 *)&cdb->wall);
		else
			SetPolyFT4(&cdb->wall);
		
		t = VSync(1);
		hp = divPolyFT4(cdb->ot, &x[0], &x[1], &x[2], &x[3],
				&cdb->wall, (POLY_FT4 *)cdb->heap,
				getScratchAddr(0));

		FntPrint("time=%d\n", VSync(1)-t);
		
		/* read controller and set parameters to genv */
		padRead(&genv);

		/* flush geometry */
		geomUpdate(&genv);
		
		/* draw and swap double buffer */
		db_swap(cdb->ot+otz);
#ifdef DEBUG		
		disp_clips(&rect);
#endif		
		opadd = padd;
	}
	PadStop();
	StopCallback();
	return;
}

static void init_prim(DB *db)
{
	extern	u_long	bgtex[];	
	
	db->wall.tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	db->wall.clut  = LoadClut(bgtex, 0, 480);
	SetPolyFT4(&db->wall);
	setRGB0(&db->wall, 0x80, 0x80, 0x80);
	/*setUVWH(&db->wall, 0, 0, 128, 128);*/
	setUVWH(&db->wall, 0, 0, 255, 255);
}

static int pad_read(void)
{
	static u_long opadd;
	u_long padd = PadRead(1);
	
	if (opadd == 0 && padd&PADstart)
		is_triangle = (is_triangle+1)&0x01;

	opadd = padd;
	return(padd);
}


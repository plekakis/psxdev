/* tritst.c - draw triangles and rotate them */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "profile.h"

#define SCR_Z	512				/* distant to screen */
#define MAX_TRI 5000

int numTri;

typedef struct
{		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[4];			/* ordering table */
	POLY_G3		tri[MAX_TRI];	/* primitives */
} DB;

int pad_read();
void init_prim( DB *db );

main()
{
	SVECTOR base[3]; /* triangle's base position */
	DB db[2];
	DB *cdb;
	int	dmy, flg;	/* dummy */
	int currTri;

	numTri = 1;
	/* initialise base triangle position */
	setVector(&base[0], -100, -100, 0);
	setVector(&base[1], 110, 80,0);
	setVector(&base[2], 90, 0,0);

	PadInit(0);
	ProfileInit(1);
	ResetGraph(0);
	SetGraphDebug(0);

	InitGeom(); /* init geom subsystem */
	SetGeomOffset(160,120);

	SetDefDrawEnv(&db[0].draw, 0,0,320,240);
	SetDefDrawEnv(&db[1].draw, 0,240,320,240);
	SetDefDispEnv(&db[0].disp, 0,240,320,240);
	SetDefDispEnv(&db[1].disp, 0,0,320,240);

	init_prim(&db[0]);
	init_prim(&db[1]);

	SetDispMask(1);


	while(pad_read() == 0)
	{
		cdb = (cdb==db)? db+1:db; /* swapping double buffers */

		ClearOTag(cdb->ot, 3);

		for (currTri = 0; currTri < numTri-1; currTri ++)
			{
			/* rotate and shift each vertex */
			RotTransPers3( &base[0], &base[1], &base[2],
								&(cdb->tri[currTri].x0), &cdb->tri[currTri].x1, 
								&cdb->tri[currTri].x2,
								&dmy, &flg);

			AddPrim(cdb->ot+2, &(cdb->tri[currTri]));
			}
		ProfileReadCount();	  			/* for CPU time bar */
		ProfileAddOT( cdb->ot+1 );		/* adds top bar to screen */
		DrawSync(0);
		ProfileAddDrawOT( cdb->ot+1 );/* adds bottom bar to screen */
		VSync(0);

		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);
		DrawOTag(cdb->ot);  /* draw OT */
		ProfileStartCount();		     
	}
	PadStop();
	exit();
}


/* pad read function */

int pad_read()
{
	static SVECTOR ang = { 512, 512, 512 };
	static VECTOR trans = { 0, 0, SCR_Z};
	static MATRIX m;

	int ret = 0;
	u_long padd = PadRead(1);

	if ((padd & PADLup) && (numTri > 1)) numTri--;
	if ((padd & PADLdown) && (numTri < MAX_TRI-1)) numTri++;
	if (padd & PADRup)	ang.vx += 32;
	if (padd & PADRdown)	ang.vx -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;
	if (padd & PADLleft)	ang.vz += 32;
	if (padd & PADLright)	ang.vz -= 32;


	if (padd & PADl)	trans.vz += 8;
	if (padd & PADn) 	trans.vz -= 8;
	if (padd & PADk) 	ret = -1;

	RotMatrix( &ang, &m );
	TransMatrix( &m, &trans );

	SetRotMatrix( &m );
	SetTransMatrix( &m );

	return (ret);
}



void init_prim( DB *db )
{
	int c;
	db->draw.isbg = 1;

	for(c = 0; c<MAX_TRI;c++) 
		{
		SetPolyG3(&db->tri[c]);
		setRGB0(&db->tri[c], 0xff, 0x00, 0x00);	/* set colors */
		setRGB1(&db->tri[c], 0xff, 0x00, 0x00);
		setRGB2(&db->tri[c], 0x00, 0x00, 0xff);
		}

}

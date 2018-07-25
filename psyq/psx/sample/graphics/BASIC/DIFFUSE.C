/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			diffusion*/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define UNIT	32			/* absolute particle size */
#define SCR_Z	(512)			/* screen depth (h) */
#define WALL	(256)			/* diffusion limitation */
#define	OTSIZE	(WALL*4+SCR_Z)		/* ordering table size */
#define NOBJ	9			/* number of particles */
#define NOBJ3	(NOBJ*NOBJ*NOBJ)

/*
 * Primitive Buffer
 */
typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	
	u_long		ot[OTSIZE];		/* ordering table */
	SPRT_16		sprt[NOBJ3];		/* sprite particle */
	POLY_F4		wall[NOBJ3];		/* quad particle */
} DB;

/*
 * Position Buffer
 */
typedef struct {
	SVECTOR	x[4];			/* vertex   */
	SVECTOR	v;			/* verocity */
} POS;

static POS	Pos[NOBJ3];	/* position buffer */
static int	Abe, Isball;	/* Ambient Enale, Ball/Wall flag */

static void init_prim(DB *db);
static void init_point(void);
static int padread(void);

void Diffuse(void)
{
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current buffer */
	char	s[128];		/* strings to print */
	int	ret, n = 0;	/* work */
	int	id;		/* font ID */

	Abe = 0, Isball = 0;
	
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	SetGeomOffset(320, 240);	/* set geometry origin as (320,120) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */

	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0,   0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0,   0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0,   0, 640, 480);
	
	/* init font environment */
	FntLoad(960, 256);
	SetDumpFnt(id = FntOpen(32, 32, 320, 440, 0, 512));
	
	init_prim(&db[0]);	/* initialize packet #0 */
	init_prim(&db[1]);	/* initialize packet #1 */
	init_point();	

	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	while ((ret = padread()) != -1)	{
		if (ret == 1) {
			init_prim(&db[0]);	
			init_prim(&db[1]);
			init_point();
		}
		if (n++ == 120) {
			n = 0;
			init_point();
		}
		cdb = (cdb == db)? db+1: db;
			
		update(cdb);		/* update primitives */
		ResetGraph(1);		/* reset drawing */
		VSync(0);		/* wait for the next V-BLNK */
		
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */
		DrawOTag(cdb->ot+OTSIZE-1);	  /* draw */
		/*DumpOTag(cdb->ot+OTSIZE-1);	  /* dump */

		FntPrint(id, "Abe   =%d\n", Abe);
		FntPrint(id, "Isball=%d\n", Isball);
		FntFlush(id);
	}
	
	DrawSync(0);
	return;
}

update(db)
DB	*db;
{
	POS	*pp;					/* work */
	u_long	*ot;					/* current OT */
	int	i, j;					/* work */
	long	p, otz, opz, dmy, flg;			/* work for GTE */
	SPRT_16	*sp;					/* sprite pointer */
	POLY_F4	*wp;					/* wall pointer */
	
	ClearOTagR(db->ot, OTSIZE);	/* clear ordering table */
		
	ot = db->ot;
	pp = Pos;
		
	if (Isball) {	/* draw ball sprite */
	    for (sp = db->sprt, i = 0; i < NOBJ3; i++, sp++, pp++) {
			
		otz = RotAverage3(&pp->x[0], (long *)&sp->x0, &dmy, &flg);
				
		if (otz > 0 && otz < OTSIZE)
			AddPrim(ot+otz, sp);
		
		addVector(&pp->x[0], &pp->v);
	    }
	}
	else {		/* draw wall polygons */
	    for (wp = db->wall, i = 0; i < NOBJ3; i++, wp++, pp++) {

		otz = RotAverage4(&pp->x[0], &pp->x[1], &pp->x[2], &pp->x[3],
			  (long *)&wp->x0, (long *)&wp->x1, 
			  (long *)&wp->x2, (long *)&wp->x3, &p, &flg);

		if (otz > (SCR_Z/2)>>2 && otz < OTSIZE) 
			addPrim(ot+otz, wp);

		addVector(&pp->x[0], &pp->v);
		addVector(&pp->x[1], &pp->v);
		addVector(&pp->x[2], &pp->v);
		addVector(&pp->x[3], &pp->v);
	    }
	}
}

/*
 * Initialize drawing Primitives
 */
#include "balltex.h"

static void init_prim(DB *db)
{
	SPRT_16	*s;
	POLY_F4	*w;
	u_short	clut[32];
	int	x, y, z, i;

	db->draw.isbg  = 1;
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
	setRGB0(&db->draw, 20, 40, 40);

	for (i = 0; i < 32; i++)
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);

	/* init sprite */
	i = 0;
	s = db->sprt;
	w = db->wall;
	for (z = 0; z < NOBJ; z++) 
	for (y = 0; y < NOBJ; y++)
	for (x = 0; x < NOBJ; x++, s++, w++, i++) {
		SetSprt16(s);			
		SetSemiTrans(s, Abe);
		SetShadeTex(s, 1);
		setRGB0(s, 0x80, 0x80, 0x80);
		setUV0(s, 0, 0);
		s->clut = clut[i%32];		/* set CLUT */
		SetPolyF4(w);
		SetSemiTrans(w, Abe);
		setRGB0(w, x*256/NOBJ, y*256/NOBJ, z*256/NOBJ);
	}
}	

/*
 * Initialize sprite position and verocity
 */
static void init_point(void)
{
	int	x, y, z;
	POS	*p;
	
	for (p = Pos, z = 0; z < NOBJ; z++) 
	for (y = 0; y < NOBJ; y++)
	for (x = 0; x < NOBJ; x++,  p++) {
		
		setVector(&p->x[0],    0,    0, 0);
		setVector(&p->x[1], UNIT,    0, 0);
		setVector(&p->x[2],    0, UNIT, 0);
		setVector(&p->x[3], UNIT, UNIT, 0);
		
		setVector(&p->v, x-NOBJ/2, y-NOBJ/2, z-NOBJ/2);
	}
}	

/*
 * Read controll-pad
 */
#define DT	32
static int padread(void)
{
	static SVECTOR	ang = {512, 512, 512};	
	static VECTOR	vec = {0,     0, SCR_Z+WALL};	
	static MATRIX	m;
	static u_long	opadd = 0;
	
	int	ret = 0;	
	u_long	padd = PadRead(1);
	
	if (padd & PADLup)	ang.vx += DT;
	if (padd & PADLdown)	ang.vx -= DT;
	if (padd & PADLleft) 	ang.vy += DT;
	if (padd & PADLright)	ang.vy -= DT;

	if ((opadd != padd) && (padd&PADRdown)) {
		Abe = Abe? 0: 1;
		ret = 1;
	}
	if ((opadd != padd) && (padd&PADRright)) {
		Isball = Isball? 0: 1;
		ret = 1;
	}
	if (padd & PADselect)
		ret = -1;

	RotMatrix(&ang, &m);
	TransMatrix(&m, &vec);
	SetRotMatrix(&m);
	SetTransMatrix(&m);

	/* reserve old data */
	opadd = padd;
	return(ret);
}		


/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* --------------------------------------------------------------------
          GetODE ()  Sample Program

        balls     suzu
        97/01/23  stanaka  vsynccallback, inter-race, getode


          PADLup  : increase number of balls by 1
          PADLdown: decrease number of balls by 1

          increasing the number of balls will result in 30 fps due to a CPU bottleneck
          further increases will generate a GPU bottleneck, but this is not
          taken into account so the screen will be corrupted.
   -------------------------------------------------------------------- */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define OTSIZE		1
#define MAXOBJ		7000
#define	FRAME_X		640
#define	FRAME_Y		480
#define WALL_X		(FRAME_X-16)
#define WALL_Y		(FRAME_Y-16)

typedef struct {
	DRAWENV		draw;
	DISPENV		disp;
	u_long		ot[OTSIZE];
	SPRT_16		sprt[MAXOBJ];
} DB;

volatile u_long VsyncWait, Frame;
DB	*Cdb, Db[2];

typedef struct {
	u_short x, y;
	u_short dx, dy;
} POS;

void init_prim(DB *);
int  pad_read(int);
void cbvsync(void);
void init_point(POS *);

main(){
	POS	pos[MAXOBJ];
	int	nobj = 3000;
	u_long	*ot;
	SPRT_16	*sp;
	POS	*pp;
	int	i, x, y;
	u_long sync = 0;

	ResetCallback();
	PadInit(0);
	ResetGraph(0);
	SetGraphDebug(0);

	SetDefDrawEnv(&Db[0].draw, 0, 0, FRAME_X, FRAME_Y);
	SetDefDrawEnv(&Db[1].draw, 0, 0, FRAME_X, FRAME_Y);
	SetDefDispEnv(&Db[0].disp, 0, 0, FRAME_X, FRAME_Y);
	SetDefDispEnv(&Db[1].disp, 0, 0, FRAME_X, FRAME_Y);

	KanjiFntOpen(160, 16, 256, 200, 704, 0, 768, 256, 0, 512);
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	init_prim(&Db[0]);
	init_prim(&Db[1]);

	/* initialize OTs for both frames in preparation of CPU bottleneck*/
	ClearOTag(Db[0].ot, OTSIZE);
	ClearOTag(Db[1].ot, OTSIZE);

	init_point(pos);

	SetDispMask(1);

	VSync(0);					/* for V-SYNC timing*/
	Frame = !GetODE();			/* initial evaluation value for GetODE() */

	/* set up callback function for V-SYNCs */
	VSyncCallback(cbvsync);

	/* main loop*/
	while ((nobj = pad_read(nobj)) > 0) {
		Cdb  = (Cdb==Db)? Db+1: Db;
		ClearOTag(Cdb->ot, OTSIZE);

		ot = Cdb->ot;
		sp = Cdb->sprt;
		pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
				x = WALL_X*2 - x;
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
				y = WALL_Y*2 - y;
			setXY0(sp, x, y);
			AddPrim(ot, sp);
		}

		KanjiFntPrint("\n\n‹Ê‚Ì”     = %d\n", nobj);
		KanjiFntPrint("CPUTIME = %d\n", VSync(1)-sync);
		KanjiFntFlush(-1);


		for(VsyncWait = 1; VsyncWait; );
		sync = VSync(1);

	}
	printf("nobj=%d\n",nobj);
	PadStop();
	StopCallback();
}

#include "balltex.h"
void init_prim(DB *db){
	u_short	clut[32];
	SPRT_16	*sp;
	int	i;

	db->disp.isinter = 1;
	db->draw.isbg = 1;
	db->draw.dfe = 0;
	setRGB0(&db->draw, 60, 120, 120);

	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);

	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
	}

	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		SetSprt16(sp);
		SetSemiTrans(sp, 0);
		SetShadeTex(sp, 1);
		setUV0(sp, 0, 0);
		sp->clut = clut[i%32];
	}
}

void init_point(POS *pos){
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();
		pos->y  = rand();
		pos->dx = (rand() % 4) + 1;
		pos->dy = (rand() % 4) + 1;
		pos++;
	}
}

int pad_read(int n){
	u_long	padd = PadRead(1);

	if(padd & PADLup)	n += 4;
	if(padd & PADLdown)	n -= 4;

	if (padd & PADL1)
		while (PadRead(1)&PADL1);

	if(padd & PADselect) 	return -1;

	limitRange(n, 1, MAXOBJ-1);
	return n;
}

/* VSync callback routine*/
void cbvsync(void){
	u_long i;

	/* GetODE() *****************************************************:
        Only even-numbered frames are returned in the callback function, so the
        static variable Frame is used for comparison.*/
	for(; !(Frame^(i = GetODE())); );
	Frame = i;

	/* In particular, the switching between odd -> even frames takes place right after V-BLNK so the display/rendering environment switching should be performed as soon as possible.*/
	PutDispEnv(&Cdb->disp);
	PutDrawEnv(&Cdb->draw);

	if(VsyncWait){
		DrawOTag(Cdb->ot);
		VsyncWait = 0;
	}
	else
		/* A CPU bottleneck has taken place, so render OT from previous frame. Note that the OT from the previous frame must be defined clearly.*/
		DrawOTag((Cdb==Db? Db+1:Db)->ot);
}

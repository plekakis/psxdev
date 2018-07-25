/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			mouse: sample program
 *
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Jan,6,1995	shino
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Primitive Buffer
 */
#define OTSIZE		16			/* size of ordering table */
#define MAXOBJ		3			/* max sprite number */
typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	SPRT_16		sprt[MAXOBJ];		/* 16x16 fixed-size sprite */
} DB;

/*
 * Position Buffer
 */
typedef struct {		
	short x, y;			/* current point */
	short dx, dy;			/* verocity */
} POS;

/*
 * Limitations
 */
#ifndef MAX_PAD
#define MAX_PAD         34
#endif

#if 0
#define MOUSEright	0x04
#define MOUSEleft	0x08
#endif
static	init_prim(DB *);
static	char buff[2][MAX_PAD];
static	u_long	buf[3];
main()
{
	DB	db[2];		/* double buffer */
	
	char	s[128];		/* strings to print */
	DB	*cdb;		/* current double buffer */
	int	nobj = 1;	/* object number */
	u_long	*ot;		/* current OT */
	SPRT_16	*sp;		/* work */
	int	i, x0,x1,y0,y1;	/* work */
	int 	j,k;

#if 0
	StopCARD();
#endif
	ResetCallback();
        InitPAD(buff[0], MAX_PAD, buff[1], MAX_PAD);
        InitMouse(buff[0], buff[1]);
        SenseMouse(3,4);
	RangeMouse(0,320,0,240);
        SetMouse(0, 80, 100);
        SetMouse(1, 250, 100);
	RangeMouse(0,300,10,220);
        StartPAD();
	ChangeClearPAD(0);

	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */

	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);
	
	/* init font environment */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	init_prim(&db[0]);		/* initialize primitive buffers #0 */
	init_prim(&db[1]);		/* initialize primitive buffers #1 */

	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	buf[0]=buf[1]=buf[2]=0;
        for(i = 0;i < MAX_PAD; i++)
                buff[0][i] = buff[1][i] = 0xff;
	
	j=k=1;
	while (j) {
		cdb  = (cdb==db)? db+1: db;	/* swap double buffer ID */

		/* report */
		
 		/* clear ordering table */
		ClearOTag(cdb->ot, OTSIZE);	
		setRGB0(&cdb->draw, 127, 120, 120);
		
		/* update sprites */
		ot = cdb->ot;
		sp = cdb->sprt;
		
			/* detect reflection */

		if(buff[0][0] == 0 && buff[0][1] == 0x41 && (~buff[0][2] & 0x01)) {
			break;
		}
		if(buff[1][0] == 0 && buff[1][1] == 0x41 && (~buff[1][2] & 0x01)) {
			break;
		}


		for(i=0;i<2;i++){
			MouseRead(i,buf);
               	 	if(buf[2]&MOUSEright)
				SetMouse(i, 230+i*20, 100);
                	if(buf[2]&MOUSEleft)
				SetMouse(i, 80+i*20, 100);
			FntPrint("X%d=%04d Y%d=%04d\n", i,buf[0],i,buf[1]);
			setXY0(sp, buf[0], buf[1]);	/* update vertex */
			AddPrim(ot, sp);	/* apend to OT */
			sp++;
		}
		
		DrawSync(0);		/* wait for end of drawing */
		VSync(0);		/* wait for V-BLNK */
		PutDispEnv(&cdb->disp); /* update display environment */
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		DrawOTag(cdb->ot);
		FntFlush(-1);
	}
	PadStop();
	/*exit();*/
	ResetGraph(3);
	StopCallback();
	return;
}

/*
 * Initialize drawing Primitives
 */
#include "balltex.h"

static init_prim(db)
DB	*db;
{
	u_short	clut[32];		/* CLUT entry */
	SPRT_16	*sp;			/* work */
	int	i;			/* work */
	
	/* inititalize double buffer */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);
	
	/* load texture pattern and CLUT */
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
	DumpTPage(db->draw.tpage);
	
	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
		/*DumpClut(clut[i]);*/
	}
	
	/* init sprite */
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		SetSprt16(sp);			/* set SPRT_16 primitve ID */
		SetSemiTrans(sp, 0);		/* semi-amibient is OFF */
		SetShadeTex(sp, 1);		/* shading&texture is OFF */
		setUV0(sp, 0, 0);		/* texture point is (0,0) */
		sp->clut = clut[i%32];		/* set CLUT */
	}
}	



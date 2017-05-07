/*
===========================================================
                Sony PlayStation 1 Source Code
===========================================================
                         FONT EXAMPLE
Displays text on the screen using the built in GPU routines
-----------------------------------------------------------

    Developer / Programmer..............: SCEI & PSXDEV.net
    Software Ddevelopment Kit...........: PSY-Q
    Last Release........................: 30/APRIL/2014

    Original code by SCEI | Edited by PSXDEV.net

	NOTE: This example uses double buffering! I may go
	ahead and remove the double buffering to make the code
	much more simplistic in the future.

    All PlayStation 1 games are written in C. If you
	don't know how to program in C, you should follow a
	book or online tutorial and then take a look at the
	PSX C code. Above all, the PSX is an excellent system
	to program on and work with and will not dissapoint you.

    If you need help, read LIBOVR46.PDF in PSYQ/DOCS
    also join our IRC channel on EFNET #psxdev

  Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
                     All Rights Reserved.

   Sony Computer Entertainment Inc. Development Department

                    http://psxdev.net/

-----------------------------------------------------------*/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

// ----------
// CONSTANTS
// ----------
#define ORIGIN_X  160 // screen width
#define	ORIGIN_Y 120 // screen height

#define MAX_OT_ENTRIES 1

#define MAX_BUFFERS 2

// ----------
// STRUCTURES
// ----------
typedef struct
{
    DRAWENV draw;
    DISPENV disp;
    u_long ot[MAX_OT_ENTRIES];
    POLY_G3 triangle;
}DB;

typedef struct
{
    VECTOR position;
    VECTOR look;
}Camera;

// ----------
// GLOBALS
// ----------
DB db[MAX_BUFFERS];
DB* cdb;

Camera cdebug;
Camera* ccam = &cdebug;

u_long frameIdx = 0;

void init_system(int x, int y, int z, int level)
{
    u_short idx;

    ResetGraph(0);		/* initialize Renderer		*/

	/* reset graphic subsystem*/
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));

	/* set debug mode (0:off, 1:monitor, 2:dump)  */
	SetGraphDebug(level);

	/* initialize geometry subsystem*/
	InitGeom();

	CdInit();		/* initialize CD-ROM		*/

    PadInit(0);     /* initialize input */

	/* set geometry origin as (160, 120)*/
	SetGeomOffset(x, y);

	/* distance to veiwing-screen*/
	SetGeomScreen(z);

	/* define frame double buffer */
	/*	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,240)-(320,480) (320x240)
	 */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	for (idx=0; idx<MAX_BUFFERS; idx++)
    {
        db[idx].draw.isbg = 1;
        setRGB0(&db[idx].draw, 0, 64, 127);
    }

    SetDispMask(1);
}

void init_prim(DB* db)
{
	SetPolyG3(&db->triangle);

	/* set colors for each vertex*/
	setRGB0(&db->triangle, 0xff, 0x00, 0x00);
	setRGB1(&db->triangle, 0x00, 0xff, 0x00);
	setRGB2(&db->triangle, 0x00, 0x00, 0xff);
}

int main()
{
    u_char running = 1;
    MATRIX m;
    MATRIX viewMat;
    SVECTOR x[3];
    long dummy0, dummy1;
    u_short padd;
    static VECTOR	vec   = {0,     0, 0};
    static SVECTOR	ang   = {0, 0, 0};

    static VECTOR viewTrans = {0, 0, 1000};

    init_system(ORIGIN_X, ORIGIN_Y, 512, 0);

	FntLoad(960, 256); // load the font from the BIOS into VRAM/SGRAM
	SetDumpFnt(FntOpen(5, 20, 320, 240, 0, 512)); // screen X,Y | max text length X,Y | autmatic background clear 0,1 | max characters

	init_prim(&db[0]);
	init_prim(&db[1]);

    setVector(&x[0], -256, 128, 0);
    setVector(&x[1],  256, 128, 0);
    setVector(&x[2], 0,  -128, 0);


	while (running) // draw and display forever
	{
	    cdb = &db[frameIdx % MAX_BUFFERS];
        padd = PadRead(1);

	    FntPrint("Demo\n");
		FntFlush(-1);

		RotMatrix(&ang, &m);

        /* set rotation*/
        SetRotMatrix(&m);

        if (padd & PADselect) vec.vx++;
		TransMatrix(&m, &vec);

		TransMatrix(&viewMat, &viewTrans);
		//SetTransMatrix(&viewMat);

        /* set translation*/
        SetTransMatrix(MulMatrix2(&m, &viewMat));

	    /* clear all OT entries */
		ClearOTag(cdb->ot, MAX_OT_ENTRIES);

        RotTransPers(&x[0], (long *)&cdb->triangle.x0, &dummy0, &dummy1);
        RotTransPers(&x[1], (long *)&cdb->triangle.x1, &dummy0, &dummy1);
        RotTransPers(&x[2], (long *)&cdb->triangle.x2, &dummy0, &dummy1);

		AddPrim(cdb->ot, &cdb->triangle);

        DrawSync(0);

        /* vsync and swap frame double buffer
		 *  set the drawing environment and display environment. */
		VSync(0);
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);

		/* start Drawing */
		DrawOTag(cdb->ot);

		++frameIdx;
	}

	DrawSync(0);

	PadStop();

	return 0;
}

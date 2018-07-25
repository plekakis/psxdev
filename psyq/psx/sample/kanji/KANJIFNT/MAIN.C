/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*****************************************************************
 *
 * file: main.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include "kanji.h"

/*
 * Primitive Buffer
 */
typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
} DB;

static void init_prim(DB *db);	/* preset unchanged primitive members */
static void print_kanji();
static void show_font_image(char *);

main()
{
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current double buffer */

	ResetCallback();
	SetVideoMode(MODE_NTSC); /* NTSC mode */
	/* SetVideoMode(MODE_PAL);  /* PAL mode */

	PadInit(0);
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
		
	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* init font environment */
	FntLoad(960, 256);		/* load basic font pattern */
        SetDumpFnt(FntOpen(100, 40, 200, 200, 0, 512));

	init_prim(&db[0]);		/* initialize primitive buffers #0 */
	init_prim(&db[1]);		/* initialize primitive buffers #1 */

	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	while ( PadRead(1) != PADk ) {
		cdb  = (cdb==db)? db+1: db;	/* swap double buffer ID */

		DrawSync(0);		/* wait for end of drawing */
		VSync(0);		/* wait for V-BLNK (1/60) */

		PutDispEnv(&cdb->disp); /* update display environment */
		PutDrawEnv(&cdb->draw); /* update drawing environment */

		print_kanji();
		FntFlush(-1);
	}

	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
}

/*
 * Initialize drawing Primitives
 */
static void init_prim(DB *db)
{
	/* inititalize double buffer */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);
}	

/*
 * Print kanji
 */
static void print_kanji()
{
  long addr;
  unsigned char gaiji[2];

  /* addr = kanji_test("‚`");	/* non-KANJI */
  addr = kanji_test("Š¿");	/* KANJI */

  /* gaiji[0] = 0x86; gaiji[1] = 0xac;
  /* addr = kanji_test(gaiji);	/* User-defined Character */

  FntPrint("%d dot kanji font\n\n", DOT);
  FntPrint("addr=0x%08x\n\n\n", addr);

  show_font_image( (char *)addr );
}

/*
 * Print Kanji Font
 */
static void show_font_image(char *ptr)
{
  int i,j,k;
  char buf[32], tmp, ttmp;
  int xchar, ychar;

  xchar = 2;
  ychar = DOT;

  for (i=0 ; i < ychar ; i++) {
    strcpy(buf, "");
    for (j=0 ; j < xchar ; j++) {
      tmp = *(ptr+i*xchar+j);
      for (k=0 ; k < 8 ; k++) {
        if (tmp & 0x80) strcat(buf, "@");
        else strcat(buf, " ");
        tmp *= 2 ;
      }
    }
    buf[xchar*8] = 0;
    FntPrint("%s\n", buf);
  }
}

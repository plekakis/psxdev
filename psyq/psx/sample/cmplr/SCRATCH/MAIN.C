/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Sample Program: How to use SCRACH PAD AREA.
 *
 *      Copyright (C) 1995 by Sony Computer Entertainment Inc.
 *          All rights Reserved
 *
 *   Version    Date
 *  ------------------------------------------
 *  1.00        May,11,1995 yoshi
 *  1.01        Aug,15,1997 yoshi moved the description to readme file.
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsn.h>

#define PAD_BUTTON ( PADRdown | PADRright )

#define SPAD_ADDR 0x1f800000
#define SPAD_SIZE 0x400
#define LOOP_CNT 100

int test0();
typedef struct {
	int loop0;
	int data0;
} STR0;
STR0 *scpad;


/* Set attribute. See main.link regarding the spad.text section */
int test1();
int loop1 __attribute__((section("spad.text"))) = LOOP_CNT;
int data1 __attribute__((section("spad.text"))) = 0;

int test2();
int loop2 = LOOP_CNT;
int data2;

typedef struct {
  DRAWENV draw;
  DISPENV disp;
} DB;

extern long SpadStock;	/* Starting position of spad.text section. See address */

main()
{
	RECT area;
	DRAWENV	draw;			/* drawing environment */
	DISPENV	disp;			/* display environment */
	unsigned long padd,pad,padr;
	DB db[2];
	DB *cdb;
	unsigned char co1,co2,co3;
	unsigned int mode,cnt;
	POLY_G4 g4;

	ResetCallback();
	PadInit(0);
	ResetGraph(0);	
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */

	SetDefDrawEnv(&(db[0].draw), 0,   0, 320, 240);
	SetDefDrawEnv(&(db[1].draw), 0, 240, 320, 240);
	SetDefDispEnv(&(db[0].disp), 0, 240, 320, 240);
	SetDefDispEnv(&(db[1].disp), 0,   0, 320, 240);

	FntLoad(960, 256);		/* load basic font pattern */
	SetDumpFnt(FntOpen(16, 96, 256, 128, 0, 256));
	padd = PadRead(1);	/* A key pressed when menu was activated is invalid */
	cdb = &db[0];
	co1 = 255;
	co2 = 0;
	co3 = 0;
	mode = 0;

	/* copy contents of spad.text section to scratch pad for test1() */
	memcpy((void *)SPAD_ADDR, (void *)SpadStock, SPAD_SIZE);

	/* Enter scratch pad address for test0() and set contents of variables */
	scpad = (STR0 *)SPAD_ADDR;
	scpad->loop0 = LOOP_CNT;

	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	while(1){
#if 0
		pollhost();
#endif

		cdb = ( cdb == &db[1] ) ? &db[0] : &db[1];  
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);

		SetPolyG4(&g4);
		setXYWH(&g4, 0, 0, 320, 240);
		setRGB0(&g4, co1, co2, co3);
		setRGB1(&g4, co3, co1, co2);
		setRGB2(&g4, co2, co3, co1);
		setRGB3(&g4, co1, co2, co3);
		DrawPrim(&g4);
		if ( co3 == 0 && co1 > 0 ){
			co1 -= 5;
			co2 += 5;
		}
		if ( co1 == 0 && co2 > 0 ){
			co2 -= 5;
			co3 += 5;
		}
		if ( co2 == 0 && co3 > 0 ){
			co3 -= 5;
			co1 += 5;
		}

		padr = PadRead(1);
		pad = padr & ~padd;
		padd = padr;
		
		if( pad & PAD_BUTTON ){
			if( mode == 2 )
				mode = 0;
			else
				mode++;
		}
		switch( mode ){
		case 0:
			cnt = test0();
			FntPrint("SCRACH PAD ACCESS 1\n\n");
			FntPrint("  LOOP COUNT: %d x %d\n", scpad->loop0, cnt);
			FntFlush(-1);
			break;
		case 1:		
			cnt = test1();
			FntPrint("SCRACH PAD ACCESS 2\n\n");
			FntPrint("  LOOP COUNT: %d x %d\n", loop1, cnt);
			FntFlush(-1);
			break;
		case 2:
			cnt = test2();
			FntPrint("MAIN MEMORY ACCESS\n\n");
			FntPrint("  LOOP COUNT: %d x %d\n", loop2, cnt);
			FntFlush(-1);
			break;
		}			

		if( pad & PADselect ) {
			PadStop();
			ResetGraph(3);
			StopCallback();
			return 0;
		}
	}
}



/***********************************************
A routine to count how many loops can be performed during one VSync.
Because the counting is done for each LOOP_CNT, there is a slight margin of error */

int
test0()
{
	long vcnt,cnt;	
	register int i;
	long loopn;
	register STR0 *pt;

	cnt = 0;
	loopn = scpad->loop0;	
	pt = scpad;	/* the pointer is taken as a register variable so that high speed 
	               operation is possible */
	VSync(0);
	vcnt = VSync(-1);
	while(1){
		if( vcnt != VSync(-1) )
			break;
		for(i=0; i < loopn; i++){
			pt->data0 = i;
		}
		cnt++;
	}
	return(cnt);
}


int 
test1()
{
	long vcnt,cnt;	
	register int i;
	long loopn;

	cnt = 0;
	loopn = loop1;
	VSync(0);
	vcnt = VSync(-1);
	while(1){
		if( vcnt != VSync(-1) )
			break;
		for(i=0; i<loopn; i++){
			data1 = i;
		}
		cnt++;
	}
	return(cnt);
}


int 
test2()
{
	long vcnt,cnt;	
	register int i;
	long loopn;

	cnt = 0;
	loopn = loop2;
	VSync(0);
	vcnt = VSync(-1);
	while(1){
		if( vcnt != VSync(-1) )
			break;
		for(i=0; i<loopn; i++){
			data2 = i;
		}
		cnt++;
	}
	return(cnt);
}


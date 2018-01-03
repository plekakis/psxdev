/************************************************************
 *                                                          *
 *                        main.c                            *
 *                                                          *
 *                                                          *
 *               Vince Diesi     13/02/97                   *
 *                                                          *
 *   Copyright (C) 1997 Sony Computer Entertainment Inc.    *
 *                  All Rights Reserved                     *
 *                                                          *
 ***********************************************************/

#include <sys/types.h>
#include <kernel.h>
#include <libsn.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include <libcd.h>
#include <libspu.h>
#include <libsnd.h>

#include "main.h"
#include "movie.h"
#include "control.h"

/* ---------------------------------------------------------------------------
 * - CONSTANTS 
 * ---------------------------------------------------------------------------
 */

// Define for timing and testing functions.
// #define TESTING

// Define for soak testing. Press L1 and R1 on start up to activate.
#define SOAK_TEST


// Maximum Volume.
#define MAX_VOL				127

#define INPUT_DELAY			25

/* ---------------------------------------------------------------------------
 * - GLOBAL DEFINITIONS 
 * ---------------------------------------------------------------------------
 */

#ifdef FINAL
u_long _ramsize   = 0x00200000;				// Use 2MB for final.
u_long _stacksize = 0x00004000;
#else
u_long _ramsize   = 0x00800000;				// Use 8MB for development.
u_long _stacksize = 0x00008000;
#endif

DB		db[2];		

short	cdb = 0;							// Current double buffer.
long	fIdA;								// Applic font id.

static volatile long frameNo = 0;			// Current frame No.


#ifdef SOAK_TEST
static short	currStr = 0;				// Current stream during soak test.
#endif

/* ---------------------------------------------------------------------------
 * - TEST MOVIES 
 * ---------------------------------------------------------------------------
 */

static StrInfo bswordStr = {
				"\\BSWORD.STR;1",
				STR_MODE16,
				STR_BORDERS_ON,
				640,
				0, 0,
				640, 240,
				1050,
				0,
				127
			};

				
static StrInfo pres01Str = {
				"\\PRES01.STR;1",
				STR_MODE16,
				STR_BORDERS_OFF,
				256,
				0, 0,
				256, 256,
				4040,
				0,
				127
			};


static StrInfo pandaStr = {
				"\\PANDA.STR;1",
				STR_MODE16,
				STR_BORDERS_ON,
				512,
				96, 8,
				320, 240,
				1375,
				53248, // Max VLC buffer size. 
				127
			};


static StrInfo demo1Str = {
				"\\DEMO1.STR;1",
				STR_MODE16,
				STR_BORDERS_ON,
				320,
				8, 8,
				304, 240,
				750,
				0,
				127
			};


static StrInfo cool01Str = {
				"\\COOL01.STR;1",
				STR_MODE16,
				STR_BORDERS_OFF,
				256,
				0, 0,
				256, 256,
				4040,
				0,
				127
			};


// Stream info array.
static StrInfo	*streams[] = {

				&bswordStr,
				&pres01Str,
				&pandaStr,
				&demo1Str,
				&cool01Str,

				NULL
			}; 
				
/* ---------------------------------------------------------------------------
 * - PRIVATE FUNCTION PROTOTYPES
 * ---------------------------------------------------------------------------
 */

static void InitSys(void); 
static void InitEnvs(DB *db);
static void SndInit(void);
static void SndShutDown(void);
static void CloseSys(void);

static void VSyncCB(void);	
static void ClearVRAM(void);

static short ExitPressed(void);

/* ---------------------------------------------------------------------------
 * - FUNCTION DEFINITIONS
 * ---------------------------------------------------------------------------
 */
 
int main(void) {


	short	block = INPUT_DELAY;

	short	mode = STR_MODE24,
			borders = STR_BORDERS_ON,
			volume = MAX_VOL,
			play = 0,
			exit;

	StrInfo	*str;


	InitSys();
	ClearVRAM();
	InitEnvs(db);
	

	PutDispEnv(&db[cdb].disp);
	SetDispMask(1);


#ifdef SOAK_TEST

	if (Pressed(L1_KEY) && Pressed(R1_KEY)) {

		while (1) {

			str = streams[currStr];
			currStr++;

			if (streams[currStr] == NULL)
				currStr = 0;

			str->mode = STR_MODE24;
			str->drawBorders = STR_BORDERS_OFF;
			str->volume = MAX_VOL;
			exit = PlayStream(str, ExitPressed);
		}
	}

#endif // SOAK_TEST


	while (1) {
	

		cdb ^= 1;


		FntPrint(fIdA, "Movie Player V2.0\n\n");
		FntPrint(fIdA, "Volume   = %d\n", volume);
		FntPrint(fIdA, "L1       = Mode %s\n", (mode) ? "24Bit" : "16Bit");
		FntPrint(fIdA, "L2       = Borders %s\n\n", (borders) ? "On" : "Off");
		FntPrint(fIdA, "Square   = Pandemonium\n");
		FntPrint(fIdA, "Triangle = Broken Sword\n");
		FntPrint(fIdA, "Circle   = Presidents\n");
		FntPrint(fIdA, "R1       = Demo 1\n");
		FntPrint(fIdA, "R2       = Cool Borders\n");

		
		if (Pressed(SQUARE_KEY)) {
			str = &pandaStr;
			play = 1;

		} else if (Pressed(TRIANGLE_KEY)) {
			str = &bswordStr;
			play = 1;

		} else if (Pressed(CIRCLE_KEY)) {
			str = &pres01Str;
			play = 1;

		} else if (Pressed(R1_KEY)) {
			str = &demo1Str;
			play = 1;

		} else if (Pressed(R2_KEY)) {
			str = &cool01Str;
			play = 1;

		} else if (Pressed(LEFT_KEY)) {
			volume -= 2;
			if (volume < 0)
				volume = 0;
							
		} else if (Pressed(RIGHT_KEY)) {
			volume += 2;
			if (volume > MAX_VOL)
				volume = MAX_VOL;

		} else if (!block && Pressed(L1_KEY)) {
			mode = (mode == STR_MODE24) ? STR_MODE16 : STR_MODE24;	
			block = INPUT_DELAY;
				
		} else if (!block && Pressed(L2_KEY)) {
			borders = (borders == STR_BORDERS_ON) ? STR_BORDERS_OFF : STR_BORDERS_ON;	
			block = INPUT_DELAY;
		}


		if (play) {
			str->mode = mode;
			str->drawBorders = borders;
			str->volume = volume;
			exit = PlayStream(str, ExitPressed);
			play = 0;

#ifdef DEBUG
			if (exit == PLAYSTR_ERROR)
				printf("FATAL ERROR during PlayStream!\n");
#endif
		}


		if (block > 0)
			block--;


#ifdef DEBUG		
		pollhost();
#endif


		FntFlush(fIdA);
		DrawSync(0);
		VSync(0);


		PutDrawEnv(&db[cdb].draw);
		PutDispEnv(&db[cdb].disp);
	}
	
	
	CloseSys();		
	return 0;
}

/* ------------------------------------------------------------------------ */

static short ExitPressed(void) {


/* - Type:	PRIVATE
 * -
 * - Ret:	1 = Exit button pressed, 0 = otherwise.
 * -
 * - Usage:	Check if exit stream player key has been pressed.
 */


	return (Pressed(X_KEY) || Pressed(SELECT_KEY));
}

/* ------------------------------------------------------------------------ */

static void InitSys(void) {


/* - Type:	PRIVATE
 * -
 * - Usage: Init system. 
 */


	ResetCallback();
	ResetGraph(0);
	SetGraphDebug(0);
	CdInit();
	InitGeom();
	InitControllers();

	// Video Mode.
#ifndef NTSC
	SetVideoMode(MODE_PAL);
#endif

	SndInit();
	VSyncCallback((void (*)()) VSyncCB);
}

/* ------------------------------------------------------------------------ */

static void InitEnvs(DB *db) {


/* - Type:	PRIVATE
 * -
 * - Usage: Init the drawing and display environments. Also init
 * -		the application and profiler FntPrint streams. 
 */


	// Init the display and drawing environments.
	SetDefDrawEnv(&db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
	SetDefDispEnv(&db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);
	SetDefDrawEnv(&db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
	SetDefDispEnv(&db[1].disp, 0,   0,       FRAME_X, FRAME_Y);
	
	setRECT(&db[0].disp.screen, SCREEN_X, SCREEN_Y, 0, FRAME_Y);	
	setRECT(&db[1].disp.screen, SCREEN_X, SCREEN_Y, 0, FRAME_Y);	
	setRGB0(&db[0].draw, 0, 0, 50);
	setRGB0(&db[1].draw, 0, 0, 50);
	db[1].draw.isbg = db[0].draw.isbg = 1;


	// Init font environment.
	FntLoad(960, 256);	
	fIdA = FntOpen(18, 16, 310, 200, 0, 512);			// Applic stream.
	SetDumpFnt(fIdA);	
}

/* ------------------------------------------------------------------------ */

static void SndInit(void) {


/* - Type:	PRIVATE
 * -
 * - Usage: Init sound system and volumes.
 */


	CdlATV	aud;
	char	result[8];


	aud.val0 = 127;
	aud.val1 = 127;
	aud.val2 = 127;
	aud.val3 = 127;


	CdControl(CdlDemute, NULL, result);
	CdControlB(CdlSetfilter, NULL, result);
	CdMix(&aud);


	// Clear last 100K of SPU RAM (i.e. maximum reverb workarea). 
	SpuInit();
	SpuSetTransStartAddr(421887);
	SpuWrite0(1024 * 100);


	SsInit();


	SsUtReverbOff();
	SsUtSetReverbType(0);
	SsUtSetReverbDepth(0, 0);

	SsSetTickMode(SS_TICKVSYNC);
	SsSetMVol(127, 127);

	SsSetSerialAttr(SS_SERIAL_A, SS_MIX, SS_SON);
	SsSetSerialVol(SS_SERIAL_A, 127, 127);

	SsUtSetReverbType(SS_REV_TYPE_STUDIO_B);
	SsUtReverbOn();
	VSync(75);						//Delay for a while =;-D
	SsUtSetReverbDepth(40, 40);
}

/* ------------------------------------------------------------------------ */

static void SndShutDown(void) {


/* - Type:	PRIVATE
 * -
 * - Usage: Shut down the sound system, clear reverb buffer etc. 
 */


    SsUtReverbOff();
    SsUtSetReverbType(0);
    SsUtSetReverbDepth(0,0);


    // Clear last 100K of SPU RAM (i.e. maximum reverb workarea). 
    SpuSetTransStartAddr(421887);
    SpuWrite0(1024 * 100);


	// Wait until SPU RAM cleared.
	VSync(100);


    SsEnd();            
} 	

/* ------------------------------------------------------------------------ */

static void CloseSys(void) {


/* - Type:	PRIVATE
 * -
 * - Usage: Clean up / shut down system.	
 */


	VSyncCallback(NULL);
	SndShutDown();
	StopCallback();
	StopControllers();
	ResetGraph(3);
}

/* ------------------------------------------------------------------------ */

static void VSyncCB(void) {


/* - Type:	CALLBACK	
 * -
 * - Usage:	VSyncCallback. Check status of controllers every second. 
 */

	
	frameNo++;

	
	if (!(frameNo % CHECK_CONTROLLERS))
		CheckControllers();
}

/* ------------------------------------------------------------------------ */

static void ClearVRAM(void) {


/* - Type:	PRIVATE	
 * -
 * - Usage:	Clear entire contents of VRAM.
 */ 


	RECT	rectTL, rectTR,
			rectBL, rectBR;
	

	setRECT(&rectTL, 0, 0, 512, 256);
	setRECT(&rectTR, 512, 0, 512, 256);
	setRECT(&rectBL, 0, 256, 512, 256);
	setRECT(&rectBR, 512, 256, 512, 256);


	ClearImage(&rectTL, 0, 0, 0);
	ClearImage(&rectTR, 0, 0, 0);
	ClearImage(&rectBL, 0, 0, 0);
	ClearImage(&rectBR, 0, 0, 0);


	DrawSync(0); // Ensure VRAM is cleared before exit.
}

/* ------------------------------------------------------------------------ */

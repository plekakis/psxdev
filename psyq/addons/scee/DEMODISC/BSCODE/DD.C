/*+--------+----------+----------+----------+----------+----------+---------+
 * (C) Sony Computer Entertainment. All Rights Reserved.
 *                                                     
 * Name:   dd.c
 * Author: Allan J. Murphy
 *
 * Description:
 * ------------
 *
 * Changes & Bug Fixes:
 * --------------------
 * 23-Nov-95 (Vince) -- New ClearVram function added which actually clears all
 * of VRAM. This is called at the started and end of DD. We cannot rely on BS
 * clearing VRAM correctly.
 *+--------+----------+----------+----------+----------+----------+---------+
 */
 
#include <sys/types.h>
#include <kernel.h>
#include <libsn.h>
//#include <libcd.h>
#include "g:\program\psx\include.32\libcd.h"
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include <libspu.h>
#include <libsnd.h>

#include <ddp.h>
#include <dd.h>
#include <fsm.h>
#include <config.h>
#include <cd.h>
#include <fx.h>
#include <icons.h>
#include <snd.h>

#include <pad.h>

/*+--------+----------+----------+----------+----------+----------+---------+*/

#ifndef LINKED_NONE2
DDPData appData;
#endif

/*
** 31-oct-95 (pholman)
**      New Music for Future Disc
*/
# ifdef FUTURE_MAG
#       define VAB_NAME        "\\DD\\FUTCOV.VB;1"
# else
#       define VAB_NAME        "\\DD\\DD.VB;1"
# endif


DB db[2];

volatile int frameNumber = 0;                           /* No. of frames since DD started. */
volatile int bootTimeOut;                                       /* Timeout when m/c should be rebooted. */
volatile int transTimeout = NO_TIMEOUT;
volatile int fsmTimeout = 0;

char oldState;

int lastInput;                                                          /* Frame No. of last input. */
int stateTimeout;

volatile int dropOut = 0;                                       /* If 1 will drop out of DD main loop. */


DDPData *data;                                                          /* Ptr to appdata. */

unsigned long _ramsize   = 0x00200000;
unsigned long _stacksize = 0x00004000;


/*+--------+----------+----------+----------+----------+----------+---------+*/

void ClearVram(void) {


/* Vince (22-11-95) -- New clear VRAM function actually clears all of VRAM.
 * Even the 511th and 1023th lines which were not cleared before.
 */
 

	RECT			vram;
	DRAWENV			draw_env;
	LINE_F2			line;
	

	/* Clear VRAM using ClearImage (foes not clear 512th and 1024th lines. */	
	vram.x = vram.y = 0;
	vram.w = 1023;
	vram.h = 511;
	ClearImage(&vram, 0, 0, 0);
 
	/* Set up the draw env. */
 	SetDefDrawEnv(&draw_env, 0, 0, 1024, 512);
	draw_env.dfe = 1;				/* Allow drawing while VRAM is being displayed. */
	draw_env.isbg = 0;				/* Don't clear the background. */
	PutDrawEnv(&draw_env);
	
	/* Init and draw lines to clear the rest of VRAM (i.e. 512th and 1024th lines). */
	SetLineF2(&line);
	setRGB0(&line, 0, 0, 0);
	setXY2(&line, 0, 511, 1023, 511);
	DrawPrim(&line);						/* Clear 512th line of VRAM. */
	setXY2(&line, 1023, 0, 1023, 511); 
	DrawPrim(&line);						/* Clear 1024th line of VRAM. */
	
	/* Ensure VRAM is cleared before exit. */
	DrawSync(0);
	
	/* Set up an invalid drawing area. */
 	SetDefDrawEnv(&draw_env, 1024, 512, 0, 0);
	PutDrawEnv(&draw_env);
}

/*+--------+----------+----------+----------+----------+----------+---------+*/
/*
** 16-oct-95 (pholman)
**      Move ResetGraph as early as possible, to allow GPU to warm
**      up (and avoid unsightly flickers)
*/
void InitSys(ControllerPacket* buffers)
  {

/* Initialise the necessary sub-systems for the DD menuing program. */

     ResetCallback();
     
     ResetGraph(0);             

# ifdef DEBUG
     SetGraphDebug(0);   /* I.E. no debugging */
# endif     

     CdInit();

     /* Init the geometry engine. */
     InitGeom();
	  
     InitPAD(&buffers[0],MAX_CONTROLLER_BYTES,&buffers[1],MAX_CONTROLLER_BYTES);
     StartPAD();
     ChangeClearPAD(0);
     
     SetVideoMode(MODE_PAL);

     SndInit();
  } 

/*+--------+----------+----------+----------+----------+----------+---------+*/

void InitEnvs(DB* db)
  {

/* Set up the double buffered draw and display environments. */

	SetDefDrawEnv(&db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
	SetDefDispEnv(&db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);

    SetDefDrawEnv(&db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
	SetDefDispEnv(&db[1].disp, 0,   0,       FRAME_X, FRAME_Y);

   db[1].disp.screen.x = db[0].disp.screen.x = SCREEN_X;
   db[1].disp.screen.y = db[0].disp.screen.y = SCREEN_Y;

   db[1].disp.screen.h = db[0].disp.screen.h = PAL_MAGIC;
   db[1].disp.screen.w = db[0].disp.screen.w = PAL_MAGIC;

	db[0].draw.isbg = db[1].draw.isbg = 1;

	setRGB0(&db[0].draw,0,0,0);
	setRGB0(&db[1].draw,0,0,0);
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

void VsyncHandler()
  {

/* VSync callback handler which updates various important timers. */

	/* Reset transition timeout if input has been recieved. */
    if ( frameNumber == lastInput && stateTimeout != NO_TIMEOUT )
      transTimeout = stateTimeout;

	/* If in interactive mode and no input for IDLE_TIMEOUT frames change to attract mode. */     
    if ( data->mode != ATTRACT )
      if ( frameNumber - lastInput > IDLE_TIMEOUT  )
	{
	  ZeroBadKeys();
	  lastInput = 0xffffffff;
	  data->mode = ATTRACT;
	}

	/* If input received change to interactive mode. */     
    if ( frameNumber == lastInput )
      {
	repBlock = INPUT_DELAY;
	data->mode = INTERACTIVE;
      }

	/* Inc frame No. since DD started. */      
    frameNumber++;

	/* Check pads every PAD_CHECK_TIME frames. */
    if (!(frameNumber % PAD_CHECK_TIME))
      CheckPads();

    if (!(frameNumber % PAD_DECAY_TIME))
      SubBadKeys();

    if ( blocking > 0 )
      blocking--;

    if ( repBlock > 0 )
      repBlock --;

	/* Update timeout which determines when the current state should finish and the next started. */      
    if ( transTimeout > 0 )
      transTimeout--;

    if ( transTimeout == 0 )
      {
	fsmTimeout = 1;
	transTimeout--;
	data->mode = ATTRACT;
      }
      
#ifdef DEBUGx

	/* Update the bootTimeOut, if < 0 then reboot the m/c as something must have gone wrong. */      
    bootTimeOut--;

    if (bootTimeOut < 0)
      {
	 StopCallback();
	 EnterCriticalSection();
	 printf("DD: Boot time out, reboot, state %d.\n", data->state);
	 _boot();
      }
#endif /* DEBUG */
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

#ifdef LINKED_NONE2
void SetUp(int argc, char** argv)
#else
void SetUp()
#endif
  {
  
 /*
    printf("MDD: Defines: ");
    #ifdef DEBUG
      printf("Debug ");
    #endif
    #ifdef LINKED_NONE2
      printf("None2 ");
    #endif
    printf("\n");
*/
	/* Set up ptr to appdata in kernel RAM if DD is linked to none2.lib. */
    #ifdef LINKED_NONE2
      data = APPDATA_POS;
    #else
      /* If DD is to run standalone (i.e. linked to libsn) prime its own appdata structure. */
      int loop;

      data = &appData;

      data->cdb = 0;
      data->state = BOOT;
      data->selected = 0;

      data->randpos = 0;

     /* Init the currently selected options of each menu. */    
     for ( loop = 0; loop < NUM_DDP_MENUS; loop++ )
	data->menus[loop] = 0;
    #endif

    srand((frameNumber + data->state + data->selected) * data->randpos);

    #if 0
    printf("MDD: heap base: %x len %d.\n",__heapbase,__heapsize);
    printf("MDD: text base: %x len %d.\n",__text,__textlen);
    printf("MDD: bss base: %x len %d.\n",__bss,__bsslen);
    printf("MDD: data base: %x len %d.\n",__data,__datalen);
    __asm__ ("move %0,$sp": "=r" (usp):);
    printf("MDD: Stack base: %x (check %x).\n",usp,&usp);
    #endif
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

void CloseDown()
  {
    VSyncCallback(NULL);

    SndShutDown();

    StopCallback();
    StopPAD();
    ResetGraph(3);
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

#ifdef LINKED_NONE2
int main(int argc, char **argv)
#else
int main()
#endif
{
    #ifdef LINKED_NONE2
     SetUp(argc, argv);
   #else
     SetUp();
   #endif

   MakeConfig();

   InitSys((ControllerPacket *)buffers);

   InitFX();
   InitIcons();

   if (!StartLoad(VAB_NAME,(unsigned long *)__heapbase,NULL,CdlModeSpeed))
     vabTransferred = 1;

   InitEnvs(db);

   ClearVram();

   VSync(0);
   CheckPads();
   PadStatus();

   bootTimeOut = REBOOT;
   /*
   ** 11-oct-95 (pholman)
   **    GetTimeout maybe return NO_TIMEOUT
   */
   if ((transTimeout = GetTimeout(data->state)) != NO_TIMEOUT)
       transTimeout  *= SYNC_RATE;

   stateTimeout = transTimeout;

   TransferVab();

   VSyncCallback(VsyncHandler);

   PutDispEnv(&db[data->cdb].disp);
   
   SetDispMask(1);

   while(!dropOut)
     {
       bootTimeOut = REBOOT;
       oldState = data->state;
       data->state = NextState(data->state);
     }

   BlowOut(FX_FADEOUT);

   /* Clear VRAM completely just before exit. */
   ClearVram();
      
   CloseDown();

   #ifndef LINKED_NONE2
     printf("\n\nLoad set up, but not none2.\n");
   #endif

   printf("DO_DD; Dropout: Mode: %s, timeout %d, track %d, arg 3 %d.\n",
	  (data->args[0] == INTERACTIVE)?"interactive":"attract",
	  data->args[1],data->args[2], data->args[3]);

   data->mode = ATTRACT;
	return(0);
}

/*+--------+----------+----------+----------+----------+----------+---------+*/
/* End dd.c                                                                  */
/*+--------+----------+----------+----------+----------+----------+---------+*/

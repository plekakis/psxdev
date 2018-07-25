/*+--------+----------+----------+----------+----------+----------+---------+*/
/*                                                                           */
/* (C) Sony Computer Entertainment. All Rights Reserved.                     */
/*                                                                           */
/* Name:   bs.c                                                              */                      
/* Author: Allan Murphy                                                      */                     
/* Date:   8/6/95                                                            */                       
/*                                                                           */
/* Description:                                                              */            
/*                                                                           */
/* Boot program which loads first exe on the disk and runs it. The compiled  */
/* program takes up less than 32k; its stack is configured to be inside      */
/* this 32k; it only takes up less than 32k if it is compiled with bs.lib    */
/* which is a small lib with minimal functions for the boot strap.           */
/*                                                                           */
/*+--------+----------+----------+----------+----------+----------+---------+*/

#include <sys/types.h>
#include <kernel.h>
#include <libsn.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

#include "ddp.h"

/*+--------+----------+----------+----------+----------+----------+---------+*/

#define	FRAME_X		320		
#define	FRAME_Y		256

#define SCREEN_X     1
#define SCREEN_Y     18

#define FORM1_SIZE               2048 /* Size of an XA mode 1 form 1 sector. */

/*+--------+----------+----------+----------+----------+----------+---------+*/
/* Macros                                                                    */

/* This macro gives the number of sectors a file of length x bytes takes up. */

#define Sectors(x) ((x+FORM1_SIZE-1)/FORM1_SIZE)

/*+--------+----------+----------+----------+----------+----------+---------+*/

unsigned long _ramsize   = 0x00018000;  /* Pretend we have 96k only. */
unsigned long _stacksize = 0x00000100;  /* 256b stack */

/* The above places the stack 95k above the base of ram, ie 31k above the    */
/* kernel RAM. This means we have 31k for the bs program, and must use less  */
/* than 1k of stack, which is probably still a little too much.              */

extern unsigned long __heapbase;
extern unsigned long __heapsize;
extern unsigned long __bss;
extern unsigned long __bsslen;
extern unsigned long __data;
extern unsigned long __datalen;
extern unsigned long __text;
extern unsigned long __textlen;

DDPData appData;

unsigned long* rambase   = (unsigned long *)0x80018000;
struct XF_HDR head;

extern unsigned long icon[];

int bootTimeOut;
int first = 1;

/*+--------+----------+----------+----------+----------+----------+---------+*/

void InitSys()
  {
    ResetCallback();                      /* Reset all of the callbacks.      */
    CdInit();

    PadInit(0);

    ResetGraph(0);                        /* Reset the GPU.                   */
    SetDispMask(1);
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

void InitEnvs()
  {
	SetDefDrawEnv(&appData.db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
	SetDefDispEnv(&appData.db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);

   SetDefDrawEnv(&appData.db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
	SetDefDispEnv(&appData.db[1].disp, 0,   0,       FRAME_X, FRAME_Y);

   appData.db[1].disp.screen.x = appData.db[0].disp.screen.x = SCREEN_X;
   appData.db[1].disp.screen.y = appData.db[0].disp.screen.y = SCREEN_Y;

   appData.db[1].disp.screen.h = appData.db[0].disp.screen.h = FRAME_Y;
   appData.db[1].disp.screen.w = appData.db[0].disp.screen.w = FRAME_X;

   appData.db[1].disp.pad0 = appData.db[0].disp.pad0 = 1; 

	appData.db[0].draw.isbg = appData.db[1].draw.isbg = 1;
	setRGB0(&appData.db[0].draw,0,0,0);
	setRGB0(&appData.db[1].draw,0,0,0);

  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

void ClearVram()
  {
     RECT vram;

     vram.x = vram.y = 0;
     vram.w = 1024;
     vram.h = 512;
    
     ClearImage(&vram,0,0,0);
     VSync(0);
     ClearImage(&vram,0,0,0);
     VSync(0);
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

int LoadProg()
 {
   unsigned long *ptr;
   CdlFILE fp;
   int	mode = CdlModeSpeed;	
   int offset;
   
   if (!CdControl(CdlSetloc, (unsigned char *)&(appData.pos), 0))
     return 0;

   if (!CdRead(Sectors(FORM1_SIZE), (unsigned long *)rambase, mode))
     return 0;
	
   while (CdReadSync(1,0) > 0 );

   memcpy(&head,rambase,sizeof(head));

	offset = CdPosToInt(&appData.pos);
	offset++;
	CdIntToPos(offset,&appData.pos);

   /* Clear BSS */ 


   printf("T addr: %x, clear to %x.\n",head.exec.t_addr,head.exec.s_addr);
   for ( ptr = (unsigned long *)head.exec.t_addr;
         ptr < (unsigned long *)head.exec.s_addr; ptr++ )
     *ptr = 0L;

   if ( !CdControl(CdlSetloc, (unsigned char *)&(appData.pos), 0))
     return 0;

   return CdRead(Sectors(head.exec.t_size),
                 (unsigned long *)head.exec.t_addr, mode);
}

/*+--------+----------+----------+----------+----------+----------+---------+*/

int GoProg()
  {
     printf("Exec, go at: %x; stack: %x.\n",head.exec.pc0,head.exec.s_addr);

 	  SetDispMask(0); 
     StopCallback(); 
     ResetGraph(3);
     PadStop();

     /* This is example data passed to a playable segment. */

     appData.args[0] = INTERACTIVE;  /* Eg interactive mode. */
     appData.args[1] = 60;           /* 1 minute timeout */
     appData.args[2] = 5;            /* Using CD track 5 */

     EnterCriticalSection();
     Exec(&(head.exec),2,appData.args);
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

void DoLogo()
  {
     RECT image;
     image.w = 20;
     image.h = 20;
     image.x = 150;
     image.y = 110;

     LoadImage(&image,icon+5);

     image.y += FRAME_Y; 

     LoadImage(&image,icon+5);
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

void SyncLoad()
  {
     while (CdReadSync(1,0) > 0 );
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

void timer()
  {
     bootTimeOut--;

     if (bootTimeOut < 0)
       {
         printf("BS: Time out, reboot.\n");
         EnterCriticalSection();
         VSyncCallback(NULL);
         _boot();
       }
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/

void main()
  {
    int usp;

    printf("heap base: %x len %d.\n",__heapbase,__heapsize);
    printf("text base: %x len %d.\n",__text,__textlen);
    printf("bss base: %x len %d.\n",__bss,__bsslen);
    printf("data base: %x len %d.\n",__data,__datalen);
    __asm__ ("move %0,$sp": "=r" (usp):);
    printf("Stack base: %x.\n",usp);
    printf("&appdata: %x.\n",&appData);

    appData.loadGame = 0;
    appData.cdb = 0;

    while(1)
      {
        printf("BS: Go inits.\n");
        InitSys();

        bootTimeOut = BOOTTIMEOUT;
        VSyncCallback(timer);   

        ClearVram();
        bootTimeOut = BOOTTIMEOUT;

        InitEnvs();
        bootTimeOut = BOOTTIMEOUT;

        PutDrawEnv(&appData.db[appData.cdb].draw); 
        PutDispEnv(&appData.db[appData.cdb].disp);


        if ( !appData.loadGame )
          {
             /* This is the logical position of the program to load */
             /* Dom, you can stick a different location in here. */

             printf("BS: Set loc for seek.\n");
             appData.pos.track  = FSM_TRACK;
             appData.pos.minute = FSM_MINUTE;
             appData.pos.second = FSM_SECOND;
             appData.pos.sector = FSM_SECTOR;
          } 
 
        appData.loadGame = 0;

        bootTimeOut = BOOTTIMEOUT;

        while(!LoadProg())
          {
             #ifdef DEBUG
               pollhost();
             #endif
          }

       bootTimeOut = BOOTTIMEOUT;

       if ( first )
          { 
            DoLogo();
            first = 0;
          }

        bootTimeOut = BOOTTIMEOUT;

        SyncLoad();
        VSyncCallback(NULL);

        printf("BS: Program go.\n");
        GoProg();

        printf("BS: Returned ok; load Game: %d.\n",appData.loadGame);


        if ( appData.mode == ERROR )
          printf("Bad return from sub, loading DDP.\n");
      }
  }

/*+--------+----------+----------+----------+----------+----------+---------+*/
/* End menu.c                                                                */
/*+--------+----------+----------+----------+----------+----------+---------+*/


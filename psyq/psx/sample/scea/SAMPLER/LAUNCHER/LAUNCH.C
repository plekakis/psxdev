#include <sys/types.h>
#include <kernel.h>
#include <libcd.h>
#include <libetc.h>
#include <assert.h>
#include <stdio.h>
#include "launch.h"

/* Modify these values to match your application */
typedef struct {
  char *Name;                       /* app filename - used for dbug*/
  int Sector;                       /* IMPORTANT!!! -  Sector whereapplication's executable begins */
  int FirstDATrack;
  int DirSector;                    /* sector # of the directory containing your app */
} AppInfo;                         
#define NUM_APPS 3

AppInfo DemoApps[NUM_APPS] = {
   /* Shadow sample - Child 1*/
  {"\\SHADOW\\SHADOW.EXE;1", 24, 0, 23},
   /* Hi-Res 60 fp balls - Child 2 */
  {"\\FSBALLS\\FBALLS.EXE;1", 53, 0, 52},     
  /* menu app - allows users to select which 
    game to play also managesthe attract mode loop*/
  {"\\MENU\\MAIN.EXE;1", 76, 0, 75}         
  
};  



int ExecMode = MODE_INTERACTIVE; // This is the mode that your program will
                                 // be launched in. 
int CurrentApp = MAX_ARGS - 1;   // launch the menu first!
int LastApp = 0;
int FirstTime = 1;

#define NONINTERACTIVE_TIMEOUT 60   /* application should timeout after 60
				       seconds when launched in non_interactive
				       mode. */


/*+--------+----------+----------+----------+----------+----------+---------+*/

#define FORM1_SIZE               2048 /* Size of an XA mode 1 form 1 sector. */

/* This macro gives the number of sectors a file of length x bytes takes up. */
#define Sectors(x) ((x+FORM1_SIZE-1)/FORM1_SIZE)

unsigned long _ramsize   = 0x00018000;  /* Pretend we have 96k only. */

unsigned long _stacksize = 0x00000100;  /* 256b stack */

unsigned long* rambase   = (unsigned long *)0x80018000;
struct XF_HDR head; 

int resetTimeout;

int LoadProg(int sector)
 {
   unsigned long *lowMem, *highMem;
   signed char result = CdlDiskError;
   unsigned long *ptr;
   CdlLOC AppLOC;

   int offset;
   printf("launcher: Loading App from sector %d\n",sector);
   
   CdIntToPos(sector,&AppLOC);

   while (result != CdlComplete) {
     if (!CdControl(CdlSetloc, (unsigned char *)&(AppLOC), 0))   /* SEEK */
       return 0;
     CdSync(0,&result);
   }

   
   if (!CdRead(Sectors(FORM1_SIZE), rambase, CdlModeSpeed))
     return 0;

   while (CdReadSync(1,&result) != 0) {
     if (result < 0) return 0;
     if (resetTimeout <= 0) {
       return 0;
     }
   }
   memcpy(&head,rambase,sizeof(head));

   offset = CdPosToInt(&AppLOC);
   offset++;
   CdIntToPos(offset,&AppLOC);

   /* Clear BSS */
   lowMem = MIN((ulong *)head.exec.t_addr,(ulong *)head.exec.s_addr);
   highMem = MAX((ulong *)head.exec.t_addr, (ulong *)head.exec.s_addr);

   printf("launcher: Clear From 0x%X,  0x%X.\n", (unsigned int)lowMem, 
	  (unsigned int)highMem);

   for ( ptr = lowMem; ptr < highMem; ptr++ ) *ptr = 0L;

   result = CdlDiskError;
   CdSync(0,NULL);
   while ( result != CdlComplete) {
     if ( !CdControl(CdlSetloc, (unsigned char *)&(AppLOC), 0))
       return 0;
     CdSync(0,&result);
   }
   
   if (!CdRead(Sectors(head.exec.t_size),(unsigned long *)head.exec.t_addr, 
	       CdlModeSpeed)) return 0;
   CdReadSync(0,&result);
   if (result < 0) 
     return 0;      
   return 1;
}


unsigned long AppArgs[4];
void GoProg(int app)
{
  StopCallback();
  VSync(0);

  if (app != (NUM_APPS - 1)) {                /* this is not the menu app, 
						 setup arguments normally. */
    AppArgs[0] = ExecMode;                    /* argv[0] = execution mode. */
    AppArgs[1] = NONINTERACTIVE_TIMEOUT;      /* argv[1]=timeout for attract */
    AppArgs[2] = DemoApps[app].FirstDATrack;  /* argv[2]=# of first DA track */
    AppArgs[3] = DemoApps[app].DirSector;   /* argv[3] = # of DA tracks */
    LastApp = CurrentApp;
    CurrentApp = NUM_APPS - 1;
  }
  else {                                      /* setup menu args */
    CurrentApp = LastApp;
    AppArgs[0] = (int)(&CurrentApp);
    AppArgs[1] = (int)(&ExecMode);
    AppArgs[2] = NUM_APPS;
    AppArgs[3] = (int)(&FirstTime);
  }
  printf("launcher: GO! (args: %d, %d, %d, %d)\n",(int)AppArgs[0],(int)AppArgs[1],
	 (int)AppArgs[2],(int)AppArgs[3]);

  EnterCriticalSection();
  Exec(&(head.exec),4,(char **)AppArgs);
  ExitCriticalSection();
  printf("launcher: We're Back!\n");
  CdReset(2);
}

/*+--------+----------+----------+----------+----------+----------+---------+*/

void timer()
{
  resetTimeout--;
}

/*+--------+----------+----------+----------+----------+----------+---------+*/

void main()
{
  int usp;
  __asm__ ("move %0,$sp": "=r" (usp):);

  ResetCallback();
  while(1)
    {
      CdInit();
      VSyncCallback(timer);  
      resetTimeout = RESETTIMEOUT;
      while(!LoadProg(DemoApps[CurrentApp].Sector)) {
	printf("launcher: LoadProg Failed. CdInitting...\n");
	CdInit();
	VSync(6);
	CdSync(0,NULL);
	resetTimeout = RESETTIMEOUT;
      }

      VSyncCallback(NULL);
	
      GoProg(CurrentApp);

      if (ExecMode == MODE_INTERACTIVE) 
	ExecMode = MODE_NONINTERACTIVE;
      else ExecMode = MODE_INTERACTIVE;
    }
}

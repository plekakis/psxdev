$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

module\execmenu: EXE file execution

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.
                                            May 2, 1997 Ver.1.6 by yoshi


<Description>

  This is a sample program in which anothor EXE files (BALLS, RCUBE,
  ANIM, BALLS2, RCUBE2, ANIM2) are loaded and carried out by a parent 
  EXE file (MENU) during its execution.
  balls\main.c is a modification of sample\graphics\basic\balls.c.
  rcube\main.c is a modification of sample\graphics\tmdview\rcube\main.c.
  anim\main.c is a modification of sample\cd\movie\tuto0.c.

<How to Execute>

DOS> psymake all
DOS> cd balls
DOS> psymake all
DOS> cd ..\rcube
DOS> psymake all
DOS> cd ..\anim
DOS> psymake all

Compile the files in the order as described above, and create a CD-R
with the following structure.
The library CD-ROM contains these data in the following directory.

        RCUBE.TIM:      \PSX\DATA\TIM\RCUBE.TIM
        RCUBE.TMD:      \PSX\DATA\TMD\RCUBE.TMD
        MOV.STR:        \XDATA\STR\MOV.STR

MENU will be started up by booting.
With the up-down key and the start button, each EXE file will be read 
and executed.
With the select button, the sample will return to MENU.

\ ------+--- SYSTEM.CNF			Described in the latter part.
        |
        |
        +--- EXECMENU --+--- EXECMENU.EXE
        |               +--- BALLS.EXE
        |               +--- RCUBE.EXE
        |               +--- ANIM.EXE
        |               +--- BALLS2.EXE
        |               +--- RCUBE2.EXE
        |               +--- ANIM2.EXE
        |
        +--- DATA ------+--- RCUBE.TIM
                        +--- RCUBE.TMD
                        +--- MOV.STR

file: SYSTEM.CNF (A space must be inserted on either side of '='.)
---------------------------------------------------
BOOT = cdrom:\EXECMENU\EXECMENU.EXE;1
TCB = 4
EVENT = 16
STACK = 801fff00
---------------------------------------------------


<Controller Button Assignment>

        Up Directional Button   : Select child program
        Down Directional Button : Select child program
        Start Button            : Read and run child program 
        Select Button           : Escape from child program


<Remarks>

(1) Notes for the initial stack value (SP)

     When the usual module is generated, the start-up routine in LIBSN.LIB
     is linked to the module.
     The start-up routine set SP value, and calls InitHeap().

     But, when you generate the child module to set SP and calls InitHeap() 
     as the usual module, you may destroy the parent module on the memory.
     So PSX\LIB\none3.obj was supplied to replace with the start-up routine.
     You should use none3.obj when you generate the child module.
     None3.obj don't set SP and don't call InitHeap().


(2) EXE files reading and execution

  There are 2 ways to start up the EXE in this sample.
  The following are the details for each processing.


   [2-1] When the parent EXE load the child EXE,

        Link none3.obj to the child EXE.

   [2-1-A] When only the CD functions are used, 

        This refers to BALLS, RCUBE, ANIM in the sample menu.
        The cdload() function, which performs the same processing as 
        the Load() with only the CD functions, is used.
        Refer to the source code of the cdloasd() in the last part. 

	< parent EXE >

	struct EXEC exe;
				[pre-processing]
	ResetCallback();	_96_remove() is called inside.
	CdInit();		CD function initialization
	CdSearchFile();		[Data file reading]
	CdRead();
		:
		:
		:
		:
	cdload("\\GAME.EXE;1",&exe);	[Loading child EXE]
					Original function. 
                                        See [4-3] for details.
	DrawSync(0);			[post- processing]
	StopRCnt(RCntCNT0);		Stop every interrupt processing 
	StopRCnt(RCntCNT1);		such as sound and events.
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)			ResetGraph(0) is also available.
	PadStop();
	StopCallback();			
	exe.s_addr = 0x8017fff0;	[Child stack pointer setting]
	exe.s_size = 0;
	EnterCriticalSection():
	Exec(&exe,1,0);			[Child EXE execution]
	ResetCallback();		[Executing pre-processing again]
	CdInit();
		:
		:


	< child EXE >

	ResetCallback();	[pre-processing]
	CdInit();
	CdSearchFile();		[Data file reading]
	CdRead();
		:
		:
		:
		:
	DrawSync(0);		[post-processing]
	StopRCnt(RCntCNT0);
	StopRCnt(RCntCNT1);
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0) is also available.
	PadStop();
	StopCallback();

	return();		[Returning from main]



  [2-1-B] When using 96 File System,

	This sample MENU.C doesn't adopt it.
        An example for using 96 File System is as follows:
        When using 96 File System (for example, Load(), open(),read(), 
        and LoadExec()), read it after the _96_init() execution, and
        immediately carry out the  _96_remove().        
	Never forget to carry out the _96_remove().
        Without this function, any processing other than the access to 
        96 File System will not be ensured.
        Also, the heavy use of the _96_init() will reduce the game   
        processing speed much because it takes long to be processed.
        Therefore, the way [2-1-A] is recommended.


	< parent EXE >

	struct EXEC exe;
				[pre-processing]
	ResetCallback();	_96_remove() is called inside.
	CdInit();		CD function initialization
				[Data file reading]
	_96_init();		For accessing 96 File System,
	read();		put the function between _96_init() and 
	_96_remove();		_96_remove().
		:
		:
		:
		:
	DrawSync(0);		[post-processing]
	StopRCnt(RCntCNT0);	Stop every interrupt such
	StopRCnt(RCntCNT1);	as sound, and events.
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0) is also available.
	PadStop();
	StopCallback();
	_96_init();		Call _96_init() following StopCallback().
	Load("cdrom:\\GAME.EXE;1",&exe);[Loading child EXE]
	exe.s_addr = 0x8017fff0;	[Child stack pointer setting]
	exe.s_size = 0;
	EnterCriticalSection():
	Exec(&exe,1,0);		[Child EXE execution]
	ResetCallback();	[Executing pre-processing again]
	CdInit();
		:
		:
	

	< child EXE >

	ResetCallback();	[pre-processing]
	CdInit();
				
	_96_init();		[Data file reading]
	read();			
	_96_remove();		
		:
		:
		:
		:
	DrawSync(0);		[post-processing]
	StopRCnt(RCntCNT0);
	StopRCnt(RCntCNT1);
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0) is also available.
	PadStop();
	StopCallback();

	return();		[Returning from main]




  [2-2] When EXE on the memory is replaced by the another EXE,

       This refers to BALLS2, RCUBE2, ANIM2 in the sample menu.
       The LoadExec(), system call function, is used.

	< parent EXE >

	ResetCallback();	[pre-processing]
	CdInit();
	CdSearchFile();		[Data file reading]
	CdRead();
		:
		:
		:
		:
	DrawSync(0);		[post-processing]
	StopRCnt(RCntCNT0);	Stop every interrupt such
	StopRCnt(RCntCNT1);	as sound, and events.
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0) is also available.
	PadStop();
	StopCallback();	Call _96_init() following StopCallback().
	_96_init();
	LoadExec("cdrom:\\GAME.EXE;1",0,0); [Child EXE loading and execution]


	< child EXE >

	ResetCallback();	[pre-processing]
	CdInit();
	CdSearchFile();		[Data file reading]
	CdRead();
		:
		:
		:
		:		If going back to parent EXE, load it again.
	DrawSync(0);		[post-processing]
	StopRCnt(RCntCNT0);
	StopRCnt(RCntCNT1);
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0) is also available.
	PadStop();
	StopCallback();
	_96_init();
	LoadExec("cdrom:\\PSX.EXE;1",0,0); [Child EXE loading and execution]




  [2-3] EXE file loading function: cdload()

       Refer to the following source of cdload(), which achieves the 
       similar operation with the Load() with the CD function.

	Example:
	struct EXEC exe;

	CdInit();
	cdload("\\GAME\\GAME.EXE;1",&exe);


cdload.c
-----------------------------------------------------------------
#include <kernel.h>
#include <libcd.h>

#define H_SIZE 2048


int
cdload(file,exep)

char *file;
struct EXEC *exep;
{
  CdlFILE	fp;			/* File location, size information */
  int	mode = CdlModeSpeed;	
  char hbuf[H_SIZE];			/* Buffer for EXE header */
  unsigned long offset;		/* Absolute sector location */
  int cnt,i;
  struct XF_HDR *head;

  for (i = 0; i < 10; i++) {	/* 10 times retrying */
    if (CdSearchFile(&fp, file) == 0) 
      continue;

    /* Target position settng */
    CdControl(CdlSetloc, (u_char *)&(fp.pos), 0);
    cnt = _read1(H_SIZE,(void *)hbuf,mode);   /* EXE header reading */
    if(cnt)
      continue;

    head = (struct XF_HDR *)hbuf;		
    memcpy((void *)exep, (void *)&head->exec, sizeof(struct EXEC));

    /* Convert the location information to the absolute sector.
      Increment it by 1 sector ( header of EXE file).
      Re-convert the sector to the loaction information.
    */
    offset = CdPosToInt(&fp.pos);
    offset++;
    CdIntToPos(offset,&fp.pos);

    /* Target position setting */
    CdControlB(CdlSetloc, (u_char *)&(fp.pos), 0);
    /* Program reading */
    cnt = _read1(exep->t_size, (void *)exep->t_addr, mode);

    /* Break if the normal end */
    if (cnt == 0)
    	return(0);	/* succeed */
  }

  return(-1);		/* fail */
}


int
_read1(byte,sectbuf,mode)

long byte;
void *sectbuf;
int mode;
{	
  int nsector,cnt;
	
  nsector = (byte+2047) / 2048;
	
  /* Starting to read */
  CdRead(nsector, sectbuf, mode);
	
  /* Normal processing can be performed in the background of reading. */
  /* Monitoring the number of left sectors until reading is completed. */
  while ((cnt = CdReadSync(1, 0)) > 0 ) {
    VSync(0);
  }

  return(cnt);
}

																		



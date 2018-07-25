$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

 << EXE file execution >>

 Sep 4, 1995 ver.1.5  by yoshi



 (1) Brief
  This is a sample program in which child EXE files (BALLS, RCUBE,
  ANIM, BALLS2, RCUBE2, ANIM2jare loaded and carried out by a parent 
  EXE file (MENU) during its execution.
  balls\main.c is a modification of sample\graphics\balls\main.c.
  rcube\main.c is a modification of sample\graphics\tmdview\rcube\main.c.
  anim\main.c is a modification of sample\cd\movie\tuto0.c.

  How to start up the sample:
---------------------------------------------------

DOS> psymake all
DOS> cd balls
DOS> psymake all
DOS> cd ..\rcube
DOS> psymake all
DOS> cd ..\anim
DOS> psymake all

Compile the files in the order as described above, and create a CD-R
with the following structure.
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
        +--- DATA ------+--- RCUBE.TIM	Use the file on DTL-S2180.	
                        +--- RCUBE.TMD	Use the file on DTL-S2180.	
                        +--- MOV.STR	Use the file on DTL-S2180.

---------------------------------------------------

file: SYSTEM.CNF (A space must be inserted on either side of '='.)
---------------------------------------------------
BOOT = cdrom:\EXECMENU\EXECMENU.EXE;1
TCB = 4
EVENT = 16
STACK = 801fff00
---------------------------------------------------



  (2) bug-fix : CdInit() function in the child EXEC

     With old libraries version 3.1, without CdInit() execution in the   
     child EXE, the first CdInit() execution after coming back to the parent 
     EXE may have the program hung up. 

     With new libraries version 3.2, this bug was fixed.



  (3) Notes for using 2mbyte.obj, none2.obj and malloc()
     
     Module to which 2mbyte.obj is linked:
       In starting up, the following processing is carried out.
       Bss area is cleared by zero.
       A stack pointer value is set at 0x80200000.
       The InitHeap() is called.

     Module to which none2.obj is linked:
       In starting up, no processing is carried out.
       InitHeap() is not called. 
       Thus, carry out the InitHeap() to use the malloc().
       
  Usually 2mbyte.obj is linked to the parent EXE.
  2mbyte.obj must be linked to the child EXE, and the initial stack
  value must be designated in the parent EXE. 

  When the malloc() is used in the parent EXE, in consideration of the 
  area to load the child EXE, the InitHeap() must be executed.
  As is the case with using it in the child EXE, in consideration of the  
  area occupied by the parent EXE, the InitHeap() must be executed. 


  (4) EXE files reading and execution

  There are 2 ways to start up the child EXE in this sample.
  The following are the details for each processing.


   [4-1] When the parent EXE is left on the memory,
        (when the child EXE is loaded to the location where the parent 
         EXE is not overlapped,)

        Link 2mbyte.obj to the parent EXE, and none2.obj to the child EXE.

   [4-1-A] When only the CD functions are used, 

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



  [4-1-B] When using 96 File System,

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
	 Therefore, the way [4-1-A] is recommended.


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




  [4-2] When the parent EXE is not left on the memory,
       (when the child EXE is loaded to the location where the parent EXE 
        is overlapped,)

       This refers to BALLS2, RCUBE2, ANIM2 in the sample menu.
       The LoadExec(), system call function, is used.
       Link 2mbyte.obj to the parent EXE, and 2mbyte.obj or none2.obj
       to the child EXE.
       ( If 2mbyte.obj is linked to the child  EXE, the stack pointer 
       setting of the second argument of the LoadExec() does't make    
       sense.)  

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
	LoadExec("cdrom:\\GAME.EXE;1",0x801fff00,0);	[Child EXE
                                                     loading and execution]
						Stack pointer designation
                                                       at the second argument


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
	LoadExec("cdrom:\\PSX.EXE;1",0x801fff00,0);	[Child EXE
                                                   loading and execution]




  [4-3] EXE file loading function: cdload()

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

																		


$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

module\overmenu : Overlaying 

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.
                                            May 2, 1997 Ver.1.5 by yoshi



<Description>

The feature with PlayStation can be described as follows:  

 "Libraries are shared by the parent and the child processes, and the 
parent file (.EXE file) can read the child files (.BIN files) dynamically, 
and execute them. However, reading must be managed by a user."  

In this sample, the parent (menu) calls the children (balls, rcube,
anim) by overlaying, and executes the children.   
BALLS.C is a modification of sample\graphics\balls\main.c.
RCUBE.C is a modification of sample\graphics\tmdview\rcube\main.c.
ANIM.C is a modification of sample\cd\movie\tuto0.c.


<How to Execute>

Make sure that ccpsx is version 1.06 or later, and that aspsx is version 2.10 or later.

DOS> psymake all

Compile the files as described above, and create a CD-R with the following 
structure.
The library CD-ROM contains these data in the following directory.

	MUTUAL2.SEQ:	\PSX\DATA\SEQ\MUTUAL2.SEQ
	MUTUAL.VH:	\PSX\DATA\VAB\MUTUAL.VH
	MUTUAL.VB:	\PSX\DATA\VAB\MUTUAL.VB
	RCUBE.TIM:	\PSX\DATA\TIM\RCUBE.TIM
	RCUBE.TMD:	\PSX\DATA\TIM\RCUBE.TMD
	MOV.STR:	\XDATA\STR\MOV.STR

MENU will be started up by booting.
With the up-down key and the start button, BALLS, RCUBE, or ANIM will be read into with the music played, and be executed.
With the select button, the sample will return to MENU. 

\ ------+--- SYSTEM.CNF
	|
	|
	+--- OVERMENU --+--- OVERMENU.EXE
	|		+--- BALLS.BIN
	|		+--- RCUBE.BIN
	|		+--- ANIM.BIN
	|
	+--- DATA ------+--- MUTUAL2.SEQ
			+--- MUTUAL.VH
			+--- MUTUAL.VB
			+--- RCUBE.TIM
			+--- RCUBE.TMD
			+--- MOV.STR

file: SYSTEM.CNF ( A space must be inserted on either side of '='.)
---------------------------------------------------
BOOT = cdrom:\OVERMENU\OVERMENU.EXE;1
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

  1) How to handle functions and variables in the child process

  Since loading the child process must be managed by the user, handling  
  'the functions and the variables in the child process' must be 
  performed carefully.
   
  (1) When the parent process accesses the functions and the variables 
     in the child process, the child process must be loaded into the memory
     beforehand. 
  
  (2) The functions and the variables in the child process loaded into  
     the memory are free to be accessed. Yet, the moment other child 
     processes are overlaid, the contents will be changed. Keep it in 
     mind in coding.

  2) .BIN file

  Usually the following are included in the EXE file.

  Header
  .rdata session
  .text session
  .data session
  .sdata session

  .sbss and.bss are not included.

  On the other hand, all sessions in the child process are included in the 
  .BIN file output by the assembler. (Refer to MENU.LNK.)
  For this reason, this file is rather long, compared with the usual EXE 
  file. (.sbss and .bss sessions are filled with zero.)

  For shortening the loading time, and saving the disk capacity, .bss 
  and .sbss sessions can be removed on the user's own responsibility.
  In this case, if zero-clearing is necessary, it must be executed in 
  the program. 

 3) File list

  MAKEFILE.MAK	Make file
  MENU.LNK	Link file
  ADDRESS.S	Assembler file
  MENU.C	Parent: MENU source file
  SND.C		Parent: MENU source file
  FONTTEX1.C	Parent: MENU source file
  BALLTEX.H	Child: BALLS header file
  BALLS.C	Child: BALLS source file
  TABLE.H	Child: RCUBE header file
  RCUBE.C	Child: RCUBE source file
  ANIM.C	Child: ANIM source file

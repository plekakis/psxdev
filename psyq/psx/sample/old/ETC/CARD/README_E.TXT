$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
Sample Program for Memory Card


< Overview >

This sample program is the extract from the Memory Card Management Tool
built into the PlayStation (The US screen design is used).  A part of 
the actual source code is disclosed for reference of writing 
the Memory Card access program.

Both screen image and Memory Card handling are the same as for 
the PlayStation. 

Since only the Memory Card handling module source code can be disclosed, 
for other modules, such as Menu System handling module, libraries are 
provided instead.  Please refer to the attached document for details of 
the libraries. 

Icon Generation Tool for the Memory Card Screen is also included.
Please make use of it as necessary.


< Files >

Files included are as follows:
(Note that the files with "*" are important.)


	readme_e.txt	        This document

	doc\
		superx_e.txt	document for super-x library
		turtle_e.txt	document for turtle graphics library
		malloc_e.txt	document for malloc related
	lib\
		superx.lib	super-x library
		turtle.lib	turtle graphics library


        max\
		cardio.h  	Memory Card management header
	      * card.c		Memory Card management source code
		makefile.mak	psymake makefile
		main.lnk	(for psylink)
		menu.lnk	(for PCMENU)
		makefile.6x	NEWS makefile
		psxload.h	super-x library header
		spritex.h	same as above
		menux.h		same as above
		turtle.h	turtle library header
		memeng.h	image data file
		memjpn.h	same as above
		pad.h		same as above
		mouse.h		same as above
		pencila.h	same as above
		libps.h		psx library header
		const.h		Menu System header
		maxtypes.h	same as above
		maxstat.h	same as above
		maxproto.h	same as above
		menuset.h	same as above
		menuset2.h	same as above
		vsync.h		same as above
		emouse.h	        same as above
		maxconf.h	English/Japanese Switch header

		max.c		main routine source code
		vsync.c		basic drawing Routine source code
		pad.c	        controller management source code
		emouse.c        controller/mouse management source code
		maxinit.c	Menu System initialize source code
		drawmax.c	Menu Image drawing routine source code
		menuset.c	Menu System handler source code
		
	makecard\
		makefile.mak	psymake makefile
		makefile.lnk	(for psylink)
                makefile.6x	NEWS makefile 
		hand1.h		hand gesture "Rock" image data file
		hand2.h		hand gesture "Scissors" image data file 
		hand3.h		hand gesture "Paper" image data file
	      * makecard.c	Icon Generation program source code


< How to use >

1.How to execute Memory Card Screen Sample Program
   	
  Compile the sample program under the subdirectory "max\".
  Default compilation environment is used. If necessary, modify 
  "makefile.mak" and "main.lnk."

	> cd max
	> psymake

  After compilation, execute the program.

	> run main


  Memory Card Screen will appear followed by a blank screen for a few 
  second. 
  Memory Card Screen appears even when the Memory Card is not inserted.
  Memory Card can be inserted or removed at anytime after the program start. 
  Please note that it may take some time until all the icons are displayed.






  Key assignments for the Memory Card Screen

	Directional Buttons           Move the cursor
       	O,X,square,triangle Buttons   Select item
       	START Button		      Cancel
	SELECT Button		      End sample program	
        L1,L2,R1,R2 Buttons           Cancel DELETE
        concurrently

	Operation Steps
	- press directional buttons to move cursor up/down to 
          the desired item, "COPY","COPY ALL", or "DELETE" from 
          the menu.
	- Press any one of four right cross buttons(e.g. O button) 
          to select item. The cursor is moved onto one of 
          the Card 1 icons.
	- Press up/down directional button to move cursor among 
          the icons.
	- Press left/right directional button to move cursor onto 
          the Card1/Card2 icons respectively.
	- In order to go back to the item selection, press START 
	  button.  
	- Once the icon is selected, prompt message appears. 
          Select YES/NO for desired operation.  During execution, 
	  enjoy the animation icons changing for different operations. 

  Pressing all four L1,L2,R1, and R2 buttons concurrently cancels 
  DELETE operation and icons will appear again.

  Mouse Key Assignment

	Left Button		Select
	Right Button		Cancel

	Left/Right Buttons 	Cancel DELETE
        concurrently

  The same operation steps can be applied to the mouse. That is 
  to select item first then icon for the desired memory card.



  The controller or mouse can be attached to Port 1 or/and Port 2. 
  Only Port 1, however, is recognized when both Port 1 and Port 2 
  are used.


2.How to execute Icon Generation Sample Program

  Compile the sample program under the subdirectory "makecard\".

	> cd makecard
	> psymake
	> run makecard

  Although nothing is displayed on the screen during execution of 
  "makecard", issuing "run" command can check if the program has 
  ended or not as follows; 

  When the program "makecard" is still under execution, following 
  message will appear;

	"Cannot connect to target"

  Please wait for little longer for the program to end.

  If the program has ended, following message will appear;

	"No file to process"


< Memory Card Access Functions >

In this sample program, all the functions related to Memory Card access 
handling are described in "max\card.c".  Although the document for the
Memory Card access is not provided, refer comments of "card.c" for 
detail operation.

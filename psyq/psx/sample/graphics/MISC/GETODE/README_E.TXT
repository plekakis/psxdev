$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
getode	Special Notes on Using Vsync with Interlace Mode

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>
  	In related to the Interlace mode drawing of PlayStation GPU 
	(Graphics Processor), following problem was encountered with some 
	titles. This document is to inform you of the detail description 
	of the problem analysis and the action to be taken to avoid the 
	problem.  For those of you who are designing the titles using the 
	mode, please follow the instructions below.  Thank you.

	[Problem Descriptions]
	While using the interlace single buffer(vertical-480 dot-mode) AND
	the drawing switch is done by VsyncCallback() instead of Vsync(), 
	only the odd number fields are canceled.  Thus this would cause the 
	residual image problem since the background screen is not completely 
	cleared.


	[Cause]
  	With the Interlace mode, odd number fields and even number fields 
	are displayed alternately at every 1/60[sec].  In other words, in 
	case for the 640x480 mode, for example, odd and even lines are 
	displayed alternately in vertical (y) direction.
	At this point, GPU will draw lines depending upon the fields being 
	displayed as follows;

	- While displaying odd fields, only even lines are drawn.
	- While displaying even fields, only odd lines are drawn. 

	This would implement the mechanism where the screen will not be 
	displayed until the drawing is completed for one line.

  	To support this mechanism, GPU needs to know whether current video 
	output is to the odd or even fields.  However, a problem was found 
	in GPU; current GPU always recognizes the video output being the 
	even fields during V-BLNK as below:  

        	------------------------------------------------------
	Video   |   even   |V-BLNK|   odd   |V-BLNK|   even   |V-BLNK|
        	------------------------------------------------------
        	------------------------------------------------------
	GPU     |   even          |   odd   |          even          |
        	------------------------------------------------------

  	VSyncCallback() is called at the beginning of V-BLNK(not at the end). 
	Therefore drawing is done only to the odd fields during V-BLNK.
	Usually the first drawing is done to the background and because of 
	this, it is possible that a part of the background is not either 
	cleared or drawn.


	[Solution]
  	In order to avoid the problem, drawing must be started immediately 
	after V-BLNK completion.  Unfortunately, since there is no mechanism 
	available to detect the completion of the V-BLNK using the current 
	VSyncCallback(), it is necessary to either

	- count the number of H-Sync using VSync()1 during V-BLNK   
	or 
	- add new callback using H-Sync callback (RCnt2).

	We would like to introduce more certain way to solve the problem by
	disclosing the function called, GetODE() to check if the current 
	field being displayed is the odd or even fields.  GetODE() was 
	officially included in the Release 3.7.

	If you are using the library releases earlier than 3.7, please add 
	the following declaration line to "libgpu.h":

	u_long GetODE(void);  /* 0...EVEN 1...ODD */

  	Please note that the return value of GetODE() indicates the current 
	field that GPU recognizes, NOT the actual video output begin odd or 
	even.
	You also need to put some thought into the program coding since 
	GetODE() always returns "EVEN" when immediately after VSyncCallbck() 
	and during V-BLNK.  For actual coding, please refer to the sample 
	program.


	[Supplemental Remarks]
  	With the Interlace mode, the fields are forced to switch to ODD or 
	EVEN at every 1/60 [sec] for the NTSC standards.  Thus for this mode, 
	all processes must complete within 1/60[sec].
  	If the process cannot be completed within 1/60[sec] due to the draw 
	time(GPU bottleneck ), it can be avoided by canceling the draw
	(ResetGraph(1)) although all the polygons cannot be drawn. 
	On the other hand, if incomplete process is due to the CPU problem
	(CPU bottleneck), screen flicker can be avoided by drawing the 
	screen again using the previous OT until the CPU completes 
	generation of the next OT. This can be implemented by calling the 
	draw instruction, DrawOTag() within VsyncCallback().  Please refer 
	to the sample program that shows the solution of the CPU bottleneck.  

<How to Execute>
	>psymake	compile
	>run balls	run

<Controller Button Assignment>
	Up/Down Directional Button  : decrease/increase the number of balls
	L1 Buttons                  : pause
	Select Button               : quit

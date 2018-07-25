$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
SAMPLE PROGRAM FOR IMPROVING PERFORMANCE USING "DMPSX"

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.


<Description>
	"tuto0.c" shows an example of improving program performance using DMPSX.
	You can compare the performance of two programs, one using the libgte low-level-
 	functions and another using the DMPSX and also optimized.
	
	This sample program runs the DMPSX version as a default. In order to modify the 
        	program to a libgte low-level-function version, just comment out the first 
	define statement;
		#define DMPSX_MACRO

	The number displayed on the screen represents the total time (in Hsync) of the
 	calculations and drawings.

<How to Execute>
        psymake   	compile
        run tuto0       run

<Controller Button Assignment>
        Up Directional Button   :Fog ON
        Down Directional Button :Fog OFF
        Right Directional Button:Move fog-near
        Left Directional Button :Move fog-near
        Square Button           :Rotation about Y-axis
        Cross Button            :Rotation about X-axis
        Triangle Button         :Rotation about X-axis
        Circle Button           :Rotation about Y-axis
        L1 Button               :Rotation about Z-axis
        L2 Button               :Translation in Z-axis direction
        R1 Button               :Rotation about Z-axis
        R2 Button               :Translation in Z-axis direction
        Select Button           :Quit


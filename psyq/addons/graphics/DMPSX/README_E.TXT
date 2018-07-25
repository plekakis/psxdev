$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

		SAMPLE PROGRAM FOR IMPROVING PERFORMANCE USING "DMPSX"

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

<How to operate>
	PADRup/down	.....	Rotation about X-axis
	PADRleft/right	.....	Rotation about Y-axis
	PADm/o		.....	Translation in Z-axis direction
	PADl/n		.....	Rotation about Z-axis
	PADLup/down	.....	Fog ON/OFF
	PADLleft/right	.....	Move fog-near
	PADk		.....	Terminate
 

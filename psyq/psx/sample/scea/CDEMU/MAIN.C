/************************************************\
  File: main.c
  Purpose:
	-Illustrates how to print information to the screen.
	-Introduces the concept of double-buffering.
	-Shows how to read from the cd.
	
  Author: Chia-Ming_Wang@sony.interactive.com
  Date: August 1997
 
\************************************************/


/****************************************\
  Standard includes.
  
\***************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include "cd.h"


static int _ramsize = 0x00200000; /* RAM SIZE */
static int _stacksize = 0x00008000; /* STACK SIZE */


#define MAX_BUFFER 128
typedef struct
{
	DISPENV dispEnv;
	DRAWENV drawEnv;

} Buffer;

Buffer buffer[2];



/****************************************************************\
	Set the video screen resolution.  For this
	demonstration program, I have set the horizontal
	to 320 and the vertical to 240.

	The PlayStation limits the video mode resolutions
	to be certain values.  Here are all the valid
	methods of calling "initVideoMode" (assuming that
	nRed, nGreen, and nBlue are integers from 0 to 255):
	
		initVideoMode(256,240, nRed, nGreen, nBlue);   
		initVideoMode(320,240, nRed, nGreen, nBlue);   
		initVideoMode(512,240, nRed, nGreen, nBlue);   

	
		initVideoMode(256,480, nRed, nGreen, nBlue);
		initVideoMode(320,480, nRed, nGreen, nBlue);
		initVideoMode(512,480, nRed, nGreen, nBlue);

	nRed, nGreen, and nBlue specify the background of the
	screen.

	For your program, you can specify the video resolution to
	anything you want. However, be careful. For example, suppose
	you write a program that is built for a 320 x 240 screen.
	THEN you design your texture maps and fit them into the
	remaining VRAM.  Later on, you decide to change the
	screen's resolution to 640 x 480, so you JUST make a
	change to "initVideoMode" and rerun the program.
	Nothing shows up on the screen!  This is because you
	obliterated any cluts and texture maps that
	were designed for the 320 x 240 screen.  To solve this,
	you will have to re-place your texture maps and cluts,
	probably using the "timutil" function.

	
\****************************************************************/

void initVideoMode(int nScreenWidth, int nScreenHeight,
		   int backRed,		// Values should be 0..255
		   int backGreen,	// Values should be 0..255
		   int backBlue,	// Values should be 0..255
		   Buffer *buffer0,
		   Buffer *buffer1)
		   
		   
{

	// The following two rectangles describe the boundaries
	// in VRAM that will serve as the double-buffering mechanism.
	// At a given instance, finished drawings will be displayed in
	// topRect, while drawings are being prepared in the bottomRect.
	// Once you are finished, you can then swap the buffers
	// (as illustrated in the "Refresh" procedure, by calling
	// PutDrawEnv() and PutDispEnv()).

	// In this instance, I happen to place the top buffer's left
	// corner at (0,0), and the bottom buffer's left corner
	// immediately below the top buffer.  In reality, you
	// CAN place the buffers anywhere in VRAM you like. However,
	// for compatibility with the "timutil" program (which
	// appears on the Graphic Artist Tools CD), you should
	// stick to this convention.  Besides, you don't gain
	// any advantage in moving the buffers elsewhere.


	
	RECT topRect,			// buffer 0
		bottomRect;		// buffer 1

	topRect.x = 0;
	topRect.y = 0;
	topRect.w = nScreenWidth;
	topRect.h = nScreenHeight;

 	bottomRect.x = 0;
	bottomRect.y = topRect.y + nScreenHeight;
	bottomRect.w = nScreenWidth;
	bottomRect.h = nScreenHeight;

	// For a screen height of 480, the
	// bottomRect and the topRect are in
	// the same rectangle.
	if (nScreenHeight == 480)
		bottomRect.y = topRect.y;

	
	/***********************************************************\
		Set the drawing environment and display
		environment parameters.			
	\***********************************************************/
	
	SetDefDrawEnv(&buffer0->drawEnv, topRect.x,		// X
					topRect.y,		// Y
					topRect.w,	// Width
					topRect.h); // Height
					
	SetDefDispEnv(&buffer0->dispEnv, bottomRect.x,			// X
					bottomRect.y,			// Y
					bottomRect.w,    // W
					bottomRect.h);    // H
				

	SetDefDrawEnv(&buffer1->drawEnv, bottomRect.x,			// X
					bottomRect.y,			// Y
					bottomRect.w,    // W
					bottomRect.h);    // H
	
	SetDefDispEnv(&buffer1->dispEnv,  topRect.x,
					topRect.y,
					topRect.w,
					topRect.h);   


	/****************************************************\
		A U T O - C L E A R   M O D E
	   Clear the screen automatically when you display it.
	   (Refer to the reference manual's descriptions
	   of  "struct DRAWENV" )
	\***************************************************/		

	buffer0->drawEnv.isbg =
	buffer1->drawEnv.isbg = 1;


	/****************************************************\
	  	D I T H E R - P R O C E S S I N G

	  Dither processing flag: 0=off, 1= on
	   (Refer to the reference manual's descriptions
	   of  "struct DRAWENV" )	  
	\***************************************************/			
	buffer0->drawEnv.dtd = buffer1->drawEnv.dtd = 0;


	/****************************************************\
 		B A C K G R O U N D    C O L O R
	
		 Set background color for both buffers.
		 (You probably want them to be the same,
		 otherwise, you will get horrible flickering).
	         (Refer to the reference manual's descriptions
	         of  "struct DRAWENV" )		 
	\***************************************************/	
	setRGB0(&buffer0->drawEnv, backRed, backGreen, backBlue);
	setRGB0(&buffer1->drawEnv, backRed, backGreen, backBlue);		


}








void InitializeMyString(char *buf, int n)
{
  int i;
  for (i=0;i<n; i++)
  	buf[i]='\0';

}

main ()
{
	int currentBuffer = 0;
	long handle=0;

	char string1[MAX_BUFFER], string2[MAX_BUFFER];
	
	/*****************************\
	  Standard initialization.
	\*****************************/

	InitGeom();
	ResetCallback();
	CdInit();	
	ResetGraph(0);



	/***********************************************\
	   I N I T I A L I Z E    V I D E O     M O D E
	   Set the resolution of the screen
	   and the background color.  Note the
	   use of the DOUBLE-BUFFERING mechanism.
	   You will draw to one buffer while the
	   computer is displaying the other.

	   Here, I've set the background color to pure
	   red.
	\**********************************************/	
	initVideoMode(320,  240,	// Width, height of screen
		     255,		// Red (0..255)
		     0,			// Green (0..255)
		     0,			// Blue (0..255)
		     &buffer[0],
		     &buffer[1]);

  	SetDispMask(1);	// Turn on the display.

	
	/***************************************\
		I N I T I A L I Z E
		O N  -  S C R E E N
		P R I N T I N G
	  
	  1. Load the font into VRAM
	  2. Get a handle.
	  3. Print to the handle.
	  4. Flush the handle, to get the
	     output to appear on the screen.
	\***************************************/

  	
	FntLoad(960, 256);	// Load font into VRAM at (x,y)
	handle = FntOpen(16, 16,		// Top left corner (x,y)
 		 	256, 200,       // Width, height
			   
			0,		// 0=Clear background to black
		   		// 1=Don't clear background
			200);	// Maximum number of characters
	SetDumpFnt(handle);


	/****************************************\
		Read from files. WARNING:
		Note the use of all capital
		letters.  Note that for this
		example, I'm expecting that the
		contents of the files do not
		exceed MAX_BUFFER. 
	\****************************************/

	InitializeMyString(string1, MAX_BUFFER);
	InitializeMyString(string2, MAX_BUFFER);
	
	MyLoadFileFromCD("\\TEXT1.TXT;1", string1);  	// Defined in cd.c
 	MyLoadFileFromCD("\\DIR1\\TEXT2.TXT;1", string2); // Defined in cd.c
	
	printf("From file 1: %s\n", string1);
	printf("From file 2: %s\n", string2);
	
	/****************************************\
		Display.  This is a typical
		loop that your programs will have.
		While one buffer is being prepared,
		the other is being drawn on the
		screen.
	\****************************************/

  	while (1)
  	{
        

		/**************************************\
		   Wait until the currently-drawing
		   screen is finished.
		\**************************************/
		DrawSync(0);

		/**************************************\
		  Wait for the vertical sync signal.
		  Otherwise, your image will be rolling
		  all over the place.
		\**************************************/        
		VSync(0);


		/**************************************\
		   Determine which buffer is the
		   "preparation" buffer. Since
		   there are only two buffers, choose
		   one of them.  We will display it
		   later.
		\**************************************/
		currentBuffer = (currentBuffer==0?1:0);
			

		/**************************************\
		   Draw into the current buffer.
		   For now, just print "HELLO" and
		   the number of the current buffer.
		\**************************************/
		FntPrint(handle, "The current buffer: %d\n", currentBuffer);
		FntPrint(handle, "From file 1: %s\n", string1);
		FntPrint(handle, "From file 2: %s\n", string2);
		
		
		FntFlush(-1);   // Make sure contents of buffer get
				// flushed to the screen.

		/**************************************\
		   Blast the "currentBuffer" on the
		   screen.  Note that TWO functions
		   are being called here, one for
		   the drawing environment, and the
		   other for the display environment.
		\**************************************/		
	  	PutDrawEnv(&buffer[currentBuffer].drawEnv);
	  	PutDispEnv(&buffer[currentBuffer].dispEnv);
	  	
	}  // end WHILE

  	
}

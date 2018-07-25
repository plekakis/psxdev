//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//	FILENAME : main.c
//	
//	DESCRIPTION : 						 
//		This file does all the averaging, 
//		scrolling, and hotspot creation necessary
//		to get the fire moving.
//		
//	USAGE :
//		main source code file
//		
//	HISTORY :
//              1.00  4-29-97 (J. Page SCEE)
//                       Polygonal version
//              1.00  11-17-97 (M. Koziniak SCEA)
//                       2D Texture version
//
//	Copyright (C) 1997 Sony Computer Entertainment, Inc. 
//	All Rights reserved						 
//
//
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// I N C L U D E S ///////////////////////////////////////////////////////
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <stdlib.h>
#include <stdio.h>
#include <libsn.h>

// include our stuff 
#include "main.h"

// M A I N //////////////////////////////////////////////////////////////

void main()
{
	InitSystem();		// set the system to a generally used state
	InitFont();			// loads GOTHIC font to VRAM for use
	InitFireTex();		// sets fire buffers to zero
	InitFirePoly();		// set position and disply of polygons
	InitFireCLUT();		// makes the fire CLUT availiable for use

	while(MKPadRead() > 0)
	{
		id = !id;		// the fire arrays are double buffered
		ot = otbuf[id]; // pointer to current ordering table

		ClearOTag(ot, OT_SIZE);   

		MoveFire();		// do this first becuase values are fresh

		CreateHotSpots(4);	// set random hotspots

		// since this is a procedural texture it must be loaded into VRAM
		// each frame. We load a top and a bottom to remove unwanted values
		// in the middle
	    LoadImage(&top_part, (unsigned long*)&flame_buf[id][0][0]);
		LoadImage(&bottom_part, (unsigned long*)&flame_buf[id][IMAGE_HEIGHT][0]);
		
		addPrim(ot + OT_SIZE - 1, &prim[id]);	// macro version of function

		DBSwap(ot+OT_SIZE -1);		// we are done building the display so swap buffers
									// and state where we want to starting drawing from
									// in the OT
	} // end while

	printf("\nNormal Shutdown.");

	PadStop();
	ResetGraph(0);
	StopCallback();

}	// end main


// F U N C T I O N S ////////////////////////////////////////////////////

void CreateHotSpots(int Count)
{
// This function adds hotspots to the bottom row of buffer at random positions

	int y;
	int x;
	static int c = 127;		// max CLUT color

	for(x = 0 ;x < IMAGE_WIDTH ;x++)
	{
	    flame_buf[id][IMAGE_HEIGHT-1][x] = 0;
	}

	for (y=0;y<Count;y++)
	{
		x = rand()%(IMAGE_WIDTH/2)+(IMAGE_WIDTH/4);
		flame_buf[id][IMAGE_HEIGHT-1][x]=c;      // random heat added to left and right to 
		flame_buf[id][IMAGE_HEIGHT-1][x+1]=c;    // strengthen fire 
		flame_buf[id][IMAGE_HEIGHT-1][x-1]=c;    
	}
}  // end CreateHotSpots


void MoveFire(void)
{
// this function averages fire, scrolls it up, and clears the bottom
	int x;
	int y;
	int a;

	for (y=1;y<IMAGE_HEIGHT-1;y++)
	{
		for (x=1;x<IMAGE_WIDTH-1;x++)
		{
	        a=flame_buf[id][y-1][x-1];
	        a+=flame_buf[id][y-1][x+1];
	        a+=flame_buf[id][y+1][x+1];
	        a+=flame_buf[id][y+1][x-1];
		    a>>=2;
	        flame_buf[id][y][x]=a;
	        flame_buf[!id][y-1][x]=a;
		}
	}

	for (y=IMAGE_HEIGHT-1;y<(IMAGE_HEIGHT+IMAGE_BOTTOM)-1;y++)
	{
		for (x=1;x<IMAGE_WIDTH-1;x++)
		{
            a=flame_buf[id][y-1][x-1];
            a+=flame_buf[id][y-1][x+1];
            a+=flame_buf[id][y+1][x+1];
            a+=flame_buf[id][y+1][x-1];
		 	a>>=2;
			a&=15;
            flame_buf[id][y][x]=a;
            flame_buf[!id][y+1][x]=a;
		}
	}
} // end MoveFire


void InitFirePoly(void)
{
// this function makes the Poly's usable by the GPU and
// sets the positioning within the display

	static int offset = 3;

	top_part.x = 320;
	top_part.y = 0;
	top_part.w = IMAGE_WIDTH/2;  // divide this by 2 since VRAM is 16bit
	top_part.h = IMAGE_HEIGHT - offset;// + IMAGE_BOTTOM;
	bottom_part.x = 320;
	bottom_part.y = IMAGE_HEIGHT - offset;
	bottom_part.w = IMAGE_WIDTH/2;  // DIVIDE for size of VRAM pixels
	bottom_part.h = IMAGE_BOTTOM;

	SetPolyGT4(&prim[0]);

	setXYWH(&prim[0], 20,70, IMAGE_WIDTH, IMAGE_HEIGHT+(IMAGE_BOTTOM));
	setUVWH(&prim[0], 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT+(IMAGE_BOTTOM));
	setRGB0(&prim[0], 127, 127, 127);
	setRGB1(&prim[0], 127, 127, 127);
	setRGB2(&prim[0], 127, 127, 127);
	setRGB3(&prim[0], 127, 127, 127);
	setSemiTrans(&prim[0], 0);
	setShadeTex(&prim[0],  1);
	prim[0].tpage = GetTPage(1, 1, 320, 0); // mode, abr, VRAM x,y
	prim[0].clut = GetClut(0, 480);
	memcpy(&prim[1], &prim[0], sizeof(POLY_GT4));
} // end InitFirePoly

void InitFireCLUT(void)
{
// this function loads the fire texture into VRAM
	TIM_IMAGE image;

	OpenTIM(CLUT_ADDR);
	ReadTIM(&image);
	LoadImage(image.prect,image.paddr); 
} // end InitFireCLUT


void DBSwap(unsigned long *ot)
{
	cdb = (cdb==db)? db+1: db;	// swap the current display
	DrawSync(0);				// sync GPU 
	VSync(0);					// wait for V-BLNK 
	FntFlush(-1);	   			// leave the FntPrint here to be closer to the VBlank
	PutDrawEnv(&cdb->draw);		// update DRAWENV 
	PutDispEnv(&cdb->disp);		// update DISPENV 
	DrawOTag(&otbuf[id][0]);	// start drawing all over again
} // end DBSwap


void InitFireTex(void)		
{
// this function initializes our fire array

	int	i, y, x;

	for(i = 0; i < 2; i++)
	{
		for(y = 0; y < (IMAGE_HEIGHT + IMAGE_BOTTOM) -2; y++)
		{
			for(x = 0; x < IMAGE_WIDTH; x++)
			{ 
	            flame_buf[i][y][x] = 0;
			} // end k
		} // end j
	}	// end i
}  // end InitFireTex


void InitFont(void)
{
// this functions makes the fonts usable
	FntLoad(960, 256); 
  	FntOpen(5, 5, 315, 235, 0, 1092);
} // end InitFont


void InitSystem(void)
{
// This function initializes the system and display to commonly used settings

	ResetCallback();			// allways a good idea

	ResetGraph(0);				// reset graphic subsystem (0:cold,1:warm)
	PadInit(0);					// initialize the controller		
	SetVideoMode( MODE_NTSC );

	// initialize environment for double buffer 
	SetDefDrawEnv(&db[0].draw, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	SetDefDrawEnv(&db[1].draw, 0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
	SetDefDispEnv(&db[0].disp, 0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDispEnv(&db[1].disp, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// needed to enable the display
	SetDispMask(1);	 // 0: inhibit display 
					 // 1: enable display

	// clear the background each frame (1 = TRUE)
	db[0].draw.isbg = 1;
	db[1].draw.isbg = 1;
} // end InitSystem


int MKPadRead(void)
{
// this function interprets controller data and sets the fire to a certain clut

	static int mode_flg = 1;
	static int y = 480;

	pad  = PadRead(1);

	while((pad & PADL1) > 0)
		VSync(0);

	if((pad & PADLleft)>0) 
		left_right -= left_right;
	else
	if((pad & PADLright)>0) 
		left_right += left_right;
	else
	if((pad & PADLup)>0) 
		up_down += up_down;
	else
	if((pad & PADLdown)>0) 
		up_down -= up_down;

	if((pad & PADRdown)>0) 
		{
			if(mode_flg == 0)
			{
				y++;
				prim[0].clut = GetClut(0, y);
				prim[1].clut = GetClut(0, y);
				mode_flg = 1;
			}
		}

	else 
	if((pad & PADRup)>0) 
		{
			if(mode_flg == 0)
			{
				y--;
				prim[0].clut = GetClut(0, y);
				prim[1].clut = GetClut(0, y);
				mode_flg = 1;
			}
		}

	else 
	if (pad & PADselect)
			return(-1);
	else
		mode_flg = 0;
		FntPrint("\nflame type = %d", y%480);
		return(1); 
} // end MKPadRead

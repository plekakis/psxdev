//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//	FILENAME : main.h
//	
//	DESCRIPTION : 						 
//		Support file for main
//		
//	USAGE :
//		main source code header file
//		
//	HISTORY :
//		1.00  11-17-97 M. Koziniak
//
//	Copyright (C) 1997 Sony Computer Entertainment, Inc. 
//	All Rights reserved						 
//
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// D E F I N E S /////////////////////////////////////////////////////////

#define SCREEN_WIDTH  320	 
#define SCREEN_HEIGHT 240

#define IMAGE_WIDTH  64
#define IMAGE_HEIGHT 64
#define IMAGE_BOTTOM 15

#define OT_SIZE 8	  // Number of ordering table entries

#define CLUT_ADDR (unsigned long*)0x80140000  	  // 16 bit image used as 8 bit CLUT 
#define CAMPSITE_ADDR (unsigned long*)0x80144000  // 16 bit background 

// S T R U C T U R E S ///////////////////////////////////////////////////
typedef struct {
	DRAWENV		draw;			// drawing environment 
	DISPENV		disp;			// display environment 
} DB;

// G L O B A L S /////////////////////////////////////////////////////////

DB  db[2];
DB  *cdb;			// current DB

unsigned long otbuf[2][OT_SIZE];	
unsigned long *ot;				// current OT

RECT  top_part,     // upper image coordinates
	  bottom_part;  // lower image coordinates

POLY_GT4 prim[2];	// our primative

unsigned char flame_buf[2][IMAGE_HEIGHT+IMAGE_BOTTOM][IMAGE_WIDTH];  // used to create 8bit texture

int id = 0;		// toggles the fire buffers

unsigned long pad;		// controller data
int left_right = SCREEN_WIDTH/2;
int up_down = SCREEN_HEIGHT/2;


// P R O T O T Y P E S ///////////////////////////////////////////////////

void InitFont(void);				// loads a font system into VRAM for use
void InitSystem(void);				// sets up PlayStation to usable state
void DBSwap(unsigned long *ot);
int	 MKPadRead(void);				// used to interprete controller data

void InitFireCLUT(void);			// load CLUT into VRAM 
void InitFireTex(void);				// zero out buffers
void InitFirePoly(void);			// Position and set poly's display attributes
void MoveFire(void);				// average and scroll fire
void CreateHotSpots(int nCount);	// get fire started
































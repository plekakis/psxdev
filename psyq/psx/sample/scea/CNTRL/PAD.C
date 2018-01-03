/**************************************************************************/
/* Controller demo                                                        */
/* Written by Mike Fulton                                                 */
/* Last Modified 6:25pm, 11/15/96                                         */
/* Copyright (c) 1996 Sony Computer Entertainment America                 */
/**************************************************************************/

#include <r3000.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgun.h>

//#include <asm.h>
//#include <kernel.h>

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

// The light gun driver records the horizontal pixel clock and vertical
// scanline counter rather than pixel-based screen position.  These values
// are used when converting these values into a position on the screen.  Th

#define GUN_X_DIVISOR		(5000)	// 5000 = 320 pixel screen width
					// 3250 = 512 pixel screen width
					// 2500 = 640 pixel screen width

#define GUN_Y_MULTIPLIER	(1)	// 1 = 240 line screen
					// 2 = 480 line screen

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

// Data structure for keeping track of screen double-buffering

typedef struct
{							
	DRAWENV		draw;		/* drawing environment */
	DISPENV		disp;		/* display environment */
	u_long		ot[5];		/* ordering table */
	unsigned char		*primitives;	/* Pointer to buffer storing GPU primitives */
	unsigned char		*primnext;	/* Pointer to next free position in prim buffer */
} Double_Buffer;

// Structure for storing processed controller data

typedef struct
{
	int		xpos, ypos;	// Stored position for sprite(s)
	int		xpos2, ypos2;	// controlled by this controller.

	unsigned char	status;		// These 8 values are obtained
	unsigned char	type;		// directly from the controller
	unsigned char	button1;	// buffer we installed with InitPAD.
	unsigned char	button2;
	unsigned char	analog0;
	unsigned char	analog1;
	unsigned char	analog2;
	unsigned char	analog3;
} Controller_Data;

// Structure for RAW hardware-based light gun position values

typedef struct
{
        unsigned short    v_count;	// Y-axis (vertical scan counter)
        unsigned short    h_count;	// H-axis (horizontal pixel clock value)
} Gun_Position;

// Buffer to receive GUN position data

typedef struct
{
	char		gun_status;	// Status of gun
	char		gun_count;	// Number of X/Y pairs contained in 'gun' member.
	Gun_Position	gun[20];	// X/Y light gun values captured
} Gun_Buffer;

// All-purpose controller data buffer

typedef struct
{
	unsigned char	pad[34];	// 8-bytes w/o Multi-Tap, 34-bytes w/Multi-Tap
	Gun_Buffer	gun;		// Light gun position data
} Controller_Buffer;

/***************************************************************************/
/***************************************************************************/
/* Read in TIM data for sprites from various C files.  These *.C files     */
/* contain TIM files which have been converted into C data statements for  */
/* easy compile-time inclusion.                                            */
/***************************************************************************/
/***************************************************************************/

char analog_tim[] =
{
	#include "analog.c"
};

char analog2_tim[] =
{
	#include "analog2.c"
};

char analog3_tim[] =
{
	#include "analog3.c"
};

char digital_tim[] =
{
	#include "digital.c"
};

char psxpad_tim[] =
{
	#include "psxpad.c"
};

char wheel_tim[] =
{
	#include "wheel.c"
};

char mouse_tim[] =
{
	#include "mouse.c"
};

char gun_tim[] =
{
	#include "gun.c"
};

TIM_IMAGE	theAnalogTim, theAnalog2Tim, theAnalog3Tim, theDigitalTim,
		thePSXPadTim, theWheelTim, theMouseTim, theGunTim;

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

#include "proto.h"

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

Controller_Buffer controllers[2];	// Buffers for reading controllers

Double_Buffer db[2];			// Structure for keeping track of screen double-buffering

unsigned char primbuffer[2][1000];	// Data storage for primitives

RECT theScreen = { 0, 0, 320, 240 };	// Rectangle defining screen area

Controller_Data theControllers[8];	// Processed controller data

unsigned long ev0,ev1,ev2,ev3,ev10,ev11,ev12,ev13;	// Event handles for SIO events we install

int font_id;				// Font ID for screen text output

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

void main(void)
{
int row, i, ii, buffer;

	ResetCallback();
	
	InitGUN( controllers[0].pad, 34, controllers[1].pad, 8,
		&controllers[0].gun,    &controllers[1].gun, 20);
	SelectGUN(0,1);
	SelectGUN(1,0);
	StartGUN();

	initialize_display();

// Load textures & init buffer-specific stuff

	load_textures();

	buffer = 0;
	PutDispEnv(&db[buffer].disp);
	PutDrawEnv(&db[buffer].draw);

	while(1)
	{
		ClearOTag( (u_long *)&db[buffer].ot, 5 );

		/* Re-initialize buffer tracking for this frame */
		
		db[buffer].primitives = &primbuffer[buffer][0];
		db[buffer].primnext = &primbuffer[buffer][0];

		/* Create primitives and add to list */

		read_controllers();		

		draw_sprite( &db[buffer] );
		make_pretty_colors( &db[buffer] );

		VSync(0);
		DrawSync(0);

		PutDispEnv(&db[buffer].disp);
		PutDrawEnv(&db[buffer].draw);

		theScreen.y = 0 + (240 * buffer);
		ClearImage( &theScreen, 255, 144, 144 );

		DrawSync(0);
		DrawOTag( (u_long *)&db[buffer].ot );

		FntFlush( font_id );		// Update text display on screen

		SelectGUN( buffer, 0 );		// Turn off gun for current port
		buffer = 1 - buffer;		// Switch to next screen buffer (& port for light gun)
		SelectGUN( buffer, 1 );		// Turn gun on for new port
	}

	StopGUN();
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void initialize_display()
{
	SetDispMask( 0 );		// Turn off display
	ResetGraph(0);			// Reset the graphics system
	SetGraphDebug(0);

	db[0].draw.dtd = 1;		// Configure the draw environment
	db[0].draw.dfe = 0;
	db[0].draw.isbg = 0;
	db[0].disp.isinter = 0;
	db[0].disp.isrgb24 = 0;
	
	db[1].draw = db[0].draw;	// Do both halves of double-buffer
	
// Initialize Double Buffer Setup...
// We draw to (0,0)-(320,240) while showing (0,240)-(320,480) and vice versa

	SetDefDrawEnv( &db[0].draw, 0,   0, 320, 240 );
	SetDefDispEnv( &db[0].disp, 0, 240, 320, 240 );
	SetDefDrawEnv( &db[1].draw, 0, 240, 320, 240 );
	SetDefDispEnv( &db[1].disp, 0, 0,   320, 240 );
	
	SetDispMask( 1 );		// Turn on display
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

/* Load textures into VRAM */

void load_textures()
{
long tim;

// load basic font pattern

	FntLoad(960, 256);
	font_id = FntOpen(4, 30, 312, 180, 0, 640);
	SetDumpFnt(font_id);

// Go through and load the various TIM images we're using for sprites

	tim = OpenTIM( (u_long *)analog_tim );
	ReadTIM( &theAnalogTim );
	LoadImage( theAnalogTim.prect, theAnalogTim.paddr );
	printf( "analog tim = %d, { %d, %d, %d, %d }\n", tim, theAnalogTim.prect->x, theAnalogTim.prect->y, theAnalogTim.prect->w, theAnalogTim.prect->h );

	DrawSync(0);	/* Let LoadImage() finish */

	tim = OpenTIM( (u_long *)analog2_tim );
	ReadTIM( &theAnalog2Tim );
	LoadImage( theAnalog2Tim.prect, theAnalog2Tim.paddr );
	printf( "analog2 tim = %d, { %d, %d, %d, %d }\n", tim, theAnalog2Tim.prect->x, theAnalog2Tim.prect->y, theAnalog2Tim.prect->w, theAnalog2Tim.prect->h );

	DrawSync(0);	/* Let LoadImage() finish */

	tim = OpenTIM( (u_long *)analog3_tim );
	ReadTIM( &theAnalog3Tim );
	LoadImage( theAnalog3Tim.prect, theAnalog3Tim.paddr );
	printf( "analog3 tim = %d, { %d, %d, %d, %d }\n", tim, theAnalog3Tim.prect->x, theAnalog3Tim.prect->y, theAnalog3Tim.prect->w, theAnalog3Tim.prect->h );

	DrawSync(0);	/* Let LoadImage() finish */

	tim = OpenTIM( (u_long *)digital_tim );
	ReadTIM( &theDigitalTim );
	LoadImage( theDigitalTim.prect, theDigitalTim.paddr );
	printf( "digital tim = %d, { %d, %d, %d, %d }\n", tim, theDigitalTim.prect->x, theDigitalTim.prect->y, theDigitalTim.prect->w, theDigitalTim.prect->h );

	DrawSync(0);	/* Let LoadImage() finish */

	tim = OpenTIM( (u_long *)psxpad_tim );
	ReadTIM( &thePSXPadTim );
	LoadImage( thePSXPadTim.prect, thePSXPadTim.paddr );
	printf( "psxpad tim = %d, { %d, %d, %d, %d }\n", tim, thePSXPadTim.prect->x, thePSXPadTim.prect->y, thePSXPadTim.prect->w, thePSXPadTim.prect->h );

	DrawSync(0);	/* Let LoadImage() finish */

	tim = OpenTIM( (u_long *)wheel_tim );
	ReadTIM( &theWheelTim );
	LoadImage( theWheelTim.prect, theWheelTim.paddr );
	printf( "wheel tim = %d, { %d, %d, %d, %d }\n", tim, theWheelTim.prect->x, theWheelTim.prect->y, theWheelTim.prect->w, theWheelTim.prect->h );

	DrawSync(0);	/* Let LoadImage() finish */

	tim = OpenTIM( (u_long *)mouse_tim );
	ReadTIM( &theMouseTim );
	LoadImage( theMouseTim.prect, theMouseTim.paddr );
	printf( "mouse tim = %d, { %d, %d, %d, %d }\n", tim, theMouseTim.prect->x, theMouseTim.prect->y, theMouseTim.prect->w, theMouseTim.prect->h );

	DrawSync(0);	/* Let LoadImage() finish */

	tim = OpenTIM( (u_long *)gun_tim );
	ReadTIM( &theGunTim );
	LoadImage( theGunTim.prect, theGunTim.paddr );
	printf( "gun tim = %d, { %d, %d, %d, %d }\n", tim, theGunTim.prect->x, theGunTim.prect->y, theGunTim.prect->w, theGunTim.prect->h );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

// Draw fancy color shifting background by using the gouraud shading
// feature of the GPU and shifting the corner colors.

void make_pretty_colors( Double_Buffer *theDB )
{
static unsigned char co1 = 255, co2 = 0, co3 = 0;
POLY_G4 *g4;

	g4 = (POLY_G4 *)theDB->primnext;
	memset( theDB->primnext, 0, sizeof(POLY_G4) );
	theDB->primnext += sizeof(POLY_G4);

	SetPolyG4( g4 );		// Set up GPU primitive
	setXYWH( g4, 0, 0, 320, 240);	// take up full-screen

	setRGB0( g4, co1, co2, co3);	// Set corner colors
	setRGB1( g4, co3, co1, co2);
	setRGB2( g4, co2, co3, co1);
	setRGB3( g4, co1, co2, co3);

	AddPrim( theDB->ot, g4 );	// Add to primitive list

// Change corner colors to do color shifting

	if ( co3 == 0 && co1 > 0 )
	{
		co1 -= 5;
		co2 += 5;
	}

	if ( co1 == 0 && co2 > 0 )
	{
		co2 -= 5;
		co3 += 5;
	}

	if ( co2 == 0 && co3 > 0 )
	{
		co3 -= 5;
		co1 += 5;
	}
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void read_controllers()
{
int controller_port, port_offset, c;

	for( controller_port = 0; controller_port <= 1; controller_port++ )
	{
		port_offset = controller_port * 4;

		// Look for multi-tap

		switch( controllers[controller_port].pad[1] )
		{
		case 0x80:	// Multi-Tap, so go read each of 4 ports.

			read_controller( &theControllers[0 + port_offset], &controllers[controller_port].pad[2], controller_port );
			read_controller( &theControllers[1 + port_offset], &controllers[controller_port].pad[10], controller_port );
			read_controller( &theControllers[2 + port_offset], &controllers[controller_port].pad[18], controller_port );
			read_controller( &theControllers[3 + port_offset], &controllers[controller_port].pad[26] , controller_port);
			break;

		default:	// Controller directly connected, read it, zero-out Multi-Tap slots.

			read_controller( &theControllers[0 + port_offset], &controllers[controller_port].pad[0], controller_port );
			theControllers[1 + port_offset].type = 0;
			theControllers[2 + port_offset].type = 0;
			theControllers[3 + port_offset].type = 0;
			break;
		}

		// Update screen text display

		if( controllers[controller_port].pad[1] == 0x80 )
			FntPrint( "PlayStation Port #%d: Multi Tap Found!\n", controller_port+1 );
		else
			FntPrint( "PlayStation Port #%d:\n", controller_port+1 );


		// Print information about each controller on this port (4 slots with Multi-Tap)

		for( c = 0; c < 4; c++ )
		{
			if( theControllers[c+port_offset].type )
			{
				FntPrint( "%02x: Buttons:%02x %02x, Analog:%02x %02x %02x %02x\n",
					theControllers[c + port_offset].type,
					theControllers[c + port_offset].button1,
					theControllers[c + port_offset].button2,
					theControllers[c + port_offset].analog0,
					theControllers[c + port_offset].analog1,
					theControllers[c + port_offset].analog2,
					theControllers[c + port_offset].analog3 );
			}
			else	// Nothing connected
			{
				FntPrint( "%02x: N/C: %02x %02x %02x %02x %02x %02x\n",
					theControllers[c + port_offset].type,
					theControllers[c + port_offset].button1,
					theControllers[c + port_offset].button2,
					theControllers[c + port_offset].analog0,
					theControllers[c + port_offset].analog1,
					theControllers[c + port_offset].analog2,
					theControllers[c + port_offset].analog3 );
			}
		}
		FntPrint( " \n" );
	}
}


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

// Translate D-pad information into sprite movement

void get_digital_direction( Controller_Data *c, int buttondata )
{
int i;

	i = ~(buttondata);

	if( i & 0x80 )
		c->xpos -= 1;
				
	if( i & 0x20 )
		c->xpos += 1;
				
	if( i & 0x40 )
		c->ypos += 1;
				
	if( i & 0x10 )
		c->ypos -= 1;
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void read_controller( Controller_Data *c, unsigned char *buf, int port )
{
register int mouse_x, mouse_y, x;
register Gun_Position *g;

	c->status =  buf[0];	// Copy over raw controller data
	c->type =    buf[1];
	c->button1 = buf[2];
	c->button2 = buf[3];
	c->analog0 = buf[4];
	c->analog1 = buf[5];
	c->analog2 = buf[6];
	c->analog3 = buf[7];

	if( buf[0] == 0xff )	// If controller returns BAD status then bail on it.
	{
		c->type = 0;
		return;
	}

	// Look at the controller type code & process controller data as indicated

	switch( c->type )
	{
		case 0x12:	// Sony Mouse
			mouse_x = buf[4];
			mouse_y = buf[5];

			if( mouse_x & 0x80 )
				mouse_x	|= 0xffffff80;
			if( mouse_y & 0x80 )
				mouse_y	|= 0xffffff80;

			c->xpos += mouse_x;
			c->ypos += mouse_y;
			break;
			
		case 0x23:	// Namco negCon
				// Steering wheel
				// Sankyo Pachinko controler
			get_digital_direction( c, buf[2] );
			break;


		case 0x31:	// Light Gun
			g = &controllers[port].gun.gun[0];

			x = (g->h_count - 140);
			if( x > 0 )
				c->xpos = (x*1000) / GUN_X_DIVISOR;
			else
				c->xpos = 0;

			c->ypos = g->v_count - 16;
                        break;
		
		case 0x53:	// Analog 2-stick
			get_digital_direction( c, buf[2] );
			break;

		case 0x41:	// Standard Sony PAD controller
			get_digital_direction( c, buf[2] );
			break;

		default:	// If don't know what it is, treat it like standard controller
			get_digital_direction( c, buf[2] );
			break;
	}

	bounds_check( &c->xpos, &c->ypos );
	bounds_check( &c->xpos2, &c->ypos2 );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

// Draw a sprite for each connected controller.

void draw_sprite( Double_Buffer *theDB )
{
int c;

	for( c = 0; c < 8; c++ )
	{
		switch( theControllers[c].type )
		{
			/* PSX Mouse */
			case 0x12:
				draw_mouse( theDB, theControllers[c].xpos, theControllers[c].ypos );
				break;			

			/* NegCon or Steering Wheel */
			case 0x23:
				draw_wheel( theDB, theControllers[c].analog0, theControllers[c].analog1 );
				draw_analog3( theDB, theControllers[c].analog2, theControllers[c].analog3 );
				draw_digital( theDB, theControllers[c].xpos, theControllers[c].ypos );
				break;			
	
			/* Light Gun */
			case 0x31:
				draw_gun( theDB, theControllers[c].xpos, theControllers[c].ypos );
				break;			

			/* 2-stick Analog */
			case 0x53:
				draw_analog( theDB, theControllers[c].analog0, theControllers[c].analog1 );
				draw_analog2( theDB, theControllers[c].analog2, theControllers[c].analog3 );
				draw_digital( theDB, theControllers[c].xpos, theControllers[c].ypos );
				break;

			/* PSX Pad */
			case 0x41:
				draw_psxpad( theDB, theControllers[c].xpos, theControllers[c].ypos );
				break;

			/* Unknown controller type, assume digital pad and maybe analog channels */
			default:
				draw_psxpad( theDB, theControllers[c].xpos, theControllers[c].ypos );

				if( (theControllers[c].type & 0x0F) >= 2 )	// Has 1 analog channel
					draw_analog( theDB, theControllers[c].analog0, theControllers[c].analog1 );
					
				if( (theControllers[c].type & 0x0F) >= 3 )	// Has 2 analog channels
					draw_analog2( theDB, theControllers[c].analog2, theControllers[c].analog3 );
				break;			

		}
	}
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

// Generic portion of "create a sprite" code

POLY_FT4 *get_sprite( Double_Buffer *theDB, int x, int y, TIM_IMAGE *theTim )
{
POLY_FT4 *theSprite;

	theSprite = (POLY_FT4 *)theDB->primnext;
	memset( theDB->primnext, 0, 2 * sizeof(POLY_FT4) );
	theDB->primnext += sizeof(POLY_FT4);

	SetPolyFT4( theSprite );

	setRGB0( theSprite, 128, 128, 128 );
	setXY4( theSprite, x-8, y-8, x+8, y-8, x-8, y+8, x+8, y+8 );

	theSprite->tpage = GetTPage( 2, 0, theTim->prect->x, theTim->prect->y );

	return( theSprite );
}


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void draw_analog( Double_Buffer *theDB, int x, int y )
{
POLY_FT4 *theSprite;
	
	x = (x * 320) / 255;	/* Scale to fit 320-pixel screen */
	y += 10;		/* Adjust for overscan */

	theSprite = get_sprite( theDB, x, y, &theAnalogTim );
        setUV4( theSprite, 0, 0, 16, 0, 0, 16, 16, 16 );
	AddPrim( theDB->ot, theSprite );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void draw_analog2( Double_Buffer *theDB, int x, int y )
{
POLY_FT4 *theSprite;
	
	x = (x * 320) / 255;	/* Scale to fit 320-pixel screen */
	y += 10;		/* Adjust for overscan */

	theSprite = get_sprite( theDB, x, y, &theAnalog2Tim );
        setUV4( theSprite, 0, 80, 16, 80, 0, 96, 16, 96 );
        AddPrim( theDB->ot, theSprite );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void draw_analog3( Double_Buffer *theDB, int x, int y )
{
POLY_FT4 *theSprite;
	
	x = (x * 320) / 255;	/* Scale to fit 320-pixel screen */
	y += 10;		/* Adjust for overscan */

	theSprite = get_sprite( theDB, x, y, &theAnalog3Tim );
        setUV4( theSprite, 0, 96, 16, 96, 0, 112, 16, 112 );
	AddPrim( theDB->ot, theSprite );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void draw_digital( Double_Buffer *theDB, int x, int y )
{
POLY_FT4 *theSprite;
	
	x = (x * 320) / 255;	/* Scale to fit 320-pixel screen */
	y += 10;		/* Adjust for overscan */

	theSprite = get_sprite( theDB, x, y, &theDigitalTim );
        setUV4( theSprite, 0, 16, 16, 16, 0, 32, 16, 32 );
	AddPrim( theDB->ot, theSprite );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void draw_psxpad( Double_Buffer *theDB, int x, int y )
{
POLY_FT4 *theSprite;
	
	x = (x * 320) / 255;	/* Scale to fit 320-pixel screen */
	y += 10;		/* Adjust for overscan */

	theSprite = get_sprite( theDB, x, y, &thePSXPadTim );
        setUV4( theSprite, 0, 32, 16, 32, 0, 48, 16, 48 );
	AddPrim( theDB->ot, theSprite );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void draw_wheel( Double_Buffer *theDB, int x, int y )
{
POLY_FT4 *theSprite;
	
	y = (y * 2) / 3;	/* 0 to 240 = 0 to 160 */
	y += 40;		/*          = 40 to 200 */

	x = (x * 320) / 255;	/* Scale to fit 320-pixel screen */
	y += 10;		/* Adjust for overscan */

	theSprite = get_sprite( theDB, x, y, &theWheelTim );
	setUV4( theSprite, 0, 48, 16, 48, 0, 64, 16, 64 );
	AddPrim( theDB->ot, theSprite );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void draw_mouse( Double_Buffer *theDB, int x, int y )
{
POLY_FT4 *theSprite;
	
       	theSprite = get_sprite( theDB, x, y, &theMouseTim );
	setUV4( theSprite, 0, 64, 16, 64, 0, 80, 16, 80 );
	AddPrim( theDB->ot, theSprite );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

void draw_gun( Double_Buffer *theDB, int x, int y )
{
POLY_FT4 *theSprite;

	theSprite = get_sprite( theDB, x, y, &theGunTim );
        setUV4( theSprite, 0, 112, 16, 112, 0, 128, 16, 128 );
	AddPrim( theDB->ot, theSprite );
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

// Make sure position values stay on screen

void bounds_check( int *xpos, int *ypos )
{
	if( *xpos < 0 )
		*xpos= 0;

	if( *ypos < 0 )
		*ypos= 0;

	if( *xpos > 320 )
		*xpos = 320;

	if( *ypos > 240 )
		*ypos = 240;
}


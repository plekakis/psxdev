/*****************************************************************************
Code by Jason Page 29-4-97
Video INIT routines: Dave Coombes

(C) 1997 Sony Computer Entertainment Ltd.
*****************************************************************************/

#include <libspu.h>
#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libetc.h>
#include <libcd.h>

#include "Flame.h"
#include "graph.h"

#define Width 40
#define Depth 32
#define CellSize 8		// Modify to set flame size.

static POLY_G4 Screen[Depth][Width];

char FlameMap[2][Depth][Width];
char FlameR[Depth][Width];
char FlameG[Depth][Width];
char FlameB[Depth][Width];

int Frame=0;
int Rnd=1965328;

int bf1=0;
int bf2=1;

/*****************************************************************************
Screen data
*****************************************************************************/
DB	   db[2];		/* packet double buffer */
DB* 	cdb;			/* current db */
extern unsigned long seq_bdrop[]; 


/*****************************************************************************
main
Top routine - This is where it all starts.
*****************************************************************************/
void main (void)
{
int f;

	ResetCallback();
	init_graph();

	InitFlame();
	InitScreen();

	do{
		cdb = (cdb==db)? db+1: db;
		ClearOTagR(cdb->ot, OTSIZE);

		DisplayFlame();
		MoveFlame();
		CreateHotSpots(8);

		Frame++;
		bf1=(bf1+1)&1;		// Swap buffers
		bf2=(bf2+1)&1;

	   DrawSync(0);

		VSync(0);
	   PutDrawEnv(&cdb->draw);
	   PutDispEnv(&cdb->disp);
	   DrawOTag(cdb->ot+OTSIZE-1);
	}while(1);
}	


/*****************************************************************************
InitFlame
	Clears Flame buffer
*****************************************************************************/
void InitFlame(void)
{
int x;
int y;

	for (y=0;y<Depth;y++)
	{
		for (x=0;x<Width;x++)
		{
			FlameMap[0][y][x]=0;
			FlameMap[1][y][x]=0;
		}
	}
}


/*****************************************************************************
MoveFlame
	Average Flame, Scroll Flame Up, Clear bottom line of HotSpots.
*****************************************************************************/
void MoveFlame(void)
{
int x;
int y;
int a;

	for (y=1;y<Depth-1;y++)
	{
		for (x=1;x<Width-1;x++)
		{
			a=FlameMap[bf1][y][x+1];
			a+=FlameMap[bf1][y][x-1];
			a+=FlameMap[bf1][y-1][x-1];
			a+=FlameMap[bf1][y-1][x];
			a+=FlameMap[bf1][y-1][x+1];
			a+=FlameMap[bf1][y+1][x+1];
			a+=FlameMap[bf1][y+1][x-1];
			a+=FlameMap[bf1][y+1][x];			// Calc average for cell
			a>>=3;
			FlameMap[bf1][y][x]=a;
			FlameMap[bf2][y-1][x]=a;
		}
	}
	for (x=0;x<Width;x++)
	{
		FlameMap[bf1][Depth-1][x]=0;			// Clear bottom line for next time
	}

}


/*****************************************************************************
CreateHotSpots
	Add hot spots to bottom row of buffer at random X position and random
	heat.

	Requires: Count	(Amount of hot spots to add)
*****************************************************************************/
void CreateHotSpots(int Count)
{
int i;
int x;
int c;

	for (i=0;i<Count;i++)
	{
		x=ModifyRnd(i);
		x&=31;
		c=ModifyRnd(i);
		c&=7;
		x+=c;
		if (x>(Width-1))						// Set random X pos
			x=(Width-1);

		c=ModifyRnd(Count);
		c&=255;
		FlameMap[bf1][Depth-1][x]=c;				/* Add Random heat at Random points */
	}
}


/*****************************************************************************
DisplayFlame
	Calculate RGB for each point and display on screen.
*****************************************************************************/
void DisplayFlame(void)
{
int sx;
int sy;
int x;
int y;
int f;
int r;
int g;
int b;
int r2;
int g2;
int b2;
int r3;
int g3;
int b3;
int r4;
int g4;
int b4;
int PCnt=0;
int PCnt2=0;

	for (y=0;y<Depth-5;y++)
	{
		for (x=0;x<Width;x++)
		{
			f=FlameMap[bf2][y][x];	/* Calculate RGB for each point */
			if (f<16)
			{
				r=f*16;
				g=0;
				b=0;
			}
			else if(f<32)
			{
				r=255;
				g=(f-32)*16;
				b=0;
			}
			else if (f<48)
			{
				r=255;
				g=255;
				b=(f-48)*16;
			}
			else
			{
				r=255;
				g=255;
				b=255;
			}
			FlameR[y][x]=r;		/* Store calculated RGB */
			FlameG[y][x]=g;
			FlameB[y][x]=b;
		}
	}

	for (y=0;y<Depth-6;y++)		/* DONT draw bottom lines - allow averaging */
	{
		for (x=0;x<Width;x++)
		{

			r=FlameR[y][x];
			g=FlameG[y][x];
			b=FlameB[y][x];
			r2=FlameR[y][x+1];
			g2=FlameG[y][x+1];
			b2=FlameB[y][x+1];
			r3=FlameR[y+1][x];
			g3=FlameG[y+1][x];
			b3=FlameB[y+1][x];
			r4=FlameR[y+1][x+1];
			g4=FlameG[y+1][x+1];
			b4=FlameB[y+1][x+1];

			setRGB0(&Screen[y][x],r,g,b);
			setRGB1(&Screen[y][x],r2,g2,b2);
			setRGB2(&Screen[y][x],r3,g3,b3);
			setRGB3(&Screen[y][x],r4,g4,b4);
			DrawPrim(&Screen[y][x]);
		}
		ModifyRnd(r);				/* Mess around with random numbers */
		ModifyRnd(b4);
		ModifyRnd(g2);
		ModifyRnd(PCnt);
	}
}


/*****************************************************************************
InitScreen
	Initialise poly's XY positions
*****************************************************************************/
void InitScreen(void)
{
int x;
int y;
int sx;
int sy;

	for (y=0;y<Depth-6;y++)
	{
		for (x=0;x<Width;x++)
		{
			sx=(x*CellSize);		/* Screen XY pos */
			sy=32+(y*CellSize);
			SetPolyG4(&Screen[y][x]);
			setXY4(&Screen[y][x],sx,sy,sx+CellSize,sy,sx,sy+CellSize,sx+CellSize,sy+CellSize);
		}
	}
}

int ModifyRnd(int f)
{
	Rnd+=f;
	Rnd+=54321;
	Rnd+=Frame;
	return (Rnd);	
}



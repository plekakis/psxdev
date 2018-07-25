/*******************************************************************
 *
 *    DESCRIPTION: Screen Overlay effects module
 *
 *    AUTHOR:	   dc 8/17/98
 *
 *    Explanation
 *
 *    HISTORY:    
 *******************************************************************/
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
#include <libsn.h>
#include <libsnd.h>
#include <libmath.h>
#include <gtemac.h>	   
#include <inline_c.h>	   

#include "main.h"
#include "effects.h"

extern int frame;
extern DB	db[2];			/* packet double buffer */
extern DB* 	cdb;			/* current db */

//32 elements aninated using cosine: range +-4 
#define COS_TABLE_4_ELEMENTS 32                
short cos_table_4[COS_TABLE_4_ELEMENTS]=       
	{                                             
	 4,                                           
	 4,                                           
	 4,                                           
	 3,                                           
	 3,                                           
	 2,                                           
	 1,                                           
	 1,                                           
	 0,                                           
	-1,                                           
	-1,                                           
	-2,                                           
	-3,                                           
	-3,                                           
	-4,                                           
	-4,                                           
	-4,                                           
	-4,                                           
	-4,                                           
	-3,                                           
	-3,                                           
	-2,                                           
	-1,                                           
	-1,                                           
	 0,                                           
	 1,                                           
	 1,                                           
	 2,                                           
	 3,                                           
	 3,                                           
	 4,                                           
	 4                                            
	};                                            

#define DISTORT_SIZE 16
#define DISTORT_NUM  32

short dist_vert_list[5][2]=
	{
		{-DISTORT_SIZE,-DISTORT_SIZE},
		{ DISTORT_SIZE,-DISTORT_SIZE},
		{ DISTORT_SIZE, DISTORT_SIZE},
		{-DISTORT_SIZE, DISTORT_SIZE},
		{-DISTORT_SIZE,-DISTORT_SIZE},
	};


typedef struct
{
short x_pos;
short y_pos;
short x_vec;
short y_vec;
}DISTORTION_CELL;

DISTORTION_CELL dist[DISTORT_NUM];





/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/*    Explanation
 *    Heres a simple motion blur example..
 *	  This is how it works:
 *	  The back drop image is rendered to the current frame buffer at 50% 
 *	  brightness.
 *	  Then the previous frame ( the back drop image with the foreground
 *	  objects already rendered on it is rendered to the current frame buffer.
 *	  The result is that the back ground image is now combined to reach 100%
 *	  brightness but the foreground objects are not so bright...
 *	  Then the current frames foreground objects are rendered over the top of
 *	  the image.
 *	  The cool thing about this is that you can have the blur effect on 3d 
 *	  objects as well as on simple sprites and you don't have to do it against
 *	  a solid background.
 *	  Of course its hideously inefficient and couldn't be used for much other 
 *	  than the front end.
 */
void motion_blur(char trans,unsigned long** poly_p,int frame)
{
POLY_FT4* ft4_p=(POLY_FT4*)*poly_p;

setPolyFT4(ft4_p);
setSemiTrans(ft4_p,1);
setUVWH(ft4_p,0,0,255,239);
setXYWH(ft4_p,0,0,255,239);
setRGB0(ft4_p,trans,trans,trans);
if(frame==0)
	ft4_p->tpage = getTPage(2,1,0,256);	
else
	ft4_p->tpage = getTPage(2,1,0,0);	

addPrim(db[frame].ot+OT_BACK,ft4_p);   
ft4_p++;

setPolyFT4(ft4_p);
setSemiTrans(ft4_p,1);
setUVWH(ft4_p,  0,0,64,239);
setXYWH(ft4_p,255,0,64,239);
setRGB0(ft4_p,trans,trans,trans);
if(frame==0)
	ft4_p->tpage = getTPage(2,1,256,256);	
else
	ft4_p->tpage = getTPage(2,1,256,0);	

addPrim(db[frame].ot+OT_BACK,ft4_p);   
ft4_p++;
*poly_p=(long*)ft4_p;

}
/**-------------------------------------------------------------------------**/

/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
void init_distortion_cells(void)
{
short i;

for(i=0; i<DISTORT_NUM; i++)
	{
	dist[i].x_pos = 2*DISTORT_SIZE+rand()%(256-(4*DISTORT_SIZE));
	dist[i].y_pos = 2*DISTORT_SIZE+rand()%(240-(4*DISTORT_SIZE));
	dist[i].x_vec = -2+rand()%4;
	dist[i].y_vec = -2+rand()%4;
	}
}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
void update_distortion_cells(unsigned long** poly_p,int frame)
{
short x,y;
DISTORTION_CELL dist_layer;
CVECTOR col = {127,127,127,0};
//distortion vectors
static short   counter=0;
short  dist_xpos;
short  dist_ypos;
short  dist_xvec;
short  dist_yvec;
POLY_FT4* ft4_p=(POLY_FT4*)*poly_p;

//this is the ying yang symbol that I'm using to cover up the working area
//of vram

setPolyFT4(ft4_p);
ft4_p->tpage = getTPage(2,1,384,256);	
setSemiTrans(ft4_p,0);
setShadeTex(ft4_p,1);
setRGB0(ft4_p,127,127,127);
setXYWH(ft4_p,15,185,32,32);
setUVWH(ft4_p,0,0,32,32);
addPrim(db[frame].ot+OT_FRONT,ft4_p);   
ft4_p++;
*poly_p=(long*)ft4_p;


//now process the screen as a grid
for(y=0; y<(FRAME_Y/32); y++)                                                                   
	{
	for(x=0; x<(FRAME_X)/32; x++)                                                               
		{                                                                                     
		dist_xpos = DISTORT_SIZE+(x*32); 
		dist_ypos = DISTORT_SIZE+(y*32);
		dist_xvec = cos_table_4[counter%COS_TABLE_4_ELEMENTS];
		dist_yvec = cos_table_4[counter%COS_TABLE_4_ELEMENTS];

		if(y&1)	dist_yvec*=-1;
		if(x&1)	dist_xvec*=-1;

		draw_distortion_cell(poly_p,
							 frame,
							 dist_xpos,
		                     dist_ypos,
							 dist_xvec,
							 dist_yvec,
							 16,
							 &col);
		}

	}

	counter++;


}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/

/**************************************************************************
In order for the distortion process to work what happens is this:
firstly the area to be distorted in this case a 32*32 area is rendered into 
a temp buffer and the contents of this temp buffer is reused to draw the 
distorted version.  The distortion I'm using is pretty simple, its just
drawing the quad as 4 trianges with a common center vertex. By animating 
the position of the center vertex a distortion effect is produced.
You have to have a temp area as if you attempt to render the distorted 
version directly over the top of the source texture you are destroying the 
source texture as you do so.

Firstly I set the draw env settings to support a frame_x+32 wide drawing 
environment but this is pretty wasteful on vram.
Then I messed about using dr_area prims to set the drawing env in the 
ordering table but I couldn't get this to work (cos I'm stupid, it 
will work I just had a bug) but also it was really slow doing it this way.
so I decided to go with the nasty solution... 
use part of the current draw environment as a drawing buffer 
and then patch over it.... thats what the ying yang sprite is for...
note that on an NTSC screen there is so much overscan top and bottom you 
could easily hide quite a lot up there eg frame_x*16 top and bottom, but 
this area is visible in PAL mode.


OK this effect is pretty cheesy in its current state, but it could be used
for a variety of effects... eg water drop splashes etc
but heres my favorite.... 
Imagine particle sprites with the distortion on them, you could make some
really really cool heat haze effects and it doesn't even use transparency..

***************************************************************************/

void draw_distortion_cell(unsigned long** poly_p,int frame,short x_pos,short y_pos,short x_dist,short y_dist,short distortion_size,CVECTOR* col)
{
short 		i;
long 		tpage;
short 		draw_tpage_y;

POLY_FT4* 	ft4_p;
POLY_GT3* 	gt3_p;

//process polygons in distortion grid
//basically 4 trigangles and I move the center common vertex around a bit
//to give the distortion effect
gt3_p=(POLY_GT3*)*poly_p;

for(i=0; i<4; i++)
	{
	setPolyGT3(gt3_p);
	setSemiTrans(gt3_p,0);
	setShadeTex(gt3_p,0);
	setRGB0(gt3_p,col->r,col->g,col->b);
	setRGB1(gt3_p,127,127,127);
	setRGB2(gt3_p,127,127,127);

	gt3_p->tpage = getTPage(2,1,0,frame<<8);	

	gt3_p->x0 =	x_pos+x_dist;	
	gt3_p->y0 = y_pos+y_dist;

	gt3_p->x1 =	x_pos+dist_vert_list[i][0];
	gt3_p->y1 = y_pos+dist_vert_list[i][1];

	gt3_p->x2 =	x_pos+dist_vert_list[i+1][0];
	gt3_p->y2 = y_pos+dist_vert_list[i+1][1];

	gt3_p->u0 =	15 +distortion_size;	
	gt3_p->v0 = 185+distortion_size;

	gt3_p->u1 =	15 +distortion_size+dist_vert_list[i][0];
	gt3_p->v1 = 185+distortion_size+dist_vert_list[i][1];

	gt3_p->u2 =	15 +distortion_size+dist_vert_list[i+1][0];
	gt3_p->v2 = 185+distortion_size+dist_vert_list[i+1][1];

	addPrim(db[frame].ot+OT_FRONT,gt3_p);   
	gt3_p++;
	}

*poly_p=(long*)gt3_p;


//store the source texture to an offscreen buffer
//ok note that this gets added to the ot after the distorted version. But 
//they are rendered in reverse order.... so in terms of the ordering table
//this bit gets done first.
//there is another problem here in that in any screen mode other than
//256*240 it is likely that some of the source textures will span a 16 bit
//tpage boundary in which case they have to be rendered into the temp
//buffer as 2 different polys

//------------------------------------------------------------------------
//don't do this because its really really slow
//set the drawing back to the !screen area 
//setRECT(&draw_area_rect,0,(frame)<<8,FRAME_X,FRAME_Y);
//dr_area_p=(DR_AREA*)poly_p;
//setDrawArea(dr_area_p,&draw_area_rect);
//addPrim(db[frame].ot+OT_FRONT,dr_area_p);   
//dr_area_p++;
//poly_p=(long*)dr_area_p;
//------------------------------------------------------------------------

ft4_p=(POLY_FT4*)*poly_p;

//get the tpage y offset
//if(frame==0)
//	draw_tpage_y=0;	
//else
//	draw_tpage_y=256;	

draw_tpage_y=frame<<8;

//if the poly is completely in left tpage 0..255
if(x_pos+distortion_size<256)
	{
	ft4_p->tpage = getTPage(2,0,0,draw_tpage_y);	
	setPolyFT4(ft4_p);
	setSemiTrans(ft4_p,0);
	setShadeTex(ft4_p,1);
	setRGB0(ft4_p,127,127,127);
	setXYWH(ft4_p,15,185,distortion_size*2,distortion_size*2);

	setUVWH(ft4_p,x_pos-distortion_size,y_pos-distortion_size,distortion_size*2,distortion_size*2);
	addPrim(db[frame].ot+OT_FRONT,ft4_p);   
	ft4_p++;
	}
else
//if the poly is completely in right tpage 256..511
if(x_pos-distortion_size>255)
	{
	ft4_p->tpage = getTPage(2,0,256,draw_tpage_y);	
	setPolyFT4(ft4_p);
	setSemiTrans(ft4_p,0);
	setShadeTex(ft4_p,1);
	setRGB0(ft4_p,127,127,127);
	setXYWH(ft4_p,15,185,distortion_size*2,distortion_size*2);

	setUVWH(ft4_p,-256+x_pos-distortion_size,y_pos-distortion_size,distortion_size*2,distortion_size*2);
	addPrim(db[frame].ot+OT_FRONT,ft4_p);   
	ft4_p++;
	}
//poly is in both tpages so have to render 2 polys....
else
	{
	ft4_p->tpage = getTPage(2,0,0,draw_tpage_y);	
	setPolyFT4(ft4_p);
	setSemiTrans(ft4_p,0);
	setShadeTex(ft4_p,1);
	setRGB0(ft4_p,127,127,127);
	setXYWH(ft4_p,15,185,distortion_size*2,distortion_size*2);

	setUVWH(ft4_p,x_pos-distortion_size,y_pos-distortion_size,255-(x_pos-distortion_size),distortion_size*2);

//	printf("sp %d %d\n",ft4_p->u0,ft4_p->u1);

	addPrim(db[frame].ot+OT_FRONT,ft4_p);   
	ft4_p++;

	ft4_p->tpage = getTPage(2,0,256,draw_tpage_y);	
	setPolyFT4(ft4_p);
	setSemiTrans(ft4_p,0);
	setShadeTex(ft4_p,1);
	setRGB0(ft4_p,127,127,127);
	setXYWH(ft4_p,15,185,distortion_size*2,distortion_size*2);
	setUVWH(ft4_p,0,y_pos-distortion_size,x_pos+distortion_size-256,distortion_size*2);
	addPrim(db[frame].ot+OT_FRONT,ft4_p);   
	ft4_p++;
	}
*poly_p=(long*)ft4_p;

//------------------------------------------------------------------------
//don't do this, its really slow, mind you this code does show how to set
//the drawing env in the ot which in itself is a pretty cool thing to be 
//able to do, just not 80 times a frame....
//set the drawing area to the small offscreen buffer
//I think I only need 1 buffer not 2 after all.
//setRECT(&draw_area_rect,320,(frame<<8),distortion_size*2,distortion_size*2);
//dr_area_p=(DR_AREA*)poly_p;
//SetDrawArea(dr_area_p,&draw_area_rect);
//addPrim(db[frame].ot+OT_FRONT,dr_area_p);   
//dr_area_p++;
//poly_p=(long*)dr_area_p;
//------------------------------------------------------------------------

}
/**-------------------------------------------------------------------------**/




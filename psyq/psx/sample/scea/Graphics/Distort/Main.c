/*******************************************************************
 *
 *    DESCRIPTION: Local Distortion Effect Demo

I'm releasing this, warts and all cos I can't be arsed to finish it....
but it might still be useful/provide inspiration...
or you might just enjoy laughing at my crappy code....

 ok this is broken in 2 places.... 
 if you leave it running for about 5 minutes one of the counters 
 flips negative and the distortion doodaahs go weird
 
 I did something stupid when I added the texture buffer ( its currently 
 under the yingyang symbol... see the comments in effects.c about how
 to change the size of the drawenvironment.  You can make it wider when 
 you need the offscreen buffer and then set it back afterwards....
 this is probably the best way to go....

Select switches between mblur and mblur+distortion overlay....


 *
 *    AUTHOR:	   Coombsey  9/25/98
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
#include "ctrller.h"
#include "tmd.h"



/**----- globals ------------------------------------------------------**/
int frame_counter=0;
int frame=0;
DB		db[2];			/* packet double buffer */
DB* 	cdb;			/* current db */


ControllerPacket buffer1, buffer2;

extern unsigned long	backdroptim[]; 
extern unsigned long	spritestim[]; 
extern unsigned long	testshiptmd[]; 

#define MAX_GT4 512
unsigned long poly_store[2][MAX_GT4*sizeof(POLY_GT4)];
unsigned long* poly_p;


void draw_back_drop(char trans);



/**---------------------------------------------------------------------**/
/**---------------------------------------------------------------------**/
/**---------------------------------------------------------------------**/
main()
{

ResetCallback();	
init_graph();	
InitPAD(&buffer1,MAX_CONTROLLER_BYTES,&buffer2,MAX_CONTROLLER_BYTES);
StartPAD();

SetBackColor(64, 64, 64); 	

load_texture(backdroptim);
load_texture(spritestim);
init_distortion_cells();
test_new_tmd_code();

}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
// loads a 16bit image from vram onto the current frame buffer.
// if trans is 0 - then the image is not semi transparent and is displayed at normal brightness
// else the image is displayed semi trans with brightness trans
/**-------------------------------------------------------------------------**/
void draw_back_drop(char trans)
{
POLY_FT4* ft4_p=(POLY_FT4*)poly_p;
char semi,shade;

if(trans)
	{
	semi 	= 1;
	shade 	= 0;
	}
else
	{
	semi 	= 0;
	shade 	= 1;
	}

setPolyFT4(ft4_p);
setSemiTrans(ft4_p,semi);
setShadeTex(ft4_p,shade);
setUVWH(ft4_p,0,0,255,239);
setXYWH(ft4_p,0,0,255,239);
setRGB0(ft4_p,trans,trans,trans);
ft4_p->tpage = getTPage(2,1,384,0);	
addPrim(db[frame].ot+OT_BACK,ft4_p);   
ft4_p++;

setPolyFT4(ft4_p);
setSemiTrans(ft4_p,semi);
setShadeTex(ft4_p,shade);
setUVWH(ft4_p,  0,0,64,239);
setXYWH(ft4_p,255,0,64,239);
ft4_p->tpage = getTPage(2,1,640,0);	
setRGB0(ft4_p,trans,trans,trans);
addPrim(db[frame].ot+OT_BACK,ft4_p);   

ft4_p++;
poly_p=(long*)ft4_p;

}
/**-------------------------------------------------------------------------**/




/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
test_new_tmd_code()
{
OBJ_G3 		test_obj;

MATRIX 		ship_mat;
SVECTOR 	ship_ang={0,0,0,0};
VECTOR 		ship_vec={0,0,530,0};

int 		test;
int			time;
int 		mode=1;
int         isok =-15;
MATRIX	light_cmat = 
	{
    ONE/4,  0,  0, /* R */
    ONE/4,  0,  0, /* G */
    ONE/2,  0,  0, /* B */
    };

SVECTOR	light_ang = {512,3072,0};	
MATRIX light_mat;
MATRIX ship_light_mat;

test = loadTMD_G3(testshiptmd,&test_obj,0);    
SetColorMatrix(&light_cmat);		
do{
   	frame_counter++;
	frame = (frame==0)? 1: 0;			
	cdb = (cdb==db)? db+1: db;			
	ClearOTagR(db[frame	].ot, OTSIZE);		
	FntPrint("~tmd test prog %d\n",frame_counter);
	poly_p = &poly_store[frame][0];

if (GoodData(&buffer1) && GetType(&buffer1)==STD_PAD & isok>0)            
		{
	  	if ( PadKeyIsPressed(&buffer1,PAD_SEL)!=0) 
			{
			isok=-15;
			if(mode==0) mode=1;
			else mode=0;
			}	
		}
	isok++;
	if(mode) FntPrint("mblur and distortion\n");
	if(!mode) FntPrint("mblur\n");



	if(1)
		{
		ship_ang.vz+=64;
		ship_ang.vy+=31;
		}
	else
	if (GoodData(&buffer1) && GetType(&buffer1)==STD_PAD)            
		{
	  	if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
			ship_vec.vz+=2;
	  	if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
			ship_vec.vz-=2;
	  	if ( PadKeyIsPressed(&buffer1,PAD_LU)!=0) 
			ship_ang.vz+=64;
	  	if ( PadKeyIsPressed(&buffer1,PAD_LD)!=0) 
			ship_ang.vz-=64;
	  	if ( PadKeyIsPressed(&buffer1,PAD_LL)!=0) 
			ship_ang.vy+=64;
	  	if ( PadKeyIsPressed(&buffer1,PAD_LR)!=0) 
			ship_ang.vy-=64;
		}

//	FntPrint("ship_vec.vz =%d\n",ship_vec.vz);

	light_ang.vy+=64;
	RotMatrix(&light_ang,&light_mat );	
	MulMatrix0(&ship_mat,&light_mat,&ship_light_mat);   
   	SetLightMatrix(&ship_light_mat);

	RotMatrix(&ship_ang,&ship_mat);   
	SetRotMatrix(&ship_mat);
	TransMatrix(&ship_mat, &ship_vec);	
	SetTransMatrix(&ship_mat);

	time  = GetRCnt(RCntCNT1);
 	draw_object_g3(&test_obj,&poly_p,frame);
	FntPrint("draw_obj_g3 %d\n",GetRCnt(RCntCNT1)-time);


	if(mode)
	{
	time  = GetRCnt(RCntCNT1);
	update_distortion_cells(&poly_p,frame);
	FntPrint("update_dist. %d\n",GetRCnt(RCntCNT1)-time);
	}

	if(1)
		{
		draw_back_drop(32);
		motion_blur(96,&poly_p,frame);
		}
	else
		draw_back_drop(0);


	if (GoodData(&buffer1) && GetType(&buffer1)==STD_PAD)            
		{
	  	if ( PadKeyIsPressed(&buffer1,PAD_FRT)!=0) 
			vram_viewer();
		}


   	FntFlush(-1);		
   	DrawSync(0);						
   	VSync(0);							
   	PutDrawEnv(&db[frame].draw); 		
   	PutDispEnv(&db[frame].disp); 		
   	DrawOTag(db[frame].ot+OTSIZE-1);	  			
}while(1);


}
/**-------------------------------------------------------------------------**/

//*************************************************************************
//*************************************************************************
//*************************************************************************
print_matrix(MATRIX *mat)
{
printf("%5d %5d %5d\n",mat->m[0][0],mat->m[0][1],mat->m[0][2]);
printf("%5d %5d %5d\n",mat->m[1][0],mat->m[1][1],mat->m[1][2]);
printf("%5d %5d %5d\n",mat->m[2][0],mat->m[2][1],mat->m[2][2]);
printf("%5d %5d %5d\n\n",mat->t[0],mat->t[1],mat->t[2]);
}
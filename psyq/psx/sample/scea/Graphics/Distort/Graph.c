/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
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

#include "main.h"	     	
#include "ctrller.h"	     	

extern int frame;
extern DB	db[2];			/* packet double buffer */
extern DB* 	cdb;			/* current db */

extern ControllerPacket buffer1, buffer2;


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
init_graph()
{
RECT rect;

ResetGraph(0);
SetGraphDebug(1);

InitGeom();							
SetGeomOffset(160,120);				
SetGeomScreen(300);				
SetBackColor(12, 12, 12);	


//note that I've set up a 32 pixel wide off screen frame buffer
//to store things in...
SetDefDrawEnv(&db[0].draw, 0,   0, FRAME_X+32, FRAME_Y);
SetDefDispEnv(&db[0].disp, 0, 256, FRAME_X, FRAME_Y);

SetDefDrawEnv(&db[1].draw, 0, 256, FRAME_X+32, FRAME_Y);
SetDefDispEnv(&db[1].disp, 0,   0, FRAME_X, FRAME_Y);

SetDefDrawEnv(&db[0].draw, 0,   0, FRAME_X, FRAME_Y);
SetDefDispEnv(&db[0].disp, 0, 256, FRAME_X, FRAME_Y);

SetDefDrawEnv(&db[1].draw, 0, 256, FRAME_X, FRAME_Y);
SetDefDispEnv(&db[1].disp, 0,   0, FRAME_X, FRAME_Y);

db[0].draw.dfe = db[1].draw.dfe = 1;
db[0].draw.dtd = db[1].draw.dtd = 0;

db[0].draw.isbg=db[1].draw.isbg= 1;
db[0].draw.r0  =db[1].draw.r0  = 0;
db[0].draw.b0  =db[1].draw.b0  = 0;
db[0].draw.g0  =db[1].draw.g0  = 0;	

FntLoad(640,256);
SetDumpFnt(FntOpen(20, 20, 300, 200, 0, 512));

setRECT(&rect,0,0,320,511);
ClearImage(&rect,0,0,0);

SetBackColor(0,0,0);      /* set background(ambient) color*/
SetFarColor(50,50,50);       /* set far color */
SetFogNear(400,OTSIZE);      /* set fog parameter*/


SetDispMask(1);	
}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
void load_texture(u_long addr)
{
RECT	rect;
GsIMAGE	tim;

GsGetTimInfo((u_long *)(addr+4),&tim);
rect.x = tim.px;
rect.y = tim.py;
rect.w = tim.pw;
rect.h = tim.ph;
printf("loading bmap: addr = 0x%x   %d %d %d %d\n",addr,rect.x,rect.y,rect.w,rect.h);
LoadImage(&rect,tim.pixel);

if((tim.pmode>>3)&0x01)
   {
	rect.x = tim.cx;
	rect.y = tim.cy;

	rect.w = tim.cw;
	rect.h = tim.ch;

	LoadImage(&rect,tim.clut);
   printf("loading clut: %d %d %d %d\n",rect.x,rect.y,rect.w,rect.h);

   }
}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
void vram_viewer(void)
{
short xpos=0;
short ypos=0;
short exit=FALSE;
char isbg =db[0].draw.isbg; 

db[0].draw.isbg=db[1].draw.isbg= 0;

printf("********************************\n");
printf("Vram Viewer\n");
printf("********************************\n");
printf("Triangle quits\n");
printf("********************************\n\n");




while(exit!=TRUE)
	{
	frame = (frame==0)? 1: 0;			
	cdb = (cdb==db)? db+1: db;			

	//=========update display environment==============
	if (GoodData(&buffer1) && GetType(&buffer1)==STD_PAD)            
		{
	  	if ( PadKeyIsPressed(&buffer1,PAD_LU)!=0) 
			ypos--;
	  	if ( PadKeyIsPressed(&buffer1,PAD_LD)!=0) 
			ypos++;
	  	if ( PadKeyIsPressed(&buffer1,PAD_LR)!=0) 
			xpos++;
	  	if ( PadKeyIsPressed(&buffer1,PAD_LL)!=0) 
			xpos--;
	  	if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0) //triangle quits
			exit=TRUE;
		}

	SetDefDispEnv(&cdb->disp,xpos,ypos, FRAME_X, FRAME_Y);
   	VSync(0);							
   	PutDrawEnv(&db[frame].draw); 		
   	PutDispEnv(&db[frame].disp); 		
	}

//reset display environment
SetDefDispEnv(&db[0].disp, 0, 256, FRAME_X, FRAME_Y);
SetDefDispEnv(&db[1].disp, 0,   0, FRAME_X, FRAME_Y);
db[0].draw.isbg=db[1].draw.isbg= isbg; 


}
/**-------------------------------------------------------------------------**/

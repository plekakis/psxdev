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


extern DB	db[2];			/* packet double buffer */
extern DB* 	cdb;			/* current db */



/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
init_graph()
{
RECT rect;

ResetGraph(0);
SetGraphDebug(0);

InitGeom();							
SetGeomOffset(FRAME_X/2,FRAME_Y/2);				
SetGeomScreen(300);				
SetBackColor(128, 128, 128);	

SetDefDrawEnv(&db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
SetDefDispEnv(&db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);

SetDefDrawEnv(&db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
SetDefDispEnv(&db[1].disp, 0,   0,       FRAME_X, FRAME_Y);

db[0].draw.dfe = db[1].draw.dfe = 1;
db[0].draw.dtd = db[1].draw.dtd = 0;

db[0].draw.isbg=db[1].draw.isbg= 1;
db[0].draw.r0  =db[1].draw.r0  = 0;
db[0].draw.b0  =db[1].draw.b0  = 70;
db[0].draw.g0  =db[1].draw.g0  = 0;	

FntLoad(320,0);
SetDumpFnt(FntOpen(20, 20, 320, 200, 0, 512));

setRECT(&rect,0,0,320,512);
ClearImage(&rect,0,0,0);

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


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

#include "graph.h"


extern DB	   db[2];		/* packet double buffer */
extern DB* 	cdb;			/* current db */



/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/

init_graph()
{
RECT rect;

ResetGraph(0);
#ifdef USE_NTSC
	SetVideoMode(MODE_NTSC);
#else
	SetVideoMode(MODE_PAL);
#endif


SetGraphDebug(0);

InitGeom();							
SetGeomOffset(FRAME_X/2,FRAME_Y/2);				
SetGeomScreen(500);				
SetBackColor(128, 128, 128);	

SetDefDrawEnv(&db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
SetDefDispEnv(&db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);

SetDefDrawEnv(&db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
SetDefDispEnv(&db[1].disp, 0,   0,       FRAME_X, FRAME_Y);

db[1].disp.screen.x = db[0].disp.screen.x = SCREEN_X;
db[1].disp.screen.y = db[0].disp.screen.y = SCREEN_Y;

db[1].disp.screen.h = db[0].disp.screen.h = PAL_MAGIC;
db[1].disp.screen.w = db[0].disp.screen.w = PAL_MAGIC;

db[0].draw.dfe = db[1].draw.dfe = 1;

db[0].draw.isbg=db[1].draw.isbg= 1;
db[0].draw.r0  =db[1].draw.r0  = 0;
db[0].draw.b0  =db[1].draw.b0  = 0;
db[0].draw.g0  =db[1].draw.g0  = 0;	

setRECT(&rect,0,0,FRAME_X,512);
ClearImage(&rect,0,0,0);

SetDispMask(1);	

}

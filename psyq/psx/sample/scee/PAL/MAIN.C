/*****************************************************************************/
/*****************************************************************************/
/** main.c 																						 **/
/*****************************************************************************/
/*****************************************************************************/

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
#include "graphic.h"


extern unsigned long marvindata[]; 


/**-------------------- display -------------------------------------------**/
DB	 db[2];		/* packet double buffer */
DB* cdb;			/* current db */



/**-------------------- main ----------------------------------------------**/
main()
{
int access = FALSE;


ResetCallback();	



/** set to pal **/

ResetGraph(0);
SetVideoMode(MODE_PAL);

SetGraphDebug(0);


SetDefDrawEnv(&db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
SetDefDispEnv(&db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);
SetDefDrawEnv(&db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
SetDefDispEnv(&db[1].disp, 0,   0,       FRAME_X, FRAME_Y);

db[1].disp.screen.x = db[0].disp.screen.x = SCREEN_X;
db[1].disp.screen.y = db[0].disp.screen.y = SCREEN_Y;

db[1].disp.screen.h = db[0].disp.screen.h = PAL_MAGIC;
db[1].disp.screen.w = db[0].disp.screen.w = PAL_MAGIC;


db[0].draw.dfe = db[1].draw.dfe = 1;



FntLoad(640,256);
FntOpen(10, 16, 500, 250, 0,800);

init_prim(&db[0]);
init_prim(&db[1]);

/* marvin's our special friend that nobody else can see */
bitmap((u_char*)marvindata);
init_marvin(&db[0]);
init_marvin(&db[1]);

SetDispMask(1);


do
{
cdb = (cdb==db)? db+1: db;			
ClearOTag(cdb->ot, OTSIZE);		
	
FntPrint("    pal demo\n");
FntPrint("    ********\n");
animate_marvin();

FntFlush(-1);
DrawSync(0);						
VSync(0);							
PutDrawEnv(&cdb->draw); 		
PutDispEnv(&cdb->disp); 		
DrawOTag(cdb->ot);	 
}while(1);


}


/**------------------------------------------------------------------------**/
/**-------------------- init_prim -----------------------------------------**/
/**------------------------------------------------------------------------**/

init_prim(DB *db)
{
int counter;
int i;

db->draw.isbg = 1;
db->draw.r0 = 100;
db->draw.b0 = 100;
db->draw.g0 =   0;	
}
/**------------------------------------------------------------------------**/





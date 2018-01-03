
/*****************************************************************************/
/*****************************************************************************/
/** graphic.c 			    																	 **/
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


extern unsigned long icontim[];

extern DB	 db[2];		/* packet double buffer */
extern DB*   cdb;			/* current db */



/*****************************************************************************/
/* bitmap loader from ram */
/*****************************************************************************/
bitmap(u_long *addr)
{
GsIMAGE TexInfo;
RECT rect1;

addr++;

GsGetTimInfo(addr, &TexInfo);
rect1.x = TexInfo.px;
rect1.y = TexInfo.py;
rect1.w = TexInfo.pw;
rect1.h = TexInfo.ph;

printf("loading pixel data %d %d   %d %d",rect1.x,rect1.y,rect1.w,rect1.h);
LoadImage(&rect1,TexInfo.pixel); 


}
/*****************************************************************************/





/*****************************************************************************/
/* create the marvin graphic */
/*****************************************************************************/
init_marvin(DB *db)
{

SetPolyFT4(&db->marvin);	
db->marvin.tpage = GetTPage(2,1,640,0);	

db->marvin.r0 = 100;
db->marvin.g0 = 100;
db->marvin.b0 = 100;

db->marvin.u0 = 1;		db->marvin.v0 = 0;
db->marvin.u1 = 127;	   db->marvin.v1 = 0;
db->marvin.u2 = 1;		db->marvin.v2 = 127;
db->marvin.u3 = 127;	   db->marvin.v3 = 127;

}
/*****************************************************************************/




draw_marvin(int xpos,int ypos)
{
cdb->marvin.x0 = xpos-48; cdb->marvin.y0 = ypos-32;
cdb->marvin.x1 = xpos+48; cdb->marvin.y1 = ypos-32;
cdb->marvin.x2 = xpos-48; cdb->marvin.y2 = ypos+32;
cdb->marvin.x3 = xpos+48; cdb->marvin.y3 = ypos+32;
cdb->marvin.x3 = xpos+48; cdb->marvin.y3 = ypos+32;

AddPrim(cdb->ot,&cdb->marvin); 	
}
/*****************************************************************************/

/*****************************************************************************/
/* animate_marvin */
/*****************************************************************************/


animate_marvin()
{
static x=160;
static y=120;
static dir = -1;

draw_marvin(x,y);

if(y< 35) dir=1;
if(y>215) dir=-1;
y+=dir;

}


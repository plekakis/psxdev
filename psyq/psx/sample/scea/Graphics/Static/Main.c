/*******************************************************************
 *
 *    DESCRIPTION: Using texture window for that techi sci fi look
 *
 *    AUTHOR:	   dc 21/9/98
 *
 *    HISTORY:    
 *
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

#include "main.h"
#include "ctrller.h"

void do_static(int frame,char r,char g,char b);


/**----- globals ------------------------------------------------------**/
DB		db[2];			/* packet double buffer */
DB* 	cdb;			/* current db */
ControllerPacket buffer1, buffer2;

extern unsigned long	surrealtim[];	

int frame_counter=0;
int frame=0;


/**---------------------------------------------------------------------**/
/**---------------------------------------------------------------------**/
/**---------------------------------------------------------------------**/
main()
{
char test=0;
char dir=4;

ResetCallback();	
init_graph();	
InitPAD(&buffer1,MAX_CONTROLLER_BYTES,&buffer2,MAX_CONTROLLER_BYTES);
StartPAD();

load_texture(surrealtim);
init_backdrop();
init_static();

do{
   	frame_counter++;
	frame = (frame==0)? 1: 0;			
	cdb = (cdb==db)? db+1: db;			
	ClearOTagR(db[frame	].ot, OTSIZE);		

	do_static(frame,test,test,test);
	do_backdrop(frame);
	test+=dir;
	if(test==0 || test==128)
		{
		dir*=-1;
		}

   	DrawSync(0);						
   	VSync(0);							
   	PutDrawEnv(&db[frame].draw); 		
   	PutDispEnv(&db[frame].disp); 		
   	DrawOTag(db[frame].ot+OTSIZE-1);	  			
}while(1);


}
/**-------------------------------------------------------------------------**/


DR_MODE backdrop_twin_on[2];
POLY_FT4 backdrop[2];
DR_MODE backdrop_twin_off[2];

/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
init_backdrop()
{
RECT twin_on = {0,0,63,63};
RECT twin_off = {0,0,0,0};

setPolyFT4(&backdrop[0]);
setSemiTrans(&backdrop[0],1);    
setShadeTex(&backdrop[0],1);    
setXYWH(&backdrop[0],0,0,320,240);
setUVWH(&backdrop[0],0,0,255,255);
setRGB0(&backdrop[0],127,127,127);
backdrop[0].tpage = getTPage(2,1,320,0);  	
memcpy(&backdrop[1],&backdrop[0],sizeof(POLY_FT4));

SetDrawMode(&backdrop_twin_off[0],1,0,backdrop[0].tpage,&twin_off);
SetDrawMode(&backdrop_twin_on[0],1,0,backdrop[0].tpage,&twin_on);
SetDrawMode(&backdrop_twin_off[1],1,0,backdrop[1].tpage,&twin_off);
SetDrawMode(&backdrop_twin_on[1],1,0,backdrop[1].tpage,&twin_on);
}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
do_backdrop(int frame)
{
setUVWH(&backdrop[frame],/*63-framecounter%64*/0,63-frame_counter%64,192,192);
addPrim(db[frame].ot+1,&backdrop_twin_off[frame]);  
addPrim(db[frame].ot+1,&backdrop[frame]);  
addPrim(db[frame].ot+1,&backdrop_twin_on[frame]);  
	
}
/**----------------------------------------------------------------------**/


typedef struct
{
DR_MODE 	twin_on[2];
POLY_FT4 	poly[2];
DR_MODE 	twin_off[2];
}SSTATIC;

#define NUM_STATIC 8

SSTATIC stat[NUM_STATIC];

/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
init_static()
{
RECT twin_on = {0,64,31,31};
RECT twin_off = {0,0,0,0};
char i;

for(i=0; i<NUM_STATIC; i++)
	{
	setPolyFT4(&stat[i].poly[0]);
	setSemiTrans(&stat[i].poly[0],1);    
	setShadeTex(&stat[i].poly[0],0);    
	setXYWH(&stat[i].poly[0],0,0,320,240);
	setUVWH(&stat[i].poly[0],0,0,255,255);
	setRGB0(&stat[i].poly[0],127,127,127);
	stat[i].poly[0].tpage = getTPage(2,1,320,0);  	
	memcpy(&stat[i].poly[1],&stat[i].poly[0],sizeof(POLY_FT4));

	SetDrawMode(&stat[i].twin_off[0],1,0,stat[i].poly[0].tpage,&twin_off);
	SetDrawMode(&stat[i].twin_on[0] ,1,0,stat[i].poly[0].tpage,&twin_on);
	SetDrawMode(&stat[i].twin_off[1],1,0,stat[i].poly[1].tpage,&twin_off);
	SetDrawMode(&stat[i].twin_on[1] ,1,0,stat[i].poly[1].tpage,&twin_on);
	}

}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
void do_static(int frame,char r,char g,char b)
{
char i;
for(i=0; i<NUM_STATIC; i++)
	{
	setUVWH(&stat[i].poly[frame],32-(rand()%32),32-(rand()%32),192,192);
	setRGB0(&stat[i].poly[frame],r,g,b);
	addPrim(db[frame].ot+1,&stat[i].twin_off[frame]);  
	addPrim(db[frame].ot+1,&stat[i].poly[frame]);  
	addPrim(db[frame].ot+1,&stat[i].twin_on[frame]);  
	}	

}
/**----------------------------------------------------------------------**/


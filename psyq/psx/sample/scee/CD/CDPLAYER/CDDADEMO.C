/******************************************************************************/
/******  An example of messing with digital audio                        ******/
/******************************************************************************/
/**------------------- defines  ---------------------------------------------**/

#define DELAY 15


/**------------------- includes ---------------------------------------------**/
#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include <libcd.h>
#include "ctrller.h"



/**-------------------- globals ---------------------------------------------**/

/** pads **/
ControllerPacket buffer1, buffer2;



/** display **/
DRAWENV     draw;    							
DISPENV     disp;    							
static RECT bg = {0, 0, 640, 480};			


/** cd related **/
CdlLOC	loc[100];
u_char	param[4], result[8];




/**-------------------- functions -------------------------------------------**/


/**--------------------------------------------------------------------------**/
/** clear display **/
/**--------------------------------------------------------------------------**/

display_clear()
{
FntFlush(-1);
ClearImage(&bg, 0, 0, 0);
}
/**--------------------------------------------------------------------------**/


/**--------------------------------------------------------------------------**/
/** update the screen with all the FntPrints since last clear display        **/
/**--------------------------------------------------------------------------**/

display_write()
{
VSync(0);	
ClearImage(&bg, 0, 0, 0);
FntFlush(-1);
DrawSync(0);
}
/**--------------------------------------------------------------------------**/











/**-------------------- main ------------------------------------------------**/
main()
{
int	tracks;				/*no of tracks*/
int	i;     				/*general purpose */
int	current_track;		/*track currently being played*/
int   old_track;			/*previous track*/
int	button_pressed;


u_char	p_com;			/* previous command */


ResetCallback();	
ResetGraph(0);
SetGraphDebug(0);

FntLoad(640, 0);
FntOpen(10, 16, 300, 200, 0, 800);

SetDefDrawEnv(&draw, 0,   0, 320, 240);
SetDefDispEnv(&disp, 0,   0, 320, 240);

ClearImage(&bg, 0, 0, 0);
SetDispMask(1);

PutDrawEnv(&draw);
PutDispEnv(&disp);   

InitPAD(&buffer1,MAX_CONTROLLER_BYTES,&buffer2,MAX_CONTROLLER_BYTES);
StartPAD();

CdInit();

param[0] = CdlModeRept|CdlModeDA;	/* report ON / CD-DA ON */
CdControlB(CdlSetmode, param, 0); 
tracks = CdGetToc(loc);			/* TOC  */

CdSetDebug(0);


current_track = 1;
CdControlB(CdlSetloc, (u_char *)&loc[1], 0);	 /* seek to start of track 1 */
CdControlB((p_com = CdlPlay), 0, 0);		    /* play track 1             */



display_clear();
do
{
old_track = current_track;

FntPrint("\n\n");
FntPrint("CD DA EXAMPLE\n");
FntPrint("*************\n");
FntPrint("\n");


FntPrint("TRACKS :%d CURRENT TRACK :%d\n\n",tracks,current_track);
/* display a list of the da tracks **/
FntPrint("  ");	 /*leading tab on first track line*/
for(i=1; i<=tracks; i++)
	{
	if(i==current_track)
		{
		FntPrint(">");
		}
	else 
		{
		FntPrint(" ");
		}

	FntPrint("t:");	
	if(i<10)
		{
		FntPrint(" ");	/*leading 0 columns align*/
		}
	FntPrint("%d",i);	
 
	if(i==current_track)
		{
		FntPrint("<");
		}
	else 
		{
		FntPrint(" ");
		}

	FntPrint(" ");

	if(i % 4 ==0 )
		{FntPrint("\n  ");
		}
	}

if(button_pressed==0)
	{
	if (PadKeyIsPressed(&buffer1,PAD_LR)!=0) 
		{
		current_track++;
		button_pressed =DELAY;
		printf("ow!");
		}
	if (PadKeyIsPressed(&buffer1,PAD_LL)!=0) 
		{
		current_track--;
		button_pressed =DELAY;
		}
	if (PadKeyIsPressed(&buffer1,PAD_LU)!=0) 
		{
		current_track-=4;
		button_pressed =DELAY;
		}
	if (PadKeyIsPressed(&buffer1,PAD_LD)!=0) 
		{
		current_track+=4;
		button_pressed =DELAY;
		}


	}

if (current_track>tracks) 
	{
	current_track = tracks;
	}

if (current_track<1) 
	{
	current_track = 1;
	}



printf("current_track old_track %d %d \n",current_track,old_track);
if(current_track != old_track)
	{
	CdControlB(CdlSetloc, (u_char *)&loc[current_track], 0);	 /* seek to start of track */
	CdControlB((p_com = CdlPlay), 0, 0);		                /* play track             */
	old_track=current_track;
	}





display_write();
if(button_pressed >0)
	{
	button_pressed--;
	}

}
while(1);






}
/** main end **/

/*****************************************************************************/
/*****************************************************************************/
/** multitap.c       																		 **/
/*****************************************************************************/
/*****************************************************************************/
/** this program shows how to make use of ctrller.h to read data from the   **/
/** controllers.  it should be quite easy to figure everything out by       **/
/** looking in ctrller.h  have fun!                                         **/
/*****************************************************************************/

#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include "main.h"
#include "ctrller.h"


/**--------- global variables ----------------------------------------------**/
/** pads **/
ControllerPacket buffer1, buffer2;

/** display **/
DRAWENV     draw;    							/* drawing environment */
DISPENV     disp;    							/* display environment */
static RECT bg = {0, 0, 640, 480};			/* the background screen area */


/**-------------------------------------------------------------------------**/
main()
{

ResetCallback();	
ResetGraph(0);
SetGraphDebug(0);


FntLoad(640, 0);
FntOpen(10, 16, 500, 200, 0, 800);

SetDefDrawEnv(&draw, 0,   0, 512, 240);
SetDefDispEnv(&disp, 0,   0, 512, 240);

ClearImage(&bg, 200, 0, 200);
SetDispMask(1);

PutDrawEnv(&draw);
PutDispEnv(&disp);   


InitTAP(&buffer1,MAX_CONTROLLER_BYTES,&buffer2,MAX_CONTROLLER_BYTES);
VSync(0);
StartTAP();
VSync(0);


do
{
FntPrint("\n\n");
FntPrint("    developers conference 1996 controller demo program\n");
FntPrint("    **************************************************\n");
FntPrint("\n\n");

/** port 1 **/

FntPrint("    port 1 :");


if(GoodData(&buffer1))
	{
	switch(GetType(&buffer1))		
		{
		case 1:
			FntPrint("mouse");			
			mouse();
			break;
		case 2:
			FntPrint("negcon");			
			negcon();
			break;

		case 4:
			FntPrint("pad");			
			pad();
			break;

		case 5:
			FntPrint("analog");			
			analog();
			break;

		case 8:
			FntPrint("multitap\n");			
			multitap();
			break;

		default:
			FntPrint("undefined controller type");			
			break;	
		}
	}
else
	{
	FntPrint("invalid data packet");
	} 
FntPrint("\n");



display_write(); 
VSync(0);							

}while (1);


}
/**-------------------------------------------------------------------------**/




/**----------------------------------------------------------------------**/
/** clear display **/
/**----------------------------------------------------------------------**/

display_clear()
{
FntFlush(-1);
ClearImage(&bg, 0, 0, 0);
}
/**----------------------------------------------------------------------**/


/**----------------------------------------------------------------------**/
/** update the screen with all the FntPrints since last clear display    **/
/**----------------------------------------------------------------------**/

display_write()
{
/*VSync(0);	*/
ClearImage(&bg, 0, 0, 0);
FntFlush(-1);
DrawSync(0);
}
/**----------------------------------------------------------------------**/




/**----------------------------------------------------------------------**/
/** the mouse                                                            **/
/**----------------------------------------------------------------------**/
mouse()
{
FntPrint("    \n");
FntPrint("    mouse test routine\n");
FntPrint("    ******************\n");
FntPrint("    controller1 0x%x\n\n",buffer1);

FntPrint("   x off :%d\n",MouseXOffset(&buffer1));
FntPrint("   y off :%d\n",MouseYOffset(&buffer1));

FntPrint("    ml :");
if (MouseKeyIsPressed(&buffer1,MOUSE_LEFT)!=0) 
	FntPrint("1");
else
	FntPrint("0");

FntPrint("    mr :");
if (MouseKeyIsPressed(&buffer1,MOUSE_RIGHT)!=0) 
	FntPrint("1");
else
	FntPrint("0");




}



/**----------------------------------------------------------------------**/
/** the negcon                                                           **/
/**----------------------------------------------------------------------**/
negcon()
{
FntPrint("    \n");
FntPrint("    negcon test routine\n");
FntPrint("    *******************\n");

FntPrint("    controller1 0x%x\n\n",buffer1);

FntPrint("    twist :%d\n",NegconTwist(&buffer1)); 
FntPrint("    tl    :%d\n",NegconTopLeft(&buffer1));   
FntPrint("    i     :%d\n",NegconI(&buffer1));         
FntPrint("    ii    :%d\n",NegconII(&buffer1));        


/*left dpad*/
FntPrint("    \n");

FntPrint("    ld :");
if (PadKeyIsPressed(&buffer1,NEGCON_DOWN)!=0) 
	FntPrint("1");
else
	FntPrint("0");

FntPrint("    lu :");
if (PadKeyIsPressed(&buffer1,NEGCON_UP)!=0) 
	FntPrint("1");
else
	FntPrint("0");
			

FntPrint("    lr :");
if (PadKeyIsPressed(&buffer1,NEGCON_RIGHT)!=0) 
		FntPrint("1");
else
		FntPrint("0");

FntPrint("    ll :");
if (PadKeyIsPressed(&buffer1,NEGCON_LEFT)!=0) 
		FntPrint("1");
else
		FntPrint ("0");

/*special keys*/
FntPrint("    \n");

FntPrint("    tr:");
if (PadKeyIsPressed(&buffer1,NEGCON_TR)!=0) 
	FntPrint("1");
else
	FntPrint("0");

FntPrint("  start:");
if (PadKeyIsPressed(&buffer1,NEGCON_START)!=0) 
	FntPrint("1");
else
	FntPrint("0");

}
/**----------------------------------------------------------------------**/







/**----------------------------------------------------------------------**/
/** the pad                                                              **/
/**----------------------------------------------------------------------**/
pad()
{
FntPrint("    \n");
FntPrint("    Pad test routine\n");
FntPrint("    ****************\n");

FntPrint("    controller1 0x%x\n\n",buffer1);

/*right dpad*/
FntPrint("    \n");

FntPrint("    RD :");
if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
	FntPrint("1");
else
	FntPrint("0");

FntPrint("    RU :");
if (PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
	FntPrint("1");
else
	FntPrint("0");
			

FntPrint("    RR :");
if (PadKeyIsPressed(&buffer1,PAD_RR)!=0) 
		FntPrint("1");
else
		FntPrint("0");

FntPrint("    RL :");
if (PadKeyIsPressed(&buffer1,PAD_RL)!=0) 
		FntPrint("1");
else
		FntPrint("0");



/*left dpad*/
FntPrint("    \n");

FntPrint("    ld :");
if (PadKeyIsPressed(&buffer1,PAD_LD)!=0) 
	FntPrint("1");
else
	FntPrint("0");

FntPrint("    lu :");
if (PadKeyIsPressed(&buffer1,PAD_LU)!=0) 
	FntPrint("1");
else
	FntPrint("0");
			

FntPrint("    lr :");
if (PadKeyIsPressed(&buffer1,PAD_LR)!=0) 
		FntPrint("1");
else
		FntPrint("0");

FntPrint("    ll :");
if (PadKeyIsPressed(&buffer1,PAD_LL)!=0) 
		FntPrint("1");
else
		FntPrint ("0");

/*shoulder pads*/
FntPrint("    \n");

FntPrint("    frb:");
if (PadKeyIsPressed(&buffer1,PAD_FRB)!=0) 
	FntPrint("1");
else
	FntPrint("0");

FntPrint("    frt:");
if (PadKeyIsPressed(&buffer1,PAD_FRT)!=0) 
	FntPrint("1");
else
	FntPrint("0");
			

FntPrint("    flt:");
if (PadKeyIsPressed(&buffer1,PAD_FLT)!=0) 
		FntPrint("1");
else
		FntPrint("0");

FntPrint("    flb:");
if (PadKeyIsPressed(&buffer1,PAD_FLB)!=0) 
		FntPrint("1");
else
		FntPrint ("0");




/*special keys*/
FntPrint("    \n");

FntPrint("    sel:");
if (PadKeyIsPressed(&buffer1,PAD_SEL)!=0) 
	FntPrint("1");
else
	FntPrint("0");

FntPrint("  start:");
if (PadKeyIsPressed(&buffer1,PAD_START)!=0) 
	FntPrint("1");
else
	FntPrint("0");

}
/**----------------------------------------------------------------------**/






/**----------------------------------------------------------------------**/
/** the analog                                                           **/
/**----------------------------------------------------------------------**/
analog()
{
FntPrint("    \n");
FntPrint("    analog test routine\n");
FntPrint("    *******************\n");
FntPrint("    controller1 0x%x\n\n",buffer1);


FntPrint("    i     :%d\n",JoystickRightX(&buffer1));         
FntPrint("    i     :%d\n",JoystickRightY(&buffer1));         

FntPrint("    tl    :%d\n",JoystickLeftX(&buffer1));   
FntPrint("    i     :%d\n",JoystickLeftY(&buffer1));         

FntPrint("    tl    :%d\n",JoystickLeftX(&buffer1));   
FntPrint("    i     :%d\n",JoystickLeftY(&buffer1));         


/** ok, this isn't finished but you get the general picture **/	







}
/**----------------------------------------------------------------------**/


















/**----------------------------------------------------------------------**/
/** the multitap                                                         **/
/**----------------------------------------------------------------------**/
multitap()	
{
unsigned char ctype;
TapCtrllerData* con;


FntPrint("    ");
/** port a **/
con =GetTapData(&buffer1,0);
ctype =GetType(con); 
switch(ctype)
	{
	case 1: 	FntPrint("mouse   ");
				break; 


	case 2: 	FntPrint("analog  ");				
				break;

  	case 4: 	FntPrint("pad     ");	
				break;

  	case 8: 	FntPrint("mtap    "); 		
 				break; 

	default: FntPrint("Invalid "); 		
					break;
		}

/** port b **/
con =GetTapData(&buffer1,1);
ctype =GetType(con); 
switch(ctype)
	{
	case 1: 	FntPrint("mouse   ");
				break; 


	case 2: 	FntPrint("analog  ");				
				break;

  	case 4: 	FntPrint("pad     ");	
				break;

  	case 8: 	FntPrint("mtap    "); 		
 				break; 

	default: FntPrint("Invalid "); 		
					break;
		}


/** port c **/
con =GetTapData(&buffer1,2);
ctype =GetType(con); 
switch(ctype)
	{
	case 1: 	FntPrint("mouse   ");
				break; 


	case 2: 	FntPrint("analog  ");				
				break;

  	case 4: 	FntPrint("pad     ");	
				break;

  	case 8: 	FntPrint("mtap    "); 		
 				break; 

	default: FntPrint("Invalid "); 		
					break;
		}


/** port d **/
con =GetTapData(&buffer1,3);
ctype =GetType(con); 
switch(ctype)
	{
	case 1: 	FntPrint("mouse   ");
				break; 

	case 2: 	FntPrint("negcon  ");				
				break;

  	case 4: 	FntPrint("pad     ");	
				break;

	case 5: 	FntPrint("analog  ");				
				break;

  	case 8: 	FntPrint("mtap    "); 		
 				break; 

	default: FntPrint("Invalid "); 		
					break;
		}

FntPrint("\n");



}

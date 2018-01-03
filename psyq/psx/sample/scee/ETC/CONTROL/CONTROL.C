/*****************************************************************
 *		Lib Rev 4.0												 *
 *																 *
 *              Filename:               control.c                                                                   *
 *																 *
 *		Author:		    Kevin Thompson						   	 *													
 *																 *
 *		History:												 *
 *			30-05-97	(LPGE)									 *
 *						Created									 *
 *																 *
 *	    Copyright (c) 1997 Sony Computer Entertainment Europe    *
 *		  All Rights Reserved									 *
 *																 *
 *****************************************************************/	

#include <sys/types.h>
#include <kernel.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctrller.h"

#define SRC_Z     1024
#define MAXSTARS  501
#define OTSIZE    1002    /* 1002 */

#define GUN_X_DIVISOR           (3250)  // 5000 = 320 pixels
                                        // 3250 = 512 pixels
                                        // 2500 = 640 pixels

#define GUN_Y_MULTILIER         (1)     // 1 = 240
                                        // 2 = 480


#define TEX_pad       ((u_long *)0x80050000)
#define TEX_negcon    ((u_long *)0x80070000)
#define TEX_mouse     ((u_long *)0x80090000)
#define TEX_gun       ((u_long *)0x80110000)
#define TEX_no_pad    ((u_long *)0x80130000)
#define TEX_anolog    ((u_long *)0x80150000)
#define TEX_anCont    ((u_long *)0x80170000)

int pad_left_port;

typedef struct {
        u_long          ot[OTSIZE];
        POLY_FT4        background;
        LINE_F2         line1[MAXSTARS];
        DRAWENV         draw;
        DISPENV         disp;
} DB;


typedef struct {                        // to store the gun's position
        char    v_count;
        char    h_count;
} Gun_position;

typedef struct  {                       // set the gun buffer
        char             gun_status;
        char             gun_count;
        Gun_position     gun[20];       //X/Y light gun values;
} Gun_buffer;


/* globals  */

ControllerPacket buffer1,buffer2;
Gun_buffer      gun;


int pic=0,f=10,vertical,horizontal;
int init_only_once=0,pmx=0,pmy=0;
int time,no_pad=0,buffer=0,speed = 15,rotation = 0,rotation2=0;
int nx,ny,b=0,viewX=256,viewY=120;
int mouseX=450,mouseY=160,pressed;
int c,a,s,g;
int lastPad,ret;
int xpos,ypos,pos;

long dmy,flg;
LINE_F2    line;
int x=160,y=160;

int screener=0;

char pad0[34],pad1[34];
char pad0V[34],pad1V[34];
int starshake[MAXSTARS];

void init_stars(void);
void init_prim(DB *b);
void loadTIM(u_long *addr);

int ranzfar[MAXSTARS];
int ranznear[MAXSTARS];

static VECTOR    vec[MAXSTARS] = {0,0,0};
static SVECTOR   ang[MAXSTARS] = {0,0,0};
static MATRIX    m;

main()
{
        DB      db[2];
        DB      *cdb;
        char *port1;
        char *port2;

        ResetCallback();               /** reset the callback **/

    	ResetGraph(0);			/* reset graphics sysmtem (0:cold,1:warm) */	
        SetGraphDebug(1);               /* set debug mode (0:off,1:monitor,2:dump) */

	/* load and initialize fonts */
        FntLoad(512, 256);   
        SetDumpFnt(FntOpen(70, 36, 500, 200, 0, 800));
                                 
        InitGeom();

        SetGeomOffset(256,120);

        SetGeomScreen(256);

	/* set up drawing and display environments */
        /* 512,240, */

        SetDefDrawEnv(&db[0].draw, 0, 0, 512, 240);    
        SetDefDrawEnv(&db[1].draw, 0, 240, 512, 240);

        SetDefDispEnv(&db[0].disp, 0, 240, 512, 240);
        SetDefDispEnv(&db[1].disp, 0, 0, 512, 240);

        SetBackColor(0,0,0);
        SetFarColor(0,0,0);

    for(c=0; c<MAXSTARS; c++)           /* init the stars. */
    {                        /* 1022  511\n  700, 239*/
        vec[c].vx = ( rand() %2000 ) - 1000;
        vec[c].vy = ( rand() %4000 ) - 2000;
        vec[c].vz = ( rand() %15000 ) + 2000;
        ranzfar[c] = vec[c].vz;
        ranznear[c] = ( rand() %500 ) -300;
        starshake[c] = ( rand() %5 ) - 2;
    }
        loadTIM(TEX_no_pad);
        pic = 6;
        init_prim(&db[0]); 
        init_prim(&db[1]);

        SetDispMask(1);
        printf(" prog don't work without this printf\n\n");
	/* set bg color */
        /*draw.isbg = 1;
        setRGB0(&draw, 60, 120, 120);*/

	/* update display environment */
        PutDrawEnv(&db[0].draw); 
		
	/* update drawing environment */
        PutDispEnv(&db[0].disp); 

	/* enable display */
	SetDispMask(1);

	/* set the clear area */
        /*setRECT(&area,0,0,640,480);*/

InitTAP(&buffer1,MAX_CONTROLLER_BYTES,&buffer2,MAX_CONTROLLER_BYTES);
StartTAP ();

	do
	{
        cdb = (cdb==db)?db+1 : db;

        port1 = (char *)&buffer1;
        port2 = (char *)&buffer2;


         FntPrint(" buffer1 0x%x\n ",buffer1);


/*--------------PRINT OUT THE BIT STATUS OF THE PERIPHERALS -------------*/

/*------------- print the status of port 1-------------------------------*/
        
        //printf("\rport1 OK %2.2X TYPE %2.2X Codes ",port1[0],port1[1] );

        //for (c=0;c<((port1[1]&15)<<1);c++) printf("%2.2X ",port1[c+2]);

        //printf("                 ");

        if(GoodData(&buffer1))
   		{

        switch(GetType(&buffer1))
  		    {
                        case 1:
                                
      // if last pad != 0, pad spec has been changed without removing pad form Controller port

                            if(screener == 0) 
                            {
                                loadTIM(TEX_mouse);
                                pic = 1;
                                init_prim(&db[0]);
                                init_prim(&db[1]);
                                screener = 1;
                                no_pad = 0;
                            }
                            check_mouse();
  		            break;
  		            
                        case 2:

                            if(screener == 0)
                            {
                                loadTIM(TEX_negcon);
                                pic = 2;
                                init_prim(&db[0]);
                                init_prim(&db[1]);
                                screener = 1;
                                no_pad = 0;
                            }

                            Check_NeGcon();
  		            break;      		        

                        case 3:

                         /*   if(screener == 0)
                            {
                                loadTIM(TEX_gun);
                                pic = 5;
                                init_prim(&db[0]);
                                init_prim(&db[1]);
                                screener = 1;
                                no_pad = 0;
                            }

                            check_gun(); */
                            break;


                        case 4:

                            if(screener == 0)
                            {
                                loadTIM(TEX_pad);
                                pic = 3;

                                init_prim(&db[0]);
                                init_prim(&db[1]);
                                screener  = 1;
                                no_pad = 0;
                            }
                            check_pad();
                            break;


                        case 5:

                            if(screener == 0)
                            {
                                loadTIM(TEX_anCont);
                                pic = 4;
                                init_prim(&db[0]);
                                init_prim(&db[1]);
                                screener = 1;
                                no_pad = 0;
                            }

                            check_anolog();
                            break;


                        case 7:

                            if(screener == 0)
                            {
                                loadTIM(TEX_anolog);
                                pic = 7;
                                init_prim(&db[0]);
                                init_prim(&db[1]);
                                screener = 1;
                                no_pad = 0;
                            }

                            check_analog_controller();
                            break;


                        case 8:
                            check_multitap();
                            break;                       
                        }

                if(lastPad != GetType(&buffer1));
                    {
                         screener = 0;
                    }

                lastPad = GetType(&buffer1);

                }
  		
                else            /** if bad packet/no pad **/
                {
                      if(no_pad == 0)
                      {

                      pad_left_port++;

                          loadTIM(TEX_no_pad);
                          pic = 6;
                          init_prim(&db[0]);
                          init_prim(&db[1]);
                          screener = 0;
                          no_pad = 1;
                          viewX = 256;
                          viewY = 120;
                          speed = 30;
                          rotation = 0;
                      }
                }

             ClearOTag(cdb->ot,OTSIZE);

             AddPrim(cdb->ot+1,&cdb->background);

              FntPrint("\n pad_left_port = %d\n",pad_left_port);


              for(c=0; c<MAXSTARS; c = c + 3)
              {

                             if(init_only_once != MAXSTARS)
                             {
                                     SetLineF2(&cdb->line1[c]);
                            
                                     cdb->line1[c].r0 = 250;
                                     cdb->line1[c].g0 = 250;
                                     cdb->line1[c].b0 = 250;

                                     SetLineF2(&cdb->line1[c+1]);
                            
                                     cdb->line1[c+1].r0 = 250;
                                     cdb->line1[c+1].g0 = 250;
                                     cdb->line1[c+1].b0 = 250;

                                     SetLineF2(&cdb->line1[c+2]);
                            
                                     cdb->line1[c+2].r0 = 250;
                                     cdb->line1[c+2].g0 = 250;
                                     cdb->line1[c+2].b0 = 250;
                            
                                     init_only_once++;
                             }

                            SetGeomOffset(viewX,viewY);

                vec[c].vz -= speed;
                ang[c].vz += rotation;

                if(vec[c].vz == 0)
                    {
                        vec[c].vz = 1;
                    }

                if(vec[c].vz <= 10)
                    {
                        vec[c].vz = ranzfar[c];
                    }

                if(vec[c].vz >= 15000)
                    {
                        vec[c].vz = ranznear[c]; 
                    }

                RotMatrix(&ang[c], &m);
                TransMatrix(&m,&vec[c]);
                SetRotMatrix(&m);
                SetTransMatrix(&m);


                TransRotPers3(&vec[c],&vec[c+1],&vec[c+2],
                             &cdb->line1[c].x0,&cdb->line1[c+1].x0,&cdb->line1[c+2].x0,
                             &dmy,
                             &flg);


                cdb->line1[c].x1 = cdb->line1[c].x0;
                cdb->line1[c].y1 = cdb->line1[c].y0;

                cdb->line1[c+1].x1 = cdb->line1[c+1].x0;
                cdb->line1[c+1].y1 = cdb->line1[c+1].y0;

                cdb->line1[c+2].x1 = cdb->line1[c+2].x0;
                cdb->line1[c+2].y1 = cdb->line1[c+2].y0;

               if(cdb->line1[c].x0 > 0 && cdb->line1[c].x0 < 512 && 
                  cdb->line1[c].y0 > 10 && cdb->line1[c].y0 < 240 && pic != 5)
                  {
                       AddPrim(cdb->ot+c+2,&cdb->line1[c]);
                       AddPrim(cdb->ot+c+3,&cdb->line1[c+1]);
                       AddPrim(cdb->ot+c+4,&cdb->line1[c+2]);
                  }

               }

        PutDrawEnv(&cdb->draw);
        PutDispEnv(&cdb->disp);


        DrawOTag(cdb->ot);

        
        pressed = 0;
        c = VSync(0);
        FntPrint(" %d ",c);
        FntFlush(-1);


	}while(1);
	
}	/** end of the main **/
	


void init_prim(DB *b)
{


SetPolyFT4(&b->background);
SetShadeTex(&b->background,1);
setXYWH(&b->background,0,0,512,240);  /* 320, 240) */
setRGB0(&b->background,60,120,120);
setUV4(&b->background ,0,0, 255,0, 0,255 ,255,255);  /* 0,0,255,0,0,255,255,255*/

if(pic == 1)   // mouse 
{
b->background.tpage = getTPage(2,0,512,0);
}

if(pic == 2)   // negcon 
{

b->background.tpage = getTPage(2,0,512,0);
}

if(pic == 3)   // gun 
{
b->background.tpage = getTPage(2,0,512,0);
}

if(pic == 4)   // pad
{
b->background.tpage = getTPage(2,0,512,0);
}

if(pic == 5)  // analog JoyStick
{
b->background.tpage = getTPage(2,0,512,0);
}

if(pic == 6)  // no_periphral 
{
b->background.tpage = getTPage(2,0,768,256);
}

if(pic == 7)  // analog Controller
{
b->background.tpage = getTPage(2,0,512,0);
}


}



void loadTIM(u_long *addr)
{


        TIM_IMAGE       image;

        OpenTIM(addr);
        while(ReadTIM(&image)){
                if(image.caddr){
                    LoadImage(image.crect, image.caddr);
                }


                if(image.paddr) {
                   LoadImage(image.prect, image.paddr);

                }
                
        }
}


check_pad()
{

/* this is to check if the standard Controller is oppertating correctly */


 	if ( PadKeyIsPressed(&buffer1,PAD_SEL)!=0) /* has select been pressed on pad 1 */
	  	{
          FntPrint("\nSelect");
          pressed = 1;
      	}
 
  	if ( PadKeyIsPressed(&buffer1,PAD_RL)!=0)  /* has square been pressed on pad 1 */
	  	{
          FntPrint("\nSquare");
          pressed = 1;
        }
      
   	if ( PadKeyIsPressed(&buffer1,PAD_RR)!=0)  /* has circle been pressed on pad 1 */
	  	{  
          FntPrint ("\nCircle");
          pressed = 1;
        }
      
  	if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0)  /* has Triangle been pressed no pad 1 */
	  	{
          FntPrint ("\nTriangle");
          pressed = 1;
          speed += 2;
        }
      
  	if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) /* has 'X' been pressed no pad 1 */
	  	{
          FntPrint("\nX");
          pressed = 1;
          speed -= 2;
        }
  
  	if ( PadKeyIsPressed(&buffer1,PAD_LU)!=0) /* has Up been pressed no pad 1 */
	  	{
          FntPrint ("\nUp");
          pressed = 1;
         viewY -= 5;
        }
  
  	if ( PadKeyIsPressed(&buffer1,PAD_LD)!=0) /* has Down been pressed no pad 1 */
	  	{
          FntPrint ("\nDown");
          pressed = 1;
          viewY += 5;
        }     
  
  	if ( PadKeyIsPressed(&buffer1,PAD_LL)!=0) /* has Left been pressed no pad 1 */
	  	{
          FntPrint ("\nLeft");
          pressed = 1;
          viewX -= 5;
        }
  
  	if ( PadKeyIsPressed(&buffer1,PAD_LR)!=0) /* has Right been pressed no pad 1 */
	  	{
          FntPrint ("\nright");
          pressed = 1;
          viewX += 5;
        }

  
  	if ( PadKeyIsPressed(&buffer1,PAD_START)!=0) /* has Start been pressed no pad 1 */
	  	{
          FntPrint ("\nStart\n");
          pressed = 1;
      	}
  
  	if ( PadKeyIsPressed(&buffer1,PAD_FRT)!=0) /* has R1 been pressed no pad 1 */
	  	{
          FntPrint ("\nR1");
          pressed = 1;
      	}
  
  	if ( PadKeyIsPressed(&buffer1,PAD_FRB)!=0) /* has R2 been pressed no pad 1 */
	  	{
          FntPrint ("\nR2");
          pressed = 1;
 	    }
  
  	if ( PadKeyIsPressed(&buffer1,PAD_FLT)!=0) /* has L1 been pressed no pad 1 */
	  	{
          FntPrint ("\nL1");
          pressed = 1;
      	}
  
  	if ( PadKeyIsPressed(&buffer1,PAD_FLB)!=0) /* has L2 been pressed no pad 1 */
	  	{
          FntPrint ("\nL2");
          pressed = 1;
      	}

        if(pressed == 1)
        {
            viewX = 256;
            viewY = 120;
        }


}                /** end of the check_pad function  */



check_anolog()
{
int JSRX,JSRY,JSLX,JSLY;

FntPrint("\t  You picked the Anolog Joystick to test ");

//SetLineF2(&line);


JSLX = JoystickLeftX(&buffer1);
JSLY = JoystickLeftY(&buffer1);

JSRX = JoystickRightX(&buffer1);
JSRY = JoystickRightY(&buffer1);

rotation = (JSRX - 127) + (JSLX - 127);
speed = ((JSRY - 127) << 1) + ((JSLY - 127) << 1);


FntPrint("\n\n     leftX    = %d\n",JoystickLeftX(&buffer1));
FntPrint("     leftY    = %d\n",JoystickLeftY(&buffer1));
FntPrint("     RightX   = %d\n",JoystickRightX(&buffer1));
FntPrint("     RightY   = %d\n\n",JoystickRightY(&buffer1));



/** check the joystick in digital mode */

	if ( JoystickKeyIsPressed(&buffer1,LEFT_MAIN_FIRE)!=0) /* has select been pressed on the Joystick */
	  	{
          FntPrint("Left trigger fire or L2\n");
          pressed = 1;
        }
 
  	if ( JoystickKeyIsPressed(&buffer1,RIGHT_MAIN_FIRE)!=0)  /* has square been pressed on the Joystick */
	  	{
          FntPrint("Right trigger fire or Square\n");
          pressed = 1;
        }


   	if ( JoystickKeyIsPressed(&buffer1,PAD_RR)!=0)  /* has circle been pressed on the Joystick */
	  	{  
          FntPrint ("Circle\n");
          pressed = 1;
      	}
      
  	if ( JoystickKeyIsPressed(&buffer1,PAD_RU)!=0)  /* has Triangle been pressed on the Joystick */
	  	{
          FntPrint ("Triangle\n");
          pressed = 1;
      	}
      
  	if ( JoystickKeyIsPressed(&buffer1,PAD_RD)!=0) /* has 'X' been pressed on the Joystick */
	  	{
          FntPrint("X\n");
          pressed = 1;
          pad0V[0] = 1;
          pad0V[1] = 0x73;
          pad0V[3] = 0;

        }
  
  	if ( JoystickKeyIsPressed(&buffer1,PAD_LU)!=0) /* has Up been pressed on the Joystick */
	  	{
          FntPrint ("Up\n");
          pressed = 1;
          viewY -= 5;
        }
  
  	if ( JoystickKeyIsPressed(&buffer1,PAD_LD)!=0) /* has Down been pressed on the Joystick*/
	  	{
          FntPrint ("Down\n");
          pressed = 1;
          viewY += 5;
      	}
  
  	if ( JoystickKeyIsPressed(&buffer1,PAD_LL)!=0) /* has Left been pressed on the Joystick */
	  	{
          FntPrint ("Left\n");
          pressed = 1;
          viewX -= 5;
        }
  
  	if ( JoystickKeyIsPressed(&buffer1,PAD_LR)!=0) /* has Right been pressed on the Joystick */
	  	{
          FntPrint ("Right\n");
          pressed = 1;
          viewX +=5;
        }
  
  	if ( JoystickKeyIsPressed(&buffer1,PAD_START)!=0) /* has Start been pressed on the Joystick */
	  	{
          FntPrint ("Start\n");
          pressed = 1;
      	}
  
  	if ( JoystickKeyIsPressed(&buffer1,PAD_FRT)!=0) /* has R1 been pressed on the Joystick */
	  	{
          FntPrint ("R1\n");
          pressed = 1;
      	}
  
  	if ( JoystickKeyIsPressed(&buffer1,PAD_FRB)!=0) /* has R2 been pressed on the Joystick */
	  	{
          FntPrint ("R2\n");
          pressed = 1;
 	    }
  
  	if ( JoystickKeyIsPressed(&buffer1,PAD_FLT)!=0) /* has L1 been pressed on the Joystick */
	  	{
          FntPrint ("L1\n");
          pressed = 1;
      	} 

        if(pressed == 1)
        {
            viewX = 256;
            viewY = 120;
        }

        
}


Check_NeGcon()
{
  	      	
int TL,TW,i,ii;
      	      	

        if ( PadKeyIsPressed(&buffer1,NEGCON_UP)!=0) /* has up been pressed on the NeGcon */
	  	{
          FntPrint ("Up\n");
          pressed = 1;
        }
    if ( PadKeyIsPressed(&buffer1,NEGCON_DOWN)!=0) /* has up been pressed on the NeGcon */
	  	{
          FntPrint ("Down\n");
          pressed = 1;
        }
    if ( PadKeyIsPressed(&buffer1,NEGCON_LEFT)!=0) /* has up been pressed on the NeGcon */
	  	{
          FntPrint ("Left\n");
          pressed = 1;
        }
    if ( PadKeyIsPressed(&buffer1,NEGCON_RIGHT)!=0) /* has up been pressed on the NeGcon */
	  	{
          FntPrint ("Right\n");
          pressed = 1;
        }
    if ( PadKeyIsPressed(&buffer1,NEGCON_A)!=0) /* has up been pressed on the NeGcon */
	  	{
          FntPrint ("A \n");
          pressed = 1;
      	}
    if ( PadKeyIsPressed(&buffer1,NEGCON_B)!=0) /* has up been pressed on the NeGcon */
	  	{
          FntPrint ("B \n");
          pressed = 1;
        }
    if ( PadKeyIsPressed(&buffer1,NEGCON_START)!=0) /* has up been pressed on the NeGcon */
	  	{
          FntPrint ("Start \n");
          pressed = 1;
      	}  	
    if ( PadKeyIsPressed(&buffer1,NEGCON_TR)!=0) /* has up been pressed on the NeGcon */
	  	{
          FntPrint ("Top Right \n");
          pressed = 1;
        }

       if(pressed == 1)
       {
           viewX = 256;
           viewY = 120;
       }

TL = NegconTopLeft(&buffer1);                                 /** divide the result by 2**/
TW = NegconTwist(&buffer1);
i =  NegconI(&buffer1);
ii = NegconII(&buffer1);   



FntPrint("  Top Left %d \n",TL);
FntPrint("  Twist    %d \n",TW);
FntPrint("  I        %d \n",i);
FntPrint("  II       %d \n\n\n",ii);

        speed =+ TL + i + ii+ 15;
        rotation = (TW - 127);
}


check_mouse()
{
/** this is the function to test the mouse **/

int mx,my;

FntPrint("\t  You picked the mouse to test ");

FntPrint("\n   x  :%d\n",MouseXOffset(&buffer1));
FntPrint("\n   y  :%d\n",MouseYOffset(&buffer1));


mx = MouseXOffset(&buffer1);
my = MouseYOffset(&buffer1);

        viewX += mx;
        viewY += my;

if (MouseKeyIsPressed(&buffer1,MOUSE_LEFT)!=0) 
 	{
            speed += 5;
            pressed = 1;
        }

if (MouseKeyIsPressed(&buffer1,MOUSE_RIGHT)!=0)
        { 
            speed -= 5;
            pressed = 1;
        }

        if(pressed == 1)
        {
            viewX = 256;
            viewY = 120;
        }

}

check_multitap()
{
unsigned char ctype;
TapCtrllerData* con;

FntPrint("\nMtap 0 0x%x ",buffer1.data.tap.ctrllers[0]);
FntPrint("\nMtap 1 0x%x ",buffer1.data.tap.ctrllers[1]);
FntPrint("\nMtap 2 0x%x ",buffer1.data.tap.ctrllers[2]);
FntPrint("\nMtap 3 0x%x ",buffer1.data.tap.ctrllers[3]);
}


check_analog_controller()
{
int JSRX,JSRY,JSLX,JSLY;

//pressed = 0;    // to regester if one of the buttons has been pressed

FntPrint("\t  You picked the Analog Controller to test");

rotation = (JoystickRightX(&buffer1) - 127) + (JoystickLeftX(&buffer1) - 127);
speed = ((JoystickRightY(&buffer1) - 127) << 1) + ((JoystickLeftY(&buffer1) - 127) << 1);

FntPrint("\n\n     leftX    = %d\n",JoystickLeftX(&buffer1));
FntPrint("     leftY    = %d\n",JoystickLeftY(&buffer1));
FntPrint("     RightX   = %d\n",JoystickRightX(&buffer1));
FntPrint("     RightY   = %d\n\n",JoystickRightY(&buffer1));


// check the joystick in digital mode 


        if ( JoystickKeyIsPressed(&buffer1,LEFT_MAIN_FIRE)!=0)  // has select been pressed on the Joystick 
	  	{
          FntPrint("L2\n");
        }
 
        if ( JoystickKeyIsPressed(&buffer1,RIGHT_MAIN_FIRE)!=0) // has square been pressed on the Joystick 
	  	{
          FntPrint("Square\n");  
        }

        if ( JoystickKeyIsPressed(&buffer1,PAD_RR)!=0)  // has circle been pressed on the Joystick 
	  	{  
          FntPrint ("Circle\n");
        }
      
        if ( JoystickKeyIsPressed(&buffer1,PAD_RU)!=0)  // has Triangle been pressed on the Joystick 
	  	{
          FntPrint ("\nTriangle\n");
        }

        if ( JoystickKeyIsPressed(&buffer1,PAD_RD)!=0) // has 'X' been pressed on the Joystick
        {
          FntPrint("\nX\n");
        }

        if ( JoystickKeyIsPressed(&buffer1,PAD_LU)!=0) // has Up been pressed on the Joystick 
	  	{
          FntPrint("Up\n");
        }
  
        if ( JoystickKeyIsPressed(&buffer1,PAD_LD)!=0) // has Down been pressed on the Joystick*/
	  	{
          FntPrint ("Down\n");
        }
  
        if ( JoystickKeyIsPressed(&buffer1,PAD_LL)!=0) // has Left been pressed on the Joystick */
	  	{
          FntPrint ("Left\n");
        }
  
        if ( JoystickKeyIsPressed(&buffer1,PAD_LR)!=0) // has Right been pressed on the Joystick */
	  	{
          FntPrint ("Right\n");
        }
  
        if ( JoystickKeyIsPressed(&buffer1,PAD_START)!=0) // has Start been pressed on the Joystick */
	  	{
          FntPrint ("Start\n");
        }
  
        if ( JoystickKeyIsPressed(&buffer1,PAD_FRT)!=0) // has R1 been pressed on the Joystick */
	  	{
          FntPrint ("R1\n");
        }
  
        if ( JoystickKeyIsPressed(&buffer1,PAD_FRB)!=0) // has R2 been pressed on the Joystick */
	  	{
          FntPrint ("R2\n");
        }
  
        if ( JoystickKeyIsPressed(&buffer1,PAD_FLT)!=0) // has L1 been pressed on the Joystick */
	  	{
          FntPrint ("L1\n");
        }

        if(JoystickKeyIsPressed(&buffer1,0x0002)!=0)
        {
        FntPrint("\n Left Analog Button\n");
        }

        if(JoystickKeyIsPressed(&buffer1,0x0004)!=0)
        {
        FntPrint("\n Right Analog Button\n");
        }

       if(s == MAXSTARS)
           {
               s = 0;
           }
       s++;
}



check_pad2()
{

/* this is to check if the standard Controller is oppertating correctly */


        if ( PadKeyIsPressed(&buffer2,PAD_SEL)!=0) /* has select been pressed on pad 1 */
	  	{
          FntPrint("\nSelect");
          pressed = 1;
      	}
 
        if ( PadKeyIsPressed(&buffer2,PAD_RL)!=0)  /* has square been pressed on pad 1 */
	  	{
          FntPrint("\nSquare");
          pressed = 1;
        }
      
        if ( PadKeyIsPressed(&buffer2,PAD_RR)!=0)  /* has circle been pressed on pad 1 */
	  	{  
          FntPrint ("\nCircle");
          pressed = 1;
        }
      
        if ( PadKeyIsPressed(&buffer2,PAD_RU)!=0)  /* has Triangle been pressed no pad 1 */
	  	{
          FntPrint ("\nTriangle");
          pressed = 1;
          speed += 2;
        }
      
        if ( PadKeyIsPressed(&buffer2,PAD_RD)!=0) /* has 'X' been pressed no pad 1 */
	  	{
          FntPrint("\nX");
          pressed = 1;
          speed -= 2;
        }
  
        if ( PadKeyIsPressed(&buffer2,PAD_LU)!=0) /* has Up been pressed no pad 1 */
	  	{
          FntPrint ("\nUp");
          pressed = 1;
         viewY -= 5;
        }
  
        if ( PadKeyIsPressed(&buffer2,PAD_LD)!=0) /* has Down been pressed no pad 1 */
	  	{
          FntPrint ("\nDown");
          pressed = 1;
          viewY += 5;
        }     
  
        if ( PadKeyIsPressed(&buffer2,PAD_LL)!=0) /* has Left been pressed no pad 1 */
	  	{
          FntPrint ("\nLeft");
          pressed = 1;
          viewX -= 5;
        }
  
        if ( PadKeyIsPressed(&buffer2,PAD_LR)!=0) /* has Right been pressed no pad 1 */
	  	{
          FntPrint ("\nright");
          pressed = 1;
          viewX += 5;
        }

  
        if ( PadKeyIsPressed(&buffer2,PAD_START)!=0) /* has Start been pressed no pad 1 */
	  	{
          FntPrint ("\nStart\n");
          pressed = 1;
      	}
  
        if ( PadKeyIsPressed(&buffer2,PAD_FRT)!=0) /* has R1 been pressed no pad 1 */
	  	{
          FntPrint ("\nR1");
          pressed = 1;
      	}
  
        if ( PadKeyIsPressed(&buffer2,PAD_FRB)!=0) /* has R2 been pressed no pad 1 */
	  	{
          FntPrint ("\nR2");
          pressed = 1;
 	    }
  
        if ( PadKeyIsPressed(&buffer2,PAD_FLT)!=0) /* has L1 been pressed no pad 1 */
	  	{
          FntPrint ("\nL1");
          pressed = 1;
      	}
  
        if ( PadKeyIsPressed(&buffer2,PAD_FLB)!=0) /* has L2 been pressed no pad 1 */
	  	{
          FntPrint ("\nL2");
          pressed = 1;
      	}

        if(pressed == 1)
        {
            viewX = 256;
            viewY = 120;
        }


}                /** end of the check_pad function  */


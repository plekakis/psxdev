/*****************************************************************************/
/*****************************************************************************/
/** mainmenu.c ** for new memory card project thingy **   	    				 **/
/*****************************************************************************/
/*****************************************************************************/

/** 17.5.95 **/

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
#include "card.h"
#include "ctrller.h"
/**-------------------------------------------------------------------------**/


extern ControllerPacket buffer1, buffer2;



extern DB	   db[2];		      /* packet double buffer    */
extern DB*     cdb;			      /* current db              */

extern int  timbuff[];
extern int  backtimbuff[];




typedef struct
{
int xpos;
int ypos;
int bright;
}ICON_INFO;

ICON_INFO icon_info[10];



int order_list[10];
int bright_list[] = { 0,0,0,10,50,127,50,10,0,0};


int game_sprt_x[] = {  0,  0,  0,  0,  0,125,125,125,125,125};
int game_sprt_y[] = {  0, 50,100,150,200,  0, 50,100,150,200};


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/

menu_test()
{
int isok=-15;
int old_icon_selected=0;
int icon_selected=0;
int i;
int draw=1;
int save_status;


set_gameicons();

//bitmap_cd(POS_MSICFILENAME,1);
//bitmap_cd(POS_FIRSTBCKFILENAME,2);


//bitmap_cd(POS_MAINBACKFILENAME,2);   
fade_out_menu(icon_selected,1);
 

do
{
cdb = (cdb==db)? db+1: db;			
ClearOTag(cdb->ot, OTSIZE);		


if (isok>20) 
   {
   switch(icon_selected)
      {
      case 0:
         /*krazy ivan      */
         printf("KrayIvan (screen change??)\n");
         anim(POS_TEKKEN,302);
//         anim(POS_KRAZY,242);
         break;

      case 1:
         /*jumping flash   */
         anim(POS_TEKKEN,302);
//          anim(POS_JFLASH,336);
         break;

      case 2:
         /*tekken          */
         anim(POS_TEKKEN,302);
//          anim(POS_TEKKEN,302);
         break;

      case 3:
         /*ace combat      */
         anim(POS_TEKKEN,302);
//          anim(POS_ACOMBAT,231);

         break;

      case 4:
         /*disk world      */
          anim(POS_TEKKEN,302);
//         anim(POS_DWORLD,267);
         break;

      case 5:
         /*dderby          */
         anim(POS_TEKKEN,302);
//          anim(POS_DDERBY,250);
         break;

      case 6:
         /*wipeout         */
         anim(POS_TEKKEN,302);
//          anim(POS_WIPEOUT,238);
         break;

      case 7:
         /*kileak          */
          anim(POS_TEKKEN,302);
//         anim(POS_KBLOOD,300);
         break;

      case 8:
         /* ridgeracer     */
          anim(POS_TEKKEN,302);
//         anim(POS_RRACER,283);
         break;

      case 9:              
         /* lemmings       */
          anim(POS_TEKKEN,302);
//         anim(POS_LEMM3D,250);
         break;


      default:
         break;

      }


   }

if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD && isok>0 ) 
   {
   if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
	   {  /*quit*/
      isok=-15;
      bleep(4);      
      fade_out_menu(icon_selected,0);
      return 1;
      }   
  
   if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
	   {  /*copy a file*/
      bleep(4);      
      save_graphic(-1,icon_selected);

      save_status = dave_copy(icon_selected);
      if (save_status==1)
         {
         gpower_line(2);

         //power_line(2);
         bitmap_cd(POS_GAICFILENAME,1);      /*new!*/
         bitmap_cd(POS_MSICFILENAME,1);
         bitmap_cd(POS_MAINBACKFILENAME,2);   

         }

      bitmap_cd(POS_GAICFILENAME,1);   

      save_graphic(1,icon_selected);


    // fade_out_menu(icon_selected,1);


      draw=0;
      isok=-15;
      }   
 

   if ( PadKeyIsPressed(&buffer1,PAD_LD)!=0 && isok>0) 
	   {  
      bleep(1);
      isok=-15;
      icon_selected--;
      if(icon_selected<0)
         {
         icon_selected=9;  /*there are 10 items */   
         }
      move_menu(-1,icon_selected);
      }   
  
   if ( PadKeyIsPressed(&buffer1,PAD_LU)!=0 && isok>0) 
	   {  
      bleep(1);
      isok=-15;
      icon_selected++;
      if(icon_selected>9)
         {
         icon_selected=0;  /*there are 10 items */   
         }
      move_menu(1,icon_selected);
      }   
   }   



if (draw==1)
{

old_icon_selected=icon_selected;

create_order(icon_selected);

cdb->main_sprt.u0 = game_sprt_x[icon_selected];          cdb->main_sprt.v0 = game_sprt_y[icon_selected];
cdb->main_sprt.u1 = game_sprt_x[icon_selected]+125;	   cdb->main_sprt.v1 = game_sprt_y[icon_selected];
cdb->main_sprt.u2 = game_sprt_x[icon_selected]; 	      cdb->main_sprt.v2 = game_sprt_y[icon_selected]+50;
cdb->main_sprt.u3 = game_sprt_x[icon_selected]+125;	   cdb->main_sprt.v3 = game_sprt_y[icon_selected]+50;

cdb->main_sprt.r0 = 128;
cdb->main_sprt.g0 = 128;
cdb->main_sprt.b0 = 128;

AddPrim(cdb->ot+3,&cdb->main_sprt); 


for(i=0; i<10; i++)
   {                            
   cdb->game_icon[order_list[i]].r0=bright_list[i];
   cdb->game_icon[order_list[i]].g0=bright_list[i];
   cdb->game_icon[order_list[i]].b0=bright_list[i];

   cdb->game_icon_mask[order_list[i]].r0=bright_list[i]*2; 
   cdb->game_icon_mask[order_list[i]].g0=bright_list[i]*2; 
   cdb->game_icon_mask[order_list[i]].b0=bright_list[i]*2; 
  
   cdb->game_icon[order_list[i]].x0 = icon_info[i].xpos; 	  cdb->game_icon[order_list[i]].y0 =icon_info[i].ypos;	 
   cdb->game_icon[order_list[i]].x1 = icon_info[i].xpos+124;  cdb->game_icon[order_list[i]].y1 =icon_info[i].ypos; 
   cdb->game_icon[order_list[i]].x2 = icon_info[i].xpos;	  	  cdb->game_icon[order_list[i]].y2 =icon_info[i].ypos+50;	 
   cdb->game_icon[order_list[i]].x3 = icon_info[i].xpos+124;  cdb->game_icon[order_list[i]].y3 =icon_info[i].ypos+50;
   
   cdb->game_icon_mask[order_list[i]].x0 = icon_info[i].xpos; 	       cdb->game_icon_mask[order_list[i]].y0 =icon_info[i].ypos;	 
   cdb->game_icon_mask[order_list[i]].x1 = icon_info[i].xpos+124;     cdb->game_icon_mask[order_list[i]].y1 =icon_info[i].ypos;   
   cdb->game_icon_mask[order_list[i]].x2 = icon_info[i].xpos;	  	    cdb->game_icon_mask[order_list[i]].y2 =icon_info[i].ypos+50;	 
   cdb->game_icon_mask[order_list[i]].x3 = icon_info[i].xpos+124;     cdb->game_icon_mask[order_list[i]].y3 =icon_info[i].ypos+50;
   
   AddPrim(cdb->ot+3,&cdb->game_icon[order_list[i]]); /* draw the game icon */

   SetDrawMode(&cdb->game_icon_mask_mode[order_list[i]],0,0,GetTPage(2,2,768,256),0); 
   AddPrim(cdb->ot+2,&cdb->game_icon_mask[order_list[i]]); 	
   AddPrim(cdb->ot+2,&cdb->game_icon_mask_mode[order_list[i]]); 
   }
}


draw=1;

isok++;
DrawSync(0);						
VSync(0);							
PutDrawEnv(&cdb->draw); 		
PutDispEnv(&cdb->disp); 		
draw_backdrop( (unsigned long*)&backtimbuff[0]); 
DrawOTag(cdb->ot);	   
}while(1);


}












/**-------------------------------------------------------------------------**/
/**-- draw_gameicon(int icon,int xpos,int ypos,int bright)                --**/
/**-------------------------------------------------------------------------**/
/*
draw_gameicon(int icon,int xpos,int ypos,int bright)
{     

cdb->game_icon[icon].x0 = xpos; 	      cdb->game_icon[icon].y0 =ypos;	 
cdb->game_icon[icon].x1 = xpos+124;    cdb->game_icon[icon].y1 =ypos; 
cdb->game_icon[icon].x2 = xpos;	  	   cdb->game_icon[icon].y2 =ypos+50;	 
cdb->game_icon[icon].x3 = xpos+124;    cdb->game_icon[icon].y3 =ypos+50;

cdb->game_icon[icon].r0 = bright;
cdb->game_icon[icon].g0 = bright;
cdb->game_icon[icon].b0 = bright;

AddPrim(cdb->ot+3,&cdb->game_icon[icon]); 

cdb->game_icon_mask[icon].x0 = xpos; 	      cdb->game_icon_mask[icon].y0 =ypos;	 
cdb->game_icon_mask[icon].x1 = xpos+124;     cdb->game_icon_mask[icon].y1 =ypos; 
cdb->game_icon_mask[icon].x2 = xpos;	  	   cdb->game_icon_mask[icon].y2 =ypos+50;	 
cdb->game_icon_mask[icon].x3 = xpos+124;     cdb->game_icon_mask[icon].y3 =ypos+50;

cdb->game_icon_mask[icon].r0 = bright*2;
cdb->game_icon_mask[icon].g0 = bright*2;
cdb->game_icon_mask[icon].b0 = bright*2;

SetDrawMode(&cdb->game_icon_mask_mode[icon],0,0,GetTPage(2,2,768,256),0); 
AddPrim(cdb->ot+2,&cdb->game_icon_mask[icon]); 	
AddPrim(cdb->ot+2,&cdb->game_icon_mask_mode[icon]); 




}
*/




move_menu(int dir,int selected_icon)
{
/*dir<0 up */
/*dir>0 down */
int epos;      /*endpos*/
int spos;      /*startpos*/
int steps;     /*steps*/

int step;      /* used by sinlear interpolation function */
int step2;      /* used by linear interpolation function */

int i;
int icon;
int adder=0;
int old_adder=0;
int shit;


int temp_ypos[10];

int bright_start[10];
int bright_end[10];
int bright;

int main_sprt_bright_start;
int main_sprt_bright_end;

int main_sprt_bright;

int game_sprt_offset;


for(icon=0; icon<10; icon++)
   {
   temp_ypos[icon] = cdb->game_icon[icon].y0; 	 
   }


if (dir<0)
   {
   spos  =  0;
   epos  =  60;
   steps =  10;


   for(icon=0; icon<10; icon++)
      {
      bright_start[order_list[icon]] = bright_list[icon];
      bright_end[order_list[icon]] = bright_list[icon+1];
      }
   }
else
if (dir>0)
   {
   spos  =  1;
   epos  =-60;
   steps = 10;

   for(icon=0; icon<10; icon++)
      {
      bright_start[order_list[icon]] = bright_list[icon];
      bright_end[order_list[icon]] = bright_list[icon-1];
      }
   }

step = sin_calc_stepsize(steps);
step2 = vince_calc_stepsize(steps);


for(i=1; i<=steps; i++)
   {
  	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

   adder =sin_inter_point(spos,epos,step*i);
   shit = adder-old_adder;
   old_adder = adder;

   main_sprt_bright_start=128;
   main_sprt_bright_end=0;
  
   main_sprt_bright = vince_inter_point(main_sprt_bright_start,main_sprt_bright_end,step2*i);

   cdb->main_sprt.u0 = game_sprt_x[selected_icon];          cdb->main_sprt.v0 = game_sprt_y[selected_icon];
   cdb->main_sprt.u1 = game_sprt_x[selected_icon]+125;	   cdb->main_sprt.v1 = game_sprt_y[selected_icon];
   cdb->main_sprt.u2 = game_sprt_x[selected_icon]; 	      cdb->main_sprt.v2 = game_sprt_y[selected_icon]+50;
   cdb->main_sprt.u3 = game_sprt_x[selected_icon]+125;	   cdb->main_sprt.v3 = game_sprt_y[selected_icon]+50;

   game_sprt_offset = selected_icon;
   if (dir<0)
      {
      game_sprt_offset += 1;
      }
   else
   if (dir>0)
      {
      game_sprt_offset -= 1;
      } 

   if(game_sprt_offset<0) game_sprt_offset=9;
   if(game_sprt_offset>9) game_sprt_offset=0;

   cdb->main_new_sprt.u0 = game_sprt_x[game_sprt_offset];         cdb->main_new_sprt.v0 = game_sprt_y[game_sprt_offset];
   cdb->main_new_sprt.u1 = game_sprt_x[game_sprt_offset]+125;	   cdb->main_new_sprt.v1 = game_sprt_y[game_sprt_offset];
   cdb->main_new_sprt.u2 = game_sprt_x[game_sprt_offset]; 	      cdb->main_new_sprt.v2 = game_sprt_y[game_sprt_offset]+50;
   cdb->main_new_sprt.u3 = game_sprt_x[game_sprt_offset]+125;	   cdb->main_new_sprt.v3 = game_sprt_y[game_sprt_offset]+50;

   cdb->main_sprt.r0 = 128-main_sprt_bright;
   cdb->main_sprt.g0 = 128-main_sprt_bright;
   cdb->main_sprt.b0 = 128-main_sprt_bright;

   cdb->main_new_sprt.r0 = main_sprt_bright;
   cdb->main_new_sprt.g0 = main_sprt_bright;
   cdb->main_new_sprt.b0 = main_sprt_bright;
  
   AddPrim(cdb->ot+3,&cdb->main_sprt); 
   AddPrim(cdb->ot+3,&cdb->main_new_sprt);   

   for(icon=0; icon<10; icon++)
      {
      temp_ypos[icon]+=shit;

      bright = sin_inter_point(bright_start[icon],bright_end[icon],step*i);
      
      cdb->game_icon[icon].r0 = bright;
      cdb->game_icon[icon].g0 = bright;
      cdb->game_icon[icon].b0 = bright;

      cdb->game_icon_mask[icon].r0 = bright*2;
      cdb->game_icon_mask[icon].g0 = bright*2;
      cdb->game_icon_mask[icon].b0 = bright*2;

      cdb->game_icon[icon].y0 =temp_ypos[icon];	 
      cdb->game_icon[icon].y1 =temp_ypos[icon]; 
      cdb->game_icon[icon].y2 =temp_ypos[icon]+50; 
      cdb->game_icon[icon].y3 =temp_ypos[icon]+50; 

      cdb->game_icon_mask[icon].y0 =temp_ypos[icon];	 
      cdb->game_icon_mask[icon].y1 =temp_ypos[icon]; 
      cdb->game_icon_mask[icon].y2 =temp_ypos[icon]+50; 
      cdb->game_icon_mask[icon].y3 =temp_ypos[icon]+50; 
           
      AddPrim(cdb->ot+3,&cdb->game_icon[icon]); 
      
      SetDrawMode(&cdb->game_icon_mask_mode[icon],0,0,GetTPage(2,2,768,256),0); 
      AddPrim(cdb->ot+2,&cdb->game_icon_mask[icon]);
      AddPrim(cdb->ot+2,&cdb->game_icon_mask_mode[icon]); 
     }
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	 
   }


ClearOTag(cdb->ot, OTSIZE);		
}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
fade_out_menu(int icon_selected,int fader)
{
int i=0;
int j=0;
int k=0;
int count=0;


if (fader == 0)
   {
   j=   5;
   k=   0;
   fade_up(&db[0].screen_fade_mask,k);
   fade_up(&db[1].screen_fade_mask,k);
   }

if (fader == 1)
   {
   j= -5;
   k=255;
   fade_up(&db[0].screen_fade_mask,k);
   fade_up(&db[1].screen_fade_mask,k);

   }


   cdb = (cdb==db)? db+1: db;			
   ClearOTag(cdb->ot, OTSIZE);		
   cdb = (cdb==db)? db+1: db;			
   ClearOTag(cdb->ot, OTSIZE);		
 

do
   {
   DrawSync(0);						
   cdb = (cdb==db)? db+1: db;			
   ClearOTag(cdb->ot, OTSIZE);		

   fade_up(&cdb->screen_fade_mask,k);
   k+=j;
   count+=5;
   create_order(icon_selected);


   if(icon_selected>=0)
      {
      cdb->main_sprt.u0 = game_sprt_x[icon_selected];          cdb->main_sprt.v0 = game_sprt_y[icon_selected];
      cdb->main_sprt.u1 = game_sprt_x[icon_selected]+125;	   cdb->main_sprt.v1 = game_sprt_y[icon_selected];
      cdb->main_sprt.u2 = game_sprt_x[icon_selected]; 	      cdb->main_sprt.v2 = game_sprt_y[icon_selected]+50;
      cdb->main_sprt.u3 = game_sprt_x[icon_selected]+125;	   cdb->main_sprt.v3 = game_sprt_y[icon_selected]+50;

      cdb->main_sprt.r0 = 128;
      cdb->main_sprt.g0 = 128;
      cdb->main_sprt.b0 = 128;

      AddPrim(cdb->ot+3,&cdb->main_sprt); 
   

      for(i=0; i<10; i++)
      {                            
         cdb->game_icon[order_list[i]].r0=bright_list[i];
         cdb->game_icon[order_list[i]].g0=bright_list[i];
         cdb->game_icon[order_list[i]].b0=bright_list[i];
      
         cdb->game_icon_mask[order_list[i]].r0=bright_list[i]*2; 
         cdb->game_icon_mask[order_list[i]].g0=bright_list[i]*2; 
         cdb->game_icon_mask[order_list[i]].b0=bright_list[i]*2; 
     
         cdb->game_icon[order_list[i]].x0 = icon_info[i].xpos; 	  cdb->game_icon[order_list[i]].y0 =icon_info[i].ypos;	 
         cdb->game_icon[order_list[i]].x1 = icon_info[i].xpos+124;  cdb->game_icon[order_list[i]].y1 =icon_info[i].ypos; 
         cdb->game_icon[order_list[i]].x2 = icon_info[i].xpos;	  	  cdb->game_icon[order_list[i]].y2 =icon_info[i].ypos+50;	 
         cdb->game_icon[order_list[i]].x3 = icon_info[i].xpos+124;  cdb->game_icon[order_list[i]].y3 =icon_info[i].ypos+50;
      
         cdb->game_icon_mask[order_list[i]].x0 = icon_info[i].xpos; 	       cdb->game_icon_mask[order_list[i]].y0 =icon_info[i].ypos;	 
         cdb->game_icon_mask[order_list[i]].x1 = icon_info[i].xpos+124;     cdb->game_icon_mask[order_list[i]].y1 =icon_info[i].ypos;   
         cdb->game_icon_mask[order_list[i]].x2 = icon_info[i].xpos;	  	    cdb->game_icon_mask[order_list[i]].y2 =icon_info[i].ypos+50;	 
         cdb->game_icon_mask[order_list[i]].x3 = icon_info[i].xpos+124;     cdb->game_icon_mask[order_list[i]].y3 =icon_info[i].ypos+50;
         
         AddPrim(cdb->ot+3,&cdb->game_icon[order_list[i]]); /* draw the game icon */
      
         SetDrawMode(&cdb->game_icon_mask_mode[order_list[i]],0,0,GetTPage(2,2,768,256),0); 
         AddPrim(cdb->ot+2,&cdb->game_icon_mask[order_list[i]]); 	
         AddPrim(cdb->ot+2,&cdb->game_icon_mask_mode[order_list[i]]); 
         }
   
      }
   
   DrawSync(0);						
   VSync(0);							
   PutDrawEnv(&cdb->draw); 		
   PutDispEnv(&cdb->disp); 		
   draw_backdrop( (unsigned long*)&backtimbuff[0]); 
   DrawOTag(cdb->ot);	   
   }while(count!=260);


}
/**-------------------------------------------------------------------------**/




/**-------------------------------------------------------------------------**/
/**-- draw_gameicon(int icon,int xpos,int ypos,int bright)                --**/
/**-------------------------------------------------------------------------**/

set_gameicons()
{
int i;

//int ypos = -154;
int ypos = -200;

for(i=0; i<10; i++)
   {
   icon_info[i].xpos     =  25;
   icon_info[i].ypos     =  ypos;
   icon_info[i].bright   =  100;
   ypos+=60; 
   }

}
/**-------------------------------------------------------------------------**/



/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/* this creates the order in which the icons are displayed. */
/* the selected icon is always the fith entry in the list  */

create_order(int selected)
{
int i;


for(i=0; i<10; i++)
   {
   order_list[i] = selected+i -5;
   if (order_list[i]<0) order_list[i]+=10;
   if (order_list[i]>9) order_list[i]-=10;
   }

}

/**-------------------------------------------------------------------------**/












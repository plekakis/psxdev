/*****************************************************************************/
/*****************************************************************************/
/** graph.c ** for new memory card project thingy **   							 **/
/*****************************************************************************/
/*****************************************************************************/

/** 7.5.95 **/

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



extern FILEDATA filedata[];

extern ControllerPacket buffer1, buffer2;


/**-------------------- display --------------------------------------------**/
extern DB	  db[2];		      /* packet double buffer */
extern DB*    cdb;			   /* current db */


extern int  timbuff[];
extern int  backtimbuff[];



extern TIM    tim;

/**-------------------------------------------------------------------------**/



typedef struct 
	{
   int tim_xpos;
   int tim_ypos;
   int scr_xpos;
   int scr_ypos;
	} INFO;


static INFO icon_info[]=
   {
   /* tim xpos tim ypos  screen xpos  screen ypos */  
     {  0,         0,         20,            0},    /* crazy ivan    */
     {  0,        50,         20,           50},    /* jumping flash */
     {  0,       100,         20,          100},    /* tekken        */
     {  0,       150,         20,          150},    /* aircombat     */
     {  0,       200,         20,          200},    /* discworld     */
     {125,         0,        160,            0},    /* destruction d */
     {125,        50,        160,           50},    /* wipe out      */
     {125,       100,        160,          100},    /* kileak the    */
     {125,       150,        160,          150},    /* ridge racer   */
     {125,       200,        160,          200},    /* lemmings      */
   
   };






/**-------------------- set up the display ---------------------------------**/

init_graph()
{
RECT rect;

ResetGraph(0);
SetVideoMode(MODE_PAL);

SetGraphDebug(0);

InitGeom();							
SetGeomOffset(0, 0);				
SetGeomScreen(500);				
SetBackColor(128, 128, 128);	

/* ntsc for now !! */

SetDefDrawEnv(&db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
SetDefDispEnv(&db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);

SetDefDrawEnv(&db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
SetDefDispEnv(&db[1].disp, 0,   0,       FRAME_X, FRAME_Y);

db[1].disp.screen.x = db[0].disp.screen.x = SCREEN_X;
db[1].disp.screen.y = db[0].disp.screen.y = SCREEN_Y;


db[1].disp.screen.h = db[0].disp.screen.h = PAL_MAGIC;
db[1].disp.screen.w = db[0].disp.screen.w = PAL_MAGIC;


db[0].draw.dfe = db[1].draw.dfe = 1;


FntLoad(576,0);
SetDumpFnt(FntOpen(20, 20, 320, 200, 0, 512));

setRECT(&rect,0,0,320,512);
ClearImage(&rect,0,0,0);

SetDispMask(1);	

}
/**-------------------------------------------------------------------------**/




/**-- init_prim ------------------------------------------------------------**/


init_prim(DB *db)
{

int counter;

db->draw.isbg = 0;
db->draw.r0 = 0;
db->draw.b0 = 0;
db->draw.g0 = 0;	




SetPolyFT4(&db->first_icon);	
												
//db->first_icon.x0 = 136;      db->first_icon.y0 = 115;
//db->first_icon.x1 = 136+49;   db->first_icon.y1 = 115;
//db->first_icon.x2 = 136; 	   db->first_icon.y2 = 115+49;
//db->first_icon.x3 = 136+49;	db->first_icon.y3 = 115+49;

db->first_icon.r0 = 128;
db->first_icon.g0 = 128;
db->first_icon.b0 = 128;

db->first_icon.tpage = GetTPage(2,1,320,0);	
SetShadeTex(&db->first_icon,1);
SetSemiTrans(&db->first_icon,0);


SetPolyFT4(&db->first_name);	
												
//db->first_name.x0 = 136;       db->first_name.y0 = 115;
//db->first_name.x1 = 136+49;    db->first_name.y1 = 115;
//db->first_name.x2 = 136; 	   db->first_name.y2 = 115+49;
//db->first_name.x3 = 136+49;	   db->first_name.y3 = 115+49;

db->first_name.r0 = 128;
db->first_name.g0 = 128;
db->first_name.b0 = 128;

db->first_name.tpage = GetTPage(2,1,320,0);	
SetShadeTex(&db->first_name,0);
SetSemiTrans(&db->first_name,0);


/***************************************************************************/
SetPolyF4(&db->screen_fade_mask);	

db->screen_fade_mask.x0 =0;	 	db->screen_fade_mask.y0 = 0;	  
db->screen_fade_mask.x1 =320;    db->screen_fade_mask.y1 = 0;	  
db->screen_fade_mask.x2 =0;	   db->screen_fade_mask.y2 = 256;
db->screen_fade_mask.x3 =320;	   db->screen_fade_mask.y3 = 256;
db->screen_fade_mask.r0 = 0;
db->screen_fade_mask.g0 = 0;
db->screen_fade_mask.b0 = 0;

SetSemiTrans(&db->screen_fade_mask,1);    

/***************************************************************************/







for(counter=0; counter<15; counter++)
	{
	SetPolyFT4(&db->card_1_slot[counter]);	
														
	db->card_1_slot[counter].u0 = (16*counter);		db->card_1_slot[counter].v0 = 0;
	db->card_1_slot[counter].u1 = (16*counter)+16;	db->card_1_slot[counter].v1 = 0;
	db->card_1_slot[counter].u2 = (16*counter);	  	db->card_1_slot[counter].v2 = 16;
	db->card_1_slot[counter].u3 = (16*counter)+16;	db->card_1_slot[counter].v3 = 16;

	db->card_1_slot[counter].r0 = 128;
	db->card_1_slot[counter].g0 = 128;
	db->card_1_slot[counter].b0 = 128;

	db->card_1_slot[counter].tpage = GetTPage(2,1,768,0);	
	SetShadeTex(&db->card_1_slot[counter],0);
	SetSemiTrans(&db->card_1_slot[counter],0);

	SetPolyFT4(&db->slot_border[counter]);	
														
	db->slot_border[counter].u0 = 0;		db->slot_border[counter].v0 = 66;
	db->slot_border[counter].u1 = 0+18;	db->slot_border[counter].v1 = 66;
	db->slot_border[counter].u2 = 0;	  	db->slot_border[counter].v2 = 66+18;
	db->slot_border[counter].u3 = 0+18;	db->slot_border[counter].v3 = 66+18;

	db->slot_border[counter].r0 = 128;
	db->slot_border[counter].g0 = 128;
	db->slot_border[counter].b0 = 128;

	db->slot_border[counter].tpage = GetTPage(2,1,768,256);	
	SetShadeTex(&db->slot_border[counter],1);
	SetSemiTrans(&db->slot_border[counter],1);

	}



SetPolyFT4(&db->wait_sprite);	
												
db->wait_sprite.x0 = 136;     db->wait_sprite.y0 = 115;
db->wait_sprite.x1 = 136+49;  db->wait_sprite.y1 = 115;
db->wait_sprite.x2 = 136; 	   db->wait_sprite.y2 = 115+49;
db->wait_sprite.x3 = 136+49;	db->wait_sprite.y3 = 115+49;

db->wait_sprite.r0 = 128;
db->wait_sprite.g0 = 128;
db->wait_sprite.b0 = 128;

db->wait_sprite.tpage = GetTPage(2,1,320,0);	
SetShadeTex(&db->wait_sprite,1);
SetSemiTrans(&db->wait_sprite,0);



SetPolyFT4(&db->yes_button);	
												
db->yes_button.u0 = 83;       db->yes_button.v0 = 0;
db->yes_button.u1 = 83+20;	   db->yes_button.v1 = 0;
db->yes_button.u2 = 83; 	   db->yes_button.v2 = 20;
db->yes_button.u3 = 83+20;	   db->yes_button.v3 = 20;

db->yes_button.r0 = 128;
db->yes_button.g0 = 128;
db->yes_button.b0 = 128;

db->yes_button.tpage = GetTPage(2,1,768,256);	
SetShadeTex(&db->yes_button,1);
SetSemiTrans(&db->yes_button,0);


SetPolyFT4(&db->yes_text);	
												
db->yes_text.u0 = 123;       db->yes_text.v0 = 0;
db->yes_text.u1 = 123+24;	  db->yes_text.v1 = 0;  
db->yes_text.u2 = 123; 	     db->yes_text.v2 = 12;
db->yes_text.u3 = 123+24;	  db->yes_text.v3 = 12;

db->yes_text.r0 = 128;
db->yes_text.g0 = 128;
db->yes_text.b0 = 128;

db->yes_text.tpage = GetTPage(2,1,768,256);	
SetShadeTex(&db->yes_text,1);
SetSemiTrans(&db->yes_text,0);


SetPolyFT4(&db->no_button);	
												
db->no_button.u0 = 103;       db->no_button.v0 = 0;
db->no_button.u1 = 103+20;	   db->no_button.v1 = 0;
db->no_button.u2 = 103; 	   db->no_button.v2 = 20;
db->no_button.u3 = 103+20;	   db->no_button.v3 = 20;

db->no_button.r0 = 128;
db->no_button.g0 = 128;
db->no_button.b0 = 128;

db->no_button.tpage = GetTPage(2,1,768,256);	
SetShadeTex(&db->no_button,1);
SetSemiTrans(&db->no_button,0);



SetPolyFT4(&db->no_text);	
												
db->no_text.u0 = 123;         db->no_text.v0 = 13;
db->no_text.u1 = 123+20;	   db->no_text.v1 = 13;
db->no_text.u2 = 123; 	      db->no_text.v2 = 13+12;
db->no_text.u3 = 123+20;	   db->no_text.v3 = 13+12;

db->no_text.r0 = 128;
db->no_text.g0 = 128;
db->no_text.b0 = 128;

db->no_text.tpage = GetTPage(2,1,768,256);	
SetShadeTex(&db->no_text,1);
SetSemiTrans(&db->no_text,0);







SetPolyFT4(&db->message_text);	
												
db->message_text.u0 = 0;      db->message_text.v0 = 31;
db->message_text.u1 = 0+94;	db->message_text.v1 = 31;
db->message_text.u2 = 0; 	   db->message_text.v2 = 31+18;
db->message_text.u3 = 0+94;	db->message_text.v3 = 31+18;

db->message_text.r0 = 128;
db->message_text.g0 = 128;
db->message_text.b0 = 128;

db->message_text.tpage = GetTPage(2,1,768,256);	
SetShadeTex(&db->message_text,1);
SetSemiTrans(&db->message_text,0);




for(counter=0; counter<10; counter++)
	{
	SetPolyFT4(&db->game_icon[counter]);	
														
	db->game_icon[counter].u0 = icon_info[counter].tim_xpos; 		db->game_icon[counter].v0 = icon_info[counter].tim_ypos;	
	db->game_icon[counter].u1 = icon_info[counter].tim_xpos+124;	db->game_icon[counter].v1 = icon_info[counter].tim_ypos;
	db->game_icon[counter].u2 = icon_info[counter].tim_xpos;	  	   db->game_icon[counter].v2 = icon_info[counter].tim_ypos+50;	  	
	db->game_icon[counter].u3 = icon_info[counter].tim_xpos+124;	db->game_icon[counter].v3 = icon_info[counter].tim_ypos+50;

	db->game_icon[counter].x0 = icon_info[counter].scr_xpos; 	   db->game_icon[counter].y0 = icon_info[counter].scr_ypos;	 
	db->game_icon[counter].x1 = icon_info[counter].scr_xpos+124;   db->game_icon[counter].y1 = icon_info[counter].scr_ypos; 
	db->game_icon[counter].x2 = icon_info[counter].scr_xpos;	  	   db->game_icon[counter].y2 = icon_info[counter].scr_ypos+50;	 
	db->game_icon[counter].x3 = icon_info[counter].scr_xpos+124;   db->game_icon[counter].y3 = icon_info[counter].scr_ypos+50;

	db->game_icon[counter].r0 = 128;
	db->game_icon[counter].g0 = 128;
	db->game_icon[counter].b0 = 128;

	db->game_icon[counter].tpage = GetTPage(2,1,320,0);	
	SetShadeTex(&db->game_icon[counter],0);
	SetSemiTrans(&db->game_icon[counter],1);


	SetPolyF4(&db->game_icon_mask[counter]);	
	db->game_icon_mask[counter].r0 = 128;
	db->game_icon_mask[counter].g0 = 128;
	db->game_icon_mask[counter].b0 = 128;

   SetSemiTrans(&db->game_icon_mask[counter],1);    
	}


SetPolyFT4(&db->main_sprt);		         
db->main_sprt.r0 = 128;
db->main_sprt.g0 = 128;
db->main_sprt.b0 = 128;

db->main_sprt.x0 = 175; 	   db->main_sprt.y0 = 146;     
db->main_sprt.x1 = 175+125;   db->main_sprt.y1 = 146;     
db->main_sprt.x2 = 175;       db->main_sprt.y2 = 146+50;   
db->main_sprt.x3 = 175+125;   db->main_sprt.y3 = 146+50;  

db->main_sprt.tpage = GetTPage(2,1,320,256);	
SetShadeTex(&db->main_sprt,0);
SetSemiTrans(&db->main_sprt,1);


SetPolyFT4(&db->main_new_sprt);		   
db->main_new_sprt.r0 = 128;
db->main_new_sprt.g0 = 128;
db->main_new_sprt.b0 = 128;

db->main_new_sprt.x0 = 175; 	    db->main_new_sprt.y0 = 146;     
db->main_new_sprt.x1 = 175+125;   db->main_new_sprt.y1 = 146;     
db->main_new_sprt.x2 = 175;       db->main_new_sprt.y2 = 146+50;   
db->main_new_sprt.x3 = 175+125;   db->main_new_sprt.y3 = 146+50;  


db->main_new_sprt.tpage = GetTPage(2,1,320,256);	
SetShadeTex(&db->main_new_sprt,0);
SetSemiTrans(&db->main_new_sprt,1);


SetPolyFT4(&db->confirm_button);	
										
db->confirm_button.u0 = 9-8;     db->confirm_button.v0 = 180-8;
db->confirm_button.u1 = 9+8;	   db->confirm_button.v1 = 180-8;
db->confirm_button.u2 = 9-8; 	   db->confirm_button.v2 = 180+8;
db->confirm_button.u3 = 9+8;	   db->confirm_button.v3 = 180+8;

db->confirm_button.r0 = 128;
db->confirm_button.g0 = 128;
db->confirm_button.b0 = 128;

db->confirm_button.tpage = GetTPage(2,1,768,256);	
SetShadeTex(&db->confirm_button,0);
SetSemiTrans(&db->confirm_button,1);

SetPolyFT4(&db->patch);	
												
db->patch.u0 = 0;       db->patch.v0 = 84;
db->patch.u1 = 0+83;	   db->patch.v1 = 84;
db->patch.u2 = 0; 	   db->patch.v2 = 84+87;
db->patch.u3 = 0+83;	   db->patch.v3 = 84+87;

db->patch.r0 = 128;
db->patch.g0 = 128;
db->patch.b0 = 128;

db->patch.tpage = GetTPage(2,2,768,256);	
SetShadeTex(&db->patch,1);
SetSemiTrans(&db->patch,1);

                         

//----------------------------------------------------------------

SetPolyFT4(&db->option_message_delete);	
												
db->option_message_delete.u0 = 0;         db->option_message_delete.v0 = 84;
db->option_message_delete.u1 = 0+77+11;	db->option_message_delete.v1 = 84;
db->option_message_delete.u2 = 0; 	      db->option_message_delete.v2 = 84+16;
db->option_message_delete.u3 = 0+77+11;	db->option_message_delete.v3 = 84+16;

db->option_message_delete.r0 = 128;
db->option_message_delete.g0 = 128;
db->option_message_delete.b0 = 128;

db->option_message_delete.tpage = GetTPage(2,2,768,256);	
SetShadeTex(&db->option_message_delete,1);
SetSemiTrans(&db->option_message_delete,1);


//----------------------------------------------------------------

SetPolyFT4(&db->option_message_quit);	
												
db->option_message_quit.u0 = 0;        db->option_message_quit.v0 = 100;
db->option_message_quit.u1 = 0+77+11;	db->option_message_quit.v1 = 100;
db->option_message_quit.u2 = 0; 	      db->option_message_quit.v2 = 100+16;
db->option_message_quit.u3 = 0+77+11;	db->option_message_quit.v3 = 100+16;

db->option_message_quit.r0 = 128;
db->option_message_quit.g0 = 128;
db->option_message_quit.b0 = 128;

db->option_message_quit.tpage = GetTPage(2,2,768,256);	
SetShadeTex(&db->option_message_quit,1);
SetSemiTrans(&db->option_message_quit,1);

//----------------------------------------------------------------

SetPolyFT4(&db->option_message_format);	
												
db->option_message_format.u0 = 0;         db->option_message_format.v0 =116;
db->option_message_format.u1 = 0+77+11;	db->option_message_format.v1 =116;
db->option_message_format.u2 = 0; 	      db->option_message_format.v2 =116+16;
db->option_message_format.u3 = 0+77+11;	db->option_message_format.v3 =116+16;

db->option_message_format.r0 = 128;
db->option_message_format.g0 = 128;
db->option_message_format.b0 = 128;

db->option_message_format.tpage = GetTPage(2,2,768,256);	
SetShadeTex(&db->option_message_format,1);
SetSemiTrans(&db->option_message_format,1);

}
/**-------------------------------------------------------------------------**/




/**-------------------------------------------------------------------------**/
/**----------- bitmap loader from cd ---------------------------------------**/
/**-------------------------------------------------------------------------**/

/* bitmap_cd(int file,int copy_flag)

loads a file from cd and depending on the status of copy_flag either copies it 
to vram using a temp buffer, tim_buff or stores in timbackbuff so it used by
load_backdrop()

int file:      the number of the file to look up in filedata to get seek data 
               and file size

int copy_flag: if its 2 load a back drop into backtimbuff else use timbuff as
               temp store and load into vram. can't use the same buffer for
               both because would have to reload the backdrop all the time
               which would be crap...
*/


/** if copy flag = 1 copy the image into vram using tim , if it is 0 don't				 **/
/** this is so the backdrop (which sits in the buffer doesn't 					 **/
/** get displayed prematurely																 **/

bitmap_cd(int file,int copy_flag)
{

CdlFILE fp;       	
long fileLength;   	
int numSectors;   	
int mode = 0;     	
long counter;			

u_long* addr;

GsIMAGE TexInfo;
RECT rect1;

printf("loading file %d %s\n",file,filedata[file].cd_filename); 
printf("cd  %s\n",filedata[file].fp.name); 


//if (!CdSearchFile(&fp,filename))           
//  		{
//		printf("ERROR: could not find file\n");  
//		return 0;
//		}                               



fileLength = filedata[file].fp.size;     

printf("filelength = %d\n",fileLength);  

retry:

if (!CdControlB(CdlSeekL,(unsigned char *)&filedata[file].fp.pos, 0))
  		{
		printf("ERROR: could not do seek\n");  
		goto retry;
		}                               

numSectors = Sectors(fileLength);
/* printf("numsectors = %d\n",numSectors); */
mode |= CdlModeSpeed;



if (copy_flag==2)
   {
   addr=(unsigned long *)&backtimbuff[0];
   }   
else
   {
   addr=(unsigned long *)&timbuff[0];
   }


if(!CdRead(numSectors,addr,mode))
  		{
		printf("ERROR: could not exectute read\n");  
		goto retry;
	   }                               


while (CdReadSync(1,0) > 0 );

if (copy_flag!=2)
	{
	addr=(unsigned long *)&timbuff[0];
	addr++;

	GsGetTimInfo(addr, &TexInfo);

	rect1.x = TexInfo.px;
	rect1.y = TexInfo.py;
	rect1.w = TexInfo.pw;
	rect1.h = TexInfo.ph;
	VSync(0);
	LoadImage(&rect1,TexInfo.pixel); 

	rect1.x = TexInfo.cx;
	rect1.y = TexInfo.cy;
	rect1.w = TexInfo.cw;
	rect1.h = TexInfo.ch;
	VSync(0);
	LoadImage(&rect1,TexInfo.clut); 
	}


}
/**------------------------------------------------------------------------**/




/**------------------------------------------------------------------------**/
/**-------------- load image the backdrop ---------------------------------**/
/**------------------------------------------------------------------------**/

draw_backdrop(u_long *addr)
{
GsIMAGE TexInfo;
RECT rect1;

addr++;

GsGetTimInfo(addr, &TexInfo);

rect1.x = 0;
rect1.y = cdb->draw.clip.y;
rect1.w = TexInfo.pw;
rect1.h = TexInfo.ph;

LoadImage(&rect1,TexInfo.pixel); 
DrawSync(0);						

}
/**------------------------------------------------------------------------**/



/**------------------------------------------------------------------------**/
/** create a blank tim                                                     **/
/**------------------------------------------------------------------------**/
init_tim(TIM* tim)
{
/** this sets up a convienient tim in main ram without having to load it   **/
/** from CD which would be slow                                            **/

tim->tim_struct.id 	= 0x10;							
tim->tim_struct.flag = 0x02;
tim->tim_struct.bnum	= 0x2000c;
tim->tim_struct.dy	= 0x0;
tim->tim_struct.dx	= 0x300;
tim->tim_struct.h		= 0x10;   /* 10*256 pixels */
tim->tim_struct.w		= 0x100;

}
/**------------------------------------------------------------------------**/



fade_up(POLY_F4* poly,int mask)
{
cdb->screen_fade_mask.r0 = mask;
cdb->screen_fade_mask.g0 = mask;
cdb->screen_fade_mask.b0 = mask;

SetDrawMode(&cdb->screen_fade_mask_mode,0,0,GetTPage(2,2,768,256),0); 

AddPrim(cdb->ot+6,&cdb->screen_fade_mask); 	
AddPrim(cdb->ot+6,&cdb->screen_fade_mask_mode); 
  
}

/**------------------------------------------------------------------------**/




power_fade(int fader)
{
int i=0;
int j=0;
int k=0;
int count=0;

if (fader == 0)
   {
   j=   5;
   k=   5;
   fade_up(&db[0].screen_fade_mask,k);
   fade_up(&db[1].screen_fade_mask,k);
   }

if (fader == 1)
   {
   j= -5;
   k=255;
   fade_up(&db[0].screen_fade_mask,k);
   fade_up(&db[1].screen_fade_mask,k);

   ClearOTag(cdb->ot, OTSIZE);		                  // this is a bodge to 
   draw_backdrop( (unsigned long*)&backtimbuff[0]);   // stop a flicker 
   fade_up(&cdb->screen_fade_mask,k);
   DrawOTag(cdb->ot);	   

   }


do
   {
   DrawSync(0);						
   cdb = (cdb==db)? db+1: db;			
   ClearOTag(cdb->ot, OTSIZE);		

   fade_up(&cdb->screen_fade_mask,k);
   k+=j;
   count+=5;

   FntFlush(-1);
   DrawSync(0);						
   VSync(0);							
   PutDrawEnv(&cdb->draw); 		
   PutDispEnv(&cdb->disp); 		
   draw_backdrop( (unsigned long*)&backtimbuff[0]); 
   DrawOTag(cdb->ot);	   
   }while(count!=255);


}
/**-------------------------------------------------------------------------**/



power_line(int pic)
   {
   gpower_line(0); // power line 1
   gpower_line(1); // power line 2
   gpower_line(4); // power line 2
   }







/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
gpower_line(int pic)
{
int isok=-15;

switch(pic)
   {
   case 0:
      bitmap_cd(POS_POWERLINE1,2);
      break;

   case 1:
      bitmap_cd(POS_POWERLINE2,2);
      break;

   case 2:
      bitmap_cd(POS_COPYRGHT,2);
      break;
   
   case 3:
      bitmap_cd(POS_CREDIT,2);
      break;

   case 4:
      bitmap_cd(POS_POWERLINE3,2);
      break;


   default:
      break;
   }


power_fade(1);
                    

do{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

	if ( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
		{
		if ( PadKeyPressed(&buffer1)!=0) 
		   {
         bleep(4);
         power_fade(0);
         return 1; 
		   }
		}

//   if(isok> (50*5) )    /*display for 5 seconds */
//      {
//      bleep(4);
//      power_fade(0);
//      return 1;
//      }

   isok++;     
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);  
	DrawOTag(cdb->ot);	  			
   }while(1);
}







/*****************************************************************************/
/*****************************************************************************/
/** copy.c ** for new memory card project thingy **   							 **/
/*****************************************************************************/
/*****************************************************************************/

/** 16.5.95 **/

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

extern CDCARDBUFFER game_buff;
extern ControllerPacket buffer1, buffer2;

/**-------------------- display --------------------------------------------**/
extern DB	  db[2];		      /* packet double buffer */
extern DB*    cdb;			   /* current db */
extern TIM    tim;

extern int  timbuff[];
extern int  backtimbuff[];




/**-------------------------------------------------------------------------**/

extern FILEDATA filedata[];


typedef struct
{
char memcard_filename[21];
char cd_filename[21];
int  filesize;
}MEMSAVE;
                  

static MEMSAVE memsave[] = 
{
        {"BESLES-00084ROB2"       ,"\\SAVEDATA\\CI.SAV;1",  8192},       /* crazy ivan  */
        {"BESCES-00003EXACT010"   ,"\\SAVEDATA\\JF.SAV;1",  8192},       /* jumping f   */
        {"BESCES-00005<TEKKEN>"   ,"\\SAVEDATA\\TE.SAV;1",  8192},       /* tekken      */
        {"BESCES-00007AIRCOMB"    ,"\\SAVEDATA\\AC.SAV;1",  8192},       /* aircombat   */
        {"BESCES-00012DISCWLD"    ,"\\SAVEDATA\\DW.SAV;1",8*8192},       /* disc w      */
        {"BESCES-00008-dd.8"      ,"\\SAVEDATA\\DD.SAV;1",  8192},       /* dest derby  */
        {"BESCES-00010WOCHEATS"   ,"\\SAVEDATA\\WO.SAV;1",  8192},       /* wipout      */
        {"BESCES-00035"           ,"\\SAVEDATA\\KI.SAV;1",  8192},       /* kileak      */
        {"BESCES-0000133244200"   ,"\\SAVEDATA\\RR.SAV;1",  8192},       /* ridge ra    */
        {"BESCES-00009LAMPWICK"   ,"\\SAVEDATA\\LE.SAV;1",  8192},       /* lemmings    */
};


int bar_tab = 100;



/**--------------------------------------------------------------------**/
/**----------------- vcounter -----------------------------------------**/
/**--------------------------------------------------------------------**/
void spinny()
{
static int xtex[] = {67,67+16,67+32,67+48,67+64};
static int i=0;
static int j=0;

RECT   spinrect;

j++;
if (j==7)
   {
   j=0;
   i++;
   if (i==5) i=0;
   }


setRECT(&spinrect,768+xtex[i],256,16,16);
//MoveImage(&spinrect,43+201,121);
//MoveImage(&spinrect,43+201,256+121);

MoveImage(&spinrect,43+201,cdb->disp.disp.y+121);

if (i==20) i=-1;


  
} 
/**--------------------------------------------------------------------**/


















/**-------------------------------------------------------------------------**/
/**-- #init_save_prim(DB *db)                                              --**/
/**-------------------------------------------------------------------------**/

init_save_prim(DB *db)
{
int counter;

for(counter=0; counter<5; counter++)
	{
	SetPolyFT4(&db->save[counter]);	
	db->save[counter].r0 = 128;
	db->save[counter].g0 = 128;
	db->save[counter].b0 = 128;

	db->save[counter].u0 =  0;			db->save[counter].v0 = 0;  
	db->save[counter].u1 = 64;			db->save[counter].v1 = 0;	 
	db->save[counter].u2 =  0;			db->save[counter].v2 = 255;
	db->save[counter].u3 = 64;			db->save[counter].v3 = 255;

	db->save[counter].x0 = counter*64;		db->save[counter].y0 = 0;  
	db->save[counter].x1 = counter*64+64;	db->save[counter].y1 = 0;	 
	db->save[counter].x2 = counter*64;		db->save[counter].y2 = 255;
	db->save[counter].x3 = counter*64+64;	db->save[counter].y3 = 255;
	db->save[counter].tpage = GetTPage(2,1,768,256);	

	SetShadeTex(&db->save[counter],1);
	SetSemiTrans(&db->save[counter],0);
	}

for(counter=0; counter<2; counter++)
	{
	SetPolyFT4(&db->savebar[counter]);	
	db->savebar[counter].r0 = 128;
	db->savebar[counter].g0 = 128;
	db->savebar[counter].b0 = 128;
	db->savebar[counter].tpage = GetTPage(2,1,768,256);	

	SetShadeTex(&db->savebar[counter],1);
	SetSemiTrans(&db->savebar[counter],0);
	}


db->savebar[0].u0 = 64;		   db->savebar[0].v0 = 16;  
db->savebar[0].u1 = 64+191;	db->savebar[0].v1 = 16;	 
db->savebar[0].u2 = 64;		   db->savebar[0].v2 = 16+51;
db->savebar[0].u3 = 64+191;	db->savebar[0].v3 = 16+51;

db->savebar[0].x0 = 0;	      db->savebar[0].y0 = 112;   
db->savebar[0].x1 = 0+191;	   db->savebar[0].y1 = 112;	  
db->savebar[0].x2 = 0;	      db->savebar[0].y2 = 112+51;
db->savebar[0].x3 = 0+191;	   db->savebar[0].y3 = 112+51;


db->savebar[1].u0 = 64;		   db->savebar[1].v0 = 68;  
db->savebar[1].u1 = 64+128;	db->savebar[1].v1 = 68;	 
db->savebar[1].u2 = 64;		   db->savebar[1].v2 = 68+51;
db->savebar[1].u3 = 64+128;	db->savebar[1].v3 = 68+51;

db->savebar[1].x0 = 191;	   db->savebar[1].y0 = 112;   
db->savebar[1].x1 = 191+128;	db->savebar[1].y1 = 112;	  
db->savebar[1].x2 = 191;	   db->savebar[1].y2 = 112+51;
db->savebar[1].x3 = 191+128;	db->savebar[1].y3 = 112+51;


for(counter=0; counter<3; counter++)
	{
	SetPolyFT4(&db->savebar_body[counter]);	
	db->savebar_body[counter].r0 = 128;
	db->savebar_body[counter].g0 = 128;
	db->savebar_body[counter].b0 = 128;
	db->savebar_body[counter].tpage = GetTPage(2,1,768,256);	

	SetShadeTex(&db->savebar_body[counter],1);
	SetSemiTrans(&db->savebar_body[counter],0);
	}


db->savebar_body[0].u0 = 64;		db->savebar_body[0].v0 = 0;  
db->savebar_body[0].u1 = 64+0;	db->savebar_body[0].v1 = 0;	 
db->savebar_body[0].u2 = 64;	   db->savebar_body[0].v2 = 0+15;
db->savebar_body[0].u3 = 64+0;	db->savebar_body[0].v3 = 0+15;
                            
db->savebar_body[1].u0 = 65;	   db->savebar_body[1].v0 = 0;   
db->savebar_body[1].u1 = 65+0;	db->savebar_body[1].v1 = 0;	 
db->savebar_body[1].u2 = 65;	   db->savebar_body[1].v2 = 0+15;
db->savebar_body[1].u3 = 65+0;	db->savebar_body[1].v3 = 0+15;

db->savebar_body[2].u0 = 66;	   db->savebar_body[2].v0 = 0;   
db->savebar_body[2].u1 = 66+0;	db->savebar_body[2].v1 = 0;	
db->savebar_body[2].u2 = 66;	   db->savebar_body[2].v2 = 0+15;
db->savebar_body[2].u3 = 66+0;	db->savebar_body[2].v3 = 0+15;


SetPolyFT4(&db->save_message);	
db->save_message.r0 = 128;
db->save_message.g0 = 128;
db->save_message.b0 = 128;

db->save_message.x0 = 35;	      db->save_message.y0 = 88;   
db->save_message.x1 = 35+255;	   db->save_message.y1 = 88;	  
db->save_message.x2 = 35;	      db->save_message.y2 = 88+93;
db->save_message.x3 = 35+255;	   db->save_message.y3 = 88+93;
   

db->save_message.tpage = GetTPage(2,1,768,0);	

SetShadeTex(&db->save_message,1);
SetSemiTrans(&db->save_message,0);


/***** alert button **************************/
SetPolyFT4(&db->alert_button);	
db->alert_button.tpage = GetTPage(2,1,768,0);	

SetShadeTex(&db->alert_button,0);
SetSemiTrans(&db->alert_button,1);



}

/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
//save_file(int selected)
//{
//int save_file_status; 
//
//save_graphic(-1,selected);
//
//save_file_status = dave_copy(selected);
//
//bitmap_cd(POS_GAICFILENAME,1);   
//save_graphic(1,selected);
//
//return save_file_status;
//
//}
/**-------------------------------------------------------------------------**/



/**-------------------------------------------------------------------------**/
/** move save graphic into positions                                        **/
/**-------------------------------------------------------------------------**/

save_graphic(int direction,int selected)
{
/* if direction < 0 move move sliders away */
/* else move sliders into position         */

int three_slider;
int three_end_point;
int three_inter;
int two_slider;
int two_end_point;
int two_inter;

int box_pos;
int box_start;
int box_end;


int steps;
int step;
int i;
int j;


if (direction>0)
	{
	three_slider=two_slider=128;
	three_end_point =  0-128;
	two_end_point   =256+129;

	box_start = 160;
	box_end =   320+160;

	}
else
	{

   bitmap_cd(POS_SAVEFILENAME,1);
//   bitmap_cd(POS_MAINBACKFILENAME,2);

	three_slider=  0-128;
	two_slider  =256+129;
	three_end_point = two_end_point = 128;

	box_start = 0-160;
	box_end =   160;

	}

steps = 20;
step = sin_calc_stepsize(steps);


for(i=1; i<=steps; i++)
	{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

	three_inter =sin_inter_point(three_slider,three_end_point,step*i);
	two_inter   =sin_inter_point(two_slider,two_end_point,step*i);
	box_pos     =sin_inter_point(box_start,box_end,step*i);

	if(i>steps-2 && direction<0 )	/*make sure it ends in exactly the right place */
		{
		three_inter=two_inter=128;
		box_pos=160;
		}

	cdb->save[0].y0 = cdb->save[0].y1 = three_inter-128;
	cdb->save[0].y2 = cdb->save[0].y3 = three_inter+128;

	cdb->save[2].y0 = cdb->save[2].y1 = three_inter-128;
	cdb->save[2].y2 = cdb->save[2].y3 = three_inter+128;

	cdb->save[4].y0 = cdb->save[4].y1 = three_inter-128;
	cdb->save[4].y2 = cdb->save[4].y3 = three_inter+128;

	cdb->save[1].y0 = cdb->save[1].y1 = two_inter-128;
	cdb->save[1].y2 = cdb->save[1].y3 = two_inter+128;

	cdb->save[3].y0 = cdb->save[3].y1 = two_inter-128;
	cdb->save[3].y2 = cdb->save[3].y3 = two_inter+128;

	AddPrim(cdb->ot+11,&cdb->save[0]); 	
	AddPrim(cdb->ot+11,&cdb->save[2]); 	
	AddPrim(cdb->ot+11,&cdb->save[4]); 	
	AddPrim(cdb->ot+11,&cdb->save[1]); 	
	AddPrim(cdb->ot+11,&cdb->save[3]); 	

   AddPrim(cdb->ot+1,&cdb->main_sprt); 
   for(j=0; j<10; j++)
      {                            
      AddPrim(cdb->ot+1,&cdb->game_icon[j]); 
      SetDrawMode(&cdb->game_icon_mask_mode[j],0,0,GetTPage(2,2,768,256),0); 
      AddPrim(cdb->ot+1,&cdb->game_icon_mask[j]); 	
      AddPrim(cdb->ot+1,&cdb->game_icon_mask_mode[j]); 
      }

	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			
	}

}

/*****************************************************************************/








/******************************************************************************/
/* static save screen with barchart                                           */
/******************************************************************************/

save_screen(int selected)
{
int isok =-15;
int counter =  0;



printf("file %s  %s",memsave[selected].memcard_filename,memsave[selected].cd_filename);

do{
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

   if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD && isok>0 ) 
      {
      if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
	      {
	      ClearOTag(cdb->ot, OTSIZE);		
         isok=-15;
         return 1;
         }   
   if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
	      {
	      ClearOTag(cdb->ot, OTSIZE);		
        // map();
         isok=-15;
         }   

      }

	AddPrim(cdb->ot+1,&cdb->save[0]); 	
	AddPrim(cdb->ot+1,&cdb->save[2]); 	
	AddPrim(cdb->ot+1,&cdb->save[4]); 	
	AddPrim(cdb->ot+1,&cdb->save[1]); 	
	AddPrim(cdb->ot+1,&cdb->save[3]); 	

	AddPrim(cdb->ot+3,&cdb->savebar[0]); 	
	AddPrim(cdb->ot+3,&cdb->savebar[1]); 	

   do_the_bar(counter);
   counter++;
   if (counter ==240) counter=0;


   isok++;
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			
	}while(1);

}

/*****************************************************************************/




/******************************************************************************/
/* draw the bar                                                               */
/******************************************************************************/

do_the_bar(int amount)
{

cdb->savebar_body[0].x0 = 43;	            cdb->savebar_body[0].y0 = 121;   
cdb->savebar_body[0].x1 = 43+1;	         cdb->savebar_body[0].y1 = 121;	  
cdb->savebar_body[0].x2 = 43;             cdb->savebar_body[0].y2 = 121+16;
cdb->savebar_body[0].x3 = 43+1;	         cdb->savebar_body[0].y3 = 121+16;
                                                      
cdb->savebar_body[1].x0 = 43+1;	         cdb->savebar_body[1].y0 = 121;   
cdb->savebar_body[1].x1 = 43+1+amount;	   cdb->savebar_body[1].y1 = 121;	
cdb->savebar_body[1].x2 = 43+1;	         cdb->savebar_body[1].y2 = 121+16;
cdb->savebar_body[1].x3 = 43+1+amount;    cdb->savebar_body[1].y3 = 121+16;

cdb->savebar_body[2].x0 = 43+1+amount;	   cdb->savebar_body[2].y0 = 121;   
cdb->savebar_body[2].x1 = 43+1+amount+1;	cdb->savebar_body[2].y1 = 121;	
cdb->savebar_body[2].x2 = 43+1+amount;	   cdb->savebar_body[2].y2 = 121+16;
cdb->savebar_body[2].x3 = 43+1+amount+1;	cdb->savebar_body[2].y3 = 121+16;

AddPrim(cdb->ot+4,&cdb->savebar_body[0]); 	
AddPrim(cdb->ot+4,&cdb->savebar_body[1]); 	
AddPrim(cdb->ot+4,&cdb->savebar_body[2]); 	

}









/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

dave_copy(int selected)
{

struct	DIRENTRY card_dir[15];	
int               card_files;

int card_status;
int wait_status;
int i;
int space_free;
int overwrite_flag=FALSE;








card_status = status(PORT0);

printf("card_status %d\n",card_status);

switch (card_status)
	{
	case IOE:
		break;

 	case ERROR: 	
		/**** to be honest I don't know what to do if this happens ****/
		/**** so return a 1 which prompts a retry                  ****/
		return 1;  /* retry */
		break;

   /**#####################################################################**/
 	case TIMEOUT:
      load_error_message_tpage(NO_CARD);
		wait_status = no_card(PORT0);
   	if(wait_status == 0) return 0; 
		break;
   /**#####################################################################**/


   /**#####################################################################**/
   case NEWCARD:
      load_error_message_tpage(NO_FORMAT);
		wait_status = no_format(PORT0);
		if(wait_status == 0) return 0;
		break;
   /**#####################################################################**/


	default:
      break;
	}/* end switch (card_status)*/

grow_bar(50,70);

card_files = dir_file(&card_dir[0]);

grow_bar(70,80);

printf("card_files = %d\n",card_files);


/**** check all the files on the card to ensure that the card about to be ****/
/**** saved does not already exist                                        ****/
i=-1; 

do
	{
	i++; 
	printf("i >>%d ",i);
	printf("  >>%s ",memsave[selected].memcard_filename);
	printf("  >>%s \n",card_dir[i].name);
	}
while( strcmp(&memsave[selected].memcard_filename[0],&card_dir[i].name[0])!=0 && (i<card_files) );


if (i<card_files) /* then the file already exits */
	{
   printf("This file already exists\n");
   load_error_message_tpage(OVERWRITE);
   wait_status = overwrite(PORT0);
	if(wait_status == 0)
      {
      return 0;  /* no retry */
      }
   else
      {
      overwrite_flag=TRUE;
      }
	}


grow_bar(80,90);



/*** load the saved game into ram ***/

load_game(&memsave[selected].cd_filename[0],memsave[selected].filesize,&game_buff.buffer[0]); 


grow_bar(90,110);


/**** check to see it there is space on the card to save the game ****/

space_free = 15;
for (i=0; i<card_files; i++)
	{
	space_free -= (card_dir[i].size/8192);
	}

   printf("************************************************************");
	printf("actual space_free %d\n",space_free);
	printf("space reqd        %d\n",game_buff.card.header.BlockEntry);
   printf("************************************************************");


grow_bar(110,160);


if (space_free < game_buff.card.header.BlockEntry && overwrite_flag == FALSE)
	{
   load_error_message_tpage(CARD_FULL);

   do
      {
      wait_status =no_room();  
      draw_save_screen(110);
      printf("***************************> space free = %d\n",wait_status);
      printf("***************************> required   = %d\n",game_buff.card.header.BlockEntry);

      if(wait_status==0)   
         {
         return 0;
         }
      }while(wait_status<game_buff.card.header.BlockEntry);
   }


/** actaully do the write.......... cool **/


printf("   doing the write ");
grow_bar(160,201);


draw_save_screen(201);
VSyncCallBack(spinny);


printf("bytes to write is ------------->%d< \n",memsave[selected].filesize);



if (write_file_memcard(&memsave[selected].memcard_filename[0],memsave[selected].filesize,&game_buff.buffer[0])!=memsave[selected].filesize)
   {
   VSyncCallBack(0);

   grow_bar(201,240);


   save_fail();
   printf("ERROR WRITE FAILED!!!!!!!!!!!!!!!!!!!!!!\n");
   return 0;
   bleep(4);

   }
else
   {
   VSyncCallBack(0);
   grow_bar(201,240);
   save_ok();

//   grow_bar(230,240);

for (i=0; i<20; i++)
	{
	VSync(0);
	}

 //  power_line(2);

//   bitmap_cd(POS_GAICFILENAME,1);      /*new!*/
//   bitmap_cd(POS_MSICFILENAME,1);
//   bitmap_cd(POS_MAINBACKFILENAME,2);   

   printf("COOL WRITE WAS OK!!!!!!!!!!!!!!!!!!!!!!\n");
bleep(4);
   return 1;
   }


return 0;

}



/*****************************************************************************/
//bar_chart(int width)
//{
//RECT rect;
//int i;
//setRECT(&rect,20,cdb->disp.disp.y+110,width,18);
//ClearImage(&rect,200,0,0);
//}

/*****************************************************************************/




/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


no_card()
{
int isok=-15;
int test=0;
int stat=0;
bleep(0);
do
   {
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		
   isok++;
   VSync(0);

   if((GoodData(&buffer1) && GetType(&buffer1)==STD_PAD)) 
      {
      if (PadKeyIsPressed(&buffer1,PAD_RU)!=0 && isok>0) 
	      {
         bleep(3);
         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);
            alert_button_flash(AL4A_NO,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RU)!=0);
  	 //     ClearOTag(cdb->ot, OTSIZE);		
         /* quit error processing */
         return 0;

        }   
      }

   error_main_graphic();
   do_the_bar(bar_tab);
	
   DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			

   test = card_present_status(PORT0);
   printf("test = %d\n",test);  

   }while(test==0);
   return 1;
   

}

/*****************************************************************************/



no_format()
{
int test=0;
int stat=0;


/* load the error message and set the uv coords for the message */


do
   {
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

   VSync(0);
   if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
      {
      if (PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
	      {
         bleep(3);

         printf("hi hi hi\n");
         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);
            alert_button_flash(AL2A_NO,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RU)!=0);
  	   //   ClearOTag(cdb->ot, OTSIZE);		
         return 0;
         }


      if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
	      {
         bleep(3);

         printf("lo lo lo\n");
         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);
            alert_button_flash(AL2A_OK,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RD)!=0);

      //   ClearOTag(cdb->ot , OTSIZE);		

         return format_confirm();
         /* quit error processing */
         }
   
      }

   error_main_graphic();
   do_the_bar(bar_tab);
	//AddPrim(cdb->ot+4,&cdb->save_message); 	
	
   DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			

   }while(1);

}
/**-------------------------------------------------------------------------**/

/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/


int format_confirm()
{
int stat=0;
int card_status;


//cdb->save_message.u0 = 0;	      cdb->save_message.v0 = 93;   
//cdb->save_message.u1 = 0+255;	   cdb->save_message.v1 = 93;	  
//cdb->save_message.u2 = 0;	      cdb->save_message.v2 = 93+93;
//cdb->save_message.u3 = 0+255;	   cdb->save_message.v3 = 93+93;

VSync(0);
db[0].save_message.u0 = 0;	         db[0].save_message.v0 = 93;   
db[0].save_message.u1 = 0+255;      db[0].save_message.v1 = 93;	  
db[0].save_message.u2 = 0;	         db[0].save_message.v2 = 93+93;
db[0].save_message.u3 = 0+255;	   db[0].save_message.v3 = 93+93;

db[1].save_message.u0 = 0;	         db[1].save_message.v0 = 93;   
db[1].save_message.u1 = 0+255;	   db[1].save_message.v1 = 93;	  
db[1].save_message.u2 = 0;	         db[1].save_message.v2 = 93+93;
db[1].save_message.u3 = 0+255;	   db[1].save_message.v3 = 93+93;


printf("format confirm\n");
do
   {
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

   VSync(0);
   if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
      {
      if (PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
	      {
         bleep(3);

         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);
            alert_button_flash(AL2B_NO,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RU)!=0);
         return 0;
         }
      if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
	      {
         bleep(3);

         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);
            alert_button_flash(AL2B_OK,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RD)!=0);

         card_status = status(PORT0); 
         if(card_status == NEWCARD)
            {
            printf("checked card again, proceed with format...");
            format("bu00:");
            return 1; // card status has changed so check status
            }
         else
            {
            printf("denied!! %d\n",card_status);
            return 1;  // card may have been swapped so force check status
            }
         }   

      }

   error_main_graphic();      //draws the save bars 
   do_the_bar(100);
	
   DrawSync(0);						
	VSync(2);
							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			

   }while(1);




 

}
/**-------------------------------------------------------------------------**/









/*****************************************************************************/



no_room()
{
int stat=0;
int space_free;

/* load the error message and set the uv coords for the message */


do
   {
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		
   VSync(0);
   if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
      {
      if (PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
	      {
         bleep(3);

         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);
            alert_button_flash(AL1B_NO,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RU)!=0);
  	     // ClearOTag(cdb->ot, OTSIZE);		
         /* quit error processing */
	         		

         return 0;
         }


      if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
	      {
         bleep(3);
         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);
            alert_button_flash(AL1B_OK,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }

         while(PadKeyIsPressed(&buffer1,PAD_RD)!=0);

  	      ClearOTag(cdb->ot, OTSIZE);		
         cdb = (cdb==db)? db+1: db;			
	      ClearOTag(cdb->ot, OTSIZE);		
  
         bitmap_cd(POS_VARIFILENAME,1); /* various buttons and shit */
         bitmap_cd(POS_WAITFILENAME,1); /* the spinning icons       */ 
         bitmap_cd(POS_BACKFILENAME,2); /* the card backdrop        */

         space_free = delete_option();

         ClearOTag(cdb->ot, OTSIZE);		
         cdb = (cdb==db)? db+1: db;			
	      ClearOTag(cdb->ot, OTSIZE);		
  


         bitmap_cd(POS_GAICFILENAME,1);  
         bitmap_cd(POS_SAVEFILENAME,1);
         bitmap_cd(POS_MAINBACKFILENAME,2);
         return space_free;
         }
   
      }


   error_main_graphic();
   do_the_bar(bar_tab);
	//AddPrim(cdb->ot+4,&cdb->save_message); 	
	
   DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			

   }while(1);

 


}
/**-------------------------------------------------------------------------**/



/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
overwrite()
{
int stat=0;


/* load the error message and set the uv coords for the message */


do
   {
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

   VSync(0);
   if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
      {
      if (PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
	      {
         bleep(3);

          for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);
            alert_button_flash(AL3A_NO,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RU)!=0);
  	     // ClearOTag(cdb->ot, OTSIZE);		
         return 0;
         }


      if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
	      {
         bleep(3);
         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);
            alert_button_flash(AL3A_OK,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RD)!=0);
  	      //ClearOTag(cdb->ot, OTSIZE);		
         //cdb = (cdb==db)? db+1: db;			
	      //ClearOTag(cdb->ot, OTSIZE);		
         //cdb = (cdb==db)? db+1: db;			

         return 1;
 
        }
   
      }

   error_main_graphic();
   do_the_bar(bar_tab);


   DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			

   }while(1);

}
/**-------------------------------------------------------------------------**/









/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/

save_fail()
{
int stat=0;


/* load the error message and set the uv coords for the message */


load_error_message_tpage(FAILED);


do
   {
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

   VSync(0);
   if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
      {
      if (PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
	      {
         bleep(3);
          for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);

            alert_button_flash(AL4B_NO,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RU)!=0);
  	      ClearOTag(cdb->ot, OTSIZE);		
         return 0;
         }
      }

   error_main_graphic();
   do_the_bar(bar_tab);

   DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			

   }while(1);

}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/

save_ok()
{
int stat=0;

/* load the error message and set the uv coords for the message */
load_error_message_tpage(SAVE_OK);


do
   {
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

   VSync(0);
   if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
      {
      if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
	      {
         bleep(3);
          for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            error_main_graphic();
            do_the_bar(bar_tab);

            alert_button_flash(AL1A_OK,100-stat);
            DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         while(PadKeyIsPressed(&buffer1,PAD_RD)!=0);
  	      ClearOTag(cdb->ot, OTSIZE);		
         return 0;
         }
      }

   error_main_graphic();
   do_the_bar(bar_tab);

   DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			

   }while(1);

}
/**-------------------------------------------------------------------------**/















/**-------------------------------------------------------------------------**/
/**-load_game()-------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/*
load_game(char* file,int bytes_2_load,char* buffer)

   loads a "memory card save file" from cd into a buffer

   file           : name of file on memory card (assume port 0)
   bytes_2_load   : how much to load
   buffer         : destination
   returns        : 0 if fails else 1
*/

load_game(char* file,int bytes_2_load,char* buffer)
{
CdlFILE fp;       	
long fileLength;   	
int numSectors;   	
int mode = 0;     


//printf("loading file %s\n",file); 

if (!CdSearchFile(&fp,file))           
  		{
		printf("ERROR: could not find file\n");  
		return 0;
		}                               

fileLength = fp.size;     

//printf("filelength = %d\n",fileLength);  


if (!CdControlB(CdlSeekL,(unsigned char *)&fp.pos, 0))
  		{
		printf("ERROR: could not do seek\n");  
		return 0;
		}                               

numSectors = Sectors(fileLength);
/* printf("numsectors = %d\n",numSectors); */
mode |= CdlModeSpeed;

if(!CdRead(numSectors,(unsigned long *)buffer,mode))
  		{
		printf("ERROR: could not exectute read\n");  
		return 0;
		}                               

while (CdReadSync(1,0) > 0 );

//printf("loaded\n");
return 1;

}
/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-write_file_memcard() ---------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/*
write_file_memcard(char* file,int bytes_2_load,char* buffer)

   writes a "memory card save file" from buffer onto a card

   file           : name of file on memory card (assume port 0)
   bytes_2_load   : how much to write
   buffer         : source
   returns        : 0 if fails else 1
*/



write_file_memcard(char* file,int bytes_2_load,char* buffer)
{
int  fd;
char card_filename[21];

int i;

int operation=0;
int test =-1;
int isok =-15;
unsigned short temp;

strcpy(card_filename,"bu00:");
strcat(card_filename,file); 
printf("*************> file >> %s <<\n",card_filename);

if((fd=open(card_filename,O_WRONLY))>=0)
	{
	printf("warning: that file already exists \n");
	close(fd);

	}
else
if((fd=open(card_filename,O_CREAT|( (bytes_2_load/8192) <<16)))==-1)
	{	
	printf("warning: could not create file \n");
	close(fd);
	}

//printf("********>write begins!\n");
if((fd=open(card_filename,O_WRONLY))>=0)
	{												 
	i = write(fd,buffer,bytes_2_load);
	if(i!=bytes_2_load) 
		{
		printf("error: failed whilst writing data\n"); 
      printf("***************> i %d   bytes_2_load = %d\n",i,bytes_2_load);
		close(fd);
      delete(card_filename);
		return 0;
		}
 	close(fd);
   return i;
	}
else
	{
	printf("error: could not open previously created file!\n");
   delete(card_filename);
	return 0;
	}


}
/**-------------------------------------------------------------------------**/






/******************************************************************************/
/* static save screen with barchart                                           */
/******************************************************************************/

draw_save_screen(int bar_tab)
{
int counter =  0;

do{
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		
	AddPrim(cdb->ot+1,&cdb->save[0]); 	
	AddPrim(cdb->ot+1,&cdb->save[2]); 	
	AddPrim(cdb->ot+1,&cdb->save[4]); 	
	AddPrim(cdb->ot+1,&cdb->save[1]); 	
	AddPrim(cdb->ot+1,&cdb->save[3]); 	
	AddPrim(cdb->ot+3,&cdb->savebar[0]); 	
	AddPrim(cdb->ot+3,&cdb->savebar[1]); 	
   do_the_bar(bar_tab);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);
	DrawOTag(cdb->ot);	  			
   counter++;
	}while(counter<2);
}

/*****************************************************************************/



/******************************************************************************/
/* grow_bar                                                                   */
/******************************************************************************/
grow_bar(int start,int end)
{
do
   {
   draw_save_screen(start);
	DrawSync(0);						
	VSync(0);	
   start+=1;
   }while(start<end);
}









/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

alert_button_flash(int button,int bright)
{


bright+=128;


cdb->alert_button.y0 = 147;    
cdb->alert_button.y1 = 147;   
cdb->alert_button.y2 = 147+15;
cdb->alert_button.y3 = 147+15;


cdb->alert_button.v0 = 186;  
cdb->alert_button.v1 = 186;  
cdb->alert_button.v2 = 186+15;
cdb->alert_button.v3 = 186+15;

switch (button)
   {
   case AL1A_OK:   /* alert1 a  ok box */
      cdb->alert_button.x0 = 117;       
      cdb->alert_button.x1 = 117+15;    
      cdb->alert_button.x2 = 117;       
      cdb->alert_button.x3 = 117+15;    

      cdb->alert_button.y0 = 143;    
      cdb->alert_button.y1 = 143;   
      cdb->alert_button.y2 = 143+15;
      cdb->alert_button.y3 = 143+15;

      cdb->alert_button.u0 = 0;    
      cdb->alert_button.u1 = 0+15;   
      cdb->alert_button.u2 = 0;
      cdb->alert_button.u3 = 0+15;

      break;


   case AL1B_OK:   /* alert1 b  ok box */
   case AL2A_OK:   /* alert1 a  ok box */
   case AL2B_OK:   /* alert1 b  ok box */
   case AL3A_OK:   /* alert1 a  ok box */
   case AL3B_OK:   /* alert1 b  ok box */
      cdb->alert_button.x0 =162;       
      cdb->alert_button.x1 =162+15;    
      cdb->alert_button.x2 =162;       
      cdb->alert_button.x3 =162+15;    

      cdb->alert_button.u0 = 0;    
      cdb->alert_button.u1 = 0+15;   
      cdb->alert_button.u2 = 0;
      cdb->alert_button.u3 = 0+15;

      break;


   case AL1B_NO:   /* alert1 b  no box */
   case AL2A_NO:   /* alert2 a  no box */
   case AL2B_NO:   /* alert2 b  no box */
   case AL3A_NO:   /* alert3 a  no box */
   case AL3B_NO:   /* alert3 b  no box */
      cdb->alert_button.x0 =139;       
      cdb->alert_button.x1 =139+15;    
      cdb->alert_button.x2 =139;       
      cdb->alert_button.x3 =139+15;    

      cdb->alert_button.u0 = 16;   
      cdb->alert_button.u1 = 16+15;
      cdb->alert_button.u2 = 16;   
      cdb->alert_button.u3 = 16+15;


      break;


   case AL4A_NO:   /* alert4 a  no box */
      cdb->alert_button.x0 =120;       cdb->alert_button.y0 = 148;    
      cdb->alert_button.x1 =120+15;    cdb->alert_button.y1 = 148;   
      cdb->alert_button.x2 =120;       cdb->alert_button.y2 = 148+15;
      cdb->alert_button.x3 =120+15;    cdb->alert_button.y3 = 148+15;
 
      cdb->alert_button.u0 = 16;         
      cdb->alert_button.u1 = 16+15;      
      cdb->alert_button.u2 = 16;         
      cdb->alert_button.u3 = 16+15;      
      break;

   case AL4B_NO:   /* alert4 b  no box */
      cdb->alert_button.x0 =116;      cdb->alert_button.y0 = 148;    
      cdb->alert_button.x1 =116+15;   cdb->alert_button.y1 = 148;    
      cdb->alert_button.x2 =116;      cdb->alert_button.y2 = 148+15; 
      cdb->alert_button.x3 =116+15;   cdb->alert_button.y3 = 148+15; 

      cdb->alert_button.u0 = 16;    
      cdb->alert_button.u1 = 16+15;   
      cdb->alert_button.u2 = 16;
      cdb->alert_button.u3 = 16+15;
      break;


   default:
      break;
   }/*end switch*/

cdb->alert_button.r0 = bright;
cdb->alert_button.g0 = bright;
cdb->alert_button.b0 = bright;

AddPrim(cdb->ot+14,&cdb->alert_button);

}










/**----------------------------------------------------------------------**/
/**----------------------------------------------------------------------**/
/**----------------------------------------------------------------------**/
/*
load_error_message_tpage(error_type)

loads an error message from cd into the vram and set the uv coords for the
message

error_type  : 11 SAVE_OK         game saved ok
              12 CARD_FULL       card full
              21 NO_FORMAT       format       
              22 FORMAT_SURE     are you sure  (format)
              31 OVERWRITE       overwrite
              32 OVERWRITE_SURE  (overwrite)
              41 NO_CARD         no card         
              42 FAILED          save failed
*/


load_error_message_tpage(error_type)
{

switch(error_type)
   {
   /********************************************************************/
   case SAVE_OK:
      bitmap_cd(POS_ALERT1FILENAME,1);
      break;

   /********************************************************************/
   case CARD_FULL:
      bitmap_cd(POS_ALERT1FILENAME,1);
      break;

   /********************************************************************/
   case NO_FORMAT:
      bitmap_cd(POS_ALERT2FILENAME,1);
      break;

   /********************************************************************/
   case FORMAT_SURE:
      bitmap_cd(POS_ALERT2FILENAME,1);
      break;         
   /********************************************************************/

   case OVERWRITE:
      bitmap_cd(POS_ALERT3FILENAME,1);
      break;

   /********************************************************************/
   case OVERWRITE_SURE:
      bitmap_cd(POS_ALERT3FILENAME,1);
      break;

   /********************************************************************/
   case NO_CARD:
      bitmap_cd(POS_ALERT4FILENAME,1);
      break;

   /********************************************************************/
   case FAILED:
      bitmap_cd(POS_ALERT4FILENAME,1);
      break;

   /********************************************************************/

   default:
      break;
}/*end switch*/


if (error_type%2!=0)
   {
   /*odd number so use top box*/
   db[0].save_message.u0 = 0;	      db[0].save_message.v0 = 0;   
   db[0].save_message.u1 = 0+255;	db[0].save_message.v1 = 0;	  
   db[0].save_message.u2 = 0;	      db[0].save_message.v2 = 0+93;
   db[0].save_message.u3 = 0+255;	db[0].save_message.v3 = 0+93;
   db[1].save_message.u0 = 0;	      db[1].save_message.v0 = 0;   
   db[1].save_message.u1 = 0+255;	db[1].save_message.v1 = 0;	  
   db[1].save_message.u2 = 0;	      db[1].save_message.v2 = 0+93;
   db[1].save_message.u3 = 0+255;	db[1].save_message.v3 = 0+93;
   }
else
   {
   /*even number so use bottom box*/
   db[0].save_message.u0 = 0;	      db[0].save_message.v0 = 93;   
   db[0].save_message.u1 = 0+255;	db[0].save_message.v1 = 93;	  
   db[0].save_message.u2 = 0;	      db[0].save_message.v2 = 93+93;
   db[0].save_message.u3 = 0+255;	db[0].save_message.v3 = 93+93;
   db[1].save_message.u0 = 0;	      db[1].save_message.v0 = 93;   
   db[1].save_message.u1 = 0+255;	db[1].save_message.v1 = 93;	  
   db[1].save_message.u2 = 0;	      db[1].save_message.v2 = 93+93;
   db[1].save_message.u3 = 0+255;	db[1].save_message.v3 = 93+93;
   }
}

/**----------------------------------------------------------------------**/


/**----------------------------------------------------------------------**/
/**-- error_main_graphic() ----------------------------------------------**/
/**----------------------------------------------------------------------**/

error_main_graphic()
{
AddPrim(cdb->ot+1,&cdb->save[0]); 	
AddPrim(cdb->ot+1,&cdb->save[2]); 	
AddPrim(cdb->ot+1,&cdb->save[4]); 	
AddPrim(cdb->ot+1,&cdb->save[1]); 	
AddPrim(cdb->ot+1,&cdb->save[3]); 	
AddPrim(cdb->ot+3,&cdb->savebar[0]); 	
AddPrim(cdb->ot+3,&cdb->savebar[1]); 	
AddPrim(cdb->ot+5,&cdb->save_message); 	
}
/**----------------------------------------------------------------------**/

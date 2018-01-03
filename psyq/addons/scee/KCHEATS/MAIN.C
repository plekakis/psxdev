/*****************************************************************************/
/*****************************************************************************/
/** main.c ** for new memory card project thingy **   							 **/
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
#include "vsdie.h"

/**-------------------------------------------------------------------------**/

/**-------------------- display --------------------------------------------**/
DB	 db[2];		/* packet double buffer */
DB* cdb;			/* current db */

int timbuff[(256*256/2)+2048];
int backtimbuff[(512*256/2)+2048];

TIM   tim;

/**-------------------------------------------------------------------------**/


#define FORM1_SIZE     2048 			/* the size of a single cd sector*/			  
#define Sectors(x) ((x+FORM1_SIZE-1)/FORM1_SIZE) /* calculates the number of */
																 /* sectors required to load */
																 /* a number of bytes        */


#define DEMO_TIMEOUT 50*30   // every 30 seconds  

/**-------------------- sound ----------------------------------------------**/

unsigned char  vab_head_buff[8192];								
unsigned char  vab_body_buff[500000];								



/**-------------------- card -----------------------------------------------**/
unsigned long 	ev0,ev1,ev2,ev3,ev10,ev11,ev12,ev13;
struct	DIRENTRY card_dir[15];	

_CARD    card_header[15];

CDCARDBUFFER game_buff;


/**-------------------- pads-----------------------------------------------**/
ControllerPacket buffer1, buffer2;
/**-------------------------------------------------------------------------**/

FILEDATA filedata[] = 
   {
      {"\\TIM\\MAINBACK.TIM;1",0},  /* POS_MAINBACKFILENAME    0 */
      {"\\TIM\\DWARNING.TIM;1",0},  /* POS_DWARNINGFILENAME    1 */
      {"\\TIM\\FIRSTBCK.TIM;1",0},  /* POS_FIRSTBCKFILENAME    2 */
      {"\\TIM\\MCSCREEN.TIM;1",0},  /* POS_BACKFILENAME 	      3 */
      {"\\TIM\\ALPHA.TIM;1"   ,0},  /* POS_FONTFILENAME 	      4 */
      {"\\TIM\\GAMEICON.TIM;1",0},  /* POS_GAICFILENAME 	      5 */
      {"\\TIM\\ELEM.TIM;1"    ,0},  /* POS_VARIFILENAME 	      6 */
      {"\\TIM\\SAVE.TIM;1"    ,0},  /* POS_SAVEFILENAME 	      7 */
      {"\\TIM\\MAINSPRT.TIM;1",0},  /* POS_MSICFILENAME 	      8 */
      {"\\TIM\\WAITSPRT.TIM;1",0},  /* POS_WAITFILENAME 	      9 */
      {"\\ALERT\\ALERT1.TIM;1",0},  /* POS_ALERT1FILENAME      10 */
      {"\\ALERT\\ALERT2.TIM;1",0},  /* POS_ALERT2FILENAME      11 */
      {"\\ALERT\\ALERT3.TIM;1",0},  /* POS_ALERT3FILENAME      12 */
      {"\\ALERT\\ALERT4.TIM;1",0},  /* POS_ALERT4FILENAME      13 */
      {"\\TIM\\POWER1.TIM;1"  ,0},  /* POS_POWERLINE1 	      14 */
      {"\\TIM\\COPYRGHT.TIM;1",0},  /* POS_COPYRGHT    	      15 */
      {"\\TIM\\POWER2.TIM;1"  ,0},  /* POS_POWERLINE2 	      16 */
      {"\\STR\\DDERBY.STR;1"  ,0},  /* POS_DDERBY              17 */
      {"\\TIM\\FIRSTELM.TIM;1",0},  /* POS_FIRSTELMFILENAME    18 */
      {"\\TIM\\SONY.TIM;1"    ,0},  /* POS_SONYPRESENTS        19 */
      {"\\TIM\\START.TIM;1"   ,0},  /* POS_START               20 */
      {"\\STR\\ACOMBAT.STR;1" ,0},  /* POS_ACOMBAT             21 */
      {"\\STR\\KIVAN.STR;1"   ,0},  /* POS_KRAZY               22 */
      {"\\STR\\LEMM3D.STR;1"  ,0},  /* POS_LEMM3D              23 */
      {"\\STR\\TEKKEN.STR;1"  ,0},  /* POS_TEKKEN              24 */
      {"\\STR\\WIPEOUT.STR;1" ,0},  /* POS_WIPEOUT             25 */
      {"\\SND\\VH.SND;1"      ,0},  /* POS_VH                  26 */
      {"\\SND\\VB.SND;1"      ,0},  /* POS_VB                  27 */
      {"\\TIM\\CREDIT.TIM;1"  ,0},  /* POS_CREDIT              28 */
      {"\\TIM\\POWER3.TIM;1"  ,0},  /* POS_POWERLINE3 	      29 */
      {"\\STR\\RRACER.STR;1"  ,0},  /* POS_RRACER              30 */
      {"\\STR\\DWORLD.STR;1"  ,0},  /* POS_DWORLD              31 */
      {"\\STR\\KBLOOD.STR;1"  ,0},  /* POS_KBLOOD              32 */
      {"\\STR\\JFLASH.STR;1"  ,0}   /* POS_JFLASH              33 */
   };


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/

main()
{
int piss=0;
int mode_sel=0;
int isok=0;
int exit_flag=0;




ResetCallback();	

// _EX_Init();		/* Install the Exception Handler */

CdInit();           
init_graph();

InitCARD(0);
StartCARD();
_bu_init();

init_cards();

initialise_sound();


/*
printf(" memory allocation\n");
printf(" *****************\n");
printf(" ram size    = %8d 0x%8x\n",_ramsize  ,_ramsize);
printf(" stack size  = %8d 0x%8x\n",_stacksize,_stacksize);
printf(" heapbase    = %8d 0x%8x\n",(80200000-__heapbase),__heapbase);
printf(" *****************\n");
*/


cache_file_pos(); /** cache the cdsearch files for use later **/


printf("loading sound files....\n");
load_cd("\\SND\\VB.SND;1",(unsigned long*)&vab_body_buff[0]);			/*load the head*/
load_cd("\\SND\\VH.SND;1",(unsigned long*)&vab_head_buff[0]);			/*load the head*/


printf("xfering sound data....\n");
load_vab(&vab_head_buff[0],&vab_body_buff[0]);



initPAD(&buffer1,MAX_CONTROLLER_BYTES,&buffer2,MAX_CONTROLLER_BYTES);
StartPAD();

ChangeClearPAD(0);



/** create dummy tim work area for loading card icons to vram for display **/
init_tim(&tim);	


/** load the piracy warning screen **/

PutDispEnv(&db[1].disp); 		
PutDrawEnv(&db[0].draw); 		
bitmap_cd(POS_DWARNINGFILENAME,2);  
VSync(0);
draw_backdrop( (unsigned long*)&backtimbuff[0]); 


bitmap_cd(POS_FONTFILENAME,1);
bitmap_cd(POS_VARIFILENAME,1);


bitmap_cd(POS_SONYPRESENTS,2);  
VSync(0);
draw_backdrop( (unsigned long*)&backtimbuff[0]); 


/** load the graphics needed to proceed **/

bitmap_cd(POS_MSICFILENAME,1);
bitmap_cd(POS_FIRSTELMFILENAME,1);
bitmap_cd(POS_FIRSTBCKFILENAME,2);

init_prim(&db[0]);
init_prim(&db[1]);

init_text_prim(&db[0]);
init_text_prim(&db[1]);

init_save_prim(&db[0]);
init_save_prim(&db[1]);



bitmap_cd(POS_START,2);  

VSync(0);
bleep(6);
draw_backdrop( (unsigned long*)&backtimbuff[0]); 


exit_flag=0;
do{
   if ( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
		{
		if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
		   {
         exit_flag=69;
         }

    	if ( PadKeyIsPressed(&buffer1,PAD_START)!=0) 
		   {
         exit_flag=69;
         }
      }
   }while(exit_flag==0);


bleep(1);

VSync(0); VSync(0); VSync(0);
VSync(0); VSync(0); VSync(0);
VSync(0); VSync(0); VSync(0);
VSync(0); VSync(0); VSync(0);
VSync(0); VSync(0); VSync(0);
VSync(0); VSync(0); VSync(0);
VSync(0); VSync(0); VSync(0);
VSync(0); VSync(0); VSync(0);
VSync(0); VSync(0); VSync(0);
VSync(0); VSync(0); VSync(0);


bitmap_cd(POS_FIRSTBCKFILENAME,2);

/** make the screen go black for a while**/
/*cdb = (cdb==db)? db+1: db;			
ClearOTag(cdb->ot, OTSIZE);		
DrawSync(0);						
VSync(0);							
PutDrawEnv(&cdb->draw); 		
PutDispEnv(&cdb->disp); 		
DrawOTag(cdb->ot);	  			

cdb = (cdb==db)? db+1: db;			
ClearOTag(cdb->ot, OTSIZE);		

DrawSync(0);						
VSync(0);							
PutDrawEnv(&cdb->draw); 		
PutDispEnv(&cdb->disp); 		
DrawOTag(cdb->ot);	  			


for(piss=0; piss<25; piss++)
   {
   VSync(2);
   }

*/


mode_sel=0;


/** test loop for deletion module **/

do{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

	if ( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD && isok>0) 
		{
		if ( PadKeyIsPressed(&buffer1,PAD_LU)!=0) 
		   {

         mode_sel--;
         if (mode_sel<0)  
            {
            mode_sel=0;
            }
         else
            {
            bleep(1);
            }
         isok=-10;
 		   }
	
		if ( PadKeyIsPressed(&buffer1,PAD_LD)!=0) 
		   {

         mode_sel++;
         if (mode_sel>2)  
            {
            mode_sel=2;
            }
         else
            {
            bleep(1);
            }
         isok=-10;
 		   }
	
		if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
		   {
         bleep(4);

         switch (mode_sel)
            {
            case 2:      /* file manager  */
               fade_out_first_menu(-2,0);

               bitmap_cd(POS_VARIFILENAME,1); /* various buttons and shit */
               bitmap_cd(POS_WAITFILENAME,1); /* the spinning icons       */ 
               bitmap_cd(POS_BACKFILENAME,2); /* the card backdrop        */
		         delete_option();

               bitmap_cd(POS_GAICFILENAME,1); /* re load this      */ 
   	         bitmap_cd(POS_FIRSTELMFILENAME,1);
               bitmap_cd(POS_FIRSTBCKFILENAME,2);  /* main menu    */

               fade_out_first_menu(-2,1);

               break;
            
            case 1:      /* power line          */
               fade_out_first_menu(-2,0);
               power_line(1);

               VSync(0);
               SetDefDrawEnv(&db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
               SetDefDispEnv(&db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);

               SetDefDrawEnv(&db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
               SetDefDispEnv(&db[1].disp, 0,   0,       FRAME_X, FRAME_Y);

               db[1].disp.screen.x = db[0].disp.screen.x = SCREEN_X;
               db[1].disp.screen.y = db[0].disp.screen.y = SCREEN_Y;

               db[1].disp.screen.h = db[0].disp.screen.h = PAL_MAGIC;
               db[1].disp.screen.w = db[0].disp.screen.w = PAL_MAGIC;
         
               db[0].draw.dfe = db[1].draw.dfe = 0;

               ClearOTag(cdb->ot, OTSIZE);	 	
       	      PutDrawEnv(&cdb->draw); 		
	            PutDispEnv(&cdb->disp); 		
   	         bitmap_cd(POS_FIRSTELMFILENAME,1);
               bitmap_cd(POS_FIRSTBCKFILENAME,2);  /* main menu    */
               fade_out_first_menu(-2,1);

               break;

 
            case 0:      /* kc */ 
               fade_out_first_menu(-2,0);

               bitmap_cd(POS_GAICFILENAME,1);      /*new!*/
               bitmap_cd(POS_MSICFILENAME,1);
               bitmap_cd(POS_MAINBACKFILENAME,2);   

               menu_test();
          //     power_line(2);

          //     VSync(0);
          //     SetDefDrawEnv(&db[0].draw, 0,   0,       FRAME_X, FRAME_Y);
          //    SetDefDispEnv(&db[0].disp, 0,   FRAME_Y, FRAME_X, FRAME_Y);

          //     SetDefDrawEnv(&db[1].draw, 0,   FRAME_Y, FRAME_X, FRAME_Y);
          //     SetDefDispEnv(&db[1].disp, 0,   0,       FRAME_X, FRAME_Y);

          //     db[1].disp.screen.x = db[0].disp.screen.x = SCREEN_X;
          //     db[1].disp.screen.y = db[0].disp.screen.y = SCREEN_Y;

          //     db[1].disp.screen.h = db[0].disp.screen.h = PAL_MAGIC;
          //     db[1].disp.screen.w = db[0].disp.screen.w = PAL_MAGIC;
         
          //     db[0].draw.dfe = db[1].draw.dfe = 0;

          //     ClearOTag(cdb->ot, OTSIZE);	 	
       	 //     PutDrawEnv(&cdb->draw); 		
	       //     PutDispEnv(&cdb->disp); 		

   	      //   bitmap_cd(POS_FIRSTELMFILENAME,1);

               bitmap_cd(POS_FIRSTBCKFILENAME,2);   
               bitmap_cd(POS_FIRSTELMFILENAME,1);
               fade_out_first_menu(-2,1);
               break;
 
            default:
               break;
            }
         }
		}
  
   menu(mode_sel);


   isok++;

   if( isok>DEMO_TIMEOUT )
      {
      isok = -15;
      fade_out_first_menu(-2,0);

      rand_demo();

      bitmap_cd(POS_FIRSTBCKFILENAME,2);   
      bitmap_cd(POS_FIRSTELMFILENAME,1);
      fade_out_first_menu(-2,1);


      }
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]); 
	DrawOTag(cdb->ot);	  			
}while(1);






}



/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
display_clear(int xpos,int ypos,int width,int height,int r,int g,int b)
{
static RECT bg;			

bg.x=xpos;
bg.y=ypos;
bg.w=width;
bg.h=height;

FntFlush(-1);
ClearImage(&bg, r, g, b);
}
/**--------------------------------------------------------------------------**/





/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
cache_file_pos()
{
int i;
CdlFILE fp;       	

//printf("caching CD Search files\n");
//printf("^^^^^^^^^^^^^^^^^^^^^^^\n");

for(i=0; i<NUM_FILES; i++)
   {
   //printf("\n\n file %d\n");
   if (CdSearchFile(&filedata[i].fp,&filedata[i].cd_filename[0])==0)
      {
      printf("       > %s NOT FOUND\n",filedata[i].cd_filename[0]);
      }

   //printf(" file %d size %d ",i,filedata[i].fp.size);
   //printf(" name %s \n",filedata[i].fp.name);
   //printf(" cd   %s\n",filedata[i].fp.name); 
   //printf("\n\n");

   }

//printf("\n\n\n\n");


}
/**--------------------------------------------------------------------------**/


/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
menu(int mode_sel)
{
static flasher = 128;
static inc=8;

switch(mode_sel)
   {
   case 0:
      cdb->first_icon.x0 = 19;      cdb->first_icon.y0 = 33;
      cdb->first_icon.x1 = 19+63;   cdb->first_icon.y1 = 33;
      cdb->first_icon.x2 = 19; 	   cdb->first_icon.y2 = 33+63;
      cdb->first_icon.x3 = 19+63;	cdb->first_icon.y3 = 33+63;

      cdb->first_icon.u0 = 0;       cdb->first_icon.v0 = 93;
      cdb->first_icon.u1 = 0+63;    cdb->first_icon.v1 = 93;
      cdb->first_icon.u2 = 0; 	   cdb->first_icon.v2 = 93+63;
      cdb->first_icon.u3 = 0+63;	   cdb->first_icon.v3 = 93+63;

      cdb->first_name.x0 = 86;      cdb->first_name.y0 = 50;
      cdb->first_name.x1 = 86+231;  cdb->first_name.y1 = 50;
      cdb->first_name.x2 = 86; 	   cdb->first_name.y2 = 50+30;
      cdb->first_name.x3 = 86+231;	cdb->first_name.y3 = 50+30;

      cdb->first_name.u0 = 0;       cdb->first_name.v0 = 0;
      cdb->first_name.u1 = 0+231;   cdb->first_name.v1 = 0;
      cdb->first_name.u2 = 0;       cdb->first_name.v2 = 0+30;
      cdb->first_name.u3 = 0+231;   cdb->first_name.v3 = 0+30;

      break;
      
    
   case 1: 
      cdb->first_icon.x0 = 19;      cdb->first_icon.y0 = 93;
      cdb->first_icon.x1 = 19+63;   cdb->first_icon.y1 = 93;
      cdb->first_icon.x2 = 19; 	   cdb->first_icon.y2 = 93+63;
      cdb->first_icon.x3 = 19+63;	cdb->first_icon.y3 = 93+63;

      cdb->first_icon.u0 = 64;      cdb->first_icon.v0 = 93;
      cdb->first_icon.u1 = 64+63;   cdb->first_icon.v1 = 93;
      cdb->first_icon.u2 = 64; 	   cdb->first_icon.v2 = 93+63;
      cdb->first_icon.u3 = 64+63;	cdb->first_icon.v3 = 93+63;
 
      cdb->first_name.x0 = 86;      cdb->first_name.y0 = 109;
      cdb->first_name.x1 = 86+231;  cdb->first_name.y1 = 109;
      cdb->first_name.x2 = 86; 	   cdb->first_name.y2 = 109+30;
      cdb->first_name.x3 = 86+231;	cdb->first_name.y3 = 109+30;

      cdb->first_name.u0 = 0;       cdb->first_name.v0 = 31;
      cdb->first_name.u1 = 0+231;   cdb->first_name.v1 = 31;
      cdb->first_name.u2 = 0; 	   cdb->first_name.v2 = 31+30;
      cdb->first_name.u3 = 0+231;	cdb->first_name.v3 = 31+30;

     break;

   case 2:
      cdb->first_icon.x0 = 19;      cdb->first_icon.y0 = 153;
      cdb->first_icon.x1 = 19+63;   cdb->first_icon.y1 = 153;
      cdb->first_icon.x2 = 19; 	   cdb->first_icon.y2 = 153+63;
      cdb->first_icon.x3 = 19+63;	cdb->first_icon.y3 = 153+63;

      cdb->first_icon.u0 = 128;     cdb->first_icon.v0 = 93;
      cdb->first_icon.u1 = 128+63;  cdb->first_icon.v1 = 93;
      cdb->first_icon.u2 = 128; 	   cdb->first_icon.v2 = 93+63;
      cdb->first_icon.u3 = 128+63;	cdb->first_icon.v3 = 93+63;
  
      cdb->first_name.x0 = 86;      cdb->first_name.y0 = 169;
      cdb->first_name.x1 = 86+231;  cdb->first_name.y1 = 169;
      cdb->first_name.x2 = 86; 	   cdb->first_name.y2 = 169+30;
      cdb->first_name.x3 = 86+231;	cdb->first_name.y3 = 169+30;

      cdb->first_name.u0 = 0;       cdb->first_name.v0 = 62;
      cdb->first_name.u1 = 0+231;   cdb->first_name.v1 = 62;
      cdb->first_name.u2 = 0; 	   cdb->first_name.v2 = 62+30;
      cdb->first_name.u3 = 0+231;	cdb->first_name.v3 = 62+30;

    break;

   default:
      break;
   }/*end switch*/


flasher+=inc;

if (flasher>128+50) inc=-8;
if (flasher<128-50) inc= 8;

cdb->first_name.r0= flasher;
cdb->first_name.g0= flasher;
cdb->first_name.b0= flasher;




AddPrim(cdb->ot+3,&cdb->first_icon); 
AddPrim(cdb->ot+3,&cdb->first_name); 


}
/**--------------------------------------------------------------------------**/



/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
fade_out_first_menu(int icon_selected,int fader)
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

   AddPrim(cdb->ot+1,&cdb->first_icon); 
   AddPrim(cdb->ot+1,&cdb->first_name); 

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



/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
load_cd(char* filename,unsigned long* buffer)
{

CdlFILE 	fp;       			/* file pointer */
long 		file_length;   		/* length of file in bytes */
int 		num_sectors;   		/* whole number of secotors required */
int 		mode = 0;     		/* cd mode                           */
long 		counter;			

u_long* 	addr;					/* pointer to an address in main ram */


printf("loading file %s\n",filename);

/* return file location and size from file name */

if (!CdSearchFile(&fp,filename))           	
  		{
		printf("ERROR: could not find file\n");  
		exit(0);
		}                               

/* blocking seek to start of file */
if (!CdControlB(CdlSeekL,(unsigned char *)&fp.pos, 0))
  		{
		printf("ERROR: could not do seek\n");  
		exit(0);
		}                               

/* calculate the number of whole sectors (2k) that need to be loaded */
/* in order to load the whole file                                   */

file_length = fp.size;     
num_sectors = Sectors(file_length);

printf("File size %d bytes 	sectors %d\n",file_length,num_sectors);

mode |= CdlModeSpeed;	 /* double speed */


/*set the address where the file will be loaded to*/
addr=(unsigned long *)buffer;

/*execute the read */
if(!CdRead(num_sectors,addr,mode))
  		{
		printf("ERROR: could not exectute read\n");  
		exit(0);
		}                               


/*wait for the read command to complete*/
do
	{
   printf("*");
	/*this is a wait loop, you could do anything here..... eggtimer ?*/
	}
	while (CdReadSync(1,0) > 0 );



}








/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

rand_demo()
{
int the_X_factor;

the_X_factor = rand()%4;


printf("  random call: ");

switch (the_X_factor)
   {
   case 0:
      // display powerline screens 
      printf("************************************power line\n");
      spower_line(0); // power line 1
      spower_line(1); // power line 2
      spower_line(4); // power line 2
      break;

   case 1:
      printf("************************************copyright\n");
      spower_line(2); // copyright screen
      break;

   case 2:
      printf("************************************credit\n");
      spower_line(3); // copyright screen
      break;

   default:
      printf("************************************credit\n");
      spower_line(3); // copyright screen
      break;

      break;
   }




}
/**------------------------------------------------------------------------**/



/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
spower_line(int pic)
{
int i,j,c;

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
      bitmap_cd(POS_START,2);
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

   if(isok> (50*5) )    /*display for 5 seconds */
      {
      bleep(4);
      power_fade(0);
      return 1;
      }

   isok++;     
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]);  
	DrawOTag(cdb->ot);	  			
   }while(1);
}







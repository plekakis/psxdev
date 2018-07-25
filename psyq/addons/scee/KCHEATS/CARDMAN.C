/****************************************************************************/
/*****************************************************************************/
/** cardman.c ** for new memory card project thingy **   	    				 **/
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


/**------- make this the header file ---------------------------------**/

/*#define DELETE 1
#define QUIT   2
#define FORMAT 3

#define DELETE    1
#define OP_FAILED 2
#define SURE      3


#define YES_BUTTON 1
#define NO_BUTTON  2

#define DELETE_BUTTON  3
#define QUIT_BUTTON    4
#define FORMAT_BUTTON  5 */


/**------- header file ends-------------------------------------------**/


/*
extern unsigned long _ramsize;  
extern unsigned long _stacksize;
extern unsigned long __heapbase; 
*/



extern ControllerPacket buffer1, buffer2;


extern struct	DIRENTRY card_dir[15];	


extern _CARD    card_header[15];

extern DB	  db[2];		/* packet double buffer */
extern DB*    cdb;			/* current db */


extern int timbuff[];
extern int backtimbuff[];




extern TIM   tim;




/**--------------------------------------------------------------------**/
/**----------------- vcounter -----------------------------------------**/
/**--------------------------------------------------------------------**/
void eggy()
{
static int xtex[] = {  0, 50,100,150,200,  0, 50,100,150,200,  0, 50,100,150,200,  0, 50,100,150,200,  0, 50,100,150,200};
static int ytex[] = {  0,  0,  0,  0,  0, 50, 50, 50, 50, 50,100,100,100,100,100,150,150,150,150,150,200,200,200,200,200};
static int i=0;

i++;

cdb->wait_sprite.u0 = xtex[i];     cdb->wait_sprite.v0 = ytex[i];
cdb->wait_sprite.u1 = xtex[i]+49;  cdb->wait_sprite.v1 = ytex[i];
cdb->wait_sprite.u2 = xtex[i];     cdb->wait_sprite.v2 = ytex[i]+49;
cdb->wait_sprite.u3 = xtex[i]+49;  cdb->wait_sprite.v3 = ytex[i]+49;

if (i==25) i=-1;


DrawPrim(&cdb->wait_sprite);
cdb = (cdb==db)? db+1: db;			
PutDrawEnv(&cdb->draw); 		
PutDispEnv(&cdb->disp); 		
draw_backdrop( (unsigned long*)&backtimbuff[0]); 
   
//pollhost();
} 
/**--------------------------------------------------------------------**/








/**-------------------------------------------------------------------------**/
/** load enough data from the card to main ram so can display the icons and **/
/** know the block sizes                                                    **/
/**-------------------------------------------------------------------------**/

load_card(int port,struct DIRENTRY *d,long files,TIM *tim)
{
int 	ret,i;
int 	slots_free;
RECT	rect1;


int x,y;
slots_free=15;


/*printf("Loading Card\n");*/


/*clearout the tim */
for(y=0; y<256; y++)
	{
	for(x=0; x<256; x++)
		{
	 	tim->tim_struct.pixel[x][y]=0;
		}
	}



for(i=0;i<files;i++) 
	{
	slots_free -= load_icon(port,d[i].name,i,15-slots_free,tim);
	Vsync(0);
	}

rect1.x = tim->tim_struct.dx;
rect1.y = tim->tim_struct.dy;
rect1.w = tim->tim_struct.w;
rect1.h = tim->tim_struct.h;

printf(" rect %d %d %d %d \n",rect1.x,rect1.y,rect1.w,rect1.h);

ClearImage(&rect1,200,0,0);

LoadImage(&rect1,&tim->tim_struct.pixel[0][0]); 
DrawSync(0);
}
/**-------------------------------------------------------------------------**/



/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/



load_icon(int port,char* file,int fileno,int slots_used,TIM* tim)
{
int 			fd;
GsIMAGE 		TexInfo;

char card_filename[30];


int xcounter,ycounter;
int i;
int sc;
unsigned short* sjispoint; 

typedef struct
	{
	int high;
	int low;
	}CHARS;

typedef union 
		{
		unsigned short kanjichar;
		CHARS asciichars;
		}CONVERSION_BUFFER;

CONVERSION_BUFFER con_buff;



_CARD card_header_buffer;




strcpy(card_filename,"bu00:");
strcat(card_filename,file); 

printf("debug files :%s\n",card_filename);

if((fd=open(card_filename,O_RDONLY))>=0)
	{     
  	if( read(fd,(char *)&card_header_buffer,sizeof(_CARD))!=sizeof(_CARD))
      {
      /* the file read failed so bail */
	   close(fd);
      return 0;
      }
	close(fd);
	}
else
	{
   return 0;
	}


card_header[fileno].Magic[0] 	  = card_header_buffer.Magic[0];
card_header[fileno].Magic[1] 	  = card_header_buffer.Magic[1];
card_header[fileno].Type 	  	  = card_header_buffer.Type;
card_header[fileno].BlockEntry  = card_header_buffer.BlockEntry;

/*for(i=0; i<32; i++)
	{
	card_header[fileno].Title[2*i  ]=card_header_buffer.Title[2*i];
	card_header[fileno].Title[2*i+1]=card_header_buffer.Title[2*i+1];
	}*/

for(i=0; i<64; i++)
	{
	card_header[fileno].Title[i]=card_header_buffer.Title[i];
	}





for(sc=0; sc<card_header[fileno].BlockEntry; sc++)
	{
	for(ycounter=0; ycounter<16; ycounter++)
		{
		for(xcounter=0; xcounter<8; xcounter+=1)
			{

			tim->tim_struct.pixel[(0*16)+ycounter][((slots_used+sc)*16)+xcounter*2]   = card_header_buffer.Clut[ card_header_buffer.Icon[0][ycounter][xcounter]&0x0f   ];
  	  		tim->tim_struct.pixel[(0*16)+ycounter][((slots_used+sc)*16)+xcounter*2+1] = card_header_buffer.Clut[ card_header_buffer.Icon[0][ycounter][xcounter]>>4     ];

  	  		tim->tim_struct.pixel[(0*16)+ycounter][((slots_used+sc)*16)+xcounter*2] = tim->tim_struct.pixel[(0*16)+ycounter][((slots_used+sc)*16)+xcounter*2] | 0x8000;
  	  		tim->tim_struct.pixel[(0*16)+ycounter][((slots_used+sc)*16)+xcounter*2+1] = tim->tim_struct.pixel[(0*16)+ycounter][((slots_used+sc)*16)+xcounter*2+1] | 0x8000;

  
			tim->tim_struct.pixel[(1*16)+ycounter][((slots_used+sc)*16)+xcounter*2]   = card_header_buffer.Clut[ card_header_buffer.Icon[1][ycounter][xcounter]&0x0f   ];
  	  		tim->tim_struct.pixel[(1*16)+ycounter][((slots_used+sc)*16)+xcounter*2+1] = card_header_buffer.Clut[ card_header_buffer.Icon[1][ycounter][xcounter]>>4     ];

			tim->tim_struct.pixel[(2*16)+ycounter][((slots_used+sc)*16)+xcounter*2]   = card_header_buffer.Clut[ card_header_buffer.Icon[2][ycounter][xcounter]&0x0f   ];
  	  		tim->tim_struct.pixel[(2*16)+ycounter][((slots_used+sc)*16)+xcounter*2+1] = card_header_buffer.Clut[ card_header_buffer.Icon[2][ycounter][xcounter]>>4     ];
				
			}
	  	}
	}

return card_header_buffer.BlockEntry;
}








/**-------------------------------------------------------------------------**/
/**-- draw_icons                                                          --**/
/**-------------------------------------------------------------------------**/

draw_icons(int card,int xpos,int ypos,_CARD* card_head,int sel)
{
int slotcounter=0;
int xcounter=0;
int ycounter=0;

int selected_slots=0;
int blocks_used=0;
int i=0;
int j=0;
int dave;
int card_files=0;


int icon_count=0;

int counter;


selected_slots = card_head[sel].BlockEntry;
slotcounter=0;



for (i=0; i<15; i++)	 /* lazy lazy lazy */
	{
	if(i==sel)
		{
		dave = icon_count+card_head[i].BlockEntry;
		for (j=icon_count; j<dave; j++)
			{	
			/*printf(" j %d limit %d \n",j,icon_count+card_1_header[i].BlockEntry);*/
			flasher(&db[0].card_1_slot[j]);
			flasher(&db[1].card_1_slot[j]);
			icon_count++;
			}
		}
	else
		{
		dave = icon_count+card_head[i].BlockEntry;
		for (j=icon_count; j<dave; j++)
			{
			fade(&db[0].card_1_slot[j],80,80,80);
			fade(&db[1].card_1_slot[j],80,80,80);

			icon_count++;
  			}
  		}
	}


for(ycounter=0; ycounter<3; ycounter++)
	{
	for(xcounter=0; xcounter<5; xcounter++)
		{
		/* set the position of the slot icons */

		cdb->slot_border[slotcounter].x0 =xpos -1 + (xcounter*(4+18));	 	cdb->slot_border[slotcounter].y0 = ypos -1 + (ycounter*(3+18));	 	  
		cdb->slot_border[slotcounter].x1 =xpos -1 + (xcounter*(4+18))+18; cdb->slot_border[slotcounter].y1 = ypos -1 + (ycounter*(3+18));	  
		cdb->slot_border[slotcounter].x2 =xpos -1 + (xcounter*(4+18));	   cdb->slot_border[slotcounter].y2 = ypos -1 + (ycounter*(3+18))+18;
		cdb->slot_border[slotcounter].x3 =xpos -1 + (xcounter*(4+18))+18;	cdb->slot_border[slotcounter].y3 = ypos -1 + (ycounter*(3+18))+18;
		AddPrim(cdb->ot+2,&cdb->slot_border[slotcounter]); /* draw the slot icon */
	
		cdb->card_1_slot[slotcounter].x0 =xpos+ (xcounter*(6+16));	 	cdb->card_1_slot[slotcounter].y0 = ypos+ (ycounter*(5+16));	 	  
		cdb->card_1_slot[slotcounter].x1 =xpos+ (xcounter*(6+16))+16;  cdb->card_1_slot[slotcounter].y1 = ypos+ (ycounter*(5+16));	  
		cdb->card_1_slot[slotcounter].x2 =xpos+ (xcounter*(6+16));	   cdb->card_1_slot[slotcounter].y2 = ypos+ (ycounter*(5+16))+16;
		cdb->card_1_slot[slotcounter].x3 =xpos+ (xcounter*(6+16))+16;	cdb->card_1_slot[slotcounter].y3 = ypos+ (ycounter*(5+16))+16;
		AddPrim(cdb->ot+3,&cdb->card_1_slot[slotcounter]); /* draw the slot icon */
		slotcounter++;
		}
	}



}






/******************************************************************************/
/* this function bright flashes an ft4 */
/******************************************************************************/

flasher(POLY_FT4* ft4)
{
static flash_inc=-10;


if(ft4->r0 < 80  )
	{
	ft4->r0 = 80; 
	ft4->g0 = 80;
	ft4->b0 = 80;
	flash_inc = 10;
	}


if(ft4->r0 > 210 )
	{
	ft4->r0 = 210; 
	ft4->g0 = 210;
	ft4->b0 = 210;
	flash_inc =-10;
	}	


ft4->r0 +=flash_inc;
ft4->g0 +=flash_inc;
ft4->b0 +=flash_inc;



}	




/**-------------------------------------------------------------------------**/
/** fade sets the rgb component of a poly                                 --**/
/**-------------------------------------------------------------------------**/

fade(POLY_FT4* ft4,char r,char g,char b)
{
ft4->r0 = r;
ft4->g0 = g;
ft4->b0 = b;
}	
/**-------------------------------------------------------------------------**/




/**-------------------------------------------------------------------------**/
/**-- format_card                                                         --**/
/**-------------------------------------------------------------------------**/
format_card()
{
int isok=-15;
int stat=0;
int card_status;

do
{
printf("format routine\n");
cdb = (cdb==db)? db+1: db;			
ClearOTag(cdb->ot, OTSIZE);		

if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD && isok>0 ) 
   {
   if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
	   {  /*quit*/
      bleep(3);
      for(stat=0; stat<100; stat+=5)
         {
	      cdb = (cdb==db)? db+1: db;			
	      ClearOTag(cdb->ot, OTSIZE);		
         card_message(SURE,128);
         dave_text2("Format Memory card ?",31,229); 
         confirm_button_flash(NO_BUTTON,100-stat);
         DrawSync(0);						
         VSync(0);							
	      PutDrawEnv(&cdb->draw); 		
	      PutDispEnv(&cdb->disp); 		
	      draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	      DrawOTag(cdb->ot);	   
         }
	   ClearOTag(cdb->ot, OTSIZE);		
      while (PadKeyIsPressed(&buffer1,PAD_RU)!=0);
      return 0;  
      }   

   if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
	   {  /*quit*/
      bleep(3);

      for(stat=0; stat<100; stat+=5)
         {
	      cdb = (cdb==db)? db+1: db;			
	      ClearOTag(cdb->ot, OTSIZE);		
         card_message(SURE,128);
         dave_text2("Format Memory card ?",31,229); 
         confirm_button_flash(YES_BUTTON,100-stat);
         DrawSync(0);						
         VSync(0);							
	      PutDrawEnv(&cdb->draw); 		
	      PutDispEnv(&cdb->disp); 		
	      draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	      DrawOTag(cdb->ot);	   
         }
  	   ClearOTag(cdb->ot, OTSIZE);		
      while (PadKeyIsPressed(&buffer1,PAD_RD)!=0);


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
isok++;

card_message(SURE,128);
dave_text2("Format Memory card ?",31,229); 


DrawSync(0);						
VSync(0);							
PutDrawEnv(&cdb->draw); 		
PutDispEnv(&cdb->disp); 		
draw_backdrop( (unsigned long*)&backtimbuff[0]); 
DrawOTag(cdb->ot);	   
}while(1);




}
/**-------------------------------------------------------------------------**/








/**------------------------------------------------------------------------**/
/**----------- delete option ----------------------------------------------**/
/**------------------------------------------------------------------------**/
delete_option()
{
int j;
int i;

int card_status;
int old_card_status;

int card_files; 						/* the number of files on the card     */
int old_card_files;               /* the old number of files on the card */

int old_card_test=-69;
int card_test=0;

int delete_status; /* did the content of the card change whilst in the delete_file module*/

int   selected=0;
int   isok =-15;
int   valid_string = 0;
char  title_buffer[64]="  ";
unsigned short temp;
int stat;

int xcounter,ycounter,slotcounter;
int format_active=FALSE;
int delete_active=FALSE;
int space_left,di;


card_files =0;
old_card_status=0;



/************************ fade the screen up ********************************/
i=255;

fade_up(&db[0].screen_fade_mask,i);
fade_up(&db[1].screen_fade_mask,i);


//bitmap_cd(POS_WAITFILENAME,1);  
//bitmap_cd(POS_BACKFILENAME,2);

do{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

   fade_up(&cdb->screen_fade_mask,i);
   i-=6;

	FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	DrawOTag(cdb->ot);	  			
  
}while(i>0); 
/*****************************************************************************/






do{
   card_test = card_present_status(0x00); /*do a quick check !!!*/
   if(card_test!=old_card_test && card_test!=TIMEOUT)         /*if something has changed investigate*/
      {
      VSyncCallBack(eggy);
      ChangeClearPAD(0);
      card_status = status(PORT0); 
      if(card_status ==IOE)             /*if the card is ready load the icons */
         {
         VSyncCallBack(eggy);
         ChangeClearPAD(0);
		   card_files = dir_file(&card_dir[0]);
         strcpy(&title_buffer[0],"   ");
         load_card(PORT0,&card_dir[0],card_files,&tim);
         if (card_files==0) selected =-1; else selected =0;
         VSyncCallBack(0);
         if(!card_present_status(0x00)) card_status=TIMEOUT;
         }

      if(card_files!=0)
         {
         delete_active=TRUE;
         format_active=FALSE;
         }
      else
         {
         strcpy(&title_buffer[0],"   ");
         delete_active=FALSE;
         format_active=FALSE;
         }
     
      }
   else
      if(card_status==TIMEOUT)
      {
      VSync(0);  /* else controller returns shitty packet. COOL!*/
      selected =-1;
      delete_active=FALSE;
      format_active=FALSE;
      }

   else
      if(card_status==NEWCARD)
      {
      VSync(0);  /* else controller returns shitty packet. COOL!*/
      selected =-1;

      format_active=TRUE;
      delete_active=FALSE;
      }

   old_card_test = card_test;

                               
   VSyncCallBack(0);
   ChangeClearPAD(0);

   cdb = (cdb==db)? db+1: db;			
   ClearOTag(cdb->ot, OTSIZE);		
 
   if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD && isok>0 ) 
      {

      /*format !!! */
      if ((PadKeyIsPressed(&buffer1,PAD_RR)!=0) && (format_active == TRUE))
	      {
         bleep(3);
         for(stat=0; stat<100; stat+=5)
            {
	         cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            draw_options(card_status,card_files,selected,title_buffer);

            confirm_button_flash(FORMAT_BUTTON,stat-100);
	         DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         DrawSync(0);						
         fade(&db[0].confirm_button,128,128,128);
         fade(&db[1].confirm_button,128,128,128);
         isok=-20;
         while (PadKeyIsPressed(&buffer1,PAD_RU)!=0);
		   if (format_card()!=0) 
            {
            printf("re-read card\n");
            card_status= old_card_test = -69;
            }

		   }


		if (PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
		   {
         /* quit this screen */
         bleep(4);
         printf("quitting cardman\n");
         for(stat=0; stat<100; stat+=5)
            {
	         cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            draw_options(card_status,card_files,selected,title_buffer);
            confirm_button_flash(QUIT_BUTTON,100-stat);

 	         DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         	
         DrawSync(0);						
         fade(&db[0].confirm_button,128,128,128);
         fade(&db[1].confirm_button,128,128,128);
         while (PadKeyIsPressed(&buffer1,PAD_RU)!=0);
         quit_cardman(&title_buffer[0],selected,card_status,card_files);

         
         /** calculate the space remaining on the card and return this as **/
         /** the exit value                                               **/
 
         space_left=8192*15;
         for(di=0; di<card_files; di++)
            {
            space_left-=card_dir[di].size;
            }
         space_left/=8192;
         printf("space left %d\n",space_left);


         return space_left;    /* the space left on the card */
		   }

		if ((PadKeyIsPressed(&buffer1,PAD_RD)!=0) && (delete_active == TRUE)) 
		   {
         /*DELETE THE CURRENT FILE */
         isok=-15;
         bleep(3);
         while(PadKeyIsPressed(&buffer1,PAD_RD)!=0);


         for(stat=0; stat<100; stat+=5)
            {
	         cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		

            draw_options(card_status,card_files,selected,title_buffer);
            confirm_button_flash(DELETE_BUTTON,100-stat);

	         DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }                   

         //VSyncCallBack(0);

         ClearOTag(cdb->ot, OTSIZE);		
         fade(&db[0].confirm_button,128,128,128);
         fade(&db[1].confirm_button,128,128,128);
	
         delete_status = delete_file(&title_buffer[0],selected);
         if(delete_status == 1)
            {
            printf("re-read card\n");
            card_status= old_card_test = -69;
            }

     		}



	if ( PadKeyIsPressed(&buffer1,PAD_LL)!=0) 
		   {
         bleep(1);

         isok=-15;
		   selected--;
         if(selected<0)
            {
            selected=0;
            }
		   }

		if ( PadKeyIsPressed(&buffer1,PAD_LR)!=0) 
		   {
         isok=-15;
         bleep(1);

		   selected++;
         if(selected>card_files-1)
            {
            selected=card_files-1;
            }
		   }

   }




   if (card_files !=0)
      {
      valid_string=0;



      if(card_dir[selected].name[1]=='I')
         {
         strcpy(&title_buffer[0],"Unknown File Type");
         
         printf("Japanese product");
         }
       else
         {
         for(i=0; i<32; i++)
	         {
            title_buffer[i] = sjis2ascii(card_header[selected].Title[i]);

	         if ( title_buffer[i] != 0xFF && title_buffer[i] != 0)
		         {
		         // printf("\n %x != 0xff or 0 \n",title_buffer[i]);
		         valid_string++;
		         }	
	         }

         if (valid_string == 0)  /* then no valid kanji chars were found */
	         {
	         //   printf("\n\n\n");
	         //   printf("this is not kanji !!\n");
	         for(i=0; i<32; i++)
		         {
		         //    printf("i %d",i);
		         temp = card_header[selected].Title[i];
		         title_buffer[2*i] =  (temp<<8)>>8;
		         temp = card_header[selected].Title[i];
		         title_buffer[2*i+1] = temp>>8;	 
		         }	
	         }
         }

      }
   else
      {
      strcpy(&title_buffer[0],"   ");
      }

      




   draw_options(card_status,card_files,selected,title_buffer);
   isok++;
	FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	DrawOTag(cdb->ot);	  			
}while(1); 
}
/**------------------------------------------------------------------------**/








/**------------------------------------------------------------------------**/
/**---quit_cardman()-------------------------------------------------------**/
/**------------------------------------------------------------------------**/
quit_cardman(char* title_buffer,int selected,int card_status,int card_files)
{
int i=0;

fade_up(&db[0].screen_fade_mask,i);
fade_up(&db[1].screen_fade_mask,i);

do{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

   fade_up(&cdb->screen_fade_mask,i);
   i+=6;

   if(card_status==IOE)
      {
      if (card_files!=0)
         {
         delete_option_message(DELETE,244,106,128);
         }
      delete_option_message(QUIT  ,244,128,128);
    //  delete_option_message(FORMAT,244,161,128);

      draw_icons(PORT0,108,111,&card_header[0],selected);
      dave_text2(&title_buffer[0],31,229);
      }
   else
   if(card_status==TIMEOUT)
      {
      strcpy(&title_buffer[0],"No Memory card");
      dave_text2(&title_buffer[0],31,229);
      delete_option_message(QUIT  ,244,128,128);
      }   
   else
   if(card_status==NEWCARD)
      {
      strcpy(&title_buffer[0],"Memory card is not formatted");
      dave_text2(&title_buffer[0],31,229);
      delete_option_message(QUIT  ,244,128,128);
      delete_option_message(FORMAT,244,161,128);
      }   
 
 	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	DrawOTag(cdb->ot);	  			
  
}while(i<250); 

ClearOTag(cdb->ot, OTSIZE);		

}
/**------------------------------------------------------------------------**/




/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/

/*
delete_option_message(int message,int xpos,int ypos,int bright)

this function displays one of the messages on the right hand side of the 
cardman

   int message :DELETE 1   
               :QUIT   2
               :FORMAT 3
   int xpos    :position of top left hand corner of message box
   int ypos    :position of top left hand corner of message box
   int bright  :brightness of message box

*/


delete_option_message(int message,int xpos,int ypos,int bright)
{
POLY_FT4* poly;

switch (message)
   {
   case DELETE:
      poly = &cdb->option_message_delete;	      
      break;

   case QUIT:
      poly = &cdb->option_message_quit;	      
      break;

   case FORMAT:
      poly = &cdb->option_message_format;	
      break;

   default:
      break;
   }

poly->x0 = xpos-11;        poly->y0 = ypos;
poly->x1 = xpos+77;        poly->y1 = ypos;
poly->x2 = xpos-11;        poly->y2 = ypos+16;
poly->x3 = xpos+77;        poly->y3 = ypos+16;

poly->r0 = bright;
poly->g0 = bright;
poly->b0 = bright;

AddPrim(cdb->ot+3,poly);

}
/**------------------------------------------------------------------------**/


/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/


/*
card_message(int message,int bright)
this function displays the textual message that appears in the text area 
on the memory card in card man

int message : DELETE    1
              OP_FAILED 2
              SURE      3
int bright  : brightness of text

*/

card_message(int message,int bright)
{

switch (message)
   {
   case DELETE:

      cdb->yes_button.x0 =96;	 	      cdb->yes_button.y0 = 151;	 	  
      cdb->yes_button.x1 =96+20;       cdb->yes_button.y1 = 151;	  
      cdb->yes_button.x2 =96;	         cdb->yes_button.y2 = 151+20;
      cdb->yes_button.x3 =96+20;	      cdb->yes_button.y3 = 151+20;

      cdb->yes_text.x0 =119;	 	      cdb->yes_text.y0 = 155;	 	  
      cdb->yes_text.x1 =119+24;        cdb->yes_text.y1 = 155;	  
      cdb->yes_text.x2 =119;           cdb->yes_text.y2 = 155+13;
      cdb->yes_text.x3 =119+24;	      cdb->yes_text.y3 = 155+13;

      cdb->yes_text.u0 = 123;          cdb->yes_text.v0 = 0;
      cdb->yes_text.u1 = 123+24;	      cdb->yes_text.v1 = 0;
      cdb->yes_text.u2 = 123; 	      cdb->yes_text.v2 = 13;
      cdb->yes_text.u3 = 123+24;	      cdb->yes_text.v3 = 13;

      cdb->no_button.x0 =204;	 	      cdb->no_button.y0 = 151;	 	  
      cdb->no_button.x1 =204+20;       cdb->no_button.y1 = 151;	  
      cdb->no_button.x2 =204;	         cdb->no_button.y2 = 151+20;
      cdb->no_button.x3 =204+20;	      cdb->no_button.y3 = 151+20;

      cdb->no_text.x0 =180;	 	      cdb->no_text.y0 = 155;	 	  
      cdb->no_text.x1 =180+22;         cdb->no_text.y1 = 155;	  
      cdb->no_text.x2 =180;	         cdb->no_text.y2 = 155+13;
      cdb->no_text.x3 =180+22;	      cdb->no_text.y3 = 155+13;

      cdb->no_text.u0 =123;            cdb->no_text.v0 = 13;
      cdb->no_text.u1 =123+22;	      cdb->no_text.v1 = 13;
      cdb->no_text.u2 =123; 	         cdb->no_text.v2 = 13+13;
      cdb->no_text.u3 =123+22;	      cdb->no_text.v3 = 13+13;

      cdb->message_text.u0 = 0;        cdb->message_text.v0 = 31;
      cdb->message_text.u1 = 0+94;     cdb->message_text.v1 = 31;
      cdb->message_text.u2 = 0; 	      cdb->message_text.v2 = 31+18;
      cdb->message_text.u3 = 0+94;	   cdb->message_text.v3 = 31+18;

      cdb->message_text.x0 =112;	 	   cdb->message_text.y0 = 116;	 	  
      cdb->message_text.x1 =112+94;    cdb->message_text.y1 = 116;	  
      cdb->message_text.x2 =112;	      cdb->message_text.y2 = 116+18;
      cdb->message_text.x3 =112+94;	   cdb->message_text.y3 = 116+18;

      break;

   case OP_FAILED:

      cdb->yes_text.u0 =123;	 	      cdb->yes_text.v0 = 26;	 	  
      cdb->yes_text.u1 =123+39;        cdb->yes_text.v1 = 26;	  
      cdb->yes_text.u2 =123;	         cdb->yes_text.v2 = 26+13;
      cdb->yes_text.u3 =123+39;	      cdb->yes_text.v3 = 26+13;

      cdb->yes_text.x0 =119;	 	      cdb->yes_text.y0 = 154;	 	  
      cdb->yes_text.x1 =119+39;        cdb->yes_text.y1 = 154;	  
      cdb->yes_text.x2 =119;	         cdb->yes_text.y2 = 154+13;
      cdb->yes_text.x3 =119+39;	      cdb->yes_text.y3 = 154+13;

      cdb->no_text.x0 =173;	 	      cdb->no_text.y0 = 154;	 	  
      cdb->no_text.x1 =173+31;         cdb->no_text.y1 = 154;	  
      cdb->no_text.x2 =173;	         cdb->no_text.y2 = 154+14;
      cdb->no_text.x3 =173+31;	      cdb->no_text.y3 = 154+14;

      cdb->no_text.u0 =123;	 	      cdb->no_text.v0 = 40;	 	  
      cdb->no_text.u1 =123+31;         cdb->no_text.v1 = 40;	  
      cdb->no_text.u2 =123;	         cdb->no_text.v2 = 40+14;
      cdb->no_text.u3 =123+31;	      cdb->no_text.v3 = 40+14;
   
      cdb->message_text.u0 = 0;        cdb->message_text.v0 = 0;
      cdb->message_text.u1 = 0+82;	   cdb->message_text.v1 = 0;
      cdb->message_text.u2 = 0; 	      cdb->message_text.v2 = 0+30;
      cdb->message_text.u3 = 0+82;	   cdb->message_text.v3 = 0+30;

      cdb->message_text.x0 =118;	 	   cdb->message_text.y0 = 116;	 	  
      cdb->message_text.x1 =118+82;    cdb->message_text.y1 = 116;	  
      cdb->message_text.x2 =118;	      cdb->message_text.y2 = 116+30;
      cdb->message_text.x3 =118+82; 	cdb->message_text.y3 = 116+30;

      break;

   case SURE:
      cdb->yes_button.x0 =96;	 	      cdb->yes_button.y0 = 151;	 	  
      cdb->yes_button.x1 =96+20;       cdb->yes_button.y1 = 151;	  
      cdb->yes_button.x2 =96;	         cdb->yes_button.y2 = 151+20;
      cdb->yes_button.x3 =96+20;	      cdb->yes_button.y3 = 151+20;

      cdb->yes_text.x0 =119;	 	      cdb->yes_text.y0 = 155;	 	  
      cdb->yes_text.x1 =119+24;        cdb->yes_text.y1 = 155;	  
      cdb->yes_text.x2 =119;           cdb->yes_text.y2 = 155+13;
      cdb->yes_text.x3 =119+24;	      cdb->yes_text.y3 = 155+13;

      cdb->yes_text.u0 = 123;          cdb->yes_text.v0 = 0;
      cdb->yes_text.u1 = 123+24;	      cdb->yes_text.v1 = 0;
      cdb->yes_text.u2 = 123; 	      cdb->yes_text.v2 = 13;
      cdb->yes_text.u3 = 123+24;	      cdb->yes_text.v3 = 13;

      cdb->no_button.x0 =204;	 	      cdb->no_button.y0 = 151;	 	  
      cdb->no_button.x1 =204+20;       cdb->no_button.y1 = 151;	  
      cdb->no_button.x2 =204;	         cdb->no_button.y2 = 151+20;
      cdb->no_button.x3 =204+20;	      cdb->no_button.y3 = 151+20;

      cdb->no_text.x0 =180;	 	      cdb->no_text.y0 = 155;	 	  
      cdb->no_text.x1 =180+22;         cdb->no_text.y1 = 155;	  
      cdb->no_text.x2 =180;	         cdb->no_text.y2 = 155+13;
      cdb->no_text.x3 =180+22;	      cdb->no_text.y3 = 155+13;

      cdb->no_text.u0 =123;            cdb->no_text.v0 = 13;
      cdb->no_text.u1 =123+22;	      cdb->no_text.v1 = 13;
      cdb->no_text.u2 =123; 	         cdb->no_text.v2 = 13+13;
      cdb->no_text.u3 =123+22;	      cdb->no_text.v3 = 13+13;


      cdb->message_text.u0 = 0;        cdb->message_text.v0 = 50;   
      cdb->message_text.u1 = 0+119;	   cdb->message_text.v1 = 50;   
      cdb->message_text.u2 = 0; 	      cdb->message_text.v2 = 50+15;
      cdb->message_text.u3 = 0+119;	   cdb->message_text.v3 = 50+15;

      cdb->message_text.x0 =101;	 	   cdb->message_text.y0 = 116;	 	  
      cdb->message_text.x1 =101+119;   cdb->message_text.y1 = 116;	  
      cdb->message_text.x2 =101;	      cdb->message_text.y2 = 116+15;
      cdb->message_text.x3 =101+119;	cdb->message_text.y3 = 116+15;

      break;

   default:
      break;
   }



AddPrim(cdb->ot+3,&cdb->message_text);
AddPrim(cdb->ot+3,&cdb->yes_button);
AddPrim(cdb->ot+3,&cdb->yes_text);
AddPrim(cdb->ot+3,&cdb->no_button);
AddPrim(cdb->ot+3,&cdb->no_text);

}
/**------------------------------------------------------------------------**/





/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/



/*

this is a rewrite of the original function
*/




int delete_file(char* title_buffer,int selected)
{
int isok=-15;
int stat;
int cor,corrected_selected;
int del_test;
int quit_flag=0;

char card_filename[30];




corrected_selected = 0;

for (cor =0; cor<selected; cor++)
    {
    corrected_selected += card_header[cor].BlockEntry;
    }


do{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

	if ( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD && isok>0 ) 
		{
      /**-----------quit the function--------------*/
		if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
		   {
         bleep(3);
         for(stat=0; stat<100; stat+=5)
            {
	         cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            cdb->card_1_slot[corrected_selected].x0 =172;	 	   cdb->card_1_slot[corrected_selected].y0 = 117;	 	  
            cdb->card_1_slot[corrected_selected].x1 =172+16;      cdb->card_1_slot[corrected_selected].y1 = 117;	  
            cdb->card_1_slot[corrected_selected].x2 =172;	      cdb->card_1_slot[corrected_selected].y2 = 117+16;
            cdb->card_1_slot[corrected_selected].x3 =172+16;	   cdb->card_1_slot[corrected_selected].y3 = 117+16;
            AddPrim(cdb->ot+6,&cdb->card_1_slot[corrected_selected]); 
            card_message(DELETE,128);
            dave_text2(title_buffer,31,229);
            confirm_button_flash(NO_BUTTON,100-stat);
            FntFlush(-1);
	         DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         ClearOTag(cdb->ot, OTSIZE);		
         fade(&db[0].confirm_button,128,128,128);
         fade(&db[1].confirm_button,128,128,128);
         while(PadKeyIsPressed(&buffer1,PAD_RU)!=0);
         isok=-15;
         return 0;
		   }


      /**-----------delete the file--------------*/

		if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
		   {
         bleep(3);
         for(stat=0; stat<100; stat+=5)
            {
	         cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            cdb->card_1_slot[corrected_selected].x0 =172;	 	   cdb->card_1_slot[corrected_selected].y0 = 117;	 	  
            cdb->card_1_slot[corrected_selected].x1 =172+16;      cdb->card_1_slot[corrected_selected].y1 = 117;	  
            cdb->card_1_slot[corrected_selected].x2 =172;	      cdb->card_1_slot[corrected_selected].y2 = 117+16;
            cdb->card_1_slot[corrected_selected].x3 =172+16;	   cdb->card_1_slot[corrected_selected].y3 = 117+16;
            AddPrim(cdb->ot+6,&cdb->card_1_slot[corrected_selected]); 
            card_message(DELETE,128);
            dave_text2(title_buffer,31,229);
            confirm_button_flash(YES_BUTTON,100-stat);
            FntFlush(-1);                        
	         DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         ClearOTag(cdb->ot, OTSIZE);		
         fade(&db[0].confirm_button,128,128,128);
         fade(&db[1].confirm_button,128,128,128);

         while(PadKeyIsPressed(&buffer1,PAD_RD)!=0);
         return delete_confirm(title_buffer,selected);

         }
      }   


   cdb->card_1_slot[corrected_selected].x0 =172;	 	 cdb->card_1_slot[corrected_selected].y0 = 117;	 	  
   cdb->card_1_slot[corrected_selected].x1 =172+16;    cdb->card_1_slot[corrected_selected].y1 = 117;	  
   cdb->card_1_slot[corrected_selected].x2 =172;	    cdb->card_1_slot[corrected_selected].y2 = 117+16;
   cdb->card_1_slot[corrected_selected].x3 =172+16;	 cdb->card_1_slot[corrected_selected].y3 = 117+16;

   flasher(&db[0].card_1_slot[corrected_selected]);
   flasher(&db[1].card_1_slot[corrected_selected]);



   AddPrim(cdb->ot+6,&cdb->card_1_slot[corrected_selected]); 
   card_message(DELETE,128);

   dave_text2(title_buffer,31,229);
   isok++;
	FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	DrawOTag(cdb->ot);	  			
  
}while(1); 






}
/**------------------------------------------------------------------------**/




/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
int delete_confirm(char* title_buffer,int selected)
{
int isok =-15;
int stat=0;
int del_test;
char card_filename[30];

do{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		

	if ( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD && isok>0 ) 
		{
      /**-----------quit the function--------------*/
		if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
		   {
         bleep(3);
         printf("quitting retry\n");
         for(stat=0; stat<100; stat+=5)
            {
	         cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            card_message(SURE,128);
            dave_text2(title_buffer,31,229);
            confirm_button_flash(NO_BUTTON,100-stat);
            FntFlush(-1);
	         DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         ClearOTag(cdb->ot, OTSIZE);		
         fade(&db[0].confirm_button,128,128,128);
         fade(&db[1].confirm_button,128,128,128);

         isok=-15;   
         while(PadKeyIsPressed(&buffer1,PAD_RU)!=0);
         return 0;
		   }

      if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
		   {
         bleep(3);
         for(stat=0; stat<100; stat+=5)
            {
	         cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            card_message(SURE,128);
            dave_text2(title_buffer,31,229);
            confirm_button_flash(YES_BUTTON,100-stat);
            FntFlush(-1);
	         DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	   
            }
         ClearOTag(cdb->ot, OTSIZE);		
         fade(&db[0].confirm_button,128,128,128);
         fade(&db[1].confirm_button,128,128,128);

         isok=-15;
         strcpy(card_filename,"bu00:");
         strcat(card_filename,&card_dir[selected].name); 
         printf("deleting the file>>%s<<\n",card_filename);

         while(PadKeyIsPressed(&buffer1,PAD_RD)!=0);
     
         del_test = delete(&card_filename[0]);
         printf("del_test = %d\n",del_test);

         /*****************************************************/
         /* if the delete fails, retry until succeeds or quit */
         /*****************************************************/
         while(del_test==0)
            {  
            del_test=delete_error(title_buffer,selected);
            printf("del_test = %d\n",del_test);
            }
         return del_test;
         }
      }

   card_message(SURE,128);
   dave_text2(title_buffer,31,229);
   isok++;
	FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	DrawOTag(cdb->ot);	  			
  
}while(1); 



}







/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
/**------------------------------------------------------------------------**/
/*
confirm_button_flash(int button,int bright)
int button: YES_BUTTON 1:
            NO_BUTTON  2:
            DELETE_BUTTON  3
            QUIT_BUTTON    4
            FORMAT_BUTTON  5
*/



confirm_button_flash(int button,int bright)
{


bright+=128;


switch (button)
   {
   case YES_BUTTON:
      cdb->confirm_button.u0 = 0;      cdb->confirm_button.v0 = 132;
      cdb->confirm_button.u1 = 0+20;   cdb->confirm_button.v1 = 132;
      cdb->confirm_button.u2 = 0;      cdb->confirm_button.v2 = 132+20;
      cdb->confirm_button.u3 = 0+20;   cdb->confirm_button.v3 = 132+20;

      cdb->confirm_button.x0 =96;      cdb->confirm_button.y0 = 151; 
      cdb->confirm_button.x1 =96+20;   cdb->confirm_button.y1 = 151;
      cdb->confirm_button.x2 =96;      cdb->confirm_button.y2 = 151+20;
      cdb->confirm_button.x3 =96+20;   cdb->confirm_button.y3 = 151+20;
      break;


   case NO_BUTTON:
      cdb->confirm_button.u0 = 0;         cdb->confirm_button.v0 = 152;   
      cdb->confirm_button.u1 = 0+20;      cdb->confirm_button.v1 = 152;   
      cdb->confirm_button.u2 = 0;         cdb->confirm_button.v2 = 152+20;
      cdb->confirm_button.u3 = 0+20;      cdb->confirm_button.v3 = 152+20;

      cdb->confirm_button.x0 =204;        cdb->confirm_button.y0 = 151;    
      cdb->confirm_button.x1 =204+20;     cdb->confirm_button.y1 = 151;   
      cdb->confirm_button.x2 =204;        cdb->confirm_button.y2 = 151+20;
      cdb->confirm_button.x3 =204+20;     cdb->confirm_button.y3 = 151+20;
      break;
   
   case DELETE_BUTTON:
      cdb->confirm_button.u0 = 0;      cdb->confirm_button.v0 = 132;
      cdb->confirm_button.u1 = 0+20;   cdb->confirm_button.v1 = 132;
      cdb->confirm_button.u2 = 0;      cdb->confirm_button.v2 = 132+20;
      cdb->confirm_button.u3 = 0+20;   cdb->confirm_button.v3 = 132+20;

      cdb->confirm_button.x0 =-11+243;      cdb->confirm_button.y0 = 104; 
      cdb->confirm_button.x1 =-11+243+20;   cdb->confirm_button.y1 = 104;
      cdb->confirm_button.x2 =-11+243;      cdb->confirm_button.y2 = 104+20;
      cdb->confirm_button.x3 =-11+243+20;   cdb->confirm_button.y3 = 104+20;
      break;

   case QUIT_BUTTON:
      cdb->confirm_button.u0 = 0;         cdb->confirm_button.v0 = 152;   
      cdb->confirm_button.u1 = 0+20;      cdb->confirm_button.v1 = 152;   
      cdb->confirm_button.u2 = 0;         cdb->confirm_button.v2 = 152+20;
      cdb->confirm_button.u3 = 0+20;      cdb->confirm_button.v3 = 152+20;

      cdb->confirm_button.x0 =-11+243;        cdb->confirm_button.y0 = 126;    
      cdb->confirm_button.x1 =-11+243+20;     cdb->confirm_button.y1 = 126;   
      cdb->confirm_button.x2 =-11+243;        cdb->confirm_button.y2 = 126+20;
      cdb->confirm_button.x3 =-11+243+20;     cdb->confirm_button.y3 = 126+20;
      break;
 
   case FORMAT_BUTTON:
      cdb->confirm_button.u0 = 0;         cdb->confirm_button.v0 = 172;   
      cdb->confirm_button.u1 = 0+20;      cdb->confirm_button.v1 = 172;   
      cdb->confirm_button.u2 = 0;         cdb->confirm_button.v2 = 172+20;
      cdb->confirm_button.u3 = 0+20;      cdb->confirm_button.v3 = 172+20;

      cdb->confirm_button.x0 =-11+243;        cdb->confirm_button.y0 = 159;    
      cdb->confirm_button.x1 =-11+243+20;     cdb->confirm_button.y1 = 159;   
      cdb->confirm_button.x2 =-11+243;        cdb->confirm_button.y2 = 159+20;
      cdb->confirm_button.x3 =-11+243+20;     cdb->confirm_button.y3 = 159+20;
      break;








   }/*end case*/ 

cdb->confirm_button.r0 = bright;
cdb->confirm_button.g0 = bright;
cdb->confirm_button.b0 = bright;

AddPrim(cdb->ot+4,&cdb->confirm_button);


}
/**------------------------------------------------------------------------**/



/**------------------------------------------------------------------------**/
/**--delete_error(char* title_buffer,int selected)-------------------------**/
/**------------------------------------------------------------------------**/

/*
delete_error(char* title_buffer,int selected)
   char* title_buffer: so can display tile of card
   int selected: so can get filename of card

returns  not 0 : quit or delete suceeded
             0 : write failed

*/


int delete_error(char* title_buffer,int selected)
{
int deletion_test;
int isok=-15;
int stat;
char card_filename[30];

printf("An error occured !!! \n");

do
   {
   cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		
   if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD && isok>0 ) 
      {
      if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0) 
		   {
         bleep(3);
         /* quit this screen */
         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            card_message(OP_FAILED,128);
            dave_text2(title_buffer,31,229);
            confirm_button_flash(NO_BUTTON,100-stat);
            FntFlush(-1);
	         DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	
            }
         ClearOTag(cdb->ot, OTSIZE);		
         fade(&db[0].confirm_button,128,128,128);
         fade(&db[1].confirm_button,128,128,128);
         isok=-15;
         printf("quiting from delete error  \n");
         while(PadKeyIsPressed(&buffer1,PAD_RU)!=0);
         return 1;
         }


      /**********************************************************/
      /* if retry, flash the confirm button and then attempt to */
      /* delete the file again                                  */
      /**********************************************************/

      if ( PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
		   {
         bleep(3);
         /* retry */
         isok=-15;
         for(stat=0; stat<100; stat+=5)
            {
            cdb = (cdb==db)? db+1: db;			
	         ClearOTag(cdb->ot, OTSIZE);		
            card_message(OP_FAILED,128);
            dave_text2(title_buffer,31,229);
            confirm_button_flash(YES_BUTTON,100-stat);
            FntFlush(-1);
	         DrawSync(0);						
            VSync(0);							
	         PutDrawEnv(&cdb->draw); 		
	         PutDispEnv(&cdb->disp); 		
	         draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	         DrawOTag(cdb->ot);	
            }
         ClearOTag(cdb->ot, OTSIZE);		
         fade(&db[0].confirm_button,128,128,128);
         fade(&db[1].confirm_button,128,128,128);

         strcpy(card_filename,"bu00:");
         strcat(card_filename,&card_dir[selected].name); 
         printf("**********fileNAME >>%s<<\n",card_dir[selected].name);
         printf("**********deleting the file >>%s<<\n",card_filename);

         deletion_test = delete(&card_filename[0]);
         printf("deletion_test = %d\n",deletion_test);
         printf("exiting delete_error");
         while(PadKeyIsPressed(&buffer1,PAD_RD)!=0);
         return deletion_test;
         
         }
      }         
   card_message(OP_FAILED,128);
   dave_text2(title_buffer,31,229);
   isok++;
   FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	draw_backdrop( (unsigned long*)&backtimbuff[0]); 
  	DrawOTag(cdb->ot);	
   }while(1);		


Toto: /* i'll leave this in as a tribute to Pascal who found the bug in */
      /* my usage of strcat                                             */


}
/**------------------------------------------------------------------------**/







draw_options(int card_status,int card_files,int selected,char* title_buffer)
   {
   if(card_status==IOE)
      {
      if(card_files!=0)
         {      
         delete_option_message(DELETE,244,106,128);
         }      
      delete_option_message(QUIT  ,244,128,128);
      
      //delete_option_message(FORMAT,244,161,128);

      draw_icons(PORT0,108,111,&card_header[0],selected);
      dave_text2(&title_buffer[0],31,229);
      }
   else
   if(card_status==TIMEOUT)
      {
      strcpy(&title_buffer[0],"No Memory card");
      dave_text2(&title_buffer[0],31,229);
      delete_option_message(QUIT  ,244,128,128);
      }   
   else
   if(card_status==NEWCARD)
      {
      strcpy(&title_buffer[0],"Memory card is not formatted");
      dave_text2(&title_buffer[0],31,229);
      delete_option_message(QUIT  ,244,128,128);
      delete_option_message(FORMAT,244,161,128);
      }   
 
   }

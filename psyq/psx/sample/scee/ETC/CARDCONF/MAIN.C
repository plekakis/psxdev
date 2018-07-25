/*****************************************************************************/
/*****************************************************************************/
/** main.c 																						 **/
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
#include "card.h"
#include "ctrller.h"
#include "sound.h"


extern unsigned long icontim[];
extern unsigned long vabheader[];
extern unsigned long vabbody[];
extern unsigned long seqdata[]; 
extern unsigned long marvindata[]; 



CARD_BUFFER input_buffer;
CARD_BUFFER output_buffer;



/**-------------------- display -------------------------------------------**/
DB	 db[2];		/* packet double buffer */
DB* cdb;			/* current db */


/**-------------------- memory cards ------------------------------------**/

unsigned long 	ev0,ev1,ev2,ev3,ev10,ev11,ev12,ev13;

struct	DIRENTRY card_dir[15];	



/**-------------------- pads -----------------------------------------**/
ControllerPacket buffer1;
ControllerPacket buffer2;


int selected=0; 			/*currently selected file             */
int card_files=0;			/*number of files on the current file */
int old_card_files=0;		/*old number of files on the current file */



int card_status=-1;	 		/*current card status*/




/**-------------------- sound -------------------------------------------**/
short vab;	/* vab data id */
short seq;	/* seq data id */
char seq_table [SS_SEQ_TABSIZ * 1 * 1];  /* seq table size */






/**-------------------- timing stuff -------------------------------------**/

int elapsed_time =0;
int v_count=0;	  				/*vsync counter      */

/**--------------------------------------------------------------------**/
/**----------------- vcounter -----------------------------------------**/
/**--------------------------------------------------------------------**/
void vcounter()
{
v_count++;
}
/**--------------------------------------------------------------------**/





/**-------------------- main ----------------------------------------------**/
main()
{
int access = FALSE;


ResetCallback();	
ResetGraph(0);

SetDefDrawEnv(&db[0].draw, 0,  0, 512, 240);     
SetDefDrawEnv(&db[1].draw, 0,240, 512, 240);     
SetDefDispEnv(&db[0].disp, 0,240, 512, 240);     
SetDefDispEnv(&db[1].disp, 0,  0, 512, 240);

FntLoad(640,256);
FntOpen(10, 16, 500, 250, 0,800);

InitCARD(0);
StartCARD();
_bu_init();

init_cards();


initPAD(&buffer1,MAX_CONTROLLER_BYTES,&buffer2,MAX_CONTROLLER_BYTES);
StartPAD();

ChangeClearPAD(0);
VSyncCallBack(vcounter);

init_sound();

init_prim(&db[0]);
init_prim(&db[1]);

/* marvin's our special friend that nobody else can see */
bitmap((u_char*)marvindata);
init_marvin(&db[0]);
init_marvin(&db[1]);



SetDispMask(1);

card_status = test_card(PORT0);
	
main_menu();
	

}


/**------------------------------------------------------------------------**/
/**-------------------- init_prim -----------------------------------------**/
/**------------------------------------------------------------------------**/

init_prim(DB *db)
{
int counter;
int i;

db->draw.isbg = 1;
db->draw.r0 = 100;
db->draw.b0 = 100;
db->draw.g0 =   0;	
}
/**------------------------------------------------------------------------**/





/**------------------------------------------------------------------------**/
/**-------------------- access file ---------------------------------------**/
/**------------------------------------------------------------------------**/
access_file()
{

/* read in the first 128 bytes off the card */
int 			fd;
char card_filename[21];
char 	title_buffer[64];

BUFFER buffer;
int i;
int valid_string=0;

int operation=0;
int test =-1;
int isok =-15;
unsigned short temp;

strcpy(card_filename,"bu00:");
strcat(card_filename,card_dir[selected].name); 

if((fd=open(card_filename,O_RDONLY))>=0)
	{     
	read(fd,&buffer,sizeof(buffer));
	close(fd);
	}
else
	{
	printf("Error could not open file to read header...%s\n",card_filename);
	}


valid_string=0;

for(i=0; i<32; i++)
	{
	title_buffer[i] = sjis2ascii(buffer.header.Title[i]);

	if ( title_buffer[i] != 0xFF && title_buffer[i] != 0)
		{
		printf("\n %x != 0xff or 0 \n",title_buffer[i]);
		valid_string++;
		}	
	}


if (valid_string == 0)  /* then no valid kanji chars were found */
	{
	printf("\n\n\n");
	printf("this is not kanji !!\n");
	for(i=0; i<32; i++)
		{
		printf("i %d",i);
		temp = buffer.header.Title[i];
		title_buffer[2*i] =  (temp<<8)>>8;
		temp = buffer.header.Title[i];
		title_buffer[2*i+1] = temp>>8;	 
		}	
	}


do
	{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		
	
	FntPrint("    developers conference 1996 memory card demo program\n");
	FntPrint("    ***************************************************\n");
	FntPrint("\n\n");
	FntPrint("    file :%21s %d  ",card_dir[selected].name,card_dir[selected].size );
	FntPrint("\n\n");
	FntPrint("    header details\n");
	FntPrint("    **************\n");
	FntPrint("    magic      : %c%c\n",buffer.header.Magic[0],buffer.header.Magic[1]);
	FntPrint("    type       : %x\n",buffer.header.Type);
	FntPrint("    blockentry : %d\n",buffer.header.BlockEntry);
	FntPrint("    title      : %s\n",title_buffer);

	FntPrint("\n\n");
	FntPrint("    hit (x) to delete this file \n");
	FntPrint("    hit sel to return to main file menu\n");

	if(test ==0)
		{
		FntPrint("    error: deletion failed\n");
		}
	else 
	if(test>0)
		{
		FntPrint("    done: deletion successful\n");
		}

	if (isok>0)
		{
		if ( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
			{
			if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
				{
				test = delete_routine(selected);
				isok=-15;
				}		


			if (PadKeyIsPressed(&buffer1,PAD_SEL)!=0) 
				{
				return 0;
				}		
		 	}
		}

	isok++;
	FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	DrawOTag(cdb->ot);	 
	}
while(operation !=1);

	

}
/**------------------------------------------------------------------------**/




/**------------------------------------------------------------------------**/
/**-------------------- main_menu -----------------------------------------**/
/**------------------------------------------------------------------------**/


main_menu()
{

#define main_menu_items	5
char *main_menu_options[main_menu_items] = 	{"view dir          ",
                                              "format card       ",
                                              "blocking test     ",
                                              "non blocking test ",  
                                              "music controls    "
                                              };
int isok = -15;
int main_menu_selected=0;
int i=0;

int stat0;
int stat1;


/*values returned by test_card() */
/*   0:No card detected
     1:Detected a formatted card
	  2:Detected a newly connected card and marked it
	  3:Communication error happened
	  4:Detected an unformatted card
*/






do
	{
	
/*	stat0 = test_card(0x00);
	stat1 = test_card(0x10);*/
	
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		
	
	FntPrint("    developers conference 1996 memory card demo program\n");
	FntPrint("    ***************************************************\n");
	FntPrint("\n\n");
	FntPrint("    main menu\n");
	FntPrint("    *********\n\n");

	for(i=0; i<main_menu_items; i++)
		{
		if (i==main_menu_selected) 
			{
			FntPrint("    *");
			}
		else
			{
			FntPrint("     ");
			}
		FntPrint("%s",main_menu_options[i]);
		if (i==main_menu_selected) 
			{
			FntPrint("*");
			}
		FntPrint("\n");
		}


	if (isok>0)	/* if no valid button has been pressed for 15 vsyncs */
		{
		if (GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
			{
			if (PadKeyIsPressed(&buffer1,PAD_LD)!=0) 
				{
				main_menu_selected++; 
				isok=-15;  

				if (main_menu_selected>main_menu_items-1)
					{
					main_menu_selected--;
					}
				}

			if (PadKeyIsPressed(&buffer1,PAD_LU)!=0) 
				{
				main_menu_selected--; 
				isok=-15;  
				if (main_menu_selected<0)
					{
					main_menu_selected=0;
  					}
				}

			if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
				{
				switch(main_menu_selected)
					{
					case 0:
						view_a_dir();
						break;

					case 1:
						format_menu();
						break;
					case 2:
                  blocking_access_test();
						break;

   				case 3:
                  no_blocking_access_test();
						break;


					case 4:
						music_menu();
						break;

					default:
						break;
					}

				isok=-15;  
				}

			}
		}
	isok++;
   animate_marvin();

	FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	DrawOTag(cdb->ot);	 
	}while(1);





}


/**------------------------------------------------------------------------**/
/**-------------------- file_menu -----------------------------------------**/
/**------------------------------------------------------------------------**/
file_menu()
{
int i;



/*while (NoPadKeyPressed(&buffer1));*/


if(card_files==0)
	{
	FntPrint("    NO FILE\n");
	FntPrint("\n");
	FntPrint("    hit sel to return to main menu\n");
	}
else 
	{
	FntPrint("    %d FILE(S)\n\n",card_files);
	for(i=0;i<card_files;i++) 
		{
		if(i==selected)
			{
			FntPrint("   *");
			}
		else
			{
			FntPrint("    ");
			}

		FntPrint("%2d:%20s %d",i+1,card_dir[i].name,card_dir[i].size);

		if(i==selected)
			{
			FntPrint("*\n");
			}
		else
			{
			FntPrint("\n");
			}
		}
	FntPrint("\n");
	FntPrint("    hit (x) to access file\n");
	FntPrint("    hit sel to return to main menu\n");

	}

}
/**------------------------------------------------------------------------**/





/**------------------------------------------------------------------------**/
/**-------------------- format_menu ---------------------------------------**/
/**------------------------------------------------------------------------**/
format_menu()
{
int test=-1;
int isok = -15;

do
	{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		
	
	FntPrint("    developers conference 1996 memory card demo program\n");
	FntPrint("    ***************************************************\n");

	FntPrint("\n\n");
	FntPrint("    format card\n");
	FntPrint("    ***********\n\n");

	FntPrint("    hit (x) to format card\n");
	FntPrint("    hit sel to exit     \n");


	if(test==0)
		{
		FntPrint("    error: the format operation failed\n");
		}

	if(test>0)
		{
		FntPrint("    done: format successful %d\n",test);
		}




	
	if (isok>0)	/* if no valid button has been pressed for 15 vsyncs */
		{
		if (GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
			{

			if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
				{

				FntPrint("\n\n\n\n\n\n\n\n\n hello!");
				FntFlush(-1);

				printf("hello ?");
				test = format_routine();
				FntPrint("done!");
				FntFlush(-1);
				
				}

			if (PadKeyIsPressed(&buffer1,PAD_SEL)!=0) 
				{
				return 0;
				}

			}
		}
   animate_marvin();
	isok++;
	FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	DrawOTag(cdb->ot);	 
	}while(1);






}
/**------------------------------------------------------------------------**/




/**------------------------------------------------------------------------**/
/**-------------- view_a_dir ----------------------------------------------**/
/**------------------------------------------------------------------------**/
view_a_dir()
{
int access = FALSE;
int isok   = -15;


do
	{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		
	
	FntPrint("    developers conference 1996 memory card demo program\n");
	FntPrint("    ***************************************************\n");
	FntPrint("    %d ",isok);

	FntPrint("    port 0 current status :>");
	
	switch(card_status)
		{
		case IOE: 	
			FntPrint(" ready\n");
			old_card_files=card_files;
			card_files = dir_file(&card_dir[0]);
			break;

		case ERROR: 	
			FntPrint(" an error has occured \n");
			card_files = 0;
			break;

		case TIMEOUT:
			FntPrint(" no card\n");
			card_files = 0;
			break;

		case NEWCARD:
			FntPrint(" card is unformatted\n");
			break;

		
		default:
			FntPrint("processing \n");
			break;

		}

	if(old_card_files!=card_files || card_files==0)
		{
		/*the content/status of the card has changed! or there is no */
		/*card connected so do some processing                       */

		selected=0;
		card_status = status(PORT0);
		card_files = dir_file(&card_dir[0]);
		}

	file_menu();
	if (isok>0)
		{

      VSync(0);     /* if you take this vsync out of the code, you can't */
                    /* read the controller when no card is connected     */
                    /* you need a delay after accessing a card before    */
                    /* you can access the controllers (nice!)            */


		if (GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
			{
			if (PadKeyIsPressed(&buffer1,PAD_LD)!=0) 
				{
				selected++; 
				isok=-15;
				if (selected>card_files-1)
					{
					selected--;
					}
				}

			if (PadKeyIsPressed(&buffer1,PAD_LU)!=0) 
				{
				selected--; 
				isok=-15;
				if (selected<0)
					{
					selected=0;
					}
				}

			access = FALSE;
			if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
				{
				isok=-15;
				access_file();
				}

			if (PadKeyIsPressed(&buffer1,PAD_SEL)!=0) 
				{
				isok=-15;
	         ClearOTag(cdb->ot, OTSIZE);		
				return 0;
				}

			}
		}

   animate_marvin();
	isok++;
	FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	DrawOTag(cdb->ot);	 
	}while(1);


}
/**------------------------------------------------------------------------**/


/**------------------------------------------------------------------------**/
/**-------------- music_menu ----------------------------------------------**/
/**------------------------------------------------------------------------**/

music_menu()
{
int isok   = -15;
static int status=OFF;

do
	{
	cdb = (cdb==db)? db+1: db;			
	ClearOTag(cdb->ot, OTSIZE);		
	
	FntPrint("    developers conference 1996 memory card demo program\n");
	FntPrint("    ***************************************************\n");

	FntPrint("\n\n");
	FntPrint("    music controls\n");
	FntPrint("    **************\n\n");

	FntPrint("    status %d\n\n",status);

	FntPrint("    hit (x) to toggle lovely music\n");
	FntPrint("    hit sel to exit     \n");

	
	if (isok>0)	/* if no valid button has been pressed for 15 vsyncs */
		{
		if (GoodData(&buffer1) && GetType(&buffer1)==STD_PAD) 
			{

			if (PadKeyIsPressed(&buffer1,PAD_RD)!=0) 
				{
            if(status ==OFF)
               {
               status =ON;
               isok=-15;
               music_on(); 
               }
            else
               {
               status =OFF;
               isok=-15;
               music_off();   
               }
				}
 
			if (PadKeyIsPressed(&buffer1,PAD_SEL)!=0) 
				{
            ClearOTag(cdb->ot, OTSIZE);		
				return 0;
				}

			}
		}
   animate_marvin();
	isok++;
	FntFlush(-1);
	DrawSync(0);						
	VSync(0);							
	PutDrawEnv(&cdb->draw); 		
	PutDispEnv(&cdb->disp); 		
	DrawOTag(cdb->ot);	 
	}while(1);

}


/**------------------------------------------------------------------------**/
/**-------------- compare_buffers -----------------------------------------**/
/**------------------------------------------------------------------------**/
compare_buffers(int bytes,char* buffy1,char* buffy2)
{
int i;
int non_matches = 0;


printf("\n");
printf("********************************************************\n");
printf("comparing buffers: 0x%x   0x%x\n",buffy1,buffy2);
printf("********************************************************\n");


for(i=0; i<bytes; i++)
   {
   if(buffy1[i] != buffy2[i])
      {
      printf("E %d %c %c\n",i,buffy1[i],buffy2[i]);
      non_matches++;
      }
   }

return non_matches;
}

/**------------------------------------------------------------------------**/




/**------------------------------------------------------------------------**/
/**-------------- fill_buffer ---------------------------------------------**/
/**------------------------------------------------------------------------**/
fill_buffer(char* buffy,char test)
{
int i;

printf("test para = %c\n",test);

printf("\n");
printf("********************************************************\n");
printf("filing buffer: 0x%x   %c\n",buffy,test);
printf("********************************************************\n");


for(i=0; i<sizeof(CARD_BUFFER); i++)
   {
   buffy[i] = test;
   }

}

/**------------------------------------------------------------------------**/



/**------------------------------------------------------------------------**/
/**----------------- create_test_data -------------------------------------**/
/**------------------------------------------------------------------------**/

create_test_data(int blocks,CARD_BUFFER* buffy,u_long *tim_addr)
{
char j;
int i;
GsIMAGE TexInfo;
char temp[] ="Dev Con Demo :test file 1       ";

printf("\n");
printf("creating test data\n");
printf("\n");

tim_addr++;
GsGetTimInfo(tim_addr, &TexInfo);

buffy->card_data.header.Magic[0] = 'S';
buffy->card_data.header.Magic[1] = 'C';
buffy->card_data.header.Type = 0x11;
buffy->card_data.header.BlockEntry = blocks;


/* convert title from ascii to sjis */
for(i=0; i<32; i++)
	{
	buffy->card_data.header.Title[i] = ascii2sjis(temp[i]);
	printf("%d  %c %x\n",i,temp[i],buffy->card_data.header.Title[i]);
	}

memcpy(&buffy->card_data.header.Clut[0],TexInfo.clut,32);
memcpy(&buffy->card_data.header.Icon[0],TexInfo.pixel,128);

printf("\n");
printf("********************************************************\n");
printf("create test data: 0x%x   blocks %d\n",buff,blocks);
printf("********************************************************\n");


j='a';
for(i=sizeof(_CARD); i<(sizeof(CARD_BUFFER)-sizeof(_CARD)); i++)
	{
	buffy->card_data.body[i]=j;
	if(i%128==0 && i!=0)
		{
		j++;
		}
	}
}

/**------------------------------------------------------------------------**/



/**------------------------------------------------------------------------**/
/**--------------------	con_dump ------------------------------------------**/
/**------------------------------------------------------------------------**/

con_dump(int bytes,char* buff)
{
int i;
printf("\n");
printf("********************************************************\n");
printf("console dump: 0x%x   bytes %d\n",buff,bytes);
printf("********************************************************\n");

for(i=0; i<bytes; i++)
	{
	printf("%c",buff[i]);	
	if(i%64==0 && i!=0)
		{
		printf("\n");
		pollhost();
		}
	}
}
/**------------------------------------------------------------------------**/


message_fudge(char* mess)
{
int counter = 0;

do{
  cdb = (cdb==db)? db+1: db;			
  ClearOTag(cdb->ot, OTSIZE);		
	
  FntPrint("\n\n\n");

  FntPrint("    developers conference 1996 memory card demo program\n");
  FntPrint("    ***************************************************\n");

  FntPrint("    blocking access test\n");
  FntPrint("    ********************\n");

  FntPrint("\n\n",mess);
  FntPrint("%s\n",mess);

  animate_marvin();
  FntFlush(-1);
  DrawSync(0);						
  VSync(0);							
  PutDrawEnv(&cdb->draw); 		
  PutDispEnv(&cdb->disp); 		
  DrawOTag(cdb->ot);	 
  counter++;
  }
  while(counter<20);



}



/**------------------------------------------------------------------------**/
/**-------------------- blocking_access_test ------------------------------**/
/**------------------------------------------------------------------------**/

blocking_access_test()
{
int errors = 0;
int counter = 0;

/** this is a crappy fudge cos i ran out of time **/
/* set the input and output buffers to be different */

message_fudge("    setting test buffers");


fill_buffer((u_char*)&input_buffer.buff[0],'a');
fill_buffer((u_char*)&output_buffer.buff[0],'b');

/*create some valid test data */
message_fudge("    creating test data");
create_test_data(2,&input_buffer,icontim);


/* write the test data to the file */
message_fudge("    writing test data to the file");
block_write(2,&input_buffer.buff[0]);

/* read the test file back into another buffer*/
message_fudge("    reading test data from the file");

block_read(2,&output_buffer.buff[0]);

/* compare the buffers */

message_fudge("    comparing the two buffers xx");

errors = compare_buffers(8192*2,&input_buffer.buff[0],&output_buffer.buff[0]);
printf("\ntest complete: errors %d\n",errors);

if (errors==0)
   {
   message_fudge("    transfer succeeded no errors");
   }
else
   {
   message_fudge("    tranfer failed ");
   }

for (counter=0; counter<50*2; counter++)
   {
   VSync(0);
   }







ClearOTag(cdb->ot, OTSIZE);		
}



/**------------------------------------------------------------------------**/
/**-------------------- blocking_access_test ------------------------------**/
/**------------------------------------------------------------------------**/

no_blocking_access_test()
{
int errors = 0;
int counter = 0;

/** this is a crappy fudge cos i ran out of time **/
/* set the input and output buffers to be different */

message_fudge("    setting test buffers");


fill_buffer((u_char*)&input_buffer.buff[0],'a');
fill_buffer((u_char*)&output_buffer.buff[0],'b');

/*create some valid test data */
message_fudge("    creating test data");
create_test_data(2,&input_buffer,icontim);


/* write the test data to the file */
message_fudge("    writing test data to the file");


no_block_write(2,&input_buffer.buff[0]);

/* read the test file back into another buffer*/
message_fudge("    reading test data from the file");

no_block_read(2,&output_buffer.buff[0]);

/* compare the buffers */

message_fudge("    comparing the two buffers xx");

errors = compare_buffers(8192*2,&input_buffer.buff[0],&output_buffer.buff[0]);
printf("\ntest complete: errors %d\n",errors);

if (errors==0)
   {
   message_fudge("    transfer succeeded no errors");
   }
else
   {
   message_fudge("    tranfer failed ");
   }

for (counter=0; counter<50*2; counter++)
   {
   VSync(0);
   }







ClearOTag(cdb->ot, OTSIZE);		
}




























/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/** cos the following functions mix graphics and card access and I can't   **/
/** really see anyone splitting the two halves up i've left them in main   **/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/


/**------------------------------------------------------------------------**/
/**---non_blocking_read ---------------------------------------------------**/
/**------------------------------------------------------------------------**/

no_block_read(int blocks,char* buffer)
{

int data_read_size=256;

int fd;
char card_filename[21];

int ret,i;


strcpy(card_filename,"bu00:");
strcat(card_filename,"testfile"); 

printf("\n");
printf("********************************************************\n");
printf("non blocking read file: %s  blocks %d\n",card_filename,blocks);
printf("********************************************************\n");

v_count=0;
printf("********>read begins!\n");

if((fd=open(card_filename,O_RDONLY|O_NOWAIT))>=0)
	{
	for(i=0; i< ((8192*blocks)/data_read_size);i++)
		{
      cdb = (cdb==db)? db+1: db;			
      ClearOTag(cdb->ot, OTSIZE);		
	
      FntPrint("\n\n\n");

      FntPrint("    developers conference 1996 memory card demo program\n");
      FntPrint("    ***************************************************\n");

      FntPrint("    non blocking access\n");
      FntPrint("    *******************\n");

      FntPrint("\n\n");
      FntPrint("    non blocking read\n");

		_clear_event();
		while((ret = read(fd,&buffer[i*data_read_size],data_read_size))!=0);
		ret = _card_event();
		if (ret==1)
			{
			printf("error: card removed?\n");
			exit(0);
			}
		lseek(fd,128,SEEK_CUR);  
	            /*********************************************************/
					/* this is to correct a bug in read used in non-blocking */
					/* the file pointer always updates by 128 bytes too few! */
					/*********************************************************/
 

      animate_marvin();
      FntFlush(-1);
      DrawSync(0);						
      VSync(0);							
      PutDrawEnv(&cdb->draw); 		
      PutDispEnv(&cdb->disp); 		
      DrawOTag(cdb->ot);	 

		}
	close(fd);
	}
else
	{
	printf("error: could not open file!\n");
	exit(0);
	}

printf("********>read ends after %d vblanks or %d secs!\n",v_count,v_count/60);
elapsed_time = v_count;

return 1;

}


/**------------------------------------------------------------------------**/
/**---non_blocking_write --------------------------------------------------**/
/**------------------------------------------------------------------------**/

no_block_write(int blocks,char* buffer)
{
int data_read_size=256;

int fd;
char card_filename[21];

int ret,i;

strcpy(card_filename,"bu00:");
strcat(card_filename,"testfile"); 


if((fd=open(card_filename,O_WRONLY))>=0)
	{
	printf("ERROR: that file already exists \n");
	return 0;
	}
 
/****** attempt to open, create and close(1) a file of size blocks  **/
if( (fd=open(card_filename,O_CREAT|blocks<<16))==-1)
	{	
	printf("ERROR: could not create file\n");
	return 0;
	}
else
	{
	close(fd);
	}

printf("\n");
printf("********************************************************\n");
printf("non blocking write file: %s  blocks %d\n",card_filename,blocks);
printf("********************************************************\n");

v_count=0;
printf("********>write begins!\n");

if((fd=open(card_filename,O_WRONLY|O_NOWAIT))>=0)
	{
	for(i=0; i< ((8192*blocks)/data_read_size);i++)
		{
      cdb = (cdb==db)? db+1: db;			
      ClearOTag(cdb->ot, OTSIZE);		
	
      FntPrint("\n\n\n");

      FntPrint("    developers conference 1996 memory card demo program\n");
      FntPrint("    ***************************************************\n");

      FntPrint("    non blocking access\n");
      FntPrint("    *******************\n");

      FntPrint("\n\n");
      FntPrint("    non blocking write\n");

      _clear_event();
		while((ret = write(fd,&buffer[i*data_read_size],data_read_size))!=0);
		ret = _card_event();
		if (ret==1)
			{
			printf("error: ?\n");
			exit(0);
			}
		
      animate_marvin();
      FntFlush(-1);
      DrawSync(0);						
      VSync(0);							
      PutDrawEnv(&cdb->draw); 		
      PutDispEnv(&cdb->disp); 		
      DrawOTag(cdb->ot);	 
      }
	close(fd);
	}
else
	{
	printf("error: could not open file!\n");
	exit(0);
	}

printf("********>write ends after %d vblanks or %d secs!\n",v_count,v_count/60);
elapsed_time = v_count;

return 1;
}

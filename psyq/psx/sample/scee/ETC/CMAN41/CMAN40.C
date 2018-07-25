/*****************************************************************
 *		Lib Rev 4.0												 *
 *																 *
 *		Filename:		card.c									 *
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

// set up the double buffer data structure

#include <sys/types.h>
#include <sys/file.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsnd.h>
#include <libspu.h>				 
#include <kernel.h>
#include <libgs.h>
#include "ctrller.h"

// define the button return values

#define MAIN_VOL				127
#define SINGLE_BLOCK			8192

#define	BUTTON_X				1
#define	BUTTON_SQUARE			2
#define	BUTTON_CIRCLE			3
#define	BUTTON_TRIANGLE			4
#define	BUTTON_DOWN				5
#define	BUTTON_RIGHT			6
#define	BUTTON_LEFT				7
#define	BUTTON_UP				8
#define	BUTTON_R1				9
#define	BUTTON_L1				10
#define	BUTTON_R2				11
#define	BUTTON_L2				12
#define	BUTTON_START			13
#define	BUTTON_SELECT			14

// define the menu values.. if the menu counter = that value go to that draw function

#define MAIN_MENU				1		
#define SELECT_PORT				2		
#define DISPLAY_FILE_LIST		3		
#define DISPLAY_TITLE			4		
#define DISPLAY_TYPE			5		
#define QUICK_UNFORMAT			6		
#define UNFORMAT				7		
#define FORMAT					8		
#define DELETE_SAVES			9		
#define CREATE_SAVES			10		
#define FILL_CARD				11		
#define MUSIC_TOGGLE			12		

#define ON_MAIN_MENU  		    13
#define ON_SELECT_PORT			10
#define ON_FILE_LIST			9
#define	ON_SAVE_TITLE			9
#define ON_SAVE_TYPE			7
#define	ON_QUICK_UNFORMAT		4
#define ON_UNFORMAT				4
#define	ON_FORMAT				4
#define ON_DELETE_SAVES			8
#define ON_CREATE_SAVES			17
#define	ON_FILL_CARD			4
#define ON_MUSIC_TOGGLE			4

// some defines for the card status NOTE some of these will change when I am finished

#define EVENT_IOE				0
#define EVENT_ERROR				1
#define EVENT_TIMEOUT			2
#define EVENT_NEWCARD			3

#define PRESENT_CARD_FORMATTED	  0
#define PRESENT_CARD_BAD		  1
#define PRESENT_CARD_NONE		  2
#define PRESENT_CARD_UNFORMATTED  3

#define NEW_CARD_FORMATTED		  4 
#define NEW_CARD_BAD			  5
#define NEW_CARD_NONE			  6 
#define NEW_CARD_UNFORMATTED	  7 

// set up the memory card file headers
// this is a structure for the reading of the memory card saves 

typedef struct 
{
  char			magic[2];
  char			type;
  char			blockEntry;
  char 			title[64];
  char 			reserved[28];
  char			clut[32];
  char			icons[3][128];
} FILE_HEADER;


// this is a structure for the saving of my Cman4.1 save files
typedef struct 
	{
	char				Magic[2];			/* always "SC "*/
	char				Type;				/* 0x11 1 icon 0x12 2icons 0x13 3icons*/
	char				BlockEntry;			/* number of 8k slots used*/
	char				Title[64];			/* title 32 chars SJIS*/
	char				reserve[28];		/* don't use this bit*/
	char			    Clut[32];			/* 4bit clut */  // this may need to be 32
	char				Icon[3][128];	    /* 3*16*16*4bit icon bitmaps*/
	}_CARD;

struct DIRENTRY 	dirEntries[15];			// for reading the cards
FILE_HEADER			fileHeader[15];			// for reading the cards
_CARD 			    SaveFiles[15];			// for card writing buffer

// define for the card status type stuff

typedef struct {
	POLY_FT4			save_icons[3];    // for drawing the Icons
	POLY_FT4			save_icons_anim[3];
	DRAWENV				draw;					  
	DISPENV				disp;
} DB;

// variables for the music

extern u_char           vabHeader[];
extern u_char           vabBody[];
extern u_long           seqAddr[];
char           		 	seqTable[SS_SEQ_TABSIZ * 1 * 1];
short           		vab;
short           		seq;

extern u_long           IconData[];

RECT		timPos[1] =			// set the rectangle for loading in the icon data
{
640,0,4,48
};
	
// some variables for use with the memory cards
long ev0,ev1,ev2,ev3,ev10,ev11,ev12,ev13;

// set varibales for the controller buffers

ControllerPacket    buffer1,buffer2;
TapCtrllerData      *MtapCon;

// some general looping variables
int menu=1,ret,lastbutton;

// initalize the rectangle for the loading of font.tim and the looping variables for the UVWH
RECT		rect;

// declair the main menu text string data. NOTE  this data file can be accessed on a charactar by charactor basis
// to access the S out of delete Saves.  treat the structure as a 2D array IE main_menu_text_data[7][8] will be the 
// afore mentioned S.

char *main_menu_text_data[ON_MAIN_MENU] = { "Main Menu",
											" ",
											"Select Port",
								  			"Display File List",
								  			"Display Save Title",
								  			"Display Save Type",
								  			"Quick Unformat",
								  			"UnFormat",
								  			"Format",
								  			"Delete Saves",
								  			"Create Saves",
								  			"Fill Card With Saves",
								  			"Music Toggle",
								  			};

char *select_port_text_data[ON_SELECT_PORT] = { "Please Select A port.",
								   				" ",
								   				"1A or 1",
								   				"1B",
								   				"1C",
								   				"1D",
								   				"2A or 2",
								   				"2B",
								   				"2C",
								   				"2D"
								   				}; 

char *display_file_list_text_data[ON_FILE_LIST] = { "File List      ",
													" ",
													"File Number	",
										 			"File Name		",
													"File Size		",
										 			"file Head		",
										 			"Magic			",
										 			"File Blocks	",
										 			"Use Directional Buttons"
										 			}; 

char *display_save_title_text_data[ON_SAVE_TITLE] = { "Title's",
													  " ",
													  "File Number	",
							    		  			  "File Name	",
							    		  			  "Title (ASCII)",
							    		  			  "	",
													  "	",
													  "	",
							    		  			  "Title (SJIS)"
							    		  			  }; 

char *display_save_type_text_data[ON_SAVE_TYPE] = { "Save Type",
													" ",
													"File Number	",
													"File Name					",
							   			 			"Save Type 					",
							   			 			"Icon Animantions			",
							   			 			"Title (SJIS) Below 		",
							   			  			}; 

								    
char *quickunformat_text_data[ON_QUICK_UNFORMAT] = { "Quick UnFormat The Card",
														   " ",
														   "start  = Yes",
														   "select = return"
														   };

	
char *unformat_text_data[ON_UNFORMAT] = { "UnFormat The Card",
												  "	",
												  "start  = Yes",
												  "select = return"
												  };

char *format_text_data[ON_FORMAT] = { "Format The Card",
											  " ",
											  "start  = Yes",
											  "select = return"
											  };

char *delete_save_text_data[ON_DELETE_SAVES] = { "Delete A Save",
												 " ",
												 "File Number",											   				 
												 "File Name",
								   				 "Title (SJIS)",
								   				 " ",
												 "start  = Yes",
											  	 "select = return"
								   				 };	

char *create_save_text_data[ON_CREATE_SAVES] = { "Create Saves",
												 " ",
												 "1 Block of 1",
												 "1 Block of 2",
												 "1 Block of 3",
												 "1 Block of 4",
												 "1 Block of 5",
												 "1 Block of 6",
												 "1 Block of 7",
												 "1 Block of 8",
												 "1 Block of 9",
												 "1 Block of 10",
												 "1 Block of 11",
												 "1 Block of 12",
												 "1 Block of 13",
												 "1 Block of 14",
												 "1 Block of 15"
												 };	

char *fill_card_with_saves_text_data[ON_FILL_CARD] = { "Fill The Card With Saves.",
											 		   " ",
											 		   "Start = Yes",
											 		   "Select = No"
											 		   }; 

char *music_toggle_text_data[ON_MUSIC_TOGGLE] = { "Select Music",
									  			  " ",
									  			  "On",
									  			  "Off",
									  			  };  


int selector=2;			// the selector for the menu;
char port;
int oldmenu,Status;			// this is to stop the code access the menu instantly
char oldport;
int NumberOfFiles,called,fileNumber;
// names for the differet text buffers
long menus,contents,messages;					// names for the text windows

// prototypes

int  block_write(int blocks, _CARD *buffer);
void CardMessages(void);
void DrawScreen(void);
int  CheckButtonAndRespond(void);
int  ReadContInfo(void);
void init_icons(DB *b);
void initAnim(DB *b);
void DrawMenuSelector(char *MenuData[0], int max);
void LimitSelector(int max);
void Incriment_Menu(void);
void select_port(void);
int  create_saves(void);
int  fill_card(void);
void SetUpCardHeader(void);
int  FileExistsOnCard (char *fileName);
void Music_Toggle(void);
int  start_music(void);
void stop_music(void);
void InitializeCardAndEvents(int shared);
int  GetCardStatus (long channel);
int  ClearCardEventsSw(void); 
int  GetCardEventSw(void);
int  GetCardEventHw(void);
int  ClearCardEventsHw (void);
int  FormatCard (long channel);
int  UnFormatCard(long channel);
int  QuickUnFormatCard (long channel);
int  DeleteFileFromCard (long channel, char *fileName);
int  GetFileHeaderFromCard (long channel, char *fileName, FILE_HEADER *fileHeader);

DB 		db[2];
DB		*cdb;


//*********************************************************************************************************
//*********************************************************************************************************

main()
{
int CheckCard;
int animate;
ResetCallback();

InitializeCardAndEvents(0);

InitTAP(&buffer1, MAX_CONTROLLER_BYTES, &buffer2, MAX_CONTROLLER_BYTES);
StartTAP();

ResetGraph(0);
SetGraphDebug(1);

SetDefDrawEnv(&db[0].draw,0,0,640,256);
SetDefDrawEnv(&db[1].draw,0,256,640,256);
SetDefDispEnv(&db[0].disp,0,256,640,256);
SetDefDispEnv(&db[1].disp,0,0,640,256);

db[0].draw.isbg = 1;
db[1].draw.isbg = 1;

setRGB0(&db[0].draw ,0,0,0);
setRGB0(&db[1].draw ,0,0,0);

port = 0x00;

// this is only used for the offical title (SJIS)
KanjiFntOpen (100,180, 400,500, 704,256, 704,450, 0,960);  

// this is used for the general Fnt
FntLoad(640,256);
menus = FntOpen(100,53,350,200,0,256);
contents = FntOpen(300,53,350,200,0,256);
messages = FntOpen(256,25,350,200,0,256);

cdb = &db[0];
SetDispMask(1);

CheckCard = 0;

animate = 0;

while(1) // this is changed for a test
{
ret = ReadContInfo();
	
	if(menu == SELECT_PORT)					DrawMenuSelector(&select_port_text_data[0],ON_SELECT_PORT);	
	if(menu == MAIN_MENU)					DrawMenuSelector(&main_menu_text_data[0],ON_MAIN_MENU);			//menu_main_menu();					
	if(menu == MUSIC_TOGGLE)				DrawMenuSelector(&music_toggle_text_data[0],ON_MUSIC_TOGGLE);
	if(menu == DISPLAY_FILE_LIST)			DrawMenu(&display_file_list_text_data[0],ON_FILE_LIST);			
	if(menu == DISPLAY_TITLE)				DrawMenu(&display_save_title_text_data[0],ON_SAVE_TITLE);		
    
	if(menu == DISPLAY_TYPE)				
	    {
			DrawMenu(&display_save_type_text_data[0],ON_SAVE_TYPE);
				
			setRECT(&rect, timPos[0].x, timPos[0].y, timPos[0].w, timPos[0].h);
  						
  			LoadImage(&rect, (u_long *)fileHeader[fileNumber].icons);
			LoadClut((u_long *)&fileHeader[fileNumber].clut,704,0);

			init_icons(&db[0]);
			init_icons(&db[1]);
			
			FntPrint(contents,"\n\n\n%d", fileNumber+1);
			FntPrint(contents,"\n%s",dirEntries[fileNumber].name);
			FntPrint(contents,"\n0x%x",fileHeader[fileNumber].type);
			KanjiFntPrint(" %s ",fileHeader[fileNumber].title);

         	// draw the correct amount of icons ( 1,2 or 3 ) depending on the type of save
 			
 			DrawPrim(&cdb->save_icons[0]);	  // show the first save
			if(fileHeader[fileNumber].type == 0x12 || fileHeader[fileNumber].type == 0x13)	DrawPrim(&cdb->save_icons[1]);  // show the second save ?
			if(fileHeader[fileNumber].type == 0x13)	DrawPrim(&cdb->save_icons[2]);  // show the third save ?//
			
			//animate the selected memory card icon
			if(fileHeader[fileNumber].type == 0x11)   if(animate >= 7)     animate = 0;
			if(fileHeader[fileNumber].type == 0x12)   if(animate >= 14)    animate = 0;
			if(fileHeader[fileNumber].type == 0x13)   if(animate >= 22)    animate = 0;
				 
			if(animate >= 0  && animate <= 7)		DrawPrim(&cdb->save_icons_anim[0]);	  // show the first save
			if(fileHeader[fileNumber].type == 0x12 || fileHeader[fileNumber].type == 0x13) if(animate > 7  && animate <= 14)		DrawPrim(&cdb->save_icons_anim[1]);	  // show the second save
			if(fileHeader[fileNumber].type == 0x13) if(animate > 14 && animate <= 21)	   DrawPrim(&cdb->save_icons_anim[2]);	  // show the third save
		}

	if(menu == QUICK_UNFORMAT)				
		{
			DrawMenu(&quickunformat_text_data[0],ON_QUICK_UNFORMAT);
			if(ret == BUTTON_START)				QuickUnFormatCard(port);
			if(ret == BUTTON_SELECT)			menu = 1;
		}

	if(menu == UNFORMAT)					
		{
			DrawMenu(&unformat_text_data[0],ON_UNFORMAT);
			if(ret == BUTTON_START)				UnFormatCard(port);
			if(ret == BUTTON_SELECT)			menu = 1;
		}

	if(menu == FORMAT)						
		{
			DrawMenu(&format_text_data[0],ON_FORMAT);
			if(ret == BUTTON_START)				FormatCard(port);
			if(ret == BUTTON_SELECT)			menu = 1;
		}

	if(menu == DELETE_SAVES)				
		{
			DrawMenu(&delete_save_text_data[0],ON_DELETE_SAVES);
			if(ret == BUTTON_START)				DeleteFileFromCard(port,dirEntries[fileNumber].name);
			if(ret == BUTTON_SELECT)			menu = 1;
		}

	if(menu == CREATE_SAVES)				DrawMenuSelector(&create_save_text_data[0],ON_CREATE_SAVES);

	if(menu == FILL_CARD)					
		{
		DrawMenu(&fill_card_with_saves_text_data[0],ON_FILL_CARD);

			if(ret == BUTTON_START)				fill_card();
			if(ret == BUTTON_SELECT)			menu = 1;
		}

	if(ret != lastbutton)					CheckButtonAndRespond();	// check the buttons and respond accordingly

  	if(port != oldport)					
  		{
			Status = GetCardStatus(port);         
			if(Status = 2)    called = 0;
  		}

	if(called != 1)
	{
	NumberOfFiles = ReadSample(&dirEntries[0]);			
	printf("\n number of files = %d ",NumberOfFiles);
	called = 1;
	}
	// this is a test and will be placed in a better position

	if(menu == DISPLAY_FILE_LIST)
		{
			FntPrint(contents,"\n\n\n%d", fileNumber+1);
			FntPrint(contents,"\n%s",dirEntries[fileNumber].name);
			FntPrint(contents,"\n%d",dirEntries[fileNumber].size);
			FntPrint(contents,"\n%d",dirEntries[fileNumber].head);
			FntPrint(contents,"\n%c%c",fileHeader[fileNumber].magic[0],fileHeader[fileNumber].magic[1]);
			FntPrint(contents,"\n%d",fileHeader[fileNumber].blockEntry);
		}

	if(menu == DISPLAY_TITLE )
	   {
			FntPrint(contents,"\n\n\n%d", fileNumber+1);
			FntPrint(contents,"\n%s",dirEntries[fileNumber].name);
			FntPrint(contents,"\n%s ",fileHeader[fileNumber].title);
			KanjiFntPrint(" %s ",fileHeader[fileNumber].title);
	   }
	   
	if(menu == DELETE_SAVES)
	   {
			FntPrint(contents,"\n\n\n%d", fileNumber);
			FntPrint(contents,"\n%s",dirEntries[fileNumber].name);
			KanjiFntPrint(" %s ",fileHeader[fileNumber].title);
	   }

	if(menu == 1)		fileNumber = 0;

	if(CheckCard == 10)
	{
		Status = GetCardStatus(port);         
		CheckCard=0;
	}
	
	CardMessages();

CheckCard++;

lastbutton = ret;	  
oldmenu = menu;
oldport = port;

VSync(0);				  
DrawScreen();

animate++;
}

}//end of the main function..

//*********************************************************************************************************
//*********************************************************************************************************

void CardMessages(void)
{
if(Status == 0)			FntPrint(messages,"Card Present");
if(Status == 1)			FntPrint(messages,"Bad Card");
if(Status == 2)			FntPrint(messages,"Please Enter A Card");
}

//*********************************************************************************************************
//*********************************************************************************************************

void DrawScreen(void)
{
cdb = (cdb==db) ? db + 1 : db;
PutDrawEnv(&cdb->draw);
PutDispEnv(&cdb->disp);

FntFlush(menus);
FntFlush(contents);
FntFlush(messages);
KanjiFntFlush(-1);
DrawSync(0);
}

//*********************************************************************************************************
//*********************************************************************************************************

int CheckButtonAndRespond(void)
{

	if(ret == BUTTON_X && menu == 1)		
	{
	Incriment_Menu();     
	printf(" \n inc menu = %d ",menu);
	}
	if(ret == BUTTON_X && menu == 2)		
		{
			if(menu == oldmenu) 		
			{
			select_port();     
			return 0;
			}

		}
	if(ret == BUTTON_X && menu == 3 && NumberOfFiles == 0)				menu = 1;	// restrict the menu when there are no saves	     
	if(ret == BUTTON_X && menu == 4 && NumberOfFiles == 0)				menu = 1;	// restrict the menu when there are no saves	     
	if(ret == BUTTON_X && menu == 5 && NumberOfFiles == 0)				menu = 1;	// restrict the menu when there are no saves
	if(ret == BUTTON_X && menu == 6 && NumberOfFiles == 0)				menu = 1;	// restrict the menu when there are no saves
	if(ret == BUTTON_X && menu == 9 && NumberOfFiles == 0)				menu = 1;	// restrict the menu when there are no saves
	if(ret == BUTTON_X && menu == 10)		if(menu == oldmenu)			create_saves();     
	if(ret == BUTTON_X && menu == 12)		if(menu == oldmenu)			Music_Toggle();


	if(ret == BUTTON_TRIANGLE)		
	{
		if(menu > 1)    
		{
			menu = 1;						    
			selector = 2;					   
		}	    
	}

	if(ret == BUTTON_UP)			selector--;
	if(ret == BUTTON_DOWN)			selector++;

	
	if(ret == BUTTON_LEFT)			
	{
		fileNumber++;
		if(fileNumber >= NumberOfFiles)		fileNumber = NumberOfFiles-1; 
	}
	
	if(ret == BUTTON_RIGHT)			
	{
		fileNumber--;
		if(fileNumber <= 0)		fileNumber = 0;
	}

}
//*********************************************************************************************************
//*********************************************************************************************************

// this is a function to read all of the controller info ( pad's and the multiTap )
// this will return 1 to 14 depending on which button is be pressed.. This can be limited to a 
// lower number if the code only, say, uses START and X just look for START and X
 
int ReadContInfo(void)
{
unsigned char contType;
if(GoodData(&buffer1))
     {
	     switch(GetType(&buffer1))
		      {
			       // case 4 is a standard pad..
			       case 4:    
						 if(PadKeyIsPressed(&buffer1,PAD_RD)!=0)	   return BUTTON_X;			   
						 if(PadKeyIsPressed(&buffer1,PAD_RL)!=0)	   return BUTTON_SQUARE;	   
						 if(PadKeyIsPressed(&buffer1,PAD_RR)!=0)	   return BUTTON_CIRCLE;	   
						 if(PadKeyIsPressed(&buffer1,PAD_RU)!=0)	   return BUTTON_TRIANGLE;	   
						 if(PadKeyIsPressed(&buffer1,PAD_LD)!=0)	   return BUTTON_DOWN;		   
						 if(PadKeyIsPressed(&buffer1,PAD_LL)!=0)	   return BUTTON_LEFT;		   
						 if(PadKeyIsPressed(&buffer1,PAD_LR)!=0)	   return BUTTON_RIGHT;		   
						 if(PadKeyIsPressed(&buffer1,PAD_LU)!=0)	   return BUTTON_UP;		   
						 if(PadKeyIsPressed(&buffer1,PAD_START)!=0)	   return BUTTON_START;		   
						 if(PadKeyIsPressed(&buffer1,PAD_SEL)!=0)	   return BUTTON_SELECT;		   
				   break;

				   case 8:
				       
					   MtapCon = GetTapData(&buffer1,0);
					   contType = GetType(MtapCon);

							switch(contType)
							      {

								  case 4:
									   if(MultiPortAIsPressed(&buffer1,PAD_RD)!=0)		return BUTTON_X;		   
									   if(MultiPortAIsPressed(&buffer1,PAD_RL)!=0)		return BUTTON_SQUARE;	     
									   if(MultiPortAIsPressed(&buffer1,PAD_RR)!=0)		return BUTTON_CIRCLE;	     
									   if(MultiPortAIsPressed(&buffer1,PAD_RU)!=0)		return BUTTON_TRIANGLE;	     
									   if(MultiPortAIsPressed(&buffer1,PAD_LD)!=0)		return BUTTON_DOWN;		   
									   if(MultiPortAIsPressed(&buffer1,PAD_LL)!=0)		return BUTTON_LEFT;		   
									   if(MultiPortAIsPressed(&buffer1,PAD_LR)!=0)		return BUTTON_RIGHT;	   	  
									   if(MultiPortAIsPressed(&buffer1,PAD_LU)!=0)		return BUTTON_UP;		   
									   if(MultiPortAIsPressed(&buffer1,PAD_START)!=0)	return BUTTON_START;		   
									   if(MultiPortAIsPressed(&buffer1,PAD_SEL) != 0)	return BUTTON_SELECT;		   
								  }
			  }
	 }
return 0;		//if none of the buttons have been pressed return 0
}

//*********************************************************************************************************
//*********************************************************************************************************

void init_icons(DB *b)
{
  
  SetPolyFT4(&b->save_icons[0]);
  SetShadeTex(&b->save_icons[0],1);
  setXYWH(&b->save_icons[0],100,150,40,20);  // alter the position of the highlight 
  setRGB0(&b->save_icons[0],255,0,0);
  setUVWH(&b->save_icons[0],0,0,15,16);
  b->save_icons[0].tpage = getTPage(0,0,640,0);
  b->save_icons[0].clut  = GetClut(704,0);
  SetSemiTrans(&b->save_icons[0],0);

  SetPolyFT4(&b->save_icons[1]);
  SetShadeTex(&b->save_icons[1],1);
  setXYWH(&b->save_icons[1],200,150,40,20);  // alter the position of the highlight 
  setRGB0(&b->save_icons[1],255,0,0);
  setUVWH(&b->save_icons[1],0,16,15,16);
  b->save_icons[1].tpage = getTPage(0,0,640,0);
  b->save_icons[1].clut  = GetClut(704,0);
  SetSemiTrans(&b->save_icons[1],0);
  
  SetPolyFT4(&b->save_icons[2]);
  SetShadeTex(&b->save_icons[2],1);
  setXYWH(&b->save_icons[2],300,150,40,20);  // alter the position of the highlight 
  setRGB0(&b->save_icons[2],255,0,0);
  setUVWH(&b->save_icons[2],0,32,15,16);
  b->save_icons[2].tpage = getTPage(0,0,640,0);	  // 0,0,640,256
  b->save_icons[2].clut  = GetClut(704,0);		  // 704,256
  SetSemiTrans(&b->save_icons[2],0);

  SetPolyFT4(&b->save_icons_anim[0]);
  SetShadeTex(&b->save_icons_anim[0],1);
  setXYWH(&b->save_icons_anim[0],400,150,40,20);  // alter the position of the highlight 
  setRGB0(&b->save_icons_anim[0],255,0,0);
  setUVWH(&b->save_icons_anim[0],0,0,15,16);
  b->save_icons_anim[0].tpage = getTPage(0,0,640,0);
  b->save_icons_anim[0].clut  = GetClut(704,0);
  SetSemiTrans(&b->save_icons_anim[0],0);
  
  SetPolyFT4(&b->save_icons_anim[1]);
  SetShadeTex(&b->save_icons_anim[1],1);
  setXYWH(&b->save_icons_anim[1],400,150,40,20);  // alter the position of the highlight 
  setRGB0(&b->save_icons_anim[1],255,0,0);
  setUVWH(&b->save_icons_anim[1],0,16,15,16);
  b->save_icons_anim[1].tpage = getTPage(0,0,640,0);
  b->save_icons_anim[1].clut  = GetClut(704,0);
  SetSemiTrans(&b->save_icons_anim[1],0);
  
  SetPolyFT4(&b->save_icons_anim[2]);
  SetShadeTex(&b->save_icons_anim[2],1);
  setXYWH(&b->save_icons_anim[2],400,150,40,20);  // alter the position of the highlight 
  setRGB0(&b->save_icons_anim[2],255,0,0);
  setUVWH(&b->save_icons_anim[2],0,32,15,16);
  b->save_icons_anim[2].tpage = getTPage(0,0,640,0);
  b->save_icons_anim[2].clut  = GetClut(704,0);
  SetSemiTrans(&b->save_icons_anim[2],0);
}

//*********************************************************************************************************
//*********************************************************************************************************

void DrawMenuSelector(char *MenuData[0], int max)
{
// this is a function to limit the movment of the selector within the confiles of 1- 15 for saves and the 
// size of the menu's
int loop;
LimitSelector(max);

	for(loop=0; loop<max; loop++)
	{
		if(loop == selector)		FntPrint(menus,"\n* %s",MenuData[loop]);
		else 						FntPrint(menus,"\n  %s",MenuData[loop]);
	}
}	  

//*********************************************************************************************************
//*********************************************************************************************************

DrawMenu(char *MenuData[0], int max)
{
int loop;
LimitSelector(max);

	for(loop=0; loop<max; loop++)
	{
		FntPrint(menus,"\n  %s",MenuData[loop]);
	}
}

//*********************************************************************************************************
//*********************************************************************************************************

void LimitSelector(int max)
{

if(selector < 2)   selector = max-1;
if(selector > max-1)   selector = 2;

}

//*********************************************************************************************************
//*********************************************************************************************************

void Incriment_Menu(void)
{
menu = selector;
if(NumberOfFiles != 0)		selector = 2;
}

//*********************************************************************************************************
//*********************************************************************************************************

void select_port(void)
{
	if((selector-1) == 1)		port = 0x00; 
	if((selector-1) == 2)		port = 0x01; 
	if((selector-1) == 3)		port = 0x02; 
	if((selector-1) == 4)		port = 0x03; 
	if((selector-1) == 5)		port = 0x10; 
	if((selector-1) == 6)		port = 0x11; 
	if((selector-1) == 7)		port = 0x12; 
	if((selector-1) == 8)		port = 0x13; 
	menu = 1;		// return to the main menu
}

//*********************************************************************************************************
//*********************************************************************************************************

int create_saves(void)
{
int BlocksFree = 15,i;
selector -= 1;
//memory -= NumberOfFiles;

for(i=0; i<15; i++)
   {
       BlocksFree -= fileHeader[i].blockEntry;
   }

if(BlocksFree < selector)			
{

DrawScreen();   // clears the screen

FntPrint(messages,"Not Enough Space");
DrawScreen();
FntPrint(messages,"Not Enough Space");
DrawScreen();

menu = 1;		// set the menu back to main menu selector
called = 0;		// reload the memory card information
selector += 1;

VSync(120);
return 0;

}

SetUpCardHeader();

menu = 1;		// set the menu back to main menu selector
called = 0;		// reload the memory card information
selector += 1;
}

//*********************************************************************************************************
//*********************************************************************************************************

int fill_card(void) 
{	
int BlocksFree=15;
int i;
selector -= 1;

for(i=0; i<15; i++)
   {
       BlocksFree -= fileHeader[i].blockEntry;
   }

if(BlocksFree < selector)			
{

DrawScreen();   // clears the screen

FntPrint(messages,"Not Enough Space");
DrawScreen();
FntPrint(messages,"Not Enough Space");
DrawScreen();

menu = 1;		// set the menu back to main menu selector
called = 0;		// reload the memory card information
selector += 1;

VSync(120);
return 0;
}

for(i=0; i<BlocksFree; i++)
{
SetUpCardHeader();
}

menu = 1;		// set the menu back to main menu
called = 0;	// set the menu back to the main menu selector
selector += 1;
}					  

//*********************************************************************************************************
//*********************************************************************************************************

void SetUpCardHeader(void)
{
GsIMAGE tim;
char FileBuffer[selector*SINGLE_BLOCK];

SaveFiles[0].Magic[0] = 'S';
SaveFiles[0].Magic[1] = 'C';
		
if(selector == 1) 		SaveFiles[0].Type = 0x11;
if(selector == 2) 		SaveFiles[0].Type = 0x12;
if(selector >= 3) 		SaveFiles[0].Type = 0x13;

SaveFiles[0].BlockEntry = selector;  // the 1 will be replaced with selector when code is working

strtosjis("CMAN SAVE(S)",SaveFiles[0].Title);

GsGetTimInfo(IconData+1, &tim);

memcpy(SaveFiles[0].Clut, tim.clut,32);
memcpy(SaveFiles[0].Icon, tim.pixel,384);

block_write(selector,&SaveFiles[0]);
}

//*********************************************************************************************************
//*********************************************************************************************************

int block_write(int blocks, _CARD *buffer)
{
int fd;
int incrim=0;
int fileOnCard,i;
char drive[6];
char card_filename[20];		// should be 20 or under

if(port == 0x00)		strcpy(drive,"bu00:");
if(port == 0x01)		strcpy(drive,"bu01:");
if(port == 0x02)		strcpy(drive,"bu02:");
if(port == 0x03)		strcpy(drive,"bu03:");
if(port == 0x10)		strcpy(drive,"bu10:");
if(port == 0x11)		strcpy(drive,"bu11:");
if(port == 0x12)		strcpy(drive,"bu12:");
if(port == 0x13)		strcpy(drive,"bu13:");

// find out what the end number should be for the save name by trying to open the file, if it exists incriment
// incrim and try agian..

sprintf(card_filename,"%sCMAN4.1.%d",drive,incrim);
do
{
		sprintf(card_filename,"%sCMAN4.1.%d",drive,incrim);
		FntPrint(messages,"Checking the file Name");
		DrawScreen();
		
		fileOnCard = FileExistsOnCard (card_filename);
		if(fileOnCard == 1) 		
		  {
			incrim++;
		  }
		  else
		  {
		  i = 1;
		  }

}while(i != 1);	

if((fd=open(card_filename,O_CREAT|(blocks<<16)))==-1)
	{	
	DrawScreen();
	FntPrint("error: could not create file \n");
	FntPrint("Please Format the Card And Try Again\n");
	DrawScreen();
	FntPrint("error: could not create file \n");
	FntPrint("Please Format the Card And Try Again\n");
	DrawScreen();
	VSync(120);			// pause for 2 seconds
	return 0;
	}

		// the messages fail to appear unless I put them Twice
		FntPrint(messages,"Writing to the Card");
		DrawScreen();	
		FntPrint(messages,"Writing to the Card");
		DrawScreen();	

	i = write(fd,buffer,(blocks*8192));
	if(i!=(blocks*8192)) 
		{
		DrawScreen();		// to clear the screen
		FntPrint(messages,"error: failed whilst writing data\n"); 
		DrawScreen();
		FntPrint(messages,"error: failed whilst writing data\n"); 
		DrawScreen();
		
		close(fd);
		return 0;
		}
 	close(fd);
}

//*********************************************************************************************************
//*********************************************************************************************************

int FileExistsOnCard (char *fileName)
{
  int 		file;

  file = open (fileName, O_WRONLY);
  if (file >= 0)
  {
    close (file);
    return 1;
  }
  close (file);
  return 0;
}

//*********************************************************************************************************
//*********************************************************************************************************

void Music_Toggle(void)
{
	if(selector-1 == 1)				start_music();
	if(selector-1 == 2)				stop_music();
	menu = 1;		// return to the main menu once the function has been compleated
}

//*********************************************************************************************************
//*********************************************************************************************************

int start_music(void)
{
  SsInit () ;              
  SsSetTableSize ( seqTable, 1, 1 ) ;		  
  SsSetTickMode ( SS_TICK240 ) ; 
												    
  vab = SsVabOpenHead ( vabHeader, -1 ) ; 
  if (vab < 0)
  {
    printf ("SsVabOpenHead failed\n") ;
    return 0 ;
  }

  SsVabTransBody(vabBody,vab);
  SsVabTransCompleted(SS_WAIT_COMPLETED);
  seq = SsSeqOpen(seqAddr,vab);

  SsStart();
  SsSetMVol(MAIN_VOL, MAIN_VOL);
							    
  SsSeqSetVol(seq, MAIN_VOL, MAIN_VOL);
  SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY);

  SsUtSetReverbType(SPU_REV_MODE_ECHO);
  SsUtReverbOn();
}
//*********************************************************************************************************
//*********************************************************************************************************

void stop_music(void)
{
	SsSeqStop(seq);
  	SsSeqClose(seq);

  	SsEnd();
  	SsQuit();
}

//*********************************************************************************************************
//*********************************************************************************************************

ReadSample(struct DIRENTRY *d)	  // reads the file directory
{
int i,p;
char key[128];
extern struct DIRENTRY *firstfile(), *nextfile();
		 
		 VSync(3);
         ClearCardEventsHw();
         _card_clear(port);
         GetCardEventHw();
         
         ClearCardEventsSw();
         _new_card();
         _card_load(port);
         GetCardEventSw();

        if(port == 0x00)		strcpy(key, "bu00:");
        if(port == 0x01)		strcpy(key, "bu01:");
        if(port == 0x02)		strcpy(key, "bu02:");
        if(port == 0x03)		strcpy(key, "bu03:");
        if(port == 0x10)		strcpy(key, "bu10:");
        if(port == 0x11)		strcpy(key, "bu11:");
        if(port == 0x12)		strcpy(key, "bu12:");
        if(port == 0x13)		strcpy(key, "bu13:");     
        strcat(key, "*");		   
        
        i=0;
			if(firstfile(key,d)==d)
            	  {
                	 do
                  	 {
                         i++;
                       	 d++;
                  	 }
                     while(nextfile(d)==d);
             	  }
            
            for(p=0; p<i; p++)
                 {
                      GetFileHeaderFromCard(port,dirEntries[p].name, &fileHeader[p]);
                 }
return i;
}

//*********************************************************************************************************
//*********************************************************************************************************

void InitializeCardAndEvents(int shared)
{
  EnterCriticalSection();
  InitCARD(shared);
  ExitCriticalSection();
  StartCARD();
  _bu_init();

  EnterCriticalSection();
  ev0 = OpenEvent(SwCARD, EvSpIOE, EvMdNOINTR, NULL) ;
  ev1 = OpenEvent(SwCARD, EvSpERROR, EvMdNOINTR, NULL) ;
  ev2 = OpenEvent(SwCARD, EvSpTIMOUT, EvMdNOINTR, NULL) ;
  ev3 = OpenEvent(SwCARD, EvSpNEW, EvMdNOINTR, NULL) ;
  ev10 = OpenEvent(HwCARD, EvSpIOE, EvMdNOINTR, NULL) ;
  ev11 = OpenEvent(HwCARD, EvSpERROR, EvMdNOINTR, NULL) ;
  ev12 = OpenEvent(HwCARD, EvSpTIMOUT, EvMdNOINTR, NULL) ;
  ev13 = OpenEvent(HwCARD, EvSpNEW, EvMdNOINTR, NULL) ;
  ExitCriticalSection();
				   
  EnableEvent(ev0);
  EnableEvent(ev1);
  EnableEvent(ev2);
  EnableEvent(ev3);
  EnableEvent(ev10);
  EnableEvent(ev11);
  EnableEvent(ev12);
  EnableEvent(ev13);
}

//*********************************************************************************************************
//*********************************************************************************************************

// This function stops all of the memory card related interupts and events
// WARNING: StopCARD() destroys interupts, so please don't call casualy
void DeInitializeCardAndEvents (void) 
{
  StopCARD();
  
  DisableEvent(ev0);
  DisableEvent(ev1);
  DisableEvent(ev2);
  DisableEvent(ev3);
  DisableEvent(ev10);
  DisableEvent(ev11);
  DisableEvent(ev12);
  DisableEvent(ev13);

  CloseEvent(ev0);
  CloseEvent(ev1);
  CloseEvent(ev2);
  CloseEvent(ev3);
  CloseEvent(ev10);
  CloseEvent(ev11);
  CloseEvent(ev12);
  CloseEvent(ev13);
}

//*********************************************************************************************************
//*********************************************************************************************************

int GetCardStatus (long channel) 
{
int 			ret ;

  // as the memory cards read on a VSync port 1 then 2,1,2,1,2 etc
  VSync(3);
  ClearCardEventsSw () ;
  _card_info (channel) ;
  ret = GetCardEventSw () ;

  switch (ret)
  {
    case EVENT_IOE:
            return PRESENT_CARD_FORMATTED;			// change to PRESENT_CARD_FORMATTED = 0
            break;

    case EVENT_ERROR:
            return PRESENT_CARD_BAD;				// change to PRESENT_CARD_BAD = 1
            break;

    case EVENT_TIMEOUT:
            return PRESENT_CARD_NONE;				// change to PRESENT_CARD_NONE = 2
            break;
  }

  // if The card is newly inserted then use the code below
  ClearCardEventsHw () ;
  _card_clear (channel) ;
  ret = GetCardEventHw () ;

  ClearCardEventsSw () ;
  _card_load (channel) ;
  ret = GetCardEventSw () ;
  switch (ret) 
  {
    case EVENT_IOE:
	  return NEW_CARD_FORMATTED;
          break ;

    case EVENT_ERROR:
	  return NEW_CARD_BAD;
          break ;

    case EVENT_TIMEOUT:
	  return NEW_CARD_NONE;
          break ;

    case EVENT_NEWCARD:
	  return NEW_CARD_UNFORMATTED;
          break ;
  }
  return -1 ;
}

//*********************************************************************************************************
//*********************************************************************************************************

int GetCardEventSw(void)
{
 int err;
  // I have realised that this could enter an infinite loop.  althogh very unlikley a have still pu
  // an error check in the loop
  // the number is so high due to the memory cards accessing on the VSync
  while (err < 200000) 
  {
  	if (TestEvent (ev0) == 1) 		return EVENT_IOE;
    if (TestEvent (ev1) == 1) 		return EVENT_ERROR;
    if (TestEvent (ev2) == 1) 		return EVENT_TIMEOUT;
    if (TestEvent (ev3) == 1) 		return EVENT_NEWCARD;
  }
}

//*********************************************************************************************************
//*********************************************************************************************************

int GetCardEventHw(void)
{
int err;
  while(err <200000) 
  {
    if (TestEvent (ev10) == 1) 		return 0;
    if (TestEvent (ev11) == 1) 		return 1;
    if (TestEvent (ev12) == 1) 		return 2;
    if (TestEvent (ev13) == 1) 		return 3;
  }
}

//*********************************************************************************************************
//*********************************************************************************************************
// this function clears all of the Sw events 

int ClearCardEventsSw(void)
{
  TestEvent (ev0);
  TestEvent (ev1);
  TestEvent (ev2);
  TestEvent (ev3);
}

//*********************************************************************************************************
//*********************************************************************************************************
// this function clears all of the Hw events
 
int ClearCardEventsHw (void)			
{
  TestEvent (ev10);
  TestEvent (ev11);
  TestEvent (ev12);
  TestEvent (ev13);
}

//*********************************************************************************************************
//*********************************************************************************************************

int FormatCard (long channel)
{
  int 		loop;
  char 		drive[16] ;

  menu = 1;		// set the menu back to main menu selector
  called = 0;		//return to the main menu when finnished


  sprintf (drive, "bu%.2x:", channel);
  
  DrawScreen();  // clear the screen
  // this message only appears if pu twice
  FntPrint(messages,"Formatting Card %s ",drive);
  DrawScreen(); 
  FntPrint(messages,"Formatting Card %s ",drive);
  DrawScreen(); 

  for(loop=0; loop<15; loop++)
  {
  	fileHeader[ret].blockEntry = 0;
  }

  return format (drive) ;
}

//*********************************************************************************************************
//*********************************************************************************************************

int UnFormatCard(long channel)
{
  long loop ;
  char buffer[128] ;

  menu = 1;		// set the menu back to main menu selector
  called = 0;		//return to the main menu when finnished

  for (loop=0; loop<128; loop++)		// set up a 128 byte buffer of absolutey nothing
  {
    buffer[loop] = 0xff;
  }
  for (loop=0; loop<1024; loop++) 
  {
    FntPrint(" This Will Take About a minute");
    FntPrint("\n %d Of 1024",loop);				   // print this on screen to prove the code has not stoped
    ClearCardEventsHw () ;
    _new_card () ;
     _card_write(channel, loop, &buffer[0]) ;
    
    if (GetCardEventHw () != 0)
    {
     return 0;
	}

	DrawScreen();
  
  }
  for(ret =0; ret<15; ret++)
  {
  	fileHeader[ret].blockEntry = 0;
  }
  return 1;
}

//*********************************************************************************************************
//*********************************************************************************************************

int QuickUnFormatCard (long channel)
{
	char buffer[128];

    called = 0;		//return to the main menu when finnished
	menu = 1;		// set the menu back to main menu selector

	buffer[0] = buffer[1] = 0xff ;
	ClearCardEventsHw () ;
	_new_card () ;
	_card_write(channel, 0, &buffer[0]) ;

	if (GetCardEventHw () != 0)
	{
		return 0;
	}
	for(ret =0; ret<15; ret++)
  	{
  	   fileHeader[ret].blockEntry = 0;
  	}
	return 1;
}

//*********************************************************************************************************
//*********************************************************************************************************

int DeleteFileFromCard (long channel, char *fileName) 
{
  char		path[64] ;

  called = 0;		//return to the main menu when finnished
  menu = 1;		// set the menu back to main menu selector

  sprintf(path, "bu%.2x:%s", channel, fileName);		// set the file to be deleted
  return delete (path) ;
}

//*********************************************************************************************************
//*********************************************************************************************************

int GetFileHeaderFromCard (long channel, char *fileName, FILE_HEADER *fileHeader)
{
  int			file ;
  char			path[64] ;
  int			count ;


  sprintf (path, "bu%.2x:%s", channel, fileName) ;
  file = open (path, O_RDONLY) ;
  if (file < 0)																					    
  {
	DrawScreen();  		// clear the screen
    
    FntPrint(messages,"ERROR: The file could not be opened\n");
	DrawScreen();
    FntPrint(messages,"ERROR: The file could not be opened\n");
	DrawScreen();
	VSync(120);			// pause for 2 seconds
	return 0 ;			// return an error
  }

  count = read (file, fileHeader, sizeof(FILE_HEADER)) ;
  if (count != sizeof (FILE_HEADER))
  {
	DrawScreen();
    FntPrint("ERROR: During reading   Read: [%d]\n", count);
	DrawScreen();
    FntPrint("ERROR: During reading   Read: [%d]\n", count);
	DrawScreen();
	VSync(120);			// pause for 2 seconds

	close (file);
    return 0;		// return, error
  }
  close (file);
  return 1;			// return, all went well
}

//*********************************************************************************************************
//*********************************************************************************************************


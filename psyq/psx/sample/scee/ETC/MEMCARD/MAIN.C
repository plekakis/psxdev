#/******************************************************************
 *	   	Lib Rev 4.0											 	  *
 *																  *
 *		Filename:		card.c									  *
 *																  *
 *		Author:		    Kevin Thompson						   	  *													
 *																  *
 *		History:												  *
 *			30-05-97	(LPGE)									  *
 *						Created									  *
 *																  * 
 *	    Copyright (c) 1997 Sony Computer Entertainment Europe     * 
 *		  All Rights Reserved									  *
 *																  *
 *		Demonstrate how to use the cards with one port accessible *
 ******************************************************************/	
// set up the double buffer data structure

#include <sys/types.h>
#include <sys/file.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <kernel.h>
#include <libtap.h>
#include <libgs.h>
#include "ctrller.h"
#include "memcard.h"					   


#define OT_SIZE		512
#define MEM_CARD_REFRESH_RATE  20
#define NEW_CARD	4
#define OLD_CARD	0	
#define NO_CARD		3	
#define BACK_POLYS	59		   

#define ICON_WIDTH	28
#define ICON_HEIGHT	14
#define ICON_ANIM_SPEED 10

typedef struct {
	POLY_FT4			save_icons[3];    // for drawing the Icons
	POLY_FT4			save_icons_anim[3];
	DRAWENV				draw;					  
	DISPENV				disp;
	POLY_FT4			IconSet1[15][3];
} DB;

					  
ControllerPacket PadBuffer1,PadBuffer2;

struct DIRENTRY 	dirEntries[15];
FILE_HEADER			fileHeader[15];


extern u_long       IconData[];

// a look up table for the Vram positions of the Card Icons
// for the clut take the same addresses but instead of 0 put a 50 in the 2nd column

RECT				IconPosVR[15] =
{
  640, 0, 4, 48,
  644, 0, 4, 48, 
  648, 0, 4, 48, 
  652, 0, 4, 48, 
  656, 0, 4, 48, 
  660, 0, 4, 48, 
  664, 0, 4, 48, 
  668, 0, 4, 48, 
  672, 0, 4, 48, 
  676, 0, 4, 48, 					    
  680, 0, 4, 48, 
  684, 0, 4, 48,   
  688, 0, 4, 48, 
  692, 0, 4, 48, 
  696, 0, 4, 48
} ;

typedef struct {
	int X1;
	int Y1;
} IconCoOrds;

IconCoOrds				IconPosSCR[15] =
{
 220,  140,  
 280,  140,  
 340,  140,  
  
 220,  110,  
 280,  110,  
 340,  110,  
 
 220,  80,  
 280,  80,  
 340,  80,  												  
 
 220,  50,    
 280,  50,    
 340,  50,    
 
 220,  20,    
 280,  20,    
 340,  20,  
} ;

// Globals and flags==================================================================================

int loop;

int EnterOnce=0;
int FrameNo;
int stat = -1;
int ScreenMessage,OfficialWarnings;
int DisplayMessage;
int FunctionNum;
int NumFilesOnCard;
int NumBlocksOnCard;
int ViewingIconNum=0;
int ScalingIconNum=0;
int NumberFreeBlocks=0;
int Title,Prompts;
int ViewMode=0;
int kill =-1;
// ProtoTypes=========================================================================================

int ReadController(void);
int CheckButtonsAndRespond(int button, int lastbutton, int ScalingIcoNum, int NumFilesOnCard);
int FrameCounterToReadCard(void);	
void ClearFileHeader(FILE_HEADER *fh);
int draw_icons(DB *b, int DisplayIconNumber);
void manipulate_icons(DB *b,int width, int height, int ScalingIconNum);
void present_card_formatted(int NumFilesOnCard);
int new_card_formatted(int NumFilesOnCard, int ret);
void init_other_icons(DB *b, int NumFilesOnCard);
void DisplayWarnings(int DisplayMessageNum, int FuncionNum);
void CheckAndLimitMessageNumbers(void);
void DrawScreen(void);

RECT rect;

//*****************************************************************************************
//*****************************************************************************************

DB 		db[2];
DB		*cdb;

main()
{												    

int ret=0;
int button=0;
int lastbutton=0;
int icon_expand=0, width=ICON_WIDTH, height=ICON_HEIGHT;
int No_Drawing;
int CardStatusFlag=-1;
int DisplayIconNumber=0;
 
ResetCallback();

_mc_InitializeCardAndEventsStandard(NOT_SHARED);

InitPAD((u_char *)&PadBuffer1, MAX_CONTROLLER_BYTES, (u_char *)&PadBuffer2, MAX_CONTROLLER_BYTES);
StartPAD();

//SetVideoMode(MODE_PAL);

ResetGraph(0);
SetGraphDebug(1);									    
					 
SetDefDrawEnv(&db[0].draw,0,0,640,256);
SetDefDrawEnv(&db[1].draw,0,256,640,256);
SetDefDispEnv(&db[0].disp,0,256,640,256);
SetDefDispEnv(&db[1].disp,0,0,640,256);

db[0].draw.isbg = 1;										 
db[1].draw.isbg = 1;

setRGB0(&db[0].draw ,0,0,120);
setRGB0(&db[1].draw ,0,0,120);

FntLoad(704,0);
Prompts  = FntOpen(400,20,450,200,0,900);
				
Title			 = KanjiFntOpen(100,200, 500,100, 704,256, 704,470, 0,32);  
ScreenMessage	 = KanjiFntOpen(50 ,75 , 500,200, 704,256, 704,470, 0,200);  
OfficialWarnings = KanjiFntOpen(50 ,75 , 500,200, 704,256, 704,470, 0,200);  

cdb = &db[0];
SetDispMask(1);											    

stat = _mc_GetCardStatus(PORT_1, START_UP_READ);

if(stat == 0)		stat = 4;
							  
  while(1)   			 
  {								   
	button = ReadController();
   	if(FrameNo == MEM_CARD_REFRESH_RATE)    	stat = FrameCounterToReadCard();	// check the card status every 1/30 sec
   	
	if(stat == NO_CARD)		
		{
		present_card_removed(NumFilesOnCard,ret);
		No_Drawing = 1;
		CardStatusFlag=0;	// Sets a flag to display the Unformatted card message

		}
	
	
	if(stat == NEW_CARD_UNFORMATTED)		
		   CardStatusFlag=1;	// Sets a flag to display the Unformatted card message

	
	if(stat == NEW_CARD)   	
		{
			NumFilesOnCard = new_card_formatted(NumFilesOnCard,ret);
			NumBlocksOnCard = _mc_NumberOfUsedBlocksOnCard(&fileHeader[0]);	
                        width = ICON_WIDTH;             // reset the size of the icons
                        height = ICON_HEIGHT;                      
                }

	if(button != lastbutton)	  
		{
				
				if(button == BUTTON_SELECT)	
				{
					ViewMode++;
					if(ViewMode > 1)	ViewMode = 0;
				}
				
				if(ViewMode == 0)	
					{
						if(stat == OLD_CARD)	ScalingIconNum = CheckButtonsAndRespond(button,lastbutton, ScalingIconNum, NumFilesOnCard);
						DisplayMessage=1;
						FunctionNum=0;
					}
				
				if(ViewMode == 1)
					{
						
						if(BUTTON_RIGHT== button)	
							{
								FunctionNum++;
								DisplayMessage=0;
							}
						if(BUTTON_LEFT == button)	
							{
								FunctionNum--;
								DisplayMessage=0;
							}		    


						if(BUTTON_UP   == button)	DisplayMessage++;
						if(BUTTON_DOWN == button)	DisplayMessage--;
						CheckAndLimitMessageNumbers();
					}
		}

	
	if(stat == OLD_CARD)	
 		{
			if(EnterOnce==0)
				{			
					present_card_formatted(NumFilesOnCard);
		 			init_other_icons(&db[0], NumFilesOnCard);
		 			init_other_icons(&db[1], NumFilesOnCard);
					EnterOnce++;
					No_Drawing = 0;
				}
		}

if(ViewMode == 0)   
{
	manipulate_icons(cdb,width,height,ScalingIconNum);

	if(icon_expand == 0)	
		{
			width++;
			height++; 
		}
	else
		{	
			width--;
			height--;
		}

	// expand the icon
	if(icon_expand == 0) 
		if(DisplayIconNumber == 15)	  
		   icon_expand++;			  

	// contract the icon
	if(icon_expand == 1) 
		if(DisplayIconNumber == 30)
			icon_expand--;

	if(No_Drawing != 1)	 DisplayIconNumber = draw_icons(cdb, DisplayIconNumber);

	fileHeader[ViewingIconNum].title[64]=0;		// error trap incase the string is not 0 terminated
	KanjiFntPrint(Title," %s ",fileHeader[ViewingIconNum].title);		// print the newly 0 terminated string
}
    
if(ViewMode == 1)
{
DisplayWarnings(DisplayMessage,FunctionNum);
width = ICON_WIDTH;
height = ICON_HEIGHT;			   
icon_expand = 0;
}
	// Any on screen prints go here.
	if(ViewMode == 0) 
		{
			FntPrint(Prompts,"\n       %s = Forward\n%s = Back\n   Start = Save",FORWARD_BUTTON,RETURN_BUTTON);
		}

	if(ViewMode == 1)
		{
			FntPrint(Prompts,"\nMessage  Up = Forward\n		 Down = Back");
			FntPrint(Prompts,"\nFunction Up = Forward\n		 Down = Back");
		}
	// any standard incrimenting here
    lastbutton = button;  
	FrameNo++;						 
    DisplayIconNumber++;

    if(CardStatusFlag == 1)
    {

        do
        {
        _mc_BootUpMessage(3, OfficialWarnings);
             button = ReadController();
             if(button == BUTTON_X)
                {
                        _mc_FormatCard(PORT_1);
                        kill = 1;
                        stat = 4; // set the code to read the card.
                }

             if(button == BUTTON_TRIANGLE)      kill = 1;

             FntPrint(" Format Memory card? \n X = Yes, Triangle = No");
             DrawScreen();

        }while(kill == -1);
        CardStatusFlag =0;
    }
   DrawScreen();

  }

} //end of the main function..			    


//*****************************************************************************************
//*****************************************************************************************

void DrawScreen(void)
{

        VSync(0);
    cdb = (cdb==db) ? db + 1 : db;								   
 	PutDrawEnv(&cdb->draw);
	PutDispEnv(&cdb->disp);
	FntFlush(Prompts);

	if(ViewMode == 0)	KanjiFntFlush(Title);
	if(ViewMode == 1)	KanjiFntFlush(ScreenMessage);
	KanjiFntFlush(OfficialWarnings);
	
	DrawSync(0);
}

//*****************************************************************************************
//*****************************************************************************************

void CheckAndLimitMessageNumbers(void)
{

   	if(FunctionNum < 0)		FunctionNum++;
   	if(FunctionNum > 9)		FunctionNum--;

	if(DisplayMessage < 1)													DisplayMessage++;
	if(DisplayMessage > BOOT_UP && FunctionNum == 0)						DisplayMessage--;
	if(DisplayMessage > AUTO_BOOT_UP && FunctionNum == 1)					DisplayMessage--;
	if(DisplayMessage > SAVING_TO_CARD && FunctionNum == 2) 				DisplayMessage--;
	if(DisplayMessage > SAVING_TO_CARD_MULTI_SLOTS && FunctionNum == 3) 	DisplayMessage--;
	if(DisplayMessage > LOADING_FROM_CARD && FunctionNum == 4)				DisplayMessage--;
	if(DisplayMessage > LOADING_FROM_CARD_MULTI_SLOTS && FunctionNum == 5)  DisplayMessage--;
	if(DisplayMessage > CHECKING_DATA_ON_CARD && FunctionNum == 6)			DisplayMessage--;
	if(DisplayMessage > FORMATTING_CARD && FunctionNum == 7)				DisplayMessage--;
	if(DisplayMessage > DELETING_DATA && FunctionNum == 8)					DisplayMessage--;
	if(DisplayMessage > OVERWRITING_DATA && FunctionNum == 9)				DisplayMessage--;
	if(DisplayMessage > SAVING_TO_INTERNAL_MEMORY && FunctionNum == 10)		DisplayMessage--;
}

//*****************************************************************************************
//*****************************************************************************************

void DisplayWarnings(int Num, int Function)
{
if(Function == 0)	ScreenMessage = _mc_BootUpMessage(Num, ScreenMessage);
if(Function == 1)	ScreenMessage = _mc_BootUpAutoMessages(Num,14, ScreenMessage);
if(Function == 2)	ScreenMessage = _mc_SavingToTheMemoryCardMessages(Num,14, ScreenMessage);
if(Function == 3)	ScreenMessage = _mc_LoadingFromTheMemoryCardMessages(Num, ScreenMessage);
if(Function == 4)	ScreenMessage = _mc_SavingToMemoryCardMultipleSlotsAvMessages(Num,14, "1", ScreenMessage); // Slot is a char
if(Function == 5)	ScreenMessage = _mc_LoadingFromMemoryCardMultipleSlotsAvMessages(Num, "1", ScreenMessage);
if(Function == 6)	ScreenMessage = _mc_CheckingDataOnCardMessages(Num, "1", ScreenMessage);
if(Function == 7)	ScreenMessage = _mc_FormattingTheMemoryCardMessages(Num, "1", ScreenMessage);
if(Function == 8)	ScreenMessage = _mc_DeletingDataFromTheMemoryCardMessages(Num, ScreenMessage);
if(Function == 9)	ScreenMessage = _mc_OverWrittingDataFromOnMemoryCardMessages(Num, ScreenMessage);
if(Function == 10)	ScreenMessage = _mc_SavingToTheInternalMemoryMessages(Num, ScreenMessage);
}											    

//*****************************************************************************************
//*****************************************************************************************

int draw_icons(DB *b, int DisplayIconNumber)
{

int c;
int loop2=0;

for(loop=0; loop<15; loop++)
{
DrawPrim(&b->IconSet1[loop][0]);

if(fileHeader[loop2].blockEntry == 2)	
	{
		DrawPrim(&b->IconSet1[loop][1]);
		DrawPrim(&b->IconSet1[loop+1][1]);
		loop++;											   
	}
									  
if(fileHeader[loop2].blockEntry >= 3)	
	{
		
		for(c=0;c<fileHeader[loop2].blockEntry;c++)
			{		
				DrawPrim(&b->IconSet1[loop+c][2]);
			}
		loop+=c-1;
	}
loop2++;
}

if(DisplayIconNumber >(ICON_ANIM_SPEED*3-1))       DisplayIconNumber=0;
return DisplayIconNumber;
}


//*****************************************************************************************
//*****************************************************************************************


void manipulate_icons(DB *b,int width, int height, int ScalingIconNum)
{
int c;
for(loop=0; loop<15; loop++)		// sets all the non moving Icons to there correct size
  {
  	setXYWH(&b->IconSet1[loop][0],IconPosSCR[loop].X1,IconPosSCR[loop].Y1,ICON_WIDTH,ICON_HEIGHT);   
	setXYWH(&b->IconSet1[loop][1],IconPosSCR[loop].X1,IconPosSCR[loop].Y1,ICON_WIDTH,ICON_HEIGHT);   
	setXYWH(&b->IconSet1[loop][2],IconPosSCR[loop].X1,IconPosSCR[loop].Y1,ICON_WIDTH,ICON_HEIGHT);   
  
  }	

// Scale all of the anims not just the fisrt anim for the games with more Anims
setXYWH(&b->IconSet1[ScalingIconNum][0],IconPosSCR[ScalingIconNum].X1,IconPosSCR[ScalingIconNum].Y1,width,height);
setXYWH(&b->IconSet1[ScalingIconNum][1],IconPosSCR[ScalingIconNum].X1,IconPosSCR[ScalingIconNum].Y1,width,height); 
setXYWH(&b->IconSet1[ScalingIconNum][2],IconPosSCR[ScalingIconNum].X1,IconPosSCR[ScalingIconNum].Y1,width,height); 

if(fileHeader[ViewingIconNum].blockEntry == 2)
  {
	setXYWH(&b->IconSet1[ScalingIconNum+1][0],IconPosSCR[ScalingIconNum+1].X1,IconPosSCR[ScalingIconNum+1].Y1,width,height);
	setXYWH(&b->IconSet1[ScalingIconNum+1][1],IconPosSCR[ScalingIconNum+1].X1,IconPosSCR[ScalingIconNum+1].Y1,width,height); 
	setXYWH(&b->IconSet1[ScalingIconNum+1][2],IconPosSCR[ScalingIconNum+1].X1,IconPosSCR[ScalingIconNum+1].Y1,width,height);
  }

if(fileHeader[ViewingIconNum].blockEntry >= 3)
  {

   for(c=0;c<fileHeader[ViewingIconNum].blockEntry-1;c++)
	{
		setXYWH(&b->IconSet1[ScalingIconNum+c+1][0],IconPosSCR[ScalingIconNum+c+1].X1,IconPosSCR[ScalingIconNum+c+1].Y1,width,height);
		setXYWH(&b->IconSet1[ScalingIconNum+c+1][1],IconPosSCR[ScalingIconNum+c+1].X1,IconPosSCR[ScalingIconNum+c+1].Y1,width,height); 
		setXYWH(&b->IconSet1[ScalingIconNum+c+1][2],IconPosSCR[ScalingIconNum+c+1].X1,IconPosSCR[ScalingIconNum+c+1].Y1,width,height);
	}

  }							    
}


//*****************************************************************************************
//*****************************************************************************************

void present_card_formatted(int NumFilesOnCard)
{


			  for(loop=0; loop<NumFilesOnCard; loop++)
			  	{
                          _mc_LoadIconDataIntoVram(IconPosVR[loop].x,                      
                                                   IconPosVR[loop].y,                      
                                                   IconPosVR[loop].w,                      
                                                   IconPosVR[loop].h,              
                                                   IconPosVR[0].x,                         
                                                   IconPosVR[loop].y+(50+loop),  
                                                   (u_char *)fileHeader[loop].icons,       
                                                   (u_char *)fileHeader[loop].clut);
			   	}
}

//*****************************************************************************************
//*****************************************************************************************

present_card_removed()
{
_mc_BootUpMessage(4,OfficialWarnings);
}

//*****************************************************************************************
//*****************************************************************************************

int new_card_formatted(int NumFilesOnCard, int ret)
{
        	ClearFileHeader(&fileHeader[0]);
			NumFilesOnCard = _mc_ReadDirectoryInformation(PORT_1, DRIVE_1, &dirEntries[0]);
			
				for(loop=0; loop<NumFilesOnCard; loop++)
	          	{
					ret = _mc_LoadFromCard(PORT_1A, dirEntries[loop].name, &fileHeader[loop], FILE_HEADER_SIZE);	 
              	}
			
			stat = _mc_GetCardStatus(PORT_1,1);
 	 		EnterOnce=0;
			return NumFilesOnCard;
}

//*****************************************************************************************
//*****************************************************************************************

void init_other_icons(DB *b, int NumFilesOnCard)
{

int c=0;								 
int GL=0;
int multiblocks;
int ClutOffSet=0;
int TexOffSet=0;

for(c=0; GL<15; c++)
{
multiblocks=0;
	if(c < NumFilesOnCard)
	   {
  			for(loop=0; loop<fileHeader[c].blockEntry; loop++)
		    {
  			  	SetPolyFT4(&b->IconSet1[GL][0]);
  				SetShadeTex(&b->IconSet1[GL][0],1);
  				setXYWH(&b->IconSet1[GL][0],IconPosSCR[GL].X1,IconPosSCR[GL].Y1,ICON_WIDTH,ICON_HEIGHT);  // alter the position of the highlight 
  				setRGB0(&b->IconSet1[GL][0],255,0,0);
  				setUVWH(&b->IconSet1[GL][0],TexOffSet,0,15,16);
  				b->IconSet1[GL][0].tpage = getTPage(0,0,640,0);
  				b->IconSet1[GL][0].clut  = GetClut(640,50+c);
  				SetSemiTrans(&b->IconSet1[GL][0],0);

  				SetPolyFT4(&b->IconSet1[GL][1]);
  				SetShadeTex(&b->IconSet1[GL][1],1);
    			setXYWH(&b->IconSet1[GL][1],IconPosSCR[GL].X1,IconPosSCR[GL].Y1,ICON_WIDTH,ICON_HEIGHT);  // alter the position of the highlight 
  				setRGB0(&b->IconSet1[GL][1],255,0,0);				   
    			setUVWH(&b->IconSet1[GL][1],TexOffSet,16,15,16);
  				b->IconSet1[GL][1].tpage = getTPage(0,0,640,0);
  				b->IconSet1[GL][1].clut  = GetClut(640,50+c);
  				SetSemiTrans(&b->IconSet1[GL][1],0);
  																  
  				SetPolyFT4(&b->IconSet1[GL][2]);
  				SetShadeTex(&b->IconSet1[GL][2],1);
    			setXYWH(&b->IconSet1[GL][2],IconPosSCR[GL].X1,IconPosSCR[GL].Y1,ICON_WIDTH,ICON_HEIGHT);  // alter the position of the highlight 
  				setRGB0(&b->IconSet1[GL][2],255,0,0);
  				setUVWH(&b->IconSet1[GL][2],TexOffSet,32,15,16);
  				b->IconSet1[GL][2].tpage = getTPage(0,0,640,0);
  				b->IconSet1[GL][2].clut  = GetClut(640,50+c);	    		  
  				SetSemiTrans(&b->IconSet1[GL][2],0);
			 multiblocks++;
			 GL++;
			}
		}
		else
		{
		  	SetPolyFT4(&b->IconSet1[GL][0]);
  			setXYWH(&b->IconSet1[GL][0],IconPosSCR[GL].X1,IconPosSCR[GL].Y1,ICON_WIDTH,ICON_HEIGHT);  // alter the position of the highlight 
  			setRGB0(&b->IconSet1[GL][0],255,5,5);
  			setUVWH(&b->IconSet1[GL][0],0,0,0,0);
  			b->IconSet1[GL][0].tpage = getTPage(0,0,640,0);
  			b->IconSet1[GL][0].clut  = GetClut(640,50);
  			SetSemiTrans(&b->IconSet1[GL][0],0);

  			SetPolyFT4(&b->IconSet1[GL][1]);
    		setXYWH(&b->IconSet1[GL][1],IconPosSCR[GL].X1,IconPosSCR[GL].Y1,ICON_WIDTH,ICON_HEIGHT);  // alter the position of the highlight 
  			setRGB0(&b->IconSet1[GL][1],255,5,5);				   
    		setUVWH(&b->IconSet1[GL][1],0,0,0,0);
  			b->IconSet1[GL][1].tpage = getTPage(0,0,640,0);
  			b->IconSet1[GL][1].clut  = GetClut(640,50);
  			SetSemiTrans(&b->IconSet1[GL][1],0);
  																  
  			SetPolyFT4(&b->IconSet1[GL][2]);
    		setXYWH(&b->IconSet1[GL][2],IconPosSCR[GL].X1,IconPosSCR[GL].Y1,ICON_WIDTH,ICON_HEIGHT);  // alter the position of the highlight 
  			setRGB0(&b->IconSet1[GL][2],255,5,5);
  			setUVWH(&b->IconSet1[GL][2],0,0,0,0);
  			b->IconSet1[GL][2].tpage = getTPage(0,0,640,0);
  			b->IconSet1[GL][2].clut  = GetClut(640,50);	    		  
  			SetSemiTrans(&b->IconSet1[GL][2],0);
			GL++;
		}
  TexOffSet+=16;												   
  }	
}

//*****************************************************************************************
//*****************************************************************************************

int ReadController(void)
{

  	if(GoodData(&PadBuffer1))
     {
	     switch(GetType(&PadBuffer1))
		      {
			       case 4:    
						 // controller buttons
						 if(PadKeyIsPressed(&PadBuffer1,PAD_RD)!=0)	   		return BUTTON_X;   
						 if(PadKeyIsPressed(&PadBuffer1,PAD_RU)!=0)	   		return BUTTON_TRIANGLE;	   				
						 if(PadKeyIsPressed(&PadBuffer1,PAD_RL)!=0)	   		return BUTTON_SQUARE;	   				  
						 if(PadKeyIsPressed(&PadBuffer1,PAD_RR)!=0)	   		return BUTTON_CIRCLE;	   				  
						 
						 // Directional buttons
						 if(PadKeyIsPressed(&PadBuffer1,PAD_LU)!=0)	   		return BUTTON_UP;
						 if(PadKeyIsPressed(&PadBuffer1,PAD_LD)!=0)	   		return BUTTON_DOWN;
						 if(PadKeyIsPressed(&PadBuffer1,PAD_LL)!=0)	   		return BUTTON_LEFT;
						 if(PadKeyIsPressed(&PadBuffer1,PAD_LR)!=0)	   		return BUTTON_RIGHT;
						 
						 // Back Buttons
						 if(PadKeyIsPressed(&PadBuffer1,PAD_FRB)!=0)	 	return BUTTON_R2;
						 if(PadKeyIsPressed(&PadBuffer1,PAD_FLB)!=0)	 	return BUTTON_L2;
						 if(PadKeyIsPressed(&PadBuffer1,PAD_FRT)!=0)	 	return BUTTON_R1;
						 if(PadKeyIsPressed(&PadBuffer1,PAD_FLT)!=0)	 	return BUTTON_L1;

						 // Special Controll buttons
						 if(PadKeyIsPressed(&PadBuffer1,PAD_START)!=0)	  	return BUTTON_START;
						 if(PadKeyIsPressed(&PadBuffer1,PAD_SEL)!=0)	  	return BUTTON_SELECT;
					break;
			  }
	 }
return -1;
}

//*****************************************************************************************
//*****************************************************************************************
										   
int CheckButtonsAndRespond(int button, int lastbutton, int ScalingIconNum, int NumFilesOnCard)
{
   if(button != -1 && button != lastbutton)
   		{
		if(button == 1)		loop ++;
		if(button == 2)		loop --;
   		}
	
	// change the number of the scaling icons.

	if(button != lastbutton)	// debounce the buttons 
		{
			if(button == BUTTON_X)	 	   	    
				{
				 	if(fileHeader[ViewingIconNum].blockEntry > 1)	
				 		{
				  			ScalingIconNum += fileHeader[ViewingIconNum].blockEntry;
				  		}
				  	 else
				  		{
							ScalingIconNum++;
				  		}
				ViewingIconNum++;
				}

			if(button == BUTTON_TRIANGLE)		
				{
					if(fileHeader[ViewingIconNum].blockEntry > 1)	
						{
							ScalingIconNum--;
						  	// cheesy i know
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 2)	ScalingIconNum--;	// if the save has more than one icon then 
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 3)	ScalingIconNum-=2;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 4)	ScalingIconNum-=3;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 5)	ScalingIconNum-=4;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 6)	ScalingIconNum-=5;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 7)	ScalingIconNum-=6;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 8)	ScalingIconNum-=7;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 9)	ScalingIconNum-=8;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 10)	ScalingIconNum-=9;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 11)	ScalingIconNum-=10;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 12)	ScalingIconNum-=11;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 13)	ScalingIconNum-=12;	// move the scaller back
						  	if(fileHeader[ViewingIconNum-1].blockEntry == 14)	ScalingIconNum-=13;	// move the scaller back
						}
					 else
						{			
							ScalingIconNum--;
						}  

				ViewingIconNum--;											    
				}

			if(button == BUTTON_START)			
				{
                                        NumberFreeBlocks = _mc_NumberOfUsedBlocksOnCard(&fileHeader[0]); 
                                        if(ScalingIconNum+1 > NumberFreeBlocks)
					    {
                                                        SetUpAndSaveBlock(ViewingIconNum);
							stat = NEW_CARD;		// set it so the card is red.
							FrameNo	= MEM_CARD_REFRESH_RATE;	// make sure that the stat is not checked before reading
                                            }
			 	}
		}
	
	// restrict the number of the scaling Icons the number of files on the card	
		
	if(ScalingIconNum < 0 )						ScalingIconNum++;  // restrain the scling figure (more Icons than saves)
	if(ScalingIconNum >= 15)            		
	    {
		   ScalingIconNum=0;
		   ViewingIconNum=0;
		}

	if(ViewingIconNum < 0 )						ViewingIconNum++;  // restrain the viewing (for the title).

return ScalingIconNum;
}

//*****************************************************************************************
//*****************************************************************************************

int FrameCounterToReadCard(void)
{
int status=0;

	status = _mc_GetCardStatus(PORT_1,1);
	FrameNo = 0;
	return status;
}

//*****************************************************************************************
// Clearing the file header...... this is only a temprorary function... it will be replaced.
//*****************************************************************************************

void ClearFileHeader(FILE_HEADER *fh)
{
int local,main;

	for(main=0; main<15; main++)
	{
		fh[main].magic[1] = 0;			   
		fh[main].magic[2] = 0;
		fh[main].blockEntry = 0;

		for(local=0; local<64; local++)
		{
			fh[main].title[local] = 0;
		}	
	}
}

//*****************************************************************************************
//*****************************************************************************************

int SetUpAndSaveBlock(int ViewingIconNum)
{
TIM_IMAGE  tim;
char Name[32];

fileHeader[ViewingIconNum].magic[0] = 'S';
fileHeader[ViewingIconNum].magic[1] = 'C';

fileHeader[ViewingIconNum].type = 0x11;

fileHeader[ViewingIconNum].blockEntry = 1;  // the 1 will be replaced with selector when code is working
							  
_mc_AsciiStringToSJIS("CMAN SAVE(S)",fileHeader[ViewingIconNum].title);
fileHeader[ViewingIconNum].title[64]=0;

OpenTIM(IconData);
ReadTIM(&tim);

memcpy(fileHeader[ViewingIconNum].clut, tim.caddr,32);
memcpy(fileHeader[ViewingIconNum].icons, tim.paddr,384);

sprintf(Name,"bu00:TestSave%d",ViewingIconNum+1);
_mc_BlockWrite(1, (u_char *)&fileHeader[ViewingIconNum], Name);
}



										  

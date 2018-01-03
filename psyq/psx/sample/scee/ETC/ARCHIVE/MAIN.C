/******************************************************************
 *	   	Lib Rev 4.0											 	  *
 *																  *
 *		Filename:		main.c  								  *
 *																  *
 *		Author:		    Kevin Thompson						   	  *													
 *																  *
 *		History:												  *
 *			18-09-97	(LPGE)									  *
 *						Created									  *
 *																  * 
 *	    Copyright (c) 1997 Sony Computer Entertainment Europe     * 
 *		  All Rights Reserved									  *
 *																  *
 *		 Saves selected Memory card saves to a PC Hard drive	  *
 ******************************************************************/

#include <sys/types.h>
#include <sys/file.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <kernel.h>
#include <libsn.h>
#include "ctrller.h"
#include "memcard.h"

#define AS_NEW_CARD		0
#define AS_OLD_CARD		1

#define NEW_CARD		4
#define OLD_CARD		0	
#define NO_CARD			3

#define CARD_REFRESH	30


typedef struct {
	DRAWENV				draw;					  
	DISPENV				disp;
} DB;

ControllerPacket PadBuffer1,PadBuffer2;

struct DIRENTRY 	MCdirEntries[15];				// Saves From The Memory card
FILE_HEADER 	MCfileHeader[15];				// Saves From The Memory card

int ViewMode=0;
int ShowFileNumber;
int NumFilesOnThePC;				 
char FileListNames[10000][21];
char FileListNumbers[10000][4];
char FileListBlocks[10000][4];
char DeletedFiles[10000][4];

int NumFilesOnCard;
int TimeToCheckTheMemoryCard= CARD_REFRESH-3;
int Status;
int select;
int Std;
int Prompts;
int Warnings;
int LoadedTheFileList;
int SaveDone;

// the buffers have to be this size incase some one want's to move a full 15 block save.
// of course not all will be read/writen.
char PCtoMCBuffer[122880];	
char MCtoPCBuffer[122880];			   


// ProtoTypes

int ReadController(void);
int CheckButtonAndRespond(int button, int select, int lastbutton);
void PrintTheContentsWithHighlighter(int select, int NumFilesOnCard);
int ReadTheCardHeaderInfo(int NumFilesOnCard);
int SaveTheFileToThePC(int select);
void DrawScreen(void);
int LoadTheFileListFromThePC(int select);
int ReadTheFilesFromThePC(char *drive,struct DIRENTRY *d);

DB		db[2];
DB		*cdb;

main()
{
int time;
int loop;		   
int NumFilesOnCard=0;
int loop2=0;
short button;
short LastButton;
int ret;

ResetCallback();

_mc_InitializeCardAndEventsStandard(NOT_SHARED);

InitPAD((u_char *)&PadBuffer1, 8, (u_char *)&PadBuffer2, MAX_CONTROLLER_BYTES);
StartPAD();

ResetGraph(0);
SetGraphDebug(1);									    
					 
SetDefDrawEnv(&db[0].draw,0,0,640,256);
SetDefDrawEnv(&db[1].draw,0,256,640,256);
SetDefDispEnv(&db[0].disp,0,256,640,256);
SetDefDispEnv(&db[1].disp,0,0,640,256);

db[0].draw.isbg = 1;										 
db[1].draw.isbg = 1;

setRGB0(&db[0].draw ,0,0,20);
setRGB0(&db[1].draw ,0,0,20);

FntLoad(704,0);
Std = FntOpen(50,20,450,200,0,900);
Prompts = FntOpen(300,20,450,200,0,900);
Warnings = FntOpen(300,200,450,200,0,900);

cdb = &db[0];
SetDispMask(1);											    

  while(1)   		
  {

// a waste of code I know
if(loop2 == 3)		 Status = _mc_GetCardStatus(PORT_1, AS_NEW_CARD);	


	button = ReadController();
	select = CheckButtonAndRespond(button, select, LastButton);

	if(select < 0)   select = NumFilesOnCard-1;		  
	if(select > NumFilesOnCard-1)   select = 0;

	if(TimeToCheckTheMemoryCard == 60)		
		{
		 	Status = _mc_GetCardStatus(PORT_1, AS_OLD_CARD);
			TimeToCheckTheMemoryCard=0;
		}

   	
   	if(loop2 < 3)		FntPrint(Std," Loading Files");
   	
	if(Status == NEW_CARD)
		{
			NumFilesOnCard = ReadTheCardHeaderInfo(NumFilesOnCard);
			Status = _mc_GetCardStatus(PORT_1, AS_OLD_CARD);
		}

	if(Status == OLD_CARD)
		{
			if(ViewMode == 0)	if(loop2 > 3)		PrintTheContentsWithHighlighter(select, NumFilesOnCard);
			if(ViewMode == 1)	FntPrint(Std,"\n Loading Files From the PC \n");
			if(ViewMode == 1)	FntPrint(Std,"\n %d %s %s %s ",ShowFileNumber+1,FileListNumbers[ShowFileNumber],FileListBlocks[ShowFileNumber],FileListNames[ShowFileNumber]);
			if(ViewMode == 1)	FntPrint(Prompts,"\n Use Directional buttons to move\n X = Save To Memory card\n Start = Format Memory card\n L2 = Delete Save From PC");
	   	}

	if(Status == NO_CARD && loop2 > 3)
		{
			FntPrint(Std," Please Enter A Memory card");
		}



	DrawScreen();

    // general looper variable.  is this == 3 then read in the card data. this is to prevent
	// the ugly colour bars being on screen for to long.

    loop2++;
	TimeToCheckTheMemoryCard++;
	LastButton = button;	    
  }								    	    
}


//*************************************************************************************
//*************************************************************************************

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


//*************************************************************************************
//*************************************************************************************


int CheckButtonAndRespond(int button, int select, int LastButton)
{	
	if(button != LastButton)
	{
		if(button == BUTTON_UP)		select--;
		if(button == BUTTON_DOWN)	select++;
		if(ViewMode == 0)		if(button == BUTTON_X)		SaveTheFileToThePC(select);
		
		if(button == BUTTON_SELECT)	ViewMode++;
		

		if(ViewMode > 1)		
			{
				ViewMode = 0;
				Status = NEW_CARD;
				TimeToCheckTheMemoryCard=0;
				LoadedTheFileList=0;
			}
		
		if(ViewMode == 1)
		   {		
				if(LoadedTheFileList == 0)	
					{
						NumFilesOnThePC = LoadTheFileListFromThePC(select);
						LoadedTheFileList = 1;
					}

				if(button == BUTTON_X)			SaveDone = SaveFileToTheMC();
 				if(button == BUTTON_UP)			ShowFileNumber++;
				if(button == BUTTON_DOWN)		ShowFileNumber--;
		   		if(ShowFileNumber < 0)					ShowFileNumber++;
				if(ShowFileNumber > NumFilesOnThePC-1) 	ShowFileNumber--;
		   		
		   		if(button == BUTTON_L2)			
		   				{
		   					   do{
								button = ReadController();
								FntPrint(Std," Are You Sure \n\n Triangle = No \n   Square = Yes");
								
								if(button == BUTTON_SQUARE)
									{							  
		   								FntPrint(Std,"\nDeleting The Save");
										DrawScreen();
										DrawScreen();
										DeleteSaveFromThePC();
										VSync(0);
										DrawSync(0);
										return; 
									}
								if(button == BUTTON_TRIANGLE)	return; 
								DrawScreen();
								}while(1);
						}

				if(button == BUTTON_START)		
					{
						// add an are you sure to the loop for the formating of cards
						do{
						button = ReadController();
						FntPrint(Std," Are You Sure \n\n Triangle = No \n        X = Yes");
						
						if(button == BUTTON_X)   
							{
								FntPrint(Std,"\nFormating The Card");
								DrawScreen();
								DrawScreen();
								format(DRIVE_1);
								return;
							}

						if(button == BUTTON_TRIANGLE)	return;
						DrawScreen();
						}while(1);
					}
						
		   }
	}							 
return select;
}

//*************************************************************************************
//*************************************************************************************

DeleteSaveFromThePC()
{
char path[12];
int fp;
int loop;
int err;
		   
fp = PCopen("DelFile.bat", 2, 0);

	if(fp == -1)
		{
			fp = PCcreat("DelFile.bat", 0);
			if(fp == -1)		
				{
				FntPrint(Warnings," \n Failed to Create FileList......");
				}
		}

// just testing again
sprintf(path,"\ndel %s",FileListNumbers[ShowFileNumber]);

PClseek(fp, 0, 2);

PCwrite(fp,(u_char *)&path[0], 8);	   
PCclose(fp);

// remove the file from the FileList.lst								   													    
fp = PCcreat("FileList.lst", 0);

	if(fp == -1)
		{
			FntPrint(Warnings," \n Unable to open FileList.lst");
			PCclose(fp);
			return -1;
		}									   

for(loop=0; loop<NumFilesOnThePC; loop++)
	{
		if(loop != ShowFileNumber)
		{
			err = PCwrite(fp,(u_char *)FileListNumbers[loop], 4);
			if(err != 4)	
				{
					FntPrint(Warnings,"\n Write Failed in Byte %d",err);
 					return -1;
 				}

			err = PCwrite(fp,(u_char *)FileListBlocks[loop], 4);
			if(err != 4)	
				{
					FntPrint(Warnings,"\n Write Failed in Byte %d ",err);
					return -1;
				}

			err =PCwrite(fp,(u_char *)FileListNames[loop], 20);
			if(err != 20)	
				{
					FntPrint(Warnings,"\n Write Failed in Byte %d ",err);
					return -1;
				}

		}
	}
NumFilesOnThePC = LoadTheFileListFromThePC(select);
ShowFileNumber=0;
PCclose(fp);
				 
}											  

//*************************************************************************************
//*************************************************************************************

SaveFileToTheMC()
{
int loop;										   
char Name[32];
int fp;
char FilePointer[4];
int err;
int ret;

// Loading from the PC
sprintf(Name,"%s%s",DRIVE_1,FileListNames[ShowFileNumber]);

NumFilesOnCard = _mc_NumberOfUsedBlocksOnCard(&MCfileHeader[0]);

if(NumFilesOnCard >= 15)	
	{
		printf("\n Card is Full");
		return -3;
	}

ret = NumFilesOnCard+atoi(FileListBlocks[ShowFileNumber]);

if(ret >15)
	{
		printf("\n Not Enough Space On Card.");
		return -4;
	}

//Load The Save Here

sprintf(FilePointer,"%d",ShowFileNumber+1);

fp = PCopen(&FilePointer[0], 2, 0);
		if(fp == -1)		
		{
				printf("\n failed to PC open file");
				return -1;
		}

err = PCread(fp, (u_char *)&PCtoMCBuffer, (COMPLETE_SAVE * atoi(FileListBlocks[ShowFileNumber])));
if(err != (COMPLETE_SAVE * atoi(FileListBlocks[ShowFileNumber])))	FntPrint(Warnings,"\n error while loading int byte %d ",err);
												 

//Saving to the Memory card.

sprintf(Name,"%s%s",DRIVE_1,FileListNames[ShowFileNumber]);
										  
ret = _mc_BlockWriteTEST(atoi(FileListBlocks[ShowFileNumber]), (u_char *)&PCtoMCBuffer, Name);


// warnings
if(ret == 0)	printf("\nSave Failed - File Already Exists ");
if(ret == 2)	printf("\nSave Failed - Unable To Open File ");
if(ret == 3)	printf("\nSave Failed During Writing");

PCclose(fp);
}

//*************************************************************************************
//*************************************************************************************

int _mc_BlockWriteTEST(int blocks, char *buffer, char *SaveName)
{
int fd=0;
int i=0;  				  

	i=_mc_FileExistsOnCard(SaveName);
	  
	if(i==1)	return 0;		// file already exists

    fd=open(SaveName,O_CREAT|(blocks<<16));
    
    if(fd == -1)	return 2;  // unable to open save

	i = write(fd,buffer,(blocks*8192));
	if(i!=(blocks*8192)) 
		{
		close(fd);
		return 3;			 // error failed while writting to the card
		}
 	close(fd);
	return 1;		// write successful
}



//*************************************************************************************
//*************************************************************************************

int ReadTheCardHeaderInfo(int NumFilesOnCard)
{												    
int loop;
NumFilesOnCard = _mc_ReadDirectoryInformation(PORT_1, DRIVE_1, &MCdirEntries[0]);
    	for(loop=0; loop<NumFilesOnCard; loop++)
 		{
 		_mc_LoadFromCard(PORT_1A, MCdirEntries[loop].name, &MCfileHeader[loop], 150);	 
 		}

return NumFilesOnCard;
}							  


//*************************************************************************************
//*************************************************************************************

void PrintTheContentsWithHighlighter(int select, int NumFilesOnCard)
{
int String[40];
int loop;											 

	FntPrint(Std,"\n Saves On inserted Memory card\n\n");
	 for(loop=0; loop<NumFilesOnCard; loop++)
		{
			if(loop != select)		sprintf(String," %s ",MCdirEntries[loop].name);
			else
			sprintf(String,"*%s*",MCdirEntries[loop].name);
			
			FntPrint(Std,"\n%s",String);
		}			
		FntPrint(Prompts,"\n Use Directional Buttons To Move\n X = DownLoad\n Select = Upload Menu");				    
}

//*************************************************************************************
//*************************************************************************************

int SaveTheFileToThePC(int select)
{
int fp=0;
int err=0;
int counter=1;
char path[4];
int ret;

// read entire MC Save Buffer
ret = _mc_LoadFromCardTEST(PORT_1A, MCdirEntries[select].name, &MCtoPCBuffer, 8192);

// Some Simple Error checking
		if(ret == 0)
			{
				FntPrint(Warnings," \n Unable to open the file for Down Load From MC "); 
			}
		if(ret == 2)
			{
				FntPrint(Warnings," \n An Error Occured while reading from the MC");
			}

// set up the path for the name of the file to open on the PC
sprintf(path,"%d",counter);		

// check if save already exists. by opening the file.  if it opens it exists.
fp = PCopen(&path[0], 2, 0);
if(fp == -1)	FntPrint(Warnings," \n Unable to open %s ",path);
	
// if save exists change name and try again.	    
	if(fp != -1)								 			  
		{
				// close the file
				PCclose(fp);	
				
				// loop, while changing the name until a name works.
				do
				{
				    counter++;
				    sprintf(path,"%d",counter);		
			      	fp = PCopen(&path[0], 2, 0);
					PCclose(fp);	// close the file.
				}while(fp != -1);
				
									  
		}

// if the files not there open it.
fp = PCcreat(&path[0], 0);

// Save the MCbuffer to the PC.
err = PCwrite(fp,(u_char *)&MCtoPCBuffer[0], (COMPLETE_SAVE * MCfileHeader[select].blockEntry));

		if(err != (COMPLETE_SAVE * MCfileHeader[select].blockEntry));
		{
			FntPrint(Warnings,"\n Saving main buffer failed in byte %d ",err);	
		}

// close the file.
PCclose(fp);

// Open/Create the file list.
fp = PCopen("FileList.lst", 2, 0);

// if the file list does not exist... create it.
	if(fp == -1)		
		{
			fp = PCcreat("FileList.lst", 0);
			if(fp == -1)		FntPrint(Warnings," \n Failed to Create FileList......");
	 	}

PClseek (fp, 0, 2);
err = PCwrite(fp, (u_char *)&path[0], 4);		
if(err != 4)		FntPrint(Warnings,"\n error while writing save num... byte num %d ",err);

sprintf(path,"%d",MCfileHeader[select].blockEntry);

err = PCwrite(fp, (u_char *)&path[0], 4);		
if(err != 4)		FntPrint(Warnings,"\n error while writing save num... byte num %d ",err);
// then the name of the save to the filelist.lst 
                                                                  
PClseek (fp, 0, 2);
err = PCwrite(fp, (u_char *)MCdirEntries[select].name , 20);

// Error check the write
if(err != 20)		FntPrint(Warnings,"\n error while writing save name... byte num %d ",err);

// close the file.
PCclose(fp);
} 

//*************************************************************************************
//*************************************************************************************

int LoadTheFileListFromThePC(int select)
{
int fp;
int err;
int loop;

fp = PCopen("FileList.lst", 2, 0);		// open the filelist file. for read/write.
	 
	if(fp == -1)					// error trap the files opening.
		{
		   	FntPrint(Warnings," \n Failed To Open FileList.lst");
			return -1;
	 	}						    

for(loop=0; loop<10000; loop++)			  
{
err = PCread(fp,(u_char *)FileListNumbers[loop], 4);
err = PCread(fp,(u_char *)FileListBlocks[loop], 4);
err = PCread(fp,(u_char *)FileListNames[loop], 20);

if(err != 20)		// end of file
	{
		PCclose(fp);
		return loop;
	}
}

PCclose(fp);					// closes the PC file.
}

//*************************************************************************************
//*************************************************************************************

void DrawScreen(void)
{
cdb = (cdb==db) ? db + 1 : db;
PutDrawEnv(&cdb->draw);
PutDispEnv(&cdb->disp);
FntFlush(Std);
FntFlush(Prompts);
FntFlush(Warnings);
DrawSync(0);
}


//*************************************************************************************
//*************************************************************************************


int _mc_LoadFromCardTEST(long channel, char *fileName, char *fileHeader, int DownLoadSize)
{
int file;
char path[64];			 
int count;

  sprintf(path, "bu%.2x:%s", channel, fileName);
  file=open(path, O_RDONLY);

  if(file <0)			return 0;	 // file could not be opened

  count = read(file, fileHeader, DownLoadSize);
							    
  if(count != DownLoadSize)   
  {																	   
       close(file);
       return 2;  // error during read
  }

  close(file);
  return 1;
}


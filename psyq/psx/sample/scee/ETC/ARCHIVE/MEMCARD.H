//**********************************************************************************
//		
//		DESCRIPTION:	Header file for the Memory card Library functions
//
//		AUTHOR:			Kevin Thompson..
//
//		HISTORY:		created on 21 July 1997..
//
//**********************************************************************************

#include <sys/types.h>
#include <sys/file.h>
#include <kernel.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>

#define PRESENT_CARD_FORMATTED   		0
#define PRESENT_CARD_UNFORMATTED 		1
#define PRESENT_CARD_BAD                2
#define PRESENT_CARD_NONE               3
#define NEW_CARD_FORMATTED   	    	4
#define NEW_CARD_UNFORMATTED 	    	5
#define NEW_CARD_BAD                    6
#define NEW_CARD_NONE                   7

#define EVENT_IOE			0  
#define EVENT_ERROR			1
#define EVENT_TIMEOUT		2
#define EVENT_NEWCARD		3

#define SHARED				1
#define NOT_SHARED			0

#define START_UP_READ		0
#define IN_GAME_READ		1

#define FILE_HEADER_SIZE	512
#define COMPLETE_SAVE		8192

// define the ports
#define PORT_1				0x00
#define PORT_2				0x10
#define PORT_1A				0x00
#define PORT_1B				0x01
#define PORT_1C				0x02
#define PORT_1D				0x03
#define PORT_2A				0x10
#define PORT_2B				0x11
#define PORT_2C				0x12
#define PORT_2D				0x13

// define the ports
#define DRIVE_1				"bu00:"
#define DRIVE_2				"bu10:"
#define DRIVE_1A			"bu00:"
#define DRIVE_1B			"bu01:"
#define DRIVE_1C			"bu02:"
#define DRIVE_1D			"bu03:"
#define DRIVE_2A			"bu10:"
#define DRIVE_2B			"bu11:"
#define DRIVE_2C			"bu12:"
#define DRIVE_2D			"bu13:"

// number of messages in each of the message files.

#define BOOT_UP							4
#define AUTO_BOOT_UP  					4
#define	SAVING_TO_CARD					8
#define SAVING_TO_CARD_MULTI_SLOTS		16
#define LOADING_FROM_CARD				8
#define LOADING_FROM_CARD_MULTI_SLOTS	8
#define CHECKING_DATA_ON_CARD			6
#define FORMATTING_CARD					7
#define DELETING_DATA					5
#define OVERWRITING_DATA				6
#define SAVING_TO_INTERNAL_MEMORY		1

// these are changeable.
#define AUTH_NAMES1 				"Memory card"
#define AUTH_NAMES2 				"Memory card slot"

// this sould be altered by the programmer to suit there game

#define	GAME_TITLE					"TestTitle"		// place your title here
#define NUMBER_OF_BLOCKS_USED 		1					// number of blocks your game uses
#define FORWARD_BUTTON				"X"					// button to go forward in the game
#define RETURN_BUTTON				"Triangle"			// button to go backwards in the game

typedef struct 
{
  char		magic[2];
  char		type;
  char		blockEntry;								  
  char 		title[64];
  char 		reserved[28];
  char		clut[32];
  char		icons[3][128];
}FILE_HEADER;

extern struct DIRENTRY 	dirEntries[15] ;
extern FILE_HEADER		fileHeader[15] ;

// needed variables

extern long Ev0,Ev1,Ev2,Ev3,Ev10,Ev11,Ev12,Ev13;
extern long fev0,fev1,fev2,fev3,fev10,fev11,fev12,fev13;
 

// PROTOTYPES
											    
int _mc_InitaliseCardsAndEventsAsy(int shared);  // don't work just yet (function inoperable with it)
int _mc_DeInitaliseCardAndEventsAsy(void);
int _mc_DisableCardEvents(void);
int _mc_EnableCardEvents(void);
int _mc_InitializeCardAndEventsStandard(int shared);

int _mc_FileExistsOnCard(char *fileName);

int _mc_GetCardEventSw(void);
int _mc_GetCardEventHw(void);
int _mc_FormatCard(long channel);
int _mc_UnFormatCard(long channel);
int _mc_QuickUnFormatCard(long channel);						    
int	_mc_DeleteFileFromCard(char *drive, char *name);
int _mc_LoadFromCard(long channel, char *fileName, FILE_HEADER *fileHeader, int DownLoadSize);
int _mc_BlockWrite(int blocks, char *buffer, char *SaveName);
int _mc_GetCardStatus(long channel, int StartUp);
int _mc_IsCardStillPresent(long channel);
int _mc_NumberOfFreeBlocksOnCard(FILE_HEADER *fileheader);
int _mc_NumberOfUsedBlocksOnCard(FILE_HEADER *fileheader);
void _mc_AsciiStringToSJIS(unsigned char *string, unsigned char *dest); 
int _mc_BootUpMessage(int message, int mess);
int _mc_BootUpAutoMessages(int message, int FreeBlocks, int mess);
int _mc_SavingToTheMemoryCardMessages(int message, int blocksfree, int mess);
int _mc_LoadingFromTheMemoryCardMessages(int message, int mess);
int _mc_SavingToMemoryCardMultipleSlotsAvMessages(int message,int blocksfree, char *slot, int mess);
int _mc_LoadingFromMemoryCardMultipleSlotsAvMessages(int message, char *slot, int mess);
int _mc_CheckingDataOnCardMessages(int message, char *slot, int mess);
int _mc_FormattingTheMemoryCardMessages(int message, char *slot, int mess);
int _mc_DeletingDataFromTheMemoryCardMessages(int message, int mess);
int _mc_OverWrittingDataFromOnMemoryCardMessages(int message, int mess);
int _mc_SavingToTheInternalMemoryMessages(int message,int mess);
int _mc_LoadIconDataIntoVram(int u, int v, int w, int h, int cu, int cv, char *IconData, char *ClutData);
void _mc_ClearEventSw(void);
void _mc_ClearEventHw(void);
int _mc_ReadDirectoryInformation(long port, char *drive, struct DIRENTRY *d);	 
				 
	  		   
				   					   

	   

					 

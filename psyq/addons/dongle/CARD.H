/*******************************************************************
 *
 *    DESCRIPTION:		Card Handling Routines Header
 *
 *    AUTHOR:			Malachy Duffin
 *
 *    HISTORY:    		Created August 96
 *
 *******************************************************************/



#define EVENT_IOE			0
#define EVENT_ERROR			1
#define EVENT_TIMEOUT		2
#define EVENT_NEWCARD		3

#define CARD_FORMATTED   	0
#define CARD_UNFORMATTED 	1
#define CARD_BAD			2
#define CARD_NONE			3

#define BLOCK_WRITE_OK		0
#define BLOCK_WRITE_ERROR	1

#define BLOCK_READ_OK		0
#define BLOCK_READ_ERROR	1

#define FALSE				0
#define TRUE				1

typedef struct 
{
  char			magic[2] ;
  char			type ;
  char			blockEntry ;
  char 			title[64] ;
  char 			reserved[28] ;
  char			clut[32] ;
  char			icons[3][128] ;
} FILE_HEADER ;

/*typedef struct
{
  int				fileCount ;
  FILE_HEADER		fileHeaders[15] ;
  struct DIRENTRY	dirEntries[15] ;
} CARD_DETAILS ;*/

static char *cardMsg[] = 
{
  "CARD FORMATTED",
  "CARD UNFORMATTED",
  "BAD CARD",
  "NO CARD FOUND"
} ;

static unsigned long 	ev0, ev1, ev2, ev3 ;
static unsigned long 	ev10, ev11, ev12, ev13 ;


void InitializeCardAndEvents () ;
void DeInitializeCardAndEvents () ;

int GetCardStatus (long channel) ;
int GetCardEventSw (void) ;
int ClearCardEventsSw () ;
int GetCardEventHw () ;
int ClearCardEventsHw () ;
int CheckIfCardFormatted (long channel) ;
int FormatCard (long channel) ;
int UnFormatCard(long channel) ;
int QuickUnFormatCard (long channel) ;
int UnFormatCard(long channel) ;
int GetFileDirectoryFromCard (long channel, struct DIRENTRY *d) ;
void GetCardData (long channel, char *buffer) ;
void DisplayFileDirectory (struct DIRENTRY *d, int fileCount) ;
void DisplayFileHeader (FILE_HEADER *fileHeader) ;
int FileExistsOnCard (long channel, char *fileName) ;
int DeleteFileFromCard (long channel, char *fileName) ;
int WriteFileBlocking (long channel, char *fileName, int blocks, char *buffer) ;
int ReadFileBlocking (long channel, char *fileName, int blocks, char *buffer) ;
int GetFileHeaderFromCard (long channel, char *fileName, FILE_HEADER *header) ;
int SetFileHeaderInCard (long channel, char *fileName, FILE_HEADER *header) ;
int CheckBlockSpacesAvailable (long channel) ;
int ReadBlockFromCard (long channel, int blockNo, char *buffer) ;
int WriteBlockToCard (long channel, int blockNo, char *buffer) ;

extern struct DIRENTRY 	dirEntries[15] ;
extern int				fileCount ;
extern FILE_HEADER		fileHeader[15] ;
extern char				fileBuffer[8192] ;
extern int				cardDataLoaded ;

/* Test Windows Program */

/* Includes */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include <kernel.h>

#include "card.h"			
#include "dongle.h"
#include "window.h"
#include "dongread.h"

/* Defines */
#define	TIM1				(u_long *)0x80100000+5

#define NUM_OPTIONS			22

#define DONGLE_INPUT_X		320
#define DONGLE_INPUT_Y		85
#define NUM_DONGLE_OPTIONS  5

#define ROW_SIZE			13
#define COL_SIZE			16

const   DONGLE_WIDTH[5] = {20, 20, 20, 8, 1} ;

/* Type Declarations */
enum OPTIONS {logoOpt, iconCrossOpt, iconTriangleOpt, iconSquareOpt, iconCircleOpt, iconOKOpt, iconYesOpt, iconNoOpt,
					   insertCardOpt, errorInCardOpt, createDongleOpt, checkDongleOpt, memoryOptionsOpt, 
					   enterTitleNameOpt, enterCompanyNameOpt, enterPersonNameOpt, enterDateOpt, enterKeyOpt, dongleHelpOpt, 
					   deleteFileOpt, viewFileContentsOpt, dongleNotFoundOpt} ;

enum STATUS {insertCardWin, errorInCardWin, mainMenuWin, memoryCardOptionsWin, viewFileContentsWin, createDongleWin, checkDongleWin} ;

static RECT 		otXY [NUM_OPTIONS] =
{
	{190, 21, 255, 28},				/* Logo */
	{0, 0, 32, 32}, 				/*Icon dependant on window position*/
	{0, 0, 32, 32}, 				/*Icon dependant on window position*/
	{0, 0, 32, 32}, 				/*Icon dependant on window position*/
	{0, 0, 32, 32}, 				/*Icon dependant on window position*/
	{0, 0, 50, 20},					/* OK */
	{0, 0, 50, 20},					/* Yes */
	{0, 0, 50, 20},					/* No */
	{190, 135, 255, 15},			/* Insert Card */
	{190, 135, 213, 15},			/* Error In Card */
	{230, 100, 195, 14},			/* Create Dongle */
	{215, 130, 230, 14},			/* Check Dongle */
	{205, 160, 251, 14},			/* Memory Options */
	{170, 82, 110, 14},				/* Title Name */
	{170, 102, 116, 14},			/* Company Name */
	{170, 122, 114, 14},			/* Persons Name */
	{170, 142, 36, 14},				/* Date */
	{170, 162, 26, 14},				/* Key */
	{220, 190, 200, 32},			/* Help */
	{60, 205, 108, 16},				/* Delete File */
	{190, 205, 174, 16},			/* View File Contents */
	{230, 130, 180, 14}				/* Dongle Not Found */
} ;

static RECT 		otUV [NUM_OPTIONS] =
{
	{0, 0, 255, 28},				/* Logo */
	{1, 238, 17, 17},				/* Icon */
	{16, 238, 17, 17},				
	{32, 238, 17, 17},
	{48, 238, 17, 17},
	{0, 236, 60, 19},				/* OK */
	{58, 236, 70, 19},				/* Yes */
	{124, 236, 64, 19},				/* No */
	{0, 27, 255, 15},				/* Insert Card */
	{0, 41, 213, 15},				/* Error In Card */
	{0, 56, 195, 14},				/* Create Dongle */
	{0, 70, 230, 14},				/* Check Dongle */
	{0, 84, 251, 14},				/* Memory Options */
	{119, 116, 110, 14},			/* Title Name */
	{1, 101, 116, 14},				/* Company Name */
	{1, 116, 114, 14},				/* Persons Name */
	{127, 101, 36, 14},				/* Date */
	{182, 101, 26, 14},				/* Key */
	{0, 152, 200, 32},				/* Help */
	{0, 221, 108, 16},				/* Delete File */
	{0, 204, 174, 16},				/* View File Contents */
	{0, 132, 180, 14}				/* Dongle not found */
} ;


/* Function Declarations */
void FillOrderTable (void) ;
void DrawMainWindow (void) ;
void DrawInsertCardWindow (void) ;
void DrawMainMenuWindow (void) ;
void DrawCreateDongleWindow (void) ;
void DrawCheckDongleWindow (void) ;
void DrawMemoryCardOptionsWindow (void) ;
void DrawFileContentsWindow (void) ;
void DisplayFileContents (int offset) ;
void DisplayFileList (void) ;
void DrawFileSelectionBar () ;
void UpdateFileInfo (void) ;
void DisplayCreateDongleList (void) ;
void DisplayCheckDongleList (void) ;
void DrawCharSelectionBar (void) ;
void InitializeOptionsGraphics (void) ;
void InitializeWindows (void) ;
void SetShadedOption (POLY_FT4 *poly) ;
void ResetShadedOption (POLY_FT4 *poly) ;
int GetInput (void) ;
int ProcessInput (int input) ;
void ProcessErrorInCardInput (int input) ;
void ProcessMainMenuInput (int input) ;
void ProcessCreateDongleInput (int input) ;
void ProcessCheckDongleInput (int input) ;
void ProcessMemoryCardOptionsInput (int input) ;
int ProcessViewFileContentsInput (int input) ;
int DecreaseCharacter (char *c) ;
int IncreaseCharacter (char *c) ;
void DecreaseNumber (char *c) ;
void IncreaseNumber (char *c) ;
                           
extern u_long       dongleTim[] ;

/* Constant Delcarations */
static POLY_FT4 	optionsPoly[NUM_OPTIONS] ;

static POLY_F4		fileSelectionBar,
					dongleCharSelectionBar ;

static FRAMEWINDOW 	mainWindow[2],
					insertCardWindow,
					mainMenuWindow ,
					fileListWindow,
					fileInfoWindow,
					fileKanjiWindow ;

static STDWINDOW	createDongleWindow,
					checkDongleWindow,
					memoryCardOptionsWindow,
					fileContentsWindow ;

static OKWINDOW		okWindow ;


static int	 		options = logoOpt ;
static int	 		status = insertCardWin ;

static int	 		shade = 0 ;
static int	 		shadeV = 2 ;

static int	 		choice = 0 ;
static int	 		fileChoice = 0 ;

DONGLE_INFO			dongleInfo = {"TITLE NAME          ",
						  		  "COMPANY NAME        ", 
						  		  "PERSONS NAME        ", 
						  		  "10/10/96"} ; 
	
static int	 		dongleInputChoiceLine = 0 ;
static int	 		dongleInputChoiceChar = 0 ;
static int	 		dongleFound = 0 ;

static long			fileList ;
static long			fileInfo ;
static long			fileKanji ;
static long			fileContents ;

long				fileOffset = 0 ;
int					overRide = 0 ;


char				keyStr[4] = "1" ;
int					key = 0 ;
int 				currentDongleInfo = 0 ;
int					dongleFileInfoCount = 0 ;

char				cardBuffer[128*64*16] ;

main ()
{
  RECT			rect, rect2 ;
  u_int			count = 0 ;
  int			ret ;
  int 			newCard = 1 ;
  int			loop ;

  ResetCallback () ;
  PadInit (0) ;
  InitializeCardAndEvents () ;
  InitializeGraphics () ;
  InitializeOptionsGraphics () ;

  InitializeWindows () ;

  while (1)
  {
	DrawSync (0) ;
    ret = GetCardStatus(0x00) ; 
	switch (ret)
	{ 
	  case CARD_NONE:
	  {
	    status = insertCardWin ;
		newCard = 1 ;
		overRide = 0 ;
	    break ;
	  }
	  case CARD_BAD:
	  case CARD_UNFORMATTED:
	  {
	    if (overRide == 0)
		{
	      status = errorInCardWin ;
		  break ;
		}
	  }
	  case CARD_FORMATTED:
	  { 
		if (newCard == 1)
		{
	      status = mainMenuWin ;
		  dongleFound = 0 ;
		  cardDataLoaded = 0 ;
          UpdateFileInfo () ;
		  if (ReadDongleInfoFromFile ("DONGLE.NFO"))
		  {
		    printf ("Dongle File Read OK\n") ;
			dongleFileInfoCount = GetDongleFileInfoCount () ;
		  }
		  else
		  {
		    printf ("Error Reading Dongle File\n") ;
		  }
//		  currentDongleInfo = 0 ;
		  fileChoice = 0 ;
		}
		newCard = 0 ;
		break ;
	  }
	}

    ClearOrderTable () ;
	FillOrderTable () ;
    DrawOrderTable () ;

    switch (status)
    { 
      case memoryCardOptionsWin:
	  { 
	    DisplayFileList () ;
		break ;
	  }  
	  case createDongleWin:
	  {
		DisplayCreateDongleList () ;
		break ;
	  }
	  case checkDongleWin:
	  {
		DisplayCheckDongleList () ;
		break ;
	  }
	  case viewFileContentsWin:
	  {
	    DisplayFileContents (fileOffset) ;
	  }
	}
 
    ret = ProcessInput(GetInput ()) ;
	SwapBuffers () ;
  }
  DeInitializeCardAndEvents () ;
  DeinitializeGraphics () ;
}

void FillOrderTable (void)
{
  static int 		count = 0 ;

  DrawMainWindow () ;
  switch (status) 
  {
    case insertCardWin:
	{
      DrawInsertCardWindow () ;
	  break ;
	} 
	case errorInCardWin:
	{
	  DrawOKMessageWindow (&okWindow, 10) ;
      AddPrim (&cdb->orderTable[5], &optionsPoly[errorInCardOpt]) ;
	  break ;
	}
	case mainMenuWin:
	{
	  DrawMainMenuWindow () ;
	  break ;
	}
	case createDongleWin:
	{
	  DrawCreateDongleWindow () ;
	  break ;
	}
	case checkDongleWin:
	{
	  DrawCheckDongleWindow () ;
	  break ;
	}
	case memoryCardOptionsWin:
	{
	  DrawMemoryCardOptionsWindow () ;
	  break ;
	}
	case viewFileContentsWin:
	{
	  DrawFileContentsWindow () ;
	  break ;
	}
  }
}

void DrawMainWindow (void)
{
  DrawFrameWindow (&mainWindow[0], 130) ;
  DrawFrameWindow (&mainWindow[1], 100) ;
  AddPrim (&cdb->orderTable[80], &optionsPoly[logoOpt]) ;
}

void DrawInsertCardWindow (void)
{
  DrawFrameWindow (&insertCardWindow, 50) ;
  SetShadedOption (&optionsPoly[insertCardOpt]) ;
  AddPrim (&cdb->orderTable[40], &optionsPoly[insertCardOpt]) ;
}

void DrawMainMenuWindow (void) 
{
  DrawFrameWindow (&mainMenuWindow, 50) ;
  switch (choice)
  {
    case 0:  SetShadedOption (&optionsPoly[createDongleOpt]) ;  break ;
    case 1:  SetShadedOption (&optionsPoly[checkDongleOpt]) ;  break ;
    case 2:  SetShadedOption (&optionsPoly[memoryOptionsOpt]) ;  break ;
  }
  AddPrim (&cdb->orderTable[40], &optionsPoly[createDongleOpt]) ;
  AddPrim (&cdb->orderTable[30], &optionsPoly[checkDongleOpt]) ;
  AddPrim (&cdb->orderTable[20], &optionsPoly[memoryOptionsOpt]) ;
}

void DrawCreateDongleWindow (void)
{
  DrawStdWindow (&createDongleWindow, 50) ;
  AddPrim (&cdb->orderTable[40], &optionsPoly[enterTitleNameOpt]) ;
  AddPrim (&cdb->orderTable[35], &optionsPoly[enterCompanyNameOpt]) ;
  AddPrim (&cdb->orderTable[30], &optionsPoly[enterPersonNameOpt]) ;
  AddPrim (&cdb->orderTable[20], &optionsPoly[enterDateOpt]) ;
  AddPrim (&cdb->orderTable[15], &optionsPoly[enterKeyOpt]) ;
  AddPrim (&cdb->orderTable[10], &optionsPoly[dongleHelpOpt]) ;
}

void DrawCheckDongleWindow (void)
{
  DrawStdWindow (&checkDongleWindow, 50) ;
}

void DrawMemoryCardOptionsWindow (void)
{
  DrawStdWindow (&memoryCardOptionsWindow, 50) ;
  AddPrim (&cdb->orderTable[40], &optionsPoly[deleteFileOpt]) ;
  AddPrim (&cdb->orderTable[30], &optionsPoly[viewFileContentsOpt]) ;
}

void DrawFileContentsWindow (void) 
{
  DrawStdWindow (&fileContentsWindow, 50) ;
}

void DisplayFileContents (int fileOffset) 
{
  int 				row,
  					col ;
  char 				hex[6] ;
  char				charB[17] ;

  charB[16] = 0 ;
  FntLoad (640, 0) ;
  fileContents = FntOpen (57, 80, 525, 112, 2, 1024) ;
  FntPrint (fileContents, "~c555Offset ~c777%d~c555/~c777%d\n", fileOffset, 8192) ;
  for (row=0; row<ROW_SIZE; row++)
  {
    FntPrint (fileContents, "~c006") ;
    for (col=0; col<COL_SIZE; col++)
	{
	  sprintf (hex, "%.2x", fileBuffer[fileOffset]) ;
	  FntPrint (fileContents, "%s ", hex) ;
	  charB[col] = fileBuffer[fileOffset] ;
	  if (charB[col] < 20) charB[col] = 32 ;
	  fileOffset ++ ;
	}
	FntPrint (fileContents, " ~c060%s\n", charB) ;
  }
  FntFlush (fileContents) ;
}

void DisplayFileList (void)
{
  int				loop ;
  char				buffer[80] ;

  FntLoad (640, 0) ;
  fileList = FntOpen (52, 72, 196, 126, 2, 500) ;
  fileInfo = FntOpen (262, 72, 304, 86, 2, 256) ;
						
  DrawFrameWindow (&fileListWindow, 30) ;
  DrawFrameWindow (&fileInfoWindow, 30) ;
  DrawFrameWindow (&fileKanjiWindow, 30) ;
  for (loop=0; loop<fileCount; loop++)
  {
    sprintf (buffer, "~c080 %s\n", dirEntries[loop].name) ; //-21
    FntPrint (fileList, buffer) ;
  }
  if (fileCount > 0)
  {
    FntPrint (fileInfo, "~c555Size         ~c080%d bytes\n", dirEntries[fileChoice].size) ;
    FntPrint (fileInfo, "~c555Head         ~c080%d\n", dirEntries[fileChoice].head) ;
    FntPrint (fileInfo, "~c555Attr (HEX)   ~c080%x\n\n", dirEntries[fileChoice].attr) ;

    FntPrint (fileInfo, "~c555Magic        ~c080%c%c\n", fileHeader[fileChoice].magic[0], fileHeader[fileChoice].magic[1]) ;
    FntPrint (fileInfo, "~c555Type (HEX)   ~c080%x\n", fileHeader[fileChoice].type) ;
    FntPrint (fileInfo, "~c555Block Entry  ~c080%d\n", fileHeader[fileChoice].blockEntry) ;
    FntPrint (fileInfo, "~c555Title (ASCII)\n~c080%s\n", fileHeader[fileChoice].title) ;
	if (fileHeader[fileChoice].title[0] == 0x82) 
    {	
      KanjiFntPrint (fileKanji, "%s\n", fileHeader[fileChoice].title) ;
	}
	else
	{
      KanjiFntPrint (fileKanji, "~pInvalid Kanji Text!!!\n") ;
	}
  }
  KanjiFntFlush (fileKanji) ;
  FntFlush (fileList) ; 
  FntFlush (fileInfo) ;
  DrawFileSelectionBar () ;
}

void DrawFileSelectionBar (void) 
{
  RECT			rect ;

  rect.x = fileListWindow.main.x0 + 2 ;
  rect.y = fileListWindow.main.y0 + 2 + (fileChoice * FONT_HEIGHT) ;
  rect.w = fileListWindow.main.x3 - fileListWindow.main.x0 - 4 ;
  rect.h = FONT_HEIGHT ;
  setXYWH (&fileSelectionBar, rect.x, rect.y, rect.w, rect.h) ;
  DrawPrim (&fileSelectionBar) ;
}

void UpdateFileInfo (void) 
{
  int			loop ;

  GetFileDirectoryFromCard (0x00, &dirEntries[0]) ;
  for (loop=0; loop<fileCount; loop++)
  {
    GetFileHeaderFromCard (0x00, dirEntries[loop].name, &fileHeader[loop]) ;
  }
/*  GetAllDongleFileInfoFromCard () ;*/
  GetDongleFileInfoFromCard (0x00) ;
//  GetCardData (0x00, cardBuffer) ;
}

void DisplayCreateDongleList (void)
{
  long			titleNameArea,
  				companyNameArea,
				personNameArea,
				dateArea,
				keyArea,
				currentDongleArea ;

  FntLoad (640, 0) ;

  titleNameArea = FntOpen (320, 85, 160, 8, 2, 30) ;
  companyNameArea = FntOpen (320, 105, 160, 8, 2, 30) ;
  personNameArea = FntOpen (320, 125, 160, 8, 2, 30) ;
  dateArea = FntOpen (320, 145, 64, 8, 2, 40) ;
  keyArea = FntOpen (320, 165, 8, 8, 2, 5) ;
 
  FntPrint (titleNameArea, "~c080%s", dongleInfo.titleName) ;
  FntPrint (companyNameArea, "~c080%s", dongleInfo.companyName) ;
  FntPrint (personNameArea, "~c080%s", dongleInfo.personName) ;
//  FntPrint (dateArea, "~c080%s", dongleInfo.date) ;
  FntPrint (dateArea, "~c080%c%c~c444/~c080%c%c~c444/~c080%c%c", 
  								 dongleInfo.date[0],
  								 dongleInfo.date[1],
  								 dongleInfo.date[3],
  								 dongleInfo.date[4],
  								 dongleInfo.date[6],
  								 dongleInfo.date[7]) ;
  FntPrint (keyArea, "~c080%s", keyStr) ;

  FntFlush (titleNameArea) ;
  FntFlush (companyNameArea) ;
  FntFlush (personNameArea) ;
  FntFlush (dateArea) ;
  FntFlush (keyArea) ;

//  FntLoad (640, 0) ;
  currentDongleArea = FntOpen (270, 65, 90, 8, 2, 30) ;
  FntPrint (currentDongleArea, "~c444Dongle ~c880%d~c444/~c660%d", currentDongleInfo, dongleFileInfoCount) ;
  FntFlush (currentDongleArea) ;

  DrawCharSelectionBar () ;
}

void DisplayCheckDongleList (void)
{
  long			dongleInfoArea ;
  int			loop ;
  int			found = 0 ;
  char			buffer[8192] ;

  FntLoad (640, 0) ;
  key = keyStr[0] - '0' ;
  if (CheckForDongle (0x00, &dongleFileInfo, key) == DONGLE_FOUND) 
  {
    printf ("Dongle Found\n") ;
    dongleInfoArea = FntOpen (155, 110, 330, 56, 2, 255) ;
	FntPrint (dongleInfoArea, "~c666Key [%c]\n\n", keyStr[0]) ;
    FntPrint (dongleInfoArea, " ~c555Name of Title     ~c080%s\n", dongleFileInfo.titleName) ;
    FntPrint (dongleInfoArea, " ~c555Company Name      ~c080%s\n", dongleFileInfo.companyName) ;
    FntPrint (dongleInfoArea, " ~c555Name of Person    ~c080%s\n", dongleFileInfo.personName) ;
    FntPrint (dongleInfoArea, " ~c555Date              ~c080%s\n", dongleFileInfo.date) ;	  
	FntFlush (dongleInfoArea) ;
  }
  else
  {
    printf ("Dongle Not Found\n") ;
    DrawPrim (&optionsPoly[dongleNotFoundOpt]) ; 
  }
}

void DrawCharSelectionBar (void)
{
  #define		CURSOR_DELAY	25

  static int	delay = 0 ;
  RECT			rect ;

  rect.x = DONGLE_INPUT_X + (FONT_WIDTH * dongleInputChoiceChar) ;
  rect.w = FONT_WIDTH ;
  rect.h = FONT_HEIGHT ;

  switch (dongleInputChoiceLine)
  {
    case 0:  rect.y = DONGLE_INPUT_Y ;  break ;
    case 1:  rect.y = DONGLE_INPUT_Y + 20 ;  break ;
    case 2:  rect.y = DONGLE_INPUT_Y + 40 ;  break ;
    case 3:  rect.y = DONGLE_INPUT_Y + 60 ;  break ;
	case 4:  rect.y = DONGLE_INPUT_Y + 80 ;  break ;
  }

  delay ++ ;

  setXYWH (&dongleCharSelectionBar, rect.x, rect.y, rect.w, rect.h) ;
   
  if (delay % CURSOR_DELAY * 2 < CURSOR_DELAY)
  {
    setRGB0 (&dongleCharSelectionBar, 0x40, 0x40, 0x40) ;
  }
  else
  {
    setRGB0 (&dongleCharSelectionBar, 0x90, 0x90, 0x90) ;
  }
  DrawPrim (&dongleCharSelectionBar) ;
}

void InitializeOptionsGraphics ()
{
  int 				loop ;
  u_short			tPage ;
  RECT				rect ;

  setRECT (&rect, 640, 256, 256, 256) ;
  LoadImage (&rect, dongleTim+5) ;  //TIM1
  tPage = GetTPage (2, 0, 640, 256) ;
  for (loop=0; loop<NUM_OPTIONS; loop++)
  {
    SetPolyFT4 (&optionsPoly[loop]) ;
	setRGB0 (&optionsPoly[loop], 0x80, 0x80, 0x80) ;
	setXYWH (&optionsPoly[loop], otXY[loop].x, otXY[loop].y, otXY[loop].w, otXY[loop].h) ;
	setUVWH (&optionsPoly[loop], otUV[loop].x, otUV[loop].y, otUV[loop].w, otUV[loop].h) ;
	optionsPoly[loop].tpage = tPage ; 
	SetSemiTrans (&optionsPoly[loop], 0) ;
	SetShadeTex (&optionsPoly[loop], 0) ;
  }
}

void InitializeWindows (void)
{
  RECT			rect ;
  int			x, y ;

  setRECT (&rect, 0, 0, 640, 480) ;
  SetFrameWindow (&mainWindow[0], &rect, 4, BORDER_UP) ;
  setRECT (&rect, 40, 20, 560, 30) ;
  SetFrameWindow (&mainWindow[1], &rect, 1, BORDER_DOWN) ;
  setRECT (&rect, 150, 100, 340, 90) ;
  SetFrameWindow (&insertCardWindow, &rect, 1, BORDER_UP) ;
  setRECT (&rect, 130, 80, 380, 130) ;
  SetOKWindow (&okWindow, &rect, 1, BORDER_UP, &otUV[iconOKOpt]) ;
  setRECT (&rect, 110, 60, 400, 150) ;
  SetFrameWindow (&mainMenuWindow, &rect, 1, BORDER_UP) ;
  setRECT (&rect, 80, 60, 480, 170) ;
  SetStdWindow (&createDongleWindow, &rect, 1, BORDER_UP, &otUV[iconTriangleOpt]) ;
  setRECT (&rect, 100, 100, 440, 75) ;
  SetStdWindow (&checkDongleWindow, &rect, 1, BORDER_UP, &otUV[iconTriangleOpt]) ;
  setRECT (&rect, 30, 60, 580, 170) ;
  SetStdWindow (&memoryCardOptionsWindow, &rect, 1, BORDER_UP, &otUV[iconTriangleOpt]) ;
  setRECT (&rect, 50, 70, 200, 130) ;
  SetFrameWindow (&fileListWindow, &rect, 2, BORDER_DOWN) ;
  setRECT (&rect, 260, 70, 310, 90) ;
  SetFrameWindow (&fileInfoWindow, &rect, 2, BORDER_DOWN) ;
  setRECT (&rect, 260, 165, 310, 35) ;
  SetFrameWindow (&fileKanjiWindow, &rect, 2, BORDER_DOWN) ;
  fileKanji = KanjiFntOpen (262, 167, 304, 31, 640+128, 0, 640, 190, 2, 100) ;
  setRECT (&rect, 50, 60, 540, 136) ;
  SetStdWindow (&fileContentsWindow, &rect, 1, BORDER_UP, &otUV[iconTriangleOpt]) ;

  SetPolyF4 (&fileSelectionBar) ;
  setRGB0 (&fileSelectionBar, 0x80, 0x80, 0x80) ;
  setXYWH (&fileSelectionBar, 52, 92, FONT_HEIGHT, 126) ;
  setSemiTrans (&fileSelectionBar, 1) ;

  SetPolyF4 (&dongleCharSelectionBar) ;
  setRGB0 (&dongleCharSelectionBar, 0x80, 0x80, 0x80) ;
  setXYWH (&dongleCharSelectionBar, 40, 40, FONT_HEIGHT, FONT_WIDTH) ;
  setSemiTrans (&dongleCharSelectionBar, 1) ;

  x = rect.x + ((rect.w - otUV[errorInCardOpt].w) / 2) ;
  y = rect.y + (((rect.h - otUV[errorInCardOpt].h) / 3) * 1) ;
  setXYWH (&optionsPoly[errorInCardOpt], x, y, otUV[errorInCardOpt].w, otUV[errorInCardOpt].h) ;
}

void SetShadedOption (POLY_FT4 *poly)
{
  static int		shade = 0x80 ;
  static int		shadeV = 8 ;

  if (shade + shadeV < 50 || shade + shadeV > 200)
  {
    shadeV = -shadeV ;
  }
  shade += shadeV ;
  setRGB0 (poly, shade, shade, shade) ;
}

void ResetShadedOption (POLY_FT4 *poly)
{
  setRGB0 (poly, 0x80, 0x80, 0x80) ;
}

int GetInput (void) 
{
  #define MAX_DELAY			4

  static int	delay = 0 ;

  u_long		pad ;
  int			ret ;

  if (delay > 0)
  {
    delay ++ ;
	if (delay > MAX_DELAY)
	{
	  delay = 0 ;
	}
	return OPTION_NONE ;
  }

  ret = OPTION_NONE ;
  pad = PadRead (1) ;

  if (pad & PADselect)	ret = OPTION_QUIT ;
  if (pad & PADLup) 	ret = OPTION_UP ;
  if (pad & PADLdown) 	ret = OPTION_DOWN ;
  if (pad & PADLleft) 	ret = OPTION_LEFT ;
  if (pad & PADLright) 	ret = OPTION_RIGHT ;
  if (pad & PADRup)		ret = OPTION_TRIANGLE ;
  if (pad & PADRleft)	ret = OPTION_SQUARE ;
  if (pad & PADRdown)	ret = OPTION_CROSS ;
  if (pad & PADRright)	ret = OPTION_CIRCLE ;
  if (pad & PADselect)  ret = OPTION_SELECT ;
  if (pad & PADstart)   ret = OPTION_START ;
  if (pad & PADL1)		ret = OPTION_LEFT1 ;
  if (pad & PADL2)		ret = OPTION_LEFT2 ;
  if (pad & PADR1)		ret = OPTION_RIGHT1 ;
  if (pad & PADR2)		ret = OPTION_RIGHT2 ;

  if (ret != OPTION_NONE)
  {
    delay = 1 ;
  }
  return ret ;
}

int ProcessInput (int input)
{
  static int		lastChoice = 0 ;

  if (input==OPTION_NONE)	return 0 ;

  switch (status)
  {
    case errorInCardWin:
	{
	  ProcessErrorInCardInput (input) ;
	  break ;
	}
    case mainMenuWin:
    {
      ProcessMainMenuInput (input) ;
	  break ;
    }
    case memoryCardOptionsWin:
    {
	  ProcessMemoryCardOptionsInput (input) ;
	  break ;
    }
	case createDongleWin:
	{
	  ProcessCreateDongleInput (input) ;
	  break ;
	}
	case checkDongleWin:
	{
	  ProcessCheckDongleInput (input) ;
	  break ;
	}
	case viewFileContentsWin:
	{
	  ProcessViewFileContentsInput (input) ;
	  break ;
	}
  }
  ResetShadedOption (&optionsPoly[createDongleOpt+lastChoice]) ;
  lastChoice = choice ;
}

void ProcessErrorInCardInput (input) 
{
  switch (input)
  {
    case OPTION_CIRCLE:
	{
	  overRide = 1 ;
	  break ;
	}
  }
}

void ProcessMainMenuInput (int input)
{
  switch (input)
  {
    case OPTION_UP:		
    {
      if (choice > 0) choice -- ; 
	  break ;
	} 
	case OPTION_DOWN:	
	{
	  if (choice < 2) choice ++ ; 
	  break ;
	}
	case OPTION_CROSS:
	{
	  if (choice == 0) status = createDongleWin ;
	  if (choice == 1) status = checkDongleWin ;
	  if (choice == 2) status = memoryCardOptionsWin ;
	  break ;
	}
  }
}

void ProcessCreateDongleInput (int input) 
{
  int				loop ;

  switch (input)
  {
    case OPTION_TRIANGLE:
	{
	  status = mainMenuWin ;
	  break ;
	}
	case OPTION_UP:
	{
	  if (dongleInputChoiceLine > 0)
	  {
	    dongleInputChoiceLine -- ;
		if (dongleInputChoiceChar > DONGLE_WIDTH[dongleInputChoiceLine]-1)
		{
		  dongleInputChoiceChar = DONGLE_WIDTH[dongleInputChoiceLine]-1 ;
		}
	  }
	  break ;
	}
	case OPTION_DOWN:
	{
	  if (dongleInputChoiceLine < NUM_DONGLE_OPTIONS - 1)
	  {
	    dongleInputChoiceLine ++ ;
		if (dongleInputChoiceChar > DONGLE_WIDTH[dongleInputChoiceLine]-1)
		{
		  dongleInputChoiceChar = DONGLE_WIDTH[dongleInputChoiceLine]-1 ;
		}
	    if (dongleInputChoiceLine == 3)
	    {
	      if (dongleInfo.date[dongleInputChoiceChar] == '/')
		  {
		    dongleInputChoiceChar -- ;
		  }
	    }
	  }
	  break ;
	}
	case OPTION_LEFT:
	{
	  if (dongleInputChoiceChar > 0)
	  {
	    dongleInputChoiceChar -- ;
	  } 
	  if (dongleInputChoiceLine == 3)
	  {
	    if (dongleInfo.date[dongleInputChoiceChar] == '/')
		{
		  dongleInputChoiceChar -- ;
		}
	  }
 	  break ;
	}
	case OPTION_RIGHT:
	{
	  if (dongleInputChoiceChar < DONGLE_WIDTH[dongleInputChoiceLine]-1) 
	  {
	    dongleInputChoiceChar ++ ;
	  }
	  if (dongleInputChoiceLine == 3)
	  {
	    if (dongleInfo.date[dongleInputChoiceChar] == '/')
		{
		  dongleInputChoiceChar ++ ;
		}
	  }
	  break ;
	}
	case OPTION_SQUARE:
	{
	  switch (dongleInputChoiceLine)
	  { 
		case 0:			DecreaseCharacter (&dongleInfo.titleName[dongleInputChoiceChar]) ; break ;
		case 1:			DecreaseCharacter (&dongleInfo.companyName[dongleInputChoiceChar]) ; break ;
		case 2:			DecreaseCharacter (&dongleInfo.personName[dongleInputChoiceChar]) ; break ;
		case 3:			DecreaseNumber (&dongleInfo.date[dongleInputChoiceChar]) ; break ;
		case 4:			DecreaseNumber (&keyStr[dongleInputChoiceChar]) ; break ;
	  }
	  break ;
	}
	case OPTION_CIRCLE:
	{
	  switch (dongleInputChoiceLine)
	  { 
		case 0:			IncreaseCharacter (&dongleInfo.titleName[dongleInputChoiceChar]) ; break ;
		case 1:			IncreaseCharacter (&dongleInfo.companyName[dongleInputChoiceChar]) ; break ;
		case 2:			IncreaseCharacter (&dongleInfo.personName[dongleInputChoiceChar]) ; break ;
		case 3:			IncreaseNumber (&dongleInfo.date[dongleInputChoiceChar]) ; break ;
		case 4:			IncreaseNumber (&keyStr[dongleInputChoiceChar]) ; break ;
	  }
	  break ;
	}
	case OPTION_LEFT1:
	{
	  switch (dongleInputChoiceLine)
	  { 
		case 0:			for (loop=0; loop<6; loop++) 
						  DecreaseCharacter (&dongleInfo.titleName[dongleInputChoiceChar]) ; break ;
		case 1:			for (loop=0; loop<6; loop++) 
						  DecreaseCharacter (&dongleInfo.companyName[dongleInputChoiceChar]) ; break ;
		case 2:			for (loop=0; loop<6; loop++)
						  DecreaseCharacter (&dongleInfo.personName[dongleInputChoiceChar]) ; break ;
		case 3:			for (loop=0; loop<4; loop++)
						  DecreaseNumber (&dongleInfo.date[dongleInputChoiceChar]) ; break ;
		case 4:			for (loop=0; loop<4; loop++)
						  DecreaseNumber (&keyStr[dongleInputChoiceChar]) ; break ;
	  }
	  break ;
	}
	case OPTION_RIGHT1:
	{
	  switch (dongleInputChoiceLine)
	  { 
		case 0:			for (loop=0; loop<6; loop++) 
						  IncreaseCharacter (&dongleInfo.titleName[dongleInputChoiceChar]) ; break ;
		case 1:			for (loop=0; loop<6; loop++) 
						  IncreaseCharacter (&dongleInfo.companyName[dongleInputChoiceChar]) ; break ;
		case 2:			for (loop=0; loop<6; loop++)
						  IncreaseCharacter (&dongleInfo.personName[dongleInputChoiceChar]) ; break ;
		case 3:			for (loop=0; loop<4; loop++)
						  IncreaseNumber (&dongleInfo.date[dongleInputChoiceChar]) ; break ;
		case 4:			for (loop=0; loop<4; loop++)
						  IncreaseNumber (&keyStr[dongleInputChoiceChar]) ; break ;
	  }
	  break ;
	}
	case OPTION_LEFT2:
	{
	  currentDongleInfo -- ;
	  if (currentDongleInfo > 0)
	  {
	    GetDongleInfo (&dongleInfo, &key, currentDongleInfo) ;
		sprintf (keyStr, "%d", key) ;
	  }
	  else
	  {
	    currentDongleInfo = 0 ;
	    strcpy (dongleInfo.titleName, "                    ") ;
        strcpy (dongleInfo.personName, "                    ") ;
		strcpy (dongleInfo.companyName, "                    ") ;
		strcpy (dongleInfo.date, "15/10/96") ;
		key = 0 ; 
		sprintf (keyStr, "%d", key) ;
	  }
	  break ;
	}
	case OPTION_RIGHT2:
	{
	  currentDongleInfo ++ ;
	  if ((GetDongleInfo (&dongleInfo, &key, currentDongleInfo)))
	  {
	    sprintf (keyStr, "%d", key) ;
	  }
	  else
	  {
  	    currentDongleInfo -- ;
	  }
	  break ;
	}
	case OPTION_START:
	{
	  key = atoi (keyStr) ;
	  CreateDongle (0x00, &dongleInfo, key) ;
	  GetCardData (0x00, cardBuffer) ;
	  status = mainMenuWin ;
	  break ;
	}
  }
}

void ProcessCheckDongleInput (int input)
{
  switch (input)
  {
    case OPTION_TRIANGLE:
	{
	  status = mainMenuWin ;
	  break ;
	}
	case OPTION_LEFT:
	{
	  DecreaseNumber (&keyStr[0]) ;
	  break ;
	}
	case OPTION_RIGHT:
	{
	  IncreaseNumber (&keyStr[0]) ;
	}
  }
}


void ProcessMemoryCardOptionsInput (int input)
{
  switch (input)
  {
    case OPTION_TRIANGLE:
	{
	  status = mainMenuWin ;
	  break ;
	}
	case OPTION_UP:
	{
	  if (fileChoice > 0)
	  {
	    fileChoice -- ;
	  }
	  break ;
	}
	case OPTION_DOWN:
	{
	  if (fileChoice < fileCount-1)
	  {
	    fileChoice ++ ;
	  }
	  break ;
	}
	case OPTION_SQUARE:
	{
	  if (fileCount > 0)
	  {
	    DeleteFileFromCard (0x00, dirEntries[fileChoice].name) ;
		UpdateFileInfo () ;
		if (fileChoice>0) 
		{
		  fileChoice -- ;
		}
	  }
	  break ;
	}
	case OPTION_CIRCLE:
	{
//	  if (cardDataLoaded == 0)
	  {
	  	ReadFileBlocking (0x00, dirEntries[fileChoice].name, 1, fileBuffer) ;
//        GetCardData (0x00, cardBuffer) ;
   		  
    	status = viewFileContentsWin ;
//		cardDataLoaded = 1 ;
	  }
	  break ;
	}
  }
}

int ProcessViewFileContentsInput (int input) 
{
  switch (input)
  {
    case OPTION_TRIANGLE:
	{
	  status = memoryCardOptionsWin ;
	  break ;
	}
	case OPTION_UP:
	{
/*	  if (fileOffset - (COL_SIZE * ROW_SIZE) >= 0)*/
	  if (fileOffset - 128 >= 0)
	  {
/*	    fileOffset -= COL_SIZE * ROW_SIZE ;*/
	    fileOffset -= 128 ;
	  }
	  else
	  {
	    fileOffset = 0 ;
	  }
	  break ;
	}
	case OPTION_DOWN:
	{
/*	  if (fileOffset + (COL_SIZE * ROW_SIZE) < 8192 - 1)*/
	  if (fileOffset + 128 < (8192) - 1)
	  {
/*	    fileOffset += COL_SIZE * ROW_SIZE ;*/
	    fileOffset += 128 ;
	  }
	  else
	  {
/*	    fileOffset = 8192 - (COL_SIZE * ROW_SIZE) - 1 ;*/
	    fileOffset = 8192 - (128) - 1 ;
	  }
	  break ;
	}
	case OPTION_RIGHT:
	{
	  if (fileOffset + 1024 < (8192) - 1)
	  {
	    fileOffset += 1024 ;
	  }
	  else
	  {
	    fileOffset = (8192) - 1024 - 1 ;
	  }
	  break ;
	}
	case OPTION_LEFT:
	{
	  if (fileOffset - 1024 >= 0)
	  {
	    fileOffset -= 1024 ;
	  }
	  else
	  {
	    fileOffset = 0 ;
	  }
	  break ;
	}
  }
}


int DecreaseCharacter (char *c)
{
  if (*c > 'A' && *c <= 'Z')
  {
    *c = *c - 1 ;
	return 0 ;
  }
  if (*c > '0' && *c <= '9')
  {
    *c = *c - 1 ;
	return 0 ;
  }
  if (*c == 'A')
  {
    *c = ' ' ;
	return 0 ;
  }
  if (*c == '0')
  {
    *c = 'Z' ;
	return 0 ;
  }
}

int IncreaseCharacter (char *c)
{
  if (*c >= 'A' && *c < 'Z')
  {
    *c = *c + 1 ;
	return 0 ;
  }
  if (*c >= '0' && *c < '9') 
  {
    *c = *c + 1 ;
	return 0 ;
  }
  if (*c == ' ')
  {
    *c = 'A' ;
	return 0 ;
  }
  if (*c == 'Z')
  {
    *c = '0' ;
	return 0 ;
  }
}

void DecreaseNumber (char *c)
{
  if (*c > '0')
  {
    *c = *c - 1 ;
  }
}

void IncreaseNumber (char *c)
{
  if (*c < '9')
  {
    *c = *c + 1 ;
  }
}



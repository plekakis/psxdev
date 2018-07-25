/* DONGLE File */

#include <kernel.h>

#include "card.h"
#include "dongle.h"

DONGLE_INFO		dongleFileInfo = {"                    ",
						  		  "                    ", 
						  		  "                    ", 
						  		  "  /  /  "} ;

char			dongleFileBuffer[8192] ;
int				dongleFound = 0 ;

int CreateDongle (long channel, DONGLE_INFO *pDongleInfo, int key) 
{
  FILE_HEADER	*fileHeader ;
  char			fileName[21] ;
  int			count = 0,
				loop ;

  if (CheckBlockSpacesAvailable(channel) == 0)
  { 
    return DONGLE_CARD_FULL ;
  }

  for (loop=0; loop<8192; loop++)
  {
    dongleFileBuffer[loop] = 0xFF ;
  }

  sprintf (fileName, "BESCES-00000DONGLE") ;
  if (FileExistsOnCard (channel, fileName))
  {
    DeleteFileFromCard (channel, fileName) ;
  }

  fileHeader = (FILE_HEADER *)dongleFileBuffer ;
  fileHeader->magic[0] = 'S' ;
  fileHeader->magic[1] = 'C' ;
  fileHeader->type = 0 ;
  fileHeader->blockEntry = 1 ;

  //CopyAsciiStringToShiftJIS ("DONGLE FILE TITLE", fileHeader->title) ;
  asc2kanji ("DONGLE FILE TITLE", fileHeader->title) ;

//  strcpy (fileHeader->title, "DONGLE FILE") ;

  EncryptDongleInfo (pDongleInfo, dongleFileBuffer, key) ;
  
  if (WriteFileBlocking (channel, fileName, 1, dongleFileBuffer))
  {
    UpdateFileInfo () ;
  }
  else
  {
    return DONGLE_WRITE_ERROR ;
  }
  return DONGLE_CREATED ;
}

int CheckForDongle (long channel, DONGLE_INFO *pDongleInfo, int key) 
{
  printf ("CFD Before DecryptDongleInfo\n") ;
  if (dongleFound)
  {
    if (DecryptDongleInfo (pDongleInfo, dongleFileBuffer, key)==DONGLE_FOUND)
    {
  	  printf ("CFD Dongle Found\n") ;
	  return DONGLE_FOUND ;
    }
  }
  printf ("CFD Dongle Not Found\n") ;
  return DONGLE_NOT_FOUND ;
}

int GetDongleFileInfoFromCard (long channel) 
{
  int				loop ;

  dongleFound = 0 ;
  printf ("File Count: %d\n", fileCount) ;
  for (loop=0; loop<fileCount; loop++)
  {
    if (strncmp (dirEntries[loop].name, "BESCES-00000DONGLE", 11) == 0)
	{
	  dongleFound = 1 ;
	  ReadFileBlocking (channel, dirEntries[loop].name, 1, dongleFileBuffer) ;
  	  return DONGLE_FOUND ;
	}
  }
  return DONGLE_NOT_FOUND ;
}

void EncryptDongleInfo (DONGLE_INFO *pDongleInfo, char *buffer, int key)
{
  strcpy (buffer + DONGLE_ID_OFFSET, "DONGLE") ;
  strcpy (buffer + TITLE_NAME_OFFSET, pDongleInfo->titleName) ;
  strcpy (buffer + COMPANY_NAME_OFFSET, pDongleInfo->companyName) ;
  strcpy (buffer + PERSON_NAME_OFFSET, pDongleInfo->personName) ;
  strcpy (buffer + DATE_OFFSET, pDongleInfo->date) ;
}

int DecryptDongleInfo (DONGLE_INFO *pDongleInfo, char *buffer, int key)
{
  char 		dongleID[7] ;
  int 		loop ;

  strcpy (dongleID, buffer + DONGLE_ID_OFFSET) ;

  if (strcmp (dongleID, "DONGLE") != 0) 
  {
    return DONGLE_NOT_FOUND ;
  }
  
  strncpy (pDongleInfo->titleName, buffer + TITLE_NAME_OFFSET, 20) ;
  strncpy (pDongleInfo->companyName, buffer + COMPANY_NAME_OFFSET, 20) ;
  strncpy (pDongleInfo->personName, buffer + PERSON_NAME_OFFSET, 20) ;
  strncpy (pDongleInfo->date, buffer + DATE_OFFSET, 8) ;

  return DONGLE_FOUND ;
}

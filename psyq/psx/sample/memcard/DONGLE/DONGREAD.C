/* DONGLE PCFS code */

#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include "dongread.h"

char			dongleBuffer[64000] ;

int ReadDongleInfoFromFile (char *pcFilename)
{
  int				pcFile ;
  char				line[80] ;
  int				loop ;
  int				length ;
  int				count ;

  PCinit () ;
  pcFile = PCopen (pcFilename, 0) ;

  if (pcFile == -1)
  {
    return 0 ;
  }

  length = PCread (pcFile, dongleBuffer, 64000) ;
  PCclose (pcFile) ;

  return 1 ;
}

int GetDongleFileInfoCount (void)
{
  DONGLE_INFO			tempDongleInfo ;
  int					key ;
  int					index = 1 ;

  while (GetDongleInfo (&tempDongleInfo, &key, index)) 
  {
    index ++ ;
  }
  return index - 1 ;
}

int GetDongleInfo (DONGLE_INFO *dongleInfo, int *key, int index) 
{
  DONGLE_FILE_INFO		dongleFileInfo ;

  if (GetDongleInfoFromStringArray (&dongleFileInfo, dongleBuffer, index))
  {
    strcpy (dongleInfo->titleName, dongleFileInfo.titleName) ;
	strcpy (dongleInfo->personName, dongleFileInfo.personName) ;
	strcpy (dongleInfo->companyName, dongleFileInfo.companyName) ;
	strcpy (dongleInfo->date, dongleFileInfo.date) ;
    *key = atoi (dongleFileInfo.key) ;
    return 1 ;
  }
  return 0 ;
}

int GetDongleInfoFromStringArray (DONGLE_FILE_INFO *dongleInfo, char *buffer, int index) 
{
  int			lineCount = 1 ;
  int			dongleCount = 0 ;
  char			line[80] ;

  strcpy (dongleInfo->titleName, "                    ") ;
  strcpy (dongleInfo->personName, "                    ") ;
  strcpy (dongleInfo->companyName, "                    ") ;
  while (GetLineFromStringArray (buffer, line, lineCount))
  {
    if (strncmp (line, "[DONGLE]", 8) == 0)
	{
	  dongleCount ++ ;
	}
	if (dongleCount == index)
	{
	  GetLineFromStringArray (buffer, line, lineCount+1) ;
	  strncpy (dongleInfo->titleName, line, strlen(line)) ;
	  dongleInfo->titleName[strlen(line)] = ' ' ;
	  GetLineFromStringArray (buffer, line, lineCount+2) ;
	  strncpy (dongleInfo->personName, line, strlen(line)) ;
	  dongleInfo->personName[strlen(line)] = ' ' ;
	  GetLineFromStringArray (buffer, line, lineCount+3) ;
	  strncpy (dongleInfo->companyName, line, strlen(line)) ;
	  dongleInfo->companyName[strlen(line)] = ' ' ;
	  GetLineFromStringArray (buffer, line, lineCount+4) ;
	  strncpy (dongleInfo->date, line, 8) ;
	  GetLineFromStringArray (buffer, line, lineCount+5) ;
	  strncpy (dongleInfo->key, line, 2) ;
	  return 1 ;
	}
	lineCount ++ ;
  }
  return 0 ;
}

int GetLineFromStringArray (char *source, char *dest, int lineNo) 
{
  int			loop ;

  for (loop=1; loop<=lineNo; loop++)
  {
    if (loop == lineNo)
	{
	  while (*source != '\n' && *source != '\0')
	  {
	    *dest = *source ;
		if (*dest >= 'a' && *dest <= 'z') *dest += ('A'-'a') ;
		source ++ ;
		dest ++ ;
	  }
	  dest -- ;
	  *dest = '\0' ;
	  return 1 ;
	}
	else
	{
      while (*source != '\n' && *source != '\0')
	    source++ ;
	  if (*source == '\0') 
	    return 0 ;
	  else
	    source ++ ;
	}
  }

  return 0 ;
}

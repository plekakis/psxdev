/*******************************************************************
 *
 *    DESCRIPTION:		Card Handling Routines
 *
 *    AUTHOR:			Malachy Duffin
 *
 *    HISTORY:    		Created August 96
 *
 *******************************************************************/

/** include files **/

#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>

#include "card.h"

/*
 ** InitializeCardAndEvents
 *
 *  PARAMETERS:		None
 *
 *  DESCRIPTION:	Initializes the memory card and the events associated with it
 *
 *  RETURNS:		None
 *
 */

struct DIRENTRY 	dirEntries[15] ;
int					fileCount ;
FILE_HEADER			fileHeader[15] ;
char				fileBuffer[8192] ;
int					cardDataLoaded ;

void InitializeCardAndEvents (void)
{
  ev0 = OpenEvent(SwCARD, EvSpIOE, EvMdNOINTR, NULL) ;
  ev1 = OpenEvent(SwCARD, EvSpERROR, EvMdNOINTR, NULL) ;
  ev2 = OpenEvent(SwCARD, EvSpTIMOUT, EvMdNOINTR, NULL) ;
  ev3 = OpenEvent(SwCARD, EvSpNEW, EvMdNOINTR, NULL) ;
  ev10 = OpenEvent(HwCARD, EvSpIOE, EvMdNOINTR, NULL) ;
  ev11 = OpenEvent(HwCARD, EvSpERROR, EvMdNOINTR, NULL) ;
  ev12 = OpenEvent(HwCARD, EvSpTIMOUT, EvMdNOINTR, NULL) ;
  ev13 = OpenEvent(HwCARD, EvSpNEW, EvMdNOINTR, NULL) ;

  EnterCriticalSection() ;
  InitCARD(1) ;
  StartCARD() ;
  _bu_init() ;
  ExitCriticalSection() ;


/*  EnterCriticalSection() ;
  InitCARD(1) ;
  ExitCriticalSection() ;
  StartCARD() ;  ** Crashes after calling this function
  _bu_init() ;*/
	
  EnableEvent(ev0) ;
  EnableEvent(ev1) ;
  EnableEvent(ev2) ;
  EnableEvent(ev3) ;
  EnableEvent(ev10) ;
  EnableEvent(ev11) ;
  EnableEvent(ev12) ;
  EnableEvent(ev13) ;
}

/*
 ** DeInitializeCardAndEvents
 *
 *  PARAMETERS:			None
 *
 *  DESCRIPTION:		DeInitializes the memory card and the events associated with it
 *
 *  RETURNS:			None
 *
 */

void DeInitializeCardAndEvents (void) 
{
  StopCARD () ;
  
  DisableEvent (ev0) ;
  DisableEvent (ev1) ;
  DisableEvent (ev2) ;
  DisableEvent (ev3) ;
  DisableEvent (ev10) ;
  DisableEvent (ev11) ;
  DisableEvent (ev12) ;
  DisableEvent (ev13) ;

  CloseEvent (ev0) ;
  CloseEvent (ev1) ;
  CloseEvent (ev2) ;
  CloseEvent (ev3) ;
  CloseEvent (ev10) ;
  CloseEvent (ev11) ;
  CloseEvent (ev12) ;
  CloseEvent (ev13) ;
}

/*
 ** GetCardStatus
 *
 *  PARAMETERS:		
 *		long channel		The channel the memory card is connected to.
 *							Possible values are
 *								0x00	(0x01	0x02	0x03)*
 *								0x10	(0x11	0x12	0x13)*
 *							*Multi-Tap only
 *										
 *
 *  DESCRIPTION:	
 *		The channel is checked to see if a memory card is connected to it
 *		and if so what the cards status is, eg formatted, unformatted.
 *
 *  RETURNS:
 *		CARD_FORMATTED		A memory card is connected and is formatted
 *		CARD_UNFORMATTED	A memory card is connected but is unformatted
 *		CARD_BAD			A memory card is connected but has errors
 *		CARD_NONE			No memory card is connected to the channel
 */

int GetCardStatus (long channel) 
{
  int 			ret ;
  char 			buffer[128] ;

  ClearCardEventsSw () ;
  _card_info (channel) ;
  ret = GetCardEventSw () ;

  switch (ret)
  {
    case EVENT_IOE:
	{
/*	  if (CheckIfCardFormatted (channel))*/
	  {
	    return CARD_FORMATTED ;
	  }
/*	  else
	  {
	    return CARD_UNFORMATTED ;
	  }*/
	} break ;
	case EVENT_ERROR:
	{
	  return CARD_BAD ;
	} break ;
	case EVENT_TIMEOUT:
	{
	  return CARD_NONE ;
	} break ;
  }

  /* The card is newly inserted */
  ClearCardEventsHw () ;
  _card_clear (channel) ;
  ret = GetCardEventHw () ;

  ClearCardEventsSw () ;
  _card_load (channel) ;
  ret = GetCardEventSw () ;

  switch (ret) 
  {
    case EVENT_IOE:
	{
	  return CARD_FORMATTED ;
	} break ;
	case EVENT_ERROR:
	{
	  return CARD_BAD ;
	} break ;
	case EVENT_TIMEOUT:
	{
	  return CARD_NONE ;
	} break ;
	case EVENT_NEWCARD:
	{ 
	  return CARD_UNFORMATTED ;
	} break ;
  }
  return -1 ;
}

/*
 ** GetCardEventSw
 *
 *  PARAMETERS:		None
 *
 *  DESCRIPTION:
 *		The event queue is checked for a required Sw message
 *
 *  RETURNS:
 *		EVENT_IOE		The previous task completed successfully
 *		EVENT_ERROR		An error occurred during the previous task
 *		EVENT_TIMEOUT	A timeout occurred during the previous task
 *		EVENT_NEWCARD	A new card was detected during the previous task
 */

int GetCardEventSw (void)
{
  while (1) 
  {
  	if (TestEvent (ev0) == 1) 
  	{         /* IOE */
      return EVENT_IOE ;
    }
    if (TestEvent (ev1) == 1) 
    {         /* ERROR */
      return EVENT_ERROR ;
    }
    if (TestEvent (ev2) == 1) 
    {         /* TIMEOUT */
      return EVENT_TIMEOUT ;
    }
    if (TestEvent (ev3) == 1) 
    {         /* NEW CARD */
      return EVENT_NEWCARD ;
    }
  }
}

/*
 ** ClearEventSw
 *
 *  PARAMETERS:			None
 *
 *  DESCRIPTION:
 *		The event queue is cleared any Sw messages it currently contains
 *
 *  RETURNS:			None
 */

int ClearCardEventsSw ()
{
  TestEvent (ev0) ;
  TestEvent (ev1) ;
  TestEvent (ev2) ;
  TestEvent (ev3) ;
}

/*
 ** GetCardEventHw
 *
 *  PARAMETERS:		None
 *
 *  DESCRIPTION:
 *		The event queue is checked for a required Hw message
 *
 *  RETURNS:
 *		EVENT_IOE		The previous task completed successfully
 *		EVENT_ERROR		An error occurred during the previous task
 *		EVENT_TIMEOUT	A timeout occurred during the previous task
 *		EVENT_NEWCARD	A new card was detected during the previous task
 */

int GetCardEventHw ()
{
  while(1) 
  {
    if (TestEvent (ev10) == 1) 
    {         /* IOE */
       return 0;
    }
    if (TestEvent (ev11) == 1) 
    {         /* ERROR */
       return 1;
    }
    if (TestEvent (ev12) == 1) 
    {         /* TIMEOUT */
       return 2;
    }
    if (TestEvent (ev13) == 1) 
    {         /* NEW CARD */
       return 3;
    }
  }
}

/*
 ** ClearEventHw
 *
 *  PARAMETERS:			None
 *
 *  DESCRIPTION:
 *		The event queue is cleared any Hw messages it currently contains
 *
 *  RETURNS:			None
 */

int ClearCardEventsHw ()
{
  TestEvent (ev10) ;
  TestEvent (ev11) ;
  TestEvent (ev12) ;
  TestEvent (ev13) ;
}

/*
 ** CheckIfCardFormatted
 *
 *  PARAMETERS:		
 *		long channel		The channel the memory card is connected to.
 *							Possible values are
 *								0x00	(0x01	0x02	0x03)*
 *								0x10	(0x11	0x12	0x13)*
 *							*Multi-Tap only
 *
 *  DESCRIPTION:
 *		The memory card connected to the specified channel is checked to see
 *		if it is formatted
 *
 *  RETURNS:
 *		0		The card is not formatted
 *		1		The card is formatted
 */

int CheckIfCardFormatted (long channel)
{
	char buffer[128] ;

	bzero (&buffer[0],128) ;
	ClearCardEventsHw () ;
	_new_card () ;
	_card_read (channel, 0, &buffer[0]) ;
	if(GetCardEventHw () != 0)
	{
	  return 0 ;
	}
	if (buffer[0] == 'M' && buffer[1] == 'C')
	{
	  return 1 ;
	}
	return 0 ;
}

/*
 ** FormatCard
 *
 *  PARAMETERS:		
 *		long channel		The channel the memory card is connected to.
 *							Possible values are
 *								0x00	(0x01	0x02	0x03)*
 *								0x10	(0x11	0x12	0x13)*
 *							*Multi-Tap only
 *
 *  DESCRIPTION:
 *		The memory card connected to the specified channel is formatted
 *
 *  RETURNS:
 *		0		The format was not successful
 *		1		The format was successful
 */

int FormatCard (long channel)
{
  int 		ret ;
  char 		drive[16] ;

  sprintf (drive, "bu%.2x:", channel) ;
  return format (drive) ;
}

/*
 ** UnFormatCard
 *
 *  PARAMETERS:
 * 		long channel		The channel the memory card is connected to.
 *							Possible values are
 *								0x00	(0x01	0x02	0x03)*
 *								0x10	(0x11	0x12	0x13)*
 *							*Multi-Tap only
 *
 *  DESCRIPTION:
 *		The card is returned to its unformatted/uninitialized state.  
 *	The process involved also clears all of the data stored on the memory card.
 *
 *  RETURNS:
 *		0		The unformat was not successful
 *		1		The unformat was successful
 */

int UnFormatCard(long channel)
{
  long loop ;
  char buffer[128] ;

  for (loop=0; loop<128; loop++)
  {
    buffer[loop] = 0xff;
  }
  for (loop=0; loop<1024; loop++) 
  {
    ClearCardEventsHw () ;
    _new_card () ;
     _card_write(channel, loop, &buffer[0]) ;
    if (GetCardEventHw () != 0)
    {
     return 0;
	}
  }
  return 1;
}

/*
 ** QuickUnFormatCard
 *
 *  PARAMETERS:
 * 		long channel		The channel the memory card is connected to.
 *							Possible values are
 *								0x00	(0x01	0x02	0x03)*
 *								0x10	(0x11	0x12	0x13)*
 *							*Multi-Tap only
 *
 *  DESCRIPTION:
 *		The card is returned to its unformatted/uninitialized state.
 *	The process involved does not clear all the data on the memory card,
 *	only enough to make the system think it's formatted.  This speeds up
 *	the process of formatting.
 *
 *  RETURNS:
 *		0		The unformat was not successful
 *		1		The unformat was successful
 */

int QuickUnFormatCard (long channel)
{
	char buffer[128];

	buffer[0] = buffer[1] = 0xff ;
	ClearCardEventsHw () ;
	_new_card () ;
	_card_write(channel, 0, &buffer[0]) ;
	if (GetCardEventHw () != 0)
	{
		return 0;
	}
	return 1;
}

/*
 ** GetFileDirectoryFromCard
 *
 *  PARAMETERS:
 * 		long channel		The channel the memory card is connected to.
 *							Possible values are
 *								0x00	(0x01	0x02	0x03)*
 *								0x10	(0x11	0x12	0x13)*
 *							*Multi-Tap only
 *		struct DIRENTRY *d	A pointer to the start of the DIRENTRY array.
 *
 *  DESCRIPTION:
 *		The details of each file on the memory card are added to the array.
 *
 *  RETURNS:
 *		The number of files on the memory card
 */

int GetFileDirectoryFromCard (long channel, struct DIRENTRY *d)
{
  char 			drive[128] ;

  extern struct DIRENTRY *firstfile(), *nextfile() ;

  sprintf (drive, "bu%.2x:*", channel) ;

  fileCount = 0 ;
  if (firstfile (drive, d) == d) 
  {
    do 
    {
      fileCount++ ;
	  d++ ;
    } while ((nextfile (d) == d) && (fileCount < 10)) ;
  }
  return fileCount ;
}

void DisplayFileDirectory (struct DIRENTRY *d, int fileCount) 
{
  int			loop ;
  FILE_HEADER	fileHeader ;

  FntPrint ("There are %d files on the memory card\n", fileCount) ;
  for (loop=0; loop<fileCount; loop++)
  {
    FntPrint ("%s  %d  %d\n", d->name, d->size, d->head) ;
	GetFileHeaderFromCard (0x00, d->name, &fileHeader) ;
	DisplayFileHeader (&fileHeader) ;
	d++ ;
  }
}

int GetFileHeaderFromCard (long channel, char *fileName, FILE_HEADER *fileHeader)
{
  int			file ;
  char			path[64] ;
  int			count ;

  sprintf (path, "bu%.2x:%s", channel, fileName) ;
  file = open (path, O_RDONLY) ;
  if (file < 0)
  {
    printf ("ERROR: The file could not be opened\n") ;
	return FALSE ;
  }

  count = read (file, fileHeader, sizeof(FILE_HEADER)) ;
  if (count != sizeof (FILE_HEADER))
  {
    printf ("ERROR: During reading   Read: [%d]\n", count) ;
	close (file) ;
    return FALSE ;
  }
  					
  close (file) ;
  return TRUE ;
}

int SetFileHeaderInCard (long channel, char *fileName, FILE_HEADER *fileHeader) 
{
  int			file ;
  char			path[64] ;
  int 			count ;

  sprintf (path, "bu%.2x:%s", channel, fileName) ;
  file = open (path, O_WRONLY) ;
  if (file < 0)
  {
    FntPrint ("ERROR: Could not open file\n") ;
    return FALSE ;
  }
  count = write (file, fileHeader, sizeof (FILE_HEADER)) ;
  if (count < sizeof(FILE_HEADER))
  {
    FntPrint ("ERROR: during writing\n") ;
	close (file) ;
    return FALSE ;
  }
  close (file) ;
  return TRUE ;
}

void GetCardData (long channel, char *cardBuffer)
{
  int		loop ;

  for (loop=0; loop<64*2; loop++)  // Only get first two files data (slow op)
  {
	ReadBlockFromCard (channel, loop, cardBuffer + (loop * 128)) ;
  }
}

void DisplayFileHeader (FILE_HEADER *fileHeader) 
{
  FntPrint ("Magic: [%c%c]   Type: [%d]\n", fileHeader->magic[0], fileHeader->magic[1], fileHeader->type) ;
}

int FileExistsOnCard (long channel, char *fileName)
{
  int 		file ;
  char 		path[64] ;

  sprintf (path, "bu%.2x:%s", channel, fileName) ;
  file = open (path, O_WRONLY) ;
  if (file >= 0)
  {
    close (file) ;
    return 1 ;
  }
  close (file) ;
  return 0 ;
}

int DeleteFileFromCard (long channel, char *fileName) 
{
  char		path[64] ;

  sprintf (path, "bu%.2x:%s", channel, fileName) ;
  return delete (path) ;
}

int WriteFileBlocking (long channel, char *fileName, int blocks, char *buffer)
{
  int 		file ;
  char		path[64] ;
  int		count ;

  sprintf (path, "bu%.2x:%s", channel, fileName) ;
  file = open (path, O_WRONLY) ;
  if (file >= 0)
  {
    FntPrint ("ERROR: The file already exists!\n") ;
	close (file) ;
	return 0 ;
  }
  file = open (path, O_CREAT|(blocks<<16)) ;
  if (file == -1)
  {
    FntPrint ("ERROR: Could not create file.\n") ;
	return 0 ;
  }
  /* The file is created OK */
  close (file) ;
  file = open (path, O_WRONLY) ;
  if (file >= 0)
  {
    printf ("before write\n") ;
    count = write (file, buffer, (blocks*8192)) ;
	printf ("after write\n") ;
	if (count != (blocks*8192))
	{
	  FntPrint ("ERROR: Failed when writing data!\n") ;
	  close (file) ;
	  return 0 ;
	}
	close (file) ;
  }
  else
  {
    FntPrint ("ERROR: Could not open file\n") ;
	return 0 ;
  }
  return count ;
}

int ReadFileBlocking (long channel, char *fileName, int blocks, char *buffer) 
{
  int 			ret ;
  int 			file ;
  char			path[64] ;
  int 			count ;

  sprintf (path, "bu%.2x:%s", channel, fileName) ;
  file = open (path, O_RDONLY) ;
  if (file >=0)
  {
    ret = read (file, buffer, (blocks*8192)) ;
	if (ret != blocks * 8192)
	{
	  printf ("ERROR: Failed when reading data!\n") ;
	  close (file) ;
	  return 0 ;
	}
	close (file) ;
  }
  else
  {
    printf ("ERROR: Could not open file\n") ;
	close (file) ;
	return 0 ;
  }
  close (file) ;
  printf ("File Read OK\n") ;
  return ret ;
}

int CheckBlockSpacesAvailable (long channel)
{
  int			loop,
				count = 0 ;

  for (loop=0; loop<fileCount; loop++)
  {
    count += fileHeader[loop].blockEntry ;
  }

  return 15 - count ;
}

int ReadBlockFromCard (long channel, int blockNo, char *buffer) 
{
  int 			ret ;

  ClearCardEventsHw () ;
  _card_read (channel, blockNo, buffer) ;
  ret = GetCardEventHw () ;

  if (ret == EVENT_IOE)
  {
    return BLOCK_READ_OK;
  }	

  return BLOCK_READ_ERROR ;
}

int WriteBlockToCard (long channel, int blockNo, char *buffer) 
{
  int			ret ;
  
  ClearCardEventsHw () ;
  _card_write (channel, blockNo, buffer) ;
  ret = GetCardEventHw () ;

  if (ret == EVENT_IOE)
  {
	return BLOCK_WRITE_OK ;
  }

  return BLOCK_WRITE_ERROR ;
}	


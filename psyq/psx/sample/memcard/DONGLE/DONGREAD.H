/* PCFS Dongle Code Header */

#ifndef DONGLE_READ_H

 #include "dongle.h"

 typedef struct
 {
   char titleName[21] ;
   char personName[21] ;
   char companyName[21] ;
   char date[9] ;
   char key[3] ;
 }  DONGLE_FILE_INFO ;
 
 int ReadDongleInfoFromFile (char *pcFileName) ;
 int GetDongleFileInfoCount (void) ;
 int GetDongleInfoFromStringArray (DONGLE_FILE_INFO *dongleInfo, char *buffer, int index) ;
 int GetLineFromStringArray (char *source, char *dest, int lineNo) ;
 int GetDongleInfo (DONGLE_INFO *dongleInfo, int *key, int index) ;
 
 extern char 		dongleBuffer [64000] ;

#define DONGLE_READ_H
#endif
/* DONGLE Header File */

#ifndef DONGLE_H

 #define DONGLE_FOUND			0
 #define DONGLE_NOT_FOUND		1
 #define DONGLE_CARD_NOT_FOUND	2
 
 #define DONGLE_CREATED			0
 #define DONGLE_CARD_FULL		1
 #define DONGLE_WRITE_ERROR		2
 
 #define DONGLE_ID_OFFSET		100
 #define TITLE_NAME_OFFSET		200
 #define COMPANY_NAME_OFFSET	300
 #define PERSON_NAME_OFFSET		400
 #define DATE_OFFSET			500
 
 typedef struct 
 {
   char 		titleName[21] ;
   char 		companyName[21] ;
   char			personName[21] ;
   char			date[9] ;
 } DONGLE_INFO ;
 
 int CreateDongle (long channel, DONGLE_INFO *pDongleInfo, int key) ;
 int GetDongleFileInfoFromCard (long channel) ;
 int CheckForDongle (long channel, DONGLE_INFO *pDongleInfo, int key) ;
 void EncryptDongleInfo (DONGLE_INFO *pDongleInfo, char *buffer, int key) ;
 int DecryptDongleInfo (DONGLE_INFO *pDongleInfo, char *buffer, int key) ;
 

 extern DONGLE_INFO		dongleFileInfo ;

#define DONGLE_H
#endif
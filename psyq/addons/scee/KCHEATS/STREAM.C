/*

	Streaming Code


	CJH @ SCEE March 1996

	Version 1.0			Simple double buffered streaming... No frills.


*/


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

#include "main.h"
#include "ctrller.h"


// Movie data...

#define RGB24

#define WIDTH  144
#define HEIGHT 112

#define SECTORS 6 // 150 sectors per sec  25fps = 6 sectors per frame


#define LASTFRAME 241					//	in theory 25 * dur (10 secs)

#define SCREENSIZE (WIDTH*HEIGHT*2/4)		//	Screen buffer size in longs for 16bit pixels
#define MWIDTH WIDTH						      //	Screen width in 16bit pixels..
#define SLICE 16						         //	MDEC slice width in 16bit pixels



//	Various double buffers
unsigned long buffer[2][SCREENSIZE];	//	Screen buffers
unsigned long run[2][16384];			   //	Run length frames 


//	Ringbuffer...(1 sector is 512 longs)
unsigned long ringbuffer[512*(2*SECTORS+1)];	// 2 CD buffers plus spare sector



extern ControllerPacket buffer1, buffer2;

extern DB	  db[2];		   /* packet double buffer */
extern DB*    cdb;			/* current db */

extern FILEDATA filedata[]; 


                     
                     

/**-------------------------------------------------------------------------**/
/**- anim(int file_no) -----------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/*
anim(int file_no,int frames)

int file_no:      the file look up 
int frames        the number of the frame to stop on -1
returns 1 on exit 

*/


anim(int file_no,int frames)
{
CdlFILE file;
int Sync;
unsigned long *nextstream;
StHEADER *header;
unsigned char result[8];
int i;

RECT frame = { 0,0,16,HEIGHT };

DecDCTReset(0);

VSync(0);


//db[1].disp.screen.x = db[0].disp.screen.x = SCREEN_X;
//db[1].disp.screen.y = db[0].disp.screen.y = SCREEN_Y;

//db[1].disp.screen.h = db[0].disp.screen.h = PAL_MAGIC;
//db[1].disp.screen.w = db[0].disp.screen.w = PAL_MAGIC;


//db[0].draw.dfe = db[1].draw.dfe = 1;


//cdb = &db[1];

PutDispEnv(&db[1].disp); 		
PutDrawEnv(&db[0].draw); 		

do{
   do{
//    printf("file %s \n",filedata[file_no].fp.name);
      while ( CdControl(CdlSeekL,(char *)&filedata[file_no].fp.pos,result)==0 );
//    printf( "CD started ok...\n" );
      } 
   while ( CdRead2( CdlModeStream|CdlModeSpeed|CdlModeRT )==0);

   Sync = 0;
   StSetRing( ringbuffer,2*SECTORS+1 );      //	Room for two frames at 15fps
   StSetStream( 0,0,0xffffffff,0,0 );			//	Start streaming with no call backs active 

//   printf( "Streaming started...\n");

   //	Pre-read first frame...

	while ( StGetNext( &nextstream,(unsigned long **)&header ) );		//	Wait for first frame...
	
   //	Show stream header info....
	//printf( "Movie is %d by %d\n",header->width,header->height );
	//printf( "Sectors per frame %d\n",header->nSectors );
   //	Main Play loop....

   while (header->frameCount < frames)
	   {
      //printf("*");
	   DecDCTvlc( nextstream,run[Sync] );		//	Decode VLC from stream to run length buffer
	   StFreeRing( nextstream );					//	Finished with VLC data
      while ( StGetNext( &nextstream,(unsigned long **)&header ) );	//	Wait for valid VLC frame from CD
		DecDCTin( run[Sync],0 );					   //	Decode valid run length (16 bit mode)
		DecDCTout( buffer[Sync],SCREENSIZE );		//	to other buffer
		VSync(0); 								//	Wait for Sync...
      frame.y = db[0].draw.clip.y+13;
		Sync ^= 1;									//	Switch buffers
      if (header->frameCount>4)
         {
		   for (i=0;i<MWIDTH;i+=SLICE)
		      {
			   frame.x = i+163;
			   LoadImage( &frame,buffer[Sync]+(HEIGHT/2)*i );	
	   	   }
         }

      if( GoodData(&buffer1) && GetType(&buffer1)==STD_PAD)
         { 
         if ( PadKeyIsPressed(&buffer1,PAD_RU)!=0 ||
            PadKeyIsPressed(&buffer1,PAD_RD)!=0 ||
            PadKeyIsPressed(&buffer1,PAD_LU)!=0 ||
            PadKeyIsPressed(&buffer1,PAD_LD)!=0 )
               {
	            StClearRing();    //	Close down streaming system
	            StUnSetRing();    //	Close down streaming system
               return 1;
               }
         }
      }
	StUnSetRing();									//	Close down streaming system

}while(1);

}

#include <sys/types.h>
#include <kernel.h>
#include <libsn.h>
#include <libcd.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#include "ddp.h"

#ifdef LINKED_NONE2
int main(int argc, char **argv)
#else
int main()
#endif
{
   int mode, timeout, start_track, num_tracks;

   ResetCallback();
   CdInit();

	PadInit(0);		   
	ResetGraph(0);		
	SetGraphDebug(0);	
	SetDispMask(1);

   printf("\n\nHarness: Go.\n");

   #ifdef LINKED_NONE2
     printf("Linked None2.\n");
     mode = ((int *)argv)[0];
     timeout = ((int *)argv)[1];
     start_track = ((int *)argv)[2];
     num_tracks = ((int *)argv)[3];

     printf("Hello, I am a playable game section.\n");
     printf("I am running in %s mode, with a timeout of %d seconds.\n",
            (mode == INTERACTIVE)?"interactive":"attract",timeout);

     if ( num_tracks > 0 )
       printf("For your entertainment, I will be using %d CD tracks starting at track %d.\n",
       			num_tracks,
       			start_track);
     else
       printf("I don't play any CD tracks, I bravely use the SPU or XA.\n");

   #else
     printf("Harness: Not linked None2.\n");
   #endif

   while (!PadRead(0));

	StopCallback();
	PadStop();
	ResetGraph(3);
	return(0);
}

/*
** Changes Made:
** -------------
** 28-sep-95 (pholman)
**      Add information about quiting the game, if required
** 09-oct-95 (pholman)
**          Random Advert will now appear, move DISCLAIM to PIRATE screen
**
** 23-Nov-95 (Vince)
**			Switched off the BadKeys mode for the future demo in GamePrevSel().
**			For Future coverdisc demos only.
**
** TO DO: 29-Nov-95 (Vince)
**			Must ensure SetupLoad() adheres to the new demo standard. The timeout needs to be
**			fixed (check SetupLoad).
**
** TO DO: 10-Jan-96 (Vince)
**			Commented out all code in VideoSel and MoviePlayer. These functions should be removed.
**			Also remove their entries in fsm.c, config.c, states.c, dd_conf.txt etc. 
*/

#include <sys/types.h>
#include <kernel.h>
#include <libsn.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
//#include <libcd.h>
#include "g:\program\psx\include.32\libcd.h"
#include <libsnd.h>

#include "ctrller.h"
#include "ddp.h"
#include "dd.h"

#include "fsm.h"
#include "states.h"
#include "config.h"

#include "snd.h"

#include "icons.h"
#include "plugins.h"
#include "loops.h"

# include <rand.h>

void SetupLoad(char* name, int track, int numTracks, int timeout)
  {
      CdlFILE file;

      CdCheck(name);

      if (!CdSearchFile(&file, name))
	{
	  printf("DD: Load for %s failed.\n",name);
	}
     else
       {
	  dropOut = 1;

	  data->pos.track  = file.pos.track;
	  data->pos.minute = file.pos.minute;
	  data->pos.second = file.pos.second;
	  data->pos.sector = file.pos.sector;

	  data->loadGame = 1;

	  data->args[0] = data->mode;

      /* TEMP -- The TIMEOUT setup code does NOT adhere to the new standard Therefore it
       * Must be changed.
       */	  
	  if ( data->mode == INTERACTIVE )
	    data->args[1] = 0x7fffff;
	  else
	    data->args[1] = timeout;

      /* Initialise the start track index and number of tracks.
       * If we are playing a stream this is the X and Y position. Hack?!?!
       */
      data->args[2] = track;
      data->args[3] = numTracks;	    
       }
  }
  
int GameSection(char *newstate, char state)
  {
    EndSequence();

    VSyncCallback(NULL); /* Kill the boot timeout. */
    

    if (GetDataNum(GAME_QUIT,data->selected))
	{
	    int ret;
		int wait = 0;

#ifdef DEBUGx       
	printf("%d - %s\n",GetDataNum(GAME_QUIT,data->selected),
			GetDataStr(QUITINFO,0));
#endif

	    ret = LoopSel(GetDataStr(QUITINFO,0),
		      NULL_INDEX, NULL, NULL, NULL);

	    while (wait++ < 75)
		    VSync(0);

	}

    SetupLoad(GetDataStr(GAME_EXE,data->selected),
	      GetDataNum(GAME_TRACK,data->selected),
	      GetDataNum(GAME_TRACKS,data->selected),
	      GetTimeout(data->state));

    data->boottype = GetDataNum(GAME_LOAD,data->selected);
    
# ifdef DEBUG
    printf("Ready to run \"%s\"\n",GetDataStr(GAME_EXE,data->selected));
#endif

    if ( data->boottype == TYPE_SCEI )
      {
	data->name[0] = '\0';
	strcpy(data->name,CD_DEV_NAME);
	strcat(data->name,GetDataStr(GAME_EXE,data->selected));
      }
      
    data->mode = INTERACTIVE;
    data->args[0] = INTERACTIVE;

    return TIMEOUT;
  }


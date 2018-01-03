#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libetc.h>
#include <libcd.h>
#include <libsn.h>
#include <libsnd.h>

			 
#include "main.h"
#include "sound.h"


extern short vab;	/* vab data id */
extern short seq;	/* seq data id */
extern char seq_table [SS_SEQ_TABSIZ * 1 * 1];  /* seq table size */


extern unsigned long icontim[];
extern unsigned long vabheader[];
extern unsigned long vabbody[];
extern unsigned long seqdata[];



/**----------------------------------------------------------------------**/
/** init the sound system                                                **/
/**----------------------------------------------------------------------**/
  
init_sound()
{
SsInit();			                  /* reset sound */
SsSetTableSize (seq_table, 1, 1);
SsSetTickMode(SS_TICK60);	         /* set tick mode = 1/240s */

/* load sound stuff */

printf("vad header = 0x%x \n",vabheader);
vab = SsVabOpenHead ((u_char *)vabheader, -1);
if (vab < 0) 
	{
  	printf ("SsVabOepnHead : failed!\n");
  	return;
   }

if (SsVabTransBody ((u_char*)vabbody, vab) != vab) 
	{
  	printf ("SsVabTransBody : failed!\n");
  	return;
	}

SsVabTransCompleted (SS_WAIT_COMPLETED);
seq = SsSeqOpen((u_long *)seqdata, vab);	/* open seq data */

SsSetMVol(MVOL, MVOL);			/* set main volume */
SsSeqSetVol(seq, SVOL, SVOL);	/* set seq data volume */

/* SsStart();			/* start sound */
/* SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY); */

}
/**----------------------------------------------------------------------**/



/**----------------------------------------------------------------------**/
/** turn the lovely music on                                             **/
/**----------------------------------------------------------------------**/
 
music_on()
{
SsStart();			/* start sound */
SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY); 
}
/**----------------------------------------------------------------------**/


/**----------------------------------------------------------------------**/
/** turn the lovely music off                                            **/
/**----------------------------------------------------------------------**/
music_off()
{
SsEnd();			/* stop sound */
}
/**----------------------------------------------------------------------**/




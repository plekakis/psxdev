/* $PSLibId: Run-time Library Release 4.4$ */
/*
 * sound sample
 */
#include <sys/types.h>
#include <sys/file.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>	
#include <libsnd.h>
#include <libspu.h>

#define MVOL		127			/* main volume */
#define SVOL		127			/* seq data volume */

static short vab,seq;
static char seq_table [SS_SEQ_TABSIZ * 1 * 1]; /* SOUND internal table */

int
snd_set(seq_a,vab_h,vab_b)
u_long	*seq_a;
u_char	*vab_h;
u_char	*vab_b;
{
    int	i;

    SsSetMVol (0,0);	/* set main volume */
    SsInit();			/* SOUND Init */
    SsSetTableSize (seq_table,	/* SOUND internal table */
		    1, 1);

    SsUtSetReverbType (SS_REV_TYPE_STUDIO_C); /* set ReverbType */
    SpuClearReverbWorkArea(SS_REV_TYPE_STUDIO_C); /* clear reverb work area */
    SsSetTickMode (SS_TICK60);	/* set tick mode = 1/60 */

    /*	.vh open    */
    vab = SsVabOpenHead ((u_char *)vab_h, -1);
    if (vab == -1) {
	printf ("SsVabOpenHead : Can NOT open header.\n");
	return;
    }

    /*
     *	.vb open
     */
    if (SsVabTransBody (vab_b, vab) != vab) {
	printf ("SsVabTransBody : failed !!!\n");
	return;
    }
    SsVabTransCompleted (SS_WAIT_COMPLETED);

    /*
     *	.seq open
     */
    seq = SsSeqOpen (seq_a, vab);

}

int
snd_play(seq_a,vab_h,vab_b)
u_long	*seq_a;
u_char	*vab_h;
u_char	*vab_b;
{
    int	i;

    /*
     *	start SOUND
     */
    SsStart2 ();

    SsSetMVol (MVOL, MVOL);	/* set main volume */
    SsSeqSetVol (seq,		/* set seq data volume */
		 SVOL, SVOL);

    SsUtReverbOn ();			/* Reverb On */
    SsUtSetReverbDepth (80, 80);	/* Reverb Depth */

    /*	Play    */
/*  SsSetMVol (0, 0);	/* set main volume */
/*  SsSetMVol (MVOL, MVOL);	/* set main volume */
    SsSeqPlay (seq, SSPLAY_PLAY, SSPLAY_INFINITY);

/*
	for(i=0;i<30;i++){	
		VSync(0);
	}
*/
}

int
snd_stop()
{
	VSync (0);
    SsSeqStop (seq);
	SsSeqClose (seq);		/* .seq close */
    return;
}

int
snd_replay(seq_a)
u_long	*seq_a;
{
    seq = SsSeqOpen (seq_a, vab);
    SsSetMVol (MVOL, MVOL);	/* set main volume */
    SsSeqSetVol (seq,		/* set seq data volume */
		 SVOL, SVOL);
    SsSeqPlay (seq, SSPLAY_PLAY, SSPLAY_INFINITY);
    return;
}

int
snd_end()
{
	short	i;

    SsSeqStop (seq);/**/

	SsUtAllKeyOff(0);
	SsSeqCalledTbyT();

   SsSeqClose (seq);		/* .seq close */
   SsVabClose (vab);		/* .vh, .vb close */

    SsEnd ();			/* sound system end */
/*	SpuInit ();			/********************/

/*	SpuSetKey( SpuOff , 0xffffffff);/**/

    return;
}


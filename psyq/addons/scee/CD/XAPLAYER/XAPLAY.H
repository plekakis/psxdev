/******************************************************************************
XAplay.h

	CD-ROM interface routines

	multi stream XA test
	by Buzz Burrowes
	Sony Interactive Studios America


******************************************************************************/
#ifndef _XAPLAY_H
#define _XAPLAY_H

/* Prototypes... */
void PrepareXA(void);
int PlayXA(int startp, int endp, int index, int addon);
int PlayingXA(void);
void UnprepareXA(void);
#endif

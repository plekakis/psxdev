/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _CONST_H_
#define _CONST_H_

/*****************************************************************
 *
 * file: const.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#define _COW0           0x801a8800L
#define _COW1           _COW0+0x2000
#define _COW2           _COW1+0x2000
#define _COW3           _COW2+0x2000
#define _COW4           _COW3+0x2000
#define _COW5           _COW4+0x2000
#define _COW6           _COW5+0x2000         /* +0x2000 */
#define _SPH0           0x801b6800L
#define _SPH1           _SPH0+0x8000
#define _SPH2           _SPH1+0x2000
#define _SPH3           _SPH2+0x2000
#define _SPH4           _SPH3+0x2000
#define _SPH5           _SPH4+0x2000
#define _SPH6           _SPH5+0x2000
#define _SPH7           _SPH6+0x4000
#define _SPH8           _SPH7+0x200          /* +0x2000 */

#define	OTSIZE		(1<<10)		        /* ordering table size */

#define PIH             640
#define PIV             480
#define OFX             (PIH/2)                 /* screen offset X */
#define OFY             (PIV/2)                 /* screen offset Y */
#define BGR             0			/* BG color R */
#define BGG             0			/* BG color G */
#define BGB             32			/* BG color B */

#define RBK             0                       /* Back color R */
#define GBK             0                       /* Back color G */
#define BBK             0                       /* Back color B */
#define RFC             128			/* Far color R */
#define GFC             128			/* Far color G */
#define BFC             255			/* Far color B */

#define DELTA    48
#define LINEW    2

/* Number Buttons Size */
#define LARGE    60 
#define NORMAL   48
#define BUTTON   40
#define SMALL    8
#define DOT      8

/* Video Mode */
#define NTSC_MODE  0
#define PAL_MODE   1

#define PALOFFSETX 3           /* v offset for PAL mode 2+1 */
#define PALOFFSETY 27          /* h offset for PAL mode (312-262)/2+3 */

#define ENVOFFSETY 2           /* hight offset for Disp/Draw Env */

#define SCE             1      /* Initialize for sce logo */
#define PS              0      /* Initialize for ps  logo */

#define REVERB_OFF      0      /* set for reverb */
#define REVERB_ROOM     1
#define REVERB_STUDIO_A 2
#define REVERB_STUDIO_B 3
#define REVERB_STUDIO_C 4
#define REVERB_HALL     5
#define REVERB_SPACE    6

#endif /* _CONST_H_ */
/* DON'T ADD STUFF AFTER THIS */

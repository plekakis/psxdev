/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


/*#define DEBUG	/**/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libhmd.h>

/*
 * Image with clut (GsUIMG1)
 */
u_long *GsU_02000001(GsARGUNIT *sp)
{
	GsARGUNIT_IMAGE		*ap = (GsARGUNIT_IMAGE *)sp;
	int			num;
	int			i;

	num = *(ap->primp)>>16;
	ap->primp++;

	for (i = 0; i < num; i++) {
#ifdef DEBUG
		RECT			*rect;

		rect = (RECT *)ap->primp;
		printf("PIX(%d,%d,%d,%d)\n",rect->x,rect->y,rect->w,rect->h);
#endif /* DEBUG */
		/* Load Image Data */
		LoadImage((RECT *)ap->primp, 
			(u_long *)(ap->imagetop+(*(ap->primp+2))));
		ap->primp += 3;

#ifdef DEBUG
		rect = (RECT *)ap->primp;
		printf("CLT(%d,%d,%d,%d)\n",rect->x,rect->y,rect->w,rect->h);
#endif /* DEBUG */
		/* Load Clut Data */
		LoadImage((RECT *)ap->primp, 
			(u_long *)(ap->cluttop+(*(ap->primp+2))));
		ap->primp += 3;
	}

	return(ap->primp);
}

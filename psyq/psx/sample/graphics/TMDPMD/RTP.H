/* $PSLibId: Run-time Library Release 4.4$ */
/*			rtp.h
 *			
 *		   Macro RotTransPers4
 *
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Sep,29,1995	suzu
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>

/*
 * main section of ROTTransPers()
 * no interleaving of the GTE and CPU is performed here
 * source code is embedded as macros, so be conscious of code size */
#define rotTransPers3(ot, otsize, p, v0, v1, v2) {			\
	long	otz, flg, clp;						\
	gte_ldv3(v0, v1, v2);						\
	gte_rtpt();							\
	gte_stflg(&flg);						\
	gte_nclip();							\
	if(flg>0) {							\
		gte_stopz(&clp);					\
		gte_avsz3();						\
		if (clp > 0) {						\
			gte_stotz(&otz);				\
			if (otz>0&&otz<otsize) {			\
				gte_stsxy3((long *)&p->x0,		\
					   (long *)&p->x1, 		\
				           (long *)&p->x2);		\
				addPrim(ot+otz, p);			\
			}						\
		}							\
	}								\
}									

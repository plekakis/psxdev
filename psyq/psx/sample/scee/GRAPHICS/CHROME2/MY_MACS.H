/*		
 *		   Macros for my stuff
 *
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <gtemac.h>

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
			if (otz<otsize) {				\
				gte_stsxy3((long *)&p->x0,		\
					   (long *)&p->x1, 		\
				           (long *)&p->x2);		\
				addPrim(ot+otz, p);			\
			}						\
		}							\
	}								\
}									


#define	dot_product(v1, v2) ((v1)->vx * (v2)->vx + (v1)->vy * (v2)->vy + (v1)->vz * (v2)->vz)

#define	subVector(v1, v2, result)   (result)->vx = (v1)->vx - (v2)->vx, \
				    (result)->vy = (v1)->vy - (v2)->vy, \
				    (result)->vz = (v1)->vz - (v2)->vz

#define shift_vec(vec, num)		(vec)->vx = (vec)->vx >> num, 	\
					(vec)->vy = (vec)->vy >> num, 	\
					(vec)->vz = (vec)->vz >> num



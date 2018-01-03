/***********************************************************************/
/***********************************************************************/
/* tmd.c */
/***********************************************************************/
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
#include <gtemac.h>	   
#include <inline_c.h>	   

#include "main.h"	     	
#include "tmd.h"	     	
#include "ctrller.h"	     	

extern int frame;
extern DB	db[2];			/* packet double buffer */
extern DB* 	cdb;			/* current db */

/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
// this function only stores the first vertex color for each polygon. The 
// assumption is that the 3 initial vertex colors are the same and the 
// gouraud effect is achieved by lighting
int loadTMD_G3(u_long *tmd, OBJ_G3 *obj,int semi)    
{
TMD_PRIM	tmdprim;
int			i			=0;
int			n_prim		=0;
VERT_G3		*vert		= obj->vert;
CVECTOR     *colour		= obj->colour;

if ((n_prim = OpenTMD(tmd, 0)) > MAX_G3_TMD_POLY)
	{		
   	n_prim = MAX_G3_TMD_POLY;
	printf("warning, exceeded MAX_G3_TMD_POLY\n");
   }
printf("nprim = %d\n",n_prim);

for (i=0; i<n_prim && ReadTMD(&tmdprim)!=0; i++) 
	{
	copyVector(&vert->n0, &tmdprim.n0);
	copyVector(&vert->n1, &tmdprim.n1);
	copyVector(&vert->n2, &tmdprim.n2);

	copyVector(&vert->v0, &tmdprim.x0);
	copyVector(&vert->v1, &tmdprim.x1);
	copyVector(&vert->v2, &tmdprim.x2);

	colour->r = tmdprim.r0;
	colour->g = tmdprim.g0;
	colour->b = tmdprim.b0;
	vert++,colour++;
	}
return(obj->n = i);
}

/**-------------------------------------------------------------------------**/


/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
/**-------------------------------------------------------------------------**/
draw_object_g3(OBJ_G3* obj,unsigned long** poly_p,int frame)
{
int i;
long outer_product,otz;
POLY_G3* g3_p=(POLY_G3*)*poly_p;

for(i=0; i<obj->n; i++)
	{
	gte_ldv3c(&obj->vert[i].v0); //contiguous vertex data 
	gte_rtpt();
	gte_nclip();		
	gte_stopz(&outer_product);		
	if (outer_product>0) 
		{
		gte_stszotz(&otz); 
  		if (otz>0 && otz< OTSIZE) 
			{
			gte_stsxy3((long *)&g3_p->x0,(long *)&g3_p->x1,(long *)&g3_p->x2);	
			gte_ldv3c((SVECTOR *)&obj->vert[i].n0); //contiguous vertex data 		
			gte_ldrgb((CVECTOR *)&obj->colour[i].r);	  
			gte_ncct();	
			gte_strgb3((CVECTOR *)&g3_p->r0,(CVECTOR *)&g3_p->r1,(CVECTOR *)&g3_p->r2);
			setPolyG3(g3_p);
			addPrim(db[frame].ot+otz,g3_p);   
			g3_p++;
			}//end if on ordering table
		}//end if nclip good

   	}
*poly_p=(long*)g3_p;

}
/**-------------------------------------------------------------------------**/

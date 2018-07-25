/***********************************************************************/
/***********************************************************************/
/* tmd.h */
#ifndef TMD_HEADER
#define TMD_HEADER

/******** POLY_G3 ***********************************************/

#define MAX_G3_TMD_POLY 900

typedef struct 
	{
	SVECTOR	n0, n1, n2; 
	SVECTOR	v0, v1, v2;	
	}VERT_G3;

typedef struct 
	{
	int n;	 	        				/* primitive number */
   	SVECTOR	l2w_ang;	  				/* local 2 world rotation angle        */
   	VECTOR	l2w_vec;	  				/* local 2 world translation vector    */
	VERT_G3 vert[MAX_G3_TMD_POLY];	  	/* vertex       */
	CVECTOR colour[MAX_G3_TMD_POLY];  	/* colour       */
   	}OBJ_G3;


#endif

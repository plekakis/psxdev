/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	MIMe Animation
 *
 *
 *	"nmime.c" ******** routine
 *
 *		Version 1.**	Mar,  14, 1994
 *
 *		Version ??	Aug/3/1994 S. Aoki
 *			 multi TMD handling
 *
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "nmime.h"

/*#define VERBOSE*/
long mimepr[MIMEMODELMAX][MIMEMAX];

/* Object information of TMD file */
typedef struct {
    SVECTOR *vtxtop;	/* Start address of vertex information */
    u_long vtxtotal;	/* Total number of vertex information */
    SVECTOR *nrmtop;	/* Start address of normal information */
    u_long nrmtotal;	/* Total number of normal information */
    u_long *prmtop;		/* Start address of primitive (no use) */
    u_long prmtotal;	/* Total number of primitive (no use) */
    u_long scale;		/* scale of object (no use) */
} TMDOBJECT;

/* Object information of MDF file (differential data file) */
typedef struct {
    SVECTOR		*top;	/* Start address of differential vector */
    u_long		offset;	/* offset of differential vector */
    u_long		total;	/* total number of differential vector */
    u_long		object;	/* object number */
} MDFOBJECT;

/* Structure of TMD information for MIMe operation */
typedef struct {
    u_long 		objtotal;	/* Total number of objects in TMD */
    SVECTOR		*orgvtx;	/* Address of original vertex buffer */
    SVECTOR		*orgnrm;	/* Address of original normal buffer */
    TMDOBJECT 	*tmdobj;	/* Address of TMD information array */
} TMDDATA;

/* Structure of MDF information for MIMe operation  */
typedef struct {
    u_long 		objtotal;	/* Total number of objects in MDF */
    MDFOBJECT	*mdfvct;	/* Address of object information array */
    long		*mime;		/* Address of MIMe Weight (Control) array */
} MDFDATA;

static void set_tmd_info(u_long *model, TMDDATA *tmddt);
static void original_vertex_save(TMDDATA *tmddt, SVECTOR *orgvtxbf);
static void original_normal_save(TMDDATA *tmddt, SVECTOR *orgnrmbf);
static void reset_mime_vertex(TMDDATA *tmddt);
static void reset_mime_norm(TMDDATA *tmddt);
static void set_mime_vertex(TMDDATA *tmddt, MDFDATA *vtxdt);
static void set_mime_norm(TMDDATA *tmddt, MDFDATA *nrmdt);
static void set_mdf_vertex(u_long *mdfdtvtx, MDFDATA *vtxdt);
static void set_mdf_normal(u_long *mdfdtnrm, MDFDATA *nrmdt);


TMDOBJECT tmdobj0[MIMEMODELMAX][OBJMAX]; /* object information array for MIMe  */

MDFOBJECT mdfvtx0[MIMEMODELMAX][MIMEMAX]; /* differential vertex data  */

MDFOBJECT mdfnrm0[MIMEMODELMAX][MIMEMAX]; /* differential normal data */

/* TMD data for MIMe */
TMDDATA tmddt0[MIMEMODELMAX];
/* differential vertex data for MIMe */
MDFDATA vtxdt0[MIMEMODELMAX];
/* differential normal data for MIMe */
MDFDATA nrmdt0[MIMEMODELMAX];

/*==MIME Function==================*/

int init_mime_data(int num, u_long *modeladdr, u_long *mdfdatavtx, u_long *mdfdatanrm, u_long *orgvtxbuf, u_long *orgnrmbuf)
{
    int i;

    tmddt0[num].objtotal=1;
    tmddt0[num].orgvtx=(SVECTOR *)orgvtxbuf;
    tmddt0[num].orgnrm=(SVECTOR *)orgnrmbuf;
    tmddt0[num].tmdobj=tmdobj0[num];
    vtxdt0[num].objtotal=1;
    vtxdt0[num].mdfvct=mdfvtx0[num];
    vtxdt0[num].mime= &mimepr[num][0];
    nrmdt0[num].objtotal=1;
    nrmdt0[num].mdfvct=mdfnrm0[num];
    nrmdt0[num].mime= &mimepr[num][0];

    /* set TMD information data */
    set_tmd_info(modeladdr, &tmddt0[num]);

    if (mdfdatavtx){
	/* reserve original vertex data */
	original_vertex_save(&tmddt0[num], (SVECTOR *)orgvtxbuf);
	/* set differential vertex data */
	set_mdf_vertex(mdfdatavtx,&vtxdt0[num]);
    }
    if (mdfdatanrm){
	/* reserve original normal data */
	original_normal_save(&tmddt0[num], (SVECTOR *)orgnrmbuf);
	/* set differential noraml data */
	set_mdf_normal(mdfdatanrm,&nrmdt0[num]);
    }

    return vtxdt0[num].objtotal;
}

/* MIMe operation (vertex) */
void vertex_mime(int num)
{
    reset_mime_vertex(&tmddt0[num]);	/* retrieve original vertex */
    set_mime_vertex(&tmddt0[num],&vtxdt0[num]); /* MIMe operation (vertex) */

}

/* MIMe operation (normal) */
void normal_mime(int num)
{
    reset_mime_norm(&tmddt0[num]);		/* retrieve original normal */
    set_mime_norm(&tmddt0[num],&nrmdt0[num]); /* MIMe operation (normal) */
}


/* set TMD information data */
static void set_tmd_info(u_long *model, TMDDATA *tmddt)
{
    u_long size,*dop;
    int i,n;

    dop= model;
    dop++;   			/* skip file header */
    dop++;   			/* skip flag */
    n = tmddt->objtotal = *dop;  	/* total number of objects in TMD */

    dop++;
    for(i = 0; i < n; i++){
	tmddt->tmdobj[i].vtxtop = (SVECTOR *)*dop++ ; /* start address of vertex */
	tmddt->tmdobj[i].vtxtotal = *dop++ ;	/* total number of vertex */
	tmddt->tmdobj[i].nrmtop = (SVECTOR *)*dop++ ; /* start address of normap */
	tmddt->tmdobj[i].nrmtotal = *dop++ ;	/* total number of normal */
	tmddt->tmdobj[i].prmtop = (u_long *)*dop++ ; /* start address of primitive */
	tmddt->tmdobj[i].prmtotal = *dop++ ;	/* total number of primitive */
	tmddt->tmdobj[i].scale  = *dop++ ;	/* scale of object */
    }

#ifdef VERBOSE
    printf("objtotal = %d\n", tmddt->objtotal);
    for(i = 0; i < n; i++){
	printf("v_top%d   = %x\n", 	i, 	tmddt->tmdobj[i].vtxtop );
	printf("v_num%d   = %d\n",	i,  	tmddt->tmdobj[i].vtxtotal );
	printf("nrm_top%d = %x\n",	i,	tmddt->tmdobj[i].nrmtop );
	printf("nrm_num%d = %d\n",	i, 	tmddt->tmdobj[i].nrmtotal );
	printf("prm_top%d = %x\n",	i, 	tmddt->tmdobj[i].prmtop );
	printf("prm_num%d = %d\n",	i, 	tmddt->tmdobj[i].prmtotal );
	printf("scale%d   = %d\n",	i, 	tmddt->tmdobj[i].scale );
    }
#endif /* VERBOSE */
}

/* set MDF data for MIMe operation (vertex) */
static void set_mdf_vertex(u_long *mdfdtvtx, MDFDATA *vtxdt)
{
    int i,n;
    u_long *dop2;

    dop2=mdfdtvtx;
    n = vtxdt->objtotal = *dop2++;

    for(i = 0; i < n; i++){
	vtxdt->mdfvct[i].object = *dop2++;	/* object number */
	vtxdt->mdfvct[i].offset = *dop2++;	/* offset of differential vertex */
	vtxdt->mdfvct[i].total 	= *dop2++;	/* total number of differential vertex */
	vtxdt->mdfvct[i].top 	= (SVECTOR *)dop2; /* address of differential information */
	dop2 	+= vtxdt->mdfvct[i].total*2;
    }
}

/* set MDF data for MIMe operation (normal) */
static void set_mdf_normal(u_long *mdfdtnrm, MDFDATA *nrmdt)
{
    int i,n;
    u_long  *dop2;


    dop2= mdfdtnrm;
    n = nrmdt->objtotal = *dop2++;
    for(i = 0; i < n; i++){
	nrmdt->mdfvct[i].object = *dop2++;		/* object number */
	nrmdt->mdfvct[i].offset = *dop2++;		/* offset of differential normal */
	nrmdt->mdfvct[i].total 	= *dop2++;		/* total number of differential normal */
	nrmdt->mdfvct[i].top 	= (SVECTOR *)dop2; 	/* address of differential information */
	dop2 	+= nrmdt->mdfvct[i].total*2;
    }
}


/* reserve original vertex data */
static void original_vertex_save(TMDDATA *tmddt, SVECTOR *orgvtxbf)
{
    SVECTOR *otp,*bsp,*dfp;
    int i,j,n,m;

    bsp= tmddt->orgvtx = orgvtxbf;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
  	otp = tmddt->tmdobj[j].vtxtop;
	bsp += n;
	n = tmddt->tmdobj[j].vtxtotal;
        for( i = 0; i < n; i++) *(bsp+i) = *(otp+i);
    }
}

/* reserve original normal data */
static void original_normal_save(TMDDATA *tmddt, SVECTOR *orgnrmbf)
{
    SVECTOR *otp,*bsp,*bspb,*dfp;
    int i,j,n,m;

    bsp= tmddt->orgnrm = orgnrmbf;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
  	otp = tmddt->tmdobj[j].nrmtop;
	bsp  += n;
	n = tmddt->tmdobj[j].nrmtotal;
        for( i = 0; i < n; i++) *(bsp+i) = *(otp+i);
    }
}

/* retrieve original vertex data */
static void reset_mime_vertex(TMDDATA *tmddt)
{
    SVECTOR *otp,*bsp;
    int i,j,n,m;

    bsp= tmddt->orgvtx;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
  	otp = tmddt->tmdobj[j].vtxtop;
	bsp += n;
	n = tmddt->tmdobj[j].vtxtotal;
        for( i = 0; i < n; i++){
	    *(otp+i) = *(bsp+i);
	}
    }
}

/* retrieve original normal data */
static void reset_mime_norm(TMDDATA *tmddt)
{
    SVECTOR *otp,*bsp;
    int i,j,n,m;

    bsp= tmddt->orgnrm;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
  	otp = tmddt->tmdobj[j].nrmtop;
	bsp += n;
	n = tmddt->tmdobj[j].nrmtotal;
        for( i = 0; i < n; i++) *(otp+i) = *(bsp+i);
    }
}

/* MIMe operation (vertex) */
static void set_mime_vertex(TMDDATA *tmddt, MDFDATA *vtxdt)
{
    SVECTOR *otp,*bsp,*dfp;
    int i,n;

    n = vtxdt->objtotal;

    for( i = 0; i < n; i++){
  	otp = tmddt->tmdobj[vtxdt->mdfvct[i].object].vtxtop+vtxdt->mdfvct[i].offset;
	dfp = vtxdt->mdfvct[i].top;
	if( vtxdt->mime[i] !=0 ) gteMIMefunc(otp,dfp, vtxdt->mdfvct[i].total,vtxdt->mime[i]);
    }
}


/* MIMe operation (normal) */
static void set_mime_norm(TMDDATA *tmddt, MDFDATA *nrmdt)
{
    SVECTOR *otp,*bsp,*dfp;
    VECTOR tmp;
    int i,n;

    n = nrmdt->objtotal;
    for( i = 0; i < n; i++){
  	otp = tmddt->tmdobj[nrmdt->mdfvct[i].object].nrmtop+nrmdt->mdfvct[i].offset;
	dfp = nrmdt->mdfvct[i].top;
	if( nrmdt->mime[i] !=0 )
	    {
	 	gteMIMefunc(otp,dfp, nrmdt->mdfvct[i].total,nrmdt->mime[i]);
#if 0
		tmp.vx = (otp+i)->vx;
 		tmp.vy = (otp+i)->vy;
		tmp.vz = (otp+i)->vz;
		VectorNormal(&tmp,&tmp);	/* normalization process*/
		(otp+i)->vx = tmp.vx;
 		(otp+i)->vy = tmp.vy;
		(otp+i)->vz = tmp.vz;
#endif
	    }
    }
}


/* the function only to retrieve vertex information  */
void reset_mime_vdf(int num)
{
    reset_mime_vertex(&tmddt0[num]);
}

/* the function only to retrieve normal information  */
void reset_mime_ndf(int num)
{
    reset_mime_norm(&tmddt0[num]);
}

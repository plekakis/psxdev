/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	e-intr.c
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "model.h"
#include "rpy_intr.h"

/***********************************************
 *	RPY angle MIMe */

/* set difference value into dangle, dtrans  */
void rpy_interpolate_start(void)
{
    int i, mime;
    SVECTOR *bsv, *msv, *dsv;
    long *bv, *mv, *dv;

    for (mime=0; mime<MIMENUM; mime++){
	for (i=0; i<PARTNUM; i++){
	    bsv= &model_hand[ARR_BASE][i].angle;
	    msv= &model_hand[ARR_MIME[mime]][i].angle;
	    dsv= &model_hand[ARR_MIME[mime]][i].dangle;
	    dsv->vx = msv->vx - bsv->vx;
	    dsv->vy = msv->vy - bsv->vy;
	    dsv->vz = msv->vz - bsv->vz;
	    marume(dsv);

	    bv= &model_hand[ARR_BASE][i].modelcoord.coord.t[0];
	    mv= &model_hand[ARR_MIME[mime]][i].modelcoord.coord.t[0];
	    dv= model_hand[ARR_MIME[mime]][i].dtrans;
	    dv[0] = mv[0] - bv[0];
	    dv[1] = mv[1] - bv[1];
	    dv[2] = mv[2] - bv[2];
	}
    }
}

/* RPY Angle MIMe calc*/
void rpy_interpolate(long *pr)
{
    int mime,j;
    SVECTOR *isv, *dsv;
    long *iv, *dv;

    for (mime=0; mime<MIMENUM; mime++){
	if (pr[mime]!=0){
	    for (j=0; j<PARTNUM; j++){
		/* angle */
		isv= &model_hand[ARR_INTR][j].angle;
		dsv= &model_hand[ARR_MIME[mime]][j].dangle;
		isv->vx += (dsv->vx*pr[mime])/4096;
		isv->vy += (dsv->vy*pr[mime])/4096;
		isv->vz += (dsv->vz*pr[mime])/4096;

		/* translation */
		iv= &model_hand[ARR_INTR][j].modelcoord.coord.t[0];
		dv= model_hand[ARR_MIME[mime]][j].dtrans;
		iv[0] += (dv[0]*pr[mime]/4096);
		iv[1] += (dv[1]*pr[mime]/4096);
		iv[2] += (dv[2]*pr[mime]/4096);
		ch_angle(ARR_INTR, j, ANG2MAT, 0);
	    }
	}
    }
}

/* reset coordinates */
void rpy_interpolate_reset(void)
{
    int i;
    long *bv, *iv;

    for (i=0; i<PARTNUM; i++){
	/* angle */
	model_hand[ARR_INTR][i].angle = model_hand[ARR_BASE][i].angle;

	/* translation */
	bv= &model_hand[ARR_BASE][i].modelcoord.coord.t[0];
	iv= &model_hand[ARR_INTR][i].modelcoord.coord.t[0];
	iv[0] = bv[0];
	iv[1] = bv[1];
	iv[2] = bv[2];
    }
}

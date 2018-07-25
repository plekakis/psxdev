/* $PSLibId: Runtime Library Release 3.6$ */
/***********************************************
 *	a-intr.c
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
#include "matrix.h"
#include "a_intr.h"

/***********************************************
 *	axes-MIMe : 軸 MIMe
 ***********************************************/
/* axis に回転軸を、theta に回転角を セットする */
void axis_interpolate_start(void)
{
    int mime, part;
    MATRIX tmpm;
    long *bv, *mv, *dv;
    MODEL *mh;

    for (mime=0; mime<MIMENUM; mime++){
	for (part=0; part<PARTNUM; part++){
	    mh= &model_hand[ARR_MIME[mime]][part];

	    TransposeMatrix(&model_hand[ARR_BASE][part].modelcoord.coord, &tmpm);
	    MulMatrix(&tmpm, &mh->modelcoord.coord);
	    if(IsIdMatrix(&tmpm)==1){
		mh->theta=0;
		mh->axis.vx=0;
		mh->axis.vy=0;
		mh->axis.vz=0;
	    } else{
		MATRIX r1, eig, ieig, rot;

		EigenVector(&tmpm, &mh->axis);
		EigenVec2Mat(&mh->axis, &eig);
		TransposeMatrix(&eig,&ieig);
		MulMatrix0(&ieig,&tmpm,&r1);
		MulMatrix0(&r1,&eig,&rot);
		mh->theta=ratan2(rot.m[1][2], rot.m[1][1]);
		if (mh->theta<0){
		    mh->theta= -mh->theta;
		    mh->axis.vx= -mh->axis.vx;
		    mh->axis.vy= -mh->axis.vy;
		    mh->axis.vz= -mh->axis.vz;
		}
	    }

	    /* set translation diff */
	    bv= &model_hand[ARR_BASE][part].modelcoord.coord.t[0];
	    mv= &model_hand[ARR_MIME[mime]][part].modelcoord.coord.t[0];
	    dv= model_hand[ARR_MIME[mime]][part].dtrans;
	    dv[0] = mv[0] - bv[0];
	    dv[1] = mv[1] - bv[1];
	    dv[2] = mv[2] - bv[2];
	}
    }

}

/* axes MIMe calclation : 実際の軸回転 MIMe の演算をおこなう */
void axis_interpolate(long *pr)
{
    int mime, part;
    long delta;
    long prtotal;
    VECTOR ax;

    for (part=0; part<PARTNUM; part++){
	/* check if mimepr value exists */
	prtotal=0;
	for (mime=0; mime<MIMENUM; mime++){
	    if (model_hand[ARR_MIME[mime]][part].theta) prtotal+=pr[mime];
	}
	delta=0;
	if (prtotal){
	    VECTOR tmpv;

	    tmpv.vx=0;
	    tmpv.vy=0;
	    tmpv.vz=0;
	    /* interpolate rotation axes and angles into ax/delta */
	    for (mime=0; mime<MIMENUM; mime++){
		MODEL *mh;

		mh= &model_hand[ARR_MIME[mime]][part];
		if (mh->theta && pr[mime]){
		    long d;

		    /* interpolate axis&theta */
		    d=pr[mime]*mh->theta/4096;
		    /* tmpv.?? format: (1,7,24) */
		    tmpv.vx+=mh->axis.vx*d;
		    tmpv.vy+=mh->axis.vy*d;
		    tmpv.vz+=mh->axis.vz*d;
		}
	    }
	    /* delta=tmpv/ax */
	    /* delta format: (1,19,12) */
	    delta=myVectorNormal(&tmpv, &ax);
	}
	if (delta){
	    MATRIX r1, tmpm;
	    MATRIX eig, ieig;
	    static MATRIX rot={
		{{4096,0,0},{0,0,0},{0,0,0},},{0,0,0}
	    };

	    /* get a matrix to rotate round about the axis */
	    /* 軸を中心とした回転をするマトリクスを計算する */

	    /* eig rotates X-axis into the axis  */
	    /* X 軸に eig を作用させると回転軸になる */
	    EigenVec2Mat(&ax, &eig);
	    /* ieig rotates the axis into X-axis*/
	    /* 回転軸に ieig を作用させると X 軸となる */
	    TransposeMatrix(&eig,&ieig);
	    /* rot does angle delta rotation round about X-axis*/
	    /* rot は X 軸周りに角 delta だけ回転させるマトリクス */
	    rot.m[1][1]=rcos(delta);
	    rot.m[1][2]=rsin(delta);
	    rot.m[2][1]= -rot.m[1][2];
	    rot.m[2][2]=rot.m[1][1];
	    /* tmpm does angle delta rotation round about the axis */
	    /* tmpm は回転軸周りに角 delta だけ回転させるマトリクス */
	    MulMatrix0(&eig,&rot,&r1);
	    MulMatrix0(&r1,&ieig,&tmpm);
	    MulMatrix0(&model_hand[ARR_BASE][part].modelcoord.coord,
		       &tmpm,
		       &model_hand[ARR_INTR][part].modelcoord.coord);
	}

	/* interpolate translations */
	for (mime=0; mime<MIMENUM; mime++){
	    long *iv, *dv;

	    iv= &model_hand[ARR_INTR][part].modelcoord.coord.t[0];
	    dv= model_hand[ARR_MIME[mime]][part].dtrans;
	    iv[0] += (dv[0]*pr[mime]/4096);
	    iv[1] += (dv[1]*pr[mime]/4096);
	    iv[2] += (dv[2]*pr[mime]/4096);
	}
    }
}

/* reset coordinates : 座標系をもとの状態へ戻す */
void  axis_interpolate_reset(void)
{
    int part;

    for (part=0; part<PARTNUM; part++){
	model_hand[ARR_INTR][part].modelcoord.coord=
	    model_hand[ARR_BASE][part].modelcoord.coord;
	model_hand[ARR_INTR][part].modelcoord.flg=0;
    }
}

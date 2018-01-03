/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	a_intr.c
 *
 *	Copyright (C) 1996,1997 Sony Computer Entertainment Inc.
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

static void rotate_matrix(VECTOR *av, long delta, MATRIX *m);
#define mul(x,y) (((x)*(y))/4096)

/***********************************************
  	axes-MIMe */
/* difference rotation axis determined for each key (mh->axis)  */
void axis_interpolate_start(void)
{
    int mime, part;
    MATRIX rotm;
    long *bv, *mv, *dv;
    MODEL *mh;

    for (mime=0; mime<MIMENUM; mime++){		/* for each key*/
	for (part=0; part<PARTNUM; part++){	/* for each joint*/
	    mh= &model_hand[ARR_MIME[mime]][part];

	    TransposeMatrix(&model_hand[ARR_BASE][part].modelcoord.coord, &rotm);
	    MulMatrix(&rotm, &mh->modelcoord.coord);
	    /* at this point rotm is the rotation matrix from the basic form to the key*/
	    if(IsIdMatrix(&rotm)==1){		/* if no difference rotation*/
		mh->axis.vx=0;
		mh->axis.vy=0;
		mh->axis.vz=0;

	    } else{				/* if difference rotation*/
		MATRIX r1, eig, ieig, xrot;
		long theta;

		/* unique vector and difference rotation axes are in the same direction*/
		EigenVector(&rotm, &mh->axis);
		/* determine rotation angle*/
			/* convert rotm to X-axis rotation matrix xrot */
		EigenVec2Mat(&mh->axis, &eig); /* eig is matrix transferring X axis to difference rotation axis */ 
		TransposeMatrix(&eig,&ieig);	/* ieig is the inverse of eig */
		MulMatrix0(&ieig,&rotm,&r1);
		MulMatrix0(&r1,&eig,&xrot);
		theta=ratan2(xrot.m[1][2], xrot.m[1][1]); /* theta is the difference rotation angle*/

		/* set length of difference rotation axis to theta*/
		mh->axis.vx= mul(mh->axis.vx,theta); /* mul(a,b)=a*b/4096 */
		mh->axis.vy= mul(mh->axis.vy,theta);
		mh->axis.vz= mul(mh->axis.vz,theta);
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


/* reset coordinates */
void  axis_interpolate_reset(void)
{
    int part;

    for (part=0; part<PARTNUM; part++){
	/* copy coordinate matrix of basic form*/
	model_hand[ARR_INTR][part].modelcoord.coord=
	    model_hand[ARR_BASE][part].modelcoord.coord;

	model_hand[ARR_INTR][part].modelcoord.flg=0;
    }
}


/* axes MIMe calculation */
void axis_interpolate(long *pr)
{
    int mime, part;
    VECTOR ax_mime;


    for (part=0; part<PARTNUM; part++){    /* for each joint*/

	/* interpolate rotation axes and angles into ax/delta */
	ax_mime.vx=0;
	ax_mime.vy=0;
	ax_mime.vz=0;
	for (mime=0; mime<MIMENUM; mime++){	/* for each key */
	    MODEL *mh;

	    if (pr[mime]){
		mh= &model_hand[ARR_MIME[mime]][part];
		/* ax_mime.vx/vy/vz format: (1,7,24) */
		ax_mime.vx+=mh->axis.vx*pr[mime]; /* add weighted axes*/
		ax_mime.vy+=mh->axis.vy*pr[mime];
		ax_mime.vz+=mh->axis.vz*pr[mime];
	    }
	}

	if (ax_mime.vx||ax_mime.vy||ax_mime.vz){ /* if interpolated axis is not 0*/
	    MATRIX m;
	    long delta;
	    VECTOR ax_norm;

	    /* determine length delta (rotation angle) of interpolated axis*/
	    /* also determine normalized axis ax_norm*/
			    /* (delta=ax_mime/ax_norm, format: (1,19,12)) */
	    delta=newVectorNormal(&ax_mime, &ax_norm);
	    /* determine a matrix m that will perform rotation exactly according to the interpolated axis*/
	    rotate_matrix(&ax_norm, delta, &m);

	    /* rotate coordinate matrix*/
	    MulMatrix(&model_hand[ARR_INTR][part].modelcoord.coord, &m);
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

/***********************************************
 *
 *   void rotate_matrix (VECTOR *av, long delta, MATRIX *m) 
 *
 *     function to determine a matrix that will perform rotation by an angle delta around normalized axis av
 *
 *
 * a rotation matrix m for an angle d around axis {x, y, z} would be calculated as follows:
 *
 *  m =  {[  (1-c)  x^2 + c,      (1-c)  x y + z s,        (1-c)  x z - y s ],
 *     [  (1-c)  x y - z s,       (1-c)  y^2 + c,     (1-c)  y z + x s ],
 *     [  (1-c)  x z + y s,       (1-c)  y z - x s,        (1-c)  z^2 + c ]}
 *
 *   where s=sin (d) , c=cos (d)
 *
 ********************************************** */
static void rotate_matrix(VECTOR *av, long delta, MATRIX *m)
{
    long c,s, c1, s1, tmp;
    extern unsigned long rcossin_tbl[];

    c=rcossin_tbl[delta];
    s=(c<<16)>>16;
    c>>=16;

    c1= 4096-c;

    /* mul(a,b)=a*b/4096 */
    m->m[0][0]= mul(c1,mul(av->vx,av->vx)) + c;
    m->m[1][1]= mul(c1,mul(av->vy,av->vy)) + c;
    m->m[2][2]= mul(c1,mul(av->vz,av->vz)) + c;

    s1=mul(av->vz,s);
    tmp=mul(av->vx,av->vy);
    m->m[0][1]= mul(c1,tmp) + s1;
    m->m[1][0]= mul(c1,tmp) - s1;

    s1=mul(av->vy,s);
    tmp=mul(av->vx,av->vz);
    m->m[0][2]= mul(c1,tmp) - s1;
    m->m[2][0]= mul(c1,tmp) + s1;

    s1=mul(av->vx,s);
    tmp=mul(av->vy,av->vz);
    m->m[1][2]= mul(c1,tmp) + s1;
    m->m[2][1]= mul(c1,tmp) - s1;
}

/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	matrix.c
 *
 *	Copyright (C) 1994-1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "matrix.h"

/********************************************************
 *							*
 *	[t]^*[m]*[t] = [1,          0,          0]	*
 *		       [0, cos(theta), sin(theta)]	*
 *		       [0,-sin(theta), cos(theta)]	*
 *							*
 ********************************************************/
/* get Eigen Vector from Matrix */
void EigenVector(MATRIX *m, VECTOR *t)
            			/* m: Rotation Matrix */
            			/* t: Eigen Vector */
{
    MATRIX me;
    long sm[9];
    long maxs,max;
    VECTOR v[2];
    VECTOR nv;
/*    VECTOR sv,tv,mv;*/
/*    long mint;*/
    long i,j;

    /* [me]=[m]-[Id] Id: unit matrix*/
    me.m[0][0] = m->m[0][0]-4096;
    me.m[0][1] = m->m[0][1];
    me.m[0][2] = m->m[0][2];
    me.m[1][0] = m->m[1][0];
    me.m[1][1] = m->m[1][1]-4096;
    me.m[1][2] = m->m[1][2];
    me.m[2][0] = m->m[2][0];
    me.m[2][1] = m->m[2][1];
    me.m[2][2] = m->m[2][2]-4096;

    /* determine nine 2x2 minors*/
    sm[0] = me.m[1][1]*me.m[2][2] - me.m[1][2]*me.m[2][1];
    sm[1] = me.m[1][0]*me.m[2][2] - me.m[0][2]*me.m[2][0];
    sm[2] = me.m[1][0]*me.m[2][1] - me.m[1][1]*me.m[2][0];

    sm[3] = me.m[0][1]*me.m[2][2] - me.m[0][2]*me.m[2][1];
    sm[4] = me.m[0][0]*me.m[2][2] - me.m[0][2]*me.m[2][0];
    sm[5] = me.m[0][0]*me.m[2][1] - me.m[0][1]*me.m[2][0];

    sm[6] = me.m[0][1]*me.m[2][2] - me.m[0][2]*me.m[2][1];
    sm[7] = me.m[0][0]*me.m[1][2] - me.m[0][2]*me.m[1][0];
    sm[8] = me.m[0][0]*me.m[1][1] - me.m[0][1]*me.m[1][0];

    /* abs() */
    for(i=0;i<3;i++)
	for(j=0;j<3;j++){
	    if(sm[i*3+j]<0) sm[i*3+j]= -sm[i*3+j];
	}

    /* find the minor having the largest absolute value*/
    maxs=0;
    max=sm[0];
    for(i=0;i<3;i++)
	for(j=0;j<3;j++){
	    if(max<sm[i*3+j]){ 
		maxs=i*3+j;
		max=sm[i*3+j];
	    }
	}

    /* put the linearly independent vector based on the minor with the largest absolute value
      into v[0],v[1]*/
    switch(maxs){
      case 6:
      case 7:
      case 8:
	v[0].vx = me.m[0][0];
	v[0].vy = me.m[0][1];
	v[0].vz = me.m[0][2];
	v[1].vx = me.m[1][0];
	v[1].vy = me.m[1][1];
	v[1].vz = me.m[1][2];
	break;
      case 3:
      case 4:
      case 5:
	v[0].vx = me.m[0][0];
	v[0].vy = me.m[0][1];
	v[0].vz = me.m[0][2];
	v[1].vx = me.m[2][0];
	v[1].vy = me.m[2][1];
	v[1].vz = me.m[2][2];
	break;
      case 0:
      case 1:
      case 2:
	v[0].vx = me.m[1][0];
	v[0].vy = me.m[1][1];
	v[0].vz = me.m[1][2];
	v[1].vx = me.m[2][0];
	v[1].vy = me.m[2][1];
	v[1].vz = me.m[2][2];
	break;
    }

    /* make a cross-product vector of v[0] and v[1]*/
    OuterProduct12(&v[0],&v[1],&nv);

    /* normalize cross-product vector (unique vector) */
    VectorNormal(&nv,t);

    return;
}

/* generate a matrix that will transfer the X axis to Vex */
void EigenVec2Mat(VECTOR *ev, MATRIX *smat)
{
    VECTOR v1, v2;
    VECTOR mv0, mv1, mv2;

    preVectorNormal(ev);
    VectorNormal(ev, &mv0);
    /* put unique vector in first column of unique matrix*/
    smat->m[0][0] = mv0.vx;
    smat->m[1][0] = mv0.vy;
    smat->m[2][0] = mv0.vz;

    /* make a vector perpendicular to the unique vector*/
    if (abs(mv0.vy)>abs(mv0.vx)){
	if (abs(mv0.vx)>abs(mv0.vz)){			/* y>x>z */
	    v1.vx = mv0.vy;
	    v1.vy = -mv0.vx;
	    if (mv0.vz)
		v1.vz = -((mv0.vx*v1.vx) + (mv0.vy * v1.vy))/mv0.vz;
	    else
		v1.vz=0;
	} else{					/* y,z>x */
	    if (mv0.vy){
		v1.vz = mv0.vy;
		v1.vy = -mv0.vz;
	    } else{
		v1.vz = -mv0.vy;
		v1.vy = mv0.vz;
	    }
	    if (mv0.vx)
		v1.vx = -((mv0.vz*v1.vz) + (mv0.vy * v1.vy))/mv0.vx;
	    else
		v1.vx=0;
	}
    } else{
	if (abs(mv0.vy)>abs(mv0.vz)){			/* x>y>z */
	    v1.vx = mv0.vy;
	    v1.vy = -mv0.vx;
	    if (mv0.vz)
		v1.vz = -((mv0.vx*v1.vx) + (mv0.vy * v1.vy))/mv0.vz;
	    else
		v1.vz=0;
	} else{					/* x,z>y */
	    if (mv0.vz){
		v1.vx = mv0.vz;
		v1.vz = -mv0.vx;
	    } else{
		v1.vx = -mv0.vz;
		v1.vz = mv0.vx;
	    }
	    if (mv0.vy)
		v1.vy = -((mv0.vx*v1.vx) + (mv0.vz * v1.vz))/mv0.vy;
	    else
		v1.vy=0;
	}
    }


    preVectorNormal(&v1);
    VectorNormal(&v1, &mv1);

    /* put unique vector in second column of unique matrix*/
    smat->m[0][1] = mv1.vx;
    smat->m[1][1] = mv1.vy;
    smat->m[2][1] = mv1.vz;

    /* determine cross-product of first column and second column of unique matrix*/
    OuterProduct12(&mv0,&mv1,&v2);
    
    /* normalize cross-product vector*/
    preVectorNormal(&v2);
    VectorNormal(&v2,&mv2);

    /* put normalized cross-product vector in third column of unique matrix*/
    smat->m[0][2] = mv2.vx;
    smat->m[1][2] = mv2.vy;
    smat->m[2][2] = mv2.vz;

    return;
}

/***********************************************
 *	fit vector size for VectorNormal()
 *	*/
long preVectorNormal(VECTOR *v)
{
    long d, min;

    min= 100;

    d=Lzc(v->vx);
    if (min>d) min=d;

    d=Lzc(v->vy);
    if (min>d) min=d;

    d=Lzc(v->vz);
    if (min>d) min=d;

    
    min=18-min;					/* limit to 2^14 */

    if (min>0){
	v->vx >>=min;
	v->vy >>=min;
	v->vz >>=min;
	return min;
    }

    return 0;
}

/* VectorNormal(v0, v1) and return  (v1/v0). (return value format= (0,32,0)  */
long myVectorNormal(VECTOR *v0, VECTOR *v1)
{
    long a,b,c,d;
    long r;
    VECTOR v2;

    a=preVectorNormal(v0);

    Square12(v0, &v2);			/* v2.vx= v0->vx^2 */
    b=v2.vx+v2.vy+v2.vz;		/* b= |v0|^2 */

#define VECTORLENMIN 10
    if (b<VECTORLENMIN){
	v1->vx=4096;
	v1->vy=0;
	v1->vz=0;
	return 0;
    }

    InvSquareRoot(b, &c, &d);		/* sqr (1/ (b*4096) )  =  (c/4096)  *  (1>>d)  */

    /* v1= v0 * sqr (1/b)  = v0 *  (c/4096)  *  (1>>d)  * sqr 4096 */
    v1->vx = ((v0->vx/64)*c)>>d;
    v1->vy = ((v0->vy/64)*c)>>d;
    v1->vz = ((v0->vz/64)*c)>>d;


    if (c){
	r=(1<<(a+d+6))/c;
	return r;

    } else{
	v1->vx=4096;
	v1->vy=0;
	v1->vz=0;
	return 0;
    }
}

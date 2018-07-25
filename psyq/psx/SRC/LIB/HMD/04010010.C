/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>
#include <asm.h>
#include <libgs.h>
#include <libhmd.h>
#include "localadd.h"
#include "mime.h"

#if 0
/***********************************************
 *	fit vector size for VectorNormal()
 ***********************************************/
static long preVectorNormal(LVECTOR *v)
{
    long d, min;

    min= 100;

    d= gte_lzc_nom(v->vx);
    if (min>d) min=d;

    d= gte_lzc_nom(v->vy);
    if (min>d) min=d;

    d= gte_lzc_nom(v->vz);
    if (min>d) min=d;


    min=18-min;					/* up to 2^14 */

    if (min>0){
	v->vx >>=min;
	v->vy >>=min;
	v->vz >>=min;
	return min;
    }

    return 0;
}
#endif /* 0 */

/* VectorNormal(v0, v1) and return  (v1/v0). (return value format= (0,32,0)  */
static long myVectorNormal(LVECTOR *v0, LVECTOR *v1)
{
    long min,b,c,d;
    long r;
    LVECTOR v2;

    {						/* preVectorNormal */
	long d;

	min= 100;
	d= gte_lzc_nom(v0->vx);
	if (min>d) min=d;
	d= gte_lzc_nom(v0->vy);
	if (min>d) min=d;
	d= gte_lzc_nom(v0->vz);
	if (min>d) min=d;

	min=18-min;				/* up to 2^14 */

	if (min>0){
	    v0->vx >>=min; v0->vy >>=min; v0->vz >>=min;
	} else{
	    min=0;
	}
    }

    gte_ldlvl_1(v0->vx);
    gte_ldlvl_2(v0->vy);
    gte_ldlvl_3(v0->vz);
    gte_sqr12();
    v2.vx=gte_stmac_1();
    v2.vy=gte_stmac_2();
    v2.vz=gte_stmac_3();

/*    Square12((VECTOR *)v0, (VECTOR *)&v2);	/* v2.vx= v0->vx^2 */
    b=v2.vx+v2.vy+v2.vz;			/* b= |v0|^2 */

#define VECTORLENMIN 10
    if (b<VECTORLENMIN){
	v1->vx=4096;
	v1->vy=0;
	v1->vz=0;
	return 0;
    }

    InvSquareRoot(b, &c, &d);	/* sqrt(1/(b*4096)) = (c/4096) * (1>>d) */

    /* v1= v0 * sqrt(1/b) = v0 * (c/4096) * (1>>d) * sqrt(4096) */
    v1->vx = ((v0->vx/64)*c)>>d;
    v1->vy = ((v0->vy/64)*c)>>d;
    v1->vz = ((v0->vz/64)*c)>>d;

    /* v1/v0 = (1<<min) * sqrt(b) = (1<<min) * (4096/c) * (1<<d) / sqrt(4096) */
    if (c){
	r=(1<<(min+d+6))/c;
	return r;

    } else{
	v1->vx=4096;
	v1->vy=0;
	v1->vz=0;
	return 0;
    }
}

static void check_z(LVECTOR *av, MATRIX *org, long delta)
{
    long c,s, c1;
    long as,bs,ds,c1ab,c1ad,c1bd;
    short m[3][3];
    extern unsigned long rcossin_tbl[];
#define mul(x,y) (((x)*(y))/4096)

/*    FntPrint("delta=%04d, abc=%04d\n", delta, (((av->vx*av->vy)>>16)*av->vz)>>16);*/

    c=rcossin_tbl[delta];
    s=(c<<16)>>16;
    c>>=16;

    gte_ldlvl_1(av->vx);
    gte_ldlvl_2(av->vy);
    gte_ldlvl_3(av->vz);


    gte_ldlvl_0(s);
    gte_gpf12();				/* s * av */
    ds=gte_stmac_3();
    bs=gte_stmac_2();
    as=gte_stmac_1();

    c1= 4096-c;
    gte_ldlvl_1(av->vx);
    gte_ldlvl_2(av->vy);
    gte_ldlvl_3(av->vz);
    gte_ldlvl_0(c1);

    gte_gpf12();				/* c1 * av */
    c1ab=mul(gte_stmac_1(),av->vy);
    c1ad=mul(gte_stmac_3(),av->vx);
    c1bd=mul(gte_stmac_2(),av->vz);

    gte_ldlvl_1(av->vx);
    gte_ldlvl_2(av->vy);
    gte_ldlvl_3(av->vz);
    gte_sqr12();				/* av * av */

    m[0][1]= c1ab + ds;
    m[1][0]= c1ab - ds;

    m[0][2]= c1ad - bs;
    m[2][0]= c1ad + bs;

    gte_ldlvl_0(c1);
    gte_gpf12();

    m[1][2]= c1bd + as;
    m[2][1]= c1bd - as;

    m[0][0]= gte_stmac_1() + c;
    m[1][1]= gte_stmac_2() + c;
    m[2][2]= gte_stmac_3() + c;

    gte_SetRotMatrix(org);
    gte_ldlvl_1(m[0][0]);
    gte_ldlvl_2(m[1][0]);
    gte_ldlvl_3(m[2][0]);
/*    gte_ldclmv(m);*/
    gte_rtir();
    org->m[0][0]=gte_stlvl_1();
    org->m[1][0]=gte_stlvl_2();
    org->m[2][0]=gte_stlvl_3();
/*    gte_stclmv(org);*/
    gte_ldlvl_1(m[0][1]);
    gte_ldlvl_2(m[1][1]);
    gte_ldlvl_3(m[2][1]);
/*    gte_ldclmv((char*)m+2);*/
    gte_rtir();
    org->m[0][1]=gte_stlvl_1();
    org->m[1][1]=gte_stlvl_2();
    org->m[2][1]=gte_stlvl_3();
/*    gte_stclmv((char*)org+2);*/
    gte_ldlvl_1(m[0][2]);
    gte_ldlvl_2(m[1][2]);
    gte_ldlvl_3(m[2][2]);
/*    gte_ldclmv((char*)m+4);*/
    gte_rtir();
    org->m[0][2]=gte_stlvl_1();
    org->m[1][2]=gte_stlvl_2();
    org->m[2][2]=gte_stlvl_3();
/*    gte_stclmv((char*)org+4);*/
    
}

/*
	JntAxesMIMe primitive driver.

		Originally written by S.Aoki.
		Aug 25, 1997	N.Yoshioka.
*/

u_long *
GsU_04010010(GsARGUNIT *sp)
{
	GsARGUNIT_JntMIMe	*ap = (GsARGUNIT_JntMIMe *)sp;
	u_long		*pCoordSect	= ap->coord_sect;
	GsCOORDUNIT	*pCoord0	= (GsCOORDUNIT *)(pCoordSect + 1);
	long		*pMIMePr	= ap->mimepr;
	u_long		nMIMeNum	= ap->mimenum;
	/*u_short	nMIMeID		= ap->mimeid;*/
	u_long		*pMIMeDiffSect	= ap->mime_diff_sect;
	JntMIMePrim	*pPrim		= (JntMIMePrim *)ap->primp;
	int		num = pPrim->nNum;
	u_long		*idxMIMeDiff = pPrim->idxMIMeDiff;
	int		i;

	for (i = 0; i < num; i++) {
		/*
			For each JntMIMeDiff ptrs...
		*/
		JntMIMeDiff	*pMIMeDiff
			= (JntMIMeDiff *)&pMIMeDiffSect[idxMIMeDiff[i]];
		u_short	nCoordId = pMIMeDiff->nCoordId;
		u_short	nDiffsNum = pMIMeDiff->nDiffsNum;
		u_long	nDflags = pMIMeDiff->nDflags;
		JntMIMeDiffData	*jntMIMeDiffData = pMIMeDiff->jntMIMeDiffData;
		u_short	j, key;
		u_short	changed = 0;
		LVECTOR	rvtmp;
		LVECTOR	rttmp;

		rvtmp.vx = rvtmp.vy = rvtmp.vz = 0;
		rttmp.vx = rttmp.vy = rttmp.vz = 0;

		j = 0;
		for (key = 0; j < nDiffsNum && key < nMIMeNum; key++) {
			/*
				For each JntMIMeDiffData...
			*/
			long	pr;

			if ((nDflags & (1 << key)) == 0) {
				continue;
			}

			pr = pMIMePr[key];

			if (pr != 0) {
				JntMIMeDiffData	*pMIMeDiffData
					= &jntMIMeDiffData[j];

				if (pMIMeDiffData->dtp & 0x01) {
					changed |= 0x01;
					rvtmp.vx += pMIMeDiffData->dvx * pr;
					rvtmp.vy += pMIMeDiffData->dvy * pr;
					rvtmp.vz += pMIMeDiffData->dvz * pr;
				}

				if (pMIMeDiffData->dtp & 0x02) {
					changed |= 0x02;
					rttmp.vx += pMIMeDiffData->dtx * pr;
					rttmp.vy += pMIMeDiffData->dty * pr;
					rttmp.vz += pMIMeDiffData->dtz * pr;
				}
			}

			j++;
		}

		if (changed) {
			/*
				Turn the flag in RstAxesMIMeData to non-zero
				if something was changed.
				And save the orginal coordinate data.
			*/
			RstAxesMIMeDiffData	*pRst
				= (RstAxesMIMeDiffData *)
					&jntMIMeDiffData[nDiffsNum];
			GsCOORDUNIT	*pCoord = &pCoord0[nCoordId];
			int	m, n;

			pRst->changed = changed;
			for (m = 0; m < 3; m++) {
				for (n = 0; n < 3; n++) {
					pRst->m[m][n] = pCoord->matrix.m[m][n];
				}
			}
			pRst->dtx = pCoord->matrix.t[0];
			pRst->dty = pCoord->matrix.t[1];
			pRst->dtz = pCoord->matrix.t[2];

			if (changed & 0x01) {
				long	delta;
				LVECTOR	ax;

				delta = myVectorNormal(&rvtmp, &ax);
				if (delta) {
					check_z(&ax, &pCoord->matrix, delta);
				}
			}

			if (changed & 0x02){
				pCoord->matrix.t[0] += rttmp.vx / 4096;
				pCoord->matrix.t[1] += rttmp.vy / 4096;
				pCoord->matrix.t[2] += rttmp.vz / 4096;
			}

			pCoord->flg = 0;
		}
	}

	/*
		Return the next primitive.
	*/
	return &((u_long *)pPrim)[pPrim->nSize];
}

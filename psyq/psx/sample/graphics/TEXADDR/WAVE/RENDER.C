/* $PSLibId: Run-time Library Release 4.4$ */
/*
 * "render2.c" - texture address modulation engine
 *               PolyG3 packet sorting routine
 * 
 * Version 1.00 July 18, 1997
 * 
 * Copyright (C) 1993-97 Sony Computer Entertainment Inc.
 * All rights Reserved 
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <inline_c.h>
#include <gtemac.h>
#include <libgpu.h>
#include <libgs.h>
#include <asm.h>

#define M_RED	0x50
#define M_GREEN	0x80
#define M_BLUE	0x78
extern int reflect;
extern int refract;

PACKET *FastG3L(op, vp, np, pk, n, shift, ot)
	TMD_P_G3 *op;
	VERT   *vp, *np;
	PACKET *pk;
	int     n, shift;
	GsOT   *ot;
{
	register	int i;
	register	long *tag;
	volatile POLY_GT3 *si_org;
	volatile POLY_GT3 *si_env;
	union {
		u_long l;
		struct {u_char r,g,b,pad;} rgb;
	} p;
	VECTOR lv;
	u_long *tablePtr;
	long	flg, otz;
	int uu, vv;
	int dd;
	CVECTOR col;
	int ddx, ddy;

	si_env = (POLY_GT3 *) pk;
	si_org = (POLY_GT3 *) (si_env+1);

	for (i = 0; i < n; i++, op++) {
		gte_ldv3(&vp[op->v0], &vp[op->v1], &vp[op->v2]);
		gte_rtpt();			/* RotTransPers3 */
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;
		gte_nclip();		/* NormalClip */
		gte_stopz(&otz);	/* back clip */
#if 0	/* double side polygon */
		if (otz <= 0)
			continue;
#endif

		gte_avsz3();
		gte_stotz(&otz);

		/* Original polygon */
		si_org->code = 0x34;	/* GT3, ABE=off, TGE=off */
		gte_stsxy3_gt3((u_long *) si_org);
		si_org->tpage = 0x013c; /* 16bit,(768,256), ABR=25% */

		/* 1st vertex */
		gte_ApplyRotMatrix(&np[op->n0], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*refract>>24):-((lv.vx*lv.vx)*refract>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*refract>>24):-((lv.vy*lv.vy)*refract>>24);
		uu = 128+((int)si_org->x0>>1)+ddx;
		vv = 128+((int)si_org->y0)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_org->u0 = uu;
		si_org->v0 = vv;
		si_org->r0 = M_RED;
		si_org->g0 = M_GREEN;
		si_org->b0 = M_BLUE;

		/* 2nd vertex */
		gte_ApplyRotMatrix(&np[op->n1], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*refract>>24):-((lv.vx*lv.vx)*refract>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*refract>>24):-((lv.vy*lv.vy)*refract>>24);
		uu = 128+((int)si_org->x1>>1)+ddx;
		vv = 128+((int)si_org->y1)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_org->u1 = uu;
		si_org->v1 = vv;
		si_org->r1 = M_RED;
		si_org->g1 = M_GREEN;
		si_org->b1 = M_BLUE;

		/* 3rd vertex */
		gte_ApplyRotMatrix(&np[op->n2], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*refract>>24):-((lv.vx*lv.vx)*refract>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*refract>>24):-((lv.vy*lv.vy)*refract>>24);
		uu = 128+((int)si_org->x2>>1)+ddx;
		vv = 128+((int)si_org->y2)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_org->u2 = uu;
		si_org->v2 = vv;
		si_org->r2 = M_RED;
		si_org->g2 = M_GREEN;
		si_org->b2 = M_BLUE;

		/* Reflection polygon */
		si_env->code = 0x36;	/* GT3, ABE=on, TGE=off */
		gte_stsxy3_gt3((u_long *) si_env);
		si_env->tpage = 0x012c; /* 16bit,(768,0), ABR=25% */

#define SHIFT  5
#define SHIFT3 1
		/* 1st vertex */
		gte_ApplyRotMatrix(&np[op->n0], &lv);   /* normal->screen coord */
		ddx = (int)lv.vx>>SHIFT;
		ddy = (int)lv.vy>>SHIFT;
		uu = 128-((int)si_env->x0>>SHIFT3)+ddx;
		vv = 128+((int)si_env->y0>>SHIFT3)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_env->u0 = uu;
		si_env->v0 = vv;
		si_env->r0 = reflect;
		si_env->g0 = reflect;
		si_env->b0 = reflect;

		/* 2nd vertex */
		gte_ApplyRotMatrix(&np[op->n1], &lv);   /* normal->screen coord */
		ddx = (int)lv.vx>>SHIFT;
		ddy = (int)lv.vy>>SHIFT;
		uu = 128-((int)si_env->x1>>SHIFT3)+ddx;
		vv = 128+((int)si_env->y1>>SHIFT3)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_env->u1 = uu;
		si_env->v1 = vv;
		si_env->r1 = reflect;
		si_env->g1 = reflect;
		si_env->b1 = reflect;

		/* 3rd vertex */
		gte_ApplyRotMatrix(&np[op->n2], &lv);   /* normal->screen coord */
		ddx = (int)lv.vx>>SHIFT;
		ddy = (int)lv.vy>>SHIFT;
		uu = 128-((int)si_env->x2>>SHIFT3)+ddx;
		vv = 128+((int)si_env->y2>>SHIFT3)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_env->u2 = uu;
		si_env->v2 = vv;
		si_env->r2 = reflect;
		si_env->g2 = reflect;
		si_env->b2 = reflect;

		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si_env = (u_long)((*tag & 0x00ffffff) | 0x09000000);
		*tag = (u_long) si_env & 0x00ffffff;

		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si_org = (u_long)((*tag & 0x00ffffff) | 0x09000000);
		*tag = (u_long) si_org & 0x00ffffff;

		si_env = (POLY_GT3 *) (si_org+1);
		si_org = (POLY_GT3 *) (si_env+1);

	}
	return (PACKET *) si_env;
}

/**************************************************************************
	LINE_F3
 ***************************************************************************/
#define PACKET2		LINE_F3
#define GTE_SETV	gte_stsxy3_f3
#define SET_PACKET	setLineF3

/* Using DMPSX function version */
PACKET *FastG3L2L3(op, vp, np, pk, n, shift, ot, scratch)
	TMD_P_G3 *op;
	VERT *vp, *np;
	PACKET *pk;
	int n, shift;
	GsOT   *ot;
	u_long *scratch;
{
	register	PACKET2 *si;
	register	int i;
	register	long *tag;
	VECTOR lv;
	long	flg, otz;
	SVECTOR *sv;

	si = (PACKET2 *) pk;

	for (i = 0; i < n; i++, op++) {
		gte_ldv3(&vp[op->v0], &vp[op->v1], &vp[op->v2]);
		gte_rtpt();			/* RotTransPers3 */
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;
		gte_nclip();		/* NormalClip */
		gte_stopz(&otz);	/* back clip */
#if 0
		if (otz <= 0)
			continue;
#endif

		GTE_SETV((u_long *) si);
		gte_avsz3();
		gte_stotz(&otz);

		SET_PACKET(si);
		setRGB0(si,0xFF,0x00,0x00);	/* default brightness */

		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si = (u_long)((*tag & 0x00ffffff) | 0x06000000);
		*tag = (u_long) si & 0x00ffffff;
		si++;
	}
	return (PACKET *) si;
}

/**************************************************************************
	PolyG3 
 ***************************************************************************/

extern GsOT *ot2;

PACKET *FastG3L2(op, vp, np, pk, n, shift, ot)
	TMD_P_G3 *op;
	VERT   *vp, *np;
	PACKET *pk;
	int     n, shift;
	GsOT   *ot;
{
	register	int i;
	register	long *tag;
	volatile POLY_GT3 *si_org;
	volatile POLY_GT3 *si_env;
	union {
		u_long l;
		struct {u_char r,g,b,pad;} rgb;
	} p;
	VECTOR lv;
	u_long *tablePtr;
	long	flg, otz;
	int uu, vv;
	int dd;
	CVECTOR col;
	int ddx, ddy;
	/* case using scratch pad stack */
	int rfl = reflect;
	int rfr = refract;	

	si_env = (POLY_GT3 *) pk;
	si_org = (POLY_GT3 *) (si_env+1);

	for (i = 0; i < n; i++, op++) {
		gte_ldv3(&vp[op->v0], &vp[op->v1], &vp[op->v2]);
		gte_rtpt();			/* RotTransPers3 */
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;
		gte_nclip();		/* NormalClip */
		gte_stopz(&otz);	/* back clip */
#if 0	/* double side polygon */
		if (otz <= 0)
			continue;
#endif

		gte_avsz3();
		gte_stotz(&otz);

		/* Original polygon */
		si_org->code = 0x34;	/* GT3, ABE=off, TGE=off */
		gte_stsxy3_gt3((u_long *) si_org);
		si_org->tpage = 0x013c; /* 16bit,(768,256), ABR=25% */

		/* 1st vertex */
		gte_ApplyRotMatrix(&np[op->n0], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*rfr>>24):-((lv.vx*lv.vx)*rfr>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*rfr>>24):-((lv.vy*lv.vy)*rfr>>24);
		uu = 128+((int)si_org->x0>>1)+ddx;
		vv = 128+((int)si_org->y0)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_org->u0 = uu;
		si_org->v0 = vv;
		si_org->r0 = M_RED;
		si_org->g0 = M_GREEN;
		si_org->b0 = M_BLUE;

		/* 2nd vertex */
		gte_ApplyRotMatrix(&np[op->n1], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*rfr>>24):-((lv.vx*lv.vx)*rfr>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*rfr>>24):-((lv.vy*lv.vy)*rfr>>24);
		uu = 128+((int)si_org->x1>>1)+ddx;
		vv = 128+((int)si_org->y1)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_org->u1 = uu;
		si_org->v1 = vv;
		si_org->r1 = M_RED;
		si_org->g1 = M_GREEN;
		si_org->b1 = M_BLUE;

		/* 3rd vertex */
		gte_ApplyRotMatrix(&np[op->n2], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*rfr>>24):-((lv.vx*lv.vx)*rfr>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*rfr>>24):-((lv.vy*lv.vy)*rfr>>24);
		uu = 128+((int)si_org->x2>>1)+ddx;
		vv = 128+((int)si_org->y2)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_org->u2 = uu;
		si_org->v2 = vv;
		si_org->r2 = M_RED;
		si_org->g2 = M_GREEN;
		si_org->b2 = M_BLUE;

		/* Reflection polygon */
		si_env->code = 0x36;	/* GT3, ABE=on, TGE=off */
		gte_stsxy3_gt3((u_long *) si_env);
		si_env->tpage = 0x012c; /* 16bit,(768,0), ABR=25% */

#define SHIFT2 15
		/* 1st vertex */
		gte_ApplyRotMatrix(&np[op->n0], &lv);   /* normal->screen coord */
		uu = 128-(((int)si_env->x0/4)+(int)lv.vx/16)/2;
		vv = 128+(((int)si_env->y0/4)+(int)lv.vy/16)/2;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_env->u0 = uu;
		si_env->v0 = vv;
		si_env->r0 = rfl;
		si_env->g0 = rfl;
		si_env->b0 = rfl;

		/* 2nd vertex */
		gte_ApplyRotMatrix(&np[op->n1], &lv);   /* normal->screen coord */
		uu = 128-(((int)si_env->x1/4)+(int)lv.vx/16)/2;
		vv = 128+(((int)si_env->y1/4)+(int)lv.vy/16)/2;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_env->u1 = uu;
		si_env->v1 = vv;
		si_env->r1 = rfl;
		si_env->g1 = rfl;
		si_env->b1 = rfl;

		/* 3rd vertex */
		gte_ApplyRotMatrix(&np[op->n2], &lv);   /* normal->screen coord */
		uu = 128-(((int)si_env->x2/4)+(int)lv.vx/16)/2;
		vv = 128+(((int)si_env->y2/4)+(int)lv.vy/16)/2;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_env->u2 = uu;
		si_env->v2 = vv;
		si_env->r2 = rfl;
		si_env->g2 = rfl;
		si_env->b2 = rfl;

		tag = (u_long *) (ot2->org + (otz >> shift));
		*(u_long *) si_env = (u_long)((*tag & 0x00ffffff) | 0x09000000);
		*tag = (u_long) si_env & 0x00ffffff;

		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si_org = (u_long)((*tag & 0x00ffffff) | 0x09000000);
		*tag = (u_long) si_org & 0x00ffffff;

		si_env = (POLY_GT3 *) (si_org+1);
		si_org = (POLY_GT3 *) (si_env+1);

	}
	return (PACKET *) si_env;
}


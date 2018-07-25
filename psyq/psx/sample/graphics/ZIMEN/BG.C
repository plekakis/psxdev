/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *
 *
 *	"tuto0.c" loop 3DBG (FOG MAP DATA)
 *
 *		Version 1.00	Jun,  5, 1995
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>	

#include "struct.h"

/* #define DEBUG */

#define max(x,y)	((x)>(y)?(x):(y))
#define max3(x,y,z)	((x)>(y)?max(x,z):max(y,z))
#define max4(x,y,z,w)	(max(x,y)>max(z,w)?max(x,y):max(z,w))

#define min(x,y)	((x)<(y)?(x):(y))
#define min3(x,y,z)	((x)<(y)?min(x,z):min(y,z))
#define min4(x,y,z,w)	(min(x,y)<min(z,w)?min(x,y):min(z,w))

#define rotTransPersPrim4(v0,v1,v2,v3,prim,p,flg)		\
	RotTransPers4((v0),(v1),(v2),(v3),			\
		     (long *)&(prim)->x0,(long *)&(prim)->x1,	\
		     (long *)&(prim)->x2,(long *)&(prim)->x3,	\
		     (p),(flg))


static POLY_FT4 *divPolyFT4(u_long *ot, POLY_FT4 *pk, Gs3DBG0 *bp, CTYPE *ctype,
		     CVECTOR rgb1, SVECTOR v0, int ndiv);

static POLY_FT4 *divPolyFT4_DPQ(u_long *ot, POLY_FT4 *pk, Gs3DBG0 *bp, CTYPE *ctype,
		     CVECTOR rgb1, SVECTOR v0, int ndiv,GsDPCLUT0 *ct);

void     GsMakeDPClut0(GsDPCLUT0 *cluth);


extern PACKET *GsOUT_PACKET_P;
extern long HWD0,VWD0;

#define AM 1 /* Area Clipping Margin */

GsSort3DBG0(bp,gsot,shift,ls)
Gs3DBG0 *bp;
GsOT   *gsot;
int    shift;
MATRIX *ls;
{
	int i,j,k,l;
	SVECTOR v0,v1,v2,v3;
	long  p,flg,startx,starty;
	long  sz,savey,savey2,savex,offnw,offnh;
	long  idxx,idxy;
	POLY_FT4 *pk;
	MATRIX tls;
	VECTOR outs,ins;
	int ndiv;
	int maxx,maxy,minx,miny;
	VECTOR tmpv1,tmpv2,tmpv3,tmpv4;
	int field_maxx,field_maxy,field_minx,field_miny;
	int field_nw,field_nh,field_nw1,field_nh1;
	static CVECTOR rgbv0 = {0x80,0x80,0x80};
	CVECTOR rgbv1;
	int Projection = ReadGeomScreen();
	CTYPE	*ctype;
	u_long *ot;
	
#ifdef DEBUG
	int primno,clipno,divprimno;
#endif

	/* Area Clipping */
	/* get the location of the VIEW point on the map coordinate */
	
	v0.vz=v1.vz=v2.vz=v3.vz=0;
	pk = (POLY_FT4 *)GsOUT_PACKET_P;
	TransposeMatrix(ls,&tls);
	ApplyMatrixLV(&tls,(VECTOR *)&ls->t[0],&outs);
  
	/* get location of the left view clipping point on the map coordinate */
	tmpv1.vx = ls->t[0]-HWD0*(bp->cw*bp->nw/2/Projection)/2;
	tmpv1.vy = ls->t[1];
	tmpv1.vz = ls->t[2]+bp->cw*(bp->nw/2);
	ApplyMatrixLV(&tls,&tmpv1,&tmpv2);
  
	/* get location of the right view clipping point on the map coordinate */
	tmpv1.vx = ls->t[0]+HWD0*(bp->cw*bp->nw/2/Projection)/2;
	tmpv1.vy = ls->t[1];
	tmpv1.vz = ls->t[2]+bp->cw*(bp->nw/2);
	ApplyMatrixLV(&tls,&tmpv1,&tmpv3);
  
	/* get the clipping area on the map coordinate */
	field_maxx = max3(tmpv2.vx, tmpv3.vx, outs.vx);
	field_maxy = max3(tmpv2.vy, tmpv3.vy, outs.vy);
	field_minx = min3(tmpv2.vx, tmpv3.vx, outs.vx);
	field_miny = min3(tmpv2.vy, tmpv3.vy, outs.vy);
  
#ifdef DEBUG
	FntPrint("(%d %d) (%d %d)\n",outs.vx-field_minx,outs.vy-field_miny,
		 field_maxx-outs.vx,field_maxy-outs.vy);
#endif
  
	/* get the orign point , width , hight of clipping area */
	field_nw  = (outs.vx-field_minx+bp->cw-1)/bp->cw+AM;
	field_nh  = (outs.vy-field_miny+bp->ch-1)/bp->ch+AM;
	field_nw1 = (field_maxx-outs.vx+bp->cw-1)/bp->cw+AM;
	field_nh1 = (field_maxy-outs.vy+bp->ch-1)/bp->ch+AM;

	offnw  = -outs.vx/bp->cw-field_nw; /* start cell no of X */
	offnh  = -outs.vy/bp->ch-field_nh; /* start cell no of Y */
	startx = offnw*bp->cw;   /* start cell location of map coordiant X */
	starty = offnh*bp->ch;     /* start cell location of map coordiant Y */
  
	ins.vx = startx;
	ins.vy = starty;
	ins.vz = 0;
	ApplyMatrixLV(ls,&ins,&outs);
  
	/* move the orign point of clipping area to the orign point of
	   the screen coordinate */
	ls->t[0]+=outs.vx;		/* X-axis position of initial cell (screen coordinates) */
	ls->t[1]+=outs.vy;		/* Y-axis position of initial cell (screen coordinates) */
	ls->t[2]+=outs.vz;		/* Z-axis position of initial cell (screen coordinates) */

#ifdef DEBUG
	primno = clipno = divprimno = 0;
#endif
  
	GsSetLsMatrix(ls);
  
	startx = starty = 0;
	/* startx,y set to 0 because alredy set to the LS-MATRIX */

	v2.vy = starty;
  
	for(i=0;i<field_nh+field_nh1;i++) {
		v0.vy = v1.vy = v2.vy;	/* next line y setting */
		v2.vy = v3.vy = v0.vy+bp->ch;
		v1.vx = startx;		/* next line x setting */
		
		idxy = (i+offnh)&bp->ih; /* unify map location x */
      
		for(j=0;j<field_nw+field_nw1;j++) {
#ifdef DEBUG
			primno++;
#endif
			setPolyFT4(pk);	/* code setting */
			idxx = (j+offnw)&bp->iw; /* unify map location x */
			ctype = &bp->ctype[(bp->map[idxy])[idxx]-'0'];
			
			v0.vx = v2.vx = v1.vx; /* next column x setting */
			v1.vx = v3.vx = v1.vx+bp->cw;
	  
			sz = rotTransPersPrim4(&v0,&v1,&v2,&v3,pk,&p,&flg);
			ot = (u_long *)gsot->org+(sz>>shift);
			
			/* get the MIN-MAX */
			maxx = max4(pk->x0, pk->x1, pk->x2, pk->x3);
			maxy = max4(pk->y0, pk->y1, pk->y2, pk->y3);
			minx = min4(pk->x0, pk->x1, pk->x2, pk->x3);
			miny = min4(pk->y0, pk->y1, pk->y2, pk->y3);

			if((maxx < -HWD0/2) || (minx > HWD0/2) ||
			   (maxy < -VWD0/2) || (miny > VWD0/2) ||
			   (maxx == minx) || (maxy == miny)) {
#ifdef DEBUG
				clipno++;
#endif
				continue;
			}
	  
			DpqColor(&rgbv0,p,&rgbv1);
			
			if(sz>bp->cond->nearz &&
			   (maxx-minx<512) && (maxy-miny<512)) {
				
				/* color setting */
				setRGB0(pk,rgbv1.r,rgbv1.g,rgbv1.b);
				setUVWH(pk, ctype->u, ctype->v, 32-1, 32-1);

				pk->tpage = ctype->tpage;
				pk->clut  = *ctype->clut;
				addPrim(ot,pk);
				pk++;
			}
			else  {
				/* DIVIDE */
				ndiv = bp->cond->cond[min(sz>>bp->cond->shift,
							  bp->cond->nz-1)];
#ifdef DEBUG
				divprimno++;
#endif
				pk = divPolyFT4(ot, pk, bp, ctype, rgbv1, v0, ndiv);
			}

		}
	}
#ifdef DEBUG
	FntPrint("%d/%d/%d\n",divprimno,primno-clipno,primno);
#endif
	GsOUT_PACKET_P = (PACKET *)pk;
}

static POLY_FT4 *divPolyFT4(u_long *ot, POLY_FT4 *pk, Gs3DBG0 *bp, CTYPE *ctype,
		     CVECTOR rgbv1, SVECTOR vorg, int ndiv)
{
	/* primitive size after division */
	int dx = bp->cw>>ndiv;
	int dy = bp->ch>>ndiv;		  
	int du = ctype->du>>ndiv;
	int dv = ctype->dv>>ndiv;
	
	/* divied number */
	int n  = 1<<ndiv;
	
	int nx, ny;
	int u, v;
	int lastx, lasty;
	SVECTOR v0, v1, v2, v3;
	long p, flg;
	
	v0.vz = v1.vz = v2.vz = v3.vz = vorg.vz;;
	
	v0.vy = v1.vy = vorg.vy;
	v2.vy = v3.vy = vorg.vy + dy;
	
	for (lasty = ny = 0, v = ctype->v; ny < n; ny++, v += dv) {
		
		v0.vx = v2.vx = vorg.vx;
		v1.vx = v3.vx = vorg.vx + dx;
		if (ny+1 == n) lasty = 1;
		
		for (lastx = nx = 0, u = ctype->u; nx < n; nx++, u += du) {
			if (nx+1 == n) lastx = 1;

			rotTransPersPrim4(&v0, &v1, &v2, &v3, pk, &p, &flg);
			if (flg >= 0) {
				setPolyFT4(pk);
				setRGB0(pk, rgbv1.r, rgbv1.g, rgbv1.b);
				setUVWH(pk, u, v, du-lastx, dv-lasty);
				pk->clut  = *ctype->clut;
				pk->tpage = ctype->tpage;
				addPrim(ot, pk);
				pk++;
			}
			v0.vx += dx; v1.vx += dx; v2.vx += dx; v3.vx += dx;
		}
		v0.vy += dy; v1.vy += dy; v2.vy += dy; v3.vy += dy;
	}
	return(pk);
}


GsSort3DBG0_DPQ(bp,gsot,shift,ls,ct)
Gs3DBG0 *bp;
GsOT   *gsot;
int    shift;
MATRIX *ls;
GsDPCLUT0 *ct;
{
	int i,j,k,l;
	SVECTOR v0,v1,v2,v3;
	long  p,flg,startx,starty;
	long  sz,savey,savey2,savex,offnw,offnh;
	long  idxx,idxy;
	POLY_FT4 *pk;
	MATRIX tls;
	VECTOR outs,ins;
	int ndiv;
	int maxx,maxy,minx,miny;
	VECTOR tmpv1,tmpv2,tmpv3,tmpv4;
	int field_maxx,field_maxy,field_minx,field_miny;
	int field_nw,field_nh,field_nw1,field_nh1;
	static CVECTOR rgbv0 = {0x80,0x80,0x80};
	CVECTOR rgbv1;
	int Projection = ReadGeomScreen();
	CTYPE	*ctype;
	u_long *ot;
	
#ifdef DEBUG
	int primno,clipno,divprimno;
#endif

	/* Area Clipping */
	/* get the location of the VIEW point on the map coordinate */
	
	v0.vz=v1.vz=v2.vz=v3.vz=0;
	pk = (POLY_FT4 *)GsOUT_PACKET_P;
	TransposeMatrix(ls,&tls);
	ApplyMatrixLV(&tls,(VECTOR *)&ls->t[0],&outs);
  
	/* get location of the left view clipping point on the map coordinate */
	tmpv1.vx = ls->t[0]-HWD0*(bp->cw*bp->nw/2/Projection)/2;
	tmpv1.vy = ls->t[1];
	tmpv1.vz = ls->t[2]+bp->cw*(bp->nw/2);
	ApplyMatrixLV(&tls,&tmpv1,&tmpv2);
  
	/* get location of the right view clipping point on the map coordinate */
	tmpv1.vx = ls->t[0]+HWD0*(bp->cw*bp->nw/2/Projection)/2;
	tmpv1.vy = ls->t[1];
	tmpv1.vz = ls->t[2]+bp->cw*(bp->nw/2);
	ApplyMatrixLV(&tls,&tmpv1,&tmpv3);
  
	/* get the clipping area on the map coordinate */
	field_maxx = max3(tmpv2.vx, tmpv3.vx, outs.vx);
	field_maxy = max3(tmpv2.vy, tmpv3.vy, outs.vy);
	field_minx = min3(tmpv2.vx, tmpv3.vx, outs.vx);
	field_miny = min3(tmpv2.vy, tmpv3.vy, outs.vy);
  
#ifdef DEBUG
	FntPrint("(%d %d) (%d %d)\n",outs.vx-field_minx,outs.vy-field_miny,
		 field_maxx-outs.vx,field_maxy-outs.vy);
#endif
  
	/* get the orign point , width , hight of clipping area */
	field_nw  = (outs.vx-field_minx+bp->cw-1)/bp->cw+AM;
	field_nh  = (outs.vy-field_miny+bp->ch-1)/bp->ch+AM;
	field_nw1 = (field_maxx-outs.vx+bp->cw-1)/bp->cw+AM;
	field_nh1 = (field_maxy-outs.vy+bp->ch-1)/bp->ch+AM;

	offnw  = -outs.vx/bp->cw-field_nw; /* start cell no of X */
	offnh  = -outs.vy/bp->ch-field_nh; /* start cell no of Y */
	startx = offnw*bp->cw;   /* start cell location of map coordiant X */
	starty = offnh*bp->ch;     /* start cell location of map coordiant Y */
	
	ins.vx = startx;
	ins.vy = starty;
	ins.vz = 0;
	ApplyMatrixLV(ls,&ins,&outs);
	
	/* move the orign point of clipping area to the orign point of
	   the screen coordinate */
	ls->t[0]+=outs.vx;		/* X-axis position of initial cell (screen coordinates) */
	ls->t[1]+=outs.vy;		/* Y-axis position of initial cell (screen coordinates) */
	ls->t[2]+=outs.vz;		/* Z-axis position of initial cell (screen coordinates) */

#ifdef DEBUG
	primno = clipno = divprimno = 0;
#endif

	GsSetLsMatrix(ls);

	startx = starty = 0;
	/* startx,y set to 0 because alredy set to the LS-MATRIX */

	v2.vy = starty;

	for(i=0;i<field_nh+field_nh1;i++) {
		v0.vy = v1.vy = v2.vy;	/* next line y setting */
		v2.vy = v3.vy = v0.vy+bp->ch;
		v1.vx = startx;		/* next line x setting */
		
		idxy = (i+offnh)&bp->ih; /* unify map location x */
      
		for(j=0;j<field_nw+field_nw1;j++) {
#ifdef DEBUG
			primno++;
#endif
			setPolyFT4(pk);	/* code setting */
			setShadeTex(pk,1);
			idxx = (j+offnw)&bp->iw; /* unify map location x */
			ctype = &bp->ctype[(bp->map[idxy])[idxx]-'0'];
			
			v0.vx = v2.vx = v1.vx; /* next column x setting */
			v1.vx = v3.vx = v1.vx+bp->cw;
	  
			sz = rotTransPersPrim4(&v0,&v1,&v2,&v3,pk,&p,&flg);
			ot = (u_long *)gsot->org+(sz>>shift);
			
			/* get the MIN-MAX */
			maxx = max4(pk->x0, pk->x1, pk->x2, pk->x3);
			maxy = max4(pk->y0, pk->y1, pk->y2, pk->y3);
			minx = min4(pk->x0, pk->x1, pk->x2, pk->x3);
			miny = min4(pk->y0, pk->y1, pk->y2, pk->y3);

			if((maxx < -HWD0/2) || (minx > HWD0/2) ||
			   (maxy < -VWD0/2) || (miny > VWD0/2) ||
			   (maxx == minx) || (maxy == miny)) {
#ifdef DEBUG
				clipno++;
#endif
				continue;
			}
	  
/*			DpqColor(&rgbv0,p,&rgbv1);*/
			
			if(sz>bp->cond->nearz &&
			   (maxx-minx<512) && (maxy-miny<512)) {
				
				/* color setting */
				setUVWH(pk, ctype->u, ctype->v, 32-1, 32-1);

				pk->tpage = ctype->tpage;
/*				pk->clut  = ctype->clut;*/
				if (p<4096){	/* draw only if FOG is not full */
				  pk->clut  = getClut(ct->rectc.x,
						      (ct->rectc.y+(p>>(12-ct->cbit))));
				  addPrim(ot,pk);
				}
				pk++;
			}
			else  {
				/* DIVIDE */
				ndiv = bp->cond->cond[min(sz>>bp->cond->shift,
							  bp->cond->nz-1)];
#ifdef DEBUG
				divprimno++;
#endif
				pk = divPolyFT4_DPQ(ot, pk, bp, ctype, rgbv1, v0, ndiv,ct);
			}

		}
	}
#ifdef DEBUG
	FntPrint("%d/%d/%d\n",divprimno,primno-clipno,primno);
#endif
	GsOUT_PACKET_P = (PACKET *)pk;
}

static POLY_FT4 *divPolyFT4_DPQ(u_long *ot, POLY_FT4 *pk, Gs3DBG0 *bp, CTYPE *ctype,
		     CVECTOR rgbv1, SVECTOR vorg, int ndiv,GsDPCLUT0 *ct)
{
	/* primitive size after division */
	int dx = bp->cw>>ndiv;
	int dy = bp->ch>>ndiv;		  
	int du = ctype->du>>ndiv;
	int dv = ctype->dv>>ndiv;
	
	/* divied number */
	int n  = 1<<ndiv;
	
	int nx, ny;
	int u, v;
	int lastx, lasty;
	SVECTOR v0, v1, v2, v3;
	long p, flg;
	
	v0.vz = v1.vz = v2.vz = v3.vz = vorg.vz;;
	
	v0.vy = v1.vy = vorg.vy;
	v2.vy = v3.vy = vorg.vy + dy;
	
	for (lasty = ny = 0, v = ctype->v; ny < n; ny++, v += dv) {
		
		v0.vx = v2.vx = vorg.vx;
		v1.vx = v3.vx = vorg.vx + dx;
		if (ny+1 == n) lasty = 1;
		
		for (lastx = nx = 0, u = ctype->u; nx < n; nx++, u += du) {
			if (nx+1 == n) lastx = 1;

			rotTransPersPrim4(&v0, &v1, &v2, &v3, pk, &p, &flg);
			if (flg >= 0) {
				setPolyFT4(pk);
				SetShadeTex(pk, 1);
				setUVWH(pk, u, v, du-lastx, dv-lasty);
				pk->tpage = ctype->tpage;
				if(p<4096)
				  {
				    pk->clut = getClut(ct->rectc.x,
						       (ct->rectc.y+(p>>(12-ct->cbit))));
				    
				    addPrim(ot, pk);
				  }
				pk++;
			}
			v0.vx += dx; v1.vx += dx; v2.vx += dx; v3.vx += dx;
		}
		v0.vy += dy; v1.vy += dy; v2.vy += dy; v3.vy += dy;
	}
	return(pk);
}

void GsMakeDPClut0(cluth)
GsDPCLUT0 *cluth;
{
    long r,g,b,stp;
    long p;
    long i,j;
    u_short newclut[256];
    RECT rect;
    u_short *clut;
    
    rect.x=cluth->rectc.x;
    rect.w=cluth->rectc.w;
    rect.h=1;
    clut = cluth->clut;
    
    for (i=0; i<cluth->rectc.h; i++){
	p=i*ONE/cluth->rectc.h;
	
	for (j=0; j<cluth->rectc.w; j++){
	    if (clut[j]==0){			/* no interpolation if transparent*/
		newclut[j]=clut[j];
	    } else{
		/* separate original CLUT into R, G, and B */
		r= (clut[j] & 0x1f)<<3;
		g= ((clut[j] >>5) & 0x1f)<<3;
		b= ((clut[j] >>10) & 0x1f)<<3;
		stp= clut[j] & 0x8000;
		
		/* interpolate R, G, and B with background color*/
		/* calculate interpolation of original color and background color*/
		r= (((long)((u_long)r * (4096-p))) +
		    (((u_long)cluth->bgc.r * p))>>15);
		g= (((long)((u_long)g * (4096-p))) +
		    (((u_long)cluth->bgc.g * p))>>15);
		b= (((long)((u_long)b * (4096-p))) +
		    (((u_long)cluth->bgc.b * p))>>15);
		
		/* rebuild CLUT with FOG */
		newclut[j]= stp |r| (g<<5) | (b<<10);
	    }
	}
	rect.y=cluth->rectc.y+i;
	
	/* load to VRAM */
	LoadImage(&rect,(u_long *)newclut);
    }
}

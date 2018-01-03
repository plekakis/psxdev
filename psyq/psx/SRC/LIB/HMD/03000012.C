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

typedef struct
{
  long vx0,vy0,vz0;
  long vx1,vy1,vz1;
  long vx2,vy2,vz2;    
  short rx,ry;
  short rz,pad;
} LECTOR;

typedef struct
{
  GsSEQ *base;
  LECTOR *src;
  LECTOR *dst;
  LECTOR *intr;
} GsARGANIM1;

extern void setBezierCof(long ,long ,long ,long ,short *);


#if defined(XYZ)
#define FUNC		GsU_03000012
#define ROT_MATRIX	RotMatrix/*XYZ*/

#elif defined(XZY)
#define FUNC		GsU_03001012
#define ROT_MATRIX	RotMatrixXZY

#elif defined(YXZ)
#define FUNC		GsU_03002012
#define ROT_MATRIX	RotMatrixYXZ

#elif defined(YZX)
#define FUNC		GsU_03003012
#define ROT_MATRIX	RotMatrixYZX

#elif defined(ZXY)
#define FUNC		GsU_03004012
#define ROT_MATRIX	RotMatrixZXY

#elif defined(ZYX)
#define FUNC		GsU_03005012
#define ROT_MATRIX	RotMatrixZYX

#else
#error "unknown rotation order specified."
#endif

int
FUNC(GsARGUNIT_ANIM *sp0)
{
  GsCOORDUNIT *dtop;
  u_long ofs0,ofs1;
  GsSEQ *seq;
  GsARGANIM1 *sp;
  long  rframe,tframe;
  long t;
  short co[3][4];
  int i;
  
  sp   = (GsARGANIM1 *)(&(sp0->header_size)+sp0->header_size);
  seq  = sp->base;
  if(seq->tframe==0)
      return 1;
  ofs0 = ((seq->rewrite_idx)&0xff000000)>>24;
  ofs1 = (seq->rewrite_idx)&0x00ffffff;
  
  rframe = seq->rframe;
  tframe = seq->tframe;
  
  t = (tframe-rframe)*0xffff/tframe;
  
  setBezierCof(sp->src->vx0,sp->src->vx1,sp->src->vx2,sp->dst->vx0,co[0]);
  setBezierCof(sp->src->vy0,sp->src->vy1,sp->src->vy2,sp->dst->vy0,co[1]);
  setBezierCof(sp->src->vz0,sp->src->vz1,sp->src->vz2,sp->dst->vz0,co[2]);
  
  dtop = (GsCOORDUNIT *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);

  for(i=0;i<3;i++)
      dtop->matrix.t[i] = ((((((((co[i][0]*t)>>16)+co[i][1])*t)>>16)+co[i][2])
			    *t)>>16)+co[i][3];
    
  dtop->rot.vx = (((long)sp->src->rx)*rframe+((long)sp->dst->rx)*(tframe-rframe))/tframe;
  dtop->rot.vy = (((long)sp->src->ry)*rframe+((long)sp->dst->ry)*(tframe-rframe))/tframe;
  dtop->rot.vz = (((long)sp->src->rz)*rframe+((long)sp->dst->rz)*(tframe-rframe))/tframe;
  ROT_MATRIX(&dtop->rot,&dtop->matrix);
  
  dtop->flg = 0;
  
  if(sp->intr)
    {
      sp->intr->vx0 = dtop->matrix.t[0];
      sp->intr->vy0 = dtop->matrix.t[1];
      sp->intr->vz0 = dtop->matrix.t[2];
      sp->intr->rx = dtop->rot.vx;
      sp->intr->ry = dtop->rot.vy;
      sp->intr->rz = dtop->rot.vz;
    }
  /*
  printf("TRANS %d/%d src (%d %d %d) dst (%d %d %d)\n",rframe,tframe,sp->src->vx,sp->src->vy,sp->src->vz,sp->dst->vx,sp->dst->vy,sp->dst->vz);
  printf("%x (%d %d %d)\n",&dtop->matrix,dtop->matrix.t[0],dtop->matrix.t[1],dtop->matrix.t[2]);
  */
  return 0;
}


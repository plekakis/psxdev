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
    short rx0,ry0,rz0;
    short rx1,ry1,rz1;
    short rx2,ry2,rz2;
}  MYSVECTOR;
    
typedef struct
{
  GsSEQ *base;
  MYSVECTOR *src;
  MYSVECTOR *dst;
  MYSVECTOR *intr;
} GsARGANIM2;

extern void setBezierCof(long ,long ,long ,long ,short *);


#if defined(XYZ)
#define FUNC		GsU_03000020
#define ROT_MATRIX	RotMatrix/*XYZ*/

#elif defined(XZY)
#define FUNC		GsU_03001020
#define ROT_MATRIX	RotMatrixXZY

#elif defined(YXZ)
#define FUNC		GsU_03002020
#define ROT_MATRIX	RotMatrixYXZ

#elif defined(YZX)
#define FUNC		GsU_03003020
#define ROT_MATRIX	RotMatrixYZX

#elif defined(ZXY)
#define FUNC		GsU_03004020
#define ROT_MATRIX	RotMatrixZXY

#elif defined(ZYX)
#define FUNC		GsU_03005020
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
  GsARGANIM2 *sp;
  long  rframe,tframe;
  long t;
  short co[3][4];
  int i;
  
  sp   = (GsARGANIM2 *)(&(sp0->header_size)+sp0->header_size);
  seq  = sp->base;
  if(seq->tframe==0)
      return 1;
  ofs0 = ((seq->rewrite_idx)&0xff000000)>>24;
  ofs1 = (seq->rewrite_idx)&0x00ffffff;
  
  rframe = seq->rframe;
  tframe = seq->tframe;
  
  t = (tframe-rframe)*0xffff/tframe;
  
  setBezierCof(sp->src->rx0,sp->src->rx1,sp->src->rx2,sp->dst->rx0,co[0]);
  setBezierCof(sp->src->ry0,sp->src->ry1,sp->src->ry2,sp->dst->ry0,co[1]);
  setBezierCof(sp->src->rz0,sp->src->rz1,sp->src->rz2,sp->dst->rz0,co[2]);
  
  dtop = (GsCOORDUNIT *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);
  dtop->rot.vx = ((((((((co[0][0]*t)>>16)+co[0][1])*t)>>16)+co[0][2])
		   *t)>>16)+co[0][3];
  dtop->rot.vy = ((((((((co[1][0]*t)>>16)+co[1][1])*t)>>16)+co[1][2])
		   *t)>>16)+co[1][3];
  dtop->rot.vz = ((((((((co[2][0]*t)>>16)+co[2][1])*t)>>16)+co[2][2])
		   *t)>>16)+co[2][3];
  
  ROT_MATRIX(&dtop->rot,&dtop->matrix);
  dtop->flg = 0;
  
  if(sp->intr) {
      sp->intr->rx0 = dtop->rot.vx;
      sp->intr->ry0 = dtop->rot.vy;
      sp->intr->rz0 = dtop->rot.vz;
    }
  
  /*
  printf("ROT %d/%d (%d %d %d) (%d %d %D)/(%d %d %d)\n",
  rframe,tframe,dtop->rot.vx,dtop->rot.vy,dtop->rot.vz,
  sp->src->vx,sp->src->vy,sp->src->vz,
  sp->dst->vx,sp->dst->vy,sp->dst->vz);
  */
  return 0;
}

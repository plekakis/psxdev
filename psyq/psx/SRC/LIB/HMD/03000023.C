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
    long vx,vy,vz;
    short rx0,ry0,rz0;
    short rx1,ry1,rz1;
    short rx2,ry2,rz2;
} MYVECTOR;

typedef struct 
{
  GsSEQ *base;
  MYVECTOR *src;
  MYVECTOR *dst;
  MYVECTOR *intr;
} GsARGANIM1;

typedef struct
{
  u_short pidx;
  u_char  tframe;
  u_char  tidx;
} ACTR;

extern void setBetaCof(long ,long ,long ,long ,short *);
extern void setBezierCof(long ,long ,long ,long ,short *);


#if defined(XYZ)
#define FUNC		GsU_03000023
#define ROT_MATRIX	RotMatrix/*XYZ*/

#elif defined(XZY)
#define FUNC		GsU_03001023
#define ROT_MATRIX	RotMatrixXZY

#elif defined(YXZ)
#define FUNC		GsU_03002023
#define ROT_MATRIX	RotMatrixYXZ

#elif defined(YZX)
#define FUNC		GsU_03003023
#define ROT_MATRIX	RotMatrixYZX

#elif defined(ZXY)
#define FUNC		GsU_03004023
#define ROT_MATRIX	RotMatrixZXY

#elif defined(ZYX)
#define FUNC		GsU_03005023
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
  ACTR *ctop;
  MYVECTOR **cpt;
  
  sp   = (GsARGANIM1 *)(&(sp0->header_size)+sp0->header_size);
  seq  = sp->base;
  ofs0 = ((seq->rewrite_idx)&0xff000000)>>24; 
  ofs1 = (seq->rewrite_idx)&0x00ffffff;
  
  rframe = seq->rframe;
  tframe = seq->tframe;
  
  ctop = (ACTR *)sp0->ctop;	/* control top */
  cpt = (MYVECTOR **)(sp0->ptop+(ctop+seq->start)->pidx);
  
  if(seq->speed>=0) {
      if(sp->src == cpt[1]) {
	  cpt[2] = cpt[1];
	  cpt[1] = 0;
	  return 1;
      }
      if(tframe == 0 || rframe == tframe) {
	  cpt[0] = cpt[1];
	  cpt[1] = cpt[2];
	  cpt[2] = sp->src;
	  cpt[3] = sp->dst;
	  for(i=0;i<4;i++)
	      if(cpt[i] == 0)
		  return 1;
	  /*
	  printf("->%x %x %x %x %x\n",cpt[0],cpt[1],cpt[2],cpt[3]);
	  */
      }
  }
  if(seq->speed<0) {
      if(sp->src == cpt[1]) {
	  cpt[1] = cpt[2];
	  cpt[2] = 0;
	  return 1;
      }
      if(tframe == 0 || rframe == 0) {
	  cpt[3] = cpt[2];
	  cpt[2] = cpt[1];
	  cpt[0] = sp->src;
	  cpt[1] = sp->dst;
	  for(i=0;i<4;i++)
	      if(cpt[i] == 0)
		  return 1;
	  /*
	  printf("<-%x %x %x %x\n",cpt[0],cpt[1],cpt[2],cpt[3]);
	  */
      }
  }
  if(tframe == 0) {
      return 1;
  }
  
  t = (tframe-rframe)*0xffff/tframe;
  
  setBezierCof(sp->src->rx0,sp->src->rx1,sp->src->rx2,sp->dst->rx0,co[0]);
  setBezierCof(sp->src->ry0,sp->src->ry1,sp->src->ry2,sp->dst->ry0,co[1]);
  setBezierCof(sp->src->rz0,sp->src->rz1,sp->src->rz2,sp->dst->rz0,co[2]);
  setBetaCof(cpt[0]->vx,cpt[1]->vx,cpt[2]->vx,cpt[3]->vx,co[0]);
  setBetaCof(cpt[0]->vy,cpt[1]->vy,cpt[2]->vy,cpt[3]->vy,co[1]);
  setBetaCof(cpt[0]->vz,cpt[1]->vz,cpt[2]->vz,cpt[3]->vz,co[2]);
  
  dtop = (GsCOORDUNIT *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);
  
  for(i=0;i<3;i++)
      dtop->matrix.t[i] = ((((((((co[i][0]*t)>>16)+co[i][1])*t)>>16)+co[i][2])
	  *t)>>16)+co[i][3];
  dtop->rot.vx = ((((((((co[0][0]*t)>>16)+co[0][1])*t)>>16)+co[0][2])
		   *t)>>16)+co[0][3];
  dtop->rot.vy = ((((((((co[1][0]*t)>>16)+co[1][1])*t)>>16)+co[1][2])
		   *t)>>16)+co[1][3];
  dtop->rot.vz = ((((((((co[2][0]*t)>>16)+co[2][1])*t)>>16)+co[2][2])
		   *t)>>16)+co[2][3];
  ROT_MATRIX(&dtop->rot,&dtop->matrix);
  dtop->flg = 0;
  
  if(sp->intr)
    {
      sp->intr->vx = dtop->matrix.t[0];
      sp->intr->vy = dtop->matrix.t[1];
      sp->intr->vz = dtop->matrix.t[2];
      sp->intr->rx0 = dtop->rot.vx;
      sp->intr->ry0 = dtop->rot.vy;
      sp->intr->rz0 = dtop->rot.vz;
    }
  /*
  printf("TRANS %d/%d src (%d %d %d) dst (%d %d %d)\n",rframe,tframe,sp->src->vx0,sp->src->vy0,sp->src->vz0,sp->dst->vx0,sp->dst->vy0,sp->dst->vz0);
  printf("%x (%d %d %d)\n",&dtop->matrix,dtop->matrix.t[0],dtop->matrix.t[1],dtop->matrix.t[2]);
  */
  return 0;
}


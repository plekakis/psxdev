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
    short vx0,vy0,vz0;
    short vx1,vy1,vz1;
    short vx2,vy2,vz2;
} MYVECTOR;


typedef struct 
{
  GsSEQ *base;
  MYVECTOR *src;
  MYVECTOR *dst;
  SVECTOR *intr;
} GsARGANIM1;

extern setBezierCof(long ,long ,long ,long ,short *);

/* hokan driver */
int GsU_03010271(GsARGUNIT_ANIM *sp0)
{
  SVECTOR *dtop;
  u_long ofs0,ofs1;
  GsSEQ *seq;
  GsARGANIM1 *sp;
  long  rframe,tframe;
  long t;
  short co[3][4];
  
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
  
  dtop = (SVECTOR *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);
  
  dtop->vx = ((((((((co[0][0]*t)>>16)+co[0][1])*t)>>16)+co[0][2]) *t)>>16)+co[0][3];
  dtop->vy = ((((((((co[1][0]*t)>>16)+co[1][1])*t)>>16)+co[1][2]) *t)>>16)+co[1][3];
  dtop->vz = ((((((((co[2][0]*t)>>16)+co[2][1])*t)>>16)+co[2][2]) *t)>>16)+co[2][3];
  
  if(sp->intr)
    {
      sp->intr->vx = dtop->vx;
      sp->intr->vy = dtop->vy;
      sp->intr->vz = dtop->vz;
    }
  
  /*
  printf("TRANS %d/%d src (%d %d %d) dst (%d %d %d)\n",rframe,tframe,sp->src->vx0,sp->src->vy0,sp->src->vz0,sp->dst->vx0,sp->dst->vy0,sp->dst->vz0);
  printf("%x (%d %d %d)\n",&dtop->matrix,dtop->matrix.t[0],dtop->matrix.t[1],dtop->matrix.t[2]);
  */
  return 0;
}

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
  GsSEQ *base;
  SVECTOR *src;
  SVECTOR *dst;
  SVECTOR *intr;
} GsARGANIM1;
  
/* hokan driver */
int GsU_03000100(GsARGUNIT_ANIM *sp0)
{
  GsCOORDUNIT *dtop;
  u_long ofs0,ofs1;
  GsSEQ *seq;
  GsARGANIM1 *sp;
  long  rframe,tframe;
  VECTOR scale;
  
  sp   = (GsARGANIM1 *)(&(sp0->header_size)+sp0->header_size);
  seq  = sp->base;
  if(seq->tframe==0)
      return 1;
  ofs0 = ((seq->rewrite_idx)&0xff000000)>>24;
  ofs1 = (seq->rewrite_idx)&0x00ffffff;
  
  rframe = seq->rframe;
  tframe = seq->tframe;
  
  dtop = (GsCOORDUNIT *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);
  scale.vx = (sp->src->vx*rframe+sp->dst->vx*(tframe-rframe))/tframe;
  scale.vy = (sp->src->vy*rframe+sp->dst->vy*(tframe-rframe))/tframe;
  scale.vz = (sp->src->vz*rframe+sp->dst->vz*(tframe-rframe))/tframe;
  
  ScaleMatrix(&dtop->matrix,&scale);
  dtop->flg = 0;
  
  if(sp->intr)
    {
      sp->intr->vx = scale.vx;
      sp->intr->vy = scale.vy;
      sp->intr->vz = scale.vz;
    }
  /*
  printf("TRANS %d/%d src (%d %d %d) dst (%d %d %d)\n",rframe,tframe,sp->src->vx,sp->src->vy,sp->src->vz,sp->dst->vx,sp->dst->vy,sp->dst->vz);
  printf("%x (%d %d %d)\n",&dtop->matrix,dtop->matrix.t[0],dtop->matrix.t[1],dtop->matrix.t[2]);
  */
  return 0;
}

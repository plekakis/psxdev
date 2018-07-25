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
  long	vx,vy,vz;
  short scale,pad;
} SSVECTOR;

typedef struct 
{
  GsSEQ *base;
  SSVECTOR *src;
  SSVECTOR *dst;
  SSVECTOR *intr;
} GsARGANIM2;


int GsU_03000901(GsARGUNIT_ANIM *sp0)
{
  GsCOORDUNIT *dtop;
  u_long ofs0,ofs1;
  GsSEQ *seq;
  GsARGANIM2 *sp;
  long  rframe,tframe;
  VECTOR scale;
  
  sp   = (GsARGANIM2 *)(&(sp0->header_size)+sp0->header_size);
  seq  = sp->base;
  if(seq->tframe==0)
      return 1;
  ofs0 = ((seq->rewrite_idx)&0xff000000)>>24;
  ofs1 = (seq->rewrite_idx)&0x00ffffff;

  rframe = seq->rframe;
  tframe = seq->tframe;
  
  dtop = (GsCOORDUNIT *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);
  dtop->matrix.t[0] = (sp->src->vx*rframe+sp->dst->vx*(tframe-rframe))/tframe;
  dtop->matrix.t[1] = (sp->src->vy*rframe+sp->dst->vy*(tframe-rframe))/tframe;
  dtop->matrix.t[2] = (sp->src->vz*rframe+sp->dst->vz*(tframe-rframe))/tframe;
  
  scale.vx = scale.vy = scale.vz =
    (sp->src->scale*rframe+sp->dst->scale*(tframe-rframe))/tframe;
  ScaleMatrix(&dtop->matrix,&scale);
  
  dtop->flg = 0;
  
  if(sp->intr)
    {
      sp->intr->vx = dtop->rot.vx;
      sp->intr->vy = dtop->rot.vy;
      sp->intr->vz = dtop->rot.vz;
      sp->intr->scale = scale.vx;
    }
  /*
  printf("ROT %d/%d (%d %d %d) (%d %d %D)/(%d %d %d)\n",
	 rframe,tframe,dtop->rot.vx,dtop->rot.vy,dtop->rot.vz,
	 sp->src->vx,sp->src->vy,sp->src->vz,
	 sp->dst->vx,sp->dst->vy,sp->dst->vz);
	 */
  return 0;
}

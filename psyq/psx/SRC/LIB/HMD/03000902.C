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
  long	vx0,vy0,vz0;
  long	vx1,vy1,vz1;
  long	vx2,vy2,vz2;
  short scale,pad;
} SSVECTOR;

typedef struct 
{
  GsSEQ *base;
  SSVECTOR *src;
  SSVECTOR *dst;
  SSVECTOR *intr;
} GsARGANIM2;


extern void setBezierCof(long ,long ,long ,long ,short *);

int GsU_03000902(GsARGUNIT_ANIM *sp0)
{
  GsCOORDUNIT *dtop;
  u_long ofs0,ofs1;
  GsSEQ *seq;
  GsARGANIM2 *sp;
  long  rframe,tframe;
  VECTOR scale;
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
  
  setBezierCof(sp->src->vx0,sp->src->vx1,sp->src->vx2,sp->dst->vx0,co[0]);
  setBezierCof(sp->src->vy0,sp->src->vy1,sp->src->vy2,sp->dst->vy0,co[1]);
  setBezierCof(sp->src->vz0,sp->src->vz1,sp->src->vz2,sp->dst->vz0,co[2]);
  
  dtop = (GsCOORDUNIT *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);

  for(i=0;i<3;i++)
      dtop->matrix.t[i] = ((((((((co[i][0]*t)>>16)+co[i][1])*t)>>16)+co[i][2])
	  *t)>>16)+co[i][3];
  
  scale.vx = scale.vy = scale.vz =
    (sp->src->scale*rframe+sp->dst->scale*(tframe-rframe))/tframe;
  ScaleMatrix(&dtop->matrix,&scale);
  
  dtop->flg = 0;
  
  if(sp->intr)
    {
      sp->intr->vx0 = dtop->rot.vx;
      sp->intr->vy0 = dtop->rot.vy;
      sp->intr->vz0 = dtop->rot.vz;
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

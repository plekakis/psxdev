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
    u_char vx0;
    u_char vx1;
    u_char vx2;
} MYVECTOR;


typedef struct 
{
  GsSEQ *base;
  MYVECTOR *src;
  MYVECTOR *dst;
  u_char *intr;
} GsARGANIM1;

extern setBezierCof(long ,long ,long ,long ,short *);

/* hokan driver */
int GsU_03010212(GsARGUNIT_ANIM *sp0)
{
  u_char *dtop;
  u_long ofs0,ofs1;
  GsSEQ *seq;
  GsARGANIM1 *sp;
  long  rframe,tframe;
  long t;
  short co[4];
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
  
  setBezierCof(sp->src->vx0,sp->src->vx1,sp->src->vx2,sp->dst->vx0,co);
  
  dtop = (u_char *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);

  *dtop = ((((((((co[0]*t)>>16)+co[1])*t)>>16)+co[2]) *t)>>16)+co[3];
  
  if(sp->intr)
    {
      *sp->intr = *dtop;
    }
  
  /*
  printf("TRANS %d/%d src (%d %d %d) dst (%d %d %d)\n",rframe,tframe,sp->src->vx0,sp->src->vy0,sp->src->vz0,sp->dst->vx0,sp->dst->vy0,sp->dst->vz0);
  printf("%x (%d %d %d)\n",&dtop->matrix,dtop->matrix.t[0],dtop->matrix.t[1],dtop->matrix.t[2]);
  */
  return 0;
}

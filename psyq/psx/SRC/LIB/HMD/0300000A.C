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
    short pad;
} MYVECTOR;


typedef struct 
{
  GsSEQ *base;
  MYVECTOR *src;
  MYVECTOR *dst;
  MYVECTOR *intr;
} GsARGANIM1;

extern void setBezierCof(long ,long ,long ,long ,short *);

/* hokan driver */
int GsU_0300000a(GsARGUNIT_ANIM *sp0)
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
  
  setBezierCof((long)sp->src->vx0,(long)sp->src->vx1,(long)sp->src->vx2,
	       (long)sp->dst->vx0,co[0]);
  setBezierCof((long)sp->src->vy0,(long)sp->src->vy1,(long)sp->src->vy2,
	       (long)sp->dst->vy0,co[1]);
  setBezierCof((long)sp->src->vz0,(long)sp->src->vz1,(long)sp->src->vz2,
	       (long)sp->dst->vz0,co[2]);
  
  dtop = (GsCOORDUNIT *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);

  for(i=0;i<3;i++)
      dtop->matrix.t[i] = ((((((((co[i][0]*t)>>16)+co[i][1])*t)>>16)+co[i][2])
	  *t)>>16)+co[i][3];
  dtop->flg = 0;
  
  if(sp->intr)
    {
      sp->intr->vx0 = dtop->matrix.t[0];
      sp->intr->vy0 = dtop->matrix.t[1];
      sp->intr->vz0 = dtop->matrix.t[2];
    }
  
  /*
  printf("TRANS %d/%d src (%d %d %d) dst (%d %d %d)\n",rframe,tframe,sp->src->vx0,sp->src->vy0,sp->src->vz0,sp->dst->vx0,sp->dst->vy0,sp->dst->vz0);
  printf("%x (%d %d %d)\n",&dtop->matrix,dtop->matrix.t[0],dtop->matrix.t[1],dtop->matrix.t[2]);
  */
  return 0;
}

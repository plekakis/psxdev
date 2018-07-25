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
    u_char *src;
    u_char *dst;
    u_char *intr;
} GsARGANIM1;
  
/* general hokan driver (32bit) */
int GsU_03010122(GsARGUNIT_ANIM *sp0)
{
  u_char  *dtop;
  u_long ofs0,ofs1;
  GsSEQ  *seq;
  GsARGANIM1 *sp;
  long  rframe,tframe;
  
  sp   = (GsARGANIM1 *)(&(sp0->header_size)+sp0->header_size);
  seq  = sp->base;
  if(seq->tframe==0)
      return 1;
  ofs0 = ((seq->rewrite_idx)&0xff000000)>>24;
  ofs1 = (seq->rewrite_idx)&0x00ffffff;
  
  rframe = seq->rframe;
  tframe = seq->tframe;
  
  dtop = (u_char *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);
  dtop++;
  *dtop = (*(sp->src)*rframe+*(sp->dst)*(tframe-rframe))/tframe;
  /*
    printf("dtop = %x ofs0= %d ofs1=%d %x\n",dtop,ofs0,ofs1,seq->rewrite_idx);
    */
  if(sp->intr)
      {
	  *sp->intr = *dtop;
      }
  
  /*
  printf("TRANS %d/%d src (%d %d %d) dst (%d %d %d)\n",rframe,tframe,sp->src->vx,sp->src->vy,sp->src->vz,sp->dst->vx,sp->dst->vy,sp->dst->vz);
  printf("%x (%d %d %d)\n",&dtop->matrix,dtop->matrix.t[0],dtop->matrix.t[1],dtop->matrix.t[2]);
  */
  return 0;
}

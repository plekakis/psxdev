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
  unsigned long size1;
  unsigned long size2;
  unsigned long *htop;
  unsigned long *ctop;
  unsigned long *ptop;
} ANIM_HEAD;

typedef struct
{
  u_short pidx;
  u_char  tframe;
  u_char  tidx;
} ACTR;

#define SHIFT_AMOUNT 10

void GsSetBetaParam(short s,short t,GsSEQ *seq,u_short start,u_long *h)
{
    short *kei;
    ACTR *ctop;
    ANIM_HEAD *head;
    long ro,s3,s2;
    
    head = (ANIM_HEAD *)h;
    ctop = (ACTR *)head->ctop;	/* control top */
    kei = (short *)(head->ptop+(ctop+seq->start)->pidx);
    
    s2 = (s*s)>>SHIFT_AMOUNT;
    s3 = (s2*s)>>SHIFT_AMOUNT;
    
    ro = 2*s3+4*s2+4*s+t+(2<<SHIFT_AMOUNT);
    
    /*
    printf("s2 = %d s3 = %d ro = %d\n",s2,s3,ro);
    */
    
    kei[0]  = ((-2*s3)<<12)/ro;
    kei[1]  = ((2*(s3+s2+s+t))<<12)/ro;
    kei[2]  = ((-2*(s2+s+t+(1<<SHIFT_AMOUNT)))<<12)/ro;
    kei[3]  = ((6*s3)<<12)/ro;
    kei[4]  = ((-3*(2*s3+2*s2+t))<<12)/ro;
    kei[5]  = ((3*(2*s2+t))<<12)/ro;
    kei[6]  = ((-6*s3)<<12)/ro;
    kei[7]  = ((6*(s3-s))<<12)/ro;
    kei[8]  = ((6*s)<<12)/ro;
    kei[9]  = ((2*s3)<<12)/ro;
    kei[10] = (((4*s3+4*s+t))<<12)/ro;
    kei[11] = ((((long)2<<SHIFT_AMOUNT))<<12)/ro;
    
    /*
      printf("KEI = %d %d %d %d %d %d %d %d %d %d %d %d\n",kei[0],kei[1],kei[2],kei[3],kei[4],kei[5],kei[6],kei[7],kei[8],kei[9],kei[10],kei[11]);
      */
}

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

u_long *GsScanAnim(u_long *addr,GsTYPEUNIT *ut)
{
  static u_long num_types;
  static u_long *top_types,*typep;

  if(addr)
    {				/* set num_types,top_types,typep */
      top_types = (u_long *)*((u_long *)(*(addr-2))+2);
      num_types = *top_types;
      if(num_types & 0x80000000 ==0)
	return 0;		/* already set */
      num_types&=0x7fffffff;
      /*
      printf("num_types = %d  from %x\n",num_types,top_types);
      */
      *top_types = num_types;	/* clear msb */
      typep = top_types+1;
      return addr;
    }
  
  if(num_types==0)
    return(0);		/* end of types*/
  
  ut->ptr = typep;
  ut->type = *typep;
  typep++;
  num_types--;
  /*
  printf("anim type = %x / %x\n",ut->type,ut->ptr);
  */
  return(ut->ptr);
}

long GsLinkAnim(GsSEQ **seq, u_long *ptr)
{
  int i,num;
  u_long *base;
  /*
  printf("link ptr = %x\n",ptr);
  */
  num = (*(ptr+1))>>16;
  /*
  printf("num = %d\n\n",num);
  */
  base = ptr+2;
  for(i=0;i<num;i++,base+= (*(base+1))&0xffff)
    seq[i] = (GsSEQ *)(base);
  return(num);
}

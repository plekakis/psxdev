/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


/*
 * op    : Original data for 4 vertices
 * level : Divide level
 */
static void Adiv_f4(scratch,ifo,level)
u_long *scratch;
GsADIV_F4 *ifo;
int level;
{
  register GsADIV_P_F4 *in,*out;
  register POLY_F4 *si;
  static void sukima();
  static int  outline();
  static void bunV();
  
  in  = (GsADIV_P_F4 *)scratch;
  scratch += (sizeof(GsADIV_P_F4)/4);
  out = (GsADIV_P_F4 *)scratch;
  
  gte_ldv3(&in->vt[0].vx,&in->vt[1].vx,&in->vt[2].vx);
  gte_rtpt();
  si = &ifo->si;
  gte_stflg(&ifo->flg);
  
  gte_stsxy3_f4((long *)si);
  
  gte_ldv0(&in->vt[3].vx);
  gte_rtps();
  gte_stflg(&ifo->flg0);
  gte_stsxy((long *)&si->x3);
  gte_avsz4();
  ifo->flg |= ifo->flg0;
  gte_stszotz(&ifo->otz);
  
  if(outline(si,ifo))
    return;
  
  if(ifo->limit == level){
    if(ifo->flg & 0x7f85e000)
      return;
    else
      goto aaa;
  }
  
  if(ifo->flg>=0 && ifo->maxx-ifo->minx<ifo->adivw &&
     ifo->maxy-ifo->miny<ifo->adivh)
    {
    aaa:
      *ifo->pk = *(u_long *)si;
      *(ifo->pk+1) = *(u_long *)((u_long *)si+1);
      *(ifo->pk+2) = *(u_long *)((u_long *)si+2);
      *(ifo->pk+3) = *(u_long *)((u_long *)si+3);
      *(ifo->pk+4) = *(u_long *)((u_long *)si+4);
      *(ifo->pk+5) = *(u_long *)((u_long *)si+5);
      ifo->tag = ifo->org+(ifo->otz>>ifo->shift);
      *(u_long *)ifo->pk  = (*ifo->tag&0x00ffffff)|0x05000000;
      *ifo->tag = (u_long)ifo->pk&0x00ffffff;
      ifo->pk += sizeof(POLY_F4)/4;
      return;
    }
  
  level++;
  /********************** 1 st ***************************/
  out->vt[0] = in->vt[0];
  bunV(&out->vt[1],&in->vt[0],&in->vt[1]);
  bunV(&out->vt[2],&in->vt[0],&in->vt[2]);
  bunV(&out->vt[3],&in->vt[0],&in->vt[3]);
  sukima(&in->vt[0],&in->vt[1],&out->vt[1],ifo);
  Adiv_f4(scratch,ifo,level);
  
  /********************** 2 nd ***************************/
  out->vt[0]=in->vt[1];
  bunV(&out->vt[2],&in->vt[1],&in->vt[3]);
  sukima(&in->vt[1],&in->vt[3],&out->vt[2],ifo);
  Adiv_f4(scratch,ifo,level);
  
  /********************** 3 rd ***************************/
  out->vt[0]=in->vt[3];
  bunV(&out->vt[1],&in->vt[3],&in->vt[2]);
  sukima(&in->vt[3],&in->vt[2],&out->vt[1],ifo);
  Adiv_f4(scratch,ifo,level);
  
  /********************** 4 th ***************************/
  out->vt[0] =in->vt[2];
  bunV(&out->vt[2],&in->vt[0],&in->vt[2]);
  sukima(&in->vt[2],&in->vt[0],&out->vt[2],ifo);
  Adiv_f4(scratch,ifo,level);
}


static sukima(v0,v1,v2,ifo)
VERT *v0,*v1,*v2;
GsADIV_F4 *ifo;
{
  POLY_F3 *si;
  
  gte_ldv3(&v0->vx,&v1->vx,&v2->vx);
  gte_rtpt();
  si = (POLY_F3 *)ifo->pk;
  *(u_long *)&si->r0 = *(u_long *)&ifo->si.r0;
  setPolyF3(si);
  
  gte_stflg(&ifo->flg);
  
/*
  if(ifo->flg&0x00060000)
    return;
*/
  
  if(ifo->flg&0x80000000)
    return;
  gte_avsz3();
  gte_stsxy3_f3((long *)si);
  gte_stotz(&ifo->otz);
  ifo->tag = ifo->org+(ifo->otz>>ifo->shift);
  *(u_long *)si  = (*ifo->tag&0x00ffffff)|0x04000000;
  *ifo->tag = (u_long)si&0x00ffffff;
  ifo->pk += sizeof(POLY_F3)/4;
}

static outline(si,ifo)
POLY_F4 *si;
GsADIV_F4 *ifo;
{
  minmax4(si->x0,si->x1,si->x2,si->x3,ifo->minx,ifo->maxx);
  if((ifo->maxx<-ifo->hwd0) || (ifo->minx>ifo->hwd0)) /* HW clip */
    return 1;
  minmax4(si->y0,si->y1,si->y2,si->y3,ifo->miny,ifo->maxy);
  if((ifo->maxy<-ifo->vwd0) || (ifo->miny>ifo->vwd0))
    return 1;
  return 0;
}

static bunV(out,in1,in2)
VERT *out,*in1,*in2;
{
  out->vx=(in1->vx+in2->vx)/2;
  out->vy=(in1->vy+in2->vy)/2;
  out->vz=(in1->vz+in2->vz)/2;
}  

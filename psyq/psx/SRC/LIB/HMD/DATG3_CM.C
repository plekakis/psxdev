/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


/*
 * op    : Original data for 3 vertices
 * level : Divide level
 */
static void Adiv_gt3(scratch,ifo,level)
u_long *scratch;
GsADIV_GT3 *ifo;
int level;
{
  register GsADIV_P_GT3 *in,*out;
  register POLY_GT3 *si;
  VECTOR   tmp;
  static void sukima();
  static int outline();
  static void bunCV();
  
  in  = (GsADIV_P_GT3 *)scratch;
  scratch += (sizeof(GsADIV_P_GT3)/4);
  out = (GsADIV_P_GT3 *)scratch;
  
  gte_ldv3(&in->vt[0].vx,&in->vt[1].vx,&in->vt[2].vx);
  gte_rtpt();
  
  si = &ifo->si;
  *(u_short *)&si->u0 = *(u_short *)&in->vt[0].tu;
  *(u_short *)&si->u1 = *(u_short *)&in->vt[1].tu;
  *(u_short *)&si->u2 = *(u_short *)&in->vt[2].tu;
  gte_stflg(&ifo->flg);
  
  gte_stsxy3_gt3((long *)si);
  gte_avsz3();
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
      *(ifo->pk+1) = *(u_long *)&in->vt[0].col.r;
      *(ifo->pk+2) = *(u_long *)((u_long *)si+2);
      *(ifo->pk+3) = *(u_long *)((u_long *)si+3);
      *(ifo->pk+4) = *(u_long *)&in->vt[1].col.r;
      *(ifo->pk+5) = *(u_long *)((u_long *)si+5);
      *(ifo->pk+6) = *(u_long *)((u_long *)si+6);
      *(ifo->pk+7) = *(u_long *)&in->vt[2].col.r;
      *(ifo->pk+8) = *(u_long *)((u_long *)si+8);
      *(ifo->pk+9) = *(u_long *)((u_long *)si+9);
      ifo->tag = ifo->org+(ifo->otz>>ifo->shift);
      *(u_long *)ifo->pk  = (*ifo->tag&0x00ffffff)|0x09000000;
      *ifo->tag = (u_long)ifo->pk&0x00ffffff;
      ifo->pk += sizeof(POLY_GT3)/4;
      return;
    }
  
  level++;
  
  /********************** 1 st ***************************/
  out->vt[0] = in->vt[0];
  bunCV(&out->vt[1],&in->vt[0],&in->vt[1]);
  bunCV(&out->vt[2],&in->vt[0],&in->vt[2]);
  sukima(&in->vt[0],&in->vt[1],&out->vt[1],ifo);
  Adiv_gt3(scratch,ifo,level);
  
  /********************** 2 nd ***************************/
  bunCV(&out->vt[0],&in->vt[1],&in->vt[2]);
  Adiv_gt3(scratch,ifo,level);
  
  /********************** 3 rd ***************************/
  out->vt[2]=in->vt[1];
  sukima(&in->vt[1],&in->vt[2],&out->vt[0],ifo);
  Adiv_gt3(scratch,ifo,level);
  
  /********************** 4 th ***************************/
  out->vt[1] =in->vt[2];
  bunCV(&out->vt[2],&in->vt[0],&in->vt[2]);
  sukima(&in->vt[2],&in->vt[0],&out->vt[2],ifo);
  Adiv_gt3(scratch,ifo,level);
}


static void sukima(v0,v1,v2,ifo)
VERTC *v0,*v1,*v2;
GsADIV_GT3 *ifo;
{
  POLY_GT3 *si;
  
  gte_ldv3(&v0->vx,&v1->vx,&v2->vx);
  gte_rtpt();
  si = (POLY_GT3 *)ifo->pk;
  *(u_long *)&si->r0 = *(u_long *)&v0->col.r;
  setPolyGT3(si);
  
  gte_stflg(&ifo->flg);
  
  if(ifo->flg<0)
    return;
  
  gte_avsz3();
  gte_stsxy3_gt3((long *)si);
  gte_stotz(&ifo->otz);
  *(u_long *)&si->u0 = *(u_short *)&v0->tu;
  *(u_long *)&si->u1 = *(u_short *)&v1->tu;
  *(u_long *)&si->u2 = *(u_short *)&v2->tu;
  *(u_long *)&si->r1 = *(u_long *)&v1->col.r;
  *(u_long *)&si->r2 = *(u_long *)&v2->col.r;
  si->clut  = ifo->si.clut;
  si->tpage = ifo->si.tpage;
  ifo->tag = ifo->org+(ifo->otz>>ifo->shift);
  *(u_long *)si  = (*ifo->tag&0x00ffffff)|0x09000000;
  *ifo->tag = (u_long)si&0x00ffffff;
  ifo->pk += sizeof(POLY_GT3)/4;
}

static int outline(si,ifo)
POLY_GT3 *si;
GsADIV_GT3 *ifo;
{
  minmax3(si->x0,si->x1,si->x2,ifo->minx,ifo->maxx);
  if((ifo->maxx<-ifo->hwd0) || (ifo->minx>ifo->hwd0)) /* HW clip */
    return 1;
  minmax3(si->y0,si->y1,si->y2,ifo->miny,ifo->maxy);
  if((ifo->maxy<-ifo->vwd0) || (ifo->miny>ifo->vwd0))
    return 1;
  return 0;
}

static void bunCV(out,in1,in2)
VERTC *out,*in1,*in2;
{
  out->vx=(in1->vx+in2->vx)/2;
  out->vy=(in1->vy+in2->vy)/2;
  out->vz=(in1->vz+in2->vz)/2;
  
  out->col.r = (in1->col.r + in2->col.r)/2;
  out->col.g = (in1->col.g + in2->col.g)/2;
  out->col.b = (in1->col.b + in2->col.b)/2;
  out->col.cd = in1->col.cd;

  out->tu = (in1->tu+in2->tu)/2;
  out->tv = (in1->tv+in2->tv)/2;
}

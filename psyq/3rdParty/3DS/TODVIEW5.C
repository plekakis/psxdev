/*
 *      tmdview5: GsDOBJ5 object viewing rotine
 *
 *      "tuto0.c" ******** simple GsDOBJ5 Viewing routine
 *
 *              Version 1.00    Jul,  14, 1994
 *
 *              Copyright (C) 1993 by Sony Computer Entertainment
 *              All rights Reserved
 */

#include <sys/types.h>

#include <libetc.h>            
#include <libgte.h>             
#include <libgpu.h>             
#include <libgs.h>              


static int _GtmdIDTbl[] = {
	1,
	-1
};

#define OBJECTMAX 100

/*
#define MODEL_ADDR      0x80020000
#define TOD_ADDR        0x80040000
#define TEX_ADDR        0x80070000
*/

#define MODEL_ADDR      0x800c0000
#define TOD_ADDR        0x800e0000

#define TIM_ID          0x10
#define ROT        3*ONE/360;   

#define OT_LENGTH  12           


int             _GmoveView = 0;  /* 0: éãì_TOD, 1: éãì_ëÄçÏ */
static int      GpauseMode = 0;       
GsOT            Wot[2];        

GsOT_TAG        zsorttable[2][1<<OT_LENGTH];

GsDOBJ5         _GobjTable[OBJECTMAX];
GsCOORDINATE2   _GcoordTable[OBJECTMAX];

u_long          _Gobjnum;         
u_long          *_GtmdAddr;
u_long          *_Goppp;  
GsCOORDINATE2   DWorld;  

SVECTOR         PWorld; 


extern MATRIX GsIDMATRIX;
extern MATRIX GsWSMATRIX;

typedef struct {
  SVECTOR vect;
  long tx,ty,tz;
} MyView;

MyView mview;
GsVIEW2 view;
GsF_LIGHT pslt[3]; 
u_long padd;                   

u_long _GPacketArea[0x10000];

u_long *SCRATCH  = (u_long *)0x1f800000;

u_char bg = 0x30;

u_long*  _GTodP;
int      _GTodNumFrame;
int      _GStartFrameNo;
u_long*  _GTmdP;

typedef struct {
	GsDOBJ5 *top;
	int nobj;
	int maxobj;
} GsOBJTABLE5;

void initTod();
void model_init();

void initMatrix(MATRIX *mat)
{
  int i,j;

  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      mat->m[i][j] = 0;
		
  mat->m[0][0] = ONE;
  mat->m[1][1] = ONE;
  mat->m[2][2] = ONE;

  mat->t[0] = 0;
  mat->t[1] = 0;
  mat->t[2] = 0;
}


u_long *SetTodPacket5(todp, tmdp, mode)
u_long *todp;
u_long *tmdp;
int mode;
{
  GsDOBJ5 *objp;
  GsDOBJ5 *pobj;
  GsCOORD2PARAM *cparam;
  MATRIX *coordp;
  u_long *dp0;
  u_long hdr, id, flag, type, len;
  VECTOR v;
  SVECTOR sv;
  GsDOBJ5 dummy;
  GsCOORD2PARAM dmy_cparam;
  MATRIX dmy_coordp, matrix;

	dp0 = todp;

	hdr = *dp0++;

	id = hdr&0x0ffff;       /* ID for 3D-Object */
	type = (hdr>>16)&0x0f;  /* Packet type (GsTOD_???) */
	flag = (hdr>>20)&0x0f;  /* Flags */
	len = (hdr>>24)&0x0ff;  /* Packet length (word) */

	objp = ((GsDOBJ5*)_GobjTable)+id;

	if(objp == NULL) {
		objp = &dummy;
		coordp = &dmy_coordp;
		cparam = &dmy_cparam;
	}
	else {
		coordp = &(objp->coord2->coord);
		cparam = (objp->coord2->param);
		objp->coord2->flg = 0;
	}
	switch(type) {

	case GsTOD_MATRIX:
	       coordp->m[0][0]= (short)(*dp0 & 0x0000ffff);
	       coordp->m[0][1]= (short)(((*dp0++) & 0xffff0000) >>16);

	       coordp->m[0][2]= (short)(*dp0 & 0x0000ffff);
	       coordp->m[1][0]= (short)(((*dp0++) & 0xffff0000) >>16);

	       coordp->m[1][1]= (short)(*dp0 & 0x0000ffff);
	       coordp->m[1][2]= (short)(((*dp0++) & 0xffff0000) >>16);

	       coordp->m[2][0]= (short)(*dp0 & 0x0000ffff);
	       coordp->m[2][1]= (short)(((*dp0++) & 0xffff0000) >>16);

	       coordp->m[2][2]= (short)(((*dp0++) & 0x0000ffff));

	       coordp->t[0]= *dp0++;
	       coordp->t[1]= *dp0++;
	       coordp->t[2]= *dp0++;

	       break;

       case GsTOD_ATTR:
		/* Modify Attribute */
		objp->attribute = (objp->attribute&*dp0)|*(dp0+1);      
		dp0 += 2;
		break;
       case GsTOD_COORD:
		/* Set Coordinate value */
		if(flag&0x01) {
		  if(flag&0x02) {
			  cparam->rotate.vx += (*(((long *)dp0)+0))/360;
				cparam->rotate.vy += (*(((long *)dp0)+1))/360;
				cparam->rotate.vz += (*(((long *)dp0)+2))/360;
				dp0 += 3;
			}
			if(flag&0x04) {
				cparam->scale.vx
				 = (cparam->scale.vx**(((short *)dp0)+0))/4096;
				cparam->scale.vy
				 = (cparam->scale.vy**(((short *)dp0)+1))/4096;
				cparam->scale.vz
				 = (cparam->scale.vz**(((short *)dp0)+2))/4096;
				dp0 += 2;
			}
			if(flag&0x08) {
				cparam->trans.vx += *(((long *)dp0)+0);
				cparam->trans.vy += *(((long *)dp0)+1);
				cparam->trans.vz += *(((long *)dp0)+2);
				dp0 += 3;
			}

			initMatrix(coordp);
			RotMatrix(&(cparam->rotate), coordp); 

			ScaleMatrix(coordp, &(cparam->scale));
			TransMatrix(coordp, &(cparam->trans));
		}
		else { 
			if(flag&0x02) {
				cparam->rotate.vx = (*(((long *)dp0)+0))/360;
				cparam->rotate.vy = (*(((long *)dp0)+1))/360;
				cparam->rotate.vz = (*(((long *)dp0)+2))/360;
				dp0 += 3;
				/*******************/
				/* initMatrix(coordp); */

				RotMatrix(&(cparam->rotate), coordp);
			}
			if(flag&0x04) {
				cparam->scale.vx = *(((short *)dp0)+0);
				cparam->scale.vy = *(((short *)dp0)+1);
				cparam->scale.vz = *(((short *)dp0)+2);
				dp0 += 2;
				if(!(flag&0x02))
			/* MyRotMatrixYZX(&(cparam->rotate), coordp); */
				  RotMatrix(&(cparam->rotate), coordp);
				ScaleMatrix(coordp, &(cparam->scale));
			}
			if(flag&0x08) {
				cparam->trans.vx = *(((long *)dp0)+0);
				cparam->trans.vy = *(((long *)dp0)+1);
				cparam->trans.vz = *(((long *)dp0)+2);
				dp0 += 3;
				TransMatrix(coordp, &(cparam->trans));
			}
		}
		break;
	    case GsTOD_TMDID:
		/* Set TMD pointer */
		if(tmdp != NULL) {
		  GsLinkObject5((u_long)_GtmdAddr, objp, (*dp0&0xffff)-1);
		  _Goppp = GsPresetObject(objp, _Goppp);
				
		}
		break;

	    case GsTOD_PARENT:
		/* Set Parent Object */
		if(mode != GsTOD_COORDONLY) {
			if((*dp0 == NULL)||(*dp0 == 0xffff)) {
				objp->coord2->super = NULL;
				dp0++;
			}
			else {
				pobj = ((GsDOBJ5*)_GobjTable)+(*dp0++);
				objp->coord2->super = pobj->coord2;
			}
		}
		break;

	     case GsTOD_OBJCTL:
		break;

	     case GsTOD_CAMERA:
	       if (!_GmoveView)
	       {
		 GsRVIEW2 rview2;

		 /* flag Çñ≥éã */
		 rview2.vpx = (long)(*dp0++);
		 rview2.vpy = (long)(*dp0++);
		 rview2.vpz = (long)(*dp0++);
		 rview2.vrx = (long)(*dp0++);
		 rview2.vry = (long)(*dp0++);
		 rview2.vrz = (long)(*dp0++);
		 rview2.rz =  (long)0;
		 
		 dp0++;
		 GsSetRefView2(&rview2);
	       }
	       break;
	    case GsTOD_USER0:
		break;
	     }
	return todp+len;
}


u_long *SetTodFrame5(fn, tfn, todp, tmdp, mode)
int fn;
int *tfn;
u_long *todp;
u_long *tmdp;
int mode;
{
	GsDOBJ5 *objp;
	GsDOBJ5 *pobj;
	u_long hdr, flen, npacket, frame;
	int i;
	
	hdr = *todp;    
	/* flen = hdr&0x0ffff; */
	npacket = (hdr>>16)&0x0ffff;
	frame = *(todp+1);

	if(frame != fn)
	  {
	    return todp;
	  }

	(*tfn)++;
	todp += 2;
	for(i = 0; i < npacket; i++) {
		todp = SetTodPacket5(todp, tmdp, mode);
	}

	return todp;
}


/************* MAIN START ******************************************/
main()
{
  int id;
  int     i, frame, todFrame;
  GsDOBJ5 *op;                 
  int     outbuf_idx;
  MATRIX  tmpls;
  u_long  *todHead, *todP;
  init_all();
  
  /* init font environment */
  FntLoad(960, 256);
  id = FntOpen(32, 32, 320, 440, 0, 512); 
  SetDumpFnt(id);
  
  printf("[K\rPSX>");  

  while(1)
    {
      todP = _GTodP;

      todFrame = 0;
      _Goppp = _GPacketArea;       /* reset packet area */

      VSyncN(2); 
      for(frame=0; todFrame < _GTodNumFrame;) {
	obj_interactive();

	if (_GmoveView)
	  setMyView(&mview, &view);

	
	outbuf_idx=GsGetActiveBuff();
	GsClearOt(0,0,&Wot[outbuf_idx]); 
	
	todP = SetTodFrame5
	  (frame,
	   &todFrame,
	   todP,        
	   _GTmdP,      
	   GsTOD_CREATE );
	
	for(i=0,op=_GobjTable+1; i<_Gobjnum; i++) 
	  {
	    u_long *tmd;
	    
	    GsGetLw(op->coord2,&tmpls);
	    GsSetLightMatrix(&tmpls);
	    GsGetLs(op->coord2,&tmpls);
	    GsSetLsMatrix(&tmpls);

	    tmd = op->tmd;
	    
	    GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,SCRATCH);
	    op++;
	  }
	
	FntPrint(id, "Frame = %d, %d, %d\n",
		 todFrame, frame, _GTodNumFrame);
	FntFlush(id);
	
	padd=PadRead(1);          
	VSync(0);                 
	ResetGraph(1);            
	GsSwapDispBuff();         
	SetDispMask(1);
	
	GsSortClear(bg,bg,bg,&Wot[outbuf_idx]);
	
	GsDrawOt(&Wot[outbuf_idx]);

	switch(GpauseMode) {
	case 1:
	  break;
	  
	case 0:
	case 2:
	  frame++;
	  break;
	  
	case 3:
	case 4:
	  default:
	}


      }
    }
}

VSyncN(n)
     int n;
{
  while(n-- > 0) 
    VSync(0);
}


obj_interactive()
{
  SVECTOR v1;
  MATRIX  tmp1;
  static int mvcount;

  if (_GmoveView) {

    if((padd & PADRleft)>0) PWorld.vy -=ROT;
    if((padd & PADRright)>0) PWorld.vy +=ROT;
    if((padd & PADRup)>0) PWorld.vx+=ROT;
    if((padd & PADRdown)>0) PWorld.vx-=ROT;
    
    if((padd & PADl)>0)
      mview.tz -= 10;
    if((padd & PADm)>0) 
      mview.tz += 10;
    
    if((padd & PADLleft)>0)
      mview.tx  -= 10;
    if((padd & PADLright)>0)
      mview.tx +=  10;
    if((padd & PADLdown)>0)
      mview.ty -=  10;
    if((padd & PADLup)>0)
      mview.ty +=  10;
    
    if((padd & PADRleft)>0)
      mview.vect.vy -= 20;
    
    if((padd & PADRright)>0)
      mview.vect.vy += 20;
    
    if((padd & PADRdown)>0)  
      mview.vect.vx -= 20;
    
    if((padd & PADRup)>0)   
      mview.vect.vx += 20;
  }

  /* change bg color */
  if((padd & PADn)>0) bg = (bg+1)%256;
  if((padd & PADo)>0) bg = (bg-1)%256;


  if((padd & PADk)>0  &&  (padd & PADh)) { 
	  RECT rect;
	  int i;
	  setRECT(&rect, 0, 0, 640, 480);
	  ClearImage(&rect, 0x0, 0x0, 0x0);
	  for (i = 0; i<10000; i++);
	  ResetGraph(1);
	  PadStop();
	  exit();
  }


  if((padd & PADl)>0 && (padd & PADm)>0)
    mvcount++;
  
  if (mvcount > 10) {
    mvcount = 0;
    _GmoveView = !(_GmoveView);
  }


  switch(GpauseMode) {
  case 0: {
    if ((padd & PADk)>0) {
      GpauseMode = 1;
     /* VSyncN(60); */
    }
    break;
   }

  case 1: {
    if ((padd & PADk)>0)
      GpauseMode = 2;
    else 
      GpauseMode = 1;
    break;
    }

  case 2: {
    if ((padd & PADk)>0)
      GpauseMode = 3;
    else
      GpauseMode = 4;
    break;
   }
    
  case 3: {
    if ((padd & PADk)>0)
      GpauseMode = 3;
    else
      GpauseMode = 4;
    break;
   }

  case 4: {
    if ((padd & PADk)>0)
      GpauseMode = 2;
    else
      GpauseMode = 4;
    break;
   }
    
  }
    
  if ((padd & PADh)>0) 
    GpauseMode = 0;

  set_coordinate(&PWorld,&DWorld);
  
  DWorld.flg = 0;
}

init_all()                      
{


  ResetGraph(0);                
  PadInit(0);                   
  padd=0;                       

  GsInitGraph(640,240,0,1,0);   
  GsDefDispBuff(0,0,0,240);     
  GsInit3D();                   
  
  Wot[0].length=OT_LENGTH;      
  Wot[0].org=zsorttable[0];     
				
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  
  coord_init();     
  model_init();     
  view_init();      
  light_init();     
  initTod();

  /* load_textures(TEX_ADDR);  */
}


void initTod()
{
  u_long *addr;
  int   n;

  _GTodP = (u_long *)TOD_ADDR;

  _GTodP++;
  _GTodNumFrame = *_GTodP++;
  n = _GTodNumFrame;
  _GStartFrameNo = *(_GTodP+1);
  n = _GTodNumFrame;
}

setMyView(MyView *mv, GsVIEW2 *v)
{
  RotMatrix(&(mv->vect), &(v->view));

  v->view.m[1][0] = (v->view.m[1][0]/2);
  v->view.m[1][1] = (v->view.m[1][1]/2);
  v->view.m[1][2] = (v->view.m[1][2]/2);
  v->view.t[0] = mv->tx;
  v->view.t[1] = mv->ty;
  v->view.t[2] = mv->tz;

  GsSetView2(v);
}

view_init()                     
{
  SVECTOR vct;

  GsSetProjection(1000);       


  mview.vect.vx = 0;
  mview.vect.vy = (short)(ONE/2);
  mview.vect.vz = 0;
  mview.tx = 0;
  mview.ty = 0;
  mview.tz = 2000;

  setMyView(&mview, &view); 
  GsSetNearClip(100);          
}



light_init() 
{
  pslt[0].vx = -10;  pslt[0].vy= 100; pslt[0].vz= 0;
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  GsSetFlatLight(0,&pslt[0]);

  pslt[1].vx = 0; pslt[1].vy=  50; pslt[1].vz= 100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  pslt[2].vx = 0; pslt[2].vy= 50; pslt[2].vz= -100;
  pslt[2].r=0x20; pslt[2].g=0x20; pslt[2].b=0x20;
  GsSetFlatLight(2,&pslt[2]);
  
  GsSetAmbient(20, 20, 20);
  GsSetLightMode(0);
}

coord_init()                   
{

  GsInitCoordinate2(WORLD,&DWorld);
  PWorld.vx=PWorld.vy=PWorld.vz=0;
  DWorld.coord.t[2] = -4000;
}



set_coordinate(pos,coor)        
SVECTOR *pos;                   
GsCOORDINATE2 *coor;          
{
  MATRIX tmp1;
  SVECTOR v1;
  
  tmp1   = GsIDMATRIX;          
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];

  v1 = *pos;

  RotMatrix(&v1,&tmp1);
  coor->coord = tmp1;
  coor->flg = 0;
}    


texture_init(addr)              
u_long addr;
{
  RECT rect1;
  GsIMAGE tim1;
  
  GsGetTimInfo((u_long *)(addr+4),&tim1); 
  rect1.x=tim1.px; 
  rect1.y=tim1.py; 
  rect1.w=tim1.pw; 
  rect1.h=tim1.ph; 
  LoadImage(&rect1,tim1.pixel); 
  
  if((tim1.pmode>>3)&0x01)      
    {
      rect1.x=tim1.cx;  
      rect1.y=tim1.cy;          
      rect1.w=tim1.cw;          
      rect1.h=tim1.ch;          
      LoadImage(&rect1,tim1.clut);
    }
}

load_textures(addr)
u_long *addr;
{
	RECT rect1;
	GsIMAGE tim1;
	u_short *ap;

	while(*addr == TIM_ID) {
		addr++;

		GsGetTimInfo(addr, &tim1);

		if((tim1.pmode>>3)&0x01) {
			rect1.x=tim1.cx;
			rect1.y=tim1.cy;
			rect1.w=tim1.cw;
			rect1.h=tim1.ch;
			LoadImage(&rect1,tim1.clut);
			addr += tim1.cw*tim1.ch/2+3;
		}
		rect1.x=tim1.px;
		rect1.y=tim1.py;
		rect1.w=tim1.pw;
		rect1.h=tim1.ph;
		LoadImage(&rect1,tim1.pixel);
		ap = (u_short*)(addr+1);
		ap += tim1.pw*tim1.ph+6;
		addr = (u_long*)ap;
		printf("\n"); 
	}
}

void
model_init()
{
  u_long *dop;
  GsDOBJ5 *objp; 
  GsCOORDINATE2 *coordp;
  int i;
  
  dop=(u_long *)MODEL_ADDR;     
  dop++;         
  
  GsMapModelingData(dop);
  _GTmdP = dop;

  dop++;
  _Gobjnum = *dop;       
  dop++;                 

  _GtmdAddr = dop;

  if (_Gobjnum == 0) return;
  GsLinkObject5((u_long)dop,&_GobjTable[0],0);

/*
  for(i=0;i<_Gobjnum;i++)
    GsLinkObject5((u_long)dop,&_GobjTable[i+1],i);
*/

  _Goppp = _GPacketArea;          
  objp = _GobjTable;
  coordp = _GcoordTable;

  /* for(i=0; i<_Gobjnum+1+1; i++)  */

  for(i=0; i<OBJECTMAX; i++)  /* original */
    {                           
      objp->coord2 =  coordp;
      objp->attribute = 0;
      objp->tmd = NULL;
      /* oppp = GsPresetObject(objp,oppp); */
      objp++;
      coordp++;
    }
}


/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	
 *	"tuto1.c" main routine
 *
 *		Version 1.00	May. 29, 1995
 *		Version 1.01	Mar.  5, 1997	sachiko	(autopad)
 *
 *	Copyright (C) 1995 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
/*
 *	fog texture changing CLUT */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>


#define OT_LENGTH  10	/* resolution of OT */
#define PROJECTION 300

#define BGR 100					/* BG color Red */
#define BGG 100					/* BG color Green */
#define BGB 100					/* BG color Blue */
void draw_poly(GsCOORDINATE2 *co, int idx, GsOT_TAG *org);
int obj_interactive(u_long fn, u_long padd);
/* void main(); */

GsOT            Wot[2];	/* OT handler */
GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* OT */

#define HNUM 5
#define WNUM 5

POLY_FT4 ft4[2][HNUM][WNUM];			/* polygon */
SVECTOR fpos[HNUM][WNUM][4];			/* polygon vertex positions */

GsCOORDINATE2   DWorld;		/* object coordinate */
SVECTOR PWorld;			/* rotation vector */
extern MATRIX GsIDMATRIX;	/* Unit Matrix */

RECT rect1;
CVECTOR cv1[256];				/* original clut */
CVECTOR bgc;					/* background color */
short clutw;

int  outbuf_idx=0;

#define CBIT 6
#define CNUM (1<<CBIT)
u_short fclut[CNUM];

GsIMAGE tim1;
long fognear, fogfar;

/************* MAIN START ******************************************/
main()
{
    u_long fn;
    u_long padd=0;	
    int i,j;

    ResetCallback();
    PadInit(0);			/* initialize controller */
    init_all();
/*    SetFogNear(8000, 5000);	/* set fog parameter */
    fognear=1000;
    fogfar=4000;
    SetFogNearFar(fognear, fogfar, PROJECTION);	/* set fog parameter */
    bgc.r=BGR; bgc.g=BGG; bgc.b=BGB;

    /* main loop */
    for (fn=0;;fn++){
	outbuf_idx=GsGetActiveBuff();		/* get one of buffers */

	if (obj_interactive(fn, padd)<0) return; /* pad data handling */

	GsClearOt(0,0,&Wot[outbuf_idx]);	/* clear ordering table */

	/* draw polygon(s) */
	draw_poly(&DWorld, outbuf_idx, Wot[outbuf_idx].org);

	padd=PadRead(1);			/* read pad */

	DrawSync(0);				/* wait drawing done */

	VSync(0);				/* wait Vertical Sync */

	GsSwapDispBuff();			/* switch double buffers */
	
	/* BG auto clear enable */
	GsSortClear(BGR,BGG,BGB,&Wot[outbuf_idx]); 
/*	GsSortClear(0,0,0,&Wot[outbuf_idx]); */

	/* draw primitives on OT */
	GsDrawOt(&Wot[outbuf_idx]);
	FntFlush(-1);				/* draw print-stream */
    }
    return;
}

/* draw polygon(s) */
void draw_poly(GsCOORDINATE2 *co, int idx, GsOT_TAG *org)
{
    int i,j;
    long otz, flag;
    MATRIX tmpls;
    POLY_FT4 *f;
    long nclip;
    long p;

    /* update coordinate and set to GTE */
    GsGetLs(co, &tmpls);
    GsSetLsMatrix(&tmpls);

    f= &ft4[idx][0][0];
    for (i=0; i<HNUM; i++){
	for (j=0; j<WNUM; j++){
	    otz=RotAverage4(&fpos[i][j][0], &fpos[i][j][1], &fpos[i][j][2], &fpos[i][j][3],
			    (long *)(&f->x0),
			    (long *)(&f->x1),
			    (long *)(&f->x2),
			    (long *)(&f->x3),
			    &p, &flag
			    );
/*	    FntPrint("p=%d, otz=%d, flag=%X\n",p,otz, flag);*/
	    if (flag >=0				/* no error */
		&& otz>0				/* otz OK */
		&& p<4096){			/* depth queue is not full */
		f->clut=fclut[p>>(12-CBIT)];
		AddPrim(org+(otz>>(14-OT_LENGTH)), f); /* set f to OT */
	    }
	    f++;
	}
    }
}

/* make depth queued clut */
make_clut(RECT *rtim, u_short *ctim, u_short *cp, long num, CVECTOR *bgc)
{
    long r,g,b,stp;
    long p;
    long i,j;
    u_short newclut[256];
    RECT rect;

    rect.x=rtim->x;
    rect.w=rtim->w;
    rect.h=rtim->h;

    for (i=0; i<num; i++){
	p=i*ONE/num;

	for (j=0; j<rtim->w; j++){
	    if (ctim[j]==0){	/* skip if transparent */
		newclut[j]=ctim[j];
	    } else{
		/* get RGB color from CLUT */
		r= (ctim[j] & 0x1f)<<3;
		g= ((ctim[j] >>5) & 0x1f)<<3;
		b= ((ctim[j] >>10) & 0x1f)<<3;
		stp= ctim[j] & 0x8000;

		/* interpolate between orginal color and background color */

		r= (((long)((u_long)r*(4096-p))) + (((u_long)bgc->r * p))>>15);
		g= (((long)((u_long)g*(4096-p))) + (((u_long)bgc->g * p))>>15);
		b= (((long)((u_long)b*(4096-p))) + (((u_long)bgc->b * p))>>15);
		
		/* make depth queued CLUT */
		newclut[j]= stp |r| (g<<5) | (b<<10);
	    }
	}
	rect.y=rtim->y+i;

	/* load to the frame buffer */
	LoadImage(&rect,(u_long *)newclut);
	cp[i]= GetClut(rect.x, rect.y); 
    }
}

/* pad data handling */
int obj_interactive(u_long fn, u_long padd)
{
#define DT 30
#define VT 30
#define ZT 100
#define FT 10
    static u_long oldpadd=0;

    if (padd & PADk){
#if 0
	if (padd & PADLdown) fognear-=FT;
	if (padd & PADLup) fognear+=FT;
	if (fognear<0) fognear=0;
	SetFogNearFar(fognear, fogfar, PROJECTION);
#endif /* 0 */
    } else if (padd & PADh){
#if 0
	if (padd & PADLdown) fogfar-=FT;
	if (padd & PADLup) fogfar+=FT;
	if (fogfar<(fognear*2)) fogfar=fognear*2;
	SetFogNearFar(fognear, fogfar, PROJECTION);
#endif /* 0 */
    } else{				/* translate and rotate */
	if (padd & PADLleft) DWorld.coord.t[0]-=DT;
	else if (padd & PADLright) DWorld.coord.t[0]+=DT;
	
	if (padd & PADLup) DWorld.coord.t[1]-=DT;
	else if (padd & PADLdown) DWorld.coord.t[1]+=DT;
	
	if (padd & PADo) DWorld.coord.t[2]+=ZT;
	else if (padd & PADn) DWorld.coord.t[2]-=ZT;
	
	if (padd & PADRleft) PWorld.vy-=VT;
	else if (padd & PADRright) PWorld.vy+=VT;
	
	if (padd & PADRup) PWorld.vx+=VT;
	else if (padd & PADRdown) PWorld.vx-=VT;
	
	if (padd & PADm) PWorld.vz-=VT;
	else if (padd & PADl) PWorld.vz+=VT;
    }

    if (padd & PADselect){
	/* exit program */
	PadStop();
	ResetGraph(3);
	StopCallback();
	return -1;
    }
/*    FntPrint("%d,%d\n",fognear, fogfar);*/

    set_coordinate(&PWorld,&DWorld);
    oldpadd=padd;
    return 0;
}

init_all()			/* initialize */
{
    ResetGraph(0);		/* initialize GPU */

    draw_init();
    coord_init();
    view_init();
    fnt_init();
    texture_init((u_long *)0x80100000);
    poly_init();
}

poly_init()
{
    int i,j,k;
    POLY_FT4 *f;
    int z;

    f= &ft4[0][0][0];
    for (k=0; k<2; k++){
	for (i=0; i<HNUM; i++){
	    for (j=0; j<WNUM; j++){
		setPolyFT4(f);			/* initialize primitive */
		SetShadeTex(f,1);			/* shading off */
		setUV4(f,
		       0, 0,
		       255, 0,
		       0, 255,		       
		       255, 255
		       );			/* set texture position */
		f->clut=GetClut(tim1.cx, tim1.cy); /* clut (temporary) */
		/* texture page */
		f->tpage=GetTPage(tim1.pmode,0,tim1.px,tim1.py);
		f++;
	    }
	}
    }
#define SIDELONG 100
#define WSPACE 500
#define HSPACE 500
#define WOFFSET (-(WNUM/2) * WSPACE )
#define HOFFSET (-(HNUM/2) * HSPACE )
#define ZOFFSET 0
#define ZSPACE 500

	    /* set vertex positions */
    for (i=0; i<HNUM; i++){
	for (j=0; j<WNUM; j++){
	    z=abs(i-(HNUM/2))+abs(j-(HNUM/2));
	    fpos[i][j][0].vx= (WSPACE*j)+ WOFFSET-SIDELONG;
	    fpos[i][j][0].vy= (HSPACE*i)+ HOFFSET-SIDELONG;
	    fpos[i][j][0].vz= (ZSPACE*z)+ ZOFFSET;
	    fpos[i][j][1].vx= (WSPACE*j)+ WOFFSET+SIDELONG;
	    fpos[i][j][1].vy= (HSPACE*i)+ HOFFSET-SIDELONG;
	    fpos[i][j][1].vz= (ZSPACE*z)+ ZOFFSET;
	    fpos[i][j][2].vx= (WSPACE*j)+ WOFFSET-SIDELONG;
	    fpos[i][j][2].vy= (HSPACE*i)+ HOFFSET+SIDELONG;
	    fpos[i][j][2].vz= (ZSPACE*z)+ ZOFFSET;
	    fpos[i][j][3].vx= (WSPACE*j)+ WOFFSET+SIDELONG;
	    fpos[i][j][3].vy= (HSPACE*i)+ HOFFSET+SIDELONG;
	    fpos[i][j][3].vz= (ZSPACE*z)+ ZOFFSET;
	}
    }
}


draw_init()
{
    /* set resolution */
    GsInitGraph(640,240,GsOFSGPU|GsINTER,1,0);	
    
    /* initialize double buffer */
    GsDefDispBuff(0,0,0,240);	
    
    /* reset 3D system */
    GsInit3D();			

    /* set OT resolution */
    Wot[0].length=OT_LENGTH;	
    Wot[1].length=OT_LENGTH;
    
    /* set OT on OT handler(0) */
    Wot[0].org=zsorttable[0];	
    Wot[1].org=zsorttable[1];
}

fnt_init()
{
    FntLoad(960, 256);				/* font load */
    SetDumpFnt(FntOpen(-290,-100,400,100,0,200)); /* stream open & define */
}


int view_init()
{
    GsRVIEW2  view;		/* View Point Handler*/

    /* Set projection */
    GsSetProjection(PROJECTION);
  
    /* Setting view point location */
    view.vpx = 0; view.vpy = 0; view.vpz = -2000;
  
    /* Setting focus point location */
    view.vrx = 0; view.vry = 0; view.vrz = 0;
  
    /* Setting bank of SCREEN */
    view.rz=0;

    /* Setting parent of viewing coordinate */
    view.super = WORLD;
  
    /* Calculate World-Screen Matrix from viewing paramter */
    GsSetRefView2(&view);
  
    /* Set Near Clip */
    GsSetNearClip(100);           
}

int coord_init()
{
    /* initialize coordinates */
    GsInitCoordinate2(WORLD,&DWorld);
    DWorld.coord.t[2]= -1000;

    /* set the rotation vector for rot-trans matrix */
    PWorld.vx=PWorld.vy=PWorld.vz=0;
    set_coordinate(&PWorld,&DWorld);
}

/* make rot-trans matrix from the rotation vector */
int set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor)
{
    MATRIX tmp1;
    SVECTOR v1;
    int i,j;

    /* start from th unit matrix */
    tmp1   = GsIDMATRIX;	

    /* set translation vectro */
    tmp1.t[0] = coor->coord.t[0];
    tmp1.t[1] = coor->coord.t[1];
    tmp1.t[2] = coor->coord.t[2];

    /* make rotaion matrix and hook to object handler */
    v1 = *pos;
    RotMatrix(&v1,&tmp1);
    coor->coord = tmp1;

    /* flush matrix cache */
    coor->flg = 0;
}

texture_init(u_long *addr)
{
    RECT rect1;
    CVECTOR bgc;

    /* Get texture information of TIM FORMAT */  
    GsGetTimInfo(addr+1,&tim1);
  
    LoadTPage(tim1.pixel, tim1.pmode&7, 0, tim1.px, tim1.py, 256, 256);

/*    printf("tim1.pw=%d, tim1.ph=%d\n", tim1.pw, tim1.ph);*/
    rect1.x=640; rect1.y=256; rect1.w=256; rect1.h=1;
    bgc.r=BGR;
    bgc.g=BGG;
    bgc.b=BGB;
    make_clut(&rect1, (u_short *)tim1.clut, (u_short *)fclut, CNUM, &bgc);
}

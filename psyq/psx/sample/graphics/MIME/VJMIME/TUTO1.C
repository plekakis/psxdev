/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	model.c
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "common.h"
#include "model.h"
#include "preset.h"
#include "control.h"
#include "paddef.h"
#include "ctlfc.h"
#include "a_intr.h"
#include "rpy_intr.h"

/* packet buffer area for GsSort*() */
#define PACKETBUF  ((u_long *)0x80600000)

/* TMD data addresses */
#define TMD_ADDR ((u_long *)0x80100000)
/* no. of vertices includes each part*/
long vtxnums[PARTNUM]={8,8,8};
/* max number of vertex per part */
#define VNUMMAX 8
/* saved location of src vertex data*/
SVECTOR orgvtx[PARTNUM][VNUMMAX];


#define CURSLEN 600

typedef struct{
    long n_vertex;		/* number of vertex data*/
    SVECTOR *vtxsrc;		/*  does vertex data exist?*/
    SVECTOR *vtxdest;		/* place for updated vertex data*/
} VHAND;

#define MAXOBJ 1
typedef struct {
    u_long header;
    u_long flags;
    u_long nobj;
    struct TMD_STRUCT objs[MAXOBJ];
} TMDHEADER;


/* menu for choosing interpolation method */
#define INTRNUM 2
typedef struct {
    char dispintrmenu[25];			/* display message */
    void (*start_func)();			/* starting function  */
    void (*intrpol_func)(long *);		/* function for interpolating*/
    void (*reset_func)();			/* reset function */
} INTRMENU;

static INTRMENU intrmenu[/* INTRNUM */]={
    {
	"axes MIMe",
	axis_interpolate_start,
	axis_interpolate,
	axis_interpolate_reset,
    },
    {
	"RPY angle MIMe",
	rpy_interpolate_start,
	rpy_interpolate,
	rpy_interpolate_reset,
    },
};

int ARR_MIME[MIMENUM]={
    ARR_MIME1, ARR_MIME2, ARR_MIME3, ARR_MIME4,
};

/* preset angle states */
static int presetmenu=0;		/* preset number in use*/
static int presetchangep=0;		/* change from preset state?*/
static int presetnum;			/* total number of preset angles */


/* number of parent part (-1: WORLD) */
static int parent_coord[/* PARTNUM */]={
    -1, 0, 1,
};

/* display area offset */
static DVECTOR geomoffset[/* DISPNUM */]={
    {-300,0},				/* ARR_BASE (base arrangement) */
    {100,-90},				/* ARR_MIME1(mime arrangement #1)*/
    {100,-30},				/* ARR_MIME2(mime arrangement #2)*/
    {100,30},				/* ARR_MIME3(mime arrangement #1)*/
    {100,90},				/* ARR_MIME4(mime arrangement #2)*/
    {-100,0},				/* ARR_INTR (interpolated arrangement)*/
};

/* MODEL data handler */
MODEL model_hand[DISPNUM][PARTNUM];

/* VERTEX data handler */
static VHAND vhand[PARTNUM];

/* current operating model/part */
static short targetpart, targetmodel;

GsDOBJ5 object[MAXOBJ*DISPNUM];	/* Array of Object Handler*/
static u_long Objnum;

static GsCOORDINATE2 DWorld;
static GsCOORDINATE2 VWorld;
static SVECTOR PWorld;
static GsLINE cursor[3];

/* function declarations */
void model(GsOT *wot, int otlen);
static void sort_cursor(GsOT *wot, int otlen);
int model_init(void);
static void model_packet_init(void);
static void model_hand_init(void);
static void model_coord_init(void);
static int model_vhand_init(void);
static int sub_model_init(u_long *addr, u_long **opppp, GsDOBJ5 **objpp, GsCOORDINATE2 *co, u_long *on);
void texture_init(int tnum, u_long *addr[]);
static void sub_texture_init(u_long *addr);
int model_move(u_long padd);
void ch_angle(int arr, int obj, int var, int value);
void set_intr_to_base(void);
void marume(SVECTOR *sv);
void dumpAngles(void);
void set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor);
void dumpParams(void);
void model_init_params(void);
void cursor_init(void);

/***********************************************
 *	 add entries to OT 
 ***********************************************/
void model(GsOT *wot, int otlen)
{
    int i,j,k;
    MATRIX  tmpls;
    GsDOBJ5 *op;			/* pointer of Object Handler*/
    MODEL *mhand;

    op= &object[0];
    for (i=0; i<DISPNUM; i++){
	for (j=0; j<PARTNUM; j++){
	    mhand= &model_hand[i][j];

	    GsGetLw(&mhand->modelcoord, &tmpls);
	    GsSetLsMatrix(&tmpls);

	    for (k=0; k<vhand[j].n_vertex; k++){
		long flag;
						/* Joint MIMe */

		RotTransSV(&(vhand[j].vtxsrc)[k],&(vhand[j].vtxdest)[k], &flag);
	    }
	}

	SetGeomOffset(geomoffset[i].vx, geomoffset[i].vy);
	for (k=0;k<Objnum;k++, op++){
	    /* Calculate Local-World Matrix */
	    GsGetLw(op->coord2,&tmpls);
		
	    /* Set LWMATRIX to GTE Lighting Registers */
	    GsSetLightMatrix(&tmpls);
		
	    /* Calculate Local-Screen Matrix */
	    GsGetLs(op->coord2,&tmpls);
		
	    /* Set LSAMTRIX to GTE Registers */
	    GsSetLsMatrix(&tmpls);

	    /* Perspective Translate Object and Set OT */
	    GsSortObject5(op,&wot[i],14-otlen,0);
	}
    }
    
    sort_cursor(wot, otlen);

    dumpParams();
}

static void sort_cursor(GsOT *wot, int otlen)
{
    int j;
    MATRIX tmpls;
    int m;

    /* draw axes of current handling part */
    GsInitCoordinate2(&DWorld, &VWorld);
    PSDCNT++;
    if (PSDCNT==0) PSDCNT=1;
    /* Calculate Local-Screen Matrix */
    VWorld.flg=0;
    if (targetmodel==ARR_ALL) m=ARR_BASE;
    else m=targetmodel;

    GsGetLs(&model_hand[m][targetpart].modelcoord,&tmpls);
    /* Set LSAMTRIX to GTE Registers */
    GsSetLsMatrix(&tmpls);
    SetGeomOffset(geomoffset[m].vx, geomoffset[m].vy);

    for (j=0; j<3; j++){
	long p, flg, otz;
	static SVECTOR v0={0,0,0};
	static SVECTOR v1={CURSLEN,0,0};
	static SVECTOR v2={0,CURSLEN,0};
	static SVECTOR v3={0,0,CURSLEN};

	otz=RotTransPers(&v0, (u_long *)&cursor[0].x0, &p, &flg);
	if (flg>=0){
	    cursor[1].x0=cursor[0].x0;
	    cursor[1].y0=cursor[0].y0;
	    cursor[2].x0=cursor[0].x0;
	    cursor[2].y0=cursor[0].y0;
	    RotTransPers3(
			  &v1, &v2, &v3,
			  (u_long *)&cursor[0].x1,
			  (u_long *)&cursor[1].x1,
			  (u_long *)&cursor[2].x1,
			  &p, &flg);
	    if (flg>=0){
		for (j=0; j<3; j++){
		    GsSortLine(&cursor[j],
			       &wot[m],
			       otz>>(14-otlen));
		}
	    }
	}
    }

    GsInitCoordinate2(WORLD, &VWorld);
}

/***********************************************
 *	initialize functions 
 ***********************************************/
int model_init(void)
{
    int i;

    /* TMD initialize for GsSortObject5() */
    model_packet_init();

    /* Gs Coordinates initialize */
    model_coord_init();

    /* initialize model handler (model_hand[][]) */
    model_hand_init();

    if (model_vhand_init()<0) return -1;

    /* count preset angles */
    for (presetnum=0; preset[presetnum][0][0].angle.vx>-10000; presetnum++)
	;
    printf("presetnum=%d\n",presetnum);

    /* set convolution function */
    init_cntrlarry(CNVNAME, CNVLEN);
    
    model_init_params();

    cursor_init();

    return 0;
}

void cursor_init(void)
{
    int i;

    cursor[0].r=255; cursor[0].g=0; cursor[0].b=0;
    cursor[1].r=0; cursor[1].g=255; cursor[1].b=0;
    cursor[2].r=0; cursor[2].g=0; cursor[2].b=255;
    for (i=0; i<3; i++){
	cursor[i].attribute=0;
    }
}

static int model_vhand_init(void)
{
    int i,j;
    SVECTOR *vt;
    u_long vtxtotal;

    vt=(SVECTOR *)(((TMDHEADER *)TMD_ADDR)->objs[0].vertop);
    vtxtotal=0;
    for (i=0; i<PARTNUM; i++){
	vhand[i].vtxsrc= &orgvtx[i][0];	/* set vertex address */
	vhand[i].vtxdest= vt+vtxtotal;
	vhand[i].n_vertex= vtxnums[i];		/* set vertex number */

	for (j=0; j<vhand[i].n_vertex; j++){
	    orgvtx[i][j]=vhand[i].vtxdest[j];
	}

	vtxtotal += vhand[i].n_vertex;
	if (vtxtotal>((TMDHEADER *)TMD_ADDR)->objs[0].vern){
	    printf("too many vertices. (%d)\n",vtxtotal);
	    return -1;
	}
    }

    return 0;
}

/* initialize model data */
static void model_packet_init(void)
{			

    GsDOBJ5 *objp;		/* object handler*/
    u_long *oppp;
    int i;
    static u_long *PacketArea;

    /* set packet*/
    PacketArea = PACKETBUF;

    oppp = PacketArea;	/* preset packet area*/
    for (i=0; i<DISPNUM; i++){
	objp= &object[i];
	/* init TMD data */
	sub_model_init(TMD_ADDR, &oppp, &objp, &DWorld, &Objnum);
    }

}

/* initialize model coordinate data */
static void model_coord_init(void)
{			
    int i,j;

    GsInitCoordinate2(WORLD, &DWorld);
    GsInitCoordinate2(WORLD, &VWorld);
    DWorld.coord.t[2]= 10000;
    PWorld.vx=512;
    PWorld.vy=0;
    PWorld.vz=0;
    set_coordinate(&PWorld, &DWorld);

    for (i=0; i<DISPNUM; i++){
	for (j=0; j<PARTNUM; j++){
	    if (parent_coord[j]<0){
		GsInitCoordinate2(&VWorld, &model_hand[i][j].modelcoord);
	    } else{
		GsInitCoordinate2(&model_hand[i][parent_coord[j]].modelcoord,
				  &model_hand[i][j].modelcoord);
	    }
	    
	}
    }
}

static int sub_model_init(u_long *addr, u_long **opppp, GsDOBJ5 **objpp, GsCOORDINATE2 *co, u_long *on)
{
    u_long *dop;
    GsDOBJ5 *objptmp;		/* object handler*/
    int i;
    u_long objntmp;


    dop=addr;			/* model data address */
    if (*dop!=0x41){
	printf("Illegal header TMD file!\n");
	return -1;
    }
    dop++;			/* hedder skip */

    GsMapModelingData(dop);	/* map the modeling data to read address */
    dop++;
    objntmp = *dop;		/* get the number of objects */
    if (objntmp >MAXOBJ ){
	printf("too many (%d) object. check MAXOBJ!\n", objntmp);
    }
    *on = objntmp;
    
    dop++;			/* inc the address to link to the handler */
    objptmp= *objpp;

    /* Link TMD data and Object Handler */
    for (i=0;i<objntmp;i++){
	GsLinkObject5((u_long)dop,objptmp,i);
	objptmp++;
    }

    for (i=0;i<objntmp;i++){
	/* default object coordinate */
	(*objpp)->coord2 = co;
	    
	/* make preset packet */
	*opppp = GsPresetObject(*objpp, *opppp);
	(*objpp)++;
    }

    return 0;
}


/* initialize model handler ( model_hand[][] ) */
static void model_hand_init(void)
{
    int i,j;
    MODEL *mhand;
    PRESET *pr;

    for (i=0; i<DISPNUM; i++){
	for (j=0; j<PARTNUM; j++){
	    mhand= &model_hand[i][j];
	    if (i==ARR_INTR){
		pr= &preset[presetmenu][ARR_BASE][j];
	    } else{
		pr= &preset[presetmenu][i][j];
	    }

	    mhand->angle=pr->angle;
	    mhand->modelcoord.coord.t[0]=pr->trans.vx;
	    mhand->modelcoord.coord.t[1]=pr->trans.vy;
	    mhand->modelcoord.coord.t[2]=pr->trans.vz;

	    set_coordinate(&mhand->angle, &mhand->modelcoord);
	}
    }
}

/***********************************************
 *	model handler operations
 ***********************************************/
#define CHROT 0
#define CHTRANS 1

#define MENUNUM (MIMENUM+1)
/* menu for change target models */
static short targetmenu[MENUNUM]={
    ARR_MIME1, ARR_MIME2, ARR_MIME3, ARR_MIME4, ARR_ALL
};

/* for display targetmodel */
static char *dispmenu[]={
    "MIMe1", "MIMe2", "MIMe3", "MIMe4", "ALL"
};

/* for dumpAngles() */
static char *dumpmenu[]={
    "Base", "MIMe1", "MIMe2", "MIMe3", "MIMe4"
};

static short rot_or_trans;
static short menu;
static short intrmenunum;
static INTRMENU *intrm;

void model_init_params(void)
{
    intrmenunum=0;
    intrm= &intrmenu[intrmenunum];
    intrm->start_func();
    menu=0;
    targetmodel=targetmenu[menu];
    rot_or_trans=CHROT;

    return;
}

/* pad operation */
int model_move(u_long padd)
{
    int i;
    
    static long mimepr[MIMENUM];	/* MIMe parameters*/
    static u_long oldpadd=0;
    static u_long fn=0;

    /* choose target to handle */
    if (padd & B_PADLcross){
	if (padd & B_PADLdown  & ~oldpadd) menu=(menu+1)%MENUNUM;
	if (padd & B_PADLup    & ~oldpadd) menu=(menu+MENUNUM-1)%MENUNUM;
	if (padd & B_PADLright & ~oldpadd) targetpart=(targetpart+1)%PARTNUM;
	if (padd & B_PADLleft  & ~oldpadd) targetpart=(targetpart+PARTNUM-1)%PARTNUM;
	targetmodel=targetmenu[menu];
    }

    if (padd & B_PADselect & ~oldpadd) rot_or_trans=CHTRANS;
    if (padd & B_PADstart  & ~oldpadd) rot_or_trans=CHROT;

    /* next preset angle set*/
    if (padd & (A_PADLright|A_PADLleft) & ~oldpadd){
	if (presetchangep) presetchangep=0;
	else{
	    if (padd&A_PADLright) presetmenu=(presetmenu+1)%presetnum;
	    if (padd&A_PADLleft ) presetmenu=(presetnum+presetmenu-1)%presetnum;
	}
	model_hand_init();
	intrm->start_func();
    }

    /* dump angles for "preset.c" */
    if (padd & A_PADselect & ~oldpadd) dumpAngles();

#define DT 10
#define RT 10

    /* rotate the world */
    if (padd&A_PADRcross){
	if (padd&A_PADRup   ) PWorld.vx+=RT;
	if (padd&A_PADRdown ) PWorld.vx-=RT;
	if (padd&A_PADRright) PWorld.vy+=RT;
	if (padd&A_PADRleft ) PWorld.vy-=RT;
	set_coordinate(&PWorld, &DWorld);
    }

    /* deform */
    if (padd & (B_PADRcross|B_PADR12)){
	switch (rot_or_trans){
	  case CHROT:
	    if (padd&B_PADR1    ) ch_angle(targetmodel, targetpart, VZ,  RT);
	    if (padd&B_PADR2    ) ch_angle(targetmodel, targetpart, VZ, -RT);
	    if (padd&B_PADRdown ) ch_angle(targetmodel, targetpart, VX,  RT);
	    if (padd&B_PADRup   ) ch_angle(targetmodel, targetpart, VX, -RT);
	    if (padd&B_PADRright) ch_angle(targetmodel, targetpart, VY,  RT);
	    if (padd&B_PADRleft ) ch_angle(targetmodel, targetpart, VY, -RT);
	    ch_angle(targetmodel, targetpart, ANG2MAT, 0);
	    break;
	  case CHTRANS:
	    if (padd&B_PADR1    ) ch_angle(targetmodel, targetpart, TZ, -DT);
	    if (padd&B_PADR2    ) ch_angle(targetmodel, targetpart, TZ,  DT);
	    if (padd&B_PADRdown ) ch_angle(targetmodel, targetpart, TY,  DT);
	    if (padd&B_PADRup   ) ch_angle(targetmodel, targetpart, TY, -DT);
	    if (padd&B_PADRright) ch_angle(targetmodel, targetpart, TX,  DT);
	    if (padd&B_PADRleft ) ch_angle(targetmodel, targetpart, TX, -DT);
	    ch_angle(targetmodel, targetpart, ANG2MAT, 0);
	    break;
	}
	intrm->start_func();
	presetchangep=1;
    }

    /* make MIMe parameters from pad data with convolution */
    if (padd&A_PADL1) ctlfc[0].in=4096;
    else ctlfc[0].in=0;
    if (padd&A_PADL2) ctlfc[1].in=4096;
    else ctlfc[1].in=0;
    if (padd&A_PADR1) ctlfc[2].in=4096;
    else ctlfc[2].in=0;
    if (padd&A_PADR2) ctlfc[3].in=4096;
    else ctlfc[3].in=0;
    set_cntrl(fn++);

    /* MIMe */
    for (i=0; i<MIMENUM; i++){
	mimepr[i]=ctlfc[i].out;	/* set MIMe parameter */
    }

    /* choose interpolation method menu */
    if (padd & A_PADstart & ~oldpadd){
	intrmenunum=(intrmenunum+1)%INTRNUM;
	intrm= &intrmenu[intrmenunum];
	intrm->start_func();
    }
    /* interpolation by {RPY angle or axes} MIMe */
    intrm->reset_func();
    intrm->intrpol_func(mimepr);

    oldpadd=padd;

    return 0;
}

void dumpParams(void)
{
    int i;

    FntPrint("%s\n",intrm->dispintrmenu);

    if (presetchangep){
	FntPrint("\n");
    } else{
	FntPrint("Preset #%d\n", presetmenu);
    }

    FntPrint("\n");
    FntPrint("%5s #%d / ", dispmenu[menu], targetpart);
    switch (rot_or_trans){
      case CHROT: FntPrint("Rotation\n"); break;
      case CHTRANS: FntPrint("Translation\n"); break;
    }

    FntPrint("\n");

    FntPrint("MIMe param:\n");
    for (i=0; i<MIMENUM; i++){
	FntPrint("\t[%d]=",i);
	FntPrint("%d\n",ctlfc[i].out);
    }

    return;
}

void ch_angle(int arr, int part, int var, int value)
{
    MODEL *mh;
    int i;

    if (arr==ARR_ALL){
	for (i=0; i<DISPNUM-1; i++){
	    ch_angle(i, part, var, value);
	}
    } else{
	switch (var){
	  case VX: model_hand[arr][part].angle.vx+=value; break;
	  case VY: model_hand[arr][part].angle.vy+=value; break;
	  case VZ: model_hand[arr][part].angle.vz+=value; break;
	  case TX: model_hand[arr][part].modelcoord.coord.t[0]+=value; break;
	  case TY: model_hand[arr][part].modelcoord.coord.t[1]+=value; break;
	  case TZ: model_hand[arr][part].modelcoord.coord.t[2]+=value; break;
	  case ANG2MAT:
	    mh= &model_hand[arr][part];
	    marume(&mh->angle);
	    set_coordinate(&mh->angle, &mh->modelcoord);
	    break;
	}
    }
}

/* copy BASE coordinates into INTR */
void set_intr_to_base(void)
{
    int i;

    for (i=0; i<PARTNUM; i++){
	model_hand[ARR_INTR][i].modelcoord.coord
	    = model_hand[ARR_BASE][i].modelcoord.coord;
	model_hand[ARR_INTR][i].modelcoord.flg=0;
    }
}

/***********************************************
 *	regularize angle (0-4095)*/
void marume(SVECTOR *sv)
{
#define MARUMEONE ((long)4096*2)
    sv->vx=((sv->vx+MARUMEONE)%4096);
    if (sv->vx>2048) sv->vx-=4096;
    sv->vy=((sv->vy+MARUMEONE)%4096);
    if (sv->vy>2048) sv->vy-=4096;
    sv->vz=((sv->vz+MARUMEONE)%4096);
    if (sv->vz>2048) sv->vz-=4096;
}
/* dump current model angles (for using in "preset.c") */
void dumpAngles(void)
{
    int m, part;

    printf("Angles:\n");
    printf("{\n");
    for (m=0; m<MIMENUM+1; m++){
	printf("{/* %s */\n",dumpmenu[m]);
	for (part=0; part<PARTNUM; part++){
	    MODEL *mh;

	    mh= &model_hand[m][part];
	    printf("{{%6d,%6d,%6d}, {%6d,%6d,%6d}},\n",
		   mh->angle.vx,
		   mh->angle.vy,
		   mh->angle.vz,
		   mh->modelcoord.coord.t[0],
		   mh->modelcoord.coord.t[1],
		   mh->modelcoord.coord.t[2]
		   );
	}
	printf("},\n");
    }
    printf("},\n");
}


/***********************************************
 *	coord functions */
/* get MATRIX from RPY angles */
void set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor)
{
    MATRIX tmp1;
    SVECTOR v1;

    tmp1.t[0] = coor->coord.t[0];
    tmp1.t[1] = coor->coord.t[1];
    tmp1.t[2] = coor->coord.t[2];

    v1 = *pos;

    RotMatrix(&v1,&tmp1);
    coor->coord = tmp1;

    /* flush the matrix cache */
    coor->flg = 0;
}


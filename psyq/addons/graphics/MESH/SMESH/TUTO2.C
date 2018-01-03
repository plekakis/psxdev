/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*				
 *
 *		固定小数点による物理シミュレーションプログラム
 *
 *		Copyright (C) 1993/1994 by Sony Corporation
 *			All rights Reserved
 *
 */
#include <sys/types.h>
#include <asm.h>
#include <kernel.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <inline_c.h>
#include <gtemac.h>

#define copySVECTOR(sv0,sv1)    (*(long*)&(sv0)->vx = *(long*)&(sv1)->vx), \
                                (*(long*)&(sv0)->vz = *(long*)&(sv1)->vz)


#define TIM_ADDR 0x80120000
#define TIM_HEADER 0x00000010

#define SCR_Z	(512)			/* screen depth (h) */
#define OTLEN	8
#define	OTSIZE	(1<<OTLEN)		/* ordering table size */
#define NEARZ	(700)
#define FARZ	(65536)

#define WALLX		(500)		/* collision wall */
#define WALLY		(400)
#define WALLZ		(400)
#define SG		(50)

#define PIH             640
#define PIV             480
#define OFX             (PIH/2)                 /* screen offset X */
#define OFY             (PIV/2)                 /* screen offset Y */
#define BGR             200                      /* BG color R */
#define BGG             200                     /* BG color G */
#define BGB             200                     /* BG color B */
#define RBK             30                       /* Back color R */
#define GBK             30                       /* Back color G */
#define BBK             30                       /* Back color B */
#define RFC             BGR                     /* Far color R */
#define GFC             BGG                     /* Far color G */
#define BFC             BGB                     /* Far color B */

#define NIC		27			/* max number of icosa */
#define FR		1
#define K		1			/* spring constant */
#define COL		4			/* collision from wall */
#define C		1			/* damping constant */
#define G		20			/* gravity acceleration */
#define R		128			/* icosa radius */
#define MESH_LEN        12			/* icosa mesh */
#define VEL_LIMIT	800			/* velocity limitter */
#define PRIM_LEN        (MESH_LEN-2)
#define RMESH_LEN       7
#define RPRIM_LEN       (RMESH_LEN-2)

#define FALL		10			/* fall speed from hole */
#define HOLE_X		100
#define HOLE_Y		100
#define HOLE_Z		100

typedef struct {
	POLY_F3		fp[6][7];		/* clipped wall */
        POLY_F3         ssurf[NIC][PRIM_LEN];   /* icosa strip mesh */ 
	POLY_F3         rsurf[NIC][RPRIM_LEN];  /* icosa round mesh(top) */
	POLY_F3         tsurf[NIC][RPRIM_LEN];  /* icosa round mesh(bottom) */
        POLY_FT3        ssurt[NIC][PRIM_LEN];   /* icosa strip mesh */ 
	POLY_FT3        rsurt[NIC][RPRIM_LEN];  /* icosa round mesh(top) */
	POLY_FT3        tsurt[NIC][RPRIM_LEN];  /* icosa round mesh(bottom) */
} PBUF;						/* GPU pachet buffer */


typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	BLK_FILL	bg;			/* background */
	PBUF		pb;			/* wall & icosa */
} DB;

typedef struct {
	SVECTOR		p[12];			/*12 vertices */
	SVECTOR		vel[12];		/*velocity of point*/
	SVECTOR		acc[12];		/*acceleration*/
	SVECTOR		frc[12];		/*force*/
	long		r;			/*radius*/
	long		mass;			/*mass*/
	int		FTflag;			/*0:Flat,1:Texture*/

	CVECTOR 	col[MESH_LEN];		/*S-mesh vertex color*/
	CVECTOR 	rcol[RMESH_LEN];	/*R-mesh vertex color*/
	CVECTOR 	tcol[RMESH_LEN];	/*R-mesh vertex color*/
	
	SVECTOR 	uv[MESH_LEN];	/*S-mesh vertex texture address*/
	SVECTOR 	ruv[RMESH_LEN];	/*R-mesh vertex texture address*/
	SVECTOR 	tuv[RMESH_LEN];	/*R-mesh vertex texture address*/

	SVECTOR 	pp[MESH_LEN];		/*S-mesh vertex position*/
	SVECTOR 	rpp[RMESH_LEN];		/*R-mesh vertex position*/
	SVECTOR 	tpp[RMESH_LEN];		/*R-mesh vertex position*/
} ICOSA;


CVECTOR wc[6]={	{ 120, 120, 120,0x20},		/*wall color and GPU code*/
		{ 100, 100, 100,0x20},
		{  80,  80,  80,0x20},
		{  60,  60,  60,0x20},
		{  40,  40,  40,0x20},
		{  20,  20,  20,0x20} };

		

MATRIX		WL0mat;			/* W/L0 */
MATRIX		SWmat;			/* S/W	*/
MATRIX		SL0mat;			/* S/L0	*/
MATRIX		L0Wmat;			/*inverse matrix of WL0mat*/


VECTOR	scale={4096,4096,4096};		/*scale vector for 640x480 mode*/
long	DPQ=0;				/*FOG flag 1:ON 0:OFF (initial OFF)*/
long	BACKC=0;
long	FOGNEAR=700;			/*FOG near (initial 700)*/


EVECTOR *evmtx[10*2];
EVECTOR evbfad[16];

long	nic=0;
u_short tpage;
u_short clut;

TMESH           msh[NIC];	/*triangle mesh for icosa*/
TMESH		rmsh[NIC];	/*triangle mesh for icosa*/
TMESH		tmsh[NIC];	/*triangle mesh for icosa*/

int	framecnt=0;

short	ISQRT[256]={
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
           0x1000, 0xFE0, 0xFC1, 0xFA3, 0xF85, 0xF68, 0xF4C, 0xF30,
           0xF15, 0xEFB, 0xEE1, 0xEC7, 0xEAE, 0xE96, 0xE7E, 0xE66,
           0xE4F, 0xE38, 0xE22, 0xE0C, 0xDF7, 0xDE2, 0xDCD, 0xDB9,
           0xDA5, 0xD91, 0xD7E, 0xD6B, 0xD58, 0xD45, 0xD33, 0xD21,
           0xD10, 0xCFF, 0xCEE, 0xCDD, 0xCCC, 0xCBC, 0xCAC, 0xC9C,
           0xC8D, 0xC7D, 0xC6E, 0xC5F, 0xC51, 0xC42, 0xC34, 0xC26,
           0xC18, 0xC0A, 0xBFD, 0xBEF, 0xBE2, 0xBD5, 0xBC8, 0xBBB,
           0xBAF, 0xBA2, 0xB96, 0xB8A, 0xB7E, 0xB72, 0xB67, 0xB5B,
           0xB50, 0xB45, 0xB39, 0xB2E, 0xB24, 0xB19, 0xB0E, 0xB04,
           0xAF9, 0xAEF, 0xAE5, 0xADB, 0xAD1, 0xAC7, 0xABD, 0xAB4,
           0xAAA, 0xAA1, 0xA97, 0xA8E, 0xA85, 0xA7C, 0xA73, 0xA6A,
           0xA61, 0xA59, 0xA50, 0xA47, 0xA3F, 0xA37, 0xA2E, 0xA26,
           0xA1E, 0xA16, 0xA0E, 0xA06, 0x9FE, 0x9F6, 0x9EF, 0x9E7,
           0x9E0, 0x9D8, 0x9D1, 0x9C9, 0x9C2, 0x9BB, 0x9B4, 0x9AD,
           0x9A5, 0x99E, 0x998, 0x991, 0x98A, 0x983, 0x97C, 0x976,
           0x96F, 0x969, 0x962, 0x95C, 0x955, 0x94F, 0x949, 0x943,
           0x93C, 0x936, 0x930, 0x92A, 0x924, 0x91E, 0x918, 0x912,
           0x90D, 0x907, 0x901, 0x8FB, 0x8F6, 0x8F0, 0x8EB, 0x8E5,
           0x8E0, 0x8DA, 0x8D5, 0x8CF, 0x8CA, 0x8C5, 0x8BF, 0x8BA,
           0x8B5, 0x8B0, 0x8AB, 0x8A6, 0x8A1, 0x89C, 0x897, 0x892,
           0x88D, 0x888, 0x883, 0x87E, 0x87A, 0x875, 0x870, 0x86B,
           0x867, 0x862, 0x85E, 0x859, 0x855, 0x850, 0x84C, 0x847,
           0x843, 0x83E, 0x83A, 0x836, 0x831, 0x82D, 0x829, 0x824,
           0x820, 0x81C, 0x818, 0x814, 0x810, 0x80C, 0x808, 0x804,
	};

short	SQRT[256]={
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
	   0,0,0,0,0,0,0,0,
           0x1000, 0x101F, 0x103F, 0x105E, 0x107E, 0x109C, 0x10BB, 0x10DA,
           0x10F8, 0x1116, 0x1134, 0x1152, 0x116F, 0x118C, 0x11A9, 0x11C6,
           0x11E3, 0x1200, 0x121C, 0x1238, 0x1254, 0x1270, 0x128C, 0x12A7,
           0x12C2, 0x12DE, 0x12F9, 0x1314, 0x132E, 0x1349, 0x1364, 0x137E,
           0x1398, 0x13B2, 0x13CC, 0x13E6, 0x1400, 0x1419, 0x1432, 0x144C,
           0x1465, 0x147E, 0x1497, 0x14B0, 0x14C8, 0x14E1, 0x14F9, 0x1512,
           0x152A, 0x1542, 0x155A, 0x1572, 0x158A, 0x15A2, 0x15B9, 0x15D1,
           0x15E8, 0x1600, 0x1617, 0x162E, 0x1645, 0x165C, 0x1673, 0x1689,
           0x16A0, 0x16B7, 0x16CD, 0x16E4, 0x16FA, 0x1710, 0x1726, 0x173C,
           0x1752, 0x1768, 0x177E, 0x1794, 0x17AA, 0x17BF, 0x17D5, 0x17EA,
           0x1800, 0x1815, 0x182A, 0x183F, 0x1854, 0x1869, 0x187E, 0x1893,
           0x18A8, 0x18BD, 0x18D1, 0x18E6, 0x18FA, 0x190F, 0x1923, 0x1938,
           0x194C, 0x1960, 0x1974, 0x1988, 0x199C, 0x19B0, 0x19C4, 0x19D8,
           0x19EC, 0x1A00, 0x1A13, 0x1A27, 0x1A3A, 0x1A4E, 0x1A61, 0x1A75,
           0x1A88, 0x1A9B, 0x1AAE, 0x1AC2, 0x1AD5, 0x1AE8, 0x1AFB, 0x1B0E,
           0x1B21, 0x1B33, 0x1B46, 0x1B59, 0x1B6C, 0x1B7E, 0x1B91, 0x1BA3,
           0x1BB6, 0x1BC8, 0x1BDB, 0x1BED, 0x1C00, 0x1C12, 0x1C24, 0x1C36,
           0x1C48, 0x1C5A, 0x1C6C, 0x1C7E, 0x1C90, 0x1CA2, 0x1CB4, 0x1CC6,
           0x1CD8, 0x1CE9, 0x1CFB, 0x1D0D, 0x1D1E, 0x1D30, 0x1D41, 0x1D53,
           0x1D64, 0x1D76, 0x1D87, 0x1D98, 0x1DAA, 0x1DBB, 0x1DCC, 0x1DDD,
           0x1DEE, 0x1E00, 0x1E11, 0x1E22, 0x1E33, 0x1E43, 0x1E54, 0x1E65,
           0x1E76, 0x1E87, 0x1E98, 0x1EA8, 0x1EB9, 0x1ECA, 0x1EDA, 0x1EEB,
           0x1EFB, 0x1F0C, 0x1F1C, 0x1F2D, 0x1F3D, 0x1F4E, 0x1F5E, 0x1F6E,
           0x1F7E, 0x1F8F, 0x1F9F, 0x1FAF, 0x1FBF, 0x1FCF, 0x1FDF, 0x1FEF,
	};

typedef struct{
	long	c[30][2];		/*connection*/
	long	d[6][2];		/*diagonal connection*/
}SCconnect;

static	pad_read();
static	init_prim();

main()
{
	long		iotz[PRIM_LEN];	/*otz array for icosa*/
	long		flag[PRIM_LEN];	/*flag array for icosa*/

	DB		db[2];		/* packet double buffer */
	DB		*cdb;		/* current db */
        int             i,j,k;
        long            ret;
        long            id;
	MATRIX		ll;		/* local light Matrix for cube color*/
	MATRIX		lc;		/* local color Matrix for cube color*/
	CVECTOR		bk;		/* ambient color for cube color*/
	long 		otz;		/* otz for cube*/

	SVECTOR 	sv0,sv1,sv2;
	VECTOR 		v0,v1;
	long		nclip[3];	/* normal clip for cube */

	long		CENTX,CENTY;	/* wave center */
	long		A=600,RR,L=10;	/* wave form parameter */

	SVECTOR		wl0t;		/* WL0mat trans. vector */

	ICOSA		ic[NIC];	/* NIC icosahedorns */
	u_long		cnt[20];
	long		frac,expo;
	

	PadInit(0);		/* reset graphic environment */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	gte_SetGeomOffset(OFX, OFY);	/* set geometry origin as (160, 120) */
	gte_SetGeomScreen(SCR_Z);	/* distance to viewing-screen */
        gte_SetBackColor(RBK,GBK,BBK);  /* set background(ambient) color*/
        gte_SetFarColor(RFC,GFC,BFC);   /* set far color */
        SetFogNear(FOGNEAR,SCR_Z);  /* set fog parameter*/
	InitClip(evbfad,PIH+10,PIV,SCR_Z,NEARZ,FARZ);
	texture_init(TIM_ADDR);

	/*interlace mode*/	
	SetDefDrawEnv(&db[0].draw, 0,   0, PIH, PIV);	
	SetDefDrawEnv(&db[1].draw, 0,   0, PIH, PIV);	
	SetDefDispEnv(&db[0].disp, 0,   0, PIH, PIV);	
	SetDefDispEnv(&db[1].disp, 0,   0, PIH, PIV);
	db[0].draw.dfe = db[1].draw.dfe = 0;

	/*init ICOSA*/
	init_icosa(ic);

	/*init connection*/
	init_connect();

        /*set MESH*/
	for(j=0;j<NIC;j++){
        	msh[j].c= ic[j].col;
		msh[j].u= ic[j].uv;
     	   	msh[j].v= ic[j].pp;
        	msh[j].len=MESH_LEN;

        	rmsh[j].c= ic[j].rcol;
		rmsh[j].u= ic[j].ruv;
        	rmsh[j].v= ic[j].rpp;
       		rmsh[j].len=RMESH_LEN;

        	tmsh[j].c= ic[j].tcol;
		tmsh[j].u= ic[j].tuv;
        	tmsh[j].v= ic[j].tpp;
        	tmsh[j].len=RMESH_LEN;
	}

	init_prim(&db[0],wc);	/* set primitive parameters on buffer #0 */
	init_prim(&db[1],wc);	/* set primitive parameters on buffer #1 */
	
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	/*Screen/World initialize(not changed in this program)*/
	init_view(&SWmat);

	ret=0;


	while (pad_read(ic) == 0) {

        	SetFogNear(FOGNEAR,SCR_Z);      /* set fog parameter*/

		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */

		/* WL0->L0W */
		TransposeMatrix(&WL0mat,&L0Wmat);
		L0Wmat.t[0]= -WL0mat.t[0];
		L0Wmat.t[1]= -WL0mat.t[1];
		L0Wmat.t[2]= -WL0mat.t[2];

		add_force_grav(ic);	/* add gravity force */

		add_force_intr(ic);	/* add interaction force */

		add_force_coll(ic);	/* add wall collision force */
		
		add_force_spring(ic);	/* add spring force */

		make_accel_force(ic);	/* make accelleration from force*/
		
		accel_icosa(ic);	/* make velocity from accel*/

		move_icosa(ic);		/* move icosa */

		make_mesh_icosa(ic);	/* make mesh */

		/* SW,WL0->SL0*/
		gte_CompMatrix(&SWmat,&WL0mat,&SL0mat);
		gte_SetRotMatrix(&SL0mat);
		gte_SetTransMatrix(&SL0mat);

		/* Add walls in Local0 coordinate */
		add_wall(cdb->ot,cdb->pb.fp,wc,DPQ);

		/* Add icosa in Local0 coordinate*/
		for(j=0;j<nic;j++){
		  if(ic[j].FTflag==0){
		    RotMeshPrimS_FC3(&msh[j],cdb->pb.ssurf[j],
				cdb->ot,OTLEN,DPQ,1);
		    RotMeshPrimR_FC3(&rmsh[j],cdb->pb.rsurf[j],
				cdb->ot,OTLEN,DPQ,1);
		    RotMeshPrimR_FC3(&tmsh[j],cdb->pb.tsurf[j],
				cdb->ot,OTLEN,DPQ,1);
		  }else{
		    RotMeshPrimS_T3(&msh[j],cdb->pb.ssurt[j],
				cdb->ot,OTLEN,DPQ,1);
		    RotMeshPrimR_T3(&rmsh[j],cdb->pb.rsurt[j],
				cdb->ot,OTLEN,DPQ,1);
		    RotMeshPrimR_T3(&tmsh[j],cdb->pb.tsurt[j],
				cdb->ot,OTLEN,DPQ,1);
		  }
		}	

		/* swap buffer */
		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */
		DrawOTag(cdb->ot+OTSIZE-1);	/* draw */
	}
	PadStop();
	return;
}

/*
 *	set matrix according to user's input
 */
#define p(x)	printf("\t%s\n", x)
static usage()
{
	p("            Moving a Cube                 ");
	p("Right Cross Key:     rotate cube.          ");
	p("I-key, J-key:        move cube position    ");
	p("K-key:		end                   ");
}
#undef p

static pad_read(ic)
ICOSA	*ic;
{
	int i,j;
	static SVECTOR	ang  = { 0, 0, 0};	/* rotate angle */
	static VECTOR	vec  = {0, 0, 0};	/* translate vector */
	static int fL;
	static int tmpic=0;

	int	ret = 0;	

	u_long	padd = PadRead(0);

	if(padd==0) fL=0;

	/* rotate light source and cube */
	if (padd & PADRup)	ang.vx += 32;
	if (padd & PADRdown)	ang.vx -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* change distance */
	if (padd & PADo)	vec.vz += 8;
	if (padd & PADm) 	vec.vz -= 8;

	if (padd & PADl)	ang.vz +=32;
	if (padd & PADn)	ang.vz -=32;

	if (padd & PADLup){
			if(fL==0){
				nic +=1; 
				if(nic>NIC) nic=NIC;
				fL=1;
			}
	}
	if (padd & PADLdown){
			if(fL==0){
				nic -=1; 
				if(nic<0) nic=0;
				fL=1;
			}
	}
	if (padd & PADLleft){
		if(fL==0){
			ic[tmpic].FTflag=1;	/*set texture*/
			tmpic++;
			if(tmpic>=NIC) tmpic=NIC-1;
			fL=1;
		}
	}
	if (padd & PADLright){
		if(fL==0){
			ic[tmpic].FTflag=0;	/*set flat*/
			tmpic--;
			if(tmpic<0) tmpic=0;
			fL=1;
		}
	}

	if ((padd & PADLup)&&(padd & PADh))	DPQ=1;
	if ((padd & PADLdown)&&(padd & PADh))	DPQ=0;
	if ((padd & PADLleft)&&(padd & PADh))	FOGNEAR += 10;
	if ((padd & PADLright)&&(padd & PADh))	FOGNEAR -= 10;

	if (padd & PADk) 	ret = -1;

	if(vec.vz< -1000) vec.vz= -1000;

	RotMatrix(&ang, &WL0mat);	/* make rot-trans matrix */
	TransMatrix(&WL0mat, &vec);	

	return(ret);
}		

/*
 *	initialize primitive parameters
 */
static init_prim(db,c)
DB	*db;
CVECTOR *c;
{
	long	i,j;

	db->draw.isbg=1;
	setRGB0(&db->draw, BGR, BGG, BGB);	/* (r,g,b) = (60,120,120) */

	/* initialize walls */
	for(i=0;i<6;i++){
		for(j=0;j<7;j++){
			SetPolyF3(&db->pb.fp[i][j]);
			setRGB0(&db->pb.fp[i][j],c[i].r,c[i].g,c[i].b);
		}
	}

        /* initialize icosa packet buffer */
	for(j=0;j<NIC;j++){
            for(i=0;i<PRIM_LEN;i++){
                SetPolyF3(&db->pb.ssurf[j][i]); 
                SetPolyFT3(&db->pb.ssurt[j][i]); 
		SetShadeTex(&db->pb.ssurt[j][i],1);
		db->pb.ssurt[j][i].tpage= tpage;
		db->pb.ssurt[j][i].clut= clut;
                db->pb.ssurt[j][i].u0= msh[j].u[i].vx;
                db->pb.ssurt[j][i].v0= msh[j].u[i].vy; 
                db->pb.ssurt[j][i].u1= msh[j].u[i+1].vx;
                db->pb.ssurt[j][i].v1= msh[j].u[i+1].vy;
                db->pb.ssurt[j][i].u2= msh[j].u[i+2].vx;
                db->pb.ssurt[j][i].v2= msh[j].u[i+2].vy;
	    }
            for(i=0;i<RPRIM_LEN;i++){
                SetPolyF3(&db->pb.rsurf[j][i]);
                SetPolyFT3(&db->pb.rsurt[j][i]);
		SetShadeTex(&db->pb.rsurt[j][i],1);
		db->pb.rsurt[j][i].tpage= tpage;
		db->pb.rsurt[j][i].clut= clut;
                db->pb.rsurt[j][i].u0= rmsh[j].u[i].vx; 
                db->pb.rsurt[j][i].v0= rmsh[j].u[i].vy; 
                db->pb.rsurt[j][i].u1= rmsh[j].u[i+1].vx; 
                db->pb.rsurt[j][i].v1= rmsh[j].u[i+1].vy; 
                db->pb.rsurt[j][i].u2= rmsh[j].u[i+2].vx; 
                db->pb.rsurt[j][i].v2= rmsh[j].u[i+2].vy; 
	    }
            for(i=0;i<RPRIM_LEN;i++){
                SetPolyF3(&db->pb.tsurf[j][i]); 
                SetPolyFT3(&db->pb.tsurt[j][i]);
		SetShadeTex(&db->pb.tsurt[j][i],1);
		db->pb.tsurt[j][i].tpage= tpage;
		db->pb.tsurt[j][i].clut= clut;
                db->pb.tsurt[j][i].u0= tmsh[j].u[i].vx; 
                db->pb.tsurt[j][i].v0= tmsh[j].u[i].vy; 
                db->pb.tsurt[j][i].u1= tmsh[j].u[i+1].vx; 
                db->pb.tsurt[j][i].v1= tmsh[j].u[i+1].vy; 
                db->pb.tsurt[j][i].u2= tmsh[j].u[i+2].vx; 
                db->pb.tsurt[j][i].v2= tmsh[j].u[i+2].vy; 
	    }
        }
}


/********************************************************
* p[0]= {0	          ,-R	   ,0                }	*
*							*
* p[1]= {R*0.836*0.809    ,-R*0.447 ,R*0.836*0.588   }	*
* p[2]= {R*0.836*(-0.309) ,-R*0.447 ,R*0.836*0.951   }	*
* p[3]= {R*(-0.836)       ,-R*0.447 ,0               }	*
* p[4]= {R*0.836*(-0.309) ,-R*0.447 ,R*0.836*(-0.951)}	*
* p[5]= {R*0.836*0.809    ,-R*0.447 ,R*0.836*(-0.588)}	*
*							*
* p[6]= {R*0.836          ,R*0.447 ,0                }	*
* p[7]= {R*0.836*0.309    ,R*0.447 ,R*0.836*0.951    }	*
* p[8]= {R*0.836*(-0.809) ,R*0.447 ,R*0.836*0.588    }	*
* p[9]= {R*0.836*(-0.809) ,R*0.447 ,R*0.836*(-0.588) }	*
* p[10]={R*0.836*0.309    ,R*0.447 ,R*0.836*(-0.951) }	*
*							*
* p[11]={0	          ,R	   ,0                }	*
*							*
* p[12]={0		  ,0       ,0		     }	*
*							*
*	0.836*0.809= 2770				*
*	0.447	   = 1831				*
*	0.836*0.588= 2013				*
*	0.836*0.309= 1058				*
*	0.836*0.951= 3256				*
*	0.836      = 3427				*
*							*
********************************************************/
/*
#define	ICO0 2770
#define	ICO1 1831
#define	ICO2 2013
#define	ICO3 1058
#define	ICO4 3256
#define	ICO5 3427
*/
#define	ICO0 2000
#define	ICO1 1000
#define	ICO2 2000
#define	ICO3 1000
#define	ICO4 2000
#define	ICO5 2000

init_icosa(ic)
ICOSA *ic;
{
	int i,j;
	long	ICENTX[NIC];			/*NIC initial position*/
	long	ICENTY[NIC];			
	long	ICENTZ[NIC];		

	for(j=0;j<NIC;j++,ic++){
		/* initialize center position*/
		ICENTX[j]= (rand()%(2*WALLX)) - WALLX;
		ICENTY[j]= (rand()%(2*WALLY)) - WALLY;
		ICENTZ[j]= (rand()%(2*WALLZ)) - WALLZ;

		/* initialize vertex position(not correct)*/
		ic->p[0].vx=  0 + ICENTX[j]; 
		ic->p[0].vy= -R + ICENTY[j]; 
		ic->p[0].vz=  0 + ICENTZ[j];	

		ic->p[1].vx=  (R>>1) + ICENTX[j];
		ic->p[1].vy= -(R>>2) + ICENTY[j];
		ic->p[1].vz=  (R>>1) + ICENTZ[j];

		ic->p[2].vx= -(R>>2) + ICENTX[j];
		ic->p[2].vy= -(R>>2) + ICENTY[j];
		ic->p[2].vz=  (R>>1) + ICENTZ[j];

		ic->p[3].vx= -(R>>1) + ICENTX[j];
		ic->p[3].vy= -(R>>2) + ICENTY[j];
		ic->p[3].vz=  0      + ICENTZ[j];

		ic->p[4].vx= -(R>>2) + ICENTX[j];
		ic->p[4].vy= -(R>>2) + ICENTY[j];
		ic->p[4].vz= -(R>>1) + ICENTZ[j];

		ic->p[5].vx=  (R>>1) + ICENTX[j];
		ic->p[5].vy= -(R>>2) + ICENTY[j];
		ic->p[5].vz= -(R>>1) + ICENTZ[j];

		ic->p[6].vx=  (R>>1) + ICENTX[j];
		ic->p[6].vy=  (R>>2) + ICENTY[j];
		ic->p[6].vz=  0      + ICENTZ[j];

		ic->p[7].vx=  (R>>1) + ICENTX[j];
		ic->p[7].vy=  (R>>2) + ICENTY[j];
		ic->p[7].vz=  (R>>1) + ICENTZ[j];

		ic->p[8].vx= -(R>>1) + ICENTX[j];
		ic->p[8].vy=  (R>>2) + ICENTY[j];
		ic->p[8].vz=  (R>>1) + ICENTZ[j];

		ic->p[9].vx= -(R>>1) + ICENTX[j];
		ic->p[9].vy=  (R>>2) + ICENTY[j];
		ic->p[9].vz= -(R>>1) + ICENTZ[j];

		ic->p[10].vx=  (R>>1) + ICENTX[j];
		ic->p[10].vy=  (R>>2) + ICENTY[j];
		ic->p[10].vz= -(R>>1) + ICENTZ[j];

		ic->p[11].vx=  0      + ICENTX[j]; 
		ic->p[11].vy=  R      + ICENTY[j]; 
		ic->p[11].vz=  0      + ICENTZ[j];	

		ic->p[12].vx=  0      + ICENTX[j]; 
		ic->p[12].vy=  0      + ICENTY[j]; 
		ic->p[12].vz=  0      + ICENTZ[j];	

/* correct icosa
		ic->p[0].vx=  0 + ICENTX[j]; 
		ic->p[0].vy= -R + ICENTY[j]; 
		ic->p[0].vz=  0 + ICENTZ[j];	

		ic->p[1].vx=  ((R*ICO0)>>12) + ICENTX[j];
		ic->p[1].vy= -((R*ICO1)>>12) + ICENTY[j];
		ic->p[1].vz=  ((R*ICO2)>>12) + ICENTZ[j];

		ic->p[2].vx= -((R*ICO3)>>12) + ICENTX[j];
		ic->p[2].vy= -((R*ICO1)>>12) + ICENTY[j];
		ic->p[2].vz=  ((R*ICO4)>>12) + ICENTZ[j];

		ic->p[3].vx= -((R*ICO5)>>12) + ICENTX[j];
		ic->p[3].vy= -((R*ICO1)>>12) + ICENTY[j];
		ic->p[3].vz=  0 	     + ICENTZ[j];

		ic->p[4].vx= -((R*ICO3)>>12) + ICENTX[j];
		ic->p[4].vy= -((R*ICO1)>>12) + ICENTY[j];
		ic->p[4].vz= -((R*ICO4)>>12) + ICENTZ[j];

		ic->p[5].vx=  ((R*ICO0)>>12) + ICENTX[j];
		ic->p[5].vy= -((R*ICO1)>>12) + ICENTY[j];
		ic->p[5].vz= -((R*ICO2)>>12) + ICENTZ[j];

		ic->p[6].vx=  ((R*ICO5)>>12) + ICENTX[j];
		ic->p[6].vy=  ((R*ICO1)>>12) + ICENTY[j];
		ic->p[6].vz=  0 	     + ICENTZ[j];

		ic->p[7].vx=  ((R*ICO2)>>12) + ICENTX[j];
		ic->p[7].vy=  ((R*ICO1)>>12) + ICENTY[j];
		ic->p[7].vz=  ((R*ICO4)>>12) + ICENTZ[j];

		ic->p[8].vx= -((R*ICO0)>>12) + ICENTX[j];
		ic->p[8].vy=  ((R*ICO1)>>12) + ICENTY[j];
		ic->p[8].vz=  ((R*ICO2)>>12) + ICENTZ[j];

		ic->p[9].vx= -((R*ICO0)>>12) + ICENTX[j];
		ic->p[9].vy=  ((R*ICO1)>>12) + ICENTY[j];
		ic->p[9].vz= -((R*ICO2)>>12) + ICENTZ[j];

		ic->p[10].vx=  ((R*ICO2)>>12) + ICENTX[j];
		ic->p[10].vy=  ((R*ICO1)>>12) + ICENTY[j];
		ic->p[10].vz= -((R*ICO4)>>12) + ICENTZ[j];

		ic->p[11].vx=  0 	      + ICENTX[j]; 
		ic->p[11].vy=  R 	      + ICENTY[j]; 
		ic->p[11].vz=  0 	      + ICENTZ[j];	

		ic->p[12].vx=  0 	      + ICENTX[j]; 
		ic->p[12].vy=  0    	      + ICENTY[j]; 
		ic->p[12].vz=  0 	      + ICENTZ[j];	
*/
		for(i=0;i<13;i++){ 
			/*initialize velocity*/
			ic->vel[i].vx=0;
			ic->vel[i].vy=0;
			ic->vel[i].vz=0;

			/*initialize acceleration*/
			ic->acc[i].vx=0;
			ic->acc[i].vy=0;
			ic->acc[i].vz=0;

			/*initialize force*/
			ic->frc[i].vx=0;
			ic->frc[i].vy=0;
			ic->frc[i].vz=0;
		}
		ic->r=R;
		ic->mass=1;
		ic->FTflag=0;

		/*set color*/
		for(i=0;i<MESH_LEN;i++){
			ic->col[i].r= rand()%255; 
			ic->col[i].g= rand()%255; 
			ic->col[i].b= rand()%255;
			ic->col[i].cd=0x20;
		}
		for(i=0;i<RMESH_LEN;i++){
			ic->rcol[i].r= rand()%255; 
			ic->rcol[i].g= rand()%255; 
			ic->rcol[i].b= rand()%255;
			ic->rcol[i].cd=0x20;

			ic->tcol[i].r= rand()%255; 
			ic->tcol[i].g= rand()%255; 
			ic->tcol[i].b= rand()%255;
			ic->tcol[i].cd=0x20;
		}

		/*initialize texture address*/
		ic->uv[0].vx=0; 	ic->uv[0].vy=0; 
		ic->uv[1].vx=16; 	ic->uv[1].vy=0; 
		ic->uv[2].vx=0; 	ic->uv[2].vy=16; 
		ic->uv[3].vx=16; 	ic->uv[3].vy=16; 
		ic->uv[4].vx=0; 	ic->uv[4].vy=0; 
		ic->uv[5].vx=16; 	ic->uv[5].vy=0; 
		ic->uv[6].vx=0; 	ic->uv[6].vy=16; 
		ic->uv[7].vx=16; 	ic->uv[7].vy=16; 
		ic->uv[8].vx=0; 	ic->uv[8].vy=0; 
		ic->uv[9].vx=16; 	ic->uv[9].vy=0; 
		ic->uv[10].vx=0; 	ic->uv[10].vy=16; 
		ic->uv[11].vx=16;	ic->uv[11].vy=16; 

		ic->ruv[0].vx=0; 	ic->ruv[0].vy=0; 
		ic->ruv[1].vx=16;	ic->ruv[1].vy=0; 
		ic->ruv[2].vx=0; 	ic->ruv[2].vy=16; 
		ic->ruv[3].vx=16;	ic->ruv[3].vy=16; 
		ic->ruv[4].vx=0; 	ic->ruv[4].vy=0; 
		ic->ruv[5].vx=16;	ic->ruv[5].vy=0; 
		ic->ruv[6].vx=0; 	ic->ruv[6].vy=16; 

		ic->tuv[0].vx=0; 	ic->tuv[0].vy=0; 
		ic->tuv[1].vx=16;	ic->tuv[1].vy=0; 
		ic->tuv[2].vx=0; 	ic->tuv[2].vy=16; 
		ic->tuv[3].vx=16;	ic->tuv[3].vy=16; 
		ic->tuv[4].vx=0; 	ic->tuv[4].vy=0; 
		ic->tuv[5].vx=16;	ic->tuv[5].vy=0; 
		ic->tuv[6].vx=0; 	ic->tuv[6].vy=16; 
	}
}

init_connect()
{
	SCconnect		*SCc;

	SCc= (SCconnect*)getScratchAddr(100);

	/*connection between vertices*/
	SCc->c[0][0]= 0;	SCc->c[0][1]= 1;
	SCc->c[1][0]= 0;	SCc->c[1][1]= 2;
	SCc->c[2][0]= 0;	SCc->c[2][1]= 3;
	SCc->c[3][0]= 0;	SCc->c[3][1]= 4;
	SCc->c[4][0]= 0;	SCc->c[4][1]= 5;

	SCc->c[5][0]= 1;	SCc->c[5][1]= 2;
	SCc->c[6][0]= 2;	SCc->c[6][1]= 3;
	SCc->c[7][0]= 3;	SCc->c[7][1]= 4;
	SCc->c[8][0]= 4;	SCc->c[8][1]= 5;
	SCc->c[9][0]= 5;	SCc->c[9][1]= 1;

	SCc->c[10][0]= 6;	SCc->c[10][1]= 1;
	SCc->c[11][0]= 1;	SCc->c[11][1]= 7;
	SCc->c[12][0]= 7;	SCc->c[12][1]= 2;
	SCc->c[13][0]= 2;	SCc->c[13][1]= 8;
	SCc->c[14][0]= 8;	SCc->c[14][1]= 3;
	SCc->c[15][0]= 3;	SCc->c[15][1]= 9;
	SCc->c[16][0]= 9;	SCc->c[16][1]= 4;
	SCc->c[17][0]= 4;	SCc->c[17][1]= 10;
	SCc->c[18][0]= 10;	SCc->c[18][1]= 5;
	SCc->c[19][0]= 5;	SCc->c[19][1]= 6;

	SCc->c[20][0]= 6;	SCc->c[20][1]= 7;
	SCc->c[21][0]= 7;	SCc->c[21][1]= 8;
	SCc->c[22][0]= 8;	SCc->c[22][1]= 9;
	SCc->c[23][0]= 9;	SCc->c[23][1]= 10;
	SCc->c[24][0]= 10;	SCc->c[24][1]= 6;

	SCc->c[25][0]= 6;	SCc->c[25][1]= 11;
	SCc->c[26][0]= 7;	SCc->c[26][1]= 11;
	SCc->c[27][0]= 8;	SCc->c[27][1]= 11;
	SCc->c[28][0]= 9;	SCc->c[28][1]= 11;
	SCc->c[29][0]= 10;	SCc->c[29][1]= 11;

	SCc->d[0][0]= 0;	SCc->d[0][1]= 11;
	SCc->d[1][0]= 1;	SCc->d[1][1]= 9;
	SCc->d[2][0]= 2;	SCc->d[2][1]= 10;
	SCc->d[3][0]= 3;	SCc->d[3][1]= 6;
	SCc->d[4][0]= 4;	SCc->d[4][1]= 7;
	SCc->d[5][0]= 5;	SCc->d[5][1]= 8;
}

make_mesh_icosa(ic)
ICOSA *ic;
{
	int	j;
	long	a,b;

	for(j=0;j<nic;j++){
		a= *(long*)&ic[j].p[6].vx;
		b= ic[j].p[6].vz;
		*(long*)&ic[j].pp[0].vx= a;
		ic[j].pp[0].vz= b;
		*(long*)&ic[j].pp[10].vx= a;
		ic[j].pp[10].vz= b;
		*(long*)&ic[j].tpp[6].vx= a;
		ic[j].tpp[6].vz= b;
		*(long*)&ic[j].tpp[1].vx= a;
		ic[j].tpp[1].vz= b;

		a= *(long*)&ic[j].p[1].vx;
		b= ic[j].p[1].vz;
		*(long*)&ic[j].pp[1].vx= a;
		ic[j].pp[1].vz= b;
		*(long*)&ic[j].pp[11].vx= a;
		ic[j].pp[11].vz= b;
		*(long*)&ic[j].rpp[1].vx= a;
		ic[j].rpp[1].vz= b;
		*(long*)&ic[j].rpp[6].vx= a;
		ic[j].rpp[6].vz= b;

		a= *(long*)&ic[j].p[7].vx;
		b= ic[j].p[7].vz;
		*(long*)&ic[j].pp[2].vx= a;
		ic[j].pp[2].vz= b;
		*(long*)&ic[j].tpp[2].vx= a;
		ic[j].tpp[2].vz= b;

		a= *(long*)&ic[j].p[2].vx;
		b= ic[j].p[2].vz;
		*(long*)&ic[j].pp[3].vx= a;
		ic[j].pp[3].vz= b;
		*(long*)&ic[j].rpp[5].vx= a;
		ic[j].rpp[5].vz= b;

		a= *(long*)&ic[j].p[8].vx;
		b= ic[j].p[8].vz;
		*(long*)&ic[j].pp[4].vx= a;
		ic[j].pp[4].vz= b;
		*(long*)&ic[j].tpp[3].vx= a;
		ic[j].tpp[3].vz= b;

		a= *(long*)&ic[j].p[3].vx;
		b= ic[j].p[3].vz;
		*(long*)&ic[j].pp[5].vx= a;
		ic[j].pp[5].vz= b;
		*(long*)&ic[j].rpp[4].vx= a;
		ic[j].rpp[4].vz= b;

		a= *(long*)&ic[j].p[9].vx;
		b= ic[j].p[9].vz;
		*(long*)&ic[j].pp[6].vx= a;
		ic[j].pp[6].vz= b;
		*(long*)&ic[j].tpp[4].vx= a;
		ic[j].tpp[4].vz= b;

		a= *(long*)&ic[j].p[4].vx;
		b= ic[j].p[4].vz;
		*(long*)&ic[j].pp[7].vx= a;
		ic[j].pp[7].vz= b;
		*(long*)&ic[j].rpp[3].vx= a;
		ic[j].rpp[3].vz= b;

		a= *(long*)&ic[j].p[10].vx;
		b= ic[j].p[10].vz;
		*(long*)&ic[j].pp[8].vx= a;
		ic[j].pp[8].vz= b;
		*(long*)&ic[j].tpp[5].vx= a;
		ic[j].tpp[5].vz= b;

		a= *(long*)&ic[j].p[5].vx;
		b= ic[j].p[5].vz;
		*(long*)&ic[j].pp[9].vx= a;
		ic[j].pp[9].vz= b;
		*(long*)&ic[j].rpp[2].vx= a;
		ic[j].rpp[2].vz= b;

		copySVECTOR(&ic[j].rpp[0],&ic[j].p[0]);
		copySVECTOR(&ic[j].tpp[0],&ic[j].p[11]);
	}
}

/********************************
*	velocity=(1,11,4)	*
********************************/
move_icosa(ic)
ICOSA *ic;
{
	int i,j;

	for(j=0;j<nic;j++){
	    for(i=0;i<12;i++){
/*
		if(ic[j].vel[i].vx>VEL_LIMIT) ic[j].vel[i].vx= VEL_LIMIT;
		if(ic[j].vel[i].vy>VEL_LIMIT) ic[j].vel[i].vy= VEL_LIMIT;
		if(ic[j].vel[i].vz>VEL_LIMIT) ic[j].vel[i].vz= VEL_LIMIT;
		if(ic[j].vel[i].vx< -VEL_LIMIT) ic[j].vel[i].vx= -VEL_LIMIT;
		if(ic[j].vel[i].vy< -VEL_LIMIT) ic[j].vel[i].vy= -VEL_LIMIT;
		if(ic[j].vel[i].vz< -VEL_LIMIT) ic[j].vel[i].vz= -VEL_LIMIT;
*/
		ic[j].p[i].vx +=((ic[j].vel[i].vx)>>4);
		ic[j].p[i].vy +=((ic[j].vel[i].vy)>>4);
		ic[j].p[i].vz +=((ic[j].vel[i].vz)>>4);
	    }
	}
}

/********************************
*	accel=(1,11,4)		*
********************************/
accel_icosa(ic)
ICOSA *ic;
{
	int i,j;

	for(j=0;j<nic;j++){
	    for(i=0;i<12;i++){
		ic[j].vel[i].vx += ic[j].acc[i].vx;
		ic[j].vel[i].vy += ic[j].acc[i].vy;
		ic[j].vel[i].vz += ic[j].acc[i].vz;
	    }
	}
}

/********************************
*	force=(1,11,4)		*
********************************/
make_accel_force(ic)
ICOSA *ic;
{
	int i,j;
	for(j=0;j<nic;j++){
	    for(i=0;i<12;i++){
		/*original
		ic->acc[i].vx=(2*ic->frc[i].vx- (C*ic->vel[i].vx)/10)/ic->mass;
		ic->acc[i].vy=(2*ic->frc[i].vy- (C*ic->vel[i].vy)/10)/ic->mass;
		ic->acc[i].vz=(2*ic->frc[i].vz- (C*ic->vel[i].vz)/10)/ic->mass;
		*/
		ic[j].acc[i].vx=(ic[j].frc[i].vx<<1)- ((ic[j].vel[i].vx)>>3);
		ic[j].acc[i].vy=(ic[j].frc[i].vy<<1)- ((ic[j].vel[i].vy)>>3);
		ic[j].acc[i].vz=(ic[j].frc[i].vz<<1)- ((ic[j].vel[i].vz)>>3);
	    }
	}
}

/********************************
*	force=(1,11,4)		*
********************************/
add_force_grav(ic)
ICOSA *ic;
{
	SVECTOR	g;		/*gravity in world*/
	VECTOR lg;		/*gravity in local0*/
	long	lgx,lgy,lgz;
	int i,j;

	/*mass=1*/
	g.vx=0;
	g.vy=G;
	g.vz=0;

	gte_ApplyMatrix(&L0Wmat,&g,&lg);
	lgx= lg.vx;
	lgy= lg.vy;
	lgz= lg.vz;

	for(j=0;j<nic;j++){
	    for(i=0;i<12;i++){
		ic[j].frc[i].vx= lgx;
		ic[j].frc[i].vy= lgy;
		ic[j].frc[i].vz= lgz;
	    }
	}
}

/********************************
*	force=(1,11,4)		*
********************************/
add_force_intr(ic)
ICOSA *ic;
{
	int i,j,k,m;
	VECTOR	v,c;
	long len;
	long	v1,expo,frac;
	long	fx,fy,fz;
	long	c1x,c1y,c1z,c2x,c2y,c2z;

	typedef struct{
		VECTOR	edge;
		VECTOR	edge2;
		VECTOR	ne;
		long	lzc;
	} SCspring;
	
	SCspring *sc;	

	sc= (SCspring*)getScratchAddr(0);


	/*this is only center to center interaction*/
	for(j=0;j<nic;j++){
		c1x= (ic[j].p[0].vx + ic[j].p[11].vx)/2;
		c1y= (ic[j].p[0].vy + ic[j].p[11].vy)/2;
		c1z= (ic[j].p[0].vz + ic[j].p[11].vz)/2;

		for(k=j+1;k<nic;k++){
			c2x= (ic[k].p[0].vx + ic[k].p[11].vx)/2;
			c2y= (ic[k].p[0].vy + ic[k].p[11].vy)/2;
			c2z= (ic[k].p[0].vz + ic[k].p[11].vz)/2;

			sc->edge.vx= c2x - c1x;
			sc->edge.vy= c2y - c1y;
			sc->edge.vz= c2z - c1z;

			if(sc->edge.vx<-230)continue;
			if(sc->edge.vx>230)continue;
			if(sc->edge.vy<-230)continue;
			if(sc->edge.vy>230)continue;
			if(sc->edge.vz<-230)continue;
			if(sc->edge.vz>230)continue;
/*
			len=VectorNormal(&sc->edge,&sc->ne);
*/
			/*this is VectorNormal's inline expansion*/
			/*here, supposing lzc is small enough(<24) */
			/*and, normal vector is multiplied by constant*/
                	gte_ldlvl(&sc->edge);
                	gte_sqr0();
               	 	gte_stlvnl(&sc->edge2);

                	len= sc->edge2.vx + sc->edge2.vy + sc->edge2.vz;

                	gte_Lzc(len,&sc->lzc);
                	v1 = (sc->lzc)&0xfffffffe;
                	expo = (31- v1)>>1;
                	frac = ISQRT[len>>(24- v1)];

                	gte_lddp(((frac)*230)>>(expo));
                	gte_ldlvl(&sc->edge);
                	gte_gpf0();
                	gte_stlvnl(&sc->ne);

			/*230=2*R*0.9*/
			if(len>230*230)continue;

			/*original
			tmp.vx += 4*((len-2*R)*ne.vx/2/R)/128;
			tmp.vy += 4*((len-2*R)*ne.vy/2/R)/128;
			tmp.vz += 4*((len-2*R)*ne.vz/2/R)/128;
			*/
			/*R=128 faster*/
			fx= ((sc->edge.vx<<12)-sc->ne.vx)>>13;
			fy= ((sc->edge.vy<<12)-sc->ne.vy)>>13;
			fz= ((sc->edge.vz<<12)-sc->ne.vz)>>13;


			for(m=0;m<12;m++){
			        ic[j].frc[m].vx += fx;
			        ic[j].frc[m].vy += fy;
			    	ic[j].frc[m].vz += fz;

			        ic[k].frc[m].vx -= fx;
			        ic[k].frc[m].vy -= fy;
			    	ic[k].frc[m].vz -= fz;
			}
		}
	}

	/****this is all vertices vs center interaction	
	for(j=0;j<NIC;j++){
	    for(i=0;i<13;i++){
		for(k=0;k<NIC;k++){
		    if(k!=j){
			edge.vx= ic[k].p[12].vx - ic[j].p[12].vx;
			edge.vy= ic[k].p[12].vy - ic[j].p[12].vy;
			edge.vz= ic[k].p[12].vz - ic[j].p[12].vz;


			len=SquareRoot0(VectorNormal(&edge,&ne));

			if(len<R){

			    for(m=0;m<13;m++){
				if(m==i){
			          ic[j].frc[m].vx += 3*((len-R)*ne.vx/R)/100;
			    	  ic[j].frc[m].vy += 3*((len-R)*ne.vy/R)/100;
			    	  ic[j].frc[m].vz += 3*((len-R)*ne.vz/R)/100;
				}else{
			          ic[j].frc[m].vx += 2*((len-R)*ne.vx/R)/100;
			    	  ic[j].frc[m].vy += 2*((len-R)*ne.vy/R)/100;
			    	  ic[j].frc[m].vz += 2*((len-R)*ne.vz/R)/100;
				}
			    }
			}
		    }
		}
	    }
	}
	*****/
}

/********************************
*	force=(1,11,4)		*
********************************/
add_force_coll(ic)
ICOSA *ic;
{
	int i,j;
	long vx,vy,vz;
	long svx,svy,svz;


	for(j=0;j<nic;j++){
	    for(i=0;i<12;i++){
		vx= ic[j].p[i].vx;
		vy= ic[j].p[i].vy;
		vz= ic[j].p[i].vz;

		/*Y-Z wall*/
		/*	svx = -(vx-WALLX)*COL;	original*/
		svx = 0;
		if(vx>WALLX) svx = -(vx-WALLX)<<2; /*COL=4 */
		if(vx<-WALLX) svx = -(vx+WALLX)<<2;
			
		/*X-Z wall*/
		svy = 0;
		if(vy<-WALLY) svy = -(vy+WALLY)<<2;
		if(vy> WALLY)	svy = -(vy-WALLY)<<2;

		/*X-Y wall*/
		svz = 0;
		if(vz>WALLZ) svz = -(vz-WALLZ)<<2;
		if(vz<-WALLZ)	svz = -(vz+WALLZ)<<2;

		ic[j].frc[i].vx += svx;
		ic[j].frc[i].vy += svy;
		ic[j].frc[i].vz += svz;
	    }
	}
}


/********************************
*	force=(1,11,4)		*
********************************/
add_force_spring(ic)
ICOSA *ic;
{
	int i,j;
	int n,m;
	long	len;
	long	frac,expo;
	long	v1;
	long	fx,fy,fz;

	typedef struct{
		VECTOR	edge;
		VECTOR	edge2;
		VECTOR	ne;
		long	lzc;
	} SCspring;
	
	SCspring *sc;	
	SCconnect *SCc;

	sc= (SCspring*)getScratchAddr(0);
	SCc= (SCconnect*)getScratchAddr(100);


	for(j=0;j<nic;j++){
	    for(i=0;i<30;i++){

		n= SCc->c[i][1];
		m= SCc->c[i][0];

/*
		sc->edge.vx= ic[j].p[n].vx - ic[j].p[m].vx;
		sc->edge.vy= ic[j].p[n].vy - ic[j].p[m].vy;
*/
		gte_subdvl((long*)&ic[j].p[n].vx,
				(long*)&ic[j].p[m].vx,
				(long*)&sc->edge.vx);

		sc->edge.vz= ic[j].p[n].vz - ic[j].p[m].vz;
/*
		VectorNormal(&sc->edge,&sc->ne);

		ic[j].frc[n].vx -= (fx=((sc->edge.vx<<12)- R*sc->ne.vx)>>10);
		ic[j].frc[n].vy -= (fy=((sc->edge.vy<<12)- R*sc->ne.vy)>>10);
		ic[j].frc[n].vz -= (fz=((sc->edge.vz<<12)- R*sc->ne.vz)>>10);

		ic[j].frc[m].vx += fx;
		ic[j].frc[m].vy += fy;
		ic[j].frc[m].vz += fz;
*/
		/*this is VectorNormal's inline expansion*/
		/*here, supposing lzc is small enough(<24) */
		/*and, normal vector is multiplied by constant*/

		gte_ldlvl(&sc->edge);
		gte_sqr0();
		gte_stlvnl(&sc->edge2);

		len= sc->edge2.vx + sc->edge2.vy + sc->edge2.vz;

		gte_Lzc(len,&sc->lzc);
		v1 = (sc->lzc)&0xfffffffe;
		expo = (31- v1)>>1;
		frac = ISQRT[len>>(24- v1)];

		gte_lddp(((frac)*R)>>(expo));
		gte_ldlvl(&sc->edge);
		gte_gpf0();
		sc->edge.vx <<=12;
		sc->edge.vy <<=12;
		sc->edge.vz <<=12;
		gte_stlvnl(&sc->ne);

		ic[j].frc[n].vx -= (fx=(sc->edge.vx- sc->ne.vx)>>10);
		ic[j].frc[n].vy -= (fy=(sc->edge.vy- sc->ne.vy)>>10);
		ic[j].frc[n].vz -= (fz=(sc->edge.vz- sc->ne.vz)>>10);

		ic[j].frc[m].vx += fx;
		ic[j].frc[m].vy += fy;
		ic[j].frc[m].vz += fz;
	    }

	    for(i=0;i<6;i++){

		n= SCc->d[i][1];
		m= SCc->d[i][0];

/*
		sc->edge.vx= ic[j].p[n].vx - ic[j].p[m].vx;
		sc->edge.vy= ic[j].p[n].vy - ic[j].p[m].vy;
*/
		gte_subdvl((long*)&ic[j].p[n].vx,
				(long*)&ic[j].p[m].vx,
				(long*)&sc->edge.vx);

		sc->edge.vz= ic[j].p[n].vz - ic[j].p[m].vz;
/*
		len=SquareRoot0(VectorNormal(&sc->edge,&sc->ne));

		ic[j].frc[n].vx -= (fx=((sc->edge.vx<<12)-2*R*sc->ne.vx)>>10);
		ic[j].frc[n].vy -= (fy=((sc->edge.vy<<12)-2*R*sc->ne.vy)>>10);
		ic[j].frc[n].vz -= (fz=((sc->edge.vz<<12)-2*R*sc->ne.vz)>>10);

		ic[j].frc[m].vx += fx;
		ic[j].frc[m].vy += fy;
		ic[j].frc[m].vz += fz;
*/
		/*this is VectorNormal's inline expansion*/
		/*here, supposing lzc is small enough(<24) */
		/*and, normal vector is multiplied by constant*/
		gte_ldlvl(&sc->edge);
		gte_sqr0();
		gte_stlvnl(&sc->edge2);

		len= sc->edge2.vx + sc->edge2.vy + sc->edge2.vz;

		gte_Lzc(len,&sc->lzc);
		v1 = (sc->lzc)&0xfffffffe;
		expo = (31- v1)>>1;
		frac = ISQRT[len>>(24- v1)];

		gte_lddp((frac*R*2)>>expo);
		gte_ldlvl(&sc->edge);
		gte_gpf0();
		sc->edge.vx <<=12;
		sc->edge.vy <<=12;
		sc->edge.vz <<=12;
		gte_stlvnl(&sc->ne);

		ic[j].frc[n].vx -= (fx=(sc->edge.vx- sc->ne.vx)>>10);
		ic[j].frc[n].vy -= (fy=(sc->edge.vy- sc->ne.vy)>>10);
		ic[j].frc[n].vz -= (fz=(sc->edge.vz- sc->ne.vz)>>10);

		ic[j].frc[m].vx += fx;
		ic[j].frc[m].vy += fy;
		ic[j].frc[m].vz += fz;
	    }
	}
}

/*
 *
 *      Simple Wall Handler
 */
static SVECTOR P0 = {-(WALLX+SG),-(WALLY+SG),-(WALLZ+SG)};
static SVECTOR P1 = { (WALLX+SG),-(WALLY+SG),-(WALLZ+SG)};
static SVECTOR P2 = { (WALLX+SG), (WALLY+SG),-(WALLZ+SG)};
static SVECTOR P3 = {-(WALLX+SG), (WALLY+SG),-(WALLZ+SG)};

static SVECTOR P4 = {-(WALLX+SG),-(WALLY+SG), (WALLZ+SG)};
static SVECTOR P5 = { (WALLX+SG),-(WALLY+SG), (WALLZ+SG)};
static SVECTOR P6 = { (WALLX+SG), (WALLY+SG), (WALLZ+SG)};
static SVECTOR P7 = {-(WALLX+SG), (WALLY+SG), (WALLZ+SG)};

static SVECTOR  *v[6*4] = {
        &P0,&P1,&P2,&P3,
        &P1,&P5,&P6,&P2,
        &P5,&P4,&P7,&P6,
        &P4,&P0,&P3,&P7,
        &P4,&P5,&P1,&P0,
        &P6,&P7,&P3,&P2,
};


add_wall(ot,s,c,dpq)
u_long *ot;
POLY_F3 *s;
CVECTOR *c;
long dpq;
{
	int 	i,j;
	int	isomote,flg,p,otz;
	SVECTOR	**vp;
	long	n;
	POLY_F3 *sp;
	POLY_F3 *spp;


	for(i=0,vp=v,sp=s; i<6; i++,sp+=7,vp+=4,c++){

		n= Clip4FP(vp[0],vp[1],vp[3],vp[2],evmtx);


        	for(j=0,spp=sp; j<(n-2); j++,spp++){
			*(long *)&spp->x0= *(long *)&evmtx[0]->sxy;
			*(long *)&spp->x1= *(long *)&evmtx[1+j]->sxy;
			*(long *)&spp->x2= *(long *)&evmtx[2+j]->sxy;

			if(dpq==1){
				DpqColor(c,evmtx[1]->sxyz.pad,(CVECTOR *)&spp->r0);
			}else{
				*(long *)&spp->r0 = *(long *)c;
			}

			isomote= NormalClip(
					*(long *)&spp->x0,
					*(long *)&spp->x1,
					*(long *)&spp->x2);
			if(isomote<0){
				SetSemiTrans(spp,0);
				addPrim(ot+OTSIZE-2,spp);
			}else{
				SetSemiTrans(spp,1);
				addPrim(ot,spp);
			}
		}
	}

}


texture_init(addr)              /* テクスチャデータをVRAMにロードする */
u_long addr;
{
  RECT rect1;
  GsIMAGE tim1;

  GsGetTimInfo((u_long *)(addr+4),&tim1); /* TIMデータのヘッダからテクスチャの
                                             データタイプの情報を得る */
  rect1.x=tim1.px;               /* テクスチャ左上のVRAMでのX座標 */
  rect1.y=tim1.py;         /* テクスチャ左上のVRAMでのY座標 */
  rect1.w=tim1.pw;              /* テクスチャ幅 */
  rect1.h=tim1.ph;              /* テクスチャ高さ */

  LoadImage(&rect1,tim1.pixel); /* VRAMにテクスチャをロードする */
  tpage=GetTPage(tim1.pmode,0,640,0);
  tpage= tpage&0xfe7f;		/*for GetTPage bug*/

  if((tim1.pmode>>3)&0x01)      /* カラールックアップテーブルが存在する */
    {
      rect1.x=tim1.cx;          /* クラット左上のVRAMでのX座標 */
      rect1.y=tim1.cy;          /* クラット左上のVRAMでのY座標 */
      rect1.w=tim1.cw;          /* クラットの幅 */
      rect1.h=tim1.ch;          /* クラットの高さ */
      LoadImage(&rect1,tim1.clut); /* VRAMにクラットをロードする */
    }
  clut=GetClut(tim1.cx,tim1.cy);
}

init_view(m)
MATRIX	*m;
{
	m->m[0][0]=4096; 	m->m[0][1]=0; 		m->m[0][2]=0;
	m->m[1][0]=0; 		m->m[1][1]=4096; 	m->m[1][2]=0;
	m->m[2][0]=0; 		m->m[2][1]=0; 		m->m[2][2]=4096;
	m->t[0]= 0; 		m->t[1]= 0; 		m->t[2]= 4*SCR_Z;
	ScaleMatrixL(m,&scale);
}

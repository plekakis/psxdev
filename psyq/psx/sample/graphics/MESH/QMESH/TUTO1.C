/* $PSLibId: Run-time Library Release 4.4$ */
/*				
 *
 *	QMESH sample program
 *					1995,5,15,	M.Oka
 *					1997,3, 5,	sachiko	(added autopad)
 *
 */
#include <sys/types.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>


#define SCR_Z		(512)		/* screen depth (h) */
#define OTLEN		10
#define	OTSIZE		(1<<OTLEN)	/* ordering table size */
#define RECTH           41		/* 41= 3*14-1*/
#define RECTV           46		/* PadRead -> 43, No Pad -> 43 */
#define EDGE            400
#define FARCLIP		14000
#define CHIGHT		200
#define PIH             640
#define PIV             480
#define OFX             (PIH/2)         /* screen offset X */
#define OFY             (PIV/2)         /* screen offset Y */
#define BGR             0               /* BG color R */
#define BGG             0               /* BG color G */
#define BGB             0               /* BG color B */
#define RBK             0               /* Back color R */
#define GBK             0               /* Back color G */
#define BBK             0               /* Back color B */
#define RFC             BGR             /* Far color R */
#define GFC             BGG             /* Far color G */
#define BFC             BGB             /* Far color B */
#define FOGNEAR         8000            /* Fog near point */
#define FOGFAR          14000            /* Fog near point */
#define DPQ             0               /*1:ON,0:OFF*/

typedef struct {
	POLY_FT4        surf[RECTV][RECTH];
} PBUF;

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	PBUF		pb;
} DB;

typedef struct {
	long		sminX;
	long		smaxX;
	long		sminY;
	long		smaxY;
	long		sminZ;
	long		smaxZ;
} SCLIP;

typedef struct {
	long	sxy;
	long	code;
} LINE_BUF;

SVECTOR map_vert[RECTV+1][RECTH+1];	/* position*/
SVECTOR map_text[RECTV+1][RECTH+1];	/* texture position*/

LINE_BUF	*line_sxy;

long	backc=0;
long	dpq=1;

/* Static Declarations */
static pad_read(MATRIX*); 
static init_prim(DB*);


main()
{
	DB		db[2];		/* packet double buffer */
	DB		*cdb;		/* current db */
	MATRIX		rottrans;	/* rot-trans matrix */
	int		dmy, flg;	/* dummy */
	QMESH		mesh;
        int             i,j,hcnt;
        long            ret;
        long            id;
	MATRIX		ll;		/* local light Matrix */
	MATRIX		lc;		/* local color Matrix */
	CVECTOR		bk;		/* ambient color */
	u_long cnt;
	SCLIP		clip;

	clip.sminX= 0;
	clip.smaxX= 640;
	clip.sminY= 0;
	clip.smaxY= 480;
	clip.smaxZ= FARCLIP;

	line_sxy= (LINE_BUF*)getScratchAddr(0);
	
	ResetCallback();
	PadInit(0);		/* reset graphic environment */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(OFX, OFY);	/* set geometry origin as (160, 120) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */
        SetBackColor(RBK,GBK,BBK);      /* set background(ambient) color*/
        SetFarColor(RFC,GFC,BFC);       /* set far color */
        SetFogNearFar(FOGNEAR,FOGFAR,SCR_Z);      /* set fog parameter*/

	/*interlace mode*/	
	SetDefDrawEnv(&db[0].draw, 0,   0, PIH, PIV);	
	SetDefDrawEnv(&db[1].draw, 0,   0, PIH, PIV);	
	SetDefDispEnv(&db[0].disp, 0,   0, PIH, PIV);	
	SetDefDispEnv(&db[1].disp, 0,   0, PIH, PIV);
	db[0].draw.dfe = db[1].draw.dfe = 0;

	FntLoad(960, 256);
	SetDumpFnt(FntOpen(64, 64, 256, 200, 0, 512));
	/*
	SetRCnt(RCntCNT2, 0xffff, RCntMdNOINTR|RCntMdFR);
	StartRCnt(RCntCNT2);
	*/
	GsInitVcount();

	init_prim(&db[0]);	/* set primitive parameters on buffer #0 */
	init_prim(&db[1]);	/* set primitive parameters on buffer #1 */
	
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

        /* generate position (x,y,z)*/
        gen_vert(map_vert,RECTV,RECTH,EDGE);

	/* generate texture postion (u,v)*/
	gen_texture(map_text,RECTV,RECTH);

	/* make QMESH strcuture*/
	mesh.lenv=RECTV+1;
	mesh.lenh=RECTH+1;
	mesh.v=(SVECTOR *)map_vert;
	mesh.u=(SVECTOR *)map_text;

        /*copy Mesh UV to GPU packet*/
        for(id=0;id<2;id++){
		CopyMeshUV(&mesh,&db[id].pb);
	}

	ret=0;
	while (pad_read(&rottrans) == 0) {
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */

		/* ResetRCnt(RCntCNT2); */
		GsClearVcount();

		/* translate each mesh vertex and add to OT */
		
		RotMeshPrimQ_T(&mesh,&cdb->pb,cdb->ot,OTLEN,dpq,backc,
				&clip,line_sxy);

		/* cnt = GetRCnt(RCntCNT2); */
		cnt = GsGetVcount();
		FntPrint("cnt=%d\n", cnt);

		/* swap buffer */
		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
		/*ret=pad_read(&rottrans);*/
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */
		/*DumpOTag(cdb->ot);	/* for debug */
		DrawOTag(cdb->ot+OTSIZE-1);	/* draw */
		FntFlush(-1);
	}

	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
}

static pad_read(rottrans)
MATRIX	*rottrans;		/* rot-trans matrix, light matrix */
{
	static SVECTOR	ang  = { 0, 0, 0};	/* rotate angle */
	static VECTOR	vec  = {0, 1000, 8000};/* rottranslate vector */

	int	ret = 0;	


	u_long	padd;
/*
	u_long	padd = PadRead(0);
	u_long	padd=0; 
*/

	padd = PadRead(0);

	/* rotate light source and cube */
/*
	if (padd & PADRup)	ang.vx += 32;
	if (padd & PADRdown)	ang.vx -= 32;
*/
	if (padd & PADRleft) 	ang.vy += 8;
	if (padd & PADRright)	ang.vy -= 8;

	/* change distance */
	if (padd & PADn)	vec.vz += 8;
	if (padd & PADl) 	vec.vz -= 8;
	if (padd & PADm)	backc = 1;
	if (padd & PADo) 	backc = 0;

	if (padd & PADRup)	dpq=1;
	if (padd & PADRdown)	dpq=0;

	if (padd & PADLup)	vec.vy += 8;
	if (padd & PADLdown)	vec.vy -= 8;
	if (padd & PADLleft) 	vec.vx += 8;
	if (padd & PADLright)	vec.vx -= 8;

	if (padd & PADk) 	ret = -1;

	RotMatrix(&ang, rottrans);	/* make rot-trans matrix  */
	
	/* set matrix */
	TransMatrix(rottrans, &vec);	
	SetRotMatrix(rottrans);
	SetTransMatrix(rottrans);

	return(ret);
}		

/*
 *	initialize primitive parameters
 */
static init_prim(db)
DB	*db;
{
	long	i,j,na;
	u_short	tpage;
	u_short	clut;
	
	extern u_long	bgtex[];	/* texture image */
	
	tpage = LoadTPage(bgtex + 0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 480);

	db->draw.isbg = 1;
	setRGB0(&db->draw, BGR, BGG, BGB);	/* (r,g,b) = (BGR,BGG,BGB) */

	for(j=0;j<RECTV;j++){
		for(i=0;i<RECTH;i++) {
			SetPolyFT4(&db->pb.surf[j][i]);
			SetShadeTex(&db->pb.surf[j][i], 1);
			db->pb.surf[j][i].tpage = tpage;
			db->pb.surf[j][i].clut  = clut;
		}
	}
}



gen_vert(p,v,h,e)
SVECTOR *p;
long v,h,e;       
{
        int i,j;

        for(i=0;i<=v;i++)
        for(j=0;j<=h;j++){
		(p+i*(h+1)+j)->vx= j*e-h*e/2;
/*
		(p+i*(h+1)+j)->vy = -CHIGHT*rand()/65536;
*/
		(p+i*(h+1)+j)->vy = -((CHIGHT*rsin(3*i*4096/v))/4096)
					*rcos(3*j*4096/h)/4096;
		(p+i*(h+1)+j)->vz= -i*e+v*e/2;
	}
}


gen_color(c,p,v,h)
CVECTOR *c;
SVECTOR *p;
long v,h;       
{
        int i,j;
        for(j=0;j<=v;j++){
            for(i=0;i<=h;i++){
		if((p+j*(h+1)+i)->vz<=0){
                	(c+j*(h+1)+i)->r= -((p+j*(h+1)+i)->vz)*255/CHIGHT;
                	(c+j*(h+1)+i)->g= -((p+j*(h+1)+i)->vz)*255/CHIGHT;
                	(c+j*(h+1)+i)->b= -((p+j*(h+1)+i)->vz)*255/CHIGHT;
		}else{
                	(c+j*(h+1)+i)->r= 0;
                	(c+j*(h+1)+i)->g= 0;
                	(c+j*(h+1)+i)->b= 0;
		}
            }
        }
}


gen_texture(t,v,h)
SVECTOR *t;
long v,h;
{
	int i,j;

        for(i=0;i<=v;i++){
                for(j=0;j<=h;j++){
                        (t+i*(h+1)+j)->vx= (256/h -1)*i;
                        (t+i*(h+1)+j)->vy= (256/v -1)*j;
                }
        }
}

CopyMeshUV(mesh,pckt)
QMESH *mesh;
POLY_FT4 *pckt;
{
        long i,j;
        long X,Y;

        X = mesh->lenh-1;
        Y = mesh->lenv-1;

        for(i=0;i<Y;i++){
            for(j=0;j<X;j++){

                (pckt+i*X+j)->u0 = (mesh->u+i*(X+1)+j)->vx;
                (pckt+i*X+j)->v0 = (mesh->u+i*(X+1)+j)->vy;

                (pckt+i*X+j)->u1 = (mesh->u+i*(X+1)+j+1)->vx;
                (pckt+i*X+j)->v1 = (mesh->u+i*(X+1)+j+1)->vy;

                (pckt+i*X+j)->u2 = (mesh->u+(i+1)*(X+1)+j)->vx;
                (pckt+i*X+j)->v2 = (mesh->u+(i+1)*(X+1)+j)->vy;

                (pckt+i*X+j)->u3 = (mesh->u+(i+1)*(X+1)+(j+1))->vx;
                (pckt+i*X+j)->v3 = (mesh->u+(i+1)*(X+1)+(j+1))->vy;

            }
        }
}

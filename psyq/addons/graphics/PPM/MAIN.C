/* $PSLibId: Runtime Library Release 3.6$ */
/*				
 *
 *		FLAT TEXTURE 4角形の完全透視変換プログラム
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>


#define TEX_ADDR   0x80180000
#define TIM_HEADER 0x00000010

#define SCR_Z	(128)			/* screen depth (h) */
#define OTLEN	8
#define	OTSIZE	(1<<OTLEN)		/* ordering table size */
#define CUBESIZ	200			/* cube size */


#define PIH             320
#define PIV             240

#define OFX             (PIH/2)                 /* screen offset X */
#define OFY             (PIV/2)                 /* screen offset Y */
#define BGR             60                      /* BG color R */
#define BGG             120                     /* BG color G */
#define BGB             120                     /* BG color B */
#define RBK             0                       /* Back color R */
#define GBK             0                       /* Back color G */
#define BBK             0                       /* Back color B */
#define RFC             BGR                     /* Far color R */
#define GFC             BGG                     /* Far color G */
#define BFC             BGB                     /* Far color B */
#define FOGNEAR         1000                    /* Fog near point */
#define DPQ             0                       /*1:ON,0:OFF*/



typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	BLK_FILL	bg;			/* background */
} DB;



u_short tpage;
u_short clut;


u_short clut4[16];
u_char  gtext[256*256];
u_short dtext[256*256];

static pad_read();
static init_prim();
static add_cube();
static get_texture();
static div_clut4();
static get_texture4();
static get_dir_texture4();


main()
{
	DB		db[2];		/* packet double buffer */
	DB		*cdb;		/* current db */
	MATRIX		rottrans;	/* rot-trans matrix */
	int		dmy, flg;	/* dummy */
        int             i,j,k;
        long            ret;
        long            id;
	MATRIX		ll;		/* local light Matrix */
	MATRIX		lc;		/* local color Matrix */
	CVECTOR		bk;	/* ambient color */
	GsIMAGE		tim;

        int             c;
	int		abuf;

	
	ResetCallback();
	PadInit(0);		/* reset graphic environment */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(OFX, OFY);	/* set geometry origin as (160, 120) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */
        SetBackColor(RBK,GBK,BBK);      /* set background(ambient) color*/
        SetFarColor(RFC,GFC,BFC);       /* set far color */
        SetFogNear(FOGNEAR,SCR_Z);      /* set fog parameter*/

	/*interlace mode*/	
	SetDefDrawEnv(&db[0].draw, 0,   0, PIH, PIV);	
	SetDefDrawEnv(&db[1].draw, 0,   PIV, PIH, PIV);	
	SetDefDispEnv(&db[0].disp, 0,   PIV, PIH, PIV);	
	SetDefDispEnv(&db[1].disp, 0,   0, PIH, PIV);

	get_texture();		/* load TIM data to MRAM */

	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	init_prim(&db[0]);
	init_prim(&db[1]);


	ret=0;
	while(pad_read(&rottrans)==0){
	   	cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		abuf = 1-(cdb==db);

	   	ClearOTagR(cdb->ot, OTSIZE);
	   	DrawSync(0);		/* wait for end of BG drawing */

		add_cube(abuf,&rottrans);

	   	VSync(0);	/* wait for the next V-BLNK */
	
	   	PutDrawEnv(&cdb->draw); /* update drawing environment */
	   	PutDispEnv(&cdb->disp); /* update display environment */

	   	DrawOTag(cdb->ot+OTSIZE-1);	/* draw BG*/
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
	static VECTOR	vec  = {0, 0, 2*SCR_Z};	/* rottranslate vector */

	int	ret = 0;	

	u_long	padd = PadRead(0);

	/* rotate light source and cube */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* change distance */
	if (padd & PADl)	vec.vz += 8;
	if (padd & PADn) 	vec.vz -= 8;
	if (padd & PADk) 	ret = -1;

	if (padd & PADLup)	vec.vy -= 8;
	if (padd & PADLdown)	vec.vy += 8;
	if (padd & PADLleft)	vec.vx -= 8;
	if (padd & PADLright)	vec.vx += 8;

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
	long	i;

	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);	/* (r,g,b) = (60,120,120) */
}




/*
 *
 *      Simple Cube Handler
 */
static SVECTOR P0 = {-CUBESIZ/2,-CUBESIZ/2,-CUBESIZ/2,0};
static SVECTOR P1 = { CUBESIZ/2,-CUBESIZ/2,-CUBESIZ/2,0};
static SVECTOR P2 = { CUBESIZ/2, CUBESIZ/2,-CUBESIZ/2,0};
static SVECTOR P3 = {-CUBESIZ/2, CUBESIZ/2,-CUBESIZ/2,0};

static SVECTOR P4 = {-CUBESIZ/2,-CUBESIZ/2, CUBESIZ/2,0};
static SVECTOR P5 = { CUBESIZ/2,-CUBESIZ/2, CUBESIZ/2,0};
static SVECTOR P6 = { CUBESIZ/2, CUBESIZ/2, CUBESIZ/2,0};
static SVECTOR P7 = {-CUBESIZ/2, CUBESIZ/2, CUBESIZ/2,0};

static SVECTOR  *v[6*4] = {
        &P0,&P1,&P2,&P3,
        &P1,&P5,&P6,&P2,
        &P5,&P4,&P7,&P6,
        &P4,&P0,&P3,&P7,
        &P4,&P5,&P1,&P0,
        &P6,&P7,&P3,&P2,
};

static int tex[4][2] = { {0,0},{256,0},{256,256},{0,256}};

static add_cube(abuf,rottrans)
int	abuf;
MATRIX	*rottrans;
{
        int     i;
        long    p, otz, opz, flg;
        int     isomote;
	SVECTOR **vp;
        MATRIX  m;
	short	vertex[4][2];
	VECTOR	vert3d[4];

        vp = v;                         /* vp: vertex pointer (work) */

        for (i = 0; i < 6; i++, vp += 4) {

		SetGeomOffset(OFX, OFY);
		SetGeomScreen(SCR_Z);
		SetRotMatrix(rottrans);
		SetTransMatrix(rottrans);


		pers_map(abuf,vp,tex,dtext);	/*inverse perspective*/	

	}
}

/*
 *      Get Texture Info.
 */
static get_texture()
{
        u_long *timP;
	GsIMAGE TimInfo;


        timP = (u_long *)TEX_ADDR;
        while(1) {
                /* TIMデータがあるかどうか */
                if(*timP != TIM_HEADER) {
                        break;
                }

                /* ヘッダ良みとばし */
                timP++;

                /* TIMデータの位置情報を得る */
                GsGetTimInfo( timP, &TimInfo );

                timP += TimInfo.pw * TimInfo.ph/2+3+1;

                /* CLUT?*/
                if((TimInfo.pmode>>3)&0x01) {
                        timP += TimInfo.cw*TimInfo.ch/2+3;
                }

                /* print texture info. */
                /*prt_text_info(&TimInfo);*/

                /*divide 4bit clut to 16 words*/
                div_clut4(TimInfo.clut,clut4);

                /*get 4bit texture*/
                get_texture4(gtext,TimInfo.pixel);

                /*get direct color texture*/
                get_dir_texture4(gtext,clut4,dtext);

        }
}

static div_clut4(c,c4)
u_long c[8];
u_short c4[16];
{
        int     i;
        for(i=0;i<16;i++){
                if(i%2==0)
                        c4[i] = c[i/2]&0xffff;
                else
                        c4[i] = c[i/2]>>16;
        }
}

static get_texture4(t,p)
u_char  *t;
u_long  *p;
{
        int i,j;

        for(i=0;i<256;i++)
        for(j=0;j<32;j++){
                /*256x256texture earth4.tim*/
                t[i*256+j*8]=p[i*32+j]&0xf;
                t[i*256+j*8+1]= (p[i*32+j]&0xf0)>>4;
                t[i*256+j*8+2]= (p[i*32+j]&0xf00)>>8;
                t[i*256+j*8+3]= (p[i*32+j]&0xf000)>>12;
                t[i*256+j*8+4]= (p[i*32+j]&0xf0000)>>16;
                t[i*256+j*8+5]= (p[i*32+j]&0xf00000)>>20;
                t[i*256+j*8+6]= (p[i*32+j]&0xf000000)>>24;
                t[i*256+j*8+7]= (p[i*32+j]&0xf0000000)>>28;
        }
}

static get_dir_texture4(gtext,clut4,dtext)
u_char  *gtext;
u_short *clut4;
u_short *dtext;
{
        int     i,j;
        for(i=0;i<256;i++)
        for(j=0;j<256;j++){
                dtext[i*256+j]= clut4[gtext[i*256+j]];
        }
}


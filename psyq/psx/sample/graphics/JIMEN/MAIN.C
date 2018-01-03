/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	Sample program for Software Raster Conversion
 *
 *		1996,3,4	by Oka
 *		1997,3,5	by sachiko	(added autopad)
 *		Copyright (C) 1996/1997 by Sony Computer Entertainment
 *			All rights Reserved
 */

#include	<sys/types.h>
#include 	<libetc.h>
#include	<libgte.h>
#include	<libgpu.h>
#include	<libgs.h>
#include	<inline_c.h>
#include 	<gtemac.h>


#define INTERLACE

#define SCR_Z	512
#define NEARZ	(SCR_Z/16)
#define OTSIZE	256

				/* texture (pattern) data*/
#define TEX_ADDR 0x801a0000     /* texture start address*/
#define TIM_HEADER 0x00000010   /* TIM header value*/
GsIMAGE TimInfo;

#ifdef INTERLACE
#define PIV	480
#define PIH	640
#define FSTART	2
#else
#define PIV	240
#define PIH	320
#define FSTART	1
#endif

#define MAXPAC	(PIV*2)


u_short tpage;
u_short clut;

/* Vertex should be rectangle */

static SVECTOR    Vertex[4]={{-2000,0, 2000,0},
                             { 2000,0, 2000,0},
                             { 2000,0,-2000,0},
                             {-2000,0,-2000,0}};

/*
 *  Other ...*/
u_long PadData;			/* Controller pad data*/

typedef struct {
        DRAWENV         draw;           /* drawing environment*/
        DISPENV         disp;           /* display environment*/
        u_long          ot[OTSIZE];     /* OT*/
        POLY_FT4        s[MAXPAC];  	/* softwear scan conv. packet area */
        POLY_FT3        ss[PIV];  	/* softwear scan conv. packet area */
	LINE_F2		normal;		/* normal vector */
	LINE_F2		sect;		/* intersection vector */
	POLY_FT4	divs[32*32];	/* divide polygon mode packet area */
} DB;


static	DB	db[2];
static 	DB	*cdb;


static	drawAll();
static	initSystem();
static  init_packet();
static 	get_texture();

static	int	POLYDIV=0;

/*
 *  main routine*/
main(void)
{
	int	c;

	initSystem();
	get_texture();

	init_packet(&db[0]);
	init_packet(&db[1]);

	div_init();

        FntLoad(960,256);
        SetDumpFnt(FntOpen(64, 64, 256, 200, 0, 512));

        GsInitVcount();

	while(1) {
		if(drawAll()) break;
	}

	PadStop();
	ResetGraph(1);
	StopCallback();
	return ;
}

/*
 *  render*/
static drawAll()
{
	int activeBuff;		/* pointer to active buffer*/
	int i,j;
	int	c,c2,d;

	static 	int	divnn=10;
	static	int	ndivv=30;
	int	divn;		/* Softwear scan conv. divide number*/
	int	ndiv;		/* Polygon divide number */

	static	int	divmodee=00;
	static	int	divmode;
	
	static SVECTOR  ang={0,0,0,0};

#ifdef INTERLACE
	static VECTOR   vec={0,400,2000};
#else
	static VECTOR   vec={0,400,2000};
#endif


	MATRIX	m;
#ifdef INTERLACE
	static 	VECTOR	sca={4096,4096,4096,0};
#else
	static 	VECTOR	sca={2048,2048,4096,0};
#endif

	static	txs0=256;
	static	txs1=256;

	GsClearVcount();

	cdb= (cdb==db)? db+1: db;
	activeBuff = (cdb==db);

	ClearOTagR(cdb->ot, OTSIZE);

	PadData = PadRead(1);
        if((PadData & PADRup)>0) 		ang.vx +=16;
        if((PadData & PADRdown)>0) 		ang.vx -=16;
        if((PadData & PADRleft)>0) 		ang.vy +=16;
        if((PadData & PADRright)>0) 		ang.vy -=16;
        if((PadData & PADn)<=0&&
		(PadData & PADl)<=0&&
		(PadData & PADo)<=0&&
		(PadData & PADh)<=0&&
		(PadData & PADm)>0)		ang.vz +=16; 
        if((PadData & PADn)<=0&&
		(PadData & PADl)<=0&&
		(PadData & PADm)<=0&&
		(PadData & PADh)<=0&&
		(PadData & PADo)>0)		ang.vz -=16;

        if((PadData & PADLup)>0) 		vec.vy -=16;
        if((PadData & PADLdown)>0) 		vec.vy +=16;
        if((PadData & PADLleft)>0) 		vec.vx -=16;
        if((PadData & PADLright)>0) 		vec.vx +=16;
        if((PadData & PADm)<=0&&
		(PadData & PADo)<=0&&
		(PadData & PADn)<=0&&
		(PadData & PADh)<=0&&
		(PadData & PADl)>0) 		vec.vz +=16;
        if((PadData & PADm)<=0&&
		(PadData & PADo)<=0&&
		(PadData & PADl)<=0&&
		(PadData & PADh)<=0&&
		(PadData & PADn)>0) 		vec.vz -=16;

	if(((PadData & PADh)>0)&&
		((PadData & PADm)>0)&&
		(divmode==0||divmode==1))	divnn++;
	if(((PadData & PADh)>0)&&
		((PadData & PADo)>0)&&
		(divmode==0||divmode==1)) 	divnn--;
	if(((PadData & PADh)>0)&&
		((PadData & PADm)>0)&&
		(divmode==2)) 			ndivv++;
	if(((PadData & PADh)>0)&&
		((PadData & PADo)>0)&&
		(divmode==2)) 			ndivv--;

	if(divnn<10) divnn=10;
	divn= divnn/10;

	if(ndivv<00) ndivv=00;
	if(ndivv>50) ndivv=50;
	ndiv= ndivv/10;

	if((PadData & PADh)>0&&(PadData & PADl)>0) divmodee++;
	if((PadData & PADh)>0&&(PadData & PADn)>0) divmodee--;
	if(divmodee<00) divmodee=00;
	if(divmodee>20) divmodee=20;
	divmode= divmodee/10;

	if((PadData & PADm)>0&&(PadData & PADl)>0) txs0++;
	if((PadData & PADm)>0&&(PadData & PADn)>0) txs0--;
	if((PadData & PADo)>0&&(PadData & PADn)>0) txs1++;
	if((PadData & PADo)>0&&(PadData & PADl)>0) txs1--;
	if(txs0>256)txs0=256;
	if(txs0<0)txs0=0;
	if(txs1>256)txs1=256;
	if(txs1<0)txs1=0;

        /* exit with K button */
        if((PadData & PADk)>0) return -1;
/*
	if((PadData & PADm)>0){
		printf("ang=(%d,%d,%d)\n",ang.vx,ang.vy,ang.vz);
		printf("vec=(%d,%d,%d)\n",vec.vx,vec.vy,vec.vz);
	}
*/
	RotMatrix(&ang,&m);
	m.t[0]= vec.vx;
	m.t[1]= vec.vy;
	m.t[2]= vec.vz;

	ScaleMatrixL(&m,&sca);

	SetRotMatrix(&m);
	SetTransMatrix(&m);

	switch(divmode){
	case	0:
		draw_general_square(Vertex,cdb->ot,cdb->s,divn,txs0,txs1);
		break;
	case	1:
		ang.vz=0;
		if(divn==1){
		    draw_ground_square(Vertex,cdb->ot,cdb->ss,txs0,txs1);
		}else{
		    draw_ground_square2(Vertex,cdb->ot,cdb->s,divn,txs0,txs1);
		}
		break;
	case	2:
		divide_polygon(Vertex,cdb,ndiv);
		break;
	default:
		break;
	}

	c=GsGetVcount();

	DrawSync(0);
	d=GsGetVcount();

	switch(divmode){
	case	0:
		FntPrint("DRAW_GENRAL_SQUARE\n");
		FntPrint("DIVN=%d\n",divn);
		FntPrint("CAL=%d\n",c);
		FntPrint("DRW=%d\n",d);
		FntPrint("TXS=(%d,%d)\n",txs0,txs1);
		break;
	case	1:
		FntPrint("DRAW_GROUND_SQUARE\n");
		FntPrint("NO Z-AXIS ROTATION\n");
		FntPrint("DIVN=%d\n",divn);
		FntPrint("CAL=%d\n",c);
		FntPrint("DRW=%d\n",d);
		FntPrint("TXS=(%d,%d)\n",txs0,txs1);
		break;
	case	2:
		FntPrint("DIVIDE FT4\n");
		switch(ndiv){
		case 0:	
			FntPrint("NDIV=1x1\n");
			break;
		case 1:	
			FntPrint("NDIV=2x2\n");
			break;
		case 2:	
			FntPrint("NDIV=4x4\n");
			break;
		case 3:	
			FntPrint("NDIV=8x8\n");
			break;
		case 4:	
			FntPrint("NDIV=16x16\n");
			break;
		case 5:	
			FntPrint("NDIV=32x32\n");
			break;
		default:
			break;
		}
		FntPrint("CAL=%d\n",c);
		FntPrint("DRW=%d\n",d);
		break;
	default:
		break;
	}
/*
        FntPrint("ang={%d,%d,%d,0}\n",ang.vx,ang.vy,ang.vz);
        FntPrint("vec={%d,%d,%d,0}\n",vec.vx,vec.vy,vec.vz);
*/
	VSync(0);

	PutDrawEnv(&cdb->draw);
	PutDispEnv(&cdb->disp);

	DrawOTag(cdb->ot+OTSIZE-1);

	FntFlush(-1);	

	return 0;
}


/*
 *  initialize*/
static initSystem()
{
	int i;
	MATRIX	mat;
		
	/* initialize pad*/
	PadInit(0);
	ResetGraph(0);
	
	GsInitGraph(PIH,PIV,2,1,0);

	InitGeom();
	SetGeomScreen(SCR_Z);
	SetGeomOffset(PIH/2,PIV/2);

	/* initialize GPU*/
#ifdef INTERLACE
        SetDefDrawEnv(&db[0].draw, 0,   0, PIH, PIV);
        SetDefDrawEnv(&db[1].draw, 0,   0, PIH, PIV);
        SetDefDispEnv(&db[0].disp, 0,   0, PIH, PIV);
        SetDefDispEnv(&db[1].disp, 0,   0, PIH, PIV);
#else
        SetDefDrawEnv(&db[0].draw, 0,   0, PIH, PIV);
        SetDefDrawEnv(&db[1].draw, 0,   PIV, PIH, PIV);
        SetDefDispEnv(&db[0].disp, 0,   PIV, PIH, PIV);
        SetDefDispEnv(&db[1].disp, 0,   0, PIH, PIV);
#endif
	SetDispMask(1);

        db[0].draw.isbg = 1;
        setRGB0(&db[0].draw, 60, 120, 120);       /* (r,g,b) = (60,120,120) */
        db[1].draw.isbg = 1;
        setRGB0(&db[1].draw, 60, 120, 120);       /* (r,g,b) = (60,120,120) */
	
	/*texture window*/
	db[0].draw.tw.w= 64;
	db[0].draw.tw.h= 64;
	db[1].draw.tw.w= 64;
	db[1].draw.tw.h= 64;
}


/*
 *	Init GPU packet
 */
static init_packet(db)
DB	*db;
{
	int	i;

	for(i=0;i<MAXPAC;i++){
	    SetPolyFT4(&db->s[i]);
	    db->s[i].tpage = tpage;
	    db->s[i].clut  = clut;
	    db->s[i].r0= 128;	
	    db->s[i].g0= 128;	
	    db->s[i].b0= 128;
	}

	for(i=0;i<PIV;i++){
	    SetPolyFT3(&db->ss[i]);
	    db->ss[i].tpage = tpage;
	    db->ss[i].clut  = clut;
	    db->ss[i].r0= 128;	
	    db->ss[i].g0= 128;	
	    db->ss[i].b0= 128;
	}

	SetLineF2(&db->normal);
	db->normal.r0= 128;	
	db->normal.g0= 0;	
	db->normal.b0= 0;

	SetLineF2(&db->sect);
	db->sect.r0= 0;	
	db->sect.g0= 128;	
	db->sect.b0= 0;

	for(i=0;i<32*32;i++){
	    SetPolyFT4(&db->divs[i]);
	    db->divs[i].tpage = tpage;
	    db->divs[i].clut  = clut;
	    db->divs[i].r0= 128;	
	    db->divs[i].g0= 128;	
	    db->divs[i].b0= 128;
	    db->divs[i].u0= 0;
	    db->divs[i].v0= 0;
	    db->divs[i].u1= 255;
	    db->divs[i].v1= 0;
	    db->divs[i].u2= 0;
	    db->divs[i].v2= 255;
	    db->divs[i].u3= 255;
	    db->divs[i].v3= 255;
	}
}

/*
 *	Get Texture Info.
 */
static get_texture()
{
	u_long *timP;
	RECT	rect;
	int	i;

        timP = (u_long *)TEX_ADDR;
        while(1) {
                /* see if there is TIM data*/
                if(*timP != TIM_HEADER) {
                        break;
                }

                /* read through header*/
                timP++;

                /* get TIM data position information*/
                GsGetTimInfo( timP, &TimInfo );

  		rect.x=TimInfo.px;
  		rect.y=TimInfo.py;
  		rect.w=TimInfo.pw;
  		rect.h=TimInfo.ph;

		LoadImage(&rect,TimInfo.pixel);

		tpage= GetTPage(TimInfo.pmode,0,640,0);
		tpage= tpage&0xfe7f;          /*for GetTPage bug*/

                timP += TimInfo.pw * TimInfo.ph/2+3+1;

                /* CLUT?*/
                if((TimInfo.pmode>>3)&0x01) {
                        timP += TimInfo.cw*TimInfo.ch/2+3;
      			rect.x=TimInfo.cx;
      			rect.y=TimInfo.cy;
      			rect.w=TimInfo.cw;
      			rect.h=TimInfo.ch;

			/*semitrans bit ON*/
			for(i=0;i<16;i++){ 
				*((short*)TimInfo.clut+i) |= 0x8000;
			}

      			LoadImage(&rect,TimInfo.clut);
                }
		clut=GetClut(TimInfo.cx,TimInfo.cy);
        }
}


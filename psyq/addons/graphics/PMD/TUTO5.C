/* $PSLibId: Runtime Library Release 3.6$ */
/*				
 *
 *		test program for inline RotPMD
 *
 *		Copyright (C) 1993/1994/1995 by Sony Corporation
 *			All rights Reserved
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <inline_c.h>
#include <gtemac.h>

#define SCR_Z	(1000)			/* screen depth (h) */
#define OTLEN	8
#define	OTSIZE	(1<<OTLEN)		/* ordering table size */

#define RECTH           25
#define RECTV           20
#define RECTD		16

#define PIH             640
#define PIV             240

#define OFX             (PIH/2)                 /* screen offset X */
#define OFY             (PIV/2)                 /* screen offset Y */

#define BGR             60                      /* BG color R */
#define BGG             120                     /* BG color G */
#define BGB             120                     /* BG color B */


typedef struct {
	POLY_F3        	surf[2];
	SVECTOR		v0,v1,v2;
} PBUF;

typedef struct {
        int     n;
        PBUF    pb[RECTD][RECTV][RECTH];
} PMD_OBJ;

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	BLK_FILL	bg;			/* background */
} DB;

typedef struct{
	long	flg;
	long	opz;
	long	otz;
} WORK;

static	int	SELECT=0;

static	pad_read();
static	init_prim(PBUF pb[RECTD][RECTV][RECTH]);

main()
{
	PMD_OBJ		obj;
	DB		db[2];		/* packet double buffer */
	DB		*cdb;		/* current db */
        int             i,j,k;
        long            ret;
        long            id;
	int		c;
	
	obj.n= RECTD*RECTV*RECTH;

	ResetCallback();
	PadInit(0);		/* reset graphic environment */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(OFX, OFY);	/* set geometry origin as (160, 120) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */

	/*interlace mode*/	
	SetDefDrawEnv(&db[0].draw, 0,   0, PIH, PIV);	
	SetDefDrawEnv(&db[1].draw, 0,   PIV, PIH, PIV);	
	SetDefDispEnv(&db[0].disp, 0,   PIV, PIH, PIV);	
	SetDefDispEnv(&db[1].disp, 0,   0, PIH, PIV);

	FntLoad(960,256);
	SetDumpFnt(FntOpen(64, 64, 256, 200, 0, 512));

	init_bg(&db[0]);
	init_bg(&db[1]);
	init_prim(obj.pb);	/* set primitive parameters */

	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	GsInitVcount();

	ret=0;
	while (pad_read() == 0) {
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */

		GsClearVcount();

		if(SELECT==0)
		  RotPMD_F3((long*)&obj,(u_long*)cdb->ot,OTLEN,(cdb==db),0);
		if(SELECT==1)
		  gte_RotPMD_F3((long*)&obj,(u_long*)cdb->ot,OTLEN,(cdb==db),0);
		if(SELECT==2)
		  gte_RotRMD_F3((long*)&obj,(u_long*)cdb->ot,OTLEN,(cdb==db));
		if(SELECT==3)
		  gte_RotSMD_F3((long*)&obj,(u_long*)cdb->ot,OTLEN,(cdb==db));
/*
		RotSMD_F3((long*)&obj,
				(u_long*)cdb->ot,OTLEN,(cdb==db),0,0,0,1);
		RotRMD_F3((long*)&obj,
				(u_long*)cdb->ot,OTLEN,(cdb==db),0,0,0,1);
*/

		c=GsGetVcount();

		/* swap buffer */
		DrawSync(0);	/* wait for end of drawing */

		if(SELECT==0)FntPrint("RotPMD_F3\n");
		if(SELECT==1)FntPrint("gte_RotPMD_F3\n");
		if(SELECT==2)FntPrint("gte_RotRMD_F3\n");
		if(SELECT==3)FntPrint("gte_RotSMD_F3\n");
                FntPrint("c=%d\n",c);

		VSync(0);	/* wait for the next V-BLNK */
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */

		DrawOTag(cdb->ot+OTSIZE-1);	/* draw */
		FntFlush(-1);
	}
	PadStop();
	ResetGraph(3);
	StopCallback();
	return;
}


static pad_read()
{
	MATRIX	rottrans;		/* rot-trans matrix, light matrix */
	static SVECTOR	ang  = { 0, 0, 0};	/* rotate angle */
	static VECTOR	vec  = {0, 0, 10*SCR_Z};/* rottranslate vector */
	static VECTOR	sca={4096,2048,4096};

	int	ret = 0;	

	u_long	padd = PadRead(0);

	/* rotate light source and cube */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;
	if (padd & PADm) 	ang.vx += 32;
	if (padd & PADo)	ang.vx -= 32;

	/* change distance */
	if (padd & PADl)	vec.vz += 32;
	if (padd & PADn) 	vec.vz -= 32;

	if (padd & PADLup) 	SELECT=1;
	if (padd & PADLdown) 	SELECT=0;
	if (padd & PADLright) 	SELECT=2;
	if (padd & PADLleft) 	SELECT=3;

	if (padd & PADk) 	ret = -1;

	RotMatrix(&ang, &rottrans);	/* make rot-trans matrix  */
	ScaleMatrixL(&rottrans,&sca);
	
	/* set matrix */
	TransMatrix(&rottrans, &vec);	
	SetRotMatrix(&rottrans);
	SetTransMatrix(&rottrans);

	return(ret);
}		

/*
 *	initialize primitive parameters
 */
init_bg(db)
DB      *db;
{
	db->draw.isbg = 1;
	setRGB0(&db->draw, BGR, BGG, BGB);

}

static init_prim(pb)
PBUF	pb[RECTD][RECTV][RECTH];
{
	int	i,j,k;

	for(k=0;k<RECTD;k++){
	    for(j=0;j<RECTV;j++){
	    	for(i=0;i<RECTH;i++){
			SetPolyF3(&pb[k][j][i].surf[0]);
			SetPolyF3(&pb[k][j][i].surf[1]);

			setRGB0(&pb[k][j][i].surf[0],10*i,10*j,10*k);
			setRGB0(&pb[k][j][i].surf[1],10*i,10*j,10*k);

                        pb[k][j][i].v0.vx= -150*12+	150*i;
                        pb[k][j][i].v0.vy= -150*10+	150*j;
                        pb[k][j][i].v0.vz= -150*4+	150*k;

                        pb[k][j][i].v1.vx= -150*12+	150*i	+100;
                        pb[k][j][i].v1.vy= -150*10+	150*j;
                        pb[k][j][i].v1.vz= -150*4+ 	150*k;

                        pb[k][j][i].v2.vx= -150*12+	150*i;
                        pb[k][j][i].v2.vy= -150*10+ 	150*j	+100;
                        pb[k][j][i].v2.vz= -150*4+	150*k;
	    	}
	    }
	}
}


gte_RotPMD_F3(pa,ot,otlen,id,backc)
long	*pa;		/*header address of PRIMITIVE Gp*/
u_long *ot;		/*header address of OT*/
int otlen;		/*OT bit length*/
int id;			/*double buffer ID*/
int backc;		/*backface clip ON/OFF flag(0=ON)*/
{
	int	pn;
	int	backcc;
	long	*pc;
	long	*pd;
	long	*pe;
	int	otsft;
	u_long	padrs;
	u_long	*ott;

	WORK	*SCw;

	SCw= (WORK*)getScratchAddr(100);

	backcc= backc;
	pn= *pa;
	pc= pa+1;
	pe= pc+5*2;
	otsft= 14-otlen;

	gte_ldv3c(pe);
	while(pn>0){
		gte_rtpt();
		  pn--;				/*parallel process*/
		  if(id!=0) pd= pc+ 5;		/**/
		  else pd= pc;			/**/
		  pc += 5*2+6;			/**/
		  pe += 5*2+6;			/**/
		gte_stflg(&SCw->flg);

		gte_ldv3c(pe);			/*faster than before gte_rtpt*/

		if((SCw->flg&0x00060000)!=0) continue;

		gte_nclip();
		if(backcc==0){			/*parallel process*/
		    gte_stopz(&SCw->opz);
		    if(SCw->opz<=0) continue;
		}

		gte_stsxy3_f3(pd);
		gte_avsz3();
		  padrs= (u_long)pd&0xffffff;	/*parallel process*/
		gte_stotz(&SCw->otz);
		ott= ot+(SCw->otz>>otsft);

		*((u_long*)padrs)= *ott|0x04000000;
		*ott= padrs;
	}
}

gte_RotRMD_F3(pa,ot,otlen,id)
long	*pa;		/*header address of PRIMITIVE Gp*/
u_long *ot;		/*header address of OT*/
int otlen;		/*OT bit length*/
int id;			/*double buffer ID*/
{
	int	pn;
	long	*pc;
	long	*pd;
	long	*pe;
	int	otsft;
	u_long	padrs;
	u_long	*ott;

	WORK	*SCw;

	SCw= (WORK*)getScratchAddr(100);

	pn= *pa;
	pc= pa+1;
	pe= pc+5*2;
	otsft= 14-otlen;

	gte_ldv3c(pe);
	while(pn>0){
		gte_rtpt();
		pn--;
		if(id!=0) pd= pc+ 5;
		else pd= pc;
		pc += 5*2+6;
		pe += 5*2+6;
		gte_stflg(&SCw->flg);

		gte_ldv3c(pe);

		if((SCw->flg&0x00060000)!=0) continue;

		gte_stsxy3_f3(pd);
		gte_avsz3();
		padrs= (u_long)pd&0xffffff;
		gte_stotz(&SCw->otz);
		ott= ot+(SCw->otz>>otsft);

		*((u_long*)padrs)= *ott|0x04000000;
		*ott= padrs;
	}
}

gte_RotSMD_F3(pa,ot,otlen,id)
long	*pa;		/*header address of PRIMITIVE Gp*/
u_long *ot;		/*header address of OT*/
int otlen;		/*OT bit length*/
int id;			/*double buffer ID*/
{
	int	pn;
	long	*pc;
	long	*pd;
	long	*pe;
	int	otsft;
	u_long	padrs;
	u_long	*ott;

	WORK	*SCw;

	SCw= (WORK*)getScratchAddr(100);

	pn= *pa;
	pc= pa+1;
	pe= pc+5*2;
	otsft= 14-otlen;

	gte_ldv3c(pe);
	while(pn>0){
		gte_rtpt();
		pn--;
		if(id!=0) pd= pc+ 5;
		else pd= pc;
		pc += 5*2+6;
		pe += 5*2+6;
		gte_stflg(&SCw->flg);

		gte_ldv3c(pe);

		if((SCw->flg&0x00060000)!=0) continue;

		gte_nclip();
	        gte_stopz(&SCw->opz);
		if(SCw->opz<=0) continue;

		gte_stsxy3_f3(pd);
		gte_avsz3();
		padrs= (u_long)pd&0xffffff;
		gte_stotz(&SCw->otz);
		ott= ot+(SCw->otz>>otsft);

		*((u_long*)padrs)= *ott|0x04000000;
		*ott= padrs;
	}
}

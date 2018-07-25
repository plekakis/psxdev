/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	"tuto0.c" HMD ANIMATION viewer 
 *
 *		Copyright (C) 1997  Sony Computer Entertainment
 *		All rights Reserved
 */

/* #define DEBUG		/**/

/*
#define INTER		/* Interlace mode */

#include <sys/types.h>

#include <libetc.h>		/* for Control Pad */
#include <libgte.h>		/* LIBGS uses libgte */
#include <libgpu.h>		/* LIBGS uses libgpu */
#include <libgs.h>		/* for LIBGS */
#include <libhmd.h>             /* for LIBHMD */
#include "../common/scan.h"

#define OBJECTMAX 100		/* Max Objects */

#define MODEL_ADDR (u_long *)0x80010000
				/* Top Address of modeling data (HMD FORMAT) */

#define OT_LENGTH 10		/* Area of OT */

/*#define PACKETMAX (10000*24)*/
#define PACKETMAX (8000*24)


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH];
				/* Area of OT */

GsUNIT		object[OBJECTMAX];
				/* Array of Object Handler */

u_long		bnum;		/* valibable of number of Objects */
long            anum;           /* animation sequence num */

GsCOORDUNIT	*DModel = NULL;	/* Coordinate for GsUNIT */

SVECTOR         PModel;

/* work short vector for making Coordinate parmeter */

GsRVIEW2	view;		/* View Point Handler */
GsF_LIGHT	pslt[3];	/* Flat Lighting Handler */
u_long		padd;		/* Controler data */

PACKET          out_packet[2][PACKETMAX];
				/* GPU PACKETS AREA */

GsSEQ  *seq[64];

/* 
 * prototype
 */
void init_all(void);
int  obj_interactive(void);
void set_coordinate(GsCOORDUNIT *coor);
void model_init(void);
void view_init(void);
void light_init(void);
void init_anim(void);
void animation_scan(void);


/************* MAIN START ******************************************/
main()
{
	GsUNIT	*op;		/* pointer of Object Handler */
	int	outbuf_idx;
	MATRIX	tmpls;
	int	i;
	
	ResetCallback();
	init_all();

	FntLoad(960, 256);
	FntOpen(16-160, 16-120, 256, 200, 0, 512); 

	GsInitVcount();
	while(1) {
		/* interactive parameter get */
		if(obj_interactive() == 0) {
			common_PadStop();
			ResetGraph(3);
			StopCallback();
			return 0;
		}
		GsSetRefView2(&view);	/* Calculate World-Screen Matrix */
		outbuf_idx=GsGetActiveBuff();
					/* Get double buffer index */
		GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);
		
		GsClearOt(0, 0, &Wot[outbuf_idx]);
					/* Clear OT for using buffer */
		
		for(i=0,op=object; i<bnum; i++,op++) {
			if (op->primtop == NULL)
				continue;
			if (op->coord) {
				/* Calculate Local-World Matrix */
				GsGetLwUnit(op->coord, &tmpls);
	
				/* Set LWMATRIX to GTE Lighting Registers */
				GsSetLightMatrix(&tmpls);
	
				/* Set LSAMTRIX to GTE Registers */
				GsGetLsUnit(op->coord, &tmpls);
	
				/* Set LSMATRIX to GTE Lighting Registers */
				GsSetLsMatrix(&tmpls);
			}

			/* Perspective Translate Object and Set OT */
			GsSortUnit(op, &Wot[outbuf_idx], getScratchAddr(0));
		}
		padd = common_PadRead();/* Readint Control Pad data */
		FntPrint("Hcount = %d\n", GsGetVcount()); /**/
#ifndef INTER
		DrawSync(0);
#endif /* !INTER */
		VSync(0);		/* Wait for VSYNC */
		GsClearVcount();
#ifdef INTER
		ResetGraph(1);		/* Reset GPU */
#endif /* INTER */
		GsSwapDispBuff();	/* Swap double buffer */
		/* Set SCREEN CLESR PACKET to top of OT */
		GsSortClear(0x0, 0x0, 0x0, &Wot[outbuf_idx]);

		/* Drawing OT */
		GsDrawOt(&Wot[outbuf_idx]);

		FntFlush(-1);
	}
}

int obj_interactive()
{
	static int num = 0;
	static u_long opadd = 0;
	int cnum;
	
	if (padd != opadd) {
	        if (padd & PADR2)	{num++;}
	        if (padd & PADL2)	{num--;}
		if (padd & PADstart)	seq[num]->sid++;
		if (padd & PADselect)	seq[num]->sid--;
		limitRange(num, 0, anum-1);
	}
	
	cnum = (((seq[num]->rewrite_idx-1)&0xffff)*4)/sizeof(GsCOORDUNIT);

	FntPrint("SEQ/COD=%d/%d  Sid = %d\n",num,cnum,seq[num]->sid);
	
	if (padd & PADRleft)	DModel[cnum].rot.vy += 5*ONE/360;
	if (padd & PADRright)	DModel[cnum].rot.vy -= 5*ONE/360;
	if (padd & PADRup)	DModel[cnum].rot.vx += 5*ONE/360;
	if (padd & PADRdown)	DModel[cnum].rot.vx -= 5*ONE/360;
	
	if (padd & PADLleft)	DModel[cnum].matrix.t[0] -= 10;
	if (padd & PADLright)	DModel[cnum].matrix.t[0] += 10;
	
	if (padd & PADLdown)	DModel[cnum].matrix.t[1] += 10;
	if (padd & PADLup)	DModel[cnum].matrix.t[1] -= 10;
	
	if (padd & PADL1)	DModel[cnum].matrix.t[2] += 50;
	if (padd & PADR1)	DModel[cnum].matrix.t[2] -= 50;
	
	if ((padd & PADselect) && (padd & PADstart)) {
		return(0);
	}
	
	set_coordinate(&DModel[cnum]);
	
	opadd = padd;
	return(1);
}


void init_all(void)		/* Initialize routine */
{
	ResetGraph(0);		/* reset GPU */
	common_PadInit();	/* Reset Controller */
	padd = 0;		/* controller value initialize */
#ifdef INTER
	GsInitGraph(640, 480, GsINTER|GsOFSGPU, 1, 0);
				/* rezolution set , interrace mode */
	GsDefDispBuff(0, 0, 0, 0);
				/* Double buffer setting */
#else /* INTER */
	GsInitGraph(320, 240, GsNONINTER|GsOFSGPU, 1, 0);
				/* rezolution set , non interrace mode */
	GsDefDispBuff(0, 0, 0, 240);
				/* Double buffer setting */
#endif /* INTER */

	GsInit3D();		/* Init 3D system */
	
	Wot[0].length = OT_LENGTH;/* Set bit length of OT handler */
	Wot[0].org = zsorttable[0];
				/* Set Top address of OT Area to OT handler */
	/* same setting for anoter OT handler */
	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	model_init();		/* Reading modeling data */
	view_init();		/* Setting view point */
	light_init();		/* Setting Flat Light */
	init_anim();
}

void view_init(void)		/* Setting view point */
{
	/*---- Set projection,view ----*/
	GsSetProjection(1000);	/* Set projection */
	
	/* Setting view point location */
	/*	view.vpx = 0; view.vpy = 0; view.vpz = -2000; */
	view.vpx = 0; view.vpy = 0; view.vpz = -8000;
	
	/* Setting focus point location */
	view.vrx = 0; view.vry = 0; view.vrz = 0;
	
	/* Setting bank of SCREEN */
	view.rz=0;
	
	/* Setting parent of viewing coordinate */
	view.super = WORLD;
	
	/* Calculate World-Screen Matrix from viewing paramter */
	GsSetRefView2(&view);
}


void light_init(void)		/* init Flat light */
{
	/* Setting Light ID 0 */	
	/* Setting direction vector of Light0 */
	pslt[0].vx = 100; pslt[0].vy = 100; pslt[0].vz = 100;

	/* Setting color of Light0 */
	pslt[0].r = 0xd0; pslt[0].g = 0xd0; pslt[0].b = 0xd0;

	/* Set Light0 from parameters */
	GsSetFlatLight(0, &pslt[0]);
	
	/* Setting Light ID 1 */
	pslt[1].vx = 20; pslt[1].vy = -50; pslt[1].vz = -100;
	pslt[1].r = 0x80; pslt[1].g = 0x80; pslt[1].b = 0x80;
	GsSetFlatLight(1, &pslt[1]);
	
	/* Setting Light ID 2 */
	pslt[2].vx = -20; pslt[2].vy = 20; pslt[2].vz = 100;
	pslt[2].r = 0x60; pslt[2].g = 0x60; pslt[2].b = 0x60;
	GsSetFlatLight(2, &pslt[2]);
	
	/* Setting Ambient */
	GsSetAmbient(0, 0, 0);
	
	/* Setting default light mode */	
	GsSetLightMode(0);
}

void init_anim()
{
  int i;
  
  /* update sequence to GsSEQ */
  for(i=0;i<anum;i++){
    seq[i]->ii     = 0xffff;
    seq[i]->ti     = seq[i]->start;
    seq[i]->aframe = 0xffff; /* endless loop */
    seq[i]->sid    = seq[i]->start_sid;
    if (seq[i]->speed == 0)
	seq[i]->speed  = 0x10;	/* normal play back */
  }
}



/* Set coordinte parameter from work vector */
void set_coordinate(GsCOORDUNIT *coor)
{
	/* Rotate Matrix */
	RotMatrix(&coor->rot, &coor->matrix);

	/* Clear flag becase of changing parameter */
	coor->flg = 0;
}

/* Read modeling data (HMD FORMAT) */
void model_init(void)
{
	u_long	*dop;
	int	i,ret;
	u_long	*oppp;
	
	dop = MODEL_ADDR;
				/* Top Address of MODELING DATA(HMD FORMAT) */
	GsMapUnit(dop);		/* Mappipng real address */
	dop++;			/* ID skip */
	dop++;			/* flag skip */
	dop++;			/* headder top skip */
	bnum = *dop;		/* Get number of Blocks */
	dop++;			/* skip block number */

	for (i = 0; i < bnum; i++) {
		GsTYPEUNIT	ut;

		object[i].primtop = (u_long *)dop[i];
		if (object[i].primtop == NULL)
			continue;

		GsScanUnit(object[i].primtop, 0, 0, 0);
		while (GsScanUnit(0, &ut, &Wot[0], getScratchAddr(0))) {
			if (((ut.type >> 24 == 0x00)	/* CTG 0: POLY */
			|| (ut.type >> 24 == 0x01)	/* CTG 1: SHARED */
			|| (ut.type >> 24 == 0x05)	/* CTG 5: GROUND */
			|| (ut.type >> 24 == 0x06)	/* CTG 6: ENVMAP(beta) */
			) && (ut.type & 0x00800000)) {	/* check INI bit */
				DModel = GsMapCoordUnit(MODEL_ADDR, ut.ptr);
				/* clear INI bit */
				ut.type &= (~0x00800000);
			}

			*ut.ptr = NULL;
			switch (ut.type >> 24) {
			case 0x00:	/* CTG 0: POLY or NULL */
				*ut.ptr = scan_poly(ut.type);
				break;
			case 0x01:	/* CTG 1: SHARED */
				*ut.ptr = scan_shared(ut.type);
				break;
			case 0x02:	/* CTG 2: IMAGE */
				*ut.ptr = scan_image(ut.type);
				/*
					The image is loaded only one time
					in this application.
				*/
				((GsUNIT_Funcp)(*ut.ptr))
					((GsARGUNIT *)getScratchAddr(0));
				*ut.ptr = (u_long)GsU_00000000;
				break;
			case 0x03:	/* CTG 3: ANIMATION */
				*ut.ptr = scan_anim(ut.type);
				if (*ut.ptr != NULL
				&& *ut.ptr != (u_long)GsU_00000000) {
					anum = GsLinkAnim(seq, ut.ptr);
					if (GsScanAnim(ut.ptr, 0) == 0) {
						printf("ScanAnim failed!!\n\n");
					} else {
						scan_interp();
					}
				}
				break;
			case 0x04:	/* CTG 4: MIMe */
				*ut.ptr = scan_mime(ut.type);
				switch (ut.type) {
				case GsVtxMIMe:
				case GsNrmMIMe:
				case GsJntAxesMIMe:
				case GsJntRPYMIMe:
					/*
						This application has
						no area for MIMePr.
					*/
					/*
					set_mimepr(ut.type, GsGetHeadpUnit());
					*/
					break;
				case GsRstVtxMIMe:
					GsInitRstVtxMIMe(
						ut.ptr, GsGetHeadpUnit());
					break;
				case GsRstNrmMIMe:
					GsInitRstNrmMIMe(
						ut.ptr, GsGetHeadpUnit());
					break;
				default:
					/* do nothing */;
				}
				break;
			case 0x05:	/* CTG 5: GROUND */
				*ut.ptr = scan_gnd(ut.type);
				break;
			case 0x06:	/* CTG 6: ENVMAP(beta) */
				*ut.ptr = scan_envmap(ut.type);
				break;
			default:
				/* do nothing */;
			}
			if (*ut.ptr == NULL) {
				printf("unsupported type 0x%08x\n", ut.type);
				*ut.ptr = (u_long)GsU_00000000;
			}

#ifdef DEBUG
			printf("DEBUG:block:%d, Code:0x%08x\n", i, ut.type);
#endif /* DEBUG */
		}
		object[i].coord = NULL;
	}

	if (DModel != NULL) {
		for(i = 1; i < bnum - 1; i++) {
			object[i].coord = &DModel[i - 1];
		}
	}
}

/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	Analog Controller "DualShock" Sample Program
 *
 *
 *	Copyright (C) 1998 by Sony Computer Entertainment
 *			All rights Reserved
 *
*/
#include <stdio.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libcd.h>
#include <libpad.h>
#include "sin.h"
#include "maps.h"
#include "move.h"
#include "main.h"

#define	PIH	320
#define	PIV	240
#define PACKETMAX	1024		/* Max GPU packets */
#define MDL_WAL0_ADR	((long*)0x80090000L)
#define MDL_WAL1_ADR	((long*)0x80090100L)
#define MDL_WAL2_ADR	((long*)0x80090200L)
#define MDL_WAL3_ADR	((long*)0x80090300L)
#define MDL_YUKA_ADR	((long*)0x80090400L)
#define TXT_WALL_ADR	((long*)0x80090800L)
#define TXT_YUKA_ADR	((long*)0x80092000L)

#define OT_LENGTH	9		/* bit length of OT */

static GsOT	Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; /* OT */

static GsF_LIGHT pslt[3];		/* Structure for parallel light source */
static PACKET packetArea[2][PACKETMAX*24]; /* GPU PACKETS AREA */
static GsFOGPARAM fogparam;
static short light;
static short nObj;

long* dop1;
long* dop2;
long* dop3;
long* dop4;
long* dop5;
long* dop6;

#define	BGR	0
#define	BGG	0
#define	BGB	4
#define	AMBIENT		(ONE/3)
#define ACT_VAIB_MAX	(255)	/* Max value of actuator vibration  */
#define ACT_VAIB_MIN	(100)	/* Minimum value of actuator vibration stop */
#define ACT_VAIB_HIT	(120)	/* Minimum value of actuator vabration stop */
#define ACT_LOW_IDLE	(0)	/* Actuator normal vabration value */

/* Controller Infromation buffer */
static unsigned char padbuff[2][34];
static unsigned char align[6]={0,1,0xFF,0xFF,0xFF,0xFF};
static struct {
	unsigned char Button;
	unsigned char Lock;
	unsigned char Motor0,Motor1;
	unsigned char Send;
} hist;

/* Connected Controller Type */
static char* padstr[] = {
	"NO CONTROLLER", "MOUSE", "NEGI-CON", "KONAMI-GUN", "STANDARD CONTROLLER",
	"ANALOG STICK",	"NAMCO-GUN", "ANALOG CONTROLLER"
};
/****************************************************************************

****************************************************************************/
static int MainRoutine(void);
static void init_all( void );
static void light_init(void);
static int texture_init(long* addr);
static long* model_init(long* adrs);
/****************************************************************************

****************************************************************************/
int main( void )
{
	/*///////////////////////////*/
	SetDispMask(0);
	ResetGraph(0);
	SetGraphDebug(0);
	ResetCallback();
	CdInit();
	CdSetDebug(0);

	/* Analog Controller Initialization */
	PadInitDirect(padbuff[0],padbuff[1]);
	PadStartCom();

	init_all();
	SetDispMask(1);

	FntLoad( 960, 256);
	FntOpen( -144, -104, 272, 200, 0, 512);

	if (MainRoutine() != 1){
                DrawSync(0);
                PadStop();
        }
        PadStopCom();
        ResetGraph(3);
        StopCallback();

	return 0;
}

long triming( unsigned char pad )
{
	long n;

	n = 0;
	if (pad<=0x60 ) n = (0x60-pad);
	if (pad>=0xa0 ) n = (0x9f-pad);

	return(n);
}

int MainRoutine(void)
{
	int n;
	int side;
	int rslt;
	int ids;
	int hsync;
	int limitt;
	int state;
	int actuate;
	unsigned char* padi;
	u_long padd;

	hist.Button = 0;
	hist.Lock = 0;
	hist.Motor0 = 0;
	hist.Motor1 = 0;
	hist.Send = 0;
	ids = hsync = limitt = state = actuate = 0;

	side = GsGetActiveBuff();
	while(1)
	{
		FntPrint("HSYNC = %d\n", hsync );

		padi = padbuff[0];
		padd = ~((padi[2]<<8) | (padi[3]));
                if (padd & PADselect ){
			if (PadInfoMode(0x00,2,0) != 0){ 
                        	PadSetMainMode(0x00,0,2);
                        	VSync(2);
                        	while (PadGetState(0x00) != 6){
                        	}
			}
                        return 1;
                }

		state = PadGetState(0x00);
		if (state==PadStateDiscon) {
			FntPrint("NO CONTROLLER\n" );
		} else {
			FntPrint("[%0d] %s\n\n", ids, padstr[ids] );
			switch((ids=PadInfoMode(0x00,InfoModeCurID,0))) {
				/* Analog Controller */
				case 7:
				if (PadInfoMode(0x00,InfoModeCurExID,0)) {

					/*// DUAL SHOCK //*/
					FntPrint("%02x %02x %02x %02x\n", padi[4], padi[5], padi[6], padi[7] );
					rslt = move_user_char( padi+4 );

					/* Setting vibration interval according to move
                                           speed */
					n = abs(triming(padi[7]))|(abs(triming(padi[6])) ? 20:0);
					if ( n>0x60 ) n = 0x60;
					limitt += n;
					if (limitt>=0x60) {
						hist.Motor0 = 1;
						limitt -= 0x60;
					} else	hist.Motor0 = 0;

					/* Setting impact rate on destroying wall */
					if (rslt!=0)
					{
						actuate = 8;
						hist.Motor1 = ACT_VAIB_MAX;
					}
					if (actuate>0)
					{
						if (--actuate==0 )
							hist.Motor1 = 0;
					}
				}
				break;
				/* Except analog controller */
				default:
				hist.Motor0 = 0;
				hist.Motor1 = 0;
				break;
			}
		}

		if (state == PadStateFindPad) {
			hist.Send = 0;
		}
		if ( hist.Send==0 ) {
			PadSetAct(0x00,&(hist.Motor0),2);
			if (state == PadStateFindCTP1) {
				hist.Send = 1;
			}
			if (state == PadStateStable) {
				if (PadSetActAlign(0x00,align)) {
					hist.Send = 1;
				}
			}
		}

		nObj  = calc_world_data( &view, &PVect );

		GsSetRefView2(&view);
		GsSetWorkBase((PACKET*)packetArea[side]);
		GsClearOt(0,0,&Wot[side]);

		pslt[0].r = pslt[0].g = pslt[0].b = light;
		pslt[0].vx = view.vrx - view.vpx;
		pslt[0].vy = 4096;
		pslt[0].vz = view.vrz - view.vpz;;
		GsSetFlatLight(0,&pslt[0]);

		draw_maps_data( &Wot[side] );
		PadSetAct(0x00, &hist.Motor0, 2);

		hsync = VSync(0);
		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(BGR,BGG,BGB,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);
	}
	return(0);
}

/* Initialization routines */
void init_all( void )
{
	/* Set resolution rate (interlace) */
	GsInitGraph(PIH,PIV,GsOFSGPU,1,0);
	if ( PIV<480 )
		GsDefDispBuff(0,0,0,PIV);
	else
		GsDefDispBuff(0,0,0,0);

	GsInit3D();			/* 3D system initialization */
	Wot[0].length=OT_LENGTH;	/* set resolution for OT handler */
	Wot[0].org=wtags[0];		/* set OT for OT handler */

	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);

	/* read texture data */
	texture_init( TXT_WALL_ADR);
	texture_init( TXT_YUKA_ADR);

	/* read modeling data */
	dop1 = model_init( MDL_WAL0_ADR);
	dop2 = model_init( MDL_WAL1_ADR);
	dop3 = model_init( MDL_WAL2_ADR);
	dop4 = model_init( MDL_WAL3_ADR);
	dop6 = model_init( MDL_YUKA_ADR);

	make_world_maps( 69, 69 );
	make_view_data();

	light_init();			/* Setting Flat Light */
	nObj  = calc_world_data( &view, &PVect );
}


/* pararrel light source setting */
void light_init(void)
{
	light = 192;

	/* Projection setting */
	GsSetProjection(256);

	pslt[0].r = pslt[0].g = pslt[0].b = light;
	pslt[0].vx = view.vrx - view.vpx;
	pslt[0].vy = 4096;
	pslt[0].vz = view.vrz - view.vpz;;
	GsSetFlatLight(0,&pslt[0]);

	/* Ambient setting */
	GsSetAmbient( AMBIENT, AMBIENT, AMBIENT );

	/* Setting default light source calculation */
	GsSetLightMode(1);

	fogparam.dqa = -912;
	fogparam.dqb = 5120*5120;
	fogparam.rfc = BGR; fogparam.gfc = BGG; fogparam.bfc = BGB;
	GsSetFogParam(&fogparam);
}


/* Loading texture data to VRAM */
int texture_init(long* addr)
{
	RECT rect;
	GsIMAGE tim1;

	/* Obtaining texture data type from TIM data header */
	GsGetTimInfo((u_long *)(addr+1),&tim1);

	rect.x=tim1.px;	/* X coordination value of upper-left of texture on VRAM */
	rect.y=tim1.py;	/* Y coordination value of upper left of texture on VRAM */
	rect.w=tim1.pw;	/* Texture width */
	rect.h=tim1.ph;	/* Texture hight */

	/* Loading texture to VRAM */
	LoadImage(&rect,tim1.pixel);
	/* Exist Color Lookup Table */
	if ((tim1.pmode>>3)&0x01) {
		rect.x=tim1.cx;	/* X coordination value of upper-left of clut on VRAM */
		rect.y=tim1.cy;	/* Y coordination value of upper-left of clut on VRAM */
		rect.w=tim1.cw;	/* Width of CLUT */
		rect.h=tim1.ch;	/* Height of CLUT */
		/* Loading clut */
		LoadImage(&rect,tim1.clut);
	}
	DrawSync(0);
	return(0);
}

/* Model Initialization */
long* model_init( long* adrs )
{
	long *dop;

	dop=adrs;
	dop++;
	GsMapModelingData(dop);
	dop++;
	dop++;
	return( dop );
}


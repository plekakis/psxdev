/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *   High-speed BG rendering sample
 *         (using GsSortFixBG32())
 *
 *        Version 1.00   Mar, 7, 1995
 *        Version 1.01   Mar, 5, 1997   sachiko    (added autopad) 
 *
 *        Copyright  (C)  1995 by Sony Computer Entertainment
 *             All rights Reserved
 **/

#include	<sys/types.h>
#include	<libetc.h>
#include	<libgte.h>
#include	<libgpu.h>
#include	<libgs.h>

/* GPU Info. */
#define	OT_LENGTH 4
GsOT WorldOT[2];
GsOT_TAG OTTags[2][1<<OT_LENGTH];

/* texture (pattern) data */
#define TEX_ADDR 0x80180000	/* texture start address */
#define	TIM_HEADER 0x00000010	/* TIM header value */
GsIMAGE TimInfo;

/*
 *  BG data */
#define N_BG 8			/* the number of BGs to prepare */
GsBG	BGData[N_BG];		/* BG data */
GsMAP	BGMap[N_BG];		/* map information */
#define CEL_ADDR  0x80140000	/* start address for cell data (shared by all frames) */

/* reserve workspace for GsSortFixBg32() (eight frames worth) */
#define BGWSIZE (((320/32+1)*(240/32+1+1)*6+4)*2+2)
u_long BGPacket[BGWSIZE*N_BG];

/* pointers to workspace for each frame */
u_long *BGWork[N_BG];

/* four frames worth of map data (BGD file) */
#define BGD0_ADDR 0x80100000	/* start address for map#0 */
#define BGD1_ADDR 0x80101000	/* start address for map#1 */
#define BGD2_ADDR 0x80102000	/* start address for map#2 */
#define BGD3_ADDR 0x80103000	/* start address for map#3 */
unsigned long *BgdAddr[N_BG] = {
	(unsigned long *)BGD0_ADDR,
	(unsigned long *)BGD1_ADDR,
	(unsigned long *)BGD2_ADDR,
	(unsigned long *)BGD3_ADDR
};

/* table to determine which map is used by a BG frame */
int MapIndex[8] = {
	0, 1, 2, 3, 1, 2, 3, 1	
};
int NumBG;			/* number of BG frames being displayed */

/*
 *  Other ... */
u_long PadData;			/* Controller pad data */
u_long OldPad;			/* data from previous frame */

/*
 *  main routine */
main(void)
{
	/* initialize system */
	ResetCallback();
	initSystem();

	/* other initializations */
	PadData = 0;
	NumBG = 1;
	initTexture();
	initBG();
	GsInitVcount();

	while(1) {
		if(moveBG()) break;
		drawAll();
	}

	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
}

/*
 *  render */
drawAll()
{
	int activeBuff;
	int i;
	
	printf("in drawAll() ");
	/* get which double buffer is active */
	activeBuff = GsGetActiveBuff();

	/* clear OT */
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* render BG */
	for(i = 0; i < NumBG; i++) {	
		GsSortFixBg32(BGData+i, BGWork[i], &WorldOT[activeBuff], N_BG-i);
	}

	/* read pad */
	OldPad = PadData;
	PadData = PadRead(1);

	/* V-Sync synchronization */
	VSync(0);

	ResetGraph(1);
	GsSwapDispBuff();

	GsSortClear(0, 0, 0, &WorldOT[activeBuff]);
	GsDrawOt(&WorldOT[activeBuff]);
}

/*
 *  move BG */
moveBG()
{
	int i;

	printf("in moveBG() ");
	
	/* up arrow = reduce number of BG frames to display */
	if(((PadData&PADLup)>0)&&
		((OldPad&PADLup)==0)&&
		(NumBG < N_BG)) NumBG++;

	/* down arrow = reduce number of BG frames to display */
	if(((PadData&PADLdown)>0)&&
		((OldPad&PADLdown)==0)&&
		(NumBG > 1)) NumBG--;

	/* BG display position */
	BGData[0].scrolly += 1;
	BGData[1].scrollx -= 1;
	BGData[1].scrolly -= 1;
	BGData[3].scrollx += 1;
	BGData[4].scrollx -= 1;
	BGData[4].scrolly += 2;
	BGData[5].scrollx += 2;
	BGData[6].scrolly -= 1;

	/* exit with K button */
	if((PadData & PADk)>0) return -1;

	return 0;
}

/*
 *  initialize */
initSystem()
{
	int i;
		
	/* initialize pad */
	PadInit(0);
	
	/* initialize GPU */
	GsInitGraph(320,240,0,0,0);
	GsDefDispBuff(0,0,0,240);

	/* initialize OT area */
	WorldOT[0].length=OT_LENGTH;
	WorldOT[0].org=OTTags[0];
	WorldOT[1].length=OT_LENGTH;
	WorldOT[1].org=OTTags[1];
	
	/* initialize 3D environment */
	GsInit3D();
}

/*
 *  read cell pattern (Texture pattern)
 *   (TIM data placed in memory beforehand is transferred to VRAM) 
 *  */
initTexture()
{
	RECT rect;
	u_long *timP;
	int i;

	/* start address of TIM data */	
	timP = (u_long *)TEX_ADDR;

	/* read through header */
	timP++;

	/* get TIM data position information */
	GsGetTimInfo( timP, &TimInfo );

	/* transfer PIXEL data to VRAM */
	timP += TimInfo.pw * TimInfo.ph/2+3+1;
	LoadImage((RECT *)&TimInfo.px, TimInfo.pixel);
		
	/* if CLUT is present, transfer to VRAM */
	if((TimInfo.pmode>>3)&0x01) {
		LoadImage( (RECT *)&TimInfo.cx, TimInfo.clut );
		timP += TimInfo.cw*TimInfo.ch/2+3;
	}
}

/*
 *  initialize BG */
initBG()
{
	int i;
	GsCELL *cellP;
        u_char *cel;
        u_char *bgd;
	int ncell;

	/* get cell information from CEL data */
        cel = (u_char *)CEL_ADDR;        
	cel += 4;
	ncell = *(u_short *)cel;	
	cel += 4;

	/* initialize BG frame maps and BG structures */
	for(i = 0; i < N_BG; i++) {

		/* get map data from BGD data */
	       	bgd = (u_char *)BgdAddr[MapIndex[i]];
		bgd += 4;

		BGMap[i].ncellw = *bgd++;		/* size of MAP */
		BGMap[i].ncellh = *bgd++;
		BGMap[i].cellw = *bgd++;		/* size of cell */
		BGMap[i].cellh = *bgd++;
		BGMap[i].index = (unsigned short *)bgd; /* main map unit */
		BGMap[i].base = (GsCELL *)cel;		/* pointer to cell array */
		
		/* reserve and initialize workspace (Primitive area) */
		BGWork[i] = BGPacket+i*BGWSIZE;
		GsInitFixBg32(&BGData[i], BGWork[i]);

		/* other (attributes, etc.) */
		BGData[i].attribute = (0<<24)|GsROTOFF;
		BGData[i].scrollx = BGData[i].scrolly = 0;
		BGData[i].r = BGData[i].g = BGData[i].b = 128;
		BGData[i].map = &(BGMap[i]);
	}
}

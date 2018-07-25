/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *   BG rendering sample program
 *
 *        Version 1.10   Jan, 11, 1995
 *        Version 1.20   Mar,  5, 1997  sachiko    (added autopad) 
 *
 *        Copyright  (C)  1994,1995 by Sony Computer Entertainment
 *             All rights Reserved
 **/

#include	<sys/types.h>
#include	<libetc.h>
#include	<libgte.h>
#include	<libgpu.h>
#include	<libgs.h>

/* ordering table (OT) definition */
#define	OT_LENGTH 4
GsOT WorldOT[2];
GsOT_TAG OTTags[2][1<<OT_LENGTH];

/* GPU packet area definition */
#define	PACKETMAX 6000*24
PACKET	GpuPacketArea[2][PACKETMAX];

/* texture data */
#define TEX_ADDR 0x80180000		/* texture start address */
#define	TIM_HEADER 0x00000010		/* TIM header value */
GsIMAGE TexInfo;			/* TIM data information */

/*
 *  BG data */
#define BGD_ADDR 0x80100000		/* address to store BGD data */
#define	BGD_HEADER 0x23			/* BGD header value */
#define CEL_ADDR 0x80140000		/* address to store CEL data */
#define	CEL_HEADER 0x22			/* CEL header value */
GsBG	BGData;				/* BG data */
GsMAP	BGMap;				/* map information */

/*
 *  Other ... */
u_long padd;			/* Controller pad data */

/*
 *  function prototype declarations */
void drawAll();
int  moveCharacter();
void initSystem();
void initTexture();
void initBG();

/*
 *  main routine */
main(void)
{
	ResetCallback();
	initSystem();

	padd = 0;

	initTexture();
	initBG();

	while(1) {
		if(moveCharacter()) break;
		drawAll();
	}

	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
}

/*
 *  render */
void drawAll()
{
	int activeBuff;		/* pointer to active buffer */
	int i;
	
	activeBuff = GsGetActiveBuff();
	GsSetWorkBase((PACKET *)GpuPacketArea[activeBuff]);
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* add BG to OT */
	GsSortBg(&BGData, &WorldOT[activeBuff], 15);

	/* if no rotation, high-speed rendering functions can be used */
	/* GsSortFastBg(&BGData, &WorldOT[activeBuff], 15); */

	padd = PadRead(0);

	VSync(0);

	ResetGraph(1);
	GsSwapDispBuff();
	GsSortClear(0, 0, 0, &WorldOT[activeBuff]);
	GsDrawOt(&WorldOT[activeBuff]);
}

/*
 *  move BG with the controller pad */
int moveCharacter()
{
	/* change BG size */
	if(((padd & PADRup)>0)&&(BGData.h < 480)) {
		BGData.scrolly -= 1;
		BGData.h += 2;
		BGData.my += 1;
	}
	if(((padd & PADRdown)>0)&&(BGData.h > 2)) {
		BGData.scrolly += 1;
		BGData.h -= 2;
		BGData.my -= 1;
	}
	if(((padd & PADRleft)>0)&&(BGData.w < 640)) {
		BGData.scrollx -= 1;
		BGData.w += 2;
		BGData.mx += 1;
	}
	if(((padd & PADRright)>0)&&(BGData.w > 2)) {
		BGData.scrollx += 1;
		BGData.w -= 2;
		BGData.mx -= 1;
	}

	/* change BG rotation angle */
	if((padd & PADl)>0) {
		BGData.rotate += ONE*4;
	}
	if((padd & PADn)>0) {
		BGData.rotate -= ONE*4;
	}

	/* change BG scale */
	if(((padd & PADm)>0)&&(BGData.scalex<28000)) {
		BGData.scalex += BGData.scalex/8;
		BGData.scaley += BGData.scaley/8;
	}
	if(((padd & PADo)>0)&&(BGData.scalex>512)) {
		BGData.scalex -= BGData.scalex/8;
		BGData.scaley -= BGData.scaley/8;
	}

	/* scroll */
	if((padd & PADLup)>0) {
		BGData.scrolly -= 2;
	}
	if((padd & PADLdown)>0) {
		BGData.scrolly += 2;
	}
	if((padd & PADLleft)>0) {
		BGData.scrollx -= 2;
	}
	if((padd & PADLright)>0) {
		BGData.scrollx += 2;
	}

	/* exit with K button */
	if((padd & PADk)>0) return -1;

	return 0;
}

/*
 *  initialize system */
void initSystem()
{
	int i;


	/* initialize pad */
	PadInit(0);

	/* initialize pad */
	GsInitGraph(320, 240, 0, 0, 0);
	GsDefDispBuff(0, 0, 0, 240);

	/* initialize OT */
	for(i = 0; i < 2; i++) {
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}

	/* make screen coordinate system same as 3D */	
	GsInit3D();
}

/*
 *  read sprite pattern
 *  (transfer multiple TIM data sets to VRAM) 
 *  */
void initTexture()
{
	u_long *timP;

	timP = (u_long *)TEX_ADDR;
	while(1) {
		/* see if there is TIM data */
		if(*timP != TIM_HEADER)	{
			break;
		}

		/* read through header */
		timP++;

		/* get TIM data position information */
		GsGetTimInfo( timP, &TexInfo );

		/* transfer PIXEL data to VRAM */
		timP += TexInfo.pw * TexInfo.ph/2+3+1;	/* move pointer forward */
		LoadImage((RECT *)&TexInfo.px, TexInfo.pixel);
		
		/* if CLUT is present, transfer to VRAM */
		if((TexInfo.pmode>>3)&0x01) {
			LoadImage( (RECT *)&TexInfo.cx, TexInfo.clut );
			timP += TexInfo.cw*TexInfo.ch/2+3;	/* move pointer forward */
		}
	}
}

/*
 *  initialize BG */
void initBG()
{
        u_char *cel;
        u_char *bgd;
	u_char celflag;
	int ncell;
	u_char bgdflag;

        cel = (u_char *)CEL_ADDR;        
	cel += 3;
	celflag = *cel++;
	ncell = *(u_short *)cel;	
	cel += 4;

       	bgd = (u_char *)BGD_ADDR;
	bgd += 3;
	bgdflag = *bgd++;

	BGMap.ncellw = *bgd++;
	BGMap.ncellh = *bgd++;
	BGMap.cellw = *bgd++;
	BGMap.cellh = *bgd++;
	BGMap.base = (GsCELL *)cel;
	BGMap.index = (u_short *)bgd;

	BGData.attribute = ((TexInfo.pmode&0x03)<<24);
	BGData.x = 0;
	BGData.y = 0;
	BGData.scrollx = BGData.scrolly = 0;
	BGData.r = BGData.g = BGData.b = 128;
	BGData.map = &BGMap;
	BGData.mx = 320/2;
	BGData.my = 224/2;
	BGData.scalex = BGData.scaley = ONE;
	BGData.rotate = 0;
	BGData.w = 320;
	BGData.h = 224;

	cel += ncell*8;
	if(celflag&0xc0 == 0x80) {
		cel += ncell;		/* Skip ATTR (8bit) */
	}
	if(celflag&0xc0 == 0xc0) {
		cel += ncell*2;		/* Skip ATTR (16bit) */
	}

	bgd += BGMap.ncellw*BGMap.ncellh*2;
	if(bgdflag&0xc0 == 0x80) {
		bgd += BGMap.ncellw*BGMap.ncellh;
	}
	if(bgdflag&0xc0 == 0xc0) {
		bgd += BGMap.ncellw*BGMap.ncellh*2;
	}
}

/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	MemoryCard GUI Module Sample  - TUTO0 -
 *
 *
 *	Copyright (C) 1998 by Sony Computer Entertainment
 *			All rights Reserved
 *
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <libapi.h>
#include <strings.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libspu.h>
#include <libcd.h>
#include <libsnd.h>
#include <libmcrd.h>
#include <mcgui.h>
#include "balls.h"
#include "hand.h"

#define	PIH		320
#define	PIV		240
#define	OTLEN		16

/* variables for drawing */
static int	side;
static long	ot[2][OTLEN];
static DISPENV	disp[2];
static DRAWENV	draw[2];

#define TEX_ADDR	(0x80100000)
#define SEQ_ADDR	(0x80104220)
#define VH_ADDR		(0x80108000)
#define VB_ADDR		(0x80109620)
#define SE_VH_ADDR	(0x80118000)
#define SE_VB_ADDR	(0x80118c20)
#define TEX_BG_ADDR	(0x80120000)

#define FILENAME	"BISLPS-00000"
#define BUFLEN		(8192*15)

static char SaveData[BUFLEN];
static unsigned char padbuf[2][36];
static McGuiEnv env;

#define BG	0
/****************************************************************************

*****************************************************************************/
unsigned long read_controller(void)
{
	unsigned long pad = 0;
	static unsigned long padrec = 0;

	if (padbuf[0][0]==0
		&&(padbuf[0][1]==0x41||padbuf[0][1]==0x53||padbuf[0][1]==0x73))
	{
		pad = (~(*(padbuf[0]+3) | *(padbuf[0]+2) << 8))&0xffff;
		pad = pad &(~padrec);
		padrec = pad;
	}

	return(pad);
}

int module(int func)
{
	/* Memory Card handling environment settting */

	/* texture */
	env.texture.addr = (unsigned long*)TEX_ADDR;

	/* BG */
	#if BG
			/* still picture BG */
	env.bg.mode = 1;
	env.bg.timadr = (unsigned long*)TEX_BG_ADDR;
	#else
			/* scrolling BG */
	env.bg.mode = 0;
	env.bg.scrollDirect = 7;	/* upper left */
	env.bg.scrollSpeed = 1;		/* 1/60 second */
	#endif

	/* Cursor */
	env.cursor.type = 0;		/* unused */
	env.cursor.r = 0;
	env.cursor.g = 64;
	env.cursor.b = 128;

	/* Controller */
	env.controller.buf[0] = padbuf[0];
	env.controller.buf[1] = padbuf[1];

	env.controller.type1.flag = 1;
	env.controller.type1.BUTTON_OK = PADRright;
	env.controller.type1.BUTTON_CANCEL = PADRdown;

	env.controller.type2.flag = 1;
	env.controller.type2.BUTTON_OK = MOUSE_LBUTTON;
	env.controller.type2.BUTTON_CANCEL = MOUSE_RBUTTON;

	env.controller.type3.flag = 1;
	env.controller.type3.BUTTON_OK = PADRright;
	env.controller.type3.BUTTON_CANCEL = PADRdown;

	env.controller.type4.flag = 1;
	env.controller.type4.BUTTON_OK = NEGICON_A;
	env.controller.type4.BUTTON_CANCEL = NEGICON_B;

	/* BGM ,sound effects */
	env.sound.MVOL = 127;
	env.sound.bgm.isbgm = 1;
	env.sound.bgm.seq = (unsigned long*)SEQ_ADDR;
	env.sound.bgm.vh = (unsigned long*)VH_ADDR;
	env.sound.bgm.vb = (unsigned long*)VB_ADDR;
	env.sound.bgm.SVOL = 127;
	env.sound.bgm.isReverb = 1;
	env.sound.bgm.reverbType = SS_REV_TYPE_STUDIO_C;
	env.sound.bgm.reverbDepth = 32;

	env.sound.se.isse = 1;
	env.sound.se.vh = (unsigned long*)SE_VH_ADDR;
	env.sound.se.vb = (unsigned long*)SE_VB_ADDR;
	env.sound.se.vol = 64;
	env.sound.se.prog = 0;
	env.sound.se.TONE_OK = 3;
	env.sound.se.TONE_CANCEL = 1;
	env.sound.se.TONE_CURSOR = 2;
	env.sound.se.TONE_ERROR = 0;

	/* Memory Card screen call */
	switch(func)
	{
		/* save */
		case 0:
		strcpy( env.cards.file, FILENAME );
		strncpy( env.cards.title, "じゃんけん　ぽん　グーチョキパー", 64 );
		env.cards.frame = 3;
		env.cards.iconAddr = TIM_HAND;
		env.cards.dataAddr = (unsigned long*)SaveData;
		env.cards.dataBytes = 8192-512;
		env.cards.block = 1;

		if (McGuiSave(&env)==-1)
		{
			printf( "McGuiEnv parameter invalid\n" );
		}
		break;

		/* load */
		case 1:
		strcpy( env.cards.file, FILENAME );
		env.cards.dataAddr = (unsigned long*)SaveData;
		env.cards.dataBytes = 8192-512;

		if (McGuiLoad(&env)==-1)
		{
			printf( "McGuiEnv parameter invalid\n" );
		}
		break;
	}

	return 1;
}


/* Main */
int main( void )
{
	RECT rect;
	char mes[2][8];
	int mcnt = 2;
	int cursor = 0;
	unsigned long pad = 0;

	strcpy(mes[0], ">SAVE");
	strcpy(mes[1], ">LOAD");

	ResetCallback();
	SetDispMask(0);
	ResetGraph(0);
	SetGraphDebug(0);

	/* initialize environment for double buffer */
	SetDefDrawEnv(&draw[0], 0,   0, PIH, PIV);
	SetDefDrawEnv(&draw[1], 0, PIV, PIH, PIV);
	SetDefDispEnv(&disp[0], 0, PIV, PIH, PIV);
	SetDefDispEnv(&disp[1], 0,   0, PIH, PIV);
	draw[0].isbg = draw[1].isbg = 1;
	setRGB0( &draw[0], 0, 0, 64 );
	setRGB0( &draw[1], 0, 0, 64 );
	PutDispEnv(&disp[0]);
	PutDrawEnv(&draw[0]);

	/* font system */
	setRECT( &rect, 64, 32, 240, 64 );
	FntLoad(960, 256);
	FntOpen( rect.x, rect.y, rect.w, rect.h, 0, 512);

	/* initialize controller */
	InitPAD( padbuf[0], 34, padbuf[1], 34 );
	StartPAD();

	/* initialize mcrd */
	MemCardInit(1);
	MemCardStart();

	/* initialize balls module */
	_make_balls_data();

	while(1) {
		pad = read_controller();

		if (pad==PADselect) break;

		if (pad==PADRright)
		{
			/* Execute McGUI */
			SetDispMask(0);
			module(cursor);
			mcnt = 2;
		}

		if (pad&PADLup && cursor==1) cursor--;
		if (pad&PADLdown && cursor==0) cursor++;

		mes[cursor][0] = '>';
		mes[cursor^1][0] = ' ';

		ClearOTag( ot[side], OTLEN );

		/* balls drawing */
		_draw_balls_data( side, ot[side], 128 );

		VSync(0);

		if (mcnt)
		{
			if( --mcnt==0) SetDispMask(1);
		}

		side ^= 1;
		PutDispEnv(&disp[side]);
		PutDrawEnv(&draw[side]);
		DrawOTag( ot[side^1] );

		/* message display */
		FntPrint("MEMORYCARD GUI MODULE\n\n");
		FntPrint("%s\n", mes[0]);
		FntPrint("%s\n", mes[1]);
		FntFlush(-1);
	}
	DrawSync(0);
	MemCardStop();
	MemCardEnd();
	return 0;
}


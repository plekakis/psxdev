/*
 Note: this is hard coded to use memcard #2 right now (just change it)
 *
 *                      balls: sample program
 *
 *              Copyright (C) 1993 by Sony Corporation
 *                      All rights Reserved
 *
 *       Version        Date            Design
 *      -----------------------------------------       
 *      1.00            Aug,31,1993     suzu
 *      2.00            Nov,17,1993     suzu    (using 'libgpu)
 *      3.00            Dec.27.1993     suzu    (rewrite)
 *      3.01            Dec.27.1993     suzu    (for newpad)
 *      3.02            Aug.31.1994     noda    (for KANJI)
 */
#include <r3000.h>
#include <asm.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <kernel.h>
/*
#include "file.h"
*/


/*
 * Kanji Printf
 */
#if 0
#define KANJI
#endif

/*
 * Primitive Buffer
 */
#define OTSIZE          16                      /* size of ordering table */
#define MAXOBJ          4000                    /* max sprite number */
typedef struct {                
	DRAWENV         draw;                   /* drawing environment */
	DISPENV         disp;                   /* display environment */
	u_long          ot[OTSIZE];             /* ordering table */
	SPRT_16         sprt[MAXOBJ];           /* 16x16 fixed-size sprite */
} DB;

typedef struct {                
	char    result;
	char    id;
	unsigned char   data[6];
}MDATA;

/*
 * Position Buffer
 */
typedef struct {                
	u_short x, y;                   /* current point */
	u_short dx, dy;                 /* verocity */
} POS;

/*
 * Limitations
 */
#define FRAME_X         320             /* frame size (320x240) */
#define FRAME_Y         240
#define WALL_X          (FRAME_X-16)    /* reflection point */
#define WALL_Y          (FRAME_Y-16)

unsigned long ev0,ev1,ev2,ev3;
unsigned long ev10,ev11,ev12,ev13;

#define BUF_MAX 34
#define PADMAX        2
/*
#define PADBUFFLEN    6
*/
#define PADBUFFLEN    34


typedef struct  {

	char            buff[PADMAX][PADBUFFLEN];
	char            Tbuf[PADMAX][PADBUFFLEN];

} PAD;

#define PADID_CON     0x41
#define PADID_MOUSE   0x12
#define PADID_MULTI   0x80

unsigned char Buf[2][BUF_MAX];
static  PAD   p;
static  MDATA *mdata[2][4];
static  card[4];
static  u_long  old_padd;

static
unsigned char old_mdata[2][4];

u_long _PadRead(PAD *);

main()
{
	POS     pos[MAXOBJ];
	DB      db[2];          /* double buffer */
	
	char    s[128];         /* strings to print */
	DB      *cdb;           /* current double buffer */
	int     nobj = 1;       /* object number */
	u_long  *ot;            /* current OT */
	SPRT_16 *sp;            /* work */
	POS     *pp;            /* work */
	int     i, cnt, x, y;   /* work */
	
	old_padd=0;
	old_mdata[0][0]=old_mdata[0][1]=old_mdata[0][2]=old_mdata[0][3]=0xff;
	old_mdata[1][0]=old_mdata[1][1]=old_mdata[1][2]=old_mdata[1][3]=0xff;

	ResetCallback();
	InitPAD(&p.buff[0][0], PADBUFFLEN, &p.buff[1][0], PADBUFFLEN);

	mdata[0][0] = (MDATA *)&p.buff[0][2];
	mdata[0][1] = (MDATA *)&p.buff[0][10];
	mdata[0][2] = (MDATA *)&p.buff[0][18];
	mdata[0][3] = (MDATA *)&p.buff[0][26];
	mdata[1][0] = (MDATA *)&p.buff[1][2];
	mdata[1][1] = (MDATA *)&p.buff[1][10];
	mdata[1][2] = (MDATA *)&p.buff[1][18];
	mdata[1][3] = (MDATA *)&p.buff[1][26];

	InitCARD(0);
	StartCARD();
	_bu_init();

	StartPAD();
	ChangeClearPAD(0);

	ResetGraph(0);          /* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);       /* set debug mode (0:off, 1:monitor, 2:dump) */

	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);
	
	/* init font environment */
#ifdef KANJI    /* KANJI */     
	KanjiFntOpen(160, 16, 256, 200, 704, 0, 768, 256, 0, 512);
#endif  
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	/* display */
	SetDispMask(1);         /* enable to display (0:inhibit, 1:enable) */

	cman2();

	exit();
}

/*
u_long PadRead(long n)
{
	return _PadRead(&p);
}
*/


u_long _PadRead(PAD *p)
{
	u_short         data[2];
	u_short         dat[2];
	u_char          id;
	short           i,j,k;


	for(i=0; i<PADMAX; i++)
	{

		if(p->buff[i][0] != 0)
		{
			id = 0;
		}else
		{
			id = p->buff[i][1];
		}
#if 0
		FntPrint("ID=%02x\n",id);
	       
		for(j=0; j<4; j++)
		{
		  for(k=0; k<8; k++)
		  {
		     FntPrint("%02x ",(u_char)p->buff[i][j*8+k+2]);
		  }
		  FntPrint("\n");
		}
#endif

		switch(id)
		{
			case PADID_MULTI :
				data[i] = 0;
				dat[0] = p->buff[i][4] & p->buff[i][12]
					& p->buff[i][20] & p->buff[i][28];
				dat[1] = p->buff[i][5] & p->buff[i][13]
					& p->buff[i][21] & p->buff[i][29];
				data[i] = (u_short)((((dat[0]&0xff)<<8))|(dat[1])&0xff);
				data[i] = ~data[i]&0xffff;
#if 0
				for(j=0; j<4; j++)
				{
					for(k=0; k<8; k++)
					{
						FntPrint("%02x ",(u_char)p->buff[i][j*8+k+2]);
					}
						FntPrint("\n");
				}
#endif
				break;
			case PADID_CON :
			case PADID_MOUSE :

				data[i] = (u_short)((((p->buff[i][2]&0xff)<<8))|(p->buff[i][3])&0xff);
				data[i] = ~data[i]&0xffff;
				break;
			default :
				data[i]    = 0;
				id      = 0;
		}

	}
	return (data[0]&0xffff)|(((data[1])<<16)&0xffff0000);
}

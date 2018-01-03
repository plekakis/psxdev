/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *          OVERLAY MENU: Sample Program
 *
 *      Copyright (C) 1995 by Sony Computer Entertainment Inc.
 *          All rights Reserved
 *
 *   Version    Date
 *  ------------------------------------------
 *  1.00        Mar,31,1995 yoshi
 */
/*===============================================================
 * This program read child process(.BIN file),and call it as function.
 * BGM sounds all through.  */
#include <sys/types.h>
#include <sys/file.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

#define TITLE_X 120
#define TITLE_Y 32
#define MENU_X 260
#define MENU_Y 64
#define FSIZE 16
#define MARG 40

#define PAD_A_SELECT 0x00000100
#define PAD_A_START 0x00000800
#define PAD_A_UP 0x00001000
#define PAD_A_DOWN 0x00004000

/*************************************************** prototype */

static int InitFont();
static int Puts();
static int file_read();
static int _read1();
extern int child_balls();
extern int child_rcube();
extern int child_anim();
#ifdef SOUND
static int sound();
extern int snd_set();
extern int snd_play();
#endif

/*************************************************** extern global vars. */

/* load-address for child process. it's defined in ADDRESS.S */
extern void *LoadAddress;

/**************************************************** overlay files */

typedef struct demo {
  char *s;			/* demo title */
  char *fn;			/* bin-file name */
  int (*func)();		/* function name */
} DMENU;

#define MENU_MAX 3

static DMENU dm[MENU_MAX] = {
  { "BALLS3","\\OVERMENU\\BALLS.BIN;1",child_balls },
  { "RCUBE3","\\OVERMENU\\RCUBE.BIN;1",child_rcube },
  { "ANIM3","\\OVERMENU\\ANIM.BIN;1",child_anim }
/* { title name, bin-file name, function name } */
};

/********************************************* sound */

#ifdef SOUND
typedef struct{
	char	*fname;
	void	*addr;
} FILE_INFO;

#define SFILENO 3

/* After transmit to SPU, VB-data area is destroyed by child process. */
#define VB_ADDR 0x80070000 

#define SEQ_SIZE (12738/2048+1)*2048		/* stay in parent process */
#define VH_SIZE (3104/2048+1)*2048		/* stay in parent process */

static char sound_seq[SEQ_SIZE];
static char sound_vh[VH_SIZE];

static FILE_INFO sfile[SFILENO] = {
  { "\\DATA\\MUTUAL2.SEQ;1",(void *)sound_seq },
  { "\\DATA\\MUTUAL.VH;1",(void *)sound_vh },
  { "\\DATA\\MUTUAL.VB;1",(void *)VB_ADDR }
/* { sound file name, load address } */
};
#endif

/***************************************** graphics */

typedef struct {
  DRAWENV draw;
  DISPENV disp;
  long x,y;
} DB;

/****************************************** main */

main()
{
  long pad,padd,padr;
  long i,j;
  char title[40];
  DB db[2];
  DB *cdb;
  unsigned char co1,co2,co3;
  POLY_G4 g4;
  int x,y,fd;
  RECT rct;

  ResetCallback();
  CdInit();
  ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
  SetGraphDebug(0);		/* set debug mode */
  PadInit(0);
#ifdef SOUND
  sound();			/* read files, and sound BGM */
#endif

  while(1){
    /* change display smoothly. */
    VSync(0);
    SetDispMask(0);
    ResetGraph(1);
    setRECT(&rct,0,0,1024,512);
    ClearImage(&rct,0,0,0);
    DrawSync(0);
 
    SetDefDrawEnv(&(db[0].draw), 0,   0, 640, 240);
    SetDefDrawEnv(&(db[1].draw), 0, 240, 640, 240);
    SetDefDispEnv(&(db[0].disp), 0, 240, 640, 240);
    SetDefDispEnv(&(db[1].disp), 0,   0, 640, 240);
    db[0].x = 0;
    db[0].y = 0;
    db[1].x = 0;
    db[1].y = 240;
    SetDispMask(1);
    
    padd = PadRead(1);
    InitFont(640, 0, 0, 481);
    strcpy(title,"PlayStation OVERLAY MENU");
    cdb = &db[0];
    co1 = 255;
    co2 = 0;
    co3 = 0;
    i = 0;
	
    while(1) {
      cdb = ( cdb == &db[1] ) ? &db[0] : &db[1];  
      PutDrawEnv(&cdb->draw);
      PutDispEnv(&cdb->disp);
      
      SetPolyG4(&g4);
      setXYWH(&g4, 0, 0, 640, 240);
      setRGB0(&g4, co1, co2, co3);
      setRGB1(&g4, co3, co1, co2);
      setRGB2(&g4, co2, co3, co1);
      setRGB3(&g4, co1, co2, co3);
      DrawPrim(&g4);

      if ( co3 == 0 && co1 > 0 ){
	co1--;
	co2++;
      }
      if ( co1 == 0 && co2 > 0 ){
	co2--;
	co3++;
      }
      if ( co2 == 0 && co3 > 0 ){
	co3--;
	co1++;
      }

      Puts(TITLE_X,TITLE_Y,title);
      for( j=0;j<MENU_MAX;j++ ){
	y = j*FSIZE + MENU_Y;
	Puts(MENU_X,y,dm[j].s);
      }

      if( i < 0 ) i = MENU_MAX-1;
      if( i >= MENU_MAX ) i = 0;
      y = i*FSIZE + MENU_Y;
      Puts(MENU_X-MARG,y,">>");
      
      padr = PadRead(1);
      pad = padr & ~padd;
      padd = padr;

      if (pad & PAD_A_START){
	file_read(dm[i].fn,LoadAddress);/* read child */
	dm[i].func();		/* run child */
	break;
      }
      if (pad & PAD_A_UP)
	i--;
      if ( (pad & PAD_A_DOWN) || (pad & PAD_A_SELECT) )
	i++;
    error:
      VSync(0);
    }
  }
  
}


static DR_MODE  mode;
static SPRT_16  sprt;

static int
InitFont(tx, ty, cx, cy)
int	tx, ty, cx, cy;
{
  extern u_long menu_font[];
	
  u_short	tpage, clut;
	
  tpage = LoadTPage(menu_font+0x80, 0, 1, tx, ty, 256,256);
  clut  = LoadClut(menu_font, cx, cy);
	
  SetDrawMode(&mode, 0, 0, tpage, 0);
  SetSprt16(&sprt);
  SetSemiTrans(&sprt, 1);
  SetShadeTex(&sprt, 1);
  sprt.clut = clut;
}

static int	
Puts(x, y, s)
int	x, y;
char	*s;
{
  int	code, u, v;
	
  DrawPrim(&mode);
  while (*s) {
    code = *s - '0' + 48;
    v = (code%16)*16;
    u = (code/16)*16;
    setUV0(&sprt, u, v);
    setXY0(&sprt, x, y);
    DrawPrim(&sprt);
		
    s++;
    x += 16;
  }
}



static int
file_read(fname,address)

char *fname;
void *address;
{
  CdlFILE	fp;		/* file's location & size */
  int	mode = CdlModeSpeed;	
  int cnt,i;

  for (i = 0; i < 10; i++) {	/* 10 times retry */
    if (CdSearchFile(&fp, fname) == 0) 
      continue;

    /* set target position */
    CdControl(CdlSetloc, (u_char *)&(fp.pos), 0);
    cnt = _read1(fp.size,address,mode); /* read */

    /* if it success, break. */
    if (cnt == 0){
      return(0);		/* success */
	}
  }
  return(-1);			/* fail */
}


static int
_read1(byte,sectbuf,mode)

long byte;
void *sectbuf;

int mode;
{	
  int nsector,cnt;
  unsigned char com;
	
  nsector = (byte+2047) / 2048;

  com = mode;
  CdControlB( CdlSetmode, &com, 0 );
  VSync(3);
	
  /* read start */
  CdRead(nsector, sectbuf, mode);
	
  while ((cnt = CdReadSync(1, 0)) > 0 ) {
    VSync(0);
  }

  return(cnt);
}


#ifdef SOUND
static int
sound()
{
  int i,fd;

  for(i=0;i<SFILENO;i++){
    file_read(sfile[i].fname,sfile[i].addr);
  }
  printf("snd_set.\n");
  snd_set(sfile[0].addr,sfile[1].addr,sfile[2].addr);
  printf("snd_play.\n");
  snd_play(sfile[0].addr,sfile[1].addr,sfile[2].addr);

  return(0);
}
#endif

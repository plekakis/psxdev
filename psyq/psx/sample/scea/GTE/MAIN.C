/*
** Main.c
   cube
   gte optimization demo
   mike acton
   macton@sonyinteractive.com
*/

#include <sys/types.h>
#include <kernel.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include <libsn.h>
#include "main.h"

uchar major_version = 0;
uchar minor_version = 2;


DRAWENV   draw[2];    
DISPENV   disp[2];
uchar     pageid;
ulong     sys_ot[2];

void draw_cube(void);
void init_cube(void);

void
main(void)
{
  ResetGraph(0);
  SetGraphDebug(0);

  InitGeom();
  SetGeomOffset(SCREEN_MAX_X/2,SCREEN_MAX_Y/2);

  SetDispMask(0);

  SetDefDrawEnv(&draw[0], 0,   0, SCREEN_MAX_X, SCREEN_MAX_Y);
  SetDefDrawEnv(&draw[1], 0, SCREEN_MAX_Y, SCREEN_MAX_X, SCREEN_MAX_Y);

  SetDefDispEnv(&disp[0], 0, SCREEN_MAX_Y, SCREEN_MAX_X, SCREEN_MAX_Y);
  SetDefDispEnv(&disp[1], 0,   0, SCREEN_MAX_X, SCREEN_MAX_Y);

  draw[0].isbg = 1;
  draw[1].isbg = 1;
  setRGB0(&draw[0],0,0,0);
  setRGB0(&draw[1],0,0,0);

  sys_ot[0] = 0x00ffffff;
  sys_ot[1] = 0x00ffffff;

  pageid = 0;

  FntLoad(1024-64,0);
  SetDumpFnt(FntOpen(32, 16, 480, 200, 0, 1024));
  SetDispMask(1);

  init_cube();

  while(1) 
  {
    static int cycle = 0;

    FntPrint("cube gte optimization demo\n");
    FntPrint("cycle = %d\n",cycle++);

    draw_cube();

    vswap();
  }
}

void
vswap(void)
{
  DrawSync(0);
  VSync(0);
  FntFlush(0);

  pageid = pageid^0x01;
  PutDrawEnv(&draw[pageid]);
  PutDispEnv(&disp[pageid]);

  DrawOTag(&sys_ot[pageid^0x01]);

  sys_ot[pageid] = 0x00ffffff;
}


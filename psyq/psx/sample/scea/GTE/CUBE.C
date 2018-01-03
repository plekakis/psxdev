/*
** Cube.c
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
#include <inline_c.h>
#include <gtemac.h>
#include "cpumac.h"
#include "main.h"



POLY_G4 cube_prims[2][6];

SVECTOR cube_verts[9]   = { {0   ,0   ,0   }
                           ,{0   ,100 ,0   }
                           ,{100 ,100 ,0   }
                           ,{100 ,0   ,0   }
                           ,{0   ,0   ,100 }
                           ,{0   ,100 ,100 }
                           ,{100 ,100 ,100 }
                           ,{100 ,0   ,100 }
                           ,{0   ,0   ,0   } };

CVECTOR cube_rgbs[9]    = { {127 ,0   ,0   }
                           ,{0   ,127 ,0   }
                           ,{127 ,127 ,0   }
                           ,{127 ,0   ,127 }
                           ,{0   ,127 ,127 }
                           ,{0   ,127 ,127 }
                           ,{127 ,127 ,127 }
                           ,{127 ,0   ,100 }
                           ,{0   ,0   ,0   } };

uchar   cube_vndx[8][4] = { {1,2,0,3}
                           ,{6,5,7,4}
                           ,{2,6,3,7}
                           ,{5,1,4,0}
                           ,{0,3,4,7}
                           ,{5,6,1,2} };


// DCache setup

#define  dc_camdirp ((sshort*)  getScratchAddr(0))
#define  dc_ip      ((uchar*)   getScratchAddr(1))
#define  dc_opzp    ((slong*)   getScratchAddr(2))
#define  dc_wmatp   ((MATRIX*)  getScratchAddr(3))
#define  dc_cmatp   ((MATRIX*)  getScratchAddr(9))
#define  dc_sxytbl  ((DVECTOR*) getScratchAddr(15))

MATRIX wm;
MATRIX cm;
slong  opz;
schar  i;

void
init_cube(void)
{
  SetGeomScreen(600);

  wm.m[0][0] = 0x1000; 
  wm.m[0][1] = 0x0000;
  wm.m[0][2] = 0x0000;

  wm.m[1][0] = 0x0000; 
  wm.m[1][1] = (0x0800*qcos(-52))>>12;  
  wm.m[1][2] = qsin(-52);

  wm.m[2][0] = 0x0000; 
  wm.m[2][1] = (0x0800*-qsin(-52))>>12; 
  wm.m[2][2] = qcos(-52);

  wm.t[0]    = 0x0000; 
  wm.t[1]    = 0x0000;                  
  wm.t[2]    = 0x0200;
}

/* draw cube
   1. This just uses a cube as an example, obviously the data can
      be anything, with the limitation that it is organized into groups
      small enough that the screen xys can fit into the dcache.
   2. I assume (for the sake of argument) that the polys come from some
      shared buffer and as such cannot be pre-initialized.
*/

void
draw_cube(void)
{
  register ulong   ur0     asm("$16");
  register ulong   ur1     asm("$17");
  register ulong   ur2     asm("$18");
  register ulong   ur3     asm("$19");
  register ulong   ur4     asm("$20");
  register ulong   ur5     asm("$21");

  /* setup camera */

  cm.m[0][0] = qcos((*dc_camdirp)); 
  cm.m[0][1] = 0x0000;  
  cm.m[0][2] = -qsin((*dc_camdirp));

  cm.m[1][0] = 0x0000;        
  cm.m[1][1] = 0x1000;  
  cm.m[1][2] = 0x0000;        

  cm.m[2][0] = qsin((*dc_camdirp)); 
  cm.m[2][1] = 0x0000;  
  cm.m[2][2] = qcos((*dc_camdirp));

  cm.t[0]    = 0x0000;        
  cm.t[1]    = -0x0018; 
  cm.t[2]    = 0x0000;

  gte_MulMatrix0(&wm,&cm,&cm);
  gte_SetRotMatrix(&cm);
  gte_SetTransMatrix(&wm);

  (*dc_camdirp) = ((*dc_camdirp)+12)&0x0fff;


  /* xform the vertex list

     1. pad the vertex count to a multiple of 3 (this will almost always
        average out to a better count then checking for the last one or two)
     2. hide the main ram lookup of the vertecies while the gte is busy
        with the transform.
     3. save screen xys in dcache, to be indexed by individual polys
  */

  // load the cpu registers with the main ram vertecies (take hit)
  i = 0;
  cpu_ldr(ur0,(ulong*)&cube_verts[0].vx);
  cpu_ldr(ur1,(ulong*)&cube_verts[0].vz);
  cpu_ldr(ur2,(ulong*)&cube_verts[1].vx);
  cpu_ldr(ur3,(ulong*)&cube_verts[1].vz);
  cpu_ldr(ur4,(ulong*)&cube_verts[2].vx);
  cpu_ldr(ur5,(ulong*)&cube_verts[2].vz);

  shared_xverts_loop:

  // load the gte registers from the cpu registers (gte-cpu move 1 cycle)
  cpu_gted0(ur0);
  cpu_gted1(ur1);
  cpu_gted2(ur2);
  cpu_gted3(ur3);
  cpu_gted4(ur4);
  cpu_gted5(ur5);

  if (i==9) goto shared_xverts_done;

  // hide part of add in the nop of rtpt
  i+=3; 
  gte_rtpt_b();

  // while rtpt, 
  // load the cpu registers with the main ram vertecies 
  cpu_ldr(ur0,(ulong*)&cube_verts[i].vx);
  cpu_ldr(ur1,(ulong*)&cube_verts[i].vz);
  cpu_ldr(ur2,(ulong*)&cube_verts[i+1].vx);
  cpu_ldr(ur3,(ulong*)&cube_verts[i+1].vz);
  cpu_ldr(ur4,(ulong*)&cube_verts[i+2].vx);
  cpu_ldr(ur5,(ulong*)&cube_verts[i+2].vz);

  // move from gte registers to dcache
  gte_stsxy3c(&dc_sxytbl[i-3]);

  goto shared_xverts_loop;

  shared_xverts_done:


  /* setup the polys (all quads)

     1. Hide one of the vertex index main ram lookups within the nclip
     2. Only hit other main ram (poly setup) when absolutely
        necessary.
  */

  
  i = 0;
  gte_ldsxy0((*(ulong*)&dc_sxytbl[cube_vndx[0][0]]));
  gte_ldsxy1((*(ulong*)&dc_sxytbl[cube_vndx[0][1]]));
  gte_ldsxy2((*(ulong*)&dc_sxytbl[cube_vndx[0][2]]));

  setup_polys_loop:
   
  if (i==6) goto setup_polys_done;

  gte_nclip();

  gte_ldsxy0((*(ulong*)&dc_sxytbl[cube_vndx[i+1][0]]));
  gte_ldsxy1((*(ulong*)&dc_sxytbl[cube_vndx[i+1][1]]));
  gte_ldsxy2((*(ulong*)&dc_sxytbl[cube_vndx[i+1][2]]));

  gte_stopz(&opz);
  if (opz >= 0) 
  {
    i++;
    goto setup_polys_loop;
  }

  #define prim_lxy0    *((ulong*)&cube_prims[pageid][i].x0)
  #define prim_lxy1    *((ulong*)&cube_prims[pageid][i].x1)
  #define prim_lxy2    *((ulong*)&cube_prims[pageid][i].x2)
  #define prim_lxy3    *((ulong*)&cube_prims[pageid][i].x3)
  #define prim_lrgb0   *((ulong*)&cube_prims[pageid][i].r0)
  #define prim_lrgb1   *((ulong*)&cube_prims[pageid][i].r1)
  #define prim_lrgb2   *((ulong*)&cube_prims[pageid][i].r2)
  #define prim_lrgb3   *((ulong*)&cube_prims[pageid][i].r3)

  #define stored_lxy0    *((ulong*)&dc_sxytbl[cube_vndx[i][0]].vx)
  #define stored_lxy1    *((ulong*)&dc_sxytbl[cube_vndx[i][1]].vx)
  #define stored_lxy2    *((ulong*)&dc_sxytbl[cube_vndx[i][2]].vx)
  #define stored_lxy3    *((ulong*)&dc_sxytbl[cube_vndx[i][3]].vx)
  #define stored_lrgb0   *((ulong*)&cube_rgbs[cube_vndx[i][0]])
  #define stored_lrgb1   *((ulong*)&cube_rgbs[cube_vndx[i][1]])
  #define stored_lrgb2   *((ulong*)&cube_rgbs[cube_vndx[i][2]])
  #define stored_lrgb3   *((ulong*)&cube_rgbs[cube_vndx[i][3]])

  prim_lxy0  = stored_lxy0;
  prim_lxy1  = stored_lxy1;
  prim_lxy2  = stored_lxy2;
  prim_lxy3  = stored_lxy3;
  prim_lrgb0 = stored_lrgb0;
  prim_lrgb1 = stored_lrgb1;
  prim_lrgb2 = stored_lrgb2;
  prim_lrgb3 = stored_lrgb3;

  #undef prim_lxy0
  #undef prim_lxy1
  #undef prim_lxy2
  #undef prim_lxy3
  #undef prim_lrgb0
  #undef prim_lrgb1
  #undef prim_lrgb2
  #undef prim_lrgb3

  #undef stored_lxy0
  #undef stored_lxy1
  #undef stored_lxy2
  #undef stored_lxy3
  #undef stored_lrgb0
  #undef stored_lrgb1
  #undef stored_lrgb2
  #undef stored_lrgb3 
 
  setPolyG4(&cube_prims[pageid][i]); // code trashed by long rgb0 set anyway
  addPrim(&sys_ot[pageid],&cube_prims[pageid][i]);
  // could save some time here by not resetting ot->addr every loop iter.

  i++;
  goto setup_polys_loop;

  setup_polys_done:
}


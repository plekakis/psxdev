/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _TURTLE_H_
#define _TURTLE_H_

/*****************************************************************
 *
 * file: turtle.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
#include "maxconf.h"

#ifdef OVAL
typedef struct {
	POLY_G3 oval[32];
	short   x, y;
	short   cx, cy;
	short   rx, ry;
	short   deltaR;
	CVECTOR color1;
	CVECTOR color2;
} OVAL_PRIM;

#define OVAL_ROT32	(128)
#define OVAL_ROT16	(256)

#endif /* OVAL */

typedef struct {
	LINE_F2 line_h[64];
	LINE_F2 line_v[64];
	short   x, y;
	short   w, h;
	short   width;
	short   delta;
	CVECTOR color;
} MESH_PRIM;

typedef struct {
	short	w, h;		/* bitmap size */
	short	rowLWord;	/* offset to next row (*sizeof(long)) */
	short	depth;		/* 1pixel/bits */
	long	mapSize;
	u_long	*baseAddr;	/* bitmap base address */
} PixMap;

#ifdef COW
typedef struct {
	short	x, y;
} Point;

typedef struct {
	Point	*stack;	/* for bucket proc */
	long	stack_size;
} TDGlobals;

extern TDGlobals	__gTD;

#define RGB2SHORT(r,g,b)	((((b)&0xf8)<<7)|(((g)&0xf8)<<2)|((r)>>3))
#endif /* COW */

#define SetRect(r,X,Y,W,H)	((r)->x=X,(r)->y=Y,(r)->w=W,(r)->h=H)
#define ABS(x)                  (((x)<0)?-(x):(x))

#define TEX_MAP_WIDTH	16
#define TEX_MAP_HEIGHT	512
#define TEX_MAP_SIZE	(TEX_MAP_WIDTH*TEX_MAP_HEIGHT)

/* function prototypes */
/* pixmap access procs */
void ClearPixMap( PixMap *bmp );
void InvertPixMap( PixMap *bmp );
void CopyPixMap( PixMap *src, PixMap *dst );
void AndPixMap( PixMap *src1, PixMap *src2, PixMap *dst );
u_long GetPixel( PixMap *bmp, short x, short y );
void SetPixel( PixMap *bmp, short x, short y, u_long color );
long InitPixMap( PixMap *bmp, short w, short h, u_long *addr, int depth );
long CalcPixMapRowSize( short w, short depth );
long CalcPixMapSize( short w, short h, short depth );

#ifdef COW
/* bucket procs */
void InitBucket( u_long *stack_addr, long stack_size );
void Bucket( PixMap *pm, short x, short y, u_long col );

/* median filter proc */
void MedianBitMap( PixMap *pixmap );

/* line proc */
void FractalLine( Point *p1, Point *p2, long w, long n, void (*func)(Point *p1, Point *p2, long user), long userData );

/* cow pattern generator */
void DrawSpectrumGrad( RECT *rct );
void MaskCowPattern( PixMap *tmpmap, PixMap *mskmap, long p1, long p2 );
void RandSpectrumColor( CVECTOR *rc );

/* for bit noise reduction */
void ExpansionPixMap( PixMap *src, PixMap *dst );
void DeflationPixMap( PixMap *src, PixMap *dst );

/* color table proc */
void GetSpectrumCTable( short *tbl );

#endif /* COW */

/* find blank space */
int FindRect( PixMap *bmp, RECT *limit, short w, short h, short *x, short *y, u_long col );

/* line proc */
void LineBitMap( PixMap *pix, short x1, short y1, short x2, short y2, u_long col);
void FillRectBitMap( PixMap *pix, RECT *rect, u_long col );

/* texture locator */
void InitTexMapManager( RECT *maskRect, RECT *clutRect );
void ResetTexMap( void );
int FindFreeArea( short w, short h, short pw, short ph, RECT *freeArea, RECT *freeClut );
int UseTexMap( RECT *useArea );
int UseClutMap( RECT *useArea );
int UnuseTexMap( RECT *useArea );
int UnuseClutMap( RECT *useArea );
void printTexMap( void );

/* square proc */
void make_square( u_long *dst, long w, long h, CVECTOR *col );
void make_button( u_long *dst, long w, long h, CVECTOR *col1, CVECTOR *col2, long fg, long bg );

#ifdef OVAL
/* makebg.c */
void make_cow_pattern( u_long* dst, long w, long h, long noise );
void make_sphere( u_long *dst, long delta, long w, long h, long x, long y, long r, long cx, long cy, CVECTOR *col, CVECTOR *ccol );

/* oval.c */
void add_oval_prim( OVAL_PRIM *oval, u_long *ot );
void draw_oval( OVAL_PRIM *oval );
void make_oval_prim( OVAL_PRIM *oval, long delta, long x, long y, long rx, long ry, long cx, long cy, CVECTOR *col, CVECTOR *ccol );
void recalc_oval( OVAL_PRIM *oval );
#endif /* OVAL */

#ifdef MESH
/* mesh create */
void add_mesh_prim( MESH_PRIM *mesh, u_long *ot );
void make_mesh_prim( MESH_PRIM *mesh, int delta, int width, int x, int y, int w, int h, CVECTOR *col );
#endif /* MESH */

#endif /* _TURTLE_H_ */
/* DON'T ADD STUFF AFTER THIS */

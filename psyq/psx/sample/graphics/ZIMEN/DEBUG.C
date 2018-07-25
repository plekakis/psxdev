/* $PSLibId: Run-time Library Release 4.4$ */
/*				debug
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 */
#include "sys.h"

/*
 * draw a rectangle in the world coordinate (z = 0)
 */
void disp_clipw(RECT32 *rect)
{
	SVECTOR	x[4];
	
	setVector(&x[0], rect->x,         rect->y,         0);
	setVector(&x[1], rect->x+rect->w, rect->y,         0);
	setVector(&x[2], rect->x+rect->w, rect->y+rect->h, 0);
	setVector(&x[3], rect->x,         rect->y+rect->h, 0);
	
	draw_line4w(128, &x[0], &x[1], &x[2], &x[3]);
}

/*
 * draw a rectangle in the screen coordinate
 */
void disp_clips(RECT *rect)
{
	long	ofx, ofy;
	SVECTOR	x0, x1, x2, x3;
	
	ReadGeomOffset(&ofx, &ofy);

	setVector(&x0, rect->x+ofx,         rect->y+ofy, 0);
	setVector(&x1, rect->x+ofx+rect->w, rect->y+ofy, 0);
	setVector(&x2, rect->x+ofx+rect->w, rect->y+ofy+rect->h, 0);
	setVector(&x3, rect->x+ofx, 	    rect->y+ofy+rect->h, 0);
	
	draw_line4s(255, &x0, &x1, &x2, &x3);
}
	

/*
 * draw quadrangle line in the world coordinate
 */
void draw_line4w(u_char col,
		     SVECTOR *x0, SVECTOR *x1, SVECTOR *x2, SVECTOR *x3)
{
	SVECTOR xy[4];
	long	dmy, flg;
	
	RotTransPers4(x0, x1, x2, x3,
		     (long *)&xy[0], (long *)&xy[1], 
		     (long *)&xy[2], (long *)&xy[3], 
		     &dmy, &flg);
	
	draw_line4s(col, &xy[0], &xy[1], &xy[2], &xy[3]);
}

/*
 * draw quadrangle line in the screen coordinate
 */
void draw_line4s(u_char col,
		     SVECTOR *x0, SVECTOR *x1, SVECTOR *x2, SVECTOR *x3)
{
	LINE_F2	line;
	SetLineF2(&line);
	setRGB0(&line, col, col, col);
	
	setXY2(&line, x0->vx, x0->vy, x1->vx, x1->vy);	DrawPrim(&line);
	setXY2(&line, x1->vx, x1->vy, x2->vx, x2->vy);	DrawPrim(&line);
	setXY2(&line, x2->vx, x2->vy, x3->vx, x3->vy);	DrawPrim(&line);
	setXY2(&line, x3->vx, x3->vy, x0->vx, x0->vy);	DrawPrim(&line);
}
	

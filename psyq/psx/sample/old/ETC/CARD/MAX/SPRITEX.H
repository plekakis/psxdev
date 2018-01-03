/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _SPRITEX_H_
#define _SPRITEX_H_

/*****************************************************************
 *
 * file: spritex.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

typedef struct {
	short	mode; 		/* 0=sprite,  1=poly */
	SPRT	sprt;
	POLY_FT4 poly;
	short	tpage;
	short	x, y;
	short	w, h;
	short	cx, cy;
	short	cw, ch;
	short	px, py;
	short	pw, ph;
	short	mx, my;
	DR_MODE	dr_mode;
	u_long	priority;
} SPRTX;

void	MakeSpriteData( SPRTX* sprite, u_long* timdata, int );
void	MakeSpriteData2( SPRTX* sprite, GsIMAGE* rectImage, _TPAGE* rectTpage, int mode );
void	SetSpritePosition( SPRTX* sprite, short x, short y );
void	SetSpriteRGB( SPRTX* sprite, short r, short g, short b );
void	SetSpriteTrans( SPRTX* sprite, int trans );
void	SetSpriteScale( SPRTX* sprite, int w, int h );
void	SetSpriteSize( SPRTX* sprite, int w, int h );
void	SetSpritePriority( SPRTX* sprite, u_long pri );
void	SetSpriteShadeTex( SPRTX* sprite, int sw );
void	AddSprite( u_long* ot, SPRTX* sprite );
void	AddSprite2( DB *db, SPRTX* sprite );

#endif /* _SPRITEX_H_ */
/* DON'T ADD STUFF AFTER THIS */

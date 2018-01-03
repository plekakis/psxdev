/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _MENUX_H_
#define _MENUX_H_

/*****************************************************************
 *
 * file: menux.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#define _PADLup     ((1<<12) |(1<<28))
#define _PADLdown   ((1<<14) |(1<<30))
#define _PADLleft   ((1<<15) |(1<<31))
#define _PADLright  ((1<<13) |(1<<29))
#define _PADRup     ((1<< 4) |(1<<20))
#define _PADRdown   ((1<< 6) |(1<<22))
#define _PADRleft   ((1<< 7) |(1<<23))
#define _PADRright  ((1<< 5) |(1<<21))
#define _PADi       ((1<< 9) |(1<<25))
#define _PADj       ((1<<10) |(1<<26))
#define _PADk       ((1<< 8) |(1<<24))
#define _PADl       ((1<< 3) |(1<<19))
#define _PADm       ((1<< 1) |(1<<17))
#define _PADn       ((1<< 2) |(1<<18))
#define _PADo       ((1<< 0) |(1<<16))
#define _PADh       ((1<<11) |(1<<27))

#define CDKey      (_PADk|_PADh|_PADl|_PADm|_PADn|_PADo)
#define OKKey      (_PADRup|_PADRdown|_PADRleft|_PADRright)

/*******************************************

*******************************************/

typedef	struct {
	int	group;
	int	id;
	u_long	tim;
	int	x;
	int	y;
	int	mw;            /* mouse sence width */
	int	mh;            /* mouse sence height */
	int	left;
	int	right;
	int	up;
	int	down;
	int	effect;
#ifdef NEWS
	void	(*drawFunc)( );
	void	(*execFunc)( );
#else
	void	(*drawFunc)( int event, POLY_FT4* poly );
	void	(*execFunc)( long pad );
#endif
	long	pad;
} _RMENU;

typedef	struct {
	_RMENU	RM;
	SPRTX	Sprites[2];
	SPRTX	Animation[2][6];
	GsIMAGE	RectIMage;
	int	intence;
	int	animecount;
	int	animeframe;
	int	animemax;
} _MENU;

typedef struct {
	int	sx;
	int	sy;
	int	x;
	int	y;
	int	dx;
	int	dy;
} _XXX;

typedef struct {
	int	u[2];
	int	v[2];
	int	sprtnum;
	int     bnum;
	u_long	*addr;
	char	name[64];
} _FFF;
/*******************************************

*******************************************/
void 	MENUinit( void );
void	MENUend( void );
int	MENUsetItem( _RMENU* rm );
int	MENUsetItem2( _RMENU* rm, GsIMAGE* rectImage, _TPAGE* rectTpage );
int	MENUsetItem3( _RMENU* rm, SPRTX* sprite );
void	MENUdrawItemAll( DB *cdb );
int	MENUsetCurrentItem( int id );
int	MENUgetCurrentItem( void );
POLY_FT4* MENUgetPolyPtr( int id );
SPRTX*	MENUgetSprtxPtr( int id );
void	MENUidol( long pad );
void	MENUsetItemIntence( int id, int intence );
void	MENUpushCurrentItem( void );
int	MENUpullCurrentItem( void );
void	MENUremoveItem( int id );
void	MENUclrQueBuffer( void );
int	MENUsetAnimationItem2( int ptr, SPRTX* sprite );
void	MENUsetItemEffect( int id, int effect );

extern _MENU *MENU;

#endif /* _MENUX_H_ */
/* DON'T ADD STUFF AFTER THIS */

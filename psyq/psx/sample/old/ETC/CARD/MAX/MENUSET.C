/* $PSLibId: Run-time Library Release 4.4$ */
/*****************************************************************
 *
 * file: menuset.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
#include "libps.h"

#include "maxtypes.h"
#include "maxstat.h"

typedef	struct {
	long	id;
	long	flag;
	long	cbnum;
	short	cx;
	short	cy;
	short	cw;
	short	ch;
	char	clut[32];
	long	pbnum;
	short	px;
	short	py;
	short	pw;
	short	ph;
	char	image[2];
} _TIM4;


extern MAXSTAT  maxstat;
extern SPRTX    cursor;

extern void funcMenu( long pad );
extern void funcExit( long pad );
extern void bcopy(char *src, char *dst, int size);

#include "menuset.h"

/* Screen Mode */
#define MEMORY_CARD  1

int	ScreenMode;
int	FirstPlayMode;

int  AttachSprite( _INIT *list, SPRTX *sprt, SPRTX *sprt2 );
void MoveCursor( DB *cdb, SPRTX *cursor );
static int  timxto4( SPRTX *sprt, u_long *timdata, int mode );
extern BackUpCard ( _RMENU *RM, _INIT *InitList );
extern SPRTX*  GetSprtxPtrFromMENUid( int id );
extern void InitCardIcons( int dev );

void MoveCursor( DB *cdb, SPRTX *cursor )
{
    SPRTX *current;

 	current = MENUgetSprtxPtr( MENUgetCurrentItem() );
	SetSpritePosition( cursor,
			  current->x+cursor->w/2, current->y+cursor->h*2/4 );
	AddSprite2( cdb, cursor );
}

/*
  tim2to4() and tim1to4() are unified*/
static int timxto4( SPRTX *sprt, u_long *timdata, int mode )
{
    int i, j, length;
    long size;
    _TIM4 *tim, *tim1;
    char *image, *image1;

    tim1 = (_TIM4*)timdata;

    if( tim1->cw == 8 && tim1->ch == 8 ) return 1;
    
    size = tim1->pbnum & 0xffff - 12;

    image1 = tim1->image;

    if((tim = (_TIM4*)malloc( sizeof(_TIM4) )) == NULL ) {
	printf("Can't Malloc Size [%d]\n", sizeof(_TIM4) );
    }

    if((image = (char *)malloc( size )) == NULL ) {
	printf("Can't Malloc Size [%d]\n", size );
    }

    if( mode ) {
	for( i=0 ; i<size/2 ; i++ ) {
	    image[i*2]    = (image1[i]&0xc0) >> 2 | (image1[i]&0x30) >> 4;
	    image[i*2+1]  = (image1[i]&0x0c) << 2 | (image1[i]&0x03);
	}
    }
    else {
	length = image1[0] & 0x0f;
	if( length==0 ) length = 16;

	for(i=0,j=0; j<size ;) {
	    image[j] = image1[i] & 0xf0;
	    if( --length <= 0) {
		length = image1[++i] & 0x0f;
		if( length==0 ) length = 16;
	    }
	    image[j++] |= (image1[i] & 0xf0) >> 4;
	    if( --length <= 0) {
		length = image1[++i] & 0x0f;
		if( length==0 ) length = 16;
	    }
	}
    }

    bcopy( (char *)tim1, (char *)tim, sizeof(_TIM4));   
    bcopy( image, tim->image, size );

    MakeSpriteData( sprt, (u_long *)tim, 1 );

    free(image);
    free(tim);

    return 0;
}

void    SetUpPageItem( int sel )
{
    int     i;
    SPRTX sprt;
    GsIMAGE rectImage;
    _TPAGE  rectTpage;

#ifdef DEBUG
    printf( "init page = %d, ScreenMode=%d\n", sel, ScreenMode );
#endif

#ifdef EMOUSE
    EMouseSetFree();
#endif /* EMOUSE */

    InitTexMapManager( &Page[sel].maskRect, &Page[sel].clutRect );

    MENUinit();

	/* Create Univers Tim */
	if( sel == MEMORY_CARD )
	  timxto4( &sprt, univ_mem, 1 );
	
	/* Init Menu */
	for( i = 0; Page[sel].menu[i].id; i++ ) {
	    rectImage.pmode = 0;
	    rectImage.px = Page[sel].imap[i].x;
	    rectImage.py = Page[sel].imap[i].y;
	    rectImage.pw = Page[sel].imap[i].w/4;
	    rectImage.ph = Page[sel].imap[i].h;
	    rectImage.cx = sprt.cx;
	    rectImage.cy = sprt.cy;
	    rectImage.cw = sprt.cw;
	    rectImage.ch = sprt.ch;

	    rectTpage.tpage = sprt.tpage;
	    rectTpage.u = Page[sel].imap[i].u*4;
	    rectTpage.v = Page[sel].imap[i].v;

	    MENUsetItem2( &Page[sel].menu[i], &rectImage, &rectTpage );
	    if ( Page[sel].menu[i].group == 100 ) {
		fot = !fot;
		SetSpritePriority( MENUgetSprtxPtr ( Page[sel].menu[i].id ), HIDE );
		fot = !fot;
		SetSpritePriority( MENUgetSprtxPtr ( Page[sel].menu[i].id ), HIDE );
	    }
	}

	/* Initialize for Memory Card */
	InitCardIcons ( 0 );
	InitCardIcons ( 1 );
#ifdef EMOUSE
	EMouseSetCardHome();
#endif /* EMOUSE */

    MENUsetCurrentItem( Page[sel].currentItem );

#ifdef EMOUSE
    {
	SPRTX *sprite;
	sprite = MENUgetSprtxPtr(MENUgetCurrentItem());
	EMouseMove( sprite->x-DELTA/4, sprite->y-DELTA/4);
    }
#endif /* EMOUSE */

    ScreenMode = sel;
    maxstat.bgsprtCount = AttachSprite( Page[sel].bg, maxstat.bgsprt[0],
				       maxstat.bgsprt[1] );
    maxstat.spriteCount = AttachSprite( Page[sel].sprt, maxstat.Sprites[0],
				       maxstat.Sprites[1] );

    AttachSprite( Page[sel].cursor, &maxstat.cursor[0], &maxstat.cursor[1] );
    SetSpriteTrans( &maxstat.cursor[0], 1 );
    SetSpriteTrans( &maxstat.cursor[1], 1 );

#ifdef EMOUSE
    EMouseInitDisp();
#endif /* EMOUSE */

}

int AttachSprite( _INIT *list, SPRTX *sprt, SPRTX *sprt2 )
{
    int     i;
    int	n, useFlag = 0;
    
    n = -1;
    
    for ( i = 0; list[i].tim; i++ ) {
	if ( list[i].copy == -1 ) {
	    MakeSpriteData( &sprt[i], (u_long *)list[i].tim, list[i].scale );
	} else {
	    memcpy( (char *)&sprt[i], (char *)&sprt[list[i].copy], sizeof( SPRTX ) );
	}
	SetSpritePosition( &sprt[i], list[i].x, list[i].y );
	SetSpriteTrans( &sprt[i], 0 );
	SetSpriteShadeTex( &sprt[i], 1 );
	
	/* Set Half Trans Sprite */
	if (list[i].scale == 2) {
	    SetSpriteTrans (&sprt[i], 1);
	}
	
	if ( list[i].scale || list[i].newW ) {
	    SetSpriteSize( &sprt[i], list[i].newW, list[i].newH );
	}
	sprt[i].priority = list[i].prio;
	
	if ( !useFlag && list[i].useMenu > -1 ) {
	    n = i;
	    useFlag = 1;
	}
	if ( useFlag ) {
	    static _RMENU rm = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, NULL, NULL, 0 };
	    
	    rm.id = list[i].useMenu;
	    MENUsetItem3( &rm, &sprt[i] );
	}

	bcopy( (char *)&sprt[i], (char *)&sprt2[i], sizeof( SPRTX ) );
    }
    
    maxstat.menusprtStart = n;
    maxstat.menusprtCount = i - n;
    
    return (n > -1)? n : i;
}

/*******************************
 *   Callbacks for MAIN_MENU   *
 *******************************/
static RECT imgRect, cltRect;

void    funcCardS( long pad )
{
    static InitFlag=0;

    if ( pad & OKKey ) {
	Page[MEMORY_CARD].currentItem = 3;
	maxstat.pageNumber = MEMORY_CARD;

	if( !InitFlag ) {
	    InitCARD(1);
	    StartCARD( );
	    ChangeClearPAD(0);
	    _bu_init( );
	    _card_auto(1);
	    InitFlag = 1;
	}
    }

}

/*******************************
 *  Callbacks for MEMORY_CARD  *
 *******************************/

void	funcMemExit( long pad )
{
    if ( pad & OKKey ) {
#ifdef EMOUSE
	EMouseVclear();
#endif /* EMOUSE */
	Page[MEMORY_CARD].currentItem = 3;
	maxstat.pageNumber = MEMORY_CARD;
    }
}

static void	funcMemConf( long pad )
{
    if ( pad & OKKey ) {
#ifdef EMOUSE
	EMouseSetCardHome() ;
#endif /* EMOUSE */

	SetSpritePriority( MENUgetSprtxPtr ( _CONF_ID_YES ),      HIDE );
	SetSpritePriority( MENUgetSprtxPtr ( _CONF_ID_NO ),       HIDE );
	SetSpritePriority( MENUgetSprtxPtr ( _CONF_ID_COPY ),     HIDE );
	SetSpritePriority( MENUgetSprtxPtr ( _CONF_ID_DELETE ),   HIDE );
	SetSpritePriority( MENUgetSprtxPtr (_CONF_SPRT_BALL_YES), HIDE );
	SetSpritePriority( MENUgetSprtxPtr (_CONF_SPRT_BALL_NO),  HIDE );

	SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_MESSAGE),    HIDE );
	SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_FILE),       HIDE );
	SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_CARD),       HIDE );
	SetSpritePriority( &maxstat.bgsprt[fot][_CONF_BG_PLATE],       HIDE );
	SetSpritePriority( MENUgetSprtxPtr ( _ID_COPY ),          4 );
	SetSpritePriority( MENUgetSprtxPtr ( _ID_ACOPY ),         5 );

	{
	    int item;
	    extern int EventHistory;
	    extern int Target;

	    MENUpullCurrentItem();
	    item = MENUgetCurrentItem();
	    switch (item) {
	      case _CONF_ID_YES:
	  	switch (EventHistory) {
		  case 4:	/* Copy */
	            MemCardCopy();
		    break;
		  case 5:	/* Copy All */
	            MemCardACopy();
		    break;
		  case 6:	/* Delete */
	            MemCardDelete();
		    break;
		    default :
		    break;
	  	}
	      case _CONF_ID_NO:
		Target = 0;
		break;
	      default:
		break;
	    }
	    MENUsetCurrentItem (EventHistory);
	}
	while(MENUpullCurrentItem() != -1);
	SetStringUVclr(0);
    }
}

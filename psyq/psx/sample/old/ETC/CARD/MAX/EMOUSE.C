/* $PSLibId: Run-time Library Release 4.4$ */
/*****************************************************************
 *
 * file: emouse.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
/*
 * Crarify each function by functionality header because EMOUSE.C
 * consists of some extended features.
 *  MIAN:       Main routine related functions in MAX.C
 *  CDP:        CDLIB.C related
 *  CARD:       CARD.C related
 *  MOUSE:      Functions for implementing mouse input 
 * */

#include "libps.h"

#include "maxtypes.h"
#include "psxload.h"
#include "spritex.h"
#include "menux.h"
#include "const.h"
#include "maxstat.h"

#include "menuset2.h"

#ifdef EMOUSE

#include "emouse.h"

/*#define DEBUG_EMOUSE       /* debug print ON */

/* Permit input from second pad even when first pad is active 
 * if this define is out of comment.
 * */
/*#define ENABLE_2P /* */

/* Definition and grobal variables */
extern void ReverseSprite( SPRTX* sprt );

extern void EMouseVclear(void) ;
static void EMouseAnimeON(void) ;
 
/* Max number of menu item */
#define ITEMMAX		96

/* Size of EXIT */
#define EXIT (DELTA-LINEW*2)

/* Range of full screen transition */
#define FULL_X		0
#define FULL_Y		8
#define FULL_DX		(640-DELTA/2-8)
#define FULL_DY		(480-48)

/* CARD: card icon area */
#define	CARDA_X		(XC1-12)
#define	CARDB_X		(XC2-36)
#define	CARD_DX		(DC*3+24)
#define	CARD_Y		(YC-12)
#define	CARD_DY		(DC*5)

/* CARD: card command area */
#define	CARDCOM_X		(320-86-12)
#define	CARDCOM_DX		172
#define	CARDCOM_Y		(138-25-12)
#define	CARDCOM_DY		263

/* CARD: card confirmation area */
#define	CARDCF_X		(320-130-12)
#define	CARDCF_DX		260
#define	CARDCF_Y		(240-60-12)
#define	CARDCF_DY		120

/* Menu status : copy from pad.c */
#define MAIN_MENU    0
#define MEMORY_CARD  1
#define CD_PLAYER    2
#define CD_PLAY_MODE 3
#define MESSAGE_VIEW 4

/* External reference */
extern int  ScreenMode;
extern u_long	padd, opadd;
extern MAXSTAT	maxstat;

/* Mouse cursor */
#include "mouse.h"

#define	MOUSETYPEMAX	1

#define	MAXPAD		2
#define	RECVLEN		32

/* Suspend input from button until button information becomes zero 
 * if menu item is executed.
 * */
/* Use of BT_ON/BT_OFF as ON/OFF status
 * */
#define	BT_ON	1
#define	BT_OFF	0
static int BTcontrol = BT_OFF ;

/* Card menu when card menu is set ON */
static int cardmenu = BT_ON ;

/* Menu ready */
static	int	menudelay = 0 ;

/*CARD:
 * Display command arrows for appropriate direction on card menu.
 * Save the current direction for that.
 * Card command reverse flag
 *      1 when reversed
 *      [0]: COPY
 *      [1]: ALLCOPY
 *      [2]: DELETE
 * */
static int	revflag[] = {0,0,0} ;
static int	revcard = 0 ;		/* Reversed command number 0..2 */
static int	revexecflag = 0 ;

/*
 * Background of EXIT
 * Executing exit animation by changing this sprite.
 * */
static SPRTX	sprt_exit ;

/*MOUSE:
 * TIM data entry
 * Register TIM data pointer as mouse cursor type.
 * For increasing types, increase MOUSETYPEMAX and add pointer
 * for TIM data.
 * */
static unsigned long *mousecursor[MOUSETYPEMAX] = {
	mouse_cursor
} ;

/*MOUSE:
 * Re-register mouse cursor as display type */
static SPRTX	CursorSprites[MOUSETYPEMAX];

static int	display = 1 ;		/* MOuse cursor display ON/OFF */
static int	cursorN = 0 ;		/* Mouse cursor number for display  */
static int	mouseRSX = 0 ;		/* Range of transition of mouse */
static int	mouseRSY = 0 ;
static int	mouseREX = 640 ;
static int	mouseREY = 480 ;
static int	mouseX = 10 ;		/* Current location of mouse */
static int	mouseY = 10 ;
static int	mouseBT = 0 ;		/* Mouse button status */

static int      sencible=1;

/* Data is set to this buffer by interrupt program
 * */
/* Do not mind on user side because decoding is executed in lower level routine of 
 * EMouse.
 * */
static unsigned char	PADbuf[MAXPAD][RECVLEN] ;	/* Receive buffer */

/* Low level routine */
/*
 * Reverse FT4 polygon. Input variable must be SPRTX. */
void	ReverseSprite( SPRTX* sprt )
{
	POLY_FT4	*poly ;
	int	x0, y0 ;
	int	x1, y1 ;
	int	x2, y2 ;
	int	x3, y3 ;

	poly = &sprt->poly;
	x0 = poly->u0 ;
	x1 = poly->u1 ;
	x2 = poly->u2 ;
	x3 = poly->u3 ;
	y0 = poly->v0 ;
	y1 = poly->v1 ;
	y2 = poly->v2 ;
	y3 = poly->v3 ;
	setUV4(poly,
	 x1, y1,
	 x0, y0,
	 x3, y3,
	 x2, y2
	) ;
}

/*MOUSE:
 * Change of mouse cursor
 * type: mouse cursor type
 * */
void EMouseType(int type)
{
	cursorN = type ;
}

/*MOUSE:
 * Display of mouse cursor */
void EMouseON(void)
{
	display = 1 ;
}

/*MOUSE:
 * Non-display of mouse cursor */
void EMouseOff(void)
{
	display = 0 ;
}


/*MOUSE:
 * Transfer mouse cursor */
void EMouseMove( int x, int y)
{
	if( x < mouseRSX) x = mouseRSX ;
		else if( x > mouseREX) x = mouseREX ;
	if( y < mouseRSY) y = mouseRSY ;
		else if( y > mouseREY) y = mouseREY ;
	mouseX = x ;
	mouseY = y ;
}

/*MOUSE:
 * X position of mouse cursor*/
int EMouseX( void)
{
	return mouseX ;
}

/*MOUSE:
 * Y position of mouse cursor */
int EMouseY( void)
{
	return mouseY ;
}

/*MOUSE:
 * Poling pad information
 *      ch:     Channel number of judged PADbuf
 *      dx,dy:  Range of mouse transition 
 *      bt:     Button information 
 *      return:       -1     Fail of data obtaining
 *                Upper 4 bits:  Terminal type
 *  */
int EMousePadStat( int ch, int *dx, int *dy, u_long *bt)
{
        static u_long bt_latest=0;
	int	err;
	u_long  tbt ;

	*dx = 0 ;
	*dy = 0 ;
	*bt = 0 ;

#ifdef DEBUG_EMOUSE
	printf("ch=%d:%02x, %02x, %02x, %02x, %02x, %02x ",
	       ch, PADbuf[ch][0], PADbuf[ch][1], PADbuf[ch][2],
	       PADbuf[ch][3], PADbuf[ch][4], PADbuf[ch][5]);
	printf("%02x, %02x, %02x ",
	       PADbuf[ch][6], PADbuf[ch][7], PADbuf[ch][8]) ;
	printf("\r");
#endif
	if( PADbuf[ch][0] != 0x00) {
		return -1 ;	/* Receive failure */
	}

	switch( PADbuf[ch][1] & 0xf0) {
#ifdef NOMOUSE
	case 0x10:	/*XY MOUSE */
	        err = -1;
		break;
#else
	case 0x10:	/*XY MOUSE */
		if( PADbuf[ch][1] != 0x12) {
			err = -1 ;
			break ;
		}
		*dx = (signed char)PADbuf[ch][4] ;
		*dy = (signed char)PADbuf[ch][5] ;
		*bt = 0 ;
		tbt = 0;

		/* Corresponds to 16-button pad.
                  Left button : circle,  Right button : start */
		if( (~PADbuf[ch][3]) & 8) *bt |= _PADRright ;	/* Circle */
		if( ScreenMode == MEMORY_CARD ) {
		    if( (~PADbuf[ch][3]) & 4) *bt |= _PADh ;	/*start */
		    if( (*bt&0xffff) == 0x0820) {
			*bt = _PADl | _PADm | _PADn | _PADo ;
		    }
		}
		err = PADbuf[ch][1] & 0xf0 ;
		/*
		 * Mask input from mouse:
                 * Avoid ON status on card menu.
                 * */
		if( BTcontrol == BT_ON) {
			if( *bt == 0) BTcontrol = BT_OFF ;
			else {
				*bt = 0 ;
				*dx = 0 ;
				*dy = 0 ;
			}
		}
		break ;
#endif /* NOMOUSE */
	case 0x20:	/*16BT ANALOG */
	case 0x40:	/*16BT */
		*bt = (PADbuf[ch][2] << 8) | PADbuf[ch][3] ;
		*bt = ~*bt & 0xffff ;
		err = PADbuf[ch][1] & 0xf0 ;
		break ;
	default:
		err = -1 ;
		break ;
	}
	return err ;
}

extern int funcokflag;
/*MOUSE:
 * Obtaining mouse cursor status
 *      x,y:    Mouse coordination
 *      bt:    Button information
 *      return:     -1    Fail of data obtaining
 *              Upper 4-bits: Terminal type
 **/
int EMouseStat( int *x, int *y, u_long *bt)
{
	int	dx, dy ;
	int	_dx, _dy ;
	u_long  rbt, _rbt ;
	int	err, _err ;

	err = EMousePadStat( 0, &dx, &dy, &rbt) ;
	if( err == -1) {
		/* Poling of second pad if first pad is not connected. */
		err = EMousePadStat( 1, &dx, &dy, &rbt) ;
	}
#ifdef ENABLE_2P
	else if( err != 0x10 && rbt == 0) {
		/* Poling of second pad if first pad is not used. */
		/* Active when first pad is not mouse */
		_dx = dx ;
		_dy = dy ;
		_rbt = rbt ;
		_err = err ;
		err = EMousePadStat( 1, &dx, &dy, &rbt) ;
		if( (err == -1 || err == 0x10) && _err != -1) {
			/* Controller 1 information if controller 2 is invalid. */
			/* Return invalid information of controller 2 if controller
                          1 is invalid. */
			/* Controller 1 information if controller 2 is mouse
                         and controller 1 is active. */
			dx = _dx ;
			dy = _dy ;
			rbt = _rbt ;
			err = _err ;
		}
	}
#endif
	if( !funcokflag ) {
	    EMouseMove( mouseX + dx, mouseY + dy) ;
	    *x = mouseX ;
	    *y = mouseY ;
	    *bt = rbt ;
	}

#ifdef DEBUG_EMOUSE
	printf("err=%02x:%d, %d, %04x\n", err, *x, *y, *bt) ;
#endif
	return err ;
}


/* Animation & control */
/*CDP: Dented button position */
static int OFFbt_x = 0 ;
static int OFFbt_y = 0 ;
static int OFFbt2_x = 0 ;
static int OFFbt2_y = 0 ;

/* CARD: Display data of card icon. Obtaining EMouseCardDonfirm */
extern	int	Target ;	/* Tasrget icon id */
extern	int	Event ;		/* Event id on confirmation */
static	int CHGiconflag = 0;	/* Icon selected, yses=BT_ON */
static	SPRTX	ICONsprt ;	/* Save selected icon sprites */
static	SPRTX	BLUEsprt ;	/* Sprite on icon unelected */

static void EMouseTARGETdisp( DB *cdb) {}
static void EMouseOFFbt( DB *cdb) {}
void EMouseCDbt( int id) {}
void EMouseCDbt2( int id) {}
static void chgclut( int id, int flag) {}
void EMouse_dispreverb( int id) {}
static void EMouseCLUT() {}
static int EMouseInitOFFbt( void) {}
static int EMouseInitEXITbt( void) {}
static void EMouseAnimeDisp( DB *cdb) {}
static void EMouseInitCard() {}
void EMouseRevCard(int com, int direc) {}
void EMouseRevCardEXECON( int count) {}
void EMouseRevCardEXEC(int direc) {}
void EMouseRevCardEXECall() {}

/*MAIN:
 * Mouse cursor display hook
 * flag: 0        Depend on display/non-display of mouse cursor
 *       1        Force display of mouse cursor
 **/
void EMouseDisp( int flag, DB *cdb)
{
	int	x, y ;

	EMouseMove( mouseX, mouseY) ;	/* Limit set */
	x = mouseX ;
	y = mouseY ;
	if( (flag == 0 && display == 1) || flag == 1) {
	    SetSpritePosition( &CursorSprites[cursorN],
		 x+CursorSprites[cursorN].mx, y+CursorSprites[cursorN].my );
	    AddSprite2( cdb, &CursorSprites[cursorN]) ;
	}
}

/*MAIN:
 * Preparation of display of mouse
 **/
int EMouseInitDisp( void)
{
	int	i ;
	SPRTX *sprt ;

	EMouseInitEXITbt() ;
	EMouseInitOFFbt() ;
	EMouseInitCard() ;
	EMouseCLUT() ;

	sprt = CursorSprites ;
	for( i = 0; i < MOUSETYPEMAX; i++) {
              /* 95/03/24 sprt -> CursorSprites */
              MakeSpriteData( &CursorSprites[i], mousecursor[i], 0 );
              SetSpritePosition( &CursorSprites[i], 200, 100 );
              SetSpriteTrans( &CursorSprites[i], 0 );
              SetSpriteShadeTex( &CursorSprites[i], 1 );
              CursorSprites[i].priority = 1 ;         /* Max priority */
	}
	if( ScreenMode != MEMORY_CARD )
	  funcokflag = 0;

	return 0;
}

/*MOUSE:
 * Setting range of mouse transition
 *       x,y:   Upper left position of rectangle
 *       dx,dy: Size of rectangle
 **/
void EMouseRange( int x, int y, int dx, int dy)
{
	mouseRSX = x ;
	mouseRSY = y ;
	mouseREX = mouseRSX + dx ;
	mouseREY = mouseRSY + dy ;
}

/*MAIN:
 * Initialization of mouse module */
int EMouseInit(void)
{
	EMouseVclear() ;
        InitPAD(PADbuf[0], MAXPAD, PADbuf[1], MAXPAD);
       	EMouseMove( 80, 100);
        StartPAD();
	ChangeClearPAD(0);

	return 0;
}

/*MAIN:
 * Move current id according to mouse transition.
 * return: 0    mouse is not on menu item
 *        -1    mouse is on menu item
 *        -2    exit animation 
 *         1    card motion, set pad value for padret
 **/
int EMouseMoveMenu( int mx, int my, SPRTX **sprt, u_long padd, u_long *padret)
{
    int	x, y, dx, dy, i, current_id, group  ;
    SPRTX	*sprite ;
    extern void	funcIcon( long pad );

    if( revexecflag) {
	revexecflag = 0 ;
	EMouseRevCardEXEC( 0) ;
    }

    group = 0 ;
    current_id = MENUgetCurrentItem();
    /*Obtaining current group */
    for( i = 0;; i++) {
	if( MENU[i].RM.id == 0) continue ;  /* break -> continue 95/04/05 */
	if( current_id == MENU[i].RM.id) {
	    group = MENU[i].RM.group ;
	    break ;
	}
    }

    /* Special process for memory card icon.
       Issue pad transition according to mouse position. */
    /* Caution when card menu is changed.
       Group number is checked directly. */
    if ( ScreenMode == MEMORY_CARD && cardmenu == BT_OFF) {
	if( mx > CARDA_X+CARD_DX-15 && group == 3) {
	    *padret = _PADLright ;
	    return 1 ;
	}
	if( mx < CARDB_X+15 && group == 4) {
	    *padret = _PADLleft ;
	    return 1 ;
	}
    }

    /* Adjest mouse position */
    mx = mx + CursorSprites[cursorN].h / 2 ;
    my = my + CursorSprites[cursorN].w / 2 ;

    /* Menu transit processing */
    for( i = 0; i < ITEMMAX; i++) {
	if( MENU[i].RM.id == 0) continue ;
	if( MENU[i].RM.execFunc == NULL) continue ;
	if( MENU[i].Sprites[fot].priority == HIDE ) continue;

	sprite = &(MENU[i].Sprites[fot]) ;

	dx = MENU[i].RM.mw ;
	dy = MENU[i].RM.mh ;
	x = sprite->x - dx / 2 ;
	y = sprite->y - dy / 2 ;

	/* Menu moving when mouse is on sprite. */
	if( (x < mx) && ((x+dx) > mx) && (y < my) && ((y+dy) > my) ) {
	    if( MENU[i].RM.id != current_id ) {
		/* Menu is not set when effect is 0x20 (on moving). */
		if( MENU[i].RM.effect&0x20 ) ;
		else {
		    if( ScreenMode == CD_PLAY_MODE && MENU[i].RM.id == 22 ) {
			current_id = 10;
			MENUsetCurrentItem( current_id );	/*Moving */
		    }
		    else {
			MENUsetCurrentItem( MENU[i].RM.id );	/*Moving */
		    }
		}
	    }

	    *sprt = sprite ;
	    /*
	      * Display card icon name
              * Whether icon area is or not is judged if 
              * funcIcon is set on menu.
              * */
	    if( MENU[i].RM.execFunc == funcIcon) {
		/* Guide is not displayed when effect is 0x20. */
		if( MENU[i].RM.effect&0x20 ) ;
		else {
		    SetStringUVclr(1);
		    SetStringUV( MENU[i].RM.id) ;
		}
	    } else if( sencible ) {
		if( MENU[i].RM.execFunc == funcExit && (padd & OKKey)) {
		    /* End of door open & animation */
		    EMouseAnimeON() ;
		    return -2 ;
		}
	    }
	    return -1 ;
	}
    }

    if ( ScreenMode == MEMORY_CARD && cardmenu == BT_OFF) {
	if ( my > CARD_Y && my < CARD_Y+CARD_DY ) {
	    if ( mx > CARDA_X && mx < CARDA_X+CARD_DX-15 ) {
		SetStringUV(0);
	    } else if ( mx > CARDB_X && mx < CARDB_X+CARD_DX-15 ) {
		SetStringUV(1);
	    }
	}
    }

    return 0 ;
}

/*MAIN:
 * Menu translation processing on PAD moving
 * return: 0    No special processing
 *         -2   exit animation processing
 **/
static int EMouseCheckMenu( u_long padd)
{
	int	i, id, group  ;

	if( revexecflag) {
		revexecflag = 0 ;
		EMouseRevCardEXEC( 0) ;
	}

	group = 0 ;
	id = MENUgetCurrentItem();
	/* Obtaining current group */
	for( i = 0;; i++) {
		if( MENU[i].RM.id == 0) break ;
		if( id == MENU[i].RM.id) {
			group = MENU[i].RM.group ;
			break ;
		}
	}
	for( i = 0;; i++) {
		if( MENU[i].RM.id == 0) break ;
		if( MENU[i].RM.id != id) continue ;
		if( MENU[i].RM.execFunc == funcExit && (padd & OKKey)) {
			/* End of door open animation */
			EMouseAnimeON() ;
			return -2 ;
		}
	}
	return 0 ;
}

/*CARD:
 * Full screen display of range of card transition */
void EMouseSetFree()
{
	EMouseRange( FULL_X, FULL_Y, FULL_DX, FULL_DY);
	EMouseMove( 100, 100);
	menudelay = 2 ;
	BTcontrol = BT_ON ;
	cardmenu = BT_OFF ;
}

/*CARD:
 * Set range of card transition for command range */
void EMouseSetCardHome()
{
	SPRTX	*sprite ;

	if( CHGiconflag == BT_ON) {
		CHGiconflag = BT_OFF ;
		sprite = &ICONsprt ;
		SetSpritePriority( sprite, (u_long)HIDE );
		SetSpritePosition( sprite, BLUEsprt.x, BLUEsprt.y) ;
	}

	EMouseRevCardEXECall() ;

	EMouseRange( CARDCOM_X, CARDCOM_Y, CARDCOM_DX, CARDCOM_DY);

        sprite = MENUgetSprtxPtr(MENUgetCurrentItem());
        EMouseMove( sprite->x-DELTA/4, sprite->y-DELTA/4);
	BTcontrol = BT_ON ;
	cardmenu = BT_ON ;
}

/*CARD:
 * Set range of card transition for confirmation range */
void EMouseSetCardConfirm()
{
	EMouseRange( CARDCF_X, CARDCF_Y, CARDCF_DX, CARDCF_DY);

        if( Event == _ID_DELETE )
          EMouseMove( CARDCF_X+CARDCF_DX/2+DELTA, CARDCF_Y+CARDCF_DY/2);
        else
          EMouseMove( CARDCF_X+CARDCF_DX/2-DELTA, CARDCF_Y+CARDCF_DY/2);

	BTcontrol = BT_ON ;
	cardmenu = BT_OFF ;
}


/*CARD:
 * Set range of card transition for A icon range */
void EMouseSetCardIconA()
{
	EMouseRevCardEXEC( 0) ;

	EMouseRange( CARDA_X, CARD_Y, CARD_DX, CARD_DY);
       	EMouseMove( CARDA_X+CARD_DX/2, CARD_Y+CARD_DY/2);
	BTcontrol = BT_ON ;
	cardmenu = BT_OFF ;
}

/*CARD:
 * Set range of card transition for B icon range */
void EMouseSetCardIconB()
{
	EMouseRevCardEXEC( 1) ;

	EMouseRange( CARDB_X, CARD_Y, CARD_DX, CARD_DY);
       	EMouseMove( CARDB_X+CARD_DX/2, CARD_Y+CARD_DY/2);
	BTcontrol = BT_ON ;
	cardmenu = BT_OFF ;
}

/* Animation by end button */
int exitanime = 0 ;
static SPRTX	*animesprt ;

/*MAIN:
 * Refer by flag for showing if animation processing is on, max.c. */
int EMouse_get_exitanime()
{
	return exitanime ;
}

/*MAIN:
 * Final animation processing (for card & CDP)
 * Called after animation 
 **/
static void EMouseAnimeON(void)
{
	int	item ;
	POLY_FT4	*poly ;

	item = MENUgetCurrentItem();
	animesprt = &sprt_exit ;
	exitanime = 8*10 ;
	poly = &(animesprt->poly) ;
}

/*MAIN:
 * Operaton on only main menu
 *   Support with temporary function because only 2 item on menu
 *   is slided .
 **/
static void MoveCursorMain( DB *cdb, SPRTX *cursor )
{
    SPRTX *current;

    if( ScreenMode != MESSAGE_VIEW ) { /* Warning */
	current = MENUgetSprtxPtr( MENUgetCurrentItem() );

	SetSpritePosition( cursor,
			  current->x+cursor->w/2, current->y+cursor->h*5/2 );
	AddSprite2( cdb, cursor );
    }
}

/**********************************
MAIN:
  Input area for mouse/pad shared
  The following extern is used.
       extern u_long padd;
       exturn MAXSTAT maxstat;
       exturn int fot ; */

static	int	delay = 0 ;		/* Counter for main menu blink  */
static	int	delaya = 64 ;		/* Counter for main menu blink */
static	int	delayb = 64 ;		/* Counter for main menu blink */
static	SPRTX	*sprite ;		/* Sprite X of mouse hit */
static	int	ctype ;			/* Device type */

void EMouseIN( DB *cdb)
{
	int		mx, my, i ;
	u_long          bt, cardpad ;
	int		col_b, col_g, col_r ;	/* Work variables for blink */
	POLY_FT4	*poly ;

	if( exitanime > 0 ) {
	    /* Animation of end button */
	    return ;
	} else if ( delay ) {
	    /* Blink of button on main */
	    return ;
	}
	/* NOrmal mouse/pad process */
        ctype = EMouseStat( &mx, &my, &bt) ;
        padd = bt ;
        if( ctype == 0x10) {
	    /* MOuse operation */
	    i = EMouseMoveMenu( mx, my, &sprite, padd, &cardpad) ;

	    if( padd == 0 ) {
		sencible = 1;
	    }

	    if( i == 0) {
		if( !(padd & _PADh) ) {   /* Right button is active out of area. */
		    if( padd != 0 ) {
			sencible = 0;
		    }
		    padd = 0 ;	/* No action it is not on menu item. */
		}
	    }
	    else if( i == -2) {
		exitanime = 0;
		delay = 64;
		padd = 0;
	    }
	    else {
		if( i == 1 && padd == 0) {
		    padd = cardpad ;	/* Transition of card icon */
		}
	    }

	    if( !sencible ) padd = 0;
	
	    if ( ScreenMode == MAIN_MENU && (padd & OKKey) ) {
		delay = 64 ;
		padd = 0 ;
	    }
        } else if( ctype != -1) {
	    /* Handle as 16-button pad if not mouse. */
	    i = EMouseCheckMenu( padd) ;
	    if( i == -2) {
		padd = 0 ;	/* Animation */
		exitanime = 0;
		delay = 64;
		sprite = MENUgetSprtxPtr( MENUgetCurrentItem() );
	    }
	    if ( ScreenMode == MAIN_MENU && (padd & OKKey) ) {
		/* Blink on main */
		delay = 64 ;
		padd = 0 ;
		sprite = MENUgetSprtxPtr( MENUgetCurrentItem() );
	    }
        }
}

/**********************************
MAIN:
 Display of mouse/pad shared 
 The following extern is used.
        extern u_long padd;
        extern MAXSTAT maxstat;
        extern int fot; */
void EMouseDISP( DB *cdb)
{
	int		mx, my, i ;
	int		col ;	/* Work variables for blink */
	POLY_FT4	*poly ;

	if ( ScreenMode == MESSAGE_VIEW ) {
		return ;
	}

	if( exitanime > 0) {
		/* Animation of end button */
		poly = (POLY_FT4 *)&(animesprt->poly) ;
		exitanime-- ;
		if( exitanime == 0) {
			padd = OKKey ;		/* EXIT */
		} else if( (exitanime % 10) == 0) {
			setXY4(poly, poly->x0, poly->y0, poly->x1 - 1, poly->y1 + 1,
			       poly->x2, poly->y2, poly->x3 - 1, poly->y3 - 1 );
			EMouseAnimeDisp( cdb) ;
		} else {
			EMouseAnimeDisp( cdb) ;
		}
	} else if ( delay) {
		/* Button blink processing on main */
		delay -= 2 ;
		col = ((delay-4)*2) & 0x7f ;
		SetSpriteRGB( sprite, col, col, col );
		if( delay <= 0) {
			padd = OKKey ;
			SetSpriteRGB( sprite, 128, 128, 128 );
		}
	} else {
		EMouseAnimeDisp( cdb) ;
	        if( ctype == 0x10) {
			/* Mouse operation */
			EMouseDisp( 0, cdb) ;
	        } else if( ctype != -1) {
		    MoveCursor( cdb, &maxstat.cursor[fot] );
	        }
	}
}

/*MAIN:
 *  Initialization of static variables */
void EMouseVclear(void)
{
	BTcontrol = BT_OFF ;
	cardmenu = BT_ON ;
	menudelay = 0 ;
	revflag[0] = 0 ;
	revflag[1] = 0 ;
	revflag[2] = 0 ;
	revcard = 0 ;
	revexecflag = 0 ;
	mousecursor[0] = mouse_cursor ;
	display = 1 ;		/* MOuse cursor display ON/OFF */
	cursorN = 0 ;		/* Display of mouse cursor number  */
	mouseRSX = 0 ;		/* Range of mouse transition */
	mouseRSY = 0 ;
	mouseREX = 640 ;
	mouseREY = 480 ;
	mouseX = 10 ;		/* MOuse current location */
	mouseY = 10 ;
	mouseBT = 0 ;		/* Mouse button status */
	/* Dented button location */
	OFFbt_x = 0 ;
	OFFbt_y = 0 ;
	OFFbt2_x = 0 ;
	OFFbt2_y = 0 ;
	/* Display data of card icon */
	CHGiconflag = 0;
	/*Animation of end button */
	exitanime = 0 ;
	delay = 0 ;			
	delaya = 64 ;			
	delayb = 64 ;			

	ICONsprt.priority = HIDE;

	sencible = 1;
}

#endif /* EMOUSE */

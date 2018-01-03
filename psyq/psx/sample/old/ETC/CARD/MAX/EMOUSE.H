/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _EMOUSE_H_
#define _EMOUSE_H_

/*****************************************************************
 *
 * file: emouse.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

/*
 * Mouse cursor display hook 
 *      flag    0       Depend on display/non-display of mouse cursor
 *              1       Force display of mouse cursor
 *  */
extern void EMouseDisp( int flag, DB *cdb) ;

/*
 * Change mouse cursor 
 *      type:   Mouse cursor type
 * */
extern void EMouseType(int type) ;

/*
 * Display of mouse cursor */
extern void EMouseON(void) ;

/*
 * Non-display of mouse cursor 
 * */
extern void EMouseOff(void) ;

/*
 * Transition of mouse cursor      x,y : transition position
 * */
extern void EMouseMove( int x, int y) ;

/*
 * X position of mouse cursor */
extern int EMouseX( void) ;

/*
 * Y position of mouse cursor */
extern int EMouseY( void) ;

/*
 * Poling of pad information
 *      ch:     Channel Number of judged PADbuf
 *      dx,dy:  Range of mouse transition 
 *      bt:     Button information
 *      return:      -1     Fail of data obtaining
 *              Upper 4 bits:       0x10: mouse
 *                                  0x20: 16-button analog
 *                                  0x40: 16-button
 *                                  0x80: Multi-tap
 * */
extern int EMousePadStat( int ch, int *dx, int *dy, u_long *bt) ;

/*
 * Obtaining mouse cursor status
 *      x,y:    mouse coordination values
 *      bt:     button information
 *      return:       -1        fail of data obtaining
 *               Upper 4 bits:  terminal type
 * */
extern int EMouseStat( int *x, int *y, u_long *bt) ;

/*
 * Preparation of display of mouse */
extern int EMouseInitDisp( void ) ;


/*
 * Setting of mouse sensitivity */
/*extern void EMouseSense( float sensx, float sensy) ;*/

/*
 * Setting of mouse transition area
 *      x,y :   Upper left location of rectangle 
 *      dx,dy:  Size of Rectangle
 * */
extern void EMouseRange( int x, int y, int dx, int dy) ;

/*
 * Initialization of mouse module */
extern int EMouseInit( void) ;

/*
 * Moving current group */
extern int EMouseMoveMenu( int mx, int my, SPRTX **sprt, u_long padd, u_long *padret) ;

/*
 * Display of insert status of surround button 
 *      Use of RM4 id = 61..65
 *      id:0.. display id
 * */
extern void EMouse_dispreverb( int id );

/*
 * Hook for changing direction of command icon with wait 
 * */
extern void EMouseRevCardEXECON( int count) ;


/*
 * Restrict transition range of card */
extern void EMouseSetFree();
extern void EMouseSetCardHome();
extern void EMouseSetCardConfirm();
extern void EMouseSetCardIcon();

#endif /* _EMOUSE_H_ */
/* DON'T ADD STUFF AFTER THIS */

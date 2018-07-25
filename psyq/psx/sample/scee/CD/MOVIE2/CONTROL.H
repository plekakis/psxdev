/************************************************************
 *                                                          *
 *                       control.h                          *
 *                                                          *
 *                                                          *
 *                Vince Diesi     13/02/97                  *
 *                                                          *
 *   Copyright (C) 1997 Sony Computer Entertainment Inc.    *
 *                  All Rights Reserved                     *
 *                                                          *
 ***********************************************************/

#ifndef __CONTROL_H
#define __CONTROL_H

#include "ctrller.h"

/* ---------------------------------------------------------------------------
 * - MACRO DEFINITIONS
 * ---------------------------------------------------------------------------
 */

// Check controllers every second.
#define CHECK_CONTROLLERS		50


// New (more meaningful) controller button names.
#define R2_KEY			PAD_FRB
#define L2_KEY			PAD_FLB
#define R1_KEY			PAD_FRT
#define L1_KEY			PAD_FLT
#define TRIANGLE_KEY	PAD_RU
#define X_KEY			PAD_RD
#define SQUARE_KEY		PAD_RL
#define CIRCLE_KEY		PAD_RR
#define UP_KEY			PAD_LU
#define DOWN_KEY		PAD_LD
#define LEFT_KEY		PAD_LL
#define RIGHT_KEY		PAD_LR
#define SELECT_KEY		PAD_SEL
#define START_KEY		PAD_START

/* ---------------------------------------------------------------------------
 * - PUBLIC FUNCTION PROTOTYPES
 * ---------------------------------------------------------------------------
 */

void InitControllers(void);

/* ------------------------------------------------------------------------ */

void StopControllers(void);

/* ------------------------------------------------------------------------ */

void CheckControllers(void);

/* ------------------------------------------------------------------------ */

short Pressed(short button);

/* ------------------------------------------------------------------------ */

#endif // __CONTROL_H 

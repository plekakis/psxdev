/************************************************************
 *                                                          *
 *                       control.c                          *
 *                                                          *
 *                                                          *
 *               Vince Diesi     13/02/96                   *
 *                                                          *
 *   Copyright (C) 1996 Sony Computer Entertainment Inc.    *
 *                  All Rights Reserved                     *
 *                                                          *
 ***********************************************************/

/* ---------------------------------------------------------------------------
 * - Description:
 * - ------------
 * - Controller functions. Only supports the standard controller. 
 * ---------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <kernel.h>
#include <libsn.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>

#include "ctrller.h"
#include "control.h"

/* ---------------------------------------------------------------------------
 * - CONSTANTS 
 * ---------------------------------------------------------------------------
 */

// Define for timing and testing functions.
// #define TESTING


// Controllers connected.
#define NO_PADS			0
#define PAD_ONE			1
#define PAD_TWO			2
#define BOTH_PADS		3

/* ---------------------------------------------------------------------------
 * - GLOBAL DEFINITIONS 
 * ---------------------------------------------------------------------------
 */

static volatile short connected = 0;			// No. controllers connected.
static volatile short currController = 0;		// Current active controller.

static volatile ControllerPacket buffers[2];

/* ---------------------------------------------------------------------------
 * - FUNCTION DEFINITIONS
 * ---------------------------------------------------------------------------
 */
 
void InitControllers(void) {


/* - Type:	PUBLIC 
 * -
 * - Usage: Init controllers. 
 */


	InitPAD((char *) &buffers[0], MAX_CONTROLLER_BYTES, (char *) &buffers[1], MAX_CONTROLLER_BYTES);
	StartPAD();
	ChangeClearPAD(0);


	VSync(0);
	CheckControllers();
}

/* ------------------------------------------------------------------------ */

void StopControllers(void) {


/* - Type:	PUBLIC 
 * -
 * - Usage: Stop controllers. 
 */


	StopPAD();
	connected = 0;
	currController = 0;
}

/* ------------------------------------------------------------------------ */

void CheckControllers(void) {


/* - Type:	PUBLIC 
 * -
 * - Usage:	Check number of controllers connected (stored in connected).
 * - 		Also selects the active controller (stored in currController). 
 * - 		Should be called in the VSyncCallback() to constantly check the
 * -		controller status.
 */


	connected = 0;


	if (GoodData(&buffers[0]) && (GetType(&buffers[0]) == STD_PAD))
			connected |= PAD_ONE;

	if (GoodData(&buffers[1]) && (GetType(&buffers[1]) == STD_PAD))
			connected |= PAD_TWO;

	
	if (connected == BOTH_PADS || connected == PAD_ONE)
		currController = 0;

	else if (connected == PAD_TWO)
		currController = 1;
}

/* ------------------------------------------------------------------------ */

short Pressed(short button) {


/* - Type:	PUBLIC 
 * -
 * - Param:	button = (In) Button to test.
 * -
 * - Ret:	1 if pressed, 0 otherwise.
 * -
 * - Usage:	Test if button has been pressed. 
 */


	return (connected && !(buffers[currController].data.pad & button));
}

/* ------------------------------------------------------------------------ */

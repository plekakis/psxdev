/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	preset.c
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "model.h"
#include "preset.h"

/* preset values for roll, pitch, yaw angles and translation  */
#define HLEN 300
#define WLEN 500
#define DLEN 300
#define GAPLEN 300

PRESET preset[][MIMENUM+1][PARTNUM]={
    {						/* simple test */
	{					/* BASE */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	},
	{					/* rotate */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{1000,0,0,},{WLEN+GAPLEN,0,0}},
	},
	{					/* yokonobi */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN+300,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN+300,0,0}},
	},
	{					/* bend1 */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,-512,},{WLEN+GAPLEN,0,0}},
	    {{0,0,-512,},{WLEN+GAPLEN,0,0}},
	},
	{					/* bend2 */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{0,-800,0,},{WLEN+GAPLEN,0,0}},
	},
    },
    {						/* caterpillar*/
	{					/* BASE */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	},
	{					/* MIMe1 */
	    {{     0,     0,  -350}, {   400,   -50,     0}},
	    {{     0,     0,   850}, {   WLEN+GAPLEN,     0,     0}},
	    {{     0,     0,  -890}, {   WLEN+GAPLEN,     0,     0}},
	},
	{					/* MIMe2 */
	    {{     0,     0,     0}, {   550,     0,     0}},
	    {{     0,     0,     0}, {   WLEN+GAPLEN+50,     0,     0}},
	    {{     0,     0,     0}, {   WLEN+GAPLEN+50,     0,     0}},
	},
	{					/* MIMe3 */
	    {{     0,     0,     0}, {   550,     0,     0}},
	    {{     0,     0,     0}, {   WLEN+GAPLEN, 0,     0}},
	    {{     0,     0,     0}, {   WLEN+GAPLEN, 0,     0}},
	},
	{					/* MIMe4 */
	    {{     0,     0,     0}, {   550,     0,     0}},
	    {{     0,     0,     0}, {   WLEN+GAPLEN, 0,     0}},
	    {{     0,     0,     0}, {   WLEN+GAPLEN, 0,     0}},
	},
    },
    {						/* simple test 2 */
	{					/* BASE */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	},
	{					/* MIMe1 */
	    {{     0,   150,     0}, {     0,     0,     0}},
	    {{     0,  -320,     0}, {WLEN+GAPLEN,0,     0}},
	    {{     0,  -500,     0}, {WLEN+GAPLEN,0,     0}},
	},
	{					/* MIMe2 */
	    {{     0,     0,     0}, {     0,     0,     0}},
	    {{  -890,  -386,   220}, {WLEN+GAPLEN,0,     0}},
	    {{     0,     0,     0}, {WLEN+GAPLEN,0,     0}},
	},
	{					/* MIMe3 */
	    {{     0,     0,     0}, {     0,     0,     0}},
	    {{   110,     0,   330}, {WLEN+GAPLEN,0,     0}},
	    {{   220,     0,   460}, {WLEN+GAPLEN,0,     0}},
	},
	{					/* MIMe4 */
	    {{     0,     0,     0}, {     0,     0,     0}},
	    {{     0,     0,     0}, {WLEN+GAPLEN,0,     0}},
	    {{  1826,  1170, -2036}, {  1200,     0,     0}},
	},
    },
    {						/* base form*/
	{					/* BASE */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	},
	{					/* MIMe1 */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	},
	{					/* MIMe2 */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	},
	{					/* MIMe3 */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	},
	{					/* MIMe4 */
	    {{0,0,0,},{0,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	    {{0,0,0,},{WLEN+GAPLEN,0,0}},
	},
    },
    {					/* end of preset data */
	{
	    {{ -30000,},},
	},
    }
};

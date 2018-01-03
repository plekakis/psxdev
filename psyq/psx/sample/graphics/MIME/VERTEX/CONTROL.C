/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	MIMe Animation
 *
 *	"control.c" ******** routine
 *
 *		Version 1.**	Mar,  14, 1994
 *
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>  
#include "control.h"  

/* ring buffer for convolution  */

int cntrlarry[CTLMAX][CTLTIME]; 

/* data array for control function  */
CTLFUNC ctlfc[CTLMAX];

/* initialize ring buffer & data array */
init_cntrlarry(cnvwave,number)
int *cnvwave;
int number;
{
int i,j;
	for(i = 0 ; i < CTLMAX; i++) {
		ctlfc[i].cnv = cnvwave;
		ctlfc[i].num = number;
		ctlfc[i].in = 0;
		ctlfc[i].out = 0;
		for(j = 0 ; j < CTLTIME; j++) cntrlarry[i][j]=0;
	}
}

/* generate control wave by transfer function */
set_cntrl(frm)
u_long frm;
{
int i,j;
	for(i = 0 ; i < CTLMAX; i++) {
		cntrlarry[i][ (frm) % ctlfc[i].num ] = ctlfc[i].in;	/* input data */
		ctlfc[i].out=0;
		for(j = 0 ; j < ctlfc[i].num; j++) {
		   ctlfc[i].out += cntrlarry[i][(frm - j ) % ctlfc[i].num] * (*(ctlfc[i].cnv+j));
		}
		ctlfc[i].out = ctlfc[i].out >> 12;			/* output data */
	}
}

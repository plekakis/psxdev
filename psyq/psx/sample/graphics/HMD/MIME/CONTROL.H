/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *		Copyright (C) 1994-1997 by Sony Corporation
 *			All rights Reserved
 *		
 *		"control.h"
 */

#define CTLMAX 	16	/* Maximum number of control waveform channels */

#define CTLTIME 32	/* Maximum length of transfer function waveform */

/* Control waveform-generating program structure */
typedef struct {
	int  *cnv;	/* Transfer function waveform data pointer */

	int num;	/* Transfer function waveform data length */

	int in;		/* Input */
	int out;	/* Output */

} CTLFUNC;

extern void	init_cntrlarry(int cnvwave[], int number,
			CTLFUNC ctlfc[CTLMAX], int cntrlarry[CTLMAX][CTLTIME]);
extern void	set_cntrl(u_long frm,
			CTLFUNC ctlfc[CTLMAX], int cntrlarry[CTLMAX][CTLTIME]);

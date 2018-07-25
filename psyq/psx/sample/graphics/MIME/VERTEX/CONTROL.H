/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *		
 *		"control.h"
 */

#define CTLMAX 	10	/* Maximum number of control waveform channels */

#define CTLTIME 32	/* Maximum length of transfer function waveform */

/* Control waveform-generating program structure */
typedef struct {
	int  *cnv;	/* Transfer function waveform data pointer */

	int num;	/* Transfer function waveform data length */

	int in;		/* Input */
	int out;	/* Output */

} CTLFUNC;

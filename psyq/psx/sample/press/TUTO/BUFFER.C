/* $PSLibId: Run-time Library Release 4.4$ */
/*			buffer: test data buffer
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>

/* test BS pattern */
u_long mdec_bs[] = {
#include "siro.bs"
};

/* test buffer */
u_long mdec_image[256*256];
u_long mdec_rl[256*256];


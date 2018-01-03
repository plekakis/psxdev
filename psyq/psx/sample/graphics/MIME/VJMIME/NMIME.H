/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *		Copyright (C) 1994,1995 by Sony Computer Entertainment, Inc.
 *			All rights Reserved
 *		
 *		"nmime.h"
*/


#define MIMEMAX	8
#define OBJMAX	1
#define MIMEMODELMAX 16

extern long mimepr[MIMEMODELMAX][MIMEMAX];

int init_mime_data(int num, u_long *modeladdr, u_long *mdfdatavtx, u_long *mdfdatanrm, u_long *orgvtxbuf, u_long *orgnrmbuf);
void vertex_mime(int num);
void normal_mime(int num);
void reset_mime_vdf(int num);
void reset_mime_ndf(int num);

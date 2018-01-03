/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	preset.h
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
typedef struct _preset{
    SVECTOR angle;
    VECTOR trans;
} PRESET;

extern PRESET preset[][MIMENUM+1][PARTNUM];

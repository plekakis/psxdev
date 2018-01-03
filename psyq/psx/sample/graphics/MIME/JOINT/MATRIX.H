/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/***********************************************
 *	matrix.h
 *
 *	Copyright (C) 1996,1997 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
extern void EigenVector(MATRIX *m, VECTOR *t);
extern void EigenVec2Mat(VECTOR *ev, MATRIX *smat);
extern long newVectorNormal(VECTOR *v0, VECTOR *v1);

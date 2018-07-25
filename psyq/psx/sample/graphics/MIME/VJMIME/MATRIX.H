/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	matrix.h
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
long myVectorNormal(VECTOR *v0, VECTOR *v1);
void EigenVector(MATRIX *m, VECTOR *t);
void EigenVec2Mat(VECTOR *ev, MATRIX *smat);
long preVectorNormal(VECTOR *v);


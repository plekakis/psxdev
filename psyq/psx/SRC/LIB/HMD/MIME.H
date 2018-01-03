/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


typedef struct {
	u_short	nSize, nNum;
	u_long	idxMIMeDiff[0];
} VNMIMePrim;

typedef struct {
	u_short	nOnum, nDiffsNum;
	u_long	nDflags;
	u_long	idxMIMeDiffData[0];
} VNMIMeDiff;

typedef struct {
	u_long	idxVstart;
	u_short	res, nVnum;
	SVECTOR	svDiff[0];
} VNMIMeDiffData;

typedef struct {
	u_long	idxVstart;
	u_long	idxOstart;
	short	nChanged, nVnum;
} RstVNMIMeDiffData;

typedef struct {
	short	nZero, nDiffsNum;
	RstVNMIMeDiffData	diffData[0];
} RstVNMIMeDiff;

typedef struct {
	u_short	nSize, n;
	u_long	idxDiff[0];
} RstVNMIMePrim;

typedef struct {
	u_short	nSize, nNum;
	u_long	idxMIMeDiff[0];
} JntMIMePrim;

typedef struct {
	short	dvx, dvy, dvz;
	u_short	dtp;
	long	dtx, dty, dtz;
} JntMIMeDiffData;

typedef struct {
	u_short	nCoordId, nDiffsNum;
	u_long	nDflags;
	JntMIMeDiffData	jntMIMeDiffData[0];
} JntMIMeDiff;

typedef struct {
	short	m[3][3];
	u_short	changed;
	long	dtx, dty, dtz;
} RstAxesMIMeDiffData;

typedef struct {
	short	dvx, dvy, dvz;
	u_short	changed;
	long	dtx, dty, dtz;
} RstRPYMIMeDiffData;

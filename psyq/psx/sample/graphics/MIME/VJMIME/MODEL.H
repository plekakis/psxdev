/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	model.h
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/

/* number of parts for a person (a model) */
#define PARTNUM 3
/* number of MIMe parameters */
#define MIMENUM 4
/* number of display models */
#define DISPNUM (MIMENUM+2)

/* max number of vertex per part */
#define VNUMMAX 8

/* element number of model_hand[] */
#define ARR_BASE 0
#define ARR_MIME1 1
#define ARR_MIME2 2
#define ARR_MIME3 3
#define ARR_MIME4 4
#define ARR_INTR (MIMENUM+1)
#define ARR_ALL (-1)

/* for "ch_angle()". they must be different each other */
#define VX 1
#define VY 2
#define VZ 3
#define TX 4
#define TY 5
#define TZ 6
#define ANG2MAT 7

/* model handler */
typedef struct{
    GsCOORDINATE2 modelcoord;	/* coordinates for these vertex */
    long dtrans[3];		/* translation difference (for MIMe) */
    SVECTOR angle;		/* RPY angle (for RPY angle MIMe) */
    SVECTOR dangle;		/* RPY angle difference (for RPY angle MIMe) */
    VECTOR axis;		/* rotation axis (for axes-MIMe) */
    long theta;			/* rotation angle (for axes-MIMe) */
} MODEL;

extern MODEL model_hand[DISPNUM][PARTNUM];
extern int ARR_MIME[MIMENUM];


void model(GsOT *wot, int otlen);
int model_init(void);
void texture_init(int tnum, u_long *addr[]);

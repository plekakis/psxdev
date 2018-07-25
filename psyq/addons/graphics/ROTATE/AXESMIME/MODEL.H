/* $PSLibId: Runtime Library Release 3.6$ */
/***********************************************
 *	model.h
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#define OBJECTMAX 4		/* Max Objects for every TMD */
typedef struct{
    GsDOBJ5 object[OBJECTMAX];	/* Array of Object Handler*/
    GsCOORDINATE2 modelcoord;	/* coordinates for this TMD : このTMD を張り付ける座標系 */
    u_long Objnum;		/* number of Objects in this TMD */
    u_long *maddr;		/* address of TMD data : tmd データのある場所 */
    long dtrans[3];		/* translation difference (for MIMe) */
    SVECTOR angle;		/* Euler angle (for Euler angle MIMe) */
    SVECTOR dangle;		/* Euler angle difference (for Euler angle MIMe) */
    VECTOR axis;		/* rotation axis (for axes-MIMe) */
    long theta;			/* rotation angle (for axes-MIMe) */
} MODEL;

/* number of parts for a person (a model) */
#define PARTNUM 10
/* number of MIMe parameters */
#define MIMENUM 4
/* number of display models */
#define DISPNUM (MIMENUM+2)

#define ARR_BASE 0
#define ARR_MIME1 1
#define ARR_MIME2 2
#define ARR_MIME3 3
#define ARR_MIME4 4
#define ARR_INTR (MIMENUM+1)
#define ARR_ALL (-1)

/* for "ch_angle()". they must be different each other : 一意な数なら OK */
#define VX 1
#define VY 2
#define VZ 3
#define TX 4
#define TY 5
#define TZ 6
#define ANG2MAT 7

extern MODEL model_hand[DISPNUM][PARTNUM];
extern int ARR_MIME[MIMENUM];


void model(GsOT *wot, int otlen);
void model_init(void);
void texture_init(int tnum, u_long *addr[]);
void ch_angle(int arr, int obj, int var, int value);

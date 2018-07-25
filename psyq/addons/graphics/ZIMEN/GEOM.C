/* $PSLibId: Runtime Library Release 3.6$ */
/*				geom
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Jun,20,1995	suzu	
 *
 *		flush gemetry environment
 */
#include "sys.h"

/*
 * update the world-screen matrix with rap-rounded by (genv->rx, genv->ry)
 * Rap-rounding is needed for rap-rounded 3D BG mode.
 : ワールドスクリーンマトリクスを更新する。その際に(genv->rx, genv->ry)
 * でラップラウンドする。ラップラウンドは、繰り返し BG に必要
 */
void geomUpdate(GEOMENV *genv)
{
	SVECTOR dvw;	/* moving direction (in the screen) */
	VECTOR	home;	/* your position (in the world) */
	VECTOR	vec;	/* translation vector */
	MATRIX	rott;	/* inversed (transposed) rotation matrix */
	
	/* limit angle of your neck */
	limitRange(genv->angle.vx, 1024, 3072); 
	
	/* make local-screen matrix */
	RotMatrix(&genv->angle, &genv->mat);		

	/* get inverse matrix of local-screen matrix. 
	 * note that the transposed matrix is equal to inversed matrix,
	 * if the matrix is "unitary".
	 */
	TransposeMatrix(&genv->mat, &rott);

	/* translate the direction vector from the screen to the world */
	ApplyMatrixSV(&rott, &genv->dvs, &dvw);

	/* update your position (in the world) */
	genv->home.vx += dvw.vx;
	genv->home.vy += dvw.vy;

	/* wrap-round (if needed)
	 * Since Rotation matrix accepts only 16bit vector, it may be needed
	 * to translate the world vector before rotation
	 */
	home = genv->home;
	if (genv->rx)	home.vx &= genv->rx-1;
	if (genv->ry)	home.vy &= genv->ry-1;

	/* calculate translation matrix 
	 * Your position should be (0,0,0) in the screen coordinate, therefore
	 * the vector genv->home have to be translated to (0,0,0) by
	 * RotTran() operation. To accomplish this, translation vector must be
	 * as follows:
	 */
	ApplyMatrixLV(&genv->mat, &home, &vec);
	vec.vx = -vec.vx;
	vec.vy = -vec.vy;
	vec.vz = -vec.vz;

	/* set the GTE registers */
	TransMatrix(&genv->mat, &vec);		
	SetRotMatrix(&genv->mat);		
	SetTransMatrix(&genv->mat);		
	
#define DEBUG	
#ifdef DEBUG
	dumpVector("angle", &genv->angle);
	dumpVector("home",  &genv->home);
	dumpVector("dvs",   &genv->dvs);
#endif	
	/* clear direction vector */
	setVector(&genv->dvs, 0, 0, 0);

}

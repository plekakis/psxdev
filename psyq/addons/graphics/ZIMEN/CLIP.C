/* $PSLibId: Runtime Library Release 3.6$ */
/*				clip
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Jun,20,1995	suzu	
 *	1.10		Feb,15,1996	suzu	
 *
 *			   Area Clip
 :			エリアクリップ
 */
	
/*
 * areaClipZ	caluculate viewing area on the plane z = 0
 *
 * SYNOPSIS
 *	int areaClipZ(RECT *clips, RECT32 *clipw, int limit)
 *
 * ARGUMENT
 *	clips	rectangle area of the screen
 *	clipw	rectangle area of plane z = 0 in which the object is
 *		projected on the screen 'clipw'
 *	limit	horizon distance (maximum distance for far clip)
 *
 * DESCRIPTION
 *	areaClipZ returns the area 'clipw' of z = 0 in which the
 *	object is projected into the screen 'clips'. the farest
 *	projected area is limited in 'limit'. Ordinary 'limit' should
 *	be the same value as far clip distance.
 *
 * NOTE
 *	Geometrical paramters (projection, world-screen matrix etc)
 *	are derived from GTE registers. Therefore all these parameters
 *	have to be set to GTE in advance.
 *
 *	When camera vector is paralell with  plane z = 0 (when projected
 *	plane z = 0 is paralell with z axis), clipping calculation may
 *	be overflowed.
 *
 * RETURN
 *	always 0
 *	
 : areaClipZ	z = 0 の平面上でスクリーンに投影される範囲を求める
 *
 * 形式	int areaClipZ(RECT *clips, RECT32 *clipw, int limit)
 *
 * 引数		clips	スクリーン座標系での投影面の範囲
 *		clipw	z = 0 平面上でスクリーンに投影される範囲
 *		limit	投影される最大の距離
 *
 * 解説		clips で指定された投影面の範囲に透視変換される z = 0 
 *		の平面の部分を clipw で返す。投影される範囲は距離 
 *		limit 以下に制限される。通常 limit は遠方クリップの値
 *		に等しいものが設定される。
 *
 *
 * 備考		計算に必要な情報（プロジェクション・ワールドスクリーン
 *		マトリクスなど）は GTE のレジスタから取得される。その
 *		ため、マトリクス情報は前もって GTE に設定されている必
 *		要がある。
 *
 *		視線と z = 0 の平面が並行になった場合（スクリーン座標
 *		系に変換された時の z = 0 の平面が z 軸に並行になった場
 *		合） は正しく計算できない。
 *
 * 返り値	常に 0
 *
 ***************************************************************************/
	
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

static void screen_to_world(VECTOR *s, VECTOR *w);
static void rot_trans_pers(VECTOR *w, VECTOR *s);
static get_z_cross_point(VECTOR *pw, VECTOR *vw, VECTOR *ww, int limit);

/*
 * calculate mini-max of vertex: mini-max を求める
 */
#define max(x,y)	((x)>(y)?(x):(y))
#define max3(x,y,z)	((x)>(y)?max(x,z):max(y,z))
#define max4(x,y,z,w)	(max(x,y)>max(z,w)?max(x,y):max(z,w))

#define min(x,y)	((x)<(y)?(x):(y))
#define min3(x,y,z)	((x)<(y)?min(x,z):min(y,z))
#define min4(x,y,z,w)	(min(x,y)<min(z,w)?min(x,y):min(z,w))


/* RECT clips;		(input)  clip window (in the screen)
 * RECT clipw;		(output) clip window (on the z = 0 plane)
 * int	limit;		(input)  far clip point
 */
int areaClipZ(RECT *clips, RECT32 *clipw, int limit)
{
	
	VECTOR	vs[4], vw[4];	/* screen area (in screen and world) */
	VECTOR	ww[4];		/* wall edge (in world) */
	VECTOR	ps, pw;		/* yourself (in screen and world) */
	int	i, ret;
	int	h;
	
	/* set clip window on 2D  */
	int	min_x = clips->x, max_x = clips->x+clips->w;
	int	min_y = clips->y, max_y = clips->y+clips->h; 
	
	/* locate clip window in the screen coordinate */
	h = ReadGeomScreen();
	setVector(&vs[0], min_x, min_y, h);
	setVector(&vs[1], max_x, min_y, h);
	setVector(&vs[2], min_x, max_y, h);
	setVector(&vs[3], max_x, max_y, h);

	/* ps: your standing position in the screen (obviously 0, 0, 0) 
	 * pw: your standing position in the world 
	 */
	setVector(&ps, 0, 0, 0);
	screen_to_world(&ps, &pw);
	
	/* get the cross point between line pw->vw and plane z = 0 */
	for (i = 0; i < 4; i++) {
		
		/* get clip window position in the world */
		screen_to_world(&vs[i], &vw[i]);
		
		/* if cross point is overflow, adjust */
		if (get_z_cross_point(&pw, &vw[i], &ww[i], limit)) {
			/*FntPrint("overflow at %d\n", i);*/

			/* return on  screen screen coordinate */
			rot_trans_pers(&ww[i], &vs[i]);

			/* change the clip window */
			limitRange(vs[i].vx, min_x, max_x);
			limitRange(vs[i].vy, min_y, max_y);

			/* get the world position of the new clip window */
			screen_to_world(&vs[i], &vw[i]);

			/* retry */
			get_z_cross_point(&pw, &vw[i], &ww[i], limit);
		}
	}
		
	/* set the clip window on plane z = 0 */
	clipw->x = min4(ww[0].vx, ww[1].vx, ww[2].vx, ww[3].vx); 
	clipw->y = min4(ww[0].vy, ww[1].vy, ww[2].vy, ww[3].vy); 
	clipw->w = max4(ww[0].vx, ww[1].vx, ww[2].vx, ww[3].vx) - clipw->x; 
	clipw->h = max4(ww[0].vy, ww[1].vy, ww[2].vy, ww[3].vy) - clipw->y; 

	return(0);
}

/*
 * translate from the screen coordinate to the world coordinate.
 : スクリーン座標からワールド座標を求める
 */
/* VECTOR *s;		(input)  position in the screen coordinate
 * VECTOR *w;		(output) position in the world coordinate
 */
static  void screen_to_world(VECTOR *s, VECTOR *w)
{
	MATRIX		rot, rott;
	VECTOR		t;
	
	/* get current rot-trans matrix */
	ReadRotMatrix(&rot);
	
	/* push matrix since ApplyMatrixLV breaks the GTE matrix register */
	PushMatrix();
	
	/* screen = Rt * (world - t); */
	t.vx = s->vx - rot.t[0];
	t.vy = s->vy - rot.t[1];
	t.vz = s->vz - rot.t[2];

	/* If rotation matrix is "unitary", transposed matrix is equal to
	 * inversed matrix.   
	 */
	TransposeMatrix(&rot, &rott);
	ApplyMatrixLV(&rott, &t, w);
	
	/* recover matrix */
	PopMatrix();
}

/*
 * Calculate the cross point between line pw->vw and plane z = 0.
 * if the cross point is very far, it is limited in "limit".
 : 点 pw と vw を結ぶ直線と平面 z = 0 との交点をもとめる。
 * もし交点が十分遠ければ、limit に制限される
 */
/* VECTOR *pw;	(input)  home poisition in the world
 * VECTOR *vw;	(input)  screen edge position in the world
 * VECTOR *ww: (output) cross point on the z = 0 plane
 * int limit;	(input)  far clip
 */
#define HUGE	0x7fff		/* max value of 16bit: 16bit の最大値 */
static get_z_cross_point(VECTOR *pw, VECTOR *vw, VECTOR *ww, int limit)
{
	int ret = 0;
	
	/* if the line doesn't cross with the plane, clip the result */
	if (vw->vz - pw->vz >= 0) {
		ww->vx = pw->vx + pw->vz*(vw->vx-pw->vx);
		ww->vy = pw->vy + pw->vz*(vw->vy-pw->vy);
		/*FntPrint("overflow(%d,%d)\n", x, y);*/
	}
	/* if there is a cross point, then calculate */
	else {
		ww->vx = pw->vx - pw->vz*(vw->vx-pw->vx)/(vw->vz-pw->vz);
		ww->vy = pw->vy - pw->vz*(vw->vy-pw->vy)/(vw->vz-pw->vz);

	}
	ww->vz = 0;
	
	/* clip in "limit" value, if clip is operateted, return -1. */
	if (ww->vx < pw->vx-limit) ret = -1, ww->vx = pw->vx-limit;
	if (ww->vx > pw->vx+limit) ret = -1, ww->vx = pw->vx+limit;
	if (ww->vy < pw->vy-limit) ret = -1, ww->vy = pw->vy-limit;
	if (ww->vy > pw->vy+limit) ret = -1, ww->vy = pw->vy+limit;

	/* normal return */
	return(ret);
}

/*
 * translate from the world coordinate to the screen coordinate with
 * perspective translation (32bit format)
 * Note that this function is translated in 32bit world
 : ワールド座標からスクリーン座標変換したのち透視変換を行なう
 * これは、32bit で動作する
 */
/*
 * VECTOR *w:	(input)  position in the world
 * VECTOR *s;	(output) position in the screen
 */
static void rot_trans_pers(VECTOR *w, VECTOR *s)
{
	int	h;
	MATRIX	rot;
	
	/* get current matrix and projection */
	h = ReadGeomScreen();	
	ReadRotMatrix(&rot);
	
	/* rot-trans-pers  with 32bit width */
	ApplyMatrixLV(&rot, w, s);				/* rot */
	applyVector(s, rot.t[0], rot.t[1], rot.t[2], +=);	/* trans */
	s->vx = s->vx*h/s->vz;					/* pers */
	s->vy = s->vy*h/s->vz;
	s->vz = h;
}


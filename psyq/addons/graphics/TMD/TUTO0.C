/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto0
 *			
 :	   TMD viewer prototype (without shading, without texture)
 *	   TMD ビューアプロトタイプ（光源計算なし  F3 型）
 *
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	---------------------------------------------------------	
 *	1.00		Mar,15,1994	suzu
 *	1.10		Jan,22,1996	suzu	(English comment)
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>
#include "rtp.h"

#define SCR_Z		1024	/* projection: 投影面までの距離 */
#define OTSIZE		4096	/* depth of OT: ＯＴの段数（２のべき乗） */
#define MAX_POLY	3000	/* max polygon: 表示ポリゴンの最大値 */

#define MODELADDR	((u_long *)0x80100000)	/* TMD address */

/*
 * Vertex database of triangle primitive 
 : ３角形ポリゴンの頂点情報
 */
typedef struct {
	SVECTOR	n0;		/* normal: 法線 */
	SVECTOR	v0, v1, v2;	/* vertex: 頂点 */
} VERT_F3;

/*
 * Triangle primitive buffer. This buffer should be allocaed dynamically
 * according to total primitive size by malloc()
 : ３角形ポリゴンパケット
 * 頂点・パケットバッファは、malloc() で動的に割り付けるべきです。	
 */
typedef struct {
	int		n;		/* primitive number: ポリゴン数 */
	VERT_F3		vert[MAX_POLY];	/* vertex: 頂点バッファ （１個）*/
	POLY_F3		prim[2][MAX_POLY];/* primitive: パケットバッファ */
} OBJ_F3;

static int pad_read(int nprim);
int loadTMD_F3(u_long *tmd, OBJ_F3 *obj);

main()
{
	static OBJ_F3	obj;			/* object */
	static u_long	otbuf[2][OTSIZE];	/* OT */
	u_long		*ot;			/* current OT */
	int		id = 0;			/* primitive buffer ID */
	VERT_F3		*vp;			/* work */
	POLY_F3		*pp;			/* work */
	int		nprim;			/* work */
	int		i; 			/* work */

	/* initialize: ダブルバッファの初期化 */
	db_init(640, 480, SCR_Z, 60, 120, 120);	
	
	/* read TMD: TMD を読み込む */
	loadTMD_F3(MODELADDR, &obj);		
	
	/* start display: 表示開始 */
	SetDispMask(1);				

	/* main loop: メインループ */
	nprim = obj.n;
	while ((nprim = pad_read(nprim)) != -1) {
		
		/* clip max primitive in [0,max_nprim]
		 :  [0,max_nprim] で nprim をクリップ
		 */
		limitRange(nprim, 0, obj.n);

		/* swap primitive buffer ID: パケットバッファ IDをスワップ */
		id = id? 0: 1;
		ot = otbuf[id];
		
		/* clear OT: ＯＴをクリア */
		ClearOTagR(ot, OTSIZE);			

		/* set primitive vertex: プリミティブの設定 */
		vp = obj.vert;
		pp = obj.prim[id];
		
		/* 3D operation: ３Ｄ表示は、この４行に集約される */
		for (i = 0; i < nprim; i++, vp++, pp++) {
			/* rotTransPers3 is macro. see rtp.h
			 : rotTransPers3 はマクロ。rtp.h を参照のこと
			 */
			pp = &obj.prim[id][i];
			rotTransPers3(ot, OTSIZE, pp,
				      &vp->v0, &vp->v1, &vp->v2);

		}
		
		/* print debug information: デバッグストリングの設定 */
		FntPrint("total=%d\n", i);
		
		/* swap OT and primitive buffer
		 :ダブルバッファのスワップとＯＴ描画
		 */
		db_swap(ot+OTSIZE-1);
	}
	PadStop();
	StopCallback();
	return;
}

static int pad_read(int nprim)
{
	static SVECTOR	ang   = {512, 512, 512};	/* rotate angle */
	static VECTOR	vec   = {0,     0, SCR_Z};	
	static MATRIX	m;				/* matrix */
	static int	scale = ONE/4;
	
	VECTOR	svec;
	int 	padd = PadRead(1);
	
	if (padd & PADselect)	return(-1);
	if (padd & PADLup) 	nprim += 4;
	if (padd & PADLdown)	nprim -= 4;
	if (padd & PADRup)	ang.vx += 32;
	if (padd & PADRdown)	ang.vx -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;
	if (padd & PADL1)	vec.vz += 32;
	if (padd & PADL2) 	vec.vz -= 32;
	if (padd & PADR1)	scale  -= ONE/256;
	if (padd & PADR2)	scale  += ONE/256;

	ang.vz += 8;
	ang.vy += 8;
	
	setVector(&svec, scale, scale, scale);
	RotMatrix(&ang, &m);	
	TransMatrix(&m, &vec);	
	ScaleMatrix(&m, &svec);
	SetRotMatrix(&m);
	SetTransMatrix(&m);

	return(nprim);
}		

/*
 * load TMD: TMD の解析
 */
int loadTMD_F3(u_long *tmd, OBJ_F3 *obj)
{
	VERT_F3		*vert;
	POLY_F3		*prim0, *prim1;
	TMD_PRIM	tmdprim;
	int		col, i, n_prim = 0;

	vert  = obj->vert;
	prim0 = obj->prim[0];
	prim1 = obj->prim[1];
	
	/* open TMD: TMD のオープン */
	if ((n_prim = OpenTMD(tmd, 0)) > MAX_POLY)
		n_prim = MAX_POLY;

	/*
	 * Set unchanged member of primitive here to deliminate main
	 * memory write access
	 : メモリライトアクセスを減らすためにプリミティブバッファのうち、
	 * 書き換えないものをあらかじめ設定しておく
	 */	 
	for (i = 0; i < n_prim && ReadTMD(&tmdprim) != 0; i++) {
		
		/* initialize primitive: プリミティブの初期化 */
		SetPolyF3(prim0);

		/* copy normal and vertex: 法線と頂点ベクトルをコピーする */
		copyVector(&vert->n0, &tmdprim.n0);
		copyVector(&vert->v0, &tmdprim.x0);
		copyVector(&vert->v1, &tmdprim.x1);
		copyVector(&vert->v2, &tmdprim.x2);
		
		col = (tmdprim.n0.vx+tmdprim.n0.vy)*128/ONE/2+128;
		setRGB0(prim0, col, col, col);
		
		/* duplicate primitive for primitive double buffering
		 : ダブルバッファを使用するのでプリミティブの複製をもう
		 * 一組つくっておく
		 */  
		memcpy(prim1, prim0, sizeof(POLY_F3));
		vert++, prim0++, prim1++;
	}
	return(obj->n = i);
}

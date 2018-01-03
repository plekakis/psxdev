/* $PSLibId: Runtime Library Release 3.6$ */
/*			    tuto2
 *			
 :	display many TMD object as PMD (POLY_F3 surface, without lighting)
 *	   TMD-PMD ビューアプロトタイプ（光源計算なし  FT3 型）
 *
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	----------------------------------------------------	
 *	1.00		Mar,15,1994	suzu
 *	2.00		Jul,14,1994	suzu	(using PMD)
 */

#include <sys/types.h>	
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Triangle primitive buffer. This buffer should be allocaed dynamically
 * according to total primitive size by malloc()
 : ３角形ポリゴンパケット
 * 頂点・パケットバッファは、malloc() で動的に割り付けるべきです。	
 */
#define MAX_POLY	512			/* max polygon (per object) */

/* object definition : オブジェクトの定義 */
typedef struct {
	MATRIX		m;	/* local-local matrix: ローカルマトリクス */
	MATRIX		lw;	/* local-world matrix: ローカルマトリクス */
	SVECTOR		v;	/* local vector: ローカルベクトル */
	int		n;	/* number of polygons: ポリゴン数 */
	struct {		/* polygon information: ポリゴンの情報 */
		POLY_FT3	prim[2];
		SVECTOR		v0, v1, v2;
	} p[MAX_POLY];
} OBJ_FT3;

#define SCR_Z		1024		/* projection: 投影面までの距離 */
#define OTLENGTH	12		/* depth of OT: ＯＴの段数 */
#define OTSIZE		(1<<OTLENGTH)	/* depth of OT: ＯＴの段数 */
#define MAX_OBJ		16		/* max object: オブジェクトの個数 */

#define MODELADDR	((u_long *)0x80120000)	/* TMD address */
#define TEXADDR		((u_long *)0x80140000)	/* TIM address */

static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m);
static void set_position(OBJ_FT3 *obj, int n);
static void add_OBJ_FT3(MATRIX *ws, u_long *ot, OBJ_FT3 *obj, int id);
static void loadTIM(u_long *addr);
static void setSTP(u_long *col, int n);
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj);

main()
{
	static SVECTOR	ang = {0,0,0};	/* self-rotation angle; 自転角 */
	MATRIX	rot;			/* self-rotation angle: 自転角 */
	MATRIX	ws;			/* world matrix:ワールドマトリクス */
	OBJ_FT3	obj[MAX_OBJ];		/* object:オブジェクト */
	u_long	ot[2][OTSIZE];		/* OT: ＯＴ バッファ*/
	int	nobj = 1;		/* number of obj: オブジェクトの個数 */
	int	id   = 0;		/* buffer ID: パケット ID */
	int	i, n; 			/* work */
	
	/* initialize frame double buffer: ダブルバッファの初期化 */
	db_init(640, 480/*240*/, SCR_Z, 60, 120, 120);	

	/* load TIM to frame buffer: TIM をフレームバッファに転送する */
	loadTIM(TEXADDR);	

	/* read each TMD: TMD をばらばらにして読み込む */
	for (i = 0; i < MAX_OBJ; i++) 
		loadTMD_FT3(MODELADDR, &obj[i]);	
	
	/* layout each object: オブジェクト位置をレイアウト */
	set_position(obj, 0);		
	
	/* start display;表示開始 */
	SetDispMask(1);			
	
	/* main loop: メインループ */
	while ((nobj = pad_read(obj, nobj, &ws)) != -1) {
		
		/* swap primitive buffer ID:パケットバッファ IDをスワップ */
		id = id? 0: 1;
		
		/* clear OT:ＯＴをクリア */
		ClearOTagR(ot[id], OTSIZE);			

		/* rotate matrix of the earth: 地球を自転させる */
		ang.vy += 32;
		RotMatrix(&ang, &rot);
		
		/* set primitives: プリミティブの設定 */
		for (i = 0; i < nobj; i++) {
			MulMatrix0(&obj[i].m, &rot, &obj[i].lw);
			add_OBJ_FT3(&ws, ot[id], &obj[i], id);
		}
		
		/* print debug information: デバッグストリングの設定 */
		FntPrint("polygon=%d\n", obj[0].n);
		FntPrint("objects=%d\n", nobj);
		FntPrint("total  =%d\n", obj[0].n*nobj);
		
		/* draw OT and swap frame double buffer
		 : ダブルバッファのスワップとＯＴ描画
		 */
		db_swap(&ot[id][OTSIZE-1]);
	}
	PadStop();
	StopCallback();
}

/*
 * Read controler and set world-screen matrix
 : パッドの読み込み
 * 回転の中心位置と回転角度を読み込みワールドスクリーンマトリクスを設定
 * 	
 */	
static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m)
{
	static SVECTOR	ang   = {0, 0, 0};
	static VECTOR	vec   = {0, 0, SCR_Z*3};
	static int	scale = ONE;
	static int	opadd = 0;
	
	VECTOR	svec;
	int 	padd = PadRead(1);
	
	if (padd & PADk)	return(-1);
	if (padd & PADRup)	ang.vx += 8;
	if (padd & PADRdown)	ang.vx -= 8;
	if (padd & PADRleft) 	ang.vy += 8;
	if (padd & PADRright)	ang.vy -= 8;
	if (padd & PADL1)	vec.vz += 8;
	if (padd & PADL2) 	vec.vz -= 8;
	
	if ((opadd==0) && (padd & PADLup))	set_position(obj, nobj++);
	if ((opadd==0) && (padd & PADLdown))	nobj--;
	
	limitRange(nobj, 1, MAX_OBJ);
	opadd = padd;
	
	setVector(&svec, scale, scale, scale);
	RotMatrix(&ang, m);	
	
#ifdef ROTATE_YORSELF	/* rotate your camera: 主観視（自分が動く) */
	{
		VECTOR	vec2;
		ApplyMatrixLV(m, &vec, &vec2);
		TransMatrix(m, &vec2);
		dumpVector("vec2=", &vec2);
	}
#else			/* rotate the world: 客観視（世界が動く) */
	TransMatrix(m, &vec);	
#endif
	/* set world screen matrix with aspecto ratio correction
	 : ワールドスクリーンマトリクスをアスペクト比補正をした後に設定
	 */
	db_set_matrix(m);
	/*
	SetRotMatrix(m);
	SetTransMatrix(m);
	*/
	return(nobj);
}


/*
 * layout many object in the world-coordinate.
 * Since the position of each object is determined by rand(),
 * two objects may be located at the same position.
 : オブジェクトをワールド座標系にレイアウト
 * ここでは乱数を使用して適当に配置している	
 * そのため、二つのオブジェクトが同じ位置にぶつかることもある。	
 */	
#define UNIT	400		/* resolution: 最小解像度 */

static void set_position(OBJ_FT3 *obj, int n)
{
	SVECTOR	ang;

	static loc_tab[][3] = {
		 0, 0, 0,
		 1, 0, 0,	0, 1, 0,	 0, 0, 1,
		-1, 0, 0,	0,-1, 0,	 0, 0,-1,
		 1, 1, 0,	0, 1, 1,	 1, 0, 1,
		-1,-1, 0,	0,-1,-1,	-1, 0,-1,
		 1,-1, 0,	0,-1, 1,	-1, 0, 1,
		-1, 1, 0,	0, 1,-1,	 1, 0,-1,
	};
	
	/* set axis of each earth: 各地球の地軸を設定 */
	ang.vx = rand()%4096;
	ang.vy = rand()%4096;
	ang.vz = rand()%4096;
	RotMatrix(&ang, &obj[n].m);	
	
	/* set position of each earth: 各地球の場所を設定 */
	obj[n].lw.t[0] = loc_tab[n][0]*UNIT;
	obj[n].lw.t[1] = loc_tab[n][1]*UNIT;
	obj[n].lw.t[2] = loc_tab[n][2]*UNIT;
}

/*
 * append object to OT: オブジェクトの登録
 */
static void add_OBJ_FT3(MATRIX *ws, u_long *ot, OBJ_FT3 *obj, int id)
{
	MATRIX	ls;		/* local-screen matrix */
	
	/* push current matrix: カレントマトリクスを退避 */
	PushMatrix();				

	/* make local-screen coordinate
	 :  ローカルスクリーンマトリクスを作る
	 */
	CompMatrix(ws, &obj->lw, &ls);

	/* set matrix: ローカルスクリーンマトリクスを設定 */
	SetRotMatrix(&ls);		/* set matrix */
	SetTransMatrix(&ls);		/* set vector */
	
	/* rotate-translate-perspective translation:回転・移動・透視変換 */
	RotPMD_FT3((long *)&obj->n, ot, OTLENGTH, id, 0);

	/* recover old matrix: マトリクスを元にもどしてリターン */
	PopMatrix();
}

/*
 * Load TIM data from main memory to the frame buffer
 : TIM をロードする
 */	
static void loadTIM(u_long *addr)
{
	TIM_IMAGE	image;		/* TIM header */
	
	OpenTIM(addr);			/* open TIM */
	while (ReadTIM(&image)) {
		if (image.caddr) {	/* load CLUT (if needed) */
			setSTP(image.caddr, image.crect->w);
			LoadImage(image.crect, image.caddr);
		}
		if (image.paddr) 	/* load texture pattern */
			LoadImage(image.prect, image.paddr);
	}
}
	
/*
 * change STP on texture pattern to inhibit transparent color
 : 透明色処理を禁止するために STP bit を 1 にする
 */	
static void setSTP(u_long *col, int n)
{
	n /= 2;  
	while (n--) 
		*col++ |= 0x80008000;
}

/*
 * parse TMD: TMD の解析
 */
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj)
{
	TMD_PRIM	tmdprim;
	int		col, i, n_prim = 0;

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
		SetPolyFT3(&obj->p[i].prim[0]);

		/* copy normal and vertex: 法線と頂点ベクトルをコピーする */
		copyVector(&obj->p[i].v0, &tmdprim.x0);
		copyVector(&obj->p[i].v1, &tmdprim.x1);
		copyVector(&obj->p[i].v2, &tmdprim.x2);
		
		/* lighting: 光源計算（最初の一回のみ）*/
		col = (tmdprim.n0.vx+tmdprim.n0.vy)*128/ONE/2+128;
		setRGB0(&obj->p[i].prim[0], col, col, col);
		
		/* copy texture point because this point never changes
		 * by rotation
		 : テクスチャ座標は変わらないのでここでコピーしておく
		 */
		setUV3(&obj->p[i].prim[0], 
		       tmdprim.u0, tmdprim.v0,
		       tmdprim.u1, tmdprim.v1,
		       tmdprim.u2, tmdprim.v2);
		
		/* copy tpage and clut
		 : テクスチャページ／テクスチャ CLUT ID をコピー
		 */
		obj->p[i].prim[0].tpage = tmdprim.tpage;
		obj->p[i].prim[0].clut  = tmdprim.clut;

		/* duplicate primitive for primitive double buffering
		 : ダブルバッファを使用するのでプリミティブの複製をもう
		 * 一組つくっておく
		 */  
		memcpy(&obj->p[i].prim[1],
		       &obj->p[i].prim[0], sizeof(POLY_FT3));
		
	}
	return(obj->n = i);
}

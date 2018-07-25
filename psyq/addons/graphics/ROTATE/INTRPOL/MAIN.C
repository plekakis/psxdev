/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *	intrpol : various kinds of interpolating about rotation
 *
 *	"main.c" main routine
 *
 *		Copyright (C) 1994,1995,1996  Sony Computer Entertainment
 *		All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

#define PACKETMAX 2400	
#define PACKETMAX2 (PACKETMAX*24) 
#define MODEL_ADDR 0x80040000
#define OT_LENGTH  8

#define PNUM	6		/* num of faces (per a cube) : 面の数 */
#define CUBESIZE	80	/* length of cube edge :辺のながさ */
#define NINTP	100		/* resolution of interpolating : 内挿の解像度 */
#define VNUM 8			/* num of vertex (per a cube) : 頂点の数 */

#define Printf(x) FntPrint x
int kstream[3];

#define CUBENUM 5		/* num of cubes 立方体の数 */

GsOT            Wot[2];
GsOT_TAG	zsorttable[2][1<<OT_LENGTH];

extern MATRIX GsIDMATRIX;	

GsRVIEW2  view;		
u_long padd;			/* to keep pad data */
static ite= -1;			/* interpolation parameter : 内挿番号 */
static int presetnum=0;		/* preseted angle set number */

PACKET out_packet[2][PACKETMAX2];


typedef struct {
    POLY_F4 surf[PNUM][2];	/* surfaces polygons : Cube 表面の正方形x6個 */
    GsCOORDINATE2 coord2;	/* coordinates  : Cube の座標系 */
    SVECTOR sv;			/* Euler Angle : Cube の回転角(使わないこともある) */
} PBUF;				/* struct of cube */

static PBUF pb[CUBENUM];

/* preseted angle set : Cube のプリセットオイラー角 */
#define PRESETNUM 5
SVECTOR initrvect[PRESETNUM][2]={
    {
	{ 3948,-4549,2162,0 },
	{ 5687, 2858, -1834,0},
    },
    {
	{ 50, 50, 50, 0 },
	{ 2098, 2098, 2098, 0},
    },
    {
	{ 0, 0, 0, 0 },
	{ 123, 456, 789, 0},
    },
    {
	{ 1074, 1074, 0, 0 },
	{ -974, -974, 0, 0},
    },
    {
	{ 1074, 1074, 0, 0 },
	{ -840, -457, 141, 0},
    },
};


/* vertex of surfaces of cubes : Cube の表面正方形の頂点 */
SVECTOR vert[PNUM][4]=
{
    {{-CUBESIZE,-CUBESIZE,-CUBESIZE,0}, { CUBESIZE,-CUBESIZE,-CUBESIZE,0},
	 {-CUBESIZE, CUBESIZE,-CUBESIZE,0}, { CUBESIZE, CUBESIZE,-CUBESIZE,0}},
    {{ CUBESIZE,-CUBESIZE,-CUBESIZE,0}, { CUBESIZE,-CUBESIZE, CUBESIZE,0},
	 { CUBESIZE, CUBESIZE,-CUBESIZE,0}, { CUBESIZE, CUBESIZE, CUBESIZE,0}},
    {{ CUBESIZE,-CUBESIZE, CUBESIZE,0}, {-CUBESIZE,-CUBESIZE, CUBESIZE,0},
	 { CUBESIZE, CUBESIZE, CUBESIZE,0}, {-CUBESIZE, CUBESIZE, CUBESIZE,0}},
    {{-CUBESIZE,-CUBESIZE, CUBESIZE,0}, {-CUBESIZE,-CUBESIZE,-CUBESIZE,0},
	 {-CUBESIZE, CUBESIZE, CUBESIZE,0}, {-CUBESIZE, CUBESIZE,-CUBESIZE,0}},
    {{-CUBESIZE,-CUBESIZE, CUBESIZE,0}, { CUBESIZE,-CUBESIZE, CUBESIZE,0},
	 {-CUBESIZE,-CUBESIZE,-CUBESIZE,0}, { CUBESIZE,-CUBESIZE,-CUBESIZE,0}},
    {{-CUBESIZE, CUBESIZE,-CUBESIZE,0}, { CUBESIZE, CUBESIZE,-CUBESIZE,0},
	 {-CUBESIZE, CUBESIZE, CUBESIZE,0}, { CUBESIZE, CUBESIZE, CUBESIZE,0}}
};

/* spatial position of cubes : Cube の空間配置 */
#define XDISTANCE 460
#define YDISTANCE 300
int position[CUBENUM][3]={
    {-XDISTANCE,0,0},
    {XDISTANCE,0,0},
    {0,-YDISTANCE,0},
    {0,0,0},
    {0,YDISTANCE,0},
};

/* locus line of interpolating : 内挿の軌跡表示線 */
LINE_F2 locus[CUBENUM-2][VNUM][NINTP+1][2];


/* vertex for locus : 軌跡用頂点 */
SVECTOR locus_vtx[VNUM]=
{
    {-CUBESIZE,-CUBESIZE,-CUBESIZE,0},
    {-CUBESIZE,-CUBESIZE,CUBESIZE,0},
    {-CUBESIZE,CUBESIZE,-CUBESIZE,0},
    {-CUBESIZE,CUBESIZE,CUBESIZE,0},
    {CUBESIZE,-CUBESIZE,-CUBESIZE,0},
    {CUBESIZE,-CUBESIZE,CUBESIZE,0},
    {CUBESIZE,CUBESIZE,-CUBESIZE,0},
    {CUBESIZE,CUBESIZE,CUBESIZE,0},
};



/************* MAIN START ******************************************/
main()
{
    int     i,j;
    int     outbuf_idx;
    MATRIX  tmpls;
    long    p,otz,flag,nclip;
    int     ret=0;
    int     m;
    DVECTOR dv;

    ResetCallback();
    PadInit(0);			/* Init Pad controller : コントローラ初期化 */
    init_all();			/* initialize */

    /* Initialize primitives for Cube */
    init_cubes();

    /* Initialize primitives for locus */
    init_locus_line();

    /* main loop */
    while (ret==0){
	disp_status();
	ret=obj_interactive();
	GsSetRefView2(&view);
	outbuf_idx=GsGetActiveBuff();
	GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);
	GsClearOt(0,0,&Wot[outbuf_idx]); 

	for (m=0; m<CUBENUM; m++){
	    GsGetLs(&pb[m].coord2,&tmpls);
	    GsSetLightMatrix(&tmpls);
	    GsSetLsMatrix(&tmpls);

	    /* draw cube surfaces: Cube 描画 */
	    for(i=0;i<PNUM;i++){
		nclip=RotAverageNclip4(
				   &vert[i][0],&vert[i][1],&vert[i][2],&vert[i][3],
				   (long *)&pb[m].surf[i][outbuf_idx].x0,
				   (long *)&pb[m].surf[i][outbuf_idx].x1,
				   (long *)&pb[m].surf[i][outbuf_idx].x2,
				   (long *)&pb[m].surf[i][outbuf_idx].x3,
				       &p,&otz,&flag);

		if(nclip>0&&flag>0){
		    otz >>= (14-OT_LENGTH);
		    AddPrim(Wot[outbuf_idx].org+otz,
			    &pb[m].surf[i][outbuf_idx]);
		}

	    }

	    if (m>1){		/* cubes for interpolating : 内挿する Cube */
		for (i=0; i<VNUM; i++){
		    /* get locus point : 頂点軌跡計算 */
		    if (ite>=0){
			RotTransPers(&locus_vtx[i], (long *)(&dv.vx), &p, &flag);
			/*for both double buffers : ダブルバッファの両方に登録*/
			make_locus_line(&dv, &locus[m-2][i][ite][0]);
			make_locus_line(&dv, &locus[m-2][i][ite][1]);
		    }
		    /* draw locus: 軌跡描画 */
		    for (j=0; j<ite; j++){
			AddPrim(Wot[outbuf_idx].org,
				&locus[m-2][i][j][outbuf_idx]);
		    }
		}
	    }
	}

	padd=PadRead(0);	
	DrawSync(0);
	VSync(0);	

	GsSwapDispBuff();	
	GsSortClear(0x0,0x0,0x0,&Wot[outbuf_idx]);
	GsDrawOt(&Wot[outbuf_idx]);

	FntFlush(kstream[0]);
	FntFlush(kstream[1]);
    }
    PadStop();
    ResetGraph(3);
    StopCallback();
    return 0;
}


/***********************************************
 *	pad control and interpolation : パッド操作と内挿
 ***********************************************/
#define ROTATE_STEP 47
obj_interactive()
{
    int m;
    SVECTOR *p;
    static int flag=0;				/* 1 when interpolating */
    static u_long oldpadd=0;
    static int presetp=1;

    if(flag==0){
	if (padd & (PADLleft|PADLright|PADLup|PADLdown|PADL1|PADL2)){
	    /* rotate Cube-A(left) : 開始Cube回転 */
	    p= &pb[0].sv;
	    if (padd & PADLleft) 	p->vy+=ROTATE_STEP;
	    if (padd & PADLright) 	p->vy-=ROTATE_STEP;
	    if (padd & PADLup) 		p->vx+=ROTATE_STEP;
	    if (padd & PADLdown) 	p->vx-=ROTATE_STEP;
	    if (padd & PADL1) 		p->vz+=ROTATE_STEP;
	    if (padd & PADL2)	 	p->vz-=ROTATE_STEP;
	    marume(p);
	    set_coordinate(&pb[0].sv, &pb[0].coord2);
	    presetp=0;
	}
	if (padd & (PADRleft|PADRright|PADRup|PADRdown|PADR1|PADR2)){
	    /* rotate Cube-B(right) : 終わりCube回転 */
	    p= &pb[1].sv;
	    if (padd & PADRleft) 	p->vy+=ROTATE_STEP;
	    if (padd & PADRright) 	p->vy-=ROTATE_STEP;
	    if (padd & PADRup) 		p->vx+=ROTATE_STEP;
	    if (padd & PADRdown) 	p->vx-=ROTATE_STEP;
	    if (padd & PADR1) 		p->vz+=ROTATE_STEP;
	    if (padd & PADR2)	 	p->vz-=ROTATE_STEP;
	    marume(p);
	    set_coordinate(&pb[1].sv, &pb[1].coord2);
	    presetp=0;
	}

  	/* finish : プログラムを終了してモニタに戻る */
  	if ((padd & PADselect) && (padd & PADstart)) return(1);

	/* get initial status again : 最初の状態へ戻る */
	if (padd & PADselect & ~oldpadd){
	    init_cubes();
	    presetp=1;
	    ite= -1;
	}
	/* start interpolating : 内挿スタート */
  	if ((padd & PADstart)>0){
	    ite= -30;
	    flag=1;
	    euler_interpolate_start();
	    matrix_interpolate_start();
	    axis_interpolate_start();
	    /* move interpolating cubes into start position : 内挿開始位置へ */
	    for (m=2; m<CUBENUM; m++){
		pb[m].sv = pb[0].sv;
		set_coordinate(&pb[m].sv, &pb[m].coord2);
	    }
	}
	
	Printf((kstream[0],"Push START to interpolate\n"));
    } else{
	/* interpolation */
	if (ite>0){
	    /* euler angle interpolation */
	    euler_interpolate(2, ite, NINTP);
	    /* matrix elements interpolation */
	    matrix_interpolate_normal(3, ite, NINTP);
	    /* rotation axis interpolation */
	    axis_interpolate(4, ite, NINTP);
	}
	if (ite==NINTP){	/* interpolation finished */
	    flag=0;
	} else{
	    ite++;
	}
    }
    oldpadd=padd;
    if (presetp){
	Printf((kstream[1],"Preset Angle #%d\n",presetnum));
	Printf((kstream[1],"Push SELECT to the next"));
    }

    return(0);
}

/***********************************************
 *	initializes routines : 初期化など
 ***********************************************/
init_all()
{
    ResetGraph(0);		/* reset GPU : GPUリセット */
    padd=0;			/* init controller value:コントローラ値初期化 */
    SetGraphDebug(1);
  
    GsInitGraph(640,240,GsOFSGPU|GsINTER,0,0);
    GsDefDispBuff(0,0,0,240);

    GsInit3D();			/* init 3d part: ３Dシステム初期化 */
  
    Wot[0].length=OT_LENGTH;	/* set the length of Wot : Wot の解像度設定 */
    Wot[0].org=zsorttable[0];	/* set the top address of Wot tags : */
				/* Wot にオーダリングテーブルの実体設定 */
    /* set another Wot for double buffer :*/
    /* ダブルバッファのためもう一方にも同じ設定 */
    Wot[1].length=OT_LENGTH;
    Wot[1].org=zsorttable[1];
  
    view_init();		/* set the viewpoint : 視点設定 */
    light_init();		/* set the lights : 光源設定 */
  
    /* for KanjiFntPrint() */
    FntLoad(960,256);

    kstream[0]=FntOpen(-320,-100, 300,200, 0, 200);
    kstream[1]=FntOpen(  100,-100, 300,200, 0, 200);
    GsInitVcount();  

}

view_init()			/* set the viewpoint : 視点設定 */
{
    /*---- Set projection,view ----*/
    GsSetProjection(1000);	/* set the projection : プロジェクション設定 */
  
    /* set the viewpoint parameter : 視点パラメータ設定 */
    view.vpx = 0; view.vpy = 0; view.vpz = -2000;
    /* set the refarence point parameter : 注視点パラメータ設定 */  
    view.vrx = 0; view.vry = 0; view.vrz = 0;
    /* set the roll pameter of viewpoint : 視点の捻りパラメータ設定 */
    view.rz=0;
    /* set the view coordinate : 視点座標パラメータ設定 */
    view.super = WORLD;
  
    /*set the view point from parameters: */
    /* 視点パラメータを設定する */
    GsSetRefView2(&view);
    GsSetNearClip(100);		/* set near clip: ニアクリップ設定 */
}


light_init()			/* init lights : 光源設定 */
{
    GsF_LIGHT pslt[3];				/* light data */

    /* Setting direction vector of Light0 : */
    /* 平行光源 #0 方向パラメータ設定 */
    pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
    /* Setting color of Light0 :*/
    /* 平行光源 #0 色パラメータ設定 */
    pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
    /* Set Light0 from parameters :*/
    /* 光源パラメータ #0 から #0 光源設定 */
    GsSetFlatLight(0,&pslt[0]);

    /* Setting  of Light1 : */
    /* ライトID1 設定 */
    pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
    pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
    GsSetFlatLight(1,&pslt[1]);
  
    /* Setting  of Light2 : */
    /* ライトID2 設定 */
    pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
    pslt[2].r=0x60; pslt[2].g=0x60; pslt[2].b=0x60;
    GsSetFlatLight(2,&pslt[2]);
  
    /* setting ambient : アンビエント設定 */
    GsSetAmbient(0,0,0);

    /* setting default light mode : 光源計算のデフォルトの方式設定 */
    GsSetLightMode(0);
}


/* initialize of cubes */
init_cubes()
{
    int m;

    for (m=0; m<CUBENUM; m++){
	if (m<2) init_prim(&pb[m], &initrvect[presetnum][m], &position[m]);
	else init_prim(&pb[m], &initrvect[presetnum][0], &position[m]);
    }

    presetnum++;
    presetnum%=PRESETNUM;
}

/* initialize of surface polygons : Cube の初期化*/
init_prim(pb, iv, pos)			
  PBUF      *pb;
  SVECTOR *iv;
  int pos[];
{
    long    i,j;

    for (i=0;i<PNUM;i++){
	SetPolyF4(&pb->surf[i][0]);
	SetPolyF4(&pb->surf[i][1]);
    }
    GsInitCoordinate2(WORLD,&pb->coord2);
    pb->coord2.coord.t[0] = 0;
    pb->coord2.coord.t[1] = 0;
    pb->coord2.coord.t[2] = 0;
    pb->sv= *iv;

    marume(&pb->sv);
    set_coordinate(&pb->sv, &pb->coord2);

    for (i=0; i<3; i++){
	pb->coord2.coord.t[i]= pos[i];
    }

    for(i=0;i<PNUM;i++){
	for (j=0; j<2; j++){
	    pb->surf[i][j].r0=((6-i)&1)*60 + 70;
	    pb->surf[i][j].g0=((6-i)&2)*30 + 70;
	    pb->surf[i][j].b0=((6-i)&4)*15 + 70;
	}

    }
}

/***********************************************
 *	euler angle => rotation matrix : オイラー角→回転マトリクス
 ***********************************************/
set_coordinate(pos,coor)        /* マトリックス計算ワークからマトリックス
                                   を作成し座標系にセットする */
  SVECTOR *pos;			/* work vector : ローテションベクタ */
  GsCOORDINATE2 *coor;		/* Coordinate : 座標系 */             
{
    MATRIX tmp1;

    /* set translation : 平行移動をセットする */
    tmp1.t[0] = coor->coord.t[0];
    tmp1.t[1] = coor->coord.t[1];
    tmp1.t[2] = coor->coord.t[2];

    /* Rotate Matrix : ローテーションベクタから回転マトリックスを作成する */
    RotMatrix(pos,&tmp1);

  /* Set Matrix to Coordinate : 求めたマトリックスを座標系にセットする */
    coor->coord = tmp1;
    /* Clear flag becase of changing parameter : マトリックスキャッシュをフラッシュする */
    coor->flg = 0;
}

/***********************************************
 *	dump matrix : マトリクスのダンプ
 ***********************************************/
prmatrix(m)
  MATRIX *m;
{
    printf("mat=[%8d,%8d,%8d]\n",m->m[0][0],m->m[0][1],m->m[0][2]);
    printf("    [%8d,%8d,%8d]\n",m->m[1][0],m->m[1][1],m->m[1][2]);
    printf("    [%8d,%8d,%8d]\n",m->m[2][0],m->m[2][1],m->m[2][2]);
    printf("    [%8d,%8d,%8d]\n\n",m->t[0],m->t[1],m->t[2]);
}



/***********************************************
 *	display euler angle and rotation matrix : 回転角と回転マトリクスの表示
 ***********************************************/
disp_status()
{
    int i,j;

    Printf((kstream[0], "Rotation Vector:\n"));
    Printf((kstream[0],
	     "<%5d,%5d,%5d>\n",
	     pb[0].sv.vx, pb[0].sv.vy, pb[0].sv.vz));
    Printf((kstream[1], "\n"));
    Printf((kstream[1],
	     "<%5d,%5d,%5d>\n",
	     pb[1].sv.vx, pb[1].sv.vy, pb[1].sv.vz));
    Printf((kstream[0], "\nRotation Matrix:\n"));
    Printf((kstream[1],"\n\n"));

    for (i=0; i<3; i++){
	for (j=0; j<3; j++){
	    Printf((kstream[0],"%5d,",pb[0].coord2.coord.m[i][j]));
	    Printf((kstream[1],"%5d,",pb[1].coord2.coord.m[i][j]));
	}
	Printf((kstream[0],"\n"));
	Printf((kstream[1],"\n"));
    }
    Printf((kstream[0],"\n\n\n\n\n\n\n\n\n\n"));
    Printf((kstream[1],"\n\n\n\n\n\n\n\n\n\n"));
}
/***********************************************
 *	angle normalization : オイラー角の正規化
 ***********************************************/
marume(SVECTOR *sv)
{
#define MARUMEONE ((long)4096*2)
    sv->vx=((sv->vx+MARUMEONE)%4096);
    if (sv->vx>2048) sv->vx-=4096;
    sv->vy=((sv->vy+MARUMEONE)%4096);
    if (sv->vy>2048) sv->vy-=4096;
    sv->vz=((sv->vz+MARUMEONE)%4096);
    if (sv->vz>2048) sv->vz-=4096;
}

/***********************************************
 *	locus : 軌跡
 ***********************************************/
CVECTOR locuscolor[CUBENUM-2]={
    {255,255,128,0},
    {255,128,255,0},
    {128,255,255,0}
};

init_locus_line()
{
    int i,j,k,m;

    for (m=0; m<CUBENUM-2; m++){
	for (i=0; i<VNUM; i++){
	    for (j=0; j<NINTP+1; j++){
		for (k=0; k<2; k++){
		    SetLineF2(&locus[m][i][j][k]);
		    setRGB0(&locus[m][i][j][k],
			    locuscolor[m].r,
			    locuscolor[m].g,
			    locuscolor[m].b
				 );
		}
	    }
	}
    }
}

make_locus_line(DVECTOR *p, LINE_F2 *l)
{
    setXY2(l,  p->vx, p->vy,  1+p->vx, p->vy);	/* 点で表示 */
}

/***********************************************
 *     interpolate about Enler's angles : オイラー角内挿
 ***********************************************/
static SVECTOR dsvector;

euler_interpolate_start()
{
    dsvector.vx=pb[1].sv.vx-pb[0].sv.vx;
    dsvector.vy=pb[1].sv.vy-pb[0].sv.vy;
    dsvector.vz=pb[1].sv.vz-pb[0].sv.vz;
    marume(&dsvector);
}

euler_interpolate(int m, int ite, int nite)
{
    pb[m].sv.vx = pb[0].sv.vx + (dsvector.vx*ite/nite);
    pb[m].sv.vy = pb[0].sv.vy + (dsvector.vy*ite/nite);
    pb[m].sv.vz = pb[0].sv.vz + (dsvector.vz*ite/nite);
    set_coordinate(&pb[m].sv, &pb[m].coord2);
}

/***********************************************
 *	interolate about rotation matrix elements (with normalization):
 * 	回転マトリクス直接内挿(正規化つき)
 ***********************************************/
static MATRIX dmatrix;
matrix_interpolate_start()
{
    int i,j;

    for (i=0; i<3; i++){
	for (j=0; j<3; j++){
	    dmatrix.m[i][j]
		=pb[1].coord2.coord.m[i][j]-pb[0].coord2.coord.m[i][j];
	}
    }
}

matrix_interpolate_normal(int m, int ite, int nite)
{
    int i,j;
    MATRIX tmpm;
    VECTOR  vec[3];
    VECTOR op, tmpv;
    long len, max;
    int jiku;

    for (i=0; i<3; i++){
	vec[i].vx= pb[0].coord2.coord.m[i][0] + ((dmatrix.m[i][0]*ite)/nite);
	vec[i].vy= pb[0].coord2.coord.m[i][1] + ((dmatrix.m[i][1]*ite)/nite);
	vec[i].vz= pb[0].coord2.coord.m[i][2] + ((dmatrix.m[i][2]*ite)/nite);
    }

    max= -1;
    OuterProduct12(vec, vec+1, &op);
    len=VectorNormal(&op, &tmpv);
    if (len>max){
	max=len;
	jiku=2;
    }
    OuterProduct12(vec+1, vec+2, &op);
    len=VectorNormal(&op, &tmpv);
    if (len>max){
	max=len;
	jiku=0;
    }
    OuterProduct12(vec, vec+2, &op);
    len=VectorNormal(&op, &tmpv);
    if (len>max){
	jiku=1;
    }

#define v2m(v,m) ((m)[0]=(v)->vx,(m)[1]=(v)->vy,(m)[2]=(v)->vz)

    switch (jiku){
      case 0:
	v2m(&vec[1], &tmpm.m[1][0]);
	v2m(&vec[2], &tmpm.m[2][0]);
	MatrixNormal_1(&tmpm, &pb[m].coord2.coord);
	break;
      case 1:
	v2m(&vec[0], &tmpm.m[0][0]);
	v2m(&vec[2], &tmpm.m[2][0]);
	MatrixNormal_2(&tmpm, &pb[m].coord2.coord);
	break;
      case 2:
	v2m(&vec[0], &tmpm.m[0][0]);
	v2m(&vec[1], &tmpm.m[1][0]);
	MatrixNormal_0(&tmpm, &pb[m].coord2.coord);
	break;
    }

    pb[m].coord2.flg=0;

}

#if 0
/***********************************************
 *	interolate about rotation matrix elements (without normalization):
 * 	回転マトリクス直接内挿(正規化なし)
 ***********************************************/
matrix_interpolate(int m, int ite, int nite)
{
    int i,j;

    for (i=0; i<3; i++){
	for (j=0; j<3; j++){
	    pb[m].coord2.coord.m[i][j]=
		pb[0].coord2.coord.m[i][j] + ((dmatrix.m[i][j]*ite)/nite);
	}
    }

    pb[m].coord2.flg=0;

}
#endif /* 0 */

/***********************************************
 *	interpolate around the rotation axis: 回転軸を使った回転内挿
 ***********************************************/
MATRIX eig, ieig, rot;
short theta;

axis_interpolate_start()
{
    MATRIX tmpm, r1;
    long tan;

    TransposeMatrix(&pb[0].coord2.coord, &tmpm);
    MulMatrix(&tmpm, &pb[1].coord2.coord);
    if(IsIdMatrix(&tmpm)==1){
	printf("Id Matrix!\n");
	theta=0;
	return;
    }

    EigenMatrix(&tmpm,&eig);
    TransposeMatrix(&eig,&ieig);
    MulMatrix0(&ieig,&tmpm,&r1);
    MulMatrix0(&r1,&eig,&rot);
    tan = rot.m[1][2]*4096/rot.m[1][1];
    theta = catan(tan);
    if(rot.m[1][1]<0){
	if(rot.m[1][2]>=0){
	    theta +=2048;
	}else{
	    theta -=2048;
	}
    }
}

axis_interpolate(int m, int ite, int nite)
{
    MATRIX r1, tmpm;
    short delta;

    if (theta){
	delta=(theta*ite)/nite;
	rot.m[1][1]=rcos(delta);
	rot.m[1][2]=rsin(delta);
	rot.m[2][1]= -rot.m[1][2];
	rot.m[2][2]=rot.m[1][1];
	MulMatrix0(&eig,&rot,&r1);
	MulMatrix0(&r1,&ieig,&tmpm);
	MulMatrix0(&pb[0].coord2.coord, &tmpm, &pb[m].coord2.coord);

	pb[m].coord2.flg=0;
    }
}


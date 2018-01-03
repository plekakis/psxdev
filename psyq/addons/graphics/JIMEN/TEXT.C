/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *
 *	---- General Scan Conversion Library ----
 *	
 *
 *		1996,1,26	by Oka
 *		Copyright (C) 1996 by Sony Computer Entertainment
 *			All rights Reserved
 */

#include	<sys/types.h>
#include 	<libetc.h>
#include	<libgte.h>
#include	<libgpu.h>
#include	<libgs.h>
#include	<inline_c.h>
#include 	<gtemac.h>


#define INTERLACE
/*
#define DRAWNORMAL
#define DRAWSECT
*/

#define NEARZ	(512/16)
#define MAXTSIZE	256
#define TXSIZE	256

#define SCR_Z	512
#define OTSIZE	256

				/* テクスチャ（パターン）情報 */
#define TEX_ADDR 0x801a0000     /* テクスチャ先頭アドレス */
#define TIM_HEADER 0x00000010   /* TIMヘッダの値 */
GsIMAGE TimInfo;

#ifdef INTERLACE
#define PIV	480
#define PIH	640
#define FSTART	2
#else
#define PIV	240
#define PIH	320
#define FSTART	1
#endif

#define MAXPAC	(PIV*2)


/* Vertex should be rectangle */
static SVECTOR    Vertex[4]={{-2000,0, 2000,0},
                             { 2000,0, 2000,0},
                             { 2000,0,-2000,0},
                             {-2000,0,-2000,0}};


/*
 *  その他...
 */
u_long PadData;			/* コントロールパッドデータ */

typedef struct {
        DRAWENV         draw;           /* drawing environment: 描画環境 */
        DISPENV         disp;           /* display environment: 表示環境 */
        u_long          ot[OTSIZE];     /* OT: オーダリングテーブル */
        POLY_FT4        s[MAXPAC];  	/* softwear scan conv. packet area */
        POLY_FT3        ss[PIV];  	/* softwear scan conv. packet area */
	LINE_F2		normal;		/*normal vector*/
	LINE_F2		sect;		/*intersection vector*/
	POLY_FT4        divs[32*32];    /* divide polygon mode packet area */
} DB;

typedef struct {
	int	istart_x;
	int	iend_x;
	long	fstart_x;
	long	fend_x;
	int	SVn[4];
	int	EVn[4];
	long	SVa[4];
	long	EVa[4];
	long	SVf[4];
	long	EVf[4];
        short   uv[4][2];
	SVECTOR	sv0,sv1;
	long	old_start_x,old_end_x;
	long	old_start_u,old_end_u;
	long	old_start_v,old_end_v;
} DDA;

typedef struct {
	long	fstart[2];	/*(1,19,12)format start address*/
	long	fend[2];	/*(1,19,12)format end address*/
	long	istart[2];	/*(1,31,0)format start address*/
	long	iend[2];	/*(1,31,0)format end address*/
	long	SVa[4];		/*start vertex scanline V-address*/
	long	EVa[4];		/*end vertex scanline V-address*/
	long	SVf[4][2];	/*start vertex fslant*/
	long	EVf[4][2];	/*end vertex fslant*/
        short   uv[4][2];
	SVECTOR	sv0,sv1;
	long	old_xy[2][2];
	long	old_uv[2][2];
} GDDA;

static int	get_minmax_quad(long v[4][2],long min[2],long max[2]);
static int	min4(long a,long b,long c,long d);
static int 	max4(long a,long b,long c,long d);
static void 	sort_vertex_left(long v[4][2],int sv[4]);
static void 	sort_vertex_right(long v[4][2],int ev[4]);
static void 	sort_vertex_left_up(long v[4][2],int sv[4]);
static void 	sort_vertex_right_up(long v[4][2],int ev[4]);
static int	get_fslant(long v1[2],long v2[2]);
static void 	get_fslant_edge(long v[4][2],int f[4],int s[4]);
static void 	get_sect_vect(SVECTOR *vertex,VECTOR *nsect);
static int	sign(long a);
static void 	rotate_sort(long vert2d[4][2],VECTOR *sect,
			int SVn[4],int EVn[4],long rot_vert2d[4][2]);
static void 	get_fslant_vector(long vert2d[4][2],VECTOR *sect,
			int vflg[4],long fslt[4][2]);
static int 	clip_scan(long ist[2],long ien[2],
			long min_x,long min_y,long max_x,long max_y);
static void 	draw_diag(long vert2d[4][2],long rot_vert2d[4][2],
			int SVn[4],int EVn[4],long fslt[4][2],u_long *ot,
			POLY_FT4 *packet,int n,int txs0,int txs1);
static void 	get_next_ends(int *ln,int *stn,int *edn,long vert2d[4][2],
			int SVn[4],long SVa[4],long SVf[4][2],
			int EVn[4],long EVa[4],long EVf[4][2],
			int n,long fstart[2],long fend[2]);
static void 	get_next_ends_X(int *ln,int *stn,int *edn,long vert2d[4][2],
			int SVn[4],long SVa[4],long SVf[4],
			int EVn[4],long EVa[4],long EVf[4],
			int n,long *fstart_x,long *fend_x);

void 		draw_general_square(SVECTOR *vertex,u_long *ot,
			POLY_FT4 *packet,int divn,int txs0,int txs1);

static void 	limit_mat(long m[3][3],MATRIX *mat);
static int 	maxabs(long *m,int n);
static void 	pers(VECTOR *v3d,long *v2d);
static int	abs(long n);
static void 	inv_mat(MATRIX *m,long im[3][3]);
static int	cofactor(MATRIX *m,int i,int j);
static void 	prt_matrix(MATRIX *m);

	
static get_minmax_quad(v,min,max)
long	v[4][2];
long	min[2],max[2];
{
	min[0]=min4(v[0][0],v[1][0],v[2][0],v[3][0]);
	min[1]=min4(v[0][1],v[1][1],v[2][1],v[3][1]);
	max[0]=max4(v[0][0],v[1][0],v[2][0],v[3][0]);
	max[1]=max4(v[0][1],v[1][1],v[2][1],v[3][1]);
}

static min4(a,b,c,d)
long	a,b,c,d;
{
	int min;
	min=a;
	if(b<min)min=b;
	if(c<min)min=c;
	if(d<min)min=d;
	return(min);
}

static max4(a,b,c,d)
long	a,b,c,d;
{
	int max;
	max=a;
	if(b>max)max=b;
	if(c>max)max=c;
	if(d>max)max=d;
	return(max);
}

/*sort vertex and get left edge vertex order (from up to down)*/
static void sort_vertex_left(v,sv)
long	v[4][2];
int	sv[4];
{
	int	i;
	int	vn;
	int	minY,maxY;

	for(i=0;i<4;i++) sv[i]= -1;

	minY=v[0][1];
	maxY=v[0][1];

	/*get Y-minimum vertex on the left edge				*/
	/*search Y-minimum vertex by order of 3->2->1->0	 	*/
	/*if there are many Y-minimums, 				*/
	/*then smallest number vertex(leftest vertex) remain except 0	*/
        for(i=3;i>=0;i--){              /*3->2->1->0*/
                if(v[i][1]<=minY){
                        sv[0]=i;
                        minY=v[i][1];
                }
        }

	/*if Y-minimum vertex 0 remains					*/
	/*then test if there are other Y-minimum vertex 		*/
	/*and if there are, replace 0 by them 				*/
        if(sv[0]==0){
                if(v[0][1]==v[3][1]){
                        sv[0]=3;
			if(v[0][1]==v[2][1]){
				sv[0]=2;
				if(v[0][1]==v[1][1]){
					sv[0]=1;
				}
			}
                }
        }

	/*get Y-maximum vertex on the left edge				*/
	/*search Y-maximum vertex by order of 0->1->2->3	 	*/
	/*if there are many Y-maximums, 				*/
	/*then biggest number vertex(leftest vertex) remain except 3	*/
        for(i=0;i<=3;i++){              /*0->1->2->3*/
                if(v[i][1]>=maxY){
                        sv[1]=i;
                        maxY=v[i][1];
                }
        }

	/*if Y-maximum vertex 3 remains					*/
	/*then test if there are other Y-maximum vertex 		*/
	/*and if there are, replace 3 by them 				*/
        if(sv[1]==3){
                if(v[3][1]==v[0][1]){
                        sv[1]=0;
			if(v[3][1]==v[1][1]){
				sv[1]=1;
				if(v[3][1]==v[2][1]){
					sv[1]=2;
				}
			}
                }
        }

	/*get the intermidiate vertex on the left edge			*/
        for(i=0; i<2; i++){
                if((sv[0]-1+4)%4==sv[1]) break;	/*if there are no 
						  intermidiate then 
						  (sv[0],sv[1])       */
                else{
                        sv[2]=sv[1];
                        sv[1]=(sv[0]-1+4)%4;
                        if((sv[0]-2+4)%4==sv[2]) break;	/*(sv[0],sv[1],sv[2])*/
                        else{
                                sv[3]=sv[2];
                                sv[2]=(sv[0]-2+4)%4;	/*(sv[0],sv[1],
							   sv[2],sv[3])*/
                        }
                }
        }
}

/*sort vertex and get right edge vertex order (from up to down)*/
static void sort_vertex_right(v,ev)
long	v[4][2];
int	ev[4];
{
	int	i;
	int	vn;
	int	minY,maxY;

	for(i=0;i<4;i++) ev[i]= -1;

	minY=v[0][1];
	maxY=v[0][1];

	/*get Y-minimum vertex on the right edge			*/
	/*search Y-minimum vertex by order of 0->1->2->3	 	*/
	/*if there are many Y-minimums, 				*/
	/*then biggest number vertex(rightest vertex) remain except 3	*/
        for(i=0;i<=3;i++){              /*0->1->2->3*/
                if(v[i][1]<=minY){
                        ev[0]=i;
                        minY=v[i][1];
                }
        }
	/*if Y-minimum vertex 3 remains					*/
	/*then test if there are other Y-minimum vertex 		*/
	/*and if there are, replace 3 by them 				*/
        if(ev[0]==3){
                if(v[3][1]==v[0][1]){
                        ev[0]=0;
			if(v[3][1]==v[1][1]){
				ev[0]=1;
				if(v[3][1]==v[2][1]){
					ev[0]=2;
				}
			}
                }
        }

	/*get Y-maximum vertex on the right edge			*/
	/*search Y-maximum vertex by order of 3->2->1->0	 	*/
	/*if there are many Y-maximums, 				*/
	/*then smallest number vertex(rightest vertex) remain except 0	*/
        for(i=3;i>=0;i--){              /*3->2->1->0*/
                if(v[i][1]>=maxY){
                        ev[1]=i;
                        maxY=v[i][1];
                }
        }
	/*if Y-maximum vertex 0 remains					*/
	/*then test if there are other Y-maximum vertex 		*/
	/*and if there are, replace 0 by them 				*/
        if(ev[1]==0){
                if(v[0][1]==v[3][1]){
                        ev[1]=3;
			if(v[0][1]==v[2][1]){
				ev[1]=2;
				if(v[0][1]==v[1][1]){
					ev[1]=1;
				}
			}
                }
        }

	/*get the intermidiate vertex on the right edge			*/
        for(i=0; i<2; i++){
                if((ev[0]+1)%4==ev[1]) break;	 /*if there are no 
						  intermidiate then 
						  (ev[0],ev[1])       */
                else{
                        ev[2]=ev[1];
                        ev[1]=(ev[0]+1)%4;
                        if((ev[0]+2)%4==ev[2]) break;	/*(ev[0],ev[1],ev[2])*/
                        else{
                                ev[3]=ev[2];
                                ev[2]=(ev[0]+2)%4;	/*(ev[0],ev[1],
							   ev[2],ev[3])*/
                        }
                }
        }
}

/*sort vertex and get left edge vertex order (from down to up)*/
static void sort_vertex_left_up(v,sv)
long	v[4][2];
int	sv[4];
{
	int	i;
	int	vn;
	int	minY,maxY;

	for(i=0;i<4;i++) sv[i]= -1;

	minY=v[0][1];
	maxY=v[0][1];

	/*get Y-maximum vertex on the left edge				*/
	/*search Y-maximum vertex by order of 0->1->2->3	 	*/
	/*if there are many Y-maximums, 				*/
	/*then biggest number vertex(leftest vertex) remain except 3	*/
        for(i=0;i<=3;i++){              /*0->1->2->3*/
                if(v[i][1]>=maxY){
                        sv[0]=i;
                        maxY=v[i][1];
                }
        }
	/*if Y-maximum vertex 3 remains					*/
	/*then test if there are other Y-maximum vertex 		*/
	/*and if there are, replace 3 by them 				*/
        if(sv[0]==3){
                if(v[3][1]==v[0][1]){
                        sv[0]=0;
			if(v[3][1]==v[1][1]){
				sv[0]=1;
				if(v[3][1]==v[2][1]){
					sv[0]=2;
				}
			}
                }
        }

	/*get Y-minimum vertex on the left edge				*/
	/*search Y-minimum vertex by order of 3->2->1->0	 	*/
	/*if there are many Y-minimums, 				*/
	/*then smallest number vertex(leftest vertex) remain except 0	*/
        for(i=3;i<=0;i--){              /*3->2->1->0*/
                if(v[i][1]<=minY){
                        sv[1]=i;
                        minY=v[i][1];
                }
        }
	/*if Y-minimum vertex 0 remains					*/
	/*then test if there are other Y-minimum vertex 		*/
	/*and if there are, replace 0 by them 				*/
        if(sv[1]==0){
                if(v[0][1]==v[3][1]){
                        sv[1]=3;
			if(v[0][1]==v[2][1]){
				sv[1]=2;
				if(v[0][1]==v[1][1]){
					sv[1]=1;
				}
			}
                }
        }

	/*get the intermidiate vertex on the left edge			*/
        for(i=0; i<2; i++){
                if((sv[0]+1)%4==sv[1]) break;		/*if there are no
							  intermidiate vertex
							  then (sv[0],sv[1]) */
                else{
                        sv[2]=sv[1];
                        sv[1]=(sv[0]+1)%4;
                        if((sv[0]+2)%4==sv[2]) break;	/*(sv[0],sv[1],sv[2])*/
                        else{
                                sv[3]=sv[2];
                                sv[2]=(sv[0]+2)%4;	/*(sv[0],sv[1],
							   sv[2],sv[3])*/
                        }
                }
        }
}

/*sort vertex and get right edge vertex order (from down to up)*/
static void sort_vertex_right_up(v,ev)
long	v[4][2];
int	ev[4];
{
	int	i;
	int	vn;
	int	minY,maxY;

	for(i=0;i<4;i++) ev[i]= -1;

	minY=v[0][1];
	maxY=v[0][1];

	/*get Y-maximum vertex on the right edge			*/
	/*search Y-maximum vertex by order of 3->2->1->0	 	*/
	/*if there are many Y-maximums, 				*/
	/*then smallest number vertex(rightest vertex) remain except 0	*/
        for(i=3;i>=0;i--){              /*3->2->1->0*/
                if(v[i][1]>=maxY){
                        ev[0]=i;
                        maxY=v[i][1];
                }
        }
	/*if Y-maximum vertex 0 remains					*/
	/*then test if there are other Y-maximum vertex 		*/
	/*and if there are, replace 0 by them 				*/
        if(ev[0]==0){
                if(v[0][1]==v[3][1]){
                        ev[0]=3;
			if(v[0][1]==v[2][1]){
				ev[0]=2;
				if(v[0][1]==v[1][1]){
					ev[0]=1;
				}
			}
                }
        }

	/*get Y-minimum vertex on the right edge			*/
	/*search Y-minimum vertex by order of 0->1->2->3	 	*/
	/*if there are many Y-minimums, 				*/
	/*then biggest number vertex(rightest vertex) remain except 3	*/
        for(i=0;i<=3;i++){              /*0->1->2->3*/
                if(v[i][1]<=minY){
                        ev[1]=i;
                        minY=v[i][1];
                }
        }
	/*if Y-minimum vertex 3 remains					*/
	/*then test if there are other Y-minimum vertex 		*/
	/*and if there are, replace 3 by them 				*/
        if(ev[1]==3){
                if(v[3][1]==v[0][1]){
                        ev[1]=0;
			if(v[3][1]==v[1][1]){
				ev[1]=1;
				if(v[3][1]==v[2][1]){
					ev[1]=2;
				}
			}
                }
        }

	/*get the intermidiate vertex on the left edge			*/
        for(i=0; i<2; i++){
                if((ev[0]-1+4)%4==ev[1]) break;		/*if there are no
							  intermidiate vertex
							  then (ev[0],ev[1])*/
                else{
                        ev[2]=ev[1];
                        ev[1]=(ev[0]-1+4)%4;
                        if((ev[0]-2+4)%4==ev[2]) break;	/*(ev[0],ev[1],ev[2])*/
                        else{
                                ev[3]=ev[2];
                                ev[2]=(ev[0]-2+4)%4;	/*(ev[0],ev[1],
							   ev[2],ev[3])*/
                        }
                }
        }
}

/* return strict slant by (1,19,12)format	*/
/* fslnt= (v2[0]-v1[0])/(v2[1]-v1[1])		*/
static get_fslant(v1,v2)
long	v1[2],v2[2];
{
	long	v21[2];

	if(abs(v2[0]-v1[0])<524288){
		v21[0]= v2[0]-v1[0];
		v21[1]= v2[1]-v1[1];
		if(v21[1]==0) return(0x7fffffff);
		else return(4096*v21[0]/v21[1]);
	}else{
		v21[0]= (v2[0]-v1[0])/4096;
		v21[1]= (v2[1]-v1[1])/4096;
		if(v21[1]==0) return(0x7fffffff);
		else return(4096*v21[0]/v21[1]);
	}
}

/* get strict slant of four edge*/
/* if(flg0==0)edge01->fslt0,else edge10->fslt0	*/
/* if(flg1==0)edge12->fslt1,else edge21->fslt1	*/
/* if(flg2==0)edge23->fslt2,else edge32->fslt2	*/
/* if(flg3==0)edge30->fslt3,else edge03->fslt3 	*/
static void get_fslant_edge(v,f,s)
long	v[4][2];
int	f[4];
int	s[4];
{
	if(f[0]==0) s[0]= get_fslant(v[0],v[1]);
	else s[0]= get_fslant(v[1],v[0]);
	if(f[1]==0) s[1]= get_fslant(v[1],v[2]);
	else s[1]= get_fslant(v[2],v[1]);
	if(f[2]==0) s[2]= get_fslant(v[2],v[3]);
	else s[2]= get_fslant(v[3],v[2]);
	if(f[3]==0) s[3]= get_fslant(v[3],v[0]);
	else s[3]= get_fslant(v[0],v[3]);
}

static void get_sect_vect(vertex,nsect)
SVECTOR	*vertex;		/*3D 4 vertices*/
VECTOR	*nsect;
{
	VECTOR	vert3d[3];
	VECTOR	v01,v02;
	VECTOR	opv;
	VECTOR	normal;
	VECTOR	sect;
	VECTOR	start,end;
	long	p,flag;
	long	abs,sqabs;
	long	v2d[2];
	MATRIX	mat;

	RotTrans(&vertex[0],&vert3d[0],&flag);
	RotTrans(&vertex[1],&vert3d[1],&flag);
	RotTrans(&vertex[2],&vert3d[2],&flag);

	v01.vx= vert3d[1].vx - vert3d[0].vx;
	v01.vy= vert3d[1].vy - vert3d[0].vy;
	v01.vz= vert3d[1].vz - vert3d[0].vz;
	v02.vx= vert3d[2].vx - vert3d[0].vx;
	v02.vy= vert3d[2].vy - vert3d[0].vy;
	v02.vz= vert3d[2].vz - vert3d[0].vz;

	abs= maxabs((long*)&v01,3);
	if(abs>30000){
	sqabs= SquareRoot0(abs);
	v01.vx /= sqabs;
	v01.vy /= sqabs;
	v01.vz /= sqabs;
	}
	abs= maxabs((long*)&v02,3);
	if(abs>30000){
	sqabs= SquareRoot0(abs);
	v02.vx /= sqabs;
	v02.vy /= sqabs;
	v02.vz /= sqabs;
	}

	OuterProduct0(&v01,&v02,&opv);
	abs= maxabs((long*)&opv,3);
	if(abs>30000){
	sqabs= SquareRoot0(abs);
	opv.vx /= sqabs;
	opv.vy /= sqabs;
	opv.vz /= sqabs;
	}
	VectorNormal(&opv,&normal);

	/*draw normal*/
#ifdef DRAWNORMAL
		start.vx= (vert3d[0].vx+vert3d[1].vx+vert3d[2].vx)/3;
		start.vy= (vert3d[0].vy+vert3d[1].vy+vert3d[2].vy)/3;
		start.vz= (vert3d[0].vz+vert3d[1].vz+vert3d[2].vz)/3;

		end.vx= start.vx-normal.vx/10;
		end.vy= start.vy-normal.vy/10;
		end.vz= start.vz-normal.vz/10;

		pers(&start,v2d);
		db->normal.x0= v2d[0];
		db->normal.y0= v2d[1];
		pers(&end,v2d);
		db->normal.x1= v2d[0];
		db->normal.y1= v2d[1];

		AddPrim(db->ot,&db->normal);
#endif
	abs= maxabs((long*)&normal,2);

	if(abs<100){
		sect.vx= 4096*sign(normal.vy);
		sect.vy= 0;
		sect.vz= 0;
	}else{
		sect.vx=  normal.vy;
		sect.vy= -normal.vx;
		sect.vz=  0;
	}

	VectorNormal(&sect,nsect);

	/*draw sect*/
#ifdef DRAWSECT
		end.vx= start.vx + nsect->vx/10;
		end.vy= start.vy + nsect->vy/10;
		end.vz= start.vz + nsect->vz/10;

		pers(&start,v2d);
		db->sect.x0= v2d[0];
		db->sect.y0= v2d[1];
		pers(&end,v2d);
		db->sect.x1= v2d[0];
		db->sect.y1= v2d[1];
	
		AddPrim(db->ot,&db->sect);
#endif
}

static sign(a)
long	a;
{
	if(a>=0) return(1);
	else	return(-1);
}

#define BIGNUM	100000
/********************************************************
*							*
*	rot_vert2d = [ sect.vx, sect.vy ]* vert2d	*
*		     [-sect.vy, sect.vx ]		*
*							*
*	rot_vert2d_1 = [-sect.vx,-sect.vy ]* vert2d	*
*		       [ sect.vy,-sect.vx ]		*
*							*
********************************************************/
static void rotate_sort(vert2d,sect,SVn,EVn,rot_vert2d)
long	vert2d[4][2];		/*2D vertices*/
VECTOR	*sect;			/*intersection vector*/
int	SVn[4];			/*start vertex number*/
int	EVn[4];			/*end vertex number*/
long	rot_vert2d[4][2];
{
	long	rot_vert2d_1[4][2];
	long	dist0,dist1;
	long	center[2];
	long	rot_center[2];
	long	rot_center_1[2];
	int	SVn_1[4];			/*start vertex number*/
	int	EVn_1[4];			/*end vertex number*/
	int	i;

/*vert2d= Order(vertex*SCRZ/NEARZ), so following limitter is unnecessary
	if(vert2d[0][0]>BIGNUM) vert2d[0][0]= BIGNUM;
	if(vert2d[0][0]<-BIGNUM) vert2d[0][0]= -BIGNUM;
	if(vert2d[0][1]>BIGNUM) vert2d[0][1]= BIGNUM;
	if(vert2d[0][1]<-BIGNUM) vert2d[0][1]= -BIGNUM;
	if(vert2d[1][0]>BIGNUM) vert2d[1][0]= BIGNUM;
	if(vert2d[1][0]<-BIGNUM) vert2d[1][0]= -BIGNUM;
	if(vert2d[1][1]>BIGNUM) vert2d[1][1]= BIGNUM;
	if(vert2d[1][1]<-BIGNUM) vert2d[1][1]= -BIGNUM;
	if(vert2d[2][0]>BIGNUM) vert2d[2][0]= BIGNUM;
	if(vert2d[2][0]<-BIGNUM) vert2d[2][0]= -BIGNUM;
	if(vert2d[2][1]>BIGNUM) vert2d[2][1]= BIGNUM;
	if(vert2d[2][1]<-BIGNUM) vert2d[2][1]= -BIGNUM;
	if(vert2d[3][0]>BIGNUM) vert2d[3][0]= BIGNUM;
	if(vert2d[3][0]<-BIGNUM) vert2d[3][0]= -BIGNUM;
	if(vert2d[3][1]>BIGNUM) vert2d[3][1]= BIGNUM;
	if(vert2d[3][1]<-BIGNUM) vert2d[3][1]= -BIGNUM;
*/
	center[0]= PIH/2;
	center[1]= PIV/2;

	rot_center[0]=  sect->vx*center[0] + sect->vy*center[1];
	rot_center[1]= -sect->vy*center[0] + sect->vx*center[1];

	rot_vert2d[0][0]=  sect->vx*vert2d[0][0] + sect->vy*vert2d[0][1];
	rot_vert2d[0][1]= -sect->vy*vert2d[0][0] + sect->vx*vert2d[0][1];
	rot_vert2d[1][0]=  sect->vx*vert2d[1][0] + sect->vy*vert2d[1][1];
	rot_vert2d[1][1]= -sect->vy*vert2d[1][0] + sect->vx*vert2d[1][1];
	rot_vert2d[2][0]=  sect->vx*vert2d[2][0] + sect->vy*vert2d[2][1];
	rot_vert2d[2][1]= -sect->vy*vert2d[2][0] + sect->vx*vert2d[2][1];
	rot_vert2d[3][0]=  sect->vx*vert2d[3][0] + sect->vy*vert2d[3][1];
	rot_vert2d[3][1]= -sect->vy*vert2d[3][0] + sect->vx*vert2d[3][1];

	sort_vertex_left(rot_vert2d,SVn);
	sort_vertex_right(rot_vert2d,EVn);

	dist0= abs(rot_center[1]-rot_vert2d[SVn[0]][1]);

	rot_center_1[0]= -sect->vx*center[0] - sect->vy*center[1];
	rot_center_1[1]=  sect->vy*center[0] - sect->vx*center[1];

	rot_vert2d_1[0][0]= -sect->vx*vert2d[0][0] - sect->vy*vert2d[0][1];
	rot_vert2d_1[0][1]=  sect->vy*vert2d[0][0] - sect->vx*vert2d[0][1];
	rot_vert2d_1[1][0]= -sect->vx*vert2d[1][0] - sect->vy*vert2d[1][1];
	rot_vert2d_1[1][1]=  sect->vy*vert2d[1][0] - sect->vx*vert2d[1][1];
	rot_vert2d_1[2][0]= -sect->vx*vert2d[2][0] - sect->vy*vert2d[2][1];
	rot_vert2d_1[2][1]=  sect->vy*vert2d[2][0] - sect->vx*vert2d[2][1];
	rot_vert2d_1[3][0]= -sect->vx*vert2d[3][0] - sect->vy*vert2d[3][1];
	rot_vert2d_1[3][1]=  sect->vy*vert2d[3][0] - sect->vx*vert2d[3][1];

	sort_vertex_left(rot_vert2d_1,SVn_1);
	sort_vertex_right(rot_vert2d_1,EVn_1);

	dist1= abs(rot_center_1[1]-rot_vert2d_1[SVn_1[0]][1]);

	if(dist0>dist1){
		SVn[0]= SVn_1[0];
		SVn[1]= SVn_1[1];
		SVn[2]= SVn_1[2];
		SVn[3]= SVn_1[3];
		EVn[0]= EVn_1[0];
		EVn[1]= EVn_1[1];
		EVn[2]= EVn_1[2];
		EVn[3]= EVn_1[3];
		rot_vert2d[0][0]= rot_vert2d_1[0][0];
		rot_vert2d[0][1]= rot_vert2d_1[0][1];
		rot_vert2d[1][0]= rot_vert2d_1[1][0];
		rot_vert2d[1][1]= rot_vert2d_1[1][1];
		rot_vert2d[2][0]= rot_vert2d_1[2][0];
		rot_vert2d[2][1]= rot_vert2d_1[2][1];
		rot_vert2d[3][0]= rot_vert2d_1[3][0];
		rot_vert2d[3][1]= rot_vert2d_1[3][1];
	}
}

/* get strict slant vector of four edge*/
/* edge01->fslt[0]	*/
/* edge12->fslt[1]	*/
/* edge23->fslt[2]	*/
/* edge30->fslt[3] 	*/
static void get_fslant_vector(vert2d,sect,vflg,fslt)
long	vert2d[4][2];		/*2D vertices*/
VECTOR	*sect;			/*intersection unit vector*/
int	vflg[4];
long	fslt[4][2];		/*slant vectors*/	
{
	int	i;
	VECTOR	scan;		/*unit vector perpendicular to sect*/
	VECTOR	edge[4];	/*edge vectors*/
	long	scmp[4];	/*component of edge vector to scan vector*/
	long	maxedge;

	edge[0].vx= vert2d[1][0] - vert2d[0][0];
	edge[0].vy= vert2d[1][1] - vert2d[0][1];
	edge[0].vz= 0;
	edge[1].vx= vert2d[2][0] - vert2d[1][0];
	edge[1].vy= vert2d[2][1] - vert2d[1][1];
	edge[1].vz= 0;
	edge[2].vx= vert2d[3][0] - vert2d[2][0];
	edge[2].vy= vert2d[3][1] - vert2d[2][1];
	edge[2].vz= 0;
	edge[3].vx= vert2d[0][0] - vert2d[3][0];
	edge[3].vy= vert2d[0][1] - vert2d[3][1];
	edge[3].vz= 0;

	scan.vx= -sect->vy;
	scan.vy= sect->vx;
	scan.vz= 0;

	for(i=0;i<4;i++){
/* edge= Order(vertex*SCRZ/NEARZ), so following limitter is unnecessary
		maxedge= maxabs(&edge[i],2);
		if(abs(maxedge)>30000){
			edge[i].vx /= 256;
			edge[i].vy /= 256;
		}
*/
		scmp[i]= (edge[i].vx*scan.vx + edge[i].vy*scan.vy)>>12;

		if(abs(scmp[i])>=10){
		    if(vflg[i]==0&&vflg[(i+1)%4]==0){
			fslt[i][0]= (edge[i].vx<<12)/abs(scmp[i]);
			fslt[i][1]= (edge[i].vy<<12)/abs(scmp[i]);
		    }else{
			fslt[i][0]= -(edge[i].vx<<12)/abs(scmp[i]);
			fslt[i][1]= -(edge[i].vy<<12)/abs(scmp[i]);
		    }
		}else{
			fslt[i][0]= 0; 
			fslt[i][1]= 0;
		}
	}
}

static clip_scan(ist,ien,min_x,min_y,max_x,max_y)
long	ist[2];
long	ien[2];
long	min_x,min_y;
long	max_x,max_y;
{
	if(ist[0]<min_x){
	    if(ien[0]>=min_x){
		ist[1] += (min_x-ist[0])*(ien[1]-ist[1])/(ien[0]-ist[0]);
		ist[0]  = min_x; 
	    }else{
		return(-1);
	    }
	}
	if(ist[1]<min_y){
	    if(ien[1]>=min_y){
		ist[0] += (min_y-ist[1])*(ien[0]-ist[0])/(ien[1]-ist[1]);
		ist[1]  = min_y; 
	    }else{
		return(-1);
	    }
	}
	if(ist[0]>max_x){
	    if(ien[0]<=max_x){
		ist[1] += (max_x-ist[0])*(ien[1]-ist[1])/(ien[0]-ist[0]);
		ist[0]  = max_x; 
	    }else{
		return(-1);
	    }
	}
	if(ist[1]>max_y){
	    if(ien[1]<=max_y){
		ist[0] += (max_y-ist[1])*(ien[0]-ist[0])/(ien[1]-ist[1]);
		ist[1]  = max_y; 
	    }else{
		return(-1);
	    }
	}

	if(ien[0]<min_x){
	    if(ist[0]>=min_x){
		ien[1] += (min_x-ien[0])*(ist[1]-ien[1])/(ist[0]-ien[0]);
		ien[0]  = min_x; 
	    }else{
		return(-1);
	    }
	}
	if(ien[1]<min_y){
	    if(ist[1]>=min_y){
		ien[0] += (min_y-ien[1])*(ist[0]-ien[0])/(ist[1]-ien[1]);
		ien[1]  = min_y; 
	    }else{
		return(-1);
	    }
	}
	if(ien[0]>max_x){
	    if(ist[0]<=max_x){
		ien[1] += (max_x-ien[0])*(ist[1]-ien[1])/(ist[0]-ien[0]);
		ien[0]  = max_x; 
	    }else{
		return(-1);
	    }
	}
	if(ien[1]>max_y){
	    if(ist[1]<=max_y){
		ien[0] += (max_y-ien[1])*(ist[0]-ien[0])/(ist[1]-ien[1]);
		ien[1]  = max_y; 
	    }else{
		return(-1);
	    }
	}
	return(0);
}

/**********************
* draw general square *
**********************/
static void draw_diag(vert2d,rot_vert2d,SVn,EVn,fslt,ot,packet,n,txs0,txs1)
long	vert2d[4][2];		/*2D vertices*/
long	rot_vert2d[4][2];	/*(1,19,12)format*/
int	SVn[4];			/*start vertex number*/
int	EVn[4];			/*end vertex number*/
long	fslt[4][2];		/*slant vectors*/	
u_long	*ot;
POLY_FT4	*packet;
int	n;			/*scan pitch*/
int	txs0,txs1;
{
	int	i,j;
	int	ln;		/*scanline number*/
	int	stn,edn;	/*scan line position*/

	long	rot_min[2];
	long	rot_max[2];

	int	dflag;		/*draw at least 1 line or not*/

	POLY_FT4	*s;

	GDDA	*SC;


	s= packet;
	SC= (GDDA*)getScratchAddr(0);

	for(i=0;i<4;i++){			/*set edge slant*/
		SC->SVf[i][0]= -fslt[(SVn[i]+3)%4][0]*n;
		SC->EVf[i][0]= fslt[EVn[i]][0]*n;
		SC->SVf[i][1]= -fslt[(SVn[i]+3)%4][1]*n;
		SC->EVf[i][1]= fslt[EVn[i]][1]*n;
		SC->SVa[i]= rot_vert2d[SVn[i]][1];
		SC->EVa[i]= rot_vert2d[EVn[i]][1];
	}
	SC->fstart[0]= vert2d[SVn[0]][0]<<12;
	SC->fstart[1]= vert2d[SVn[0]][1]<<12;
	SC->fend[0]= vert2d[EVn[0]][0]<<12;
	SC->fend[1]= vert2d[EVn[0]][1]<<12;

	get_minmax_quad(rot_vert2d,rot_min,rot_max);

        SC->sv0.vz= ReadGeomScreen()*16;
        SC->sv1.vz= ReadGeomScreen()*16;
        SetGeomScreen(MAXTSIZE);

	SC->old_xy[0][0]= vert2d[SVn[0]][0];
	SC->old_xy[0][1]= vert2d[SVn[0]][1];
	SC->old_xy[1][0]= vert2d[SVn[0]][0];
	SC->old_xy[1][1]= vert2d[SVn[0]][1];

	switch(SVn[0]){
	case 0:	
		SC->old_uv[0][0]= 0;
		SC->old_uv[0][1]= 0;
		SC->old_uv[1][0]= 0;
		SC->old_uv[1][1]= 0;
		break;
	case 1:
		SC->old_uv[0][0]= txs0;
		SC->old_uv[0][1]= 0;
		SC->old_uv[1][0]= txs0;
		SC->old_uv[1][1]= 0;
		break;
	case 2:
		SC->old_uv[0][0]= txs0;
		SC->old_uv[0][1]= txs1;
		SC->old_uv[1][0]= txs0;
		SC->old_uv[1][1]= txs1;
		break;
	case 3:
		SC->old_uv[0][0]= 0;
		SC->old_uv[0][1]= txs1;
		SC->old_uv[1][0]= 0;
		SC->old_uv[1][1]= txs1;
		break;
	default:
		break;
	}
	
	dflag=0;	/*no draw*/

	stn=0;
	edn=0;

	ln= rot_min[1];

	for(i=0;i<MAXPAC;i++){


	    SC->istart[0]= SC->fstart[0]>>12;
	    SC->istart[1]= SC->fstart[1]>>12;
	    SC->iend[0]= SC->fend[0]>>12;
	    SC->iend[1]= SC->fend[1]>>12;

	    if(clip_scan(SC->istart,SC->iend,0,0,PIH,PIV)!= -1){
		dflag=1;		/*in screen ->draw*/

	        SC->sv0.vx= (SC->istart[0] - (PIH/2))*16;
	    	SC->sv0.vy= (SC->istart[1] - (PIV/2))*16;
	    	SC->sv1.vx= (SC->iend[0] - (PIH/2))*16;
	    	SC->sv1.vy= (SC->iend[1] - (PIV/2))*16;

	    	gte_ldv0(&SC->sv0);		/*load only 2 screen address*/
	    	gte_ldv1(&SC->sv1);
	    	gte_rtpt();			/*RotTransPers3*/
					/*GTE delay slot*/

	    	s->x0= SC->old_xy[0][0];
	    	s->y0= SC->old_xy[0][1];
	    	s->x1= SC->old_xy[1][0];
	    	s->y1= SC->old_xy[1][1];
	    	s->x2= SC->istart[0];
	    	s->y2= SC->istart[1];
	    	s->x3= SC->iend[0];
	    	s->y3= SC->iend[1];

 	    	gte_stsxy0((long*)SC->uv[0]);	/*store 2 texture address*/
	    	gte_stsxy1((long*)SC->uv[1]);

	    	for(j=0;j<2;j++){		/*texture address limiter*/
                    if(SC->uv[j][0]<0)SC->uv[j][0]=0;		
                    if(SC->uv[j][0]>255)SC->uv[j][0]=255;	
                    if(SC->uv[j][1]<0)SC->uv[j][1]=0;
                    if(SC->uv[j][1]>255)SC->uv[j][1]=255;
	    	}
					/*set UV-address to packet*/

	    	s->u0= SC->old_uv[0][0];
	    	s->v0= SC->old_uv[0][1];
	    	s->u1= SC->old_uv[1][0];
	    	s->v1= SC->old_uv[1][1];
	    	s->u2= SC->old_uv[0][0]= SC->uv[0][0];
	    	s->v2= SC->old_uv[0][1]= SC->uv[0][1];
	    	s->u3= SC->old_uv[1][0]= SC->uv[1][0];
	    	s->v3= SC->old_uv[1][1]= SC->uv[1][1];

	    	addPrim(ot+1,s);
		s++;
	    }else{
		if(dflag==1) break;	/*out screen after draw->end*/
	    }

	    SC->old_xy[0][0]= SC->istart[0];
	    SC->old_xy[0][1]= SC->istart[1];
	    SC->old_xy[1][0]= SC->iend[0];
	    SC->old_xy[1][1]= SC->iend[1];

	    if(ln>=rot_max[1]) break;

	    get_next_ends(
		&ln,&stn,&edn,vert2d,
		SVn,SC->SVa,SC->SVf,
		EVn,SC->EVa,SC->EVf,
		n,
		SC->fstart,SC->fend);
	}
}

static void get_next_ends(
		ln,stn,edn,vert2d,SVn,SVa,SVf,EVn,EVa,EVf,n,fstart,fend)
int	*ln;		/*scanline number*/
int	*stn,*edn;	/*scan line position of start edge & end edge*/
long	vert2d[4][2];	/*2D vertices*/
int	SVn[4];		/*start vertex number*/
long	SVa[4];		/*start vertex scanline V-address*/
long	SVf[4][2];	/*start vertex fslant*/
int	EVn[4];		/*end vertex number*/
long	EVa[4];		/*end vertex scanline V-address*/
long	EVf[4][2];	/*end vertex fslant*/
int	n;
long	fstart[2];	/*(1,19,12)format start address*/
long	fend[2];	/*(1,19,12)format end address*/
{
	long	next_S,next_E;
	long	fract;
	long	pitch;


	pitch= 4096*n;

	next_S= SVa[*stn+1]- (*ln);
	next_E= EVa[*edn+1]- (*ln);

	if(next_S>pitch&&next_E>pitch){
	    fstart[0] += SVf[*stn][0];
	    fstart[1] += SVf[*stn][1];
	    fend[0] += EVf[*edn][0];
	    fend[1] += EVf[*edn][1];
	    (*ln) += pitch;
	}else{
	    if(next_S==next_E){
		(*stn)++;
		(*edn)++;
		fstart[0] = vert2d[SVn[*stn]][0]<<12;
		fstart[1] = vert2d[SVn[*stn]][1]<<12;
		fend[0] = vert2d[EVn[*edn]][0]<<12;
		fend[1] = vert2d[EVn[*edn]][1]<<12;
		(*ln) = SVa[*stn];
	    }else{
		if(next_S<next_E){
		    fract= next_S/n;
		    (*stn)++;
		    fstart[0] = vert2d[SVn[*stn]][0]<<12;
		    fstart[1] = vert2d[SVn[*stn]][1]<<12;
		    fend[0] += (EVf[*edn][0]*fract)>>12;
		    fend[1] += (EVf[*edn][1]*fract)>>12;
		    (*ln) = SVa[*stn];
		}else{
		    fract= next_E/n;
		    (*edn)++;
	    	    fstart[0] += (SVf[*stn][0]*fract)>>12;
	            fstart[1] += (SVf[*stn][1]*fract)>>12;
		    fend[0] = vert2d[EVn[*edn]][0]<<12;
		    fend[1] = vert2d[EVn[*edn]][1]<<12;
		    (*ln) = EVa[*edn];
		}
	    }
	}

}

void draw_general_square(vertex,ot,packet,divn,txs0,txs1)
SVECTOR	*vertex;		/*3D 4 vertices*/
u_long	*ot;
POLY_FT4	*packet;
int	divn;
int     txs0,txs1;
{
	VECTOR	sect;		/*intersection of plane & Z-plane*/
	int	SVn[4];		/*start vertex number*/
	int	EVn[4];		/*end vertex number*/

	int	i,j;
	long	min[2],max[2];
	long	fslt[4][2];		/*slant vectors*/	
	long	flg;
	VECTOR	vert3d[4];
	MATRIX	pmat;
	long	iparray[3][3];
	MATRIX	ipmat;
	long	ofx,ofy;
	MATRIX	SWmat;
	long	scrn;
	long 	vert2d[4][2];		/*2D 4 vertices(limited)*/
	long 	vert2d_1[4][2];		/*2D 4 vertices(non limited)*/
	int	vflg[4];

	int	DDAflag;
	VECTOR  sca;

	long	rot_vert2d[4][2];
	VECTOR	v0,v1;
	VECTOR	nml;

	int	clip;

	/*get 2D vertex for draw range*/
	RotTrans(&vertex[0],&vert3d[0],&flg);
	RotTrans(&vertex[1],&vert3d[1],&flg);
	RotTrans(&vertex[2],&vert3d[2],&flg);
	RotTrans(&vertex[3],&vert3d[3],&flg);

	/*normal clip*/
	v0.vx= vert3d[1].vx - vert3d[0].vx;
	v0.vy= vert3d[1].vy - vert3d[0].vy;
	v0.vz= vert3d[1].vz - vert3d[0].vz;
	v1.vx= vert3d[2].vx - vert3d[0].vx;
	v1.vy= vert3d[2].vy - vert3d[0].vy;
	v1.vz= vert3d[2].vz - vert3d[0].vz;

	OuterProduct12(&v1,&v0,&nml);

	clip= nml.vx*vert3d[0].vx + nml.vy*vert3d[0].vy + nml.vz*vert3d[0].vz;

	if(clip>=0) return;

	/*clip view point backward polygon*/
	if(vert3d[0].vz<=0&&			
	   vert3d[1].vz<=0&&
	   vert3d[2].vz<=0&&
	   vert3d[3].vz<=0) return;

	/*clear vflg*/
	vflg[0]=0;
	vflg[1]=0;
	vflg[2]=0;
	vflg[3]=0;

	/*save GTE parameters*/
	ReadRotMatrix(&SWmat);
	ReadGeomOffset(&ofx,&ofy);
	scrn= ReadGeomScreen();

	/*define polygon matrix as parallelogram*/
	pmat.m[0][0]= vert3d[1].vx - vert3d[0].vx; 
	pmat.m[1][0]= vert3d[1].vy - vert3d[0].vy; 
	pmat.m[2][0]= vert3d[1].vz - vert3d[0].vz; 
	pmat.m[0][1]= vert3d[3].vx - vert3d[0].vx; 
	pmat.m[1][1]= vert3d[3].vy - vert3d[0].vy; 
	pmat.m[2][1]= vert3d[3].vz - vert3d[0].vz; 
	pmat.m[0][2]= vert3d[0].vx; 
	pmat.m[1][2]= vert3d[0].vy; 
	pmat.m[2][2]= vert3d[0].vz; 

	/*if(Z==0) limit Z-value to NEARZ*/
	if(-NEARZ<vert3d[0].vz&&vert3d[0].vz<0)vert3d[0].vz= -NEARZ;
	if(-NEARZ<vert3d[1].vz&&vert3d[1].vz<0)vert3d[1].vz= -NEARZ;
	if(-NEARZ<vert3d[2].vz&&vert3d[2].vz<0)vert3d[2].vz= -NEARZ;
	if(-NEARZ<vert3d[3].vz&&vert3d[3].vz<0)vert3d[3].vz= -NEARZ;

	if(0<=vert3d[0].vz&&vert3d[0].vz<NEARZ)vert3d[0].vz=NEARZ;
	if(0<=vert3d[1].vz&&vert3d[1].vz<NEARZ)vert3d[1].vz=NEARZ;
	if(0<=vert3d[2].vz&&vert3d[2].vz<NEARZ)vert3d[2].vz=NEARZ;
	if(0<=vert3d[3].vz&&vert3d[3].vz<NEARZ)vert3d[3].vz=NEARZ;

	/*perspective for correct slant calculation*/
	pers(&vert3d[0],vert2d_1[0]);
	pers(&vert3d[1],vert2d_1[1]);
	pers(&vert3d[2],vert2d_1[2]);
	pers(&vert3d[3],vert2d_1[3]);

	/*if(Z<0) limit Z-value to NEARZ and vflg=ON*/
	if(vert3d[0].vz<0){vflg[0]=1;}
	if(vert3d[1].vz<0){vflg[1]=1;}
	if(vert3d[2].vz<0){vflg[2]=1;}
	if(vert3d[3].vz<0){vflg[3]=1;}

	if(vert3d[0].vz<NEARZ){vert3d[0].vz=NEARZ;}
	if(vert3d[1].vz<NEARZ){vert3d[1].vz=NEARZ;}
	if(vert3d[2].vz<NEARZ){vert3d[2].vz=NEARZ;}
	if(vert3d[3].vz<NEARZ){vert3d[3].vz=NEARZ;}

	/*perspective for limitted DDA*/
	pers(&vert3d[0],vert2d[0]);
	pers(&vert3d[1],vert2d[1]);
	pers(&vert3d[2],vert2d[2]);
	pers(&vert3d[3],vert2d[3]);

	/*get minmax of Quad. */
	get_minmax_quad(vert2d,min,max);

	/*if Quad. is out of screen, return*/
	if(max[0]<0||max[1]<0||min[0]>=PIH||min[1]>=PIV){
		SetGeomOffset(ofx,ofy); 
		SetGeomScreen(scrn); 
		SetRotMatrix(&SWmat);
		SetTransMatrix(&SWmat);
		return;
	}

	/*get inverse matrix*/
	inv_mat(&pmat,iparray);

	/*limit matrix for GTE calculation*/
	limit_mat(iparray,&ipmat);
	ipmat.t[0]= 0;
	ipmat.t[1]= 0;
	ipmat.t[2]= 0;
	

	get_sect_vect(Vertex,&sect);

	get_fslant_vector(vert2d_1,&sect,vflg,fslt);

        /*scale vector set*/
        sca.vx= 4096*txs0/MAXTSIZE;
        sca.vy= 4096*txs1/MAXTSIZE;
        sca.vz= 4096;

        /*scale ipmat*/
        ScaleMatrixL(&ipmat,&sca);

	/*GTE set for inverse perspective*/
	SetRotMatrix(&ipmat);
	SetTransMatrix(&ipmat);
	SetGeomOffset(0,0);


	rotate_sort(vert2d,&sect,SVn,EVn,rot_vert2d);

	draw_diag(vert2d,rot_vert2d,SVn,EVn,fslt,ot,packet,divn,txs0,txs1);

	/*reload GTE registers*/
	SetGeomOffset(ofx,ofy);
	SetGeomScreen(scrn);
	SetRotMatrix(&SWmat);
	SetTransMatrix(&SWmat);
}


static void limit_mat(m,mat)
long	m[3][3];
MATRIX	*mat;
{
	long	max;
	int	lzc;
	int	i,j;
	int	shift;

	max=maxabs((long*)m,9);
	lzc= Lzc(max);
	if(lzc>=19) shift=0;
	else shift= 19-lzc;

	for(i=0;i<3;i++)
	for(j=0;j<3;j++){
		mat->m[i][j]= m[i][j]>>shift;
	}
}

static maxabs(m,n)
long	*m;
int	n;
{
	int	i;
	long	a;
	long	max;

	max=0;
	for(i=0;i<n;i++){
		if(m[i]<0)a= -m[i];
		else	a= m[i];
		if(a>max) max=a;
	}
	return(max);
}

static void pers(v3d,v2d)
VECTOR	*v3d;
long	*v2d;
{
	long	h;
	long	ofx,ofy;
	long	a,b;

	h= ReadGeomScreen();
	ReadGeomOffset(&ofx,&ofy);

	a= ofx + (v3d->vx*h)/v3d->vz;
	b= ofy + (v3d->vy*h)/v3d->vz;
/*
	if(a<-32768)a= -32768;
	if(a>32767)a= 32767;
	if(b<-32768)b= -32768;
	if(b>32767)b= 32767;
*/
	v2d[0]=a;
	v2d[1]=b;
}



static abs(n)
long	n;
{
	if(n<0) return(-n);
	else return(n);
}



/*inveres matrix without /det(M) */
void inv_mat(m,im)
MATRIX *m;
long im[3][3];
{
        int     i,j;

        for(i=0;i<3;i++)
        for(j=0;j<3;j++){
                im[i][j]= cofactor(m,j,i);
        }
}

/*cofactor of matrix(余因子)*/
static cofactor(m,i,j)
MATRIX *m;
int     i,j;
{
        int     p,q;
        int     r,s;
        int     a[2][2];
        int     sign;
        int     value;

        if(i<0||i>=3||j<0||j>=3){
                printf("Error: invalid row/column number\n");
                exit();
        }

        if((i+j)%2==0) sign=1;
        else    sign= -1;

        for(p=0,r=0;p<2;p++,r++){
                if(r==i) r++;
                for(q=0,s=0;q<2;q++,s++){
                        if(s==j) s++;
                        a[p][q]= m->m[r][s];
                }
        }
        return(sign*(a[0][0]*a[1][1]-a[0][1]*a[1][0]));
}


static void prt_matrix(m)
MATRIX *m;
{
	printf("m=(%d,%d,%d)\n",m->m[0][0] ,m->m[0][1] ,m->m[0][2]);
	printf("m=(%d,%d,%d)\n",m->m[1][0] ,m->m[1][1] ,m->m[1][2]);
	printf("m=(%d,%d,%d)\n",m->m[2][0] ,m->m[2][1] ,m->m[2][2]);
	printf("m=(%d,%d,%d)\n",m->t[0] ,m->t[1] ,m->t[2]);
	printf("\n");
}

/*
 * Draw Square Image
 */
draw_ground_square2(vertex,ot,s,divn,txs0,txs1)
SVECTOR *vertex;                /*3D 4 vertices*/
u_long	*ot;
POLY_FT4      *s;
int	divn;
int	txs0,txs1;
{
        int     i,j;
        long     min[2],max[2];
        int     fslt[4];
        long    flg;
        VECTOR  vert3d[4];
        MATRIX  pmat;
        long    iparray[3][3];
        MATRIX  ipmat;
        long    ofx,ofy;
        MATRIX  SWmat;
        long    scrn;
        long    vert2d[4][2];           /*2D 4 vertices(limited)*/
        long    vert2d_1[4][2];         /*2D 4 vertices(non limited)*/
        int     vflg[4];

        int     DDAflag;
	VECTOR  sca;


        /*get 2D vertex for draw range*/
        gte_ldv0(&vertex[0]);
        gte_rt();
        gte_stlvnl(&vert3d[0]);
        gte_ldv0(&vertex[1]);
        gte_rt();
        gte_stlvnl(&vert3d[1]);
        gte_ldv0(&vertex[2]);
        gte_rt();
        gte_stlvnl(&vert3d[2]);
        gte_ldv0(&vertex[3]);
        gte_rt();
        gte_stlvnl(&vert3d[3]);

        /*clip view point backward polygon*/
        if(vert3d[0].vz<=0&&
           vert3d[1].vz<=0&&
           vert3d[2].vz<=0&&
           vert3d[3].vz<=0) return;

        /*clear vflg*/
        vflg[0]=0;
        vflg[1]=0;
        vflg[2]=0;
        vflg[3]=0;

        /*save GTE parameters*/
        gte_ReadRotMatrix(&SWmat);
        gte_ReadGeomOffset(&ofx,&ofy);
        gte_ReadGeomScreen(&scrn);

        /*define polygon matrix as parallelogram*/
        pmat.m[0][0]= vert3d[1].vx - vert3d[0].vx;
        pmat.m[1][0]= vert3d[1].vy - vert3d[0].vy;
        pmat.m[2][0]= vert3d[1].vz - vert3d[0].vz;
        pmat.m[0][1]= vert3d[3].vx - vert3d[0].vx;
        pmat.m[1][1]= vert3d[3].vy - vert3d[0].vy;
        pmat.m[2][1]= vert3d[3].vz - vert3d[0].vz;
        pmat.m[0][2]= vert3d[0].vx;
        pmat.m[1][2]= vert3d[0].vy;
        pmat.m[2][2]= vert3d[0].vz;

        /*if(Z==0) limit Z-value to NEARZ*/
        if(vert3d[0].vz==0)vert3d[0].vz=NEARZ;
        if(vert3d[1].vz==0)vert3d[1].vz=NEARZ;
        if(vert3d[2].vz==0)vert3d[2].vz=NEARZ;
        if(vert3d[3].vz==0)vert3d[3].vz=NEARZ;

        /*perspective for collect slant calculation*/
        pers(&vert3d[0],vert2d_1[0]);
        pers(&vert3d[1],vert2d_1[1]);
        pers(&vert3d[2],vert2d_1[2]);
        pers(&vert3d[3],vert2d_1[3]);

        /*if(Z<0) limit Z-value to NEARZ and vflg=ON*/
        if(vert3d[0].vz<NEARZ){vert3d[0].vz=NEARZ; vflg[0]=1;}
        if(vert3d[1].vz<NEARZ){vert3d[1].vz=NEARZ; vflg[1]=1;}
        if(vert3d[2].vz<NEARZ){vert3d[2].vz=NEARZ; vflg[2]=1;}
        if(vert3d[3].vz<NEARZ){vert3d[3].vz=NEARZ; vflg[3]=1;}

        /*perspective for limitted DDA*/
        pers(&vert3d[0],vert2d[0]);
        pers(&vert3d[1],vert2d[1]);
        pers(&vert3d[2],vert2d[2]);
        pers(&vert3d[3],vert2d[3]);

        /*get minmax of Quad. */
        get_minmax_quad(vert2d,min,max);

        /*if Quad. is out of screen, return*/
        if(max[0]<0||max[1]<0||min[0]>=PIH||min[1]>=PIV){
                gte_SetGeomOffset(ofx,ofy);
                gte_SetGeomScreen(scrn);
                gte_SetRotMatrix(&SWmat);
                gte_SetTransMatrix(&SWmat);
                return;
        }

        /*get inverse matrix*/
        inv_mat(&pmat,iparray);

        /*limit matrix for GTE calculation*/
        limit_mat(iparray,&ipmat);
        ipmat.t[0]= 0;
        ipmat.t[1]= 0;
        ipmat.t[2]= 0;

        /*scale vector set*/
        sca.vx= 4096*txs0/MAXTSIZE;
        sca.vy= 4096*txs1/MAXTSIZE;
        sca.vz= 4096;

        /*scale ipmat*/
        ScaleMatrixL(&ipmat,&sca);

        /*GTE set for inverse perspective*/
        gte_SetRotMatrix(&ipmat);
        gte_SetTransMatrix(&ipmat);
        gte_SetGeomOffset(0,0);


        /*get slant of each line                        */
        /* if(vflg0==0)edge01->fslt0,else edge10->fslt0 */
        /* if(vflg1==0)edge12->fslt1,else edge21->fslt1 */
        /* if(vflg2==0)edge23->fslt2,else edge32->fslt2 */
        /* if(vflg3==0)edge30->fslt3,else edge03->fslt3 */
        get_fslant_edge(vert2d_1,vflg,fslt);

        if(min[1]>=0){
                DDAflag=0;      /*if Y-min is in screen, then up to down*/
        }else{
                if(max[1]<=PIV){        /*if Y-min is out of screen &   */
                        DDAflag=1;      /*Y-max is in screen then down to up*/
                }else{
                        if(abs(min[1])<=abs(max[1]-PIV)){
                                        /*if Y-min & Y-max are both out */
                                        /*of screen and Y-min is nearer */
                                DDAflag=0;      /*then up to down*/
                        }else{
                                        /*if Y-min & Y-max are both out */
                                        /*of screen and Y-max is nearer */
                                DDAflag=1;      /*then down to up*/
                        }
                }
        }


        draw_down2(vert2d,fslt,min,max,ot,s,divn,txs0,txs1);

        SetGeomOffset(ofx,ofy);
        SetGeomScreen(scrn);
        SetRotMatrix(&SWmat);
        SetTransMatrix(&SWmat);
}

/************************************
* draw FLOOR square from up to down *
*	multi lines		    *
************************************/
draw_down2(vert2d,fslt,min,max,ot,packet,divn,txs0,txs1)
long    vert2d[4][2];
int     fslt[4];                /*strict slant (1,19,12)*/
int     min[2],max[2];
u_long	*ot;
POLY_FT4      *packet;
int	divn;
int	txs0,txs1;
{
        int     i,j;
	int	ln;
	int	stn,edn;
	long	next_S,next_E;

	long	fract;
	long	old_ln;

	POLY_FT4	*s;

	DDA	*SC;		/*ScratchPad pointer*/

	SC= (DDA*)getScratchAddr(0);

	s= packet;

        /*sort vertex*/
        sort_vertex_left(vert2d,SC->SVn);
        sort_vertex_right(vert2d,SC->EVn);

        for(i=0;i<4;i++){                       /*set edge slant*/
                SC->SVf[i]= fslt[(SC->SVn[i]+3)%4];
                SC->EVf[i]= fslt[SC->EVn[i]];
                SC->SVa[i]= vert2d[SC->SVn[i]][1];
                SC->EVa[i]= vert2d[SC->EVn[i]][1];
        }


        SC->fstart_x= vert2d[SC->SVn[0]][0]<<12;        /*initial value*/
        SC->fend_x= vert2d[SC->EVn[0]][0]<<12;

        SC->sv0.vz= ReadGeomScreen()*16;
        SC->sv1.vz= ReadGeomScreen()*16;
	SetGeomScreen(MAXTSIZE);

	SC->old_start_x= vert2d[SC->SVn[0]][0];
	SC->old_end_x= vert2d[SC->EVn[0]][0];

	switch(SC->SVn[0]){
	case 0:	
		SC->old_start_u= 0;
		SC->old_start_v= 0;
		SC->old_end_u= 0;
		SC->old_end_v= 0;
		break;
	case 1:
		SC->old_start_u= txs0;
		SC->old_start_v= 0;
		SC->old_end_u= txs0;
		SC->old_end_v= 0;
		break;
	case 2:
		SC->old_start_u= txs0;
		SC->old_start_v= txs1;
		SC->old_end_v= txs0;
		SC->old_end_v= txs1;
		break;
	case 3:
		SC->old_start_u= 0;
		SC->old_start_v= txs1;
		SC->old_end_u= 0;
		SC->old_end_v= txs1;
		break;
	default:
		break;
	}

	stn=0;
	edn=0;
	old_ln=min[1];
	ln=min[1];
	
	while(ln<=max[1]&&ln<PIV){

            if(ln>= 0){
                SC->istart_x= SC->fstart_x>>12;         /*(1,19,12)->(1,19,0)*/
                SC->iend_x= SC->fend_x>>12;

		/********************************************************/
                /*avoid GPU clipping                                    */
                /*if abs(istart_x-iend_x)<1024 && abs(istart_x)<1000    */
                /*&& abs(iend_x)<1000 then you can comment out the      */
                /*fllowing 2 lines. It will make perspective pixels     */
                /*shape at near viewpoint.                              */
		/*If widen the clipping area to (PIH/2-500,PIH/2+500)   */
		/*drawing time increase                                 */
		/********************************************************/
                if(SC->istart_x<0) SC->istart_x= 0;
                if(SC->iend_x>PIH) SC->iend_x=PIH;

                if(SC->iend_x>= 0 && SC->istart_x<=PIH){

                  /*set screen address to SVECTOR*/
                  SC->sv0.vx= (SC->istart_x - (PIH/2))*16;
                  SC->sv0.vy= (ln - (PIV/2))*16;
                  SC->sv1.vx= (SC->iend_x - (PIH/2))*16;
                  SC->sv1.vy= (ln - (PIV/2))*16;

                  gte_ldv0(&SC->sv0);      /*load only 2 screen address*/
                  gte_ldv1(&SC->sv1);
                  gte_rtpt();                   /*RotTransPers3*/

                  s->x0= SC->old_start_x;        /*GTE delay slot*/
                  s->y0= old_ln;
                  s->x1= SC->old_end_x;
                  s->y1= old_ln;
                  s->x2= SC->istart_x;
                  s->y2= ln;
                  s->x3= SC->iend_x;
                  s->y3= ln;

                  gte_stsxy0((long*)SC->uv[0]);     /*store 2 texture address*/
                  gte_stsxy1((long*)SC->uv[1]);

                  for(j=0;j<2;j++){             /*texture address limiter*/
                    if(SC->uv[j][0]<0)SC->uv[j][0]=0;
                    if(SC->uv[j][0]>255)SC->uv[j][0]=255;
                    if(SC->uv[j][1]<0)SC->uv[j][1]=0;
                    if(SC->uv[j][1]>255)SC->uv[j][1]=255;
                  }
		  /*set UV-address to packet*/
                  s->u0= SC->old_start_u;
                  s->v0= SC->old_start_v;
                  s->u1= SC->old_end_u;
                  s->v1= SC->old_end_v;
                  s->u2= SC->old_start_u= SC->uv[0][0];
                  s->v2= SC->old_start_v= SC->uv[0][1];
                  s->u3= SC->old_end_u= SC->uv[1][0];
                  s->v3= SC->old_end_v= SC->uv[1][1];

                  addPrim(ot,s);

	    	  SC->old_start_x= SC->istart_x;
	    	  SC->old_end_x= SC->iend_x;
	    	  old_ln= ln;

	    	  s++;
                }
            }

	    next_S= SC->SVa[stn+1]- ln;
	    next_E= SC->EVa[edn+1]- ln;

	    if(next_S>divn&&next_E>divn){
	    	SC->fstart_x += SC->SVf[stn]*divn;
	    	SC->fend_x += SC->EVf[edn]*divn;
	    	ln += divn;
	    }else{
	    	if(next_S==next_E){
		    stn++;
		    edn++;
		    SC->fstart_x = vert2d[SC->SVn[stn]][0]<<12;
		    SC->fend_x = vert2d[SC->EVn[edn]][0]<<12;
		    ln = SC->SVa[stn];
	    	}else{
		    if(next_S<next_E){
		    	fract= next_S*4096/divn;
		    	stn++;
		    	SC->fstart_x = vert2d[SC->SVn[stn]][0]<<12;
		    	SC->fend_x += ((SC->EVf[edn]*divn)*fract)>>12;
		    	ln = SC->SVa[stn];
		    }else{
		     	fract= next_E*4096/divn;
		    	edn++;
	    	    	SC->fstart_x += ((SC->SVf[stn]*divn)*fract)>>12;
		    	SC->fend_x = vert2d[SC->EVn[edn]][0]<<12;
		    	ln = SC->EVa[edn];
		    }
	    	}
	    }


            if(SC->fstart_x>SC->fend_x) SC->fstart_x=SC->fend_x;
        }
}

/*
 * Draw Square Image
 */
draw_ground_square(vertex,ot,s,txs0,txs1)
SVECTOR *vertex;                /*3D 4 vertices*/
u_long	*ot;
POLY_FT3      *s;
int	txs0,txs1;
{
        int     i,j;
        long     min[2],max[2];
        int     fslt[4];
        long    flg;
        VECTOR  vert3d[4];
        MATRIX  pmat;
        long    iparray[3][3];
        MATRIX  ipmat;
        long    ofx,ofy;
        MATRIX  SWmat;
        long    scrn;
        long    vert2d[4][2];           /*2D 4 vertices(limited)*/
        long    vert2d_1[4][2];         /*2D 4 vertices(non limited)*/
        int     vflg[4];

        int     DDAflag;
	VECTOR	sca;
	int	count;

        /*get 2D vertex for draw range*/
	gte_ldv0(&vertex[0]);
	gte_rt();
	gte_stlvnl(&vert3d[0]);
	gte_ldv0(&vertex[1]);
	gte_rt();
	gte_stlvnl(&vert3d[1]);
	gte_ldv0(&vertex[2]);
	gte_rt();
	gte_stlvnl(&vert3d[2]);
	gte_ldv0(&vertex[3]);
	gte_rt();
	gte_stlvnl(&vert3d[3]);

        /*clip view point backward polygon*/
        if(vert3d[0].vz<=0&&
           vert3d[1].vz<=0&&
           vert3d[2].vz<=0&&
           vert3d[3].vz<=0) return;

        /*clear vflg*/
        vflg[0]=0;
        vflg[1]=0;
        vflg[2]=0;
        vflg[3]=0;

        /*save GTE parameters*/
        gte_ReadRotMatrix(&SWmat);
        gte_ReadGeomOffset(&ofx,&ofy);
        gte_ReadGeomScreen(&scrn);

        /*define polygon matrix as parallelogram*/
        pmat.m[0][0]= vert3d[1].vx - vert3d[0].vx;
        pmat.m[1][0]= vert3d[1].vy - vert3d[0].vy;
        pmat.m[2][0]= vert3d[1].vz - vert3d[0].vz;
        pmat.m[0][1]= vert3d[3].vx - vert3d[0].vx;
        pmat.m[1][1]= vert3d[3].vy - vert3d[0].vy;
        pmat.m[2][1]= vert3d[3].vz - vert3d[0].vz;
        pmat.m[0][2]= vert3d[0].vx;
        pmat.m[1][2]= vert3d[0].vy;
        pmat.m[2][2]= vert3d[0].vz;

        /*if(Z==0) limit Z-value to NEARZ*/
        if(vert3d[0].vz==0)vert3d[0].vz=NEARZ;
        if(vert3d[1].vz==0)vert3d[1].vz=NEARZ;
        if(vert3d[2].vz==0)vert3d[2].vz=NEARZ;
        if(vert3d[3].vz==0)vert3d[3].vz=NEARZ;

        /*perspective for collect slant calculation*/
        pers(&vert3d[0],vert2d_1[0]);
        pers(&vert3d[1],vert2d_1[1]);
        pers(&vert3d[2],vert2d_1[2]);
        pers(&vert3d[3],vert2d_1[3]);

        /*if(Z<0) limit Z-value to NEARZ and vflg=ON*/
        if(vert3d[0].vz<NEARZ){vert3d[0].vz=NEARZ; vflg[0]=1;}
        if(vert3d[1].vz<NEARZ){vert3d[1].vz=NEARZ; vflg[1]=1;}
        if(vert3d[2].vz<NEARZ){vert3d[2].vz=NEARZ; vflg[2]=1;}
        if(vert3d[3].vz<NEARZ){vert3d[3].vz=NEARZ; vflg[3]=1;}

        /*perspective for limitted DDA*/
        pers(&vert3d[0],vert2d[0]);
        pers(&vert3d[1],vert2d[1]);
        pers(&vert3d[2],vert2d[2]);
        pers(&vert3d[3],vert2d[3]);

        /*get minmax of Quad. */
        get_minmax_quad(vert2d,min,max);

        /*if Quad. is out of screen, return*/
        if(max[0]<0||max[1]<0||min[0]>=PIH||min[1]>=PIV){
                gte_SetGeomOffset(ofx,ofy);
                gte_SetGeomScreen(scrn);
                gte_SetRotMatrix(&SWmat);
                gte_SetTransMatrix(&SWmat);
                return;
        }

        /*get inverse matrix*/
        inv_mat(&pmat,iparray);

        /*limit matrix for GTE calculation*/
        limit_mat(iparray,&ipmat);
        ipmat.t[0]= 0;
        ipmat.t[1]= 0;
        ipmat.t[2]= 0;

	/*scale vector set*/
	sca.vx= 4096*txs0/MAXTSIZE;
	sca.vy= 4096*txs1/MAXTSIZE;
	sca.vz= 4096;

	/*scale ipmat*/
	ScaleMatrixL(&ipmat,&sca);

        /*GTE set for inverse perspective*/
        gte_SetRotMatrix(&ipmat);
        gte_SetTransMatrix(&ipmat);
        gte_SetGeomOffset(0,0);


        /*get slant of each line                        */
        /* if(vflg0==0)edge01->fslt0,else edge10->fslt0 */
        /* if(vflg1==0)edge12->fslt1,else edge21->fslt1 */
        /* if(vflg2==0)edge23->fslt2,else edge32->fslt2 */
        /* if(vflg3==0)edge30->fslt3,else edge03->fslt3 */
        get_fslant_edge(vert2d_1,vflg,fslt);


        if(min[1]>=0){
                DDAflag=0;      /*if Y-min is in screen, then up to down*/
        }else{
                if(max[1]<=PIV){        /*if Y-min is out of screen &   */
                        DDAflag=1;      /*Y-max is in screen then down to up*/
                }else{
                        if(abs(min[1])<=abs(max[1]-PIV)){
                                        /*if Y-min & Y-max are both out */
                                        /*of screen and Y-min is nearer */
                                DDAflag=0;      /*then up to down*/
                        }else{
                                        /*if Y-min & Y-max are both out */
                                        /*of screen and Y-max is nearer */
                                DDAflag=1;      /*then down to up*/
                        }
                }
        }



        if(DDAflag==0)
                draw_down(vert2d,fslt,min,max,ot,s,txs0,txs1);
        else
                draw_up(vert2d,fslt,min,max,ot,s,txs0,txs1);

        gte_SetGeomOffset(ofx,ofy);
        gte_SetGeomScreen(scrn);
        gte_SetRotMatrix(&SWmat);
        gte_SetTransMatrix(&SWmat);
}


/************************************
* draw FLOOR square from up to down *
* 1 line divide only		    *
************************************/
draw_down(vert2d,fslt,min,max,ot,packet,txs0,txs1)
long    vert2d[4][2];		/*2D vertex		*/
int     fslt[4];                /*strict slant (1,19,12)*/
int     min[2],max[2];		/*min,max of 2D vertex	*/
u_long	*ot;			/*OT header		*/
POLY_FT3      *packet;		/*packet header		*/
int	txs0,txs1;		/*texture size		*/
{
        int     i;
	int	ln;		/*line number		*/
	int	stn,edn;	/*edge number		*/
	long	next_S,next_E;	/*line number to next vertex	*/
	POLY_FT3	*s;	/*packet pointer	*/
        int     activeBuff;	/*buffer ID		*/

	DDA	*SC;		/*ScratchPad pointer*/

	SC= (DDA*)getScratchAddr(0);

	s= packet;
        activeBuff= GetODE();                   /*get Odd or Even field*/

        /*sort vertex*/
        sort_vertex_left(vert2d,SC->SVn);
        sort_vertex_right(vert2d,SC->EVn);

        for(i=0;i<4;i++){                       /*set edge slant*/
                SC->SVf[i]= fslt[(SC->SVn[i]+3)%4];
                SC->EVf[i]= fslt[SC->EVn[i]];
                SC->SVa[i]= vert2d[SC->SVn[i]][1];
                SC->EVa[i]= vert2d[SC->EVn[i]][1];
        }


        SC->fstart_x= vert2d[SC->SVn[0]][0]<<12;        /*initial value*/
        SC->fend_x= vert2d[SC->EVn[0]][0]<<12;

        SC->sv0.vz= ReadGeomScreen()*16;
        SC->sv1.vz= ReadGeomScreen()*16;
	SetGeomScreen(MAXTSIZE);


	stn=0;
	edn=0;
	ln=min[1];

	while(ln<=max[1]&&ln<PIV){

#ifdef INTERLACE
            if(ln>=0&&((ln%2)==activeBuff)){
#else
            if(ln>= 0){
#endif

                SC->istart_x= SC->fstart_x>>12;         /*(1,19,12)->(1,19,0)*/
                SC->iend_x= SC->fend_x>>12;

		/********************************************************/
                /*avoid GPU clipping                                    */
                /*if abs(istart_x-iend_x)<1024 && abs(istart_x)<1000    */
                /*&& abs(iend_x)<1000 then you can comment out the      */
                /*fllowing 2 lines. It will make perspective pixels     */
                /*shape at near viewpoint.                              */
		/*If widen the clipping area to (PIH/2-500,PIH/2+500)	*/
		/*drawing time increase					*/
		/********************************************************/
                if(SC->istart_x<0) SC->istart_x= 0;
                if(SC->iend_x>PIH) SC->iend_x= PIH;

                if(SC->iend_x>= 0 && SC->istart_x<=PIH){

                  /*set screen address to SVECTOR*/
                  SC->sv0.vx= (SC->istart_x - (PIH/2))*16;
                  SC->sv0.vy= (ln - (PIV/2))*16;
                  SC->sv1.vx= (SC->iend_x - (PIH/2))*16;
                  SC->sv1.vy= (ln - (PIV/2))*16;

                  gte_ldv0(&SC->sv0);	/*load only 2 screen address*/
                  gte_ldv1(&SC->sv1);
                  gte_rtpt();                   /*RotTransPers3*/

                  s->x0= SC->istart_x;        /*GTE delay slot*/
                  s->y0= ln;
                  s->x1= SC->iend_x;
                  s->y1= ln;
                  s->x2= SC->istart_x;
                  s->y2= ln+1;

                  gte_stsxy0((long*)SC->uv[0]);   /*store 2 texture address*/
                  gte_stsxy1((long*)SC->uv[1]);

                  /*texture address limiter*/
		  /*MAXTSIZE limiter by GTE SXY limiter*/
                  if(SC->uv[0][0]<0)SC->uv[0][0]=0;	
                  if(SC->uv[0][1]<0)SC->uv[0][1]=0;
                  if(SC->uv[1][0]<0)SC->uv[1][0]=0;
                  if(SC->uv[1][1]<0)SC->uv[1][1]=0;
                  if(SC->uv[0][0]>255)SC->uv[0][0]=255;	
                  if(SC->uv[0][1]>255)SC->uv[0][1]=255;
                  if(SC->uv[1][0]>255)SC->uv[1][0]=255;
                  if(SC->uv[1][1]>255)SC->uv[1][1]=255;

		  /*set UV-address to packet*/
                  s->u0= SC->uv[0][0];
                  s->v0= SC->uv[0][1];
                  s->u1= SC->uv[1][0];
                  s->v1= SC->uv[1][1];

                  addPrim(ot,s);

		  s++;

                }
            }

	    next_S= SC->SVa[stn+1]- ln;
	    next_E= SC->EVa[edn+1]- ln;

	    if(next_S>1&&next_E>1){

	    	SC->fstart_x += SC->SVf[stn];
	    	SC->fend_x += SC->EVf[edn];
	    }else{
	    	if(next_S==next_E){
		    stn++;
		    edn++;
		    SC->fstart_x = vert2d[SC->SVn[stn]][0]<<12;
		    SC->fend_x = vert2d[SC->EVn[edn]][0]<<12;
	    	}else{
		    if(next_S<next_E){
		    	stn++;
		    	SC->fstart_x = vert2d[SC->SVn[stn]][0]<<12;
		    	SC->fend_x += SC->EVf[edn];
		    }else{
		    	edn++;
	    	    	SC->fstart_x += SC->SVf[stn];
		    	SC->fend_x = vert2d[SC->EVn[edn]][0]<<12;
		    }
	    	}
	    }
            if(SC->fstart_x>SC->fend_x) SC->fstart_x=SC->fend_x;
	    ln++;
        }
}


/**************************************
* draw CEILING square from down to up *
* 1 line divide only		      *
**************************************/
draw_up(vert2d,fslt,min,max,ot,packet,txs0,txs1)
long    vert2d[4][2];		/*2D vertex             */
int     fslt[4];                /*strict slant (1,19,12)*/
int     min[2],max[2];		/*min,max of 2D vertex  */
u_long	*ot;			/*OT header             */
POLY_FT3      *packet;		/*packet header         */
int     txs0,txs1;		/*texture size          */
{
        int     i;
	int     ln;		/*line number           */
	int     stn,edn;	/*edge number           */
	long    next_S,next_E;	/*line number to next vertex    */
	POLY_FT3        *s;	/*packet pointer        */
        int     activeBuff;	/*buffer ID             */

	DDA     *SC;		/*ScratchPad pointer*/


	SC= (DDA*)getScratchAddr(0);

	s= packet;
        activeBuff= GetODE();                   /*get Odd or Even field*/

        /*sort vertex*/
        sort_vertex_left_up(vert2d,SC->SVn);
        sort_vertex_right_up(vert2d,SC->EVn);

        for(i=0;i<4;i++){                       /*set edge slant*/
                SC->SVf[i]= -fslt[SC->SVn[i]];
                SC->EVf[i]= -fslt[(SC->EVn[i]+3)%4];
                SC->SVa[i]= vert2d[SC->SVn[i]][1];
                SC->EVa[i]= vert2d[SC->EVn[i]][1];
        }
        SC->fstart_x= vert2d[SC->SVn[0]][0]<<12;        /*initial value*/
        SC->fend_x= vert2d[SC->EVn[0]][0]<<12;

        SC->sv0.vz= ReadGeomScreen()*16;	/*preset Projection to Z*/
        SC->sv1.vz= ReadGeomScreen()*16;
	SetGeomScreen(MAXTSIZE);

	stn=0;
	edn=0;
	ln=max[1];

	while(ln>=min[1]&&ln>0){
#ifdef INTERLACE
            if(ln<PIV&&((ln%2)==activeBuff)){
#else
            if(ln<PIV){
#endif
                SC->istart_x= SC->fstart_x>>12;	/*(1,19,12)->(1,19,0)*/
                SC->iend_x= SC->fend_x>>12;

		/********************************************************/
                /*avoid GPU clipping                                    */
                /*if abs(istart_x-iend_x)<1024 && abs(istart_x)<1000    */
                /*&& abs(iend_x)<1000 then you can comment out the      */
                /*fllowing 2 lines. It will make perspective pixels     */
                /*shape at near viewpoint.                              */
		/*If widen the clipping area to (PIH/2-500,PIH/2+500)   */
		/*drawing time increase                                 */
		/********************************************************/
                if(SC->istart_x<0) SC->istart_x=0;
                if(SC->iend_x>PIH) SC->iend_x=PIH;

                if(SC->iend_x>=0&&SC->istart_x<=PIH){

                  /*set screen address to SVECTOR*/
                  SC->sv0.vx= (SC->istart_x - (PIH/2))*16;
                  SC->sv0.vy= (ln - (PIV/2))*16;
                  SC->sv1.vx= (SC->iend_x - (PIH/2))*16;
                  SC->sv1.vy= (ln - (PIV/2))*16;

                  gte_ldv0(&SC->sv0);	/*load only 2 screen address*/
                  gte_ldv1(&SC->sv1);
                  gte_rtpt();		/*RotTransPers3*/

                  s->x0= SC->istart_x;	/*GTE delay slot*/
                  s->y0= ln;
                  s->x1= SC->iend_x;
                  s->y1= ln;
                  s->x2= SC->istart_x;
                  s->y2= ln+1;

                  gte_stsxy0((long*)SC->uv[0]);/*store 2 texture address*/
                  gte_stsxy1((long*)SC->uv[1]);

		  /*texture address limiter*/
		  /*MAXTSIZE limiter by GTE SXY limiter*/
		  if(SC->uv[0][0]<0)SC->uv[0][0]=0;
		  if(SC->uv[0][1]<0)SC->uv[0][1]=0;
		  if(SC->uv[1][0]<0)SC->uv[1][0]=0;
		  if(SC->uv[1][1]<0)SC->uv[1][1]=0;
		  if(SC->uv[0][0]>255)SC->uv[0][0]=255;
		  if(SC->uv[0][1]>255)SC->uv[0][1]=255;
		  if(SC->uv[1][0]>255)SC->uv[1][0]=255;
		  if(SC->uv[1][1]>255)SC->uv[1][1]=255;


		  /*set UV-address to packet*/
                  s->u0= SC->uv[0][0];
                  s->v0= SC->uv[0][1];
                  s->u1= SC->uv[1][0];
                  s->v1= SC->uv[1][1];

                  addPrim(ot,s);

		  s++;

                }
            }
            next_S= ln - SC->SVa[stn+1];
            next_E= ln - SC->EVa[edn+1];

            if(next_S>1&&next_E>1){
                SC->fstart_x += SC->SVf[stn];
                SC->fend_x += SC->EVf[edn];
            }else{
                if(next_S==next_E){
                    stn++;
                    edn++;
                    SC->fstart_x = vert2d[SC->SVn[stn]][0]<<12;
                    SC->fend_x = vert2d[SC->EVn[edn]][0]<<12;
                }else{
                    if(next_S<next_E){
                        stn++;
                        SC->fstart_x = vert2d[SC->SVn[stn]][0]<<12;
                        SC->fend_x += SC->EVf[edn];
                    }else{
                        edn++;
                        SC->fstart_x += SC->SVf[stn];
                        SC->fend_x = vert2d[SC->EVn[edn]][0]<<12;
                    }
                }
            }
            if(SC->fstart_x>SC->fend_x) SC->fstart_x=SC->fend_x;
            ln--;

        }
}

static void get_next_ends_X(
		ln,stn,edn,vert2d,SVn,SVa,SVf,EVn,EVa,EVf,n,fstart_x,fend_x)
int	*ln;		/*scanline number*/
int	*stn,*edn;	/*scan line position of start edge & end edge*/
long	vert2d[4][2];	/*2D vertices*/
int	SVn[4];		/*start vertex number*/
long	SVa[4];		/*start vertex scanline V-address*/
long	SVf[4];		/*start vertex fslant*/
int	EVn[4];		/*end vertex number*/
long	EVa[4];		/*end vertex scanline V-address*/
long	EVf[4];		/*end vertex fslant*/
int	n;
long	*fstart_x;	/*(1,19,12)format start address*/
long	*fend_x;	/*(1,19,12)format end address*/
{
	long	next_S,next_E;
	long	fract;
	long	pitch;

/*
printf("stn,edn,ln=(%d,%d),%d\n",*stn,*edn,*ln);
printf("fstart_x=%d\n",fstart_x);
printf("fend_x=%d\n",fend_x);
*/

	pitch= n;

	next_S= SVa[*stn+1]- (*ln);
	next_E= EVa[*edn+1]- (*ln);

	if(next_S>pitch&&next_E>pitch){
	    *fstart_x += SVf[*stn];
	    *fend_x += EVf[*edn];
	    (*ln) += pitch;
	}else{
	    if(next_S==next_E){
		(*stn)++;
		(*edn)++;
		*fstart_x = vert2d[SVn[*stn]][0]<<12;
		*fend_x = vert2d[EVn[*edn]][0]<<12;
		(*ln) = SVa[*stn];
	    }else{
		if(next_S<next_E){
		    fract= next_S*4096/n;
		    (*stn)++;
		    *fstart_x = vert2d[SVn[*stn]][0]<<12;
		    *fend_x += (EVf[*edn]*fract)>>12;
		    (*ln) = SVa[*stn];
		}else{
		    fract= next_E*4096/n;
		    (*edn)++;
	    	    *fstart_x += (SVf[*stn]*fract)>>12;
		    *fend_x = vert2d[EVn[*edn]][0]<<12;
		    (*ln) = EVa[*edn];
		}
	    }
	}

}


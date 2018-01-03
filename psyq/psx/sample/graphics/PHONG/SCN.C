/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *   Phong Shading program
 *
 *   This program includes the function phong_tri, which performs Phong
 *   shading on one triangle. "Triangle color", "normal vectors of vertices", 
*    and "screen coordinates of vertices" are provided to phong_tri, which
 *   then performs Phong shading and renders the triangle.
 *   Since phong_tri renders directly to video memory, Z-sorting with other
 *   polygons requires rendering and re-texturing in a non-display area.
 *
 *        1995,3,29 by Oka
 *        Copyright  (C)  1995 by Sony Computer Entertainment
 *             All rights Reserved */

#include	<sys/types.h>
#include        <libetc.h>
#include        <libgte.h>
#include        <libgpu.h>

/*#define EQUIVALENT*/

#define PIH	320
#define PIV	240

#define	DITHER	1

int	DITH[4][4]= {{0,8,2,10},{12,4,14,6},{3,11,1,9},{15,7,13,5}};

extern	MATRIX lmat;
extern	MATRIX cmat;

static min3();
static max3();


static get_minmax_tri(v,min,max)
short  	v[3][2];
int     min[2],max[2];
{
        min[0]=min3(v[0][0],v[1][0],v[2][0]);
        min[1]=min3(v[0][1],v[1][1],v[2][1]);
        max[0]=max3(v[0][0],v[1][0],v[2][0]);
        max[1]=max3(v[0][1],v[1][1],v[2][1]);
}

static min3(a,b,c)
int   	a,b,c;
{
        int min;
        min=a;
        if(b<min)min=b;
        if(c<min)min=c;
        return(min);
}

static max3(a,b,c)
short   a,b,c;
{
        int max;
        max=a;
        if(b>max)max=b;
        if(c>max)max=c;
        return(max);
}

/*sort 3 vertices by Y-coordinates*/
/*generate border vertex number for left&right border*/
/*from minimum vertex to maximum vertex*/
static sort_vertex(v,bd1,bd2)
short  	v[3][2];	/*screen coordinates of 3 vertices of tri.*/
int     bd1[3];		/*left border vertex number(max 3 vertices)*/
int	bd2[3];		/*right border vertex number(max 3 vertices)*/
{
        int     i;
	int	f0,f1,f2;	/*flags of vertex (min=0,max=1,otherwise=2)*/
	int	flag;		/*flag= (f0<<4)+(f1<<2)+(f2)*/

        for(i=0;i<3;i++){ 
		bd1[i]= -1;
		bd2[i]= -1;
	}

	/*if(V0:min->f0=0,V0:max->f0=1,else:f0=2*/
	if((v[0][1]<=v[1][1])&&(v[0][1]<=v[2][1]))f0=0;
	else if((v[0][1]>=v[1][1])&&(v[0][1]>=v[2][1]))f0=1;
	     else f0=2;

	/*if(V1:min->f1=0,V1:max->f1=1,else:f1=2*/
	if((v[1][1]<=v[0][1])&&(v[1][1]<=v[2][1]))f1=0;
	else if((v[1][1]>=v[0][1])&&(v[1][1]>=v[2][1]))f1=1;
	     else f1=2;

	/*if(V2:min->f2=0,V2:max->f2=1,else:f2=2*/
	if((v[2][1]<=v[0][1])&&(v[2][1]<=v[1][1]))f2=0;
	else if((v[2][1]>=v[0][1])&&(v[2][1]>=v[1][1]))f2=1;
	     else f2=2;

	flag= (f0<<4)+(f1<<2)+(f2);

	switch(flag){
	case (0<<4)+(0<<2)+(0):		/*no draw*/
		break;
	case (0<<4)+(0<<2)+(1):		
		bd1[0]=0; bd1[1]=2;
		bd2[0]=1; bd2[1]=2;
		break;
	case (0<<4)+(0<<2)+(2):		/*impossible*/
		break;
	case (0<<4)+(1<<2)+(0):		
		bd1[0]=2; bd1[1]=1;
		bd2[0]=0; bd2[1]=1;
		break;
	case (0<<4)+(1<<2)+(1):
		bd1[0]=0; bd1[1]=1;
		bd2[0]=0; bd2[1]=2;
		break;
	case (0<<4)+(1<<2)+(2):		
		bd1[0]=0; bd1[1]=2; bd1[2]=1;
		bd2[0]=0; bd2[1]=1;
		break;
	case (0<<4)+(2<<2)+(0):		/*impossible*/
		break;
	case (0<<4)+(2<<2)+(1):
		bd1[0]=0; bd1[1]=2;
		bd2[0]=0; bd2[1]=1; bd2[2]=2;
		break;
	case (0<<4)+(2<<2)+(2):		/*impossible*/
		break;
	case (1<<4)+(0<<2)+(0):
		bd1[0]=1; bd1[1]=0;
		bd2[0]=2; bd2[1]=0;
		break;
	case (1<<4)+(0<<2)+(1):
		bd1[0]=1; bd1[1]=0;
		bd2[0]=1; bd2[1]=2;
		break;
	case (1<<4)+(0<<2)+(2):
		bd1[0]=1; bd1[1]=0;
		bd2[0]=1; bd2[1]=2; bd2[2]=0;
		break;
	case (1<<4)+(1<<2)+(0):
		bd1[0]=2; bd1[1]=1;
		bd2[0]=2; bd2[1]=0;
		break;
	case (1<<4)+(1<<2)+(1):		/*impossible*/
		break;
	case (1<<4)+(1<<2)+(2):		/*impossible*/
		break;
	case (1<<4)+(2<<2)+(0):
		bd1[0]=2; bd1[1]=1; bd1[2]=0;
		bd2[0]=2; bd2[1]=0;
		break;
	case (1<<4)+(2<<2)+(1):		/*impossible*/
		break;
	case (1<<4)+(2<<2)+(2):		/*impossible*/
		break;
	case (2<<4)+(0<<2)+(0):		/*impossible*/
		break;
	case (2<<4)+(0<<2)+(1):
		bd1[0]=1; bd1[1]=0; bd1[2]=2;
		bd2[0]=1; bd2[1]=2;
		break;
	case (2<<4)+(0<<2)+(2):		/*impossible*/
		break;
	case (2<<4)+(1<<2)+(0):
		bd1[0]=2; bd1[1]=1;
		bd2[0]=2; bd2[1]=0; bd2[2]=1;
		break;
	case (2<<4)+(1<<2)+(1):		/*impossible*/
		break;
	case (2<<4)+(1<<2)+(2):		/*impossible*/
		break;
	case (2<<4)+(2<<2)+(0):		/*impossible*/
		break;
	case (2<<4)+(2<<2)+(1):		/*impossible*/
		break;
	case (2<<4)+(2<<2)+(2):		/*impossible*/
		break;
	default:
		break;
	}
}

/* return strict slant by (1,19,12)format*/
static get_fslant(v1,v2)
short   v1[2],v2[2];
{
        if(v1[1]==v2[1]) return(0x7fffffff);
        else  return((long)(4096*(v2[0]-v1[0]))/(v2[1]-v1[1]));
}

/* get strict slant of four edge*/
static get_fslant_edge(v,s)
short   v[3][2];
int     s[3];
{
        s[0]= get_fslant(v[0],v[1]);
        s[1]= get_fslant(v[1],v[2]);
        s[2]= get_fslant(v[2],v[0]);
}



/*Phong Shading Triangle*/
phong_tri(COL,nml,v)
CVECTOR	*COL;			/*original color of triangle*/
SVECTOR	nml[3];			/*normal vectors on each vertex*/ 
short	v[3][2];		/*screen coordinates of vertex*/
{
	int	i,j;
	int	min[2],max[2];	/*Left-Upper and Right-down points*/
	int	SVn[3],EVn[3];	/*vertex number on 
				  start&end vertex on left&right border*/
	int	SVf[3],EVf[3];	/*start&end vertex strict slant*/
	int	SVa[3],EVa[3];	/*start&end vertex Y coordinates*/
	int	fstart_x,fend_x;/*strict start&end X coordinates(1,19,12)*/
	int	istart_x,iend_x;/*integer start&end X coordinates*/
	int	fslt[3];	/*strict slants(1,19,12) of 3 edges*/
	int	mat[2][2];
	int	imat[2][2];
	int	x[2];
	int	p,q;
	VECTOR	normal;
	SVECTOR	snormal;
	int 	det;
	RECT	rect;
	u_short	pix[PIH*PIV];	/*drawing area in Main Memory*/
	VECTOR	lef;
	int	col[3];
	MATRIX	nmat;
	SVECTOR	stvec;
	long	flag;
	MATRIX	lm;
	u_short	*pixx;		/*drawing area pointer*/
	int	idet;
	int	fs,ft;
	int	i4;		/*for Dither*/
	int	dith;
	MATRIX	WLmat;
	MATRIX	lvmat;

	/*normals matrix for interpolation*/
	nmat.m[0][0]= nml[0].vx;
	nmat.m[1][0]= nml[0].vy;
	nmat.m[2][0]= nml[0].vz;
	nmat.m[0][1]= nml[1].vx;
	nmat.m[1][1]= nml[1].vy;
	nmat.m[2][1]= nml[1].vz;
	nmat.m[0][2]= nml[2].vx;
	nmat.m[1][2]= nml[2].vy;
	nmat.m[2][2]= nml[2].vz;
	nmat.t[0]= 0;
	nmat.t[1]= 0;
	nmat.t[2]= 0;

	/*save Rotation Matrix*/
	ReadRotMatrix(&WLmat);

	/*make lighting matrix*/
	MulMatrix0(&lmat,&WLmat,&lvmat);
	MulMatrix0(&cmat,&lvmat,&lm);

	/*Rotation Matrix set for normal vector interpolation*/
	SetRotMatrix(&nmat);
	SetTransMatrix(&nmat);

	/*make lighting matrix include original color*/
	lm.m[0][0]= (COL->r*lm.m[0][0])>>12; 
	lm.m[0][1]= (COL->r*lm.m[0][1])>>12; 
	lm.m[0][2]= (COL->r*lm.m[0][2])>>12;
	lm.m[1][0]= (COL->g*lm.m[1][0])>>12;
	lm.m[1][1]= (COL->g*lm.m[1][1])>>12;
	lm.m[1][2]= (COL->g*lm.m[1][2])>>12;
	lm.m[2][0]= (COL->b*lm.m[2][0])>>12;
	lm.m[2][1]= (COL->b*lm.m[2][1])>>12;
	lm.m[2][2]= (COL->b*lm.m[2][2])>>12;

	SetLightMatrix(&lm);

	/*make vertex matrix for calculation of interpoaltoin coeff.*/
	mat[0][0]= v[1][0]-v[0][0];
	mat[1][0]= v[1][1]-v[0][1];
	mat[0][1]= v[2][0]-v[0][0];
	mat[1][1]= v[2][1]-v[0][1];

	imat[0][0]= mat[1][1];
	imat[1][0]= -mat[1][0];
	imat[0][1]= -mat[0][1];
	imat[1][1]= mat[0][0];

	det= mat[0][0]*mat[1][1]-mat[0][1]*mat[1][0];

	idet = 4096*4096/det;

	/*get minmax of tri. LeftUpper=min[2],RightDown=max[2]*/
	get_minmax_tri(v,min,max);

        /*get strict slant of each 2D line*/
        get_fslant_edge(v,fslt);

        /*sort vertex: get vertex number on left&right border*/
        sort_vertex(v,SVn,EVn);

        /*set start&end vertex Y-address and fslant*/
        /*non start|end vertex is don't care	*/
	/*					*/
	/*        SVn[0]=EVn[0]	   ---SVa[0],EVa[0]	*/
	/*	    0					*/
	/*   SVf[0]/ \					*/
	/*        /   \					*/
	/*SVn[1]=2     \ EVf[0]				*/
	/*   SVf[1] \   \	   ---SVa[1]		*/
	/*             \ \				*/
        /*          SV[2]=1=EVn[1] ---SVa[2],EVa[1]	*/
	/*						*/
	/*	SVf[2],EVf[1],EVf[2]: not used		*/
	/*						*/
        for(i=0;i<3;i++){
                SVf[i]= fslt[(SVn[i]+2)%3];	
                EVf[i]= fslt[EVn[i]];
                SVa[i]= v[SVn[i]][1];
                EVa[i]= v[EVn[i]][1];
        }


        /*set initial strict start_x&end_x*/
        fstart_x= v[SVn[0]][0]<<12;
        fend_x= v[EVn[0]][0]<<12;

	/*read texture area*/
	/*because phong shading destroys previous phong shading*/
	rect.x= min[0];
	rect.y= min[1];
	rect.w= max[0]-min[0]+1;
	rect.h= max[1]-min[1]+1;
	if(rect.h%2==1)rect.h++;

	DrawSync(0);
	StoreImage(&rect,(u_long*)pix);

	/*pixel pointer initialiaze to drawing area head*/
	pixx= pix;

	/*scan minimum Y to maximum Y*/
        for(i=min[1];i<=max[1];i++){
		/*set integer start X and end X coordinates*/
                istart_x= fstart_x>>12;
                iend_x= fend_x>>12;

		/*limitter*/
                if(istart_x<min[0]) istart_x=min[0];
                if(istart_x>max[0]) istart_x=max[0];
                if(iend_x<min[0]) iend_x=min[0];
                if(iend_x>max[0]) iend_x=max[0];

		/*start vector from v[0]*/
		x[1]= i-v[0][1];
		x[0]= istart_x-v[0][0];

		/*interpolation coeff. of (x[0],x[1]) 
				by (v[1]-v[0]) and (v2[1]-v[0])*/
		/* [mat]*[fs]=[x[0]]	*/
		/*       [ft] [x[1]]	*/
		/*			*/
		/* [fs]=[imat]*[x[0]]	*/
		/* [ft]	       [x[1]]	*/
		fs= (imat[0][0]*x[0]+imat[0][1]*x[1])*idet;
		ft= (imat[1][0]*x[0]+imat[1][1]*x[1])*idet;

		/*diffrential of fs,ft by x[0])*/
		p= imat[0][0]*idet;
		q= imat[1][0]*idet;

		/*for Dither*/
		i4= i%4;

		/*move pixel pointer to start point of line*/
		pixx += istart_x-min[0];

#ifndef EQUIVALENT
		/*phong shade 1 line*/
		PhongLine(istart_x,iend_x,p,q,&pixx,fs,ft,i4,det);

#else
		/*equivalent to PhongLine*/
              	for(j=istart_x;j<=iend_x;j++,fs+=p,ft+=q,pixx++){  
			/*(1.0-fs-ft,fs,ft) is interpolation coeff.
			  for interpolation of 3 vertices normal vectors*/
			stvec.vx= (4096*4096-fs-ft)>>12;
			stvec.vy= fs>>12;
			stvec.vz= ft>>12;

			if(det<100){
				if(stvec.vx>4096)stvec.vx=4096;
				if(stvec.vy>4096)stvec.vy=4096;
				if(stvec.vz>4096)stvec.vz=4096;
				if(stvec.vx<0) stvec.vx= 0;
				if(stvec.vy<0) stvec.vy= 0;
				if(stvec.vz<0) stvec.vz= 0;
			}

			/*interpolation of normals*/
			RotTrans(&stvec,&normal,&flag);

			/*vector normalize*/
			VectorNormalS(&normal,&snormal);

			/*lighting*/
			LocalLight(&snormal,&lef);

			/*Dithering*/
			dith= DITH[i4][j%4]/2-4;

			col[0]= (lef.vx+ dith)>>3;
			col[1]= (lef.vy+ dith)>>3;
			col[2]= (lef.vz+ dith)>>3;

			if(col[0]>0x1f) col[0]= 0x1f;
			if(col[0]<0) col[0]= 0;
			if(col[1]>0x1f) col[1]= 0x1f;
			if(col[1]<0) col[1]= 0;
			if(col[2]>0x1f) col[2]= 0x1f;
			if(col[2]<=0) col[2]= 1; /*no transparent*/

			/*pixel draw*/
			*pixx=	(col[2]<<10)+(col[1]<<5)+(col[0]);
			
                }                            
#endif

		/*move pixel pointer to end of drawing Right end*/
		pixx += max[0]-iend_x;

		/*update strict start X*/
	        if(i<SVa[1]) 	fstart_x += SVf[0];
       	        else 		fstart_x += SVf[1];

		/*update strict end X*/
            	if(i<EVa[1]) 	fend_x += EVf[0];
            	else 		fend_x += EVf[1];

		/*limiter*/
            	if(fstart_x>fend_x) fstart_x=fend_x;
        }
	DrawSync(0);
	LoadImage(&rect,(u_long*)pix);

	/*reset Rotation Matrix*/
	SetRotMatrix(&WLmat);
	SetTransMatrix(&WLmat);
}


/* $PSLibId: Run-time Library Release 4.4$ */
/* Walk On Polygon function library*/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

typedef struct {
        POLY_G3 p[2];
        SVECTOR v1;
        SVECTOR v2;
        SVECTOR v3;
} polyG3;

Angle(n1,n2)
VECTOR *n1,*n2;                         /*Rotated unit Vectors(u1->u2)*/
{
	long	inp,oup;
	VECTOR	v;
	long	ang;

	inp= (n1->vx*n2->vx + n1->vy*n2->vy + n1->vz*n2->vz)>>12;
	OuterProduct12(n1,n2,&v);
	oup= (v.vx*v.vx + v.vy*v.vy + v.vz*v.vz)>>12;
	oup= SquareRoot12(oup);
	ang= ratan2(oup,inp);
	return(ang);
}

/*Rotate vector by two unit vectors*/
RotVectorByUnit(n1,n2,v1,v2,m)
VECTOR *n1,*n2;                         /*two unit Vectors(n1->n2)*/
VECTOR *v1,*v2;				/*rotated vectors(v1->v2)*/
MATRIX *m;				/*rotation matrix*/
{
        VECTOR a,b,c;
        MATRIX Rmat,Tmat;
        MATRIX rot;
        MATRIX Rrot,RrotT;
        SVECTOR sv;
        long    ang;

        OuterProduct12(n1,n2,&b);
	if(b.vx==0&&b.vy==0&&b.vz==0){ 
		v2->vx= v1->vx;
		v2->vy= v1->vy;
		v2->vz= v1->vz;
		m->m[0][0]= 4096;
		m->m[0][1]= 0;
		m->m[0][2]= 0;
		m->m[1][0]= 0;
		m->m[1][1]= 4096;
		m->m[1][2]= 0;
		m->m[2][0]= 0;
		m->m[2][1]= 0;
		m->m[2][2]= 4096;
		return;
	}
        VectorNormal(&b,&c);
        OuterProduct12(&c,n1,&a);
        VectorNormal(&a,&b);

        Rmat.m[0][0]= c.vx;
        Rmat.m[1][0]= c.vy;
        Rmat.m[2][0]= c.vz;
        Rmat.m[0][1]= n1->vx;
        Rmat.m[1][1]= n1->vy;
        Rmat.m[2][1]= n1->vz;
        Rmat.m[0][2]= b.vx;
        Rmat.m[1][2]= b.vy;
        Rmat.m[2][2]= b.vz;

        TransposeMatrix(&Rmat,&Tmat);

        ang= Angle(n1,n2);

        rot.m[0][0]=4096; rot.m[0][1]=0; rot.m[0][2]=0;
        rot.m[1][0]=0; rot.m[1][1]= rcos(ang); rot.m[1][2]= -rsin(ang);
        rot.m[2][0]=0; rot.m[2][1]= rsin(ang); rot.m[2][2]=  rcos(ang);

        MulMatrix0(&Rmat,&rot,&Rrot);
        MulMatrix0(&Rrot,&Tmat,m);

        sv.vx= v1->vx;
        sv.vy= v1->vy;
        sv.vz= v1->vz;

        ApplyMatrix(m,&sv,v2);

}

OnTriangle(v,p,n,e)
SVECTOR *v;                     /*test point*/
SVECTOR	p[3];                   /*vertices of triangle*/
VECTOR  n[3];                   /*normals of triangle wall*/
int	*e;			/*crossing edge*/
{
	int	i;
        VECTOR pp[3];
        int ppn[3];
        VECTOR  PP[3];
        long    len[3];
        long    dist;
	SVECTOR	mid[3];


	/*get middle point of each edge*/
	mid[0].vx= (p[0].vx + p[1].vx)/2;
	mid[0].vy= (p[0].vy + p[1].vy)/2;
	mid[0].vz= (p[0].vz + p[1].vz)/2;
	mid[1].vx= (p[1].vx + p[2].vx)/2;
	mid[1].vy= (p[1].vy + p[2].vy)/2;
	mid[1].vz= (p[1].vz + p[2].vz)/2;
	mid[2].vx= (p[2].vx + p[0].vx)/2;
	mid[2].vy= (p[2].vy + p[0].vy)/2;
	mid[2].vz= (p[2].vz + p[0].vz)/2;

	/*get innerproduct of wall's normal & mid.point to test point vector*/
        for(i=0;i<3;i++){
                pp[i].vx= v->vx - mid[i].vx;
                pp[i].vy= v->vy - mid[i].vy;
                pp[i].vz= v->vz - mid[i].vz;
                ppn[i]= pp[i].vx*n[i].vx + pp[i].vy*n[i].vy + pp[i].vz*n[i].vz;
        }

	/*if all innerproduct are >=0 then test point is belongs to triangle*/
        if(ppn[0]>=0&&ppn[1]>=0&&ppn[2]>=0){
                for(i=0;i<3;i++){

                       	Square0(&pp[i],&PP[i]);

                        len[i]= PP[i].vx+PP[i].vy+PP[i].vz;
                }

                dist= len[0]+len[1]+len[2];

                return(dist);
	/*if not return crossing edge #*/
        }else{
		if(ppn[0]<0) *e=0;
		if(ppn[1]<0) *e=1;
		if(ppn[2]<0) *e=2;
                return(-1);
        }
}


ConstraintOnPolygon(v,v1,n)
SVECTOR *v;		/*constrainted point*/
SVECTOR *v1;		/*vertex of Polygon*/
VECTOR  *n;		/*normal vector*/
{
        VECTOR  vv1;
        long    nvv1;

        vv1.vx= v->vx - v1->vx;
        vv1.vy= v->vy - v1->vy;
        vv1.vz= v->vz - v1->vz;

        nvv1= (n->vx*vv1.vx + n->vy*vv1.vy + n->vz*vv1.vz)>>12;

        v->vx -= (nvv1*n->vx)>>12;
        v->vy -= (nvv1*n->vy)>>12;
        v->vz -= (nvv1*n->vz)>>12;
}


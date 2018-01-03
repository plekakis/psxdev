/*
 * $PSLibId: Runtime Library Release 3.6$
 */
#include        <sys/types.h>
#include        <libetc.h>
#include        <libgte.h>
#include        <libgpu.h>
#include        <libgs.h>

#define INTERLACE

#define TXSIZE  256

#ifdef INTERLACE
#define PIV     480
#define PIH     640
#define FSTART  2
#else
#define PIV     240
#define PIH     320
#define FSTART  1
#endif

#define SCR_Z   512
#define OTSIZE  256
#define MAXPAC  (PIV*2)
#define OT_LENGTH	8
#define OTSIZE		256


typedef struct {
        DRAWENV         draw;           /* drawing environment: 描画環境 */
        DISPENV         disp;           /* display environment: 表示環境 */
        u_long          ot[OTSIZE];     /* OT: オーダリングテーブル */
        POLY_FT4        s[MAXPAC];      /* softwear scan conv. packet area */
        POLY_FT3        ss[PIV];      /* softwear scan conv. packet area */
        LINE_F2         normal;         /* normal vector */
        LINE_F2         sect;           /* intersection vector */
        POLY_FT4        divs[32*32];    /* divide polygon mode packet area */
} DB;

struct {
    char u;
    char v;
    short cd;
} uv0, uv1, uv2, uv3;           /* texture parameters for divide */

CVECTOR fcol;

extern u_short tpage;
extern u_short clut;


void divide_polygon(v,db,ndiv)
SVECTOR *v;                /*3D 4 vertices*/
DB      *db;
int     ndiv;
{
    long nclip;
    long otz, p, flag;
    DIVPOLYGON4  *divp;

    divp= (DIVPOLYGON4*)getScratchAddr(0);

    fcol.r=128;                 /* set color */
    fcol.g=128;
    fcol.b=128;
    fcol.cd=db->divs[0].code;

    nclip=RotAverageNclip4(&v[0], &v[1], &v[3], &v[2],
                           (long *)(&db->divs[0].x0),
                           (long *)(&db->divs[0].x1),
                           (long *)(&db->divs[0].x2),
                           (long *)(&db->divs[0].x3),
                           &p, &otz, &flag
                           );

    if (ndiv==0){                          /* without using division */
        if (flag >=0                            /* error OK */
            && nclip>=0                         /* normal clip OK */
            && otz>0)                           /* otz OK */
            AddPrim(db->ot+(otz>>(14-OT_LENGTH)), &db->divs[0]); /* draw */
    } else{                                     /* using division */
	divp->ndiv= ndiv;
	divp->pih= PIH;
	divp->piv= PIV;

	DivideFT4(&v[0],&v[1],&v[3],&v[2],
			&uv0,&uv1,&uv2,&uv3,
			&fcol,
			&db->divs,
			db->ot,
			divp);
    }
}

div_init()
{
    uv0.u=0;
    uv0.v=0;
    uv0.cd=clut;
    uv1.u=255;
    uv1.v=0;
    uv1.cd=tpage;
    uv2.u=0;
    uv2.v=255;
    uv3.u=255;
    uv3.v=255;
}

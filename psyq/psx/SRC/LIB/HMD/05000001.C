/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>
#include <asm.h>
#include <libgs.h>
#include <libhmd.h>

typedef struct {
	short	sx,	sy;
	u_short	w, 	h;
	u_short	nw, 	nh;
	u_short	size, 	base;
	u_short idx,	nc;
}	HMD_P_JIMEN;

typedef struct {
	u_short n,	uv;
}	HMD_M_JIMEN;

typedef struct {
	short	z;
}	HMD_C_JIMEN;

typedef struct {
	u_long	uv0cba;
	u_long	uv1tsb;
	u_long	uv2uv3;
}	HMD_U_JIMEN;

/*
 * Ground Flat Texture
 */
u_long *GsU_05000001(GsARGUNIT *sp)
{
	SVECTOR			v[3];
	SVECTOR			*np;
	POLY_FT4		*polyp;
	HMD_P_JIMEN 		*pp;
	HMD_M_JIMEN 		*mp;
	HMD_C_JIMEN 		*cp;
	HMD_U_JIMEN 		*up;
	GsARGUNIT_GNDT		*ap = (GsARGUNIT_GNDT *)sp;
	int			num;
	long			flg, otz;
	u_long			*tag;
	int			i, j;
	u_short			w, h;
	u_short			idx;
	short			sx, sy;
	u_long			col = 0x80808080;
	u_long			tmpuv;

	polyp = (POLY_FT4 *)ap->out_packetp;

	ap->primp++;
	pp = (HMD_P_JIMEN *)(ap->polytop+(*(ap->primp)));
	mp = (HMD_M_JIMEN *)(ap->boxtop+(*(ap->primp+1)));
	cp = (HMD_C_JIMEN *)(ap->pointtop+(*(ap->primp+2)));
	np = ap->nortop;
	up = (HMD_U_JIMEN *)(ap->uvtop);

	for (i = 0; i < 3; i++) {
		v[i].vx = v[i].vy = v[i].vz = 0;
	}

	gte_ldrgb((CVECTOR *)&col);

	w = pp->w;
	h = pp->h;
	sx = pp->sx;
	sy = pp->sy;
	for (i = 0; i < pp->size; i++) {
	    int sidx;
	    int cidx;
	    int midx;

	    num = *(u_short *)(&(pp->nc)+i*2);
	    idx = *(u_short *)(&(pp->idx)+i*2);
	    sidx = idx%pp->nw;
	    {
		cidx = pp->base+idx;
		midx = pp->base+idx-i;

		v[0].vx = sx+sidx*w;
		v[0].vy = sy+i*h;
		v[0].vz = cp[cidx].z;
		v[1].vx = sx+sidx*w;
		v[1].vy = sy+(i+1)*h;
		v[1].vz = cp[cidx+pp->nw].z;
		v[2].vx = sx+(sidx+1)*w;
		v[2].vy = sy+i*h;
		v[2].vz = cp[cidx+1].z;

		gte_ldv3(&v[0], &v[1], &v[2]);
		gte_rtpt();

		gte_stflg(&flg);

		gte_nclip();
		gte_stopz(&otz);    /* back clip */

		gte_stsxy3_ft4((u_long *)polyp);

		v[0].vx = sx+(sidx+1)*w;
		v[0].vy = sy+(i+1)*h;
		v[0].vz = cp[cidx+pp->nw+1].z;

		gte_ldv0(&v[0]);
		gte_rtps();

		if (flg & 0x80000000) {
			goto NEXT;
		}

		gte_stflg(&flg);
		if (flg & 0x80000000) {
			goto NEXT;
		}

		if (otz >= 0) {
			gte_nclip();
			gte_stopz(&otz);    /* back clip */
			if (otz <= 0) {
				goto NEXT;
			}
			gte_stsxy((u_long *)&polyp->x0);

			gte_ldv0(&(np[mp[midx].n]));	/* lighting */
			gte_nccs();

			gte_avsz3();

			gte_stotz(&otz);

			tag = (u_long *)((u_long *)ap->tagp->org + 
				(otz >> ap->shift));
			*(u_long *)polyp = (*tag & 0x00ffffff) | 0x07000000;
			*tag = (u_long)polyp & 0x00ffffff;
			gte_strgb(&polyp->r0);
			polyp->code = 0x24;
			tmpuv = up[mp[midx].uv].uv2uv3;
			*(u_long *)&polyp->u0 = (tmpuv>>16) |
					((up[mp[midx].uv].uv0cba)&0xffff0000);
			*(u_long *)&polyp->u1 = up[mp[midx].uv].uv1tsb;
			*(u_long *)&polyp->u2 = tmpuv;
	
			polyp += sizeof(POLY_FT3);
		} else {
			gte_stsxy((u_long *)&polyp->x3);
	
			gte_ldv0(&(np[mp[midx].n]));	/* lighting */
			gte_nccs();
	
			gte_avsz4();
	
			gte_stotz(&otz);
	
			tag = (u_long *)((u_long *)ap->tagp->org + 
				(otz >> ap->shift));
			*(u_long *)polyp = (*tag & 0x00ffffff) | 0x09000000;
			*tag = (u_long)polyp & 0x00ffffff;
			gte_strgb(&polyp->r0);
			polyp->code = 0x2c;;
			tmpuv = up[mp[midx].uv].uv2uv3;
			*(u_long *)&polyp->u0 = up[mp[midx].uv].uv0cba;
			*(u_long *)&polyp->u1 = up[mp[midx].uv].uv1tsb;
			*(u_long *)&polyp->u2 = tmpuv;
			*(u_long *)&polyp->u3 = tmpuv>>16;
	
			polyp++;
		}
	    }
NEXT:
	    sidx+=2;
	    for (j = 1; j < num; j++,sidx++) {
		cidx = pp->base+idx+j;
		midx = pp->base+idx-i+j;

		v[0].vx = sx+sidx*w;
		v[0].vy = sy+i*h;
		v[0].vz = cp[cidx].z;
		v[1].vx = sx+sidx*w;
		v[1].vy = sy+(i+1)*h;
		v[1].vz = cp[cidx+pp->nw].z;

		gte_ldv0(&v[0]);
		gte_rtps();

		gte_stflg(&flg);

		gte_nclip();
		gte_stopz(&otz);    /* back clip */

		gte_stsxy3_ft4((u_long *)polyp);

		gte_ldv0(&v[1]);
		gte_rtps();

		if (flg & 0x80000000) {
			continue;
		}

		gte_stflg(&flg);
		if (flg & 0x80000000) {
			continue;
		}

		if (otz >= 0) {
			gte_nclip();
			gte_stopz(&otz);    /* back clip */
			if (otz <= 0) {
				continue;
			} else {
				gte_stsxy((u_long *)&polyp->x0);

				gte_ldv0(&(np[mp[midx].n]));	/* lighting */
				gte_nccs();
	
				gte_avsz3();
	
				gte_stotz(&otz);
	
				tag = (u_long *)((u_long *)ap->tagp->org + 
					(otz >> ap->shift));
				*(u_long *)polyp = (*tag & 0x00ffffff) | 
					0x07000000;
				*tag = (u_long)polyp & 0x00ffffff;
				gte_strgb(&polyp->r0);
				polyp->code = 0x24;
				tmpuv = up[mp[midx].uv].uv2uv3;
				*(u_long *)&polyp->u0 = (tmpuv>>16) |
					((up[mp[midx].uv].uv0cba)&0xffff0000);
				*(u_long *)&polyp->u1 = up[mp[midx].uv].uv1tsb;
				*(u_long *)&polyp->u2 = tmpuv;
	
				polyp += sizeof(POLY_FT3);
			}
		} else {
			gte_stsxy((u_long *)&polyp->x3);
	
			gte_ldv0(&(np[mp[midx].n]));	/* lighting */
			gte_nccs();
	
			gte_avsz4();
	
			gte_stotz(&otz);
	
			tag = (u_long *)((u_long *)ap->tagp->org + 
				(otz >> ap->shift));
			*(u_long *)polyp = (*tag & 0x00ffffff) | 0x09000000;
			*tag = (u_long)polyp & 0x00ffffff;
			gte_strgb(&polyp->r0);
			polyp->code = 0x2c;
			tmpuv = up[mp[midx].uv].uv2uv3;
			*(u_long *)&polyp->u0 = up[mp[midx].uv].uv0cba;
			*(u_long *)&polyp->u1 = up[mp[midx].uv].uv1tsb;
			*(u_long *)&polyp->u2 = tmpuv;
			*(u_long *)&polyp->u3 = tmpuv>>16;
	
			polyp++;
		}
	    }
	}
	GsOUT_PACKET_P = (PACKET *)polyp;
	return(ap->primp+3);
}

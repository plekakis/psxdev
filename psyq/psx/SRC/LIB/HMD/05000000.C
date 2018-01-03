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
	u_char	r,g,b,	pad;
	u_short n,	pad2;
}	HMD_M_JIMEN;

typedef struct {
	short	z;
}	HMD_C_JIMEN;

/*
 * Ground Flat
 */
u_long *GsU_05000000(GsARGUNIT *sp)
{
	SVECTOR			v[3];
	SVECTOR			*np;
	POLY_F4			*pp;
	HMD_P_JIMEN 		*tp;
	HMD_M_JIMEN 		*mp;
	HMD_C_JIMEN 		*cp;
	GsARGUNIT_GND		*ap = (GsARGUNIT_GND *)sp;
	int			num;
	long			flg, otz;
	u_long			*tag;
	int			i, j;
	u_short			w, h;
	u_short			idx;
	short			sx, sy;

	pp = (POLY_F4 *)ap->out_packetp;

	ap->primp++;
	tp = (HMD_P_JIMEN *)(ap->polytop+(*(ap->primp)));
	mp = (HMD_M_JIMEN *)(ap->boxtop+(*(ap->primp+1)));
	cp = (HMD_C_JIMEN *)(ap->pointtop+(*(ap->primp+2)));
	np = ap->nortop;

	for (i = 0; i < 3; i++) {
		v[i].vx = v[i].vy = v[i].vz = 0;
	}

	w = tp->w;
	h = tp->h;
	sx = tp->sx;
	sy = tp->sy;
	for (i = 0; i < tp->size; i++) {
	    int sidx;
	    int cidx;
	    int midx;

	    num = *(u_short *)(&(tp->nc)+i*2);
	    idx = *(u_short *)(&(tp->idx)+i*2);
	    sidx = idx%tp->nw;
	    {
		cidx = tp->base+idx;
		midx = tp->base+idx-i;

		v[0].vx = sx+sidx*w;
		v[0].vy = sy+i*h;
		v[0].vz = cp[cidx].z;
		v[1].vx = sx+sidx*w;
		v[1].vy = sy+(i+1)*h;
		v[1].vz = cp[cidx+tp->nw].z;
		v[2].vx = sx+(sidx+1)*w;
		v[2].vy = sy+i*h;
		v[2].vz = cp[cidx+1].z;

		gte_ldv3(&v[0], &v[1], &v[2]);
		gte_rtpt();

		gte_stflg(&flg);

		gte_nclip();
		gte_stopz(&otz);    /* back clip */

		gte_stsxy3_f4((u_long *)pp);

		v[0].vx = sx+(sidx+1)*w;
		v[0].vy = sy+(i+1)*h;
		v[0].vz = cp[cidx+tp->nw+1].z;

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
			gte_stsxy((u_long *)&pp->x0);

			gte_ldrgb((CVECTOR *)&mp[midx].r);
			gte_ldv0(&(np[mp[midx].n]));	/* lighting */
			gte_nccs();

			gte_avsz3();

			gte_stotz(&otz);

			tag = (u_long *)((u_long *)ap->tagp->org + 
				(otz >> ap->shift));
			*(u_long *)pp = (*tag & 0x00ffffff) | 0x04000000;
			*tag = (u_long)pp & 0x00ffffff;
			gte_strgb(&pp->r0);
			pp->code = 0x20;
	
			pp += sizeof(POLY_F3);
		} else {
			gte_stsxy((u_long *)&pp->x3);
	
			gte_ldrgb((CVECTOR *)&mp[midx].r);
			gte_ldv0(&(np[mp[midx].n]));	/* lighting */
			gte_nccs();
	
			gte_avsz4();
	
			gte_stotz(&otz);
	
			tag = (u_long *)((u_long *)ap->tagp->org + 
				(otz >> ap->shift));
			*(u_long *)pp = (*tag & 0x00ffffff) | 0x05000000;
			*tag = (u_long)pp & 0x00ffffff;
			gte_strgb(&pp->r0);
			pp->code = 0x28;
	
			pp++;
		}
	    }
NEXT:
	    sidx+=2;
	    for (j = 1; j < num; j++,sidx++) {
		cidx = tp->base+idx+j;
		midx = tp->base+idx-i+j;

		v[0].vx = sx+sidx*w;
		v[0].vy = sy+i*h;
		v[0].vz = cp[cidx].z;
		v[1].vx = sx+sidx*w;
		v[1].vy = sy+(i+1)*h;
		v[1].vz = cp[cidx+tp->nw].z;

		gte_ldv0(&v[0]);
		gte_rtps();

		gte_stflg(&flg);

		gte_nclip();
		gte_stopz(&otz);    /* back clip */

		gte_stsxy3_f4((u_long *)pp);

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
				gte_stsxy((u_long *)&pp->x0);

				gte_ldrgb((CVECTOR *)&mp[midx].r);
				gte_ldv0(&(np[mp[midx].n]));	/* lighting */
				gte_nccs();
	
				gte_avsz3();
	
				gte_stotz(&otz);
	
				tag = (u_long *)((u_long *)ap->tagp->org + 
					(otz >> ap->shift));
				*(u_long *)pp = (*tag & 0x00ffffff) | 
					0x04000000;
				*tag = (u_long)pp & 0x00ffffff;
				gte_strgb(&pp->r0);
				pp->code = 0x20;
	
				pp += sizeof(POLY_F3);
			}
		} else {
			gte_stsxy((u_long *)&pp->x3);
	
			gte_ldrgb((CVECTOR *)&mp[midx].r);
			gte_ldv0(&(np[mp[midx].n]));	/* lighting */
			gte_nccs();
	
			gte_avsz4();
	
			gte_stotz(&otz);
	
			tag = (u_long *)((u_long *)ap->tagp->org + 
				(otz >> ap->shift));
			*(u_long *)pp = (*tag & 0x00ffffff) | 0x05000000;
			*tag = (u_long)pp & 0x00ffffff;
			gte_strgb(&pp->r0);
			pp->code = 0x28;
	
			pp++;
		}
	    }
	}
	GsOUT_PACKET_P = (PACKET *)pp;
	return(ap->primp+3);
}

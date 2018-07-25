/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>
#include <asm.h>
#include <libgs.h>
#include <assert.h>
#include <libhmd.h>

typedef struct 
{
	u_long	*base;
	u_long	*src;
	u_long	*dst;
	u_long	*intr;
} GsARGANIM_OPAQ;


/*
#define DEBUG
*/

typedef unsigned long	DWORD;
#define FALSE	0

#define IS_NORM(x)	(((x) & 0x80000000) == 0)
#define IS_NOT_NORM(x)	(((x) & 0x80000000) != 0)
#define IS_JUMP(x)	(((x) & 0xc0000000) == 0x80000000)
#define IS_CTRL(x)	(((x) & 0xc0000000) == 0xc0000000)

/* for NORM */
#define TYPE_IDX(x)	(((x) & 0x7f000000) >> 24)
#define TFRAME(x)	(((x) & 0x00ff0000) >> 16)
#define PARAM_IDX(x)	((x) & 0x0000ffff)

/* for JUMP */
#define SID_DST(x)	(((x) & 0x3f800000) >> 23)
#define SID_CND(x)	(((x) & 0x007f0000) >> 16)
#define SEQ_IDX(x)	((x) & 0x0000ffff)

/* for CTRL */
#define CODE(x)		(((x) & 0x3f800000) >> 23)
#define P1(x)		(((x) & 0x007f0000) >> 16)
#define P2(x)		((x) & 0x0000ffff)
#define CODE_END	0x01
#define CODE_WORK	0x02

static u_short
get_tctr(u_short idx, DWORD *p, GsSEQ *seq)
{
	int	dir = seq->speed < 0 ? -1 : 1;
	DWORD	dw;

	while (dw = p[idx], IS_NOT_NORM(dw)) {
		if (IS_JUMP(dw)) {
			int	cnd = SID_CND(dw);

			if (cnd == 0 || seq->sid == cnd) {
				int	dst = SID_DST(dw);

				if (cnd != 0 || dst != 0) {
					seq->sid = dst;
				}
				idx = SEQ_IDX(dw);
				continue;
			}
		} else if (IS_CTRL(dw)) {
			int	cnd = P1(dw);

			if (CODE(dw) == CODE_END
			&& (cnd == 0 || seq->sid == cnd)) {
				return 0xffff;		/* END CODE */
			}
		} else {
			assert(FALSE);
		}
		idx += dir;
	}

	return idx;
}

u_long *
GsU_03000000(GsARGUNIT_ANIM *sp)	/* update driver */
{
	int	size, wnum, i;
	int	(*func)(GsARGUNIT_ANIM *);
	DWORD	*htop = sp->htop;	/* hokan top */
	DWORD	*ctop = sp->ctop;	/* control top */
	DWORD	*ptop = sp->ptop;	/* parameter top */
	DWORD	*base;

	wnum = (*(sp->primp)) >> 16;
	for (base = &sp->primp[1], i = 0;
			i < wnum; i++, base += base[1] & 0xffff) {
		GsSEQ	*seq = (GsSEQ *)base;
		GsARGANIM_OPAQ	*parm;
		int	dir;
      
		if (seq->aframe == 0) {
			continue;
		} else if(seq->aframe != 0xffff) {
			seq->aframe--;
		}
      
		dir = seq->speed < 0 ? -1 : 1;
      
		/* to treat overshoot in the last interpolation */
		if (seq->rframe == 0x7000) {
			dir = 1;
			seq->rframe = 0;
		} else if (seq->rframe == 0x6000) {
			dir = -1;
			seq->rframe = 0;
		}
      
START:
		if (seq->rframe == 0 || seq->tframe ==0) {
			/* key frame switching */
			seq->traveling = 0;
			if (dir == 1) {
				if (IS_NOT_NORM(ctop[seq->ti])
				&& (seq->ti = get_tctr(
						seq->ti + dir,
						ctop, seq)) == 0xffff) {
					seq->aframe = 0;
					continue;
				}
				seq->ci = seq->ti;
				if ((seq->ti = get_tctr(
						seq->ci + dir,
						ctop, seq)) == 0xffff) {
					seq->aframe = 0;
					continue;
				}
			} else {
				if (IS_NOT_NORM(ctop[seq->ci])
				&& (seq->ci = get_tctr(
						seq->ci + dir,
						ctop, seq))==0xffff) {
					seq->aframe = 0;
					continue;
				}
				seq->ti = seq->ci;
				if ((seq->ci = get_tctr(
						seq->ti + dir,
						ctop, seq))==0xffff) {
					seq->aframe = 0;
					continue;
				}
			}
			if (dir == 1) {
				seq->tframe = TFRAME(ctop[seq->ti]) << 4;
				seq->rframe = seq->tframe - seq->rframe;
			} else {
				seq->tframe = TFRAME(ctop[seq->ti]) << 4;
			}
		}
      
		(u_long)func = htop[TYPE_IDX(ctop[seq->ti]) + 1];
		parm = (GsARGANIM_OPAQ *)
			(&(sp->header_size) + sp->header_size);
		parm->base = base;
		parm->src  = &ptop[PARAM_IDX(ctop[seq->ci])];
		parm->dst  = &ptop[PARAM_IDX(ctop[seq->ti])];
		if (seq->ii != 0xffff) {
			parm->intr = &ptop[PARAM_IDX(ctop[seq->ii])];
		} else {
			parm->intr = 0;
		}
      
		if (seq->ci != seq->ti) {
			if (func(sp) == 1) {
				seq->rframe = 0;
				goto START;
			}
	  
			if (TFRAME(ctop[seq->ti]) == 0) { /* 0 tframe */
				goto START;
			}
      
			seq->rframe -= seq->speed;
      
			if (dir == 1 && seq->rframe < 0) {
				seq->rframe = 0x7000;
			} else if (dir == -1 && (seq->rframe >= seq->tframe)) {
				seq->rframe = 0x6000;
			}
		}
	}
	size = (*(sp->primp)) & 0xffff;

	return sp->primp + size;
}

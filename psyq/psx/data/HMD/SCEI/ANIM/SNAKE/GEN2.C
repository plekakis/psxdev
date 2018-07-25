/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
	gen: generates LAB file to create "snake" sample.

	usage: gen2 [-v|-div num|-tframe num|-speed num] >a.lab

	Copyright (C) 1997 by Sony Computer Entertainment Inc.
	All rights Reserved.

	Ver 1.00	Sep 12, 1997	by N.Yoshioka
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <values.h>

static int	opt_v = 0;
static int	opt_div = 32;
static int	opt_tframe = 64;
static int	opt_speed = 16;

/*#define NUM_RINGS	12	/* num of rings */
#define NUM_RINGS	24	/* num of rings */
/*#define NUM_RINGS	64	/* num of rings */
#define NUM_PTS		6	/* points of a ring */
#define RAD		30.0	/* radius of a ring */
#define HEIGHT		100.0	/* height of a ring */
#define TH		(M_PI / 2 / 4)	/* to make round edge */

/*#define CX	(-NUM_RINGS * HEIGHT / 2)*/
#define CX	0
#define CY	0
#define CZ	0
#define RX	0
#define RY	1024
#define RZ	0

#define ONE	4096

typedef struct {
	long	vx, vy, vz;
} Vector;

typedef struct {
	short	vx, vy, vz;
} SVector;

static void
calc_dz(double dz[NUM_RINGS])
{
	double	zz;
	int	i;

	for (i = 0; i < NUM_RINGS; i++) {
		dz[i] = HEIGHT;
	}

#ifdef notdef
	zz = 0;
	for (i = 0; i < NUM_RINGS; i++) {
		if (zz < RAD) {
			double	th0 = i * TH;
			double	th1 = (i + 1) * TH;

			dz[i] = RAD * (cos(th0) - cos(th1));
			dz[NUM_RINGS - i - 1] = dz[i];
			zz += dz[i];
		} else {
			break;
		}
	}
#endif /* notdef */
}

static void
calc_rad(double rad[NUM_RINGS])
{
	double	zz;
	int	i;

	for (i = 0; i < NUM_RINGS; i++) {
		rad[i] = RAD;
	}

#ifdef notdef
	zz = 0;
	for (i = 0; i < NUM_RINGS; i++) {
		if (zz < RAD) {
			double	th0 = i * TH;
			double	th1 = (i + 1) * TH;
			double	dz = RAD * (cos(th0) - cos(th1));

			rad[i] = RAD * sin(th0);
			rad[NUM_RINGS - i - 1] = rad[i];
			zz += dz;
		} else {
			break;
		}
	}
#endif /* notdef */
}

static void
put_hmd_hdr(void)
{
	printf("#include <hmd/hmd.def>\n");
	printf("\n");
	printf("\tHMD_VERSION;\n");
	printf("\t0x00000000;\t/* MAP FLAG */\n");
	printf("\tPrimHdrSect / 4;\n");
}

static void
put_coord(Vector *trans, SVector *rot)
{
	printf("\t0x00000000;\t/* flg */\n");

	printf("\tCOORDM(%d, %d, %d,\t/* rot */\n",
		rot->vx, rot->vy, rot->vz);
	printf("\t\t%ld, %ld, %ld);\t/* trans */\n",
		trans->vx, trans->vy, trans->vz);
}

static void
put_coord_sect(void)
{
	int	i;
	double	dz[NUM_RINGS];
	Vector	trans;
	SVector	rot;

	printf("CoordSect:\n");
	printf("\t%d;\t/* num of coords */\n", NUM_RINGS + 1);
	printf("\n");

	trans.vx = CX;
	trans.vy = CY;
	trans.vz = CZ;
	rot.vx = RX;
	rot.vy = RY;
	rot.vz = RZ;

	printf("Coord_root:\n");
	put_coord(&trans, &rot);
	printf("\t0;\t/* super */\n");
	printf("\n");

	calc_dz(dz);

	for (i = 0; i < NUM_RINGS; i++) {
		trans.vx = trans.vy = trans.vz = 0;
		rot.vx = rot.vy = rot.vz = 0;

		trans.vz = dz[i];

		printf("Coord_%04d:\n", i);
		put_coord(&trans, &rot);
		if (i == 0) {
			printf("\tCoord_root / 4;\t/* super */\n");
		} else {
			printf("\tCoord_%04d / 4;\t/* super */\n", i - 1);
		}
		printf("\n");
	}
}

static void
put_prim_hdr_sect(void)
{
	printf("PrimHdrSect:\n");
	printf("\t2;\t/* num prim hdrs */\n");
	printf("\n");

	printf("SharedPrimHdr:\n");
	printf("\t6;\t/* hdr size */\n");
	printf("\tM(SharedPoly / 4);\n");
	printf("\tM(SharedVert / 4);\n");
	printf("\tM(CalcedVert / 4);\n");
	printf("\tM(SharedNorm / 4);\n");
	printf("\tM(CalcedNorm / 4);\n");
	printf("\tM(CoordSect / 4);\n");
	printf("\n");

	printf("AnimPrimHdr:\n");
	printf("\t5;\t/* hdr size */\n");
	printf("\t5;\t/* anim hdr size */\n");
	printf("\tM(InterpFuncSect / 4);\n");
	printf("\tM(CtrlSect / 4);\n");
	printf("\tM(ParamSect / 4);\n");
	printf("\tM(CoordSect / 4);\n");
	printf("\n");
}

static void
put_prim_set(void)
{
	int	i;

	for (i = 0; i < NUM_RINGS; i++) {
		printf("SharedPrimSet_%04d:\n", i);
		printf("\tTERMINATE;\t/* next prim */\n");
		printf("\tSharedPrimHdr / 4;\n");
		printf("\tM(1);\t/* num of types */\n");
		printf("\n");

		if (i == 0) {
			printf("\tDEV_ID(SCE)|CTG(CTG_SHARED)|DRV(INI)\n");
			printf("\t\t|PRIM_TYPE(0);\n");
		} else {
			printf("\tDEV_ID(SCE)|CTG(CTG_SHARED)|DRV(0)\n");
			printf("\t\t|PRIM_TYPE(0);\n");
		}
		printf("\tH(7); M(H(0));\t/* size, data */\n");
		printf("\t%d;\t/* num of shared vertices */\n", NUM_PTS);
		printf("\t%d;\t/* src */\n", i * NUM_PTS);
		printf("\t%d;\t/* dst */\n", i * NUM_PTS);
		printf("\t%d;\t/* num of shared normal vectors */\n", NUM_PTS);
		printf("\t%d;\t/* src */\n", i * NUM_PTS);
		printf("\t%d;\t/* dst */\n", i * NUM_PTS);
		printf("\n");
	}

	printf("SharedPolyPrimSet:\n");
	printf("\tTERMINATE;\t/* next prim */\n");
	printf("\tSharedPrimHdr / 4;\n");
	printf("\tM(1);\t/* num of types */\n");
	printf("\n");

	printf("\tDEV_ID(SCE)|CTG(CTG_SHARED)|DRV(0)\n");
	printf("\t\t|PRIM_TYPE(TRI|IIP);\n");
	printf("\tH(2); M(H(%d));\t/* size, data */\n",
		2 * NUM_PTS * (NUM_RINGS - 1));
	printf("\t(SharedPoly - SharedPolySect) / 4;\n");
	printf("\n");
}

static void
put_G3(int r, int g, int b, int v0, int n0, int v1, int n1, int v2, int n2)
{
	printf("\tB(%d); B(%d); B(%d); B(0x30);\t/* r, g, b, code */\n", r, g, b);
	printf("\tH(%d); H(%d);\t/* vert0, norm0 */\n", v0, n0);
	printf("\tH(%d); H(%d);\t/* vert1, norm1 */\n", v1, n1);
	printf("\tH(%d); H(%d);\t/* vert2, norm2 */\n", v2, n2);
}

static void
put_tube(int n, int np)
{
	int	i, v0, n0, r, g, b;

	v0 = n * np;
	n0 = n * np;

	for (i = 0; i < np; i++) {
		r = 255; g = 0; b = 0;
		put_G3(r, g, b,
			v0 + i,			n0 + i,
			v0 + np + i,		n0 + np + i,
			v0 + (i + 1) % np,	n0 + (i + 1) % np);

		r = 255; g = 255; b = 255;
		put_G3(r, g, b,
			v0 + ((i + 1) % np),	n0 + ((i + 1) % np),
			v0 + np + i,		n0 + np + i,
			v0 + np + (i + 1) % np,	n0 + np + (i + 1) % np);
	}
	printf("\n");
}

static void
put_shared_poly(void)
{
	int	i;

	printf("SharedPolySect:\n");
	printf("\n");

	printf("SharedPoly:\n");
	for (i = 0; i < NUM_RINGS - 1; i++) {
		printf("\t/* ring %d */\n", i);
		put_tube(i, NUM_PTS);
	}
}

static void
put_circle(int n, int r, int x, int y, int z)
{
	int	i;

	for (i = 0; i < n; i++) {
		printf("\tSVECTOR(%4d, %4d, %4d);\n",
			x + (int)(r * cos(2 * M_PI / n * i)),
			y + (int)(r * sin(2 * M_PI / n * i)),
			z);
	}
}

static void
put_shared_vert(void)
{
	int	i;
	double	rad[NUM_RINGS];

	calc_rad(rad);

	printf("SharedVert:\n");
	for (i = 0; i < NUM_RINGS; i++) {
		double	r = rad[i];
		int	x = 0;
		int	y = 0;
		int	z = 0;

		put_circle(NUM_PTS, r, x, y, z);
	}
	printf("\n");
}

static void
put_anim_set(void)
{
	int	i;

	printf("AnimPrimSet:\n");
	printf("\tTERMINATE;\t/* next prim */\n");
	printf("\tAnimPrimHdr / 4;\n");
	printf("\tM(1);\t/* num of types */\n");
	printf("\n");

	printf("AnimPrim:\n");
	printf("\tDEV_ID(SCE)|CTG(CTG_ANIM)|DRV(INI|CAT_STD|TGT_COORD)\n");
	printf("\t\t|PRIM_TYPE(SI_NONE|RI_NONE|TI_NONE);\n");
	printf("\tH(%d); M(H(%d));\t/* size, data */\n",
		1 + 7 * NUM_RINGS, NUM_RINGS);

	for (i = 0; i < NUM_RINGS; i++) {
		const int	n = NUM_RINGS / 2;
		int	key, rframe;

		key = i / n;
		rframe = (i % n) * ((double)opt_tframe / n) * 16;

		printf("\tSECT_OFFSET(4 /* CoordSect */)\n");
		printf("\t\t|OFFSET_IN_SECT((Coord_%04d - CoordSect) / 4);\n", i);
		printf("\tH(7); H(1);\t/* siz, num of sequences */\n");
		printf("\tH(0); H(0);\t/* intr idx, aframe */\n");
		printf("\tB(0); B(%d); H(0xffff);", opt_speed);
		printf("\t/* stream id, speed(fixed), src intr idx */\n");
		printf("\tH(0x%04x); H(0);", rframe);
		printf("\t/* rframe(fixed), tframe */\n");
		printf("\tH(0); H(0);\t/* ctr idx, tctr idx */\n");
		printf("\tH((Ctrl_%04d - CtrlSect) / 4); B(0); B(0);", key);
		printf("\t/* start idx, start sid, travering */\n");
	}
	printf("\n");
}

static void
put_interp_sect(void)
{
	printf("InterpFuncSect:\n");
	printf("\tM(1);\t/* num of types */\n");
	printf("\tDEV_ID(SCE)|CTG(CTG_ANIM)|DRV(CAT_STD|TGT_COORD)\n");
	printf("\t\t|PRIM_TYPE(SI_NONE|RI_LINEAR|TI_NONE);\n");
	printf("\n");
}

static void
put_ctrl_sect(void)
{
	int	i;

	printf("CtrlSect:\n");
	printf("\n");
	printf("\tSEQD(JUMP)|SID_DST(0)|SID_CND(0)\n");
	printf("\t\t|SEQ_IDX((CtrlEnd - CtrlSect) / 4);\n");
	printf("CtrlBegin:\n");
	for (i = 0; i < 2; i++) {
		printf("Ctrl_%04d:\n", i);
		if (i == 1) {
			printf("CtrlEnd:\n");
		}
		printf("\tSEQD(NORMAL)|TYPE_IDX(0)|TFRAME(%d)\n", opt_tframe);
		printf("\t\t|PARAM_IDX((Key_%04d - ParamSect) / 4);\n", i);
	}
	printf("\tSEQD(JUMP)|SID_DST(0)|SID_CND(0)\n");
	printf("\t\t|SEQ_IDX((CtrlBegin - CtrlSect) / 4);\n");
	printf("\n");
}

static void
put_param_sect(void)
{
	int	i;

	printf("ParamSect:\n");
	for (i = 0; i < 2; i++) {
		double	r;

		printf("Key_%04d:\n", i);
		r = cos(2 * M_PI / 2 * i);
		printf("\t%d; %d; %d;\n",
			(int)floor(r * ONE / opt_div + 0.5), 0, 0);
	}
	printf("\n");
}

int
main(int argc, char **argv)
{
	int	i;

	while (argv++, --argc > 0) {
		if (strcmp(*argv, "-v") == 0) {
			opt_v = 1;
		} else if (strcmp(*argv, "-div") == 0) {
			argv++;
			argc--;
			opt_div = strtol(*argv, NULL, 10);
		} else if (strcmp(*argv, "-tframe") == 0) {
			argv++;
			argc--;
			opt_tframe = strtol(*argv, NULL, 10);
		} else if (strcmp(*argv, "-speed") == 0) {
			argv++;
			argc--;
			opt_speed = strtol(*argv, NULL, 10);
		} else {
			break;
		}
	}

	put_hmd_hdr();

	printf("\t%d;\t/* num of blocks */\n", 2 + NUM_RINGS + 1);
	printf("\tAnimPrimSet / 4;\t/* pre-process */\n");
	printf("\t0;\t/* null coordinate */\n");
	for (i = 0; i < NUM_RINGS; i++) {
		printf("\tSharedPrimSet_%04d / 4;\n", i);
	}
	printf("\tSharedPolyPrimSet / 4;\n");
	printf("\n");

	put_coord_sect();
	put_prim_hdr_sect();
	put_prim_set();
	put_shared_poly();
	put_shared_vert();
	put_anim_set();
	put_interp_sect();
	put_ctrl_sect();
	put_param_sect();

	printf("CalcedVert:\n");
	for (i = 0; i < NUM_RINGS * NUM_PTS; i++) {
		printf("\tH(0); H(0); H(0); H(0);\t/* vx, vy, otz, p */\n");
	}
	printf("\n");

	printf("SharedNorm:\n");
	for (i = 0; i < NUM_RINGS; i++) {
		put_circle(NUM_PTS, ONE, 0 /* x */, 0 /* y */, 0 /* z */);
	}
	printf("\n");

	printf("CalcedNorm:\n");
	for (i = 0; i < NUM_RINGS * NUM_PTS; i++) {
		printf("\tH(0); H(0); H(0); H(0x0000);\t/* r, g, b, pad */\n");
	}
	printf("\n");

	return 0;
}

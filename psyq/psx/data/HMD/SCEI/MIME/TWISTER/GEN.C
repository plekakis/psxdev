/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
	gen: generates LAB file to create "twister" sample.

	usage: gen [-v|-bend|-popye|-twist] >a.lab

	Copyright (C) 1997 by Sony Computer Entertainment Inc.
	All rights Reserved.

	Ver 1.00	Aug 28, 1997	by N.Yoshioka
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <values.h>

static int	opt_v = 0;
static int	opt_bend = 0;
static int	opt_popye = 0;
static int	opt_twist = 0;

/*#define NUM_RINGS	12	/* num of rings */
#define NUM_RINGS	24	/* num of rings */
/*#define NUM_RINGS	64	/* num of rings */
#define NUM_PTS		8	/* points of a ring */
#define RAD		200.0	/* radius of a ring */
#define HEIGHT		100.0	/* height of a ring */
#define TH		(M_PI / 2 / 4)	/* to make round edge */
#define BEND		128	/* angle to bend */
#define TWIST		100	/* angle to twist */

#define CX	(-NUM_RINGS * HEIGHT / 2)
#define CY	0
#define CZ	2048
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
}

static void
calc_rad(double rad[NUM_RINGS])
{
	double	zz;
	int	i;

	for (i = 0; i < NUM_RINGS; i++) {
		rad[i] = RAD;
	}

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

	printf("CoordSect:\n");
	printf("\t%d;\t/* num of coords */\n", NUM_RINGS);
	printf("\n");

	calc_dz(dz);

	for (i = 0; i < NUM_RINGS; i++) {
		Vector	trans;
		SVector	rot;

		trans.vx = trans.vy = trans.vz = 0;
		rot.vx = rot.vy = rot.vz = 0;

		if (i == 0) {
			trans.vx = CX;
			trans.vy = CY;
			trans.vz = CZ;
			rot.vx = RX;
			rot.vy = RY;
			rot.vz = RZ;
		} else {
			trans.vz = dz[i];
		}

		if (opt_bend) {
			if (i > NUM_RINGS / 4) {
				rot.vx = BEND;
			}
		}

		if (opt_twist) {
			if (i > 0) {
				rot.vz = TWIST;
			}
		}

		printf("Coord_%04d:\n", i);
		put_coord(&trans, &rot);
		if (i == 0) {
			printf("\t0;\t/* super */\n");
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
	printf("\t1;\t/* num prim hdrs */\n");
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

		if (opt_popye) {
			double	cx = NUM_RINGS / 6.0;
			double	ex = i - cx;
			double	a = 10.0 / NUM_RINGS;

			r = r * (1 + exp(-(a * ex * a * ex)));
			y = (RAD - r) / 2;
		}

		put_circle(NUM_PTS, r, x, y, z);
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
		} else if (strcmp(*argv, "-bend") == 0) {
			opt_bend = 1;
		} else if (strcmp(*argv, "-popye") == 0) {
			opt_popye = 1;
		} else if (strcmp(*argv, "-twist") == 0) {
			opt_twist = 1;
		} else {
			break;
		}
	}

	put_hmd_hdr();

	printf("\t%d;\t/* num of blocks */\n", 2 + NUM_RINGS);
	printf("\t0x00000000;\t/* no pre-process */\n");
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

/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


/*
 *  Aliases	ID		Primitive types
 *  ---------	----------	-----------------------------------------------
 *  GsUSCAL2	0x06000100	envmap shared calculate
 *  GsUE1G3	0x0600100c	envmap 1D gour tri
 *  GsUE1G4	0x06001014	envmap 1D gour quad
 *  GsUE1SG3	0x0600110c	envmap 1D shared gour tri
 *  GsUE1SG4	0x06001114	envmap 1D shared gour quad
 *  GsUE2LG3	0x0600200c	envmap 2D reflect gour tri
 *  GsUE2LG4	0x06002014	envmap 2D reflect gour quad
 *  GsUE2RG3	0x0600300c	envmap 2D refract gour tri
 *  GsUE2RG4	0x06003014	envmap 2D refract gour quad
 *  GsUE2RLG3	0x0600400c	envmap 2D both gour tri
 *  GsUE2RLG4	0x06004014	envmap 2D both gour quad
 *  GsUE2OLG3	0x0600500c	envmap 2D org+reflect gour tri
 *  GsUE2OLG4	0x06005014	envmap 2D org+reflect gour quad
 */

/* UV calcurate parameters */
#define SHIFT1 5
#define SHIFT2 1
#define SHIFT3 5
#define SHIFT4 3

typedef struct {
	unsigned long	*primp;
	GsOT		*tagp;
	int		shift;
	int		offset;
	PACKET		*out_packetp;
	unsigned long	*primtop;
	SVECTOR		*vertop;
	SVECTOR		*nortop;
	unsigned long	*tex1;
	unsigned long	*tex2;
	unsigned long	*clut2;
	unsigned long	*tex3;
	unsigned long	*clut3;
	u_char	p00; /* texture mode 0,1,2 */
	u_char	p01;
	u_char	p02; /* material (y offset) */
	u_char	p03;
	u_char	p10; /* texture mode 0,1,2 */
	u_char	p11; /* abr */
	u_char	p12; /* reflect rate */
	u_char	p13;
	u_char	p14;
	u_char	p15;
	u_char	p16;
	u_char	p17;
	u_char	p20; /* texture mode 0,1,2 */
	u_char	p21; /* abr */
	u_char	p22; /* refract rate */
	u_char	p23;
	u_char	p24; 
	u_char	p25; /* r */
	u_char	p26; /* g */
	u_char	p27; /* b */
}	GsARGUNIT_ENVMAP;

typedef struct {
	unsigned long	*primp;
	unsigned long	*primtop;
	SVECTOR		*vertop;
	SVECTOR		*nortop;
	unsigned long	*tex1;
	unsigned long	*tex2;
	unsigned long	*clut2;
	unsigned long	*tex3;
	unsigned long	*clut3;
	u_char	p00; /* texture mode 0,1,2 */
	u_char	p01;
	u_char	p02; /* material (y offset) */
	u_char	p03;
	u_char	p10; /* texture mode 0,1,2 */
	u_char	p11; /* abr */
	u_char	p12; /* reflect rate */
	u_char	p13;
	u_char	p14;
	u_char	p15;
	u_char	p16;
	u_char	p17;
	u_char	p20; /* texture mode 0,1,2 */
	u_char	p21; /* abr */
	u_char	p22; /* refract rate */
	u_char	p23;
	u_char	p24; 
	u_char	p25; /* r */
	u_char	p26; /* g */
	u_char	p27; /* b */
}	GsEnvPrimHdr;

typedef struct {
	int     shift;		/* OT shift */
	u_long *org;		/* OT org */
	u_long *pk;		/* packet base */
	long    otz;		/* gte otz */
	long    flg0;		/* gte flag */
	long    flg;		/* gte flag */
	u_long *tag;		/* work temprly for addPrim */
	/* added */
	u_short			tpage_env;
	u_short			clut_env;
	u_short			tpage_org;
	u_short			clut_org;
	u_char			reflect;
	u_char			refract;
#if 0
	/* work */
	VECTOR lv;
	int uu, vv;
	int ddx, ddy;
#endif
}       GsENV;

typedef struct {
	u_char  r0, g0, b0, code;
	u_short n0, v0;
	u_short n1, v1;
	u_short n2, v2;
}	HMD_P_G3;

typedef struct {
	u_char  r0, g0, b0, code;
	u_short n0, v0;
	u_short n1, v1;
	u_short n2, v2;
	u_short n3, v3;
}	HMD_P_G4;

typedef struct {
	u_short	uv0, cba;
	u_short	uv1, tsb;
	u_short	uv2, pad;
	u_short n0, v0;
	u_short n1, v1;
	u_short n2, v2;
}	HMD_P_GT3;

typedef struct {
	u_short	uv0, cba;
	u_short	uv1, tsb;
	u_short	uv2, n0;
	u_short	uv3, v0;
	u_short n1, v1;
	u_short n2, v2;
	u_short n3, v3;
}	HMD_P_GT4;


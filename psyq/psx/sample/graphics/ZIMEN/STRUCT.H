/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	Structures for the ground */
typedef struct {
	u_long nearz;	/* Z value to start divide */
	u_long shift;	/* shift value */
	u_long nz;	/* maximum divide iteration */
	u_long *cond;	/* pointer to table that keeps divide number */
} GsDIVCOND0;


typedef struct {
	u_char	u, v;		/* cell texture uv */
	u_char	du,dv;		/* texture width, height */
	u_short	tpage;		/* texture page */
	u_short	nclut;		/* number of CLUTs (for fog)  */
	u_short	*clut;		/* cell clut */
	u_long	attr;		/* attribute (reserved) */
} CTYPE;

typedef struct {
	/* map coordinate system information */
	int	mx, my;		/* map size */
	int	nctype;		/* number of cell type */
	char	**map;		/* map */
	CTYPE	*ctype;		/* cell type */
	
	/* 3D coordinate system information */
	GsCOORDINATE2 *coord2;	/* local coordinate system */
	u_long cw,ch;		/* cell width */
	u_long nw,nh;		/* vertical and horizontal cell count for display */
	u_long iw,ih;		/* vertical and horizontal cell count for a pattern (2^n) */
	GsDIVCOND0 *cond;	/* pointer to automatic divide condition structure */
} Gs3DBG0;
	
typedef struct
{
  u_short *clut;
  u_short cbit;
  RECT    rectc;
  CVECTOR bgc;
} GsDPCLUT0;

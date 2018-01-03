/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *	地面で使用する構造体
 */
typedef struct {
	u_long nearz;	/* divideを分割を開始するZ値 */
	u_long shift;	/* shift値 */
	u_long nz;	/* divide段数の最大値 */
	u_long *cond;	/* 分割数テーブルへのポインタ */
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
	/* マップ座標系情報 */
	int	mx, my;		/* マップサイズ */
	int	nctype;		/* セルの種類 */
	char	**map;		/* マップ */
	CTYPE	*ctype;		/* セルタイプ */
	
	/* 3D 座標系情報 */
	GsCOORDINATE2 *coord2;	/* ローカル座標系 */
	u_long cw,ch;		/* セルの幅 */
	u_long nw,nh;		/* セルの縦横（表示）の数 */
	u_long iw,ih;		/* セルの縦横（１パターン）の数（2^n） */
	GsDIVCOND0 *cond;	/* 自動分割条件構造体へのポインタ */
} Gs3DBG0;
	
typedef struct
{
  u_short *clut;
  u_short cbit;
  RECT    rectc;
  CVECTOR bgc;
} GsDPCLUT0;

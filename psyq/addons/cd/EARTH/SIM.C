/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *			オンメモリSTR データビューア
 *
 */	
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>	

#define PPW		1
#define STRADDR		(CDSECTOR *)0xa0300000	
#define USRSIZE		(512-8)		

typedef struct {
	u_short	id;			/* always 0x0x0160 */
	u_short	type;			
	u_short	secCount;	
	u_short	nSectors;
	u_long	frameCount;
	u_long	frameSize;
	
	u_short	width;
	u_short	height;
	
	u_long	headm;
	u_long	headv;
	
	u_char	reserved[4];
	
	u_long	data[USRSIZE];		/* ユーザデータ */
} CDSECTOR;				/* CD-ROM STR 構造体 */

CDSECTOR	*Sector;		/* 現在のセクタの位置 */

StGetNextS(addr, sector)
u_long		**addr;
CDSECTOR	**sector;
{
	static int	first = 1;
	static u_long data[20*USRSIZE];	/* ミニヘッダを取り除いたデータ */
	
	int		i, j, len;
	u_long		*sp, *dp;

	if (first) {
		first = 0;
		Sector = STRADDR;
	}
#ifdef DEBUG
	printf("%d: %d sect,(%d,%d)\n",
	       Sector->frameCount, Sector->nSectors,
	       Sector->width, Sector->height);
#endif

	/* リターン値を設定 */
	*addr   = data;
	*sector = Sector;
	
	/* ミニヘッダを取り除いたユーザデータを作成する */
	dp  = data;
	len = Sector->nSectors;
	while (len--) {

		/* 意味のあるヘッダかを確かめる。*/
		if (Sector->id != 0x0160) {
			Sector = STRADDR;
			return(-1);
		}
		/* データをコピー */
		for (sp = Sector->data, i = 0; i < USRSIZE; i++) 
			*dp++ = *sp++;
		Sector++;
	}

	/* データヘッダをコピー */
	(*sector)->headm = data[0];
	(*sector)->headv = data[1];
	return(0);
}


StFreeRingS(next)
u_long	*next;
{
	/* do nothing */
}

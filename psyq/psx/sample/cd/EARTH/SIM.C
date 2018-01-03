/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *             In-memory STR data viewer */	
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
	
	u_long	data[USRSIZE];		/* User data */
} CDSECTOR;				/* CD-ROM STR structure */

CDSECTOR	*Sector;		/* current sector position */

StGetNextS(addr, sector)
u_long		**addr;
CDSECTOR	**sector;
{
	static int	first = 1;
	static u_long data[20*USRSIZE];	/* data with mini-header removed */
	
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

	/* set return value */
	*addr   = data;
	*sector = Sector;
	
	/* prepare user data with mini-header removed */
	dp  = data;
	len = Sector->nSectors;
	while (len--) {

		/* check with header is valid */
		if (Sector->id != 0x0160) {
			Sector = STRADDR;
			return(-1);
		}
		/* copy data */
		for (sp = Sector->data, i = 0; i < USRSIZE; i++) 
			*dp++ = *sp++;
		Sector++;
	}

	/* copy data header */
	(*sector)->headm = data[0];
	(*sector)->headv = data[1];
	return(0);
}


StFreeRingS(next)
u_long	*next;
{
	/* do nothing */
}

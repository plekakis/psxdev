/*
 * $PSLibId: Run-time Library Release 4.4$
 */
#ifndef MENU_INCLUDED
#define MENU_INCLUDED

#define	MAX_DIRDEPTH	10

/* struct MENU */
typedef struct {
	char 	str[81];			/* item name */
	char	root[81];			/* root */
	char	fname[MAX_DIRDEPTH][81];	/* file name */
	u_short	kind[MAX_DIRDEPTH];		/* & 0x7fff -> kind */
						/* & 0x8000 -> flag */
	int	depth;		/* depth of path */
	int	ndata;		/* the number of data */
	u_long  daddr[16];	/* data address */
	char	dname[16][81];	/* path of data file */
	char	sentj[161];	/* the explanation in Japanese */
	char	sente[161];	/* the explanation in English */
} MENU;

typedef struct {
	int	top;	/* top of display items*/
	int	no;	/* current item No.*/
	int	cnt;	/* the count of display items*/
	short	item[100];	/* display items of MENU items */
} DISP_MENU;

#endif

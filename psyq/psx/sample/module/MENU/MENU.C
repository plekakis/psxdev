/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *  (C) Copyright 1995 Sony Computer Entertainment Inc. Tokyo, Japan.
 *                      All Rights Reserved
 *
 *	Sample Program Viewer (pcmenu, cdmenu)
 *	menu.c:
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00		Mar,09,1995	yoshi
 *	2.00		Oct,20,1995	sachiko
 */

#include <sys/types.h>
#include <libetc.h>
#include <libcd.h>
#include <libapi.h>
#include <libgte.h>
#include <libgpu.h>
#include "menu.h"
#include "errmsg.h"

#define DISP_MAX 8

#define REP_NUM1 10	/* the first repeat timing */
#define REP_NUM2 3	/* the repeat timing */

/*
 * Primitive Buffer
 */
#define OTSIZE		8		/* ordering table size*/
typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment*/
	u_long		ot[OTSIZE];	/* ordering table*/
	POLY_G4		poly;		/* poly-g4 */
} DB;

typedef struct {
	int	lang;		/* language: 0:English 1:Japanese */
	int	help;		/* help mode: 0:off 1:ON */
	int	err;		/* error code */
	int	exec;
} FLAGS;

/* initialize graphic system */
static void init_system(DB *db);
/* preset unchanged primitive members */
static void init_prim(DB *db);
/* parse controller */
static int  pad_read(MENU *menu,int depth,DISP_MENU *dm,int *execno,FLAGS *fl);
static void in_help(u_long padd,FLAGS *fl);
static void print_menu(int depth,int execno,DISP_MENU *dm,MENU *menu,FLAGS *fl);
static char *get_cdir(MENU *menu,int menuno,int depth,char *cdir);
static void set_dispitem(int depth,MENU *menu,int menu_max,DISP_MENU *dm);
static void draw_BG(DB *cdb);
static void print_help(int lang);
static void draw_flush(void);

extern int  load_menu(MENU **menu, int *menu_max);       /* loadmenu.c */
extern int  exec(MENU *menu);                            /* exec.c */
extern void set_execno(int execno);                      /* printerr.c */
extern void print_errmsg(int errno,MENU *menu,int lang); /* printerr.c */

main()
{
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current double buffer */
	int	depth=0;	/* the depth of tree */
	int	wait=0;		/* work */
	int	newdepth;	/* work */
	FLAGS	fl={JAPANESE,0,0,0};	/* flags */

	static int		menu_max;	/* the count of menu items */
	static int		execno = 0;
	static MENU		*menu=NULL;
	static DISP_MENU	dm[MAX_DIRDEPTH];

	/* InitHeap(0x80600000, 0x00100000); */
	EnterCriticalSection();
	InitHeap(0x80540000, 0x00100000);
	ExitCriticalSection();
	ResetCallback();

	/* initialize graphics system*/
	init_system(db);

	/* DEBUG
	CdInit();
	CdSetDebug(0);
	*/

	/* clear the dm*/
	memset(dm,0,sizeof(DISP_MENU)*MAX_DIRDEPTH);

	/* load menu file*/
	if(load_menu(&menu, &menu_max)) {
		ResetGraph(3);
		PadStop();
		StopCallback();
		return 0;
	}

	/* select items to display*/
	set_dispitem(depth,menu,menu_max,&dm[depth]);

	/* initialize sound*/
	sndInit();

	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	while (1) {

		/* parse controller*/
		newdepth = pad_read(menu,depth,&dm[depth],&execno,&fl);

		if ( depth != newdepth ) {
			if ( depth < newdepth ) {
				set_dispitem(newdepth,menu,
						menu_max,&dm[newdepth]);
				dm[newdepth].no = 0;
			}
			depth = newdepth;
		}

		if (execno) {
			if (++wait > 2) {

			    /* run*/
				if((fl.err = exec(&menu[execno-1])) !=NO_ERR) {
					set_execno(execno);
				} else {
					fl.exec = 1;
				}
				execno = 0;
				wait = 0;       /* wait clear */
				draw_flush();	/* reload FONT textures */

			}
		}

		/* swap double buffer ID*/
		cdb = (cdb==db)? db+1: db;

		/* clear ordering table*/
		ClearOTag(cdb->ot, OTSIZE);

		/* draw BG*/
		draw_BG(cdb);

		if(!fl.help) {
			/* print menu*/
			print_menu(depth,execno,&dm[depth],menu,&fl);
		} else {
			/* print help*/
			print_help(fl.lang);
		}

		/* wait for end of drawing*/
		DrawSync(0);

		/* wait for V-BLNK (1/60)*/
		VSync(0);

	}
}

/* initialize graphic system*/
static void init_system(DB *db)
{
	PadInit(0);		/* reset PAD */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */

	/* initialize environment for double buffer*/
	SetDefDrawEnv(&db[0].draw, 0,   0, 640, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 640, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 640, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 640, 240);

	/* init font environment */
	KanjiFntOpen(20, 20, 640, 240, 960, 0, 0, 480, 0, 512);

	init_prim(&db[0]);		/* initialize primitive buffers #0 */
	init_prim(&db[1]);		/* initialize primitive buffers #1 */
}

/* preset unchanged primitive members */
static void init_prim(DB *db)
{
	POLY_G4	*sp;

	/* initialize POLY_G4*/
	sp = &db->poly;
	SetPolyG4(sp);			/* set SPRT_16 primitve ID */
	SetSemiTrans(sp, 0);		/* semi-amibient is OFF */
	SetShadeTex(sp, 1);		/* shading&texture is OFF */
	setXYWH(sp, 0, 0, 640, 240);
}

static void draw_flush(void)
{
	KanjiFntOpen(20, 32, 640, 240, 960, 0, 0, 480, 0, 512);
}

static void set_dispitem(int depth,MENU *menu,int menu_max,DISP_MENU *dm)
{
	static int	first=1;	/* first flag */
	char		cdir[256];	/* current directory */
	int		mc,dc;		/* counter */
	int		kind;		/* kind */
	char		buf[256];	/* work */

	dm->item[0] = 0;

	if ( first ) {
		get_cdir(menu,dm->item[dm->no],depth,cdir);
		first = 0;
	} else
		get_cdir(menu,dm[-1].item[dm[-1].no],depth,cdir);

	for ( kind=-1,mc=0,dc=1; mc<menu_max; mc++ ) {
		if ( (menu[mc].kind[depth]&0x7fff) > kind ) {
			if ( get_cdir(menu,mc,depth,buf) ) {
				if ( strcmp(cdir,buf) == NULL ) {
					dm->item[dc++] = mc;
					kind = menu[mc].kind[depth]&0x7fff;
				}
			}
		}
	}
	dm->cnt = dc;
}

static char *get_cdir(MENU *menu,int menuno,int depth,char *cdir)
{
	int	i;	/* counter */

	if ( menu[menuno].depth < depth ) return(NULL);
	strcpy(cdir,menu[menuno].root);
	for ( i=0; i<depth; i++ )
		strcat(cdir,menu[menuno].fname[i]);
	return(cdir);
}

/* Read controll-pad*/
static int pad_read(MENU *menu,int depth,DISP_MENU *dm,int *execno,FLAGS *fl)
{
	MENU		*mp;
	static u_long	opadd = 0;
	u_long		padd = PadRead(1);

	static int	f_rep = 0;  /* the flag of auto repeat */
	static int	pcnt  = 0;  /* the count of press the button */

	if (fl->exec) {
		if(padd) {
			return depth;
		} else {
			opadd = 0;
			fl->exec = 0;
		}
	}
	if (padd == opadd) {
		switch(f_rep) {
			case 0:
				return(depth);
			case 1:
				pcnt++;
				if(pcnt < REP_NUM1) return(depth);
			case 2:
				pcnt++;
				if(pcnt < REP_NUM2) return(depth);
		}
	} else {
		pcnt  = 0;
		f_rep = 0;
		if(padd && !(padd & PADselect)) fl->err = 0;
	}

	/* in help mode*/
	if ( fl->help ) {
		in_help(padd,fl);
		opadd=padd;
		return(depth);
	}

	/* in MENU mode*/

	/* Up */
	if ((padd & PADLup) && (dm->no > 0))
	{
		--(dm->no);
		if(dm->top>dm->no) --(dm->top);

		switch(f_rep) {
			case 0:
				f_rep = 1;
				break;
			case 1:
				pcnt  = 0;
				f_rep = 2;
				break;
			default:
				pcnt  = 0;
				break;
		}
	}
	/* Down*/
	if ((padd & PADLdown) && (dm->no < dm->cnt - 1))
	{
		++(dm->no);
		if((dm->top+DISP_MAX)<=dm->no) ++(dm->top);

		switch(f_rep) {
			case 0:
				f_rep = 1;
				break;
			case 1:
				pcnt  = 0;
				f_rep = 2;
				break;
			default:
				pcnt  = 0;
				break;
		}
	}

	/* Select*/
	if (padd & PADRright || padd & PADstart) {
		fl->err = 0;
		mp = &menu[dm->item[dm->no]];
		if ( dm->no==0 ) {
			if ( depth>0 ) --depth;
		} else if ( mp->depth>depth+1 ) {
			if ( depth+1 < MAX_DIRDEPTH ) ++depth;
		} else
			*execno = (dm->item[dm->no])+1;
	}

	/* Move to top*/
	if (padd & PADRdown)
	{
		dm->no  = 0;
		dm->top = 0;
	}

	/* Change Languge*/
	if (padd & PADselect)
		fl->lang ^= 1;

	/* MENU or HELP mode*/
	if (padd & PADRleft)
		fl->help = 1;

	opadd = padd;

	return(depth);
}

static void in_help(u_long padd,FLAGS *fl)
{
	/* exit*/
	if ( (padd & PADRdown) || (padd & PADRleft) )
		fl->help = 0;

	/* Change Language*/
	if (padd & PADselect)
		fl->lang ^= 1;
}

static void draw_BG(DB *cdb)
{
	static	u_char		c[3]={255,0,0};	/* BG color */
	POLY_G4			*sp;		/* work */

	sp = &cdb->poly;
	setRGB0(sp, c[0], c[1], c[2]);
	setRGB1(sp, c[2], c[0], c[1]);
	setRGB2(sp, c[1], c[2], c[0]);
	setRGB3(sp, c[0], c[1], c[2]);
	AddPrim(cdb->ot, sp);	/* apend to OT */

	if ((c[2] == 0) && (c[0] > 0)) {
		c[0]--; c[1]++;
	}
	if ((c[0] == 0) && (c[1] > 0)) {
		c[1]--; c[2]++;
	}
	if ((c[1] == 0) && (c[2] > 0)) {
		c[2]--; c[0]++;
	} 

	/* swap double buffer*/
	PutDispEnv(&cdb->disp); /* update display environment */
	PutDrawEnv(&cdb->draw); /* update drawing environment */
	DrawOTag(cdb->ot);
}

static void print_menu(int depth,int execno,DISP_MENU *dm,MENU *menu,FLAGS *fl)
{
	char			buf[256];	/* work */
	int			i, j;
	long			len;
	char			*cp;
	short			*di;

	static char		fname[81];

	/*-------- flush strings ----------*/
	FntFlush(-1);		/* Alphabet */
	KanjiFntFlush(-1);	/* Kanji */

	di = dm->item;
	/*-------- Title ----------*/
	KanjiFntPrint("        Sample Program Menu\n");

	/*---- current directory ---*/
	strcpy(buf,menu[di[dm->top]].root);
	for ( i=0; i<depth; i++ ) strcat(buf,menu[di[1]].fname[i]);
	if ( (i=strlen(buf)) > 38 ) strcpy(buf,&buf[i-38]);
	KanjiFntPrint("%s\n",buf);

	/*--------- ITEM ---------*/
	for (i = dm->top; i<dm->cnt&&i<(dm->top+DISP_MAX); i++) {
		/*------ >> --------*/ 
		if ( dm->no == i )
			strcpy(buf,">> ");
		else
			strcpy(buf,"   ");
		/*------- name -------*/
		if ( i==0 ) {
			strcat(buf,"..");
		} else {
			cp = (menu[di[i]].depth>depth+1)
				? menu[di[i]].fname[depth]
				: menu[di[i]].str;
			strcat(buf,cp);
		}
		KanjiFntPrint("%s\n", buf);
	}

	for (; i<DISP_MAX; i++) {
		KanjiFntPrint("\n");
	}

	/*------- print "sent" ----*/

	if(!fl->err) {
		/*if((menu[di[dm->no]].kind[depth]&0x8000) || (dm->no== 0)) {*/
		if((menu[di[dm->no]].depth>depth+1) || (dm->no== 0)) {
			KanjiFntPrint("\n");
		} else {
			if(fl->lang) {
				KanjiFntPrint("%s\n", menu[di[dm->no]].sentj);
			} else {
				KanjiFntPrint("%s\n", menu[di[dm->no]].sente);
			}
		}

	} else {
		print_errmsg(fl->err,menu,fl->lang);
	}

	if (!fl->err && execno) /**/
		KanjiFntPrint("Now Loading...\n");

	KanjiFntFlush(-1);
}

static void print_help(int lang)
{

        KanjiFntPrint("\n");
        KanjiFntPrint("    Help of Sample Program Menu       \n");
	KanjiFntPrint(" „¡„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ"
	              "„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„¢\n");
	KanjiFntPrint(" „  › or START: Select               „ \n");
	KanjiFntPrint(" „      ª     : Up                   „ \n");
	KanjiFntPrint(" „      «     : Down                 „ \n");
	KanjiFntPrint(" „      ~     : Top                  „ \n");
	KanjiFntPrint(" „            : This HELP            „ \n");
	KanjiFntPrint(" „                                   „ \n");
	KanjiFntPrint(" „  SELECT    : Language ( %s )„ \n",
	               lang ? "Japanese" : "English " );
	KanjiFntPrint(" „¤„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ"
	              "„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„£\n");
	KanjiFntPrint("        (EXIT:   or ~)\n");

	KanjiFntFlush(-1);
}

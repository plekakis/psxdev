/* $PSLibId: Runtime Library Release 3.6$ */
/*****************************************************************
 *
 * file: card.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
#include "libps.h"
#include <sys/file.h>

#include "maxconf.h"
#include "maxtypes.h"

#include "psxload.h"
#include "spritex.h"
#include "menux.h"

#include "cardio.h"

#include "maxstat.h"

/*
#define DEBUG
#define DEBUG1
#define DUMP_ICON1
#define DEBUG0
#define DUMP_ICON0
*/

#include "menuset2.h"

#define	_ICONTEXT	0x80020000L

#define CPWIDTH		4

#define NULLSTRING1	"　　　　　　　　　　　　　　　　"
#define NULLSTRING2	"　　　　　　　　　　　　　　　　"
#ifdef EMOUSE
#define NULLSTRING3	"　　　　　　　　　　　　　　　　"
#define NULLSTRING4	"　　　　　　　　　　　　　　　　"
#else
#define NULLSTRING3	"ただ今コピーしています。しばらく"
#define NULLSTRING4	"お待ちください。　　　　　　　　"
#endif /* EMOUSE */

#define	TIMADDR		0x80180000
#define	TIMSIZE		32832

#define	TMPFNAM		"__tmp_file"
#define MEMORY_CARD	1
#define VCOUNT		60

#define OFFSET 2

static	SPRTX	StrSprites[2][4];
static	char	strsprtcnt;
int     SetStringTim(u_long *addr, u_char *str, int i);
void    SetStringSprt(int i);
void    SetStringUVclr(int flag);
void    SetStringUV(int current);

static	char*	EventMessage[7] = { NULL, NULL, NULL, "EXIT", "COPY", "COPY ALL", "DELETE" };
/************************************************
  Functions Definition
************************************************/
extern void mmemcpy(char *dst, char *src, int size);
extern void bcopy(char *src, char *dst, int size);
extern void bzero(char *src, int size);
extern	void	DefineMenuTable( void );

void	funcExit( long pad );
void	funcMenu( long pad );
#ifdef EMOUSE
void	funcIcon( long pad );
#else
static	void	funcIcon( long pad );
#endif /* EMOUSE */
void    funcMemCard( long pad );

static	void	IconGets( int side );
static	void	IconMapping( int side, int id, int mode );
static	void	IconRemove( int side, int id );
static	void	IconRemove2(int id);

extern  void    funcMemExit( long pad );

static void	funcInitCard(int side);
static int      funcMove();
static void     funcStringTim(int side);
static void     IconMapping2( int side, int id, int n);

static int      StringTimInitFlag;
/************************************************
  Memorys Definition
************************************************/
static	RECT	RectWrk;
static	SPRTX	Sprites[2][99];
static	int	spriteCount;

int	EventHistory = 0;
int	Target = 0;
#ifdef EMOUSE
int	Event = 0;
#else
static	int	Event = 0;
#endif /* EMOUSE */

/***********************************************************
  バックアップカード用変数
***********************************************************/
#define	_ICONMAX	15

#include "const.h"

#define	IXA	32
#define	IXB	458
#define	IY	125
#define WD      50
#define STRX    192
#define STRY    411

typedef struct {
    short	x;
    short	y;
} _POS;

_POS	IconPos[2][_ICONMAX] = {{
	{ IXA     , IY+WD*4 }, { IXA+WD, IY+WD*4 }, { IXA+WD*2, IY+WD*4 },
	{ IXA+WD*2, IY+WD*3 }, { IXA+WD, IY+WD*3 }, { IXA     , IY+WD*3 }, 
	{ IXA     , IY+WD*2 }, { IXA+WD, IY+WD*2 }, { IXA+WD*2, IY+WD*2 },
	{ IXA+WD*2, IY+WD   }, { IXA+WD, IY+WD   }, { IXA     , IY+WD   }, 
	{ IXA     , IY	    }, { IXA+WD, IY      }, { IXA+WD*2, IY      },
	}, {
	{ IXB     , IY+WD*4 }, { IXB+WD, IY+WD*4 }, { IXB+WD*2, IY+WD*4 },
	{ IXB+WD*2, IY+WD*3 }, { IXB+WD, IY+WD*3 }, { IXB     , IY+WD*3 },
	{ IXB     , IY+WD*2 }, { IXB+WD, IY+WD*2 }, { IXB+WD*2, IY+WD*2 },
	{ IXB+WD*2, IY+WD   }, { IXB+WD, IY+WD   }, { IXB     , IY+WD   },
	{ IXB     , IY      }, { IXB+WD, IY      }, { IXB+WD*2, IY      },
}};

extern  _XXX	xxx[];
extern  MAXSTAT maxstat;
extern	DB	*cdb;
extern  int	fot;

_FFF	FileList[2][16];
int	IconCount[2];
SPRTX	IconSprites[2][_ICONMAX*3];
static	int	FileEntry[2];
static	_CARD	CardInf[2][_ICONMAX];
static	int	ActiveSide = 0;
static	int	IconId[2] = { _ID_ICON_A, _ID_ICON_B };
static	u_long	*straddr;

typedef struct {
    int pos;
    int cnt;
    int sprt;
    int flag;
    int ent;
} CARDMV;

CARDMV  cardmv[2];
static int openingflag;
static int openingcnt;
static int vcount;
extern int funcokflag;
extern int cpokflag;
extern int opflag;
extern int bkcpflag;

RECT FreeRect[2], FreeClut[2];

static char	deletefnam[64];
static	int	blocknum[2];
static	int	Confirm;
static	char	copy_fnam[64];

/***********************************************************


***********************************************************/

/***********************************************************

	カード関係の処理ルーチンここから
	:Card-related subroutines start here

***********************************************************/

#define	CARDINF		512
#define	BUFMAX		128
#define TRBYTE		128
#define TRBYTE2		128
#define MAXRETRY	10

static void     CardPoll();
static void     CardOpenEvent();
static	int	GetCardInf( char* fnam, _CARD* cardinf );
static	int	CheckCard(int side);
static void	CloseCard();
static int	OSD_test_media( long chan );
static int	OSD_card_event_1();
static void	OSD_undeliv();

static	_CARD	CardInf2;
static	char	CardBuff[BUFMAX];

static 	long	ev0, ev1, ev2, ev3;
static long     OSD_TESTVAL[4] = {0, -2, -1, -2};
static	int	lastevent[2];


/***********************************************************
	カードの抜き差しをポーリングでチェックする
	:Use polling to check card operations
***********************************************************/
static void CardPoll()
{
	if (opflag == -1) {
		if (++vcount > VCOUNT) {
			vcount = 0;
			CheckCard(0);
			CheckCard(1);
		} 
	}
}

/***********************************************************
	カード関係のイベントをオープンする
	:Open card-related event
***********************************************************/
static void CardOpenEvent()
{
	EnterCriticalSection();
	ev0 = OpenEvent(SwCARD, EvSpIOE, EvMdNOINTR, NULL);
	ev1 = OpenEvent(SwCARD, EvSpERROR, EvMdNOINTR, NULL);
	ev2 = OpenEvent(SwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
	ev3 = OpenEvent(SwCARD, EvSpUNKNOWN, EvMdNOINTR, NULL);
	EnableEvent(ev0);
	EnableEvent(ev1);
	EnableEvent(ev2);
	EnableEvent(ev3);
	ExitCriticalSection();
}

/***********************************************************
	カードのヘッダ情報を読み込む。
	:Read card header info
***********************************************************/
static	int	GetCardInf( char* fnam, _CARD* cardinf )
{
    long	fd;
    int i,j;
    unsigned char *cp;
    
    fd = open( fnam,  O_RDWR );
    if( fd==-1 ) {
#ifdef DEBUG
	printf("GetCardInf: open %s error.\n", fnam);
#endif /* DEBUG */
	return( -1 );
    }
    
    if ((i = read( fd, (char*)cardinf, sizeof( _CARD ))) != sizeof(_CARD)) {
#ifdef DEBUG
	printf("GetCardInf: read %s error. ret=%d\n", fnam, i);
#endif /* DEBUG */
    	close( fd );
	return(-1);
    }
    close( fd );
    return( 0 );
}

/***********************************************************
	カードのコピールーチン
	128 byte に分割して read/write している
	取りあえずテンポラリファイルにコピーしておきコピー
	終了後に rename する
***********************************************************/
/***********************************************************
	Card copy routine
	perform read/write in 128-byte chunks
	Copy immediately to temporary file
	Rename after copying
***********************************************************/
int CardCopy2(int side, char *fnam, int bnum)
{
	static	int sfd = 0;
	static	int dfd = 0;
	static 	int rwflag;
	static	int trans;
	static	char cbuf[128];	
	static	char des_fnam[64];
	static	char des_card[5];
	char	src_fnam[64];
	char	tmp_fnam[64];
	char	src_card[5];
	int	i;
	int	retry;

	retry = 0;

	if (fnam) {
		if(!side) {
			strcpy(src_card, "bu00:");
			strcpy(des_card, "bu10:");
		} else {
			strcpy(src_card, "bu10:");
			strcpy(des_card, "bu00:");
		}
		strcpy(src_fnam, src_card);
		strcpy(des_fnam, des_card);
		strcpy(tmp_fnam, des_card);
		strcat(src_fnam, fnam);
		strcat(des_fnam, fnam);
		strcat(tmp_fnam, TMPFNAM);
		strcpy(copy_fnam, fnam);
#ifdef DEBUG
		printf("src=%s: des=%s\n", src_fnam, des_fnam);
#endif /* DEBUG */

		/*
		 * コピー元のファイルをオープンする
		 */
		/* :Open original (source) file*/
		if ((sfd = open(src_fnam,  O_RDONLY)) == -1) {
#ifdef DEBUG
			printf("open %s error.\n", src_fnam);
#endif /* DEBUG */
			sfd = 0;
			return(-1);
		}
readhead:
		/*
		 * コピー元のヘッダーを読み込む
		 */
		/* :Read header in original file*/
		if (read(sfd, &CardInf2, TRBYTE2) == -1) {
#ifdef DEBUG
			printf("read %s error.\n", src_fnam);
#endif /* DEBUG */
			if (++retry < MAXRETRY)
				goto readhead;
			close(sfd);
			sfd = 0;
			return(-1);
		}
		retry = 0;

		CardInf2.BlockEntry = bnum;

		/*
		 * テンポラリーファイルを消去する
		 */
		/* :Delete temporary file */
		delete(tmp_fnam);

		/*
		 * コピー先のファイルを作成する
		 *	    コピー中の抜き差し対策のためにテンポラリーファイル
		 *	    にコピーし、コピー終了後に rename する
		 */
		/* :Make duplicate file
		 * :Copy to temporary file for card operations during copying
		 * :Rename file after copying. */
		if ((dfd = open(tmp_fnam,  O_CREAT | CardInf2.BlockEntry << 16))
		     == -1) {
#ifdef DEBUG
			printf("open %s error.[%d]\n", des_fnam,CardInf2.BlockEntry);
#endif /* DEBUG */
			close(sfd);
			sfd = 0;
			return(-1);
		}
		close(dfd);
		/*
		 * コピー先のファイルをオープンする
		 */
		/* :Open duplicate (destination) file */
		if ((dfd = open(tmp_fnam,  O_WRONLY)) == -1) {
#ifdef DEBUG
			printf("open %s error2.\n", des_fnam);
#endif /* DEBUG */
			close(sfd);
			sfd = 0;
			if (delete(tmp_fnam) == 0) {
#ifdef DEBUG
				printf("delete error.\n");
#endif /* DEBUG */
			}
			return(-1);
		}
writehead:
		/*
		 * コピー先のファイルにヘッダーを書き込む
		 */
		/* :Write header to duplicate file*/
		if (write(dfd, &CardInf2, TRBYTE2) == -1) {
#ifdef DEBUG
			printf("write %s error.\n", des_fnam);
#endif /* DEBUG */
			if (++retry < MAXRETRY)
				goto writehead;
			close(sfd);
			close(dfd);
			sfd = 0;
			dfd = 0;
			if (delete(tmp_fnam) == 0) {
#ifdef DEBUG
				printf("delete error.\n");
#endif /* DEBUG */
			}
			return(-1);
		}
		trans = TRBYTE2;
		rwflag = 0;
	} else {
		if (sfd != 0) {
			if (!rwflag) {
readdata:
			/*
		 	 * コピー元のファイルから 128 byte 読み込む
		 	 */
			 /* :Read 128 bytes from original file */
				if (read(sfd, cbuf, TRBYTE) != TRBYTE) {
#ifdef DEBUG
					printf("read error.\n");
#endif /* DEBUG */
					if (++retry < MAXRETRY)
						goto readdata;
					close(dfd);
					close(sfd);
					sfd = 0;
					dfd = 0;
					strcpy(tmp_fnam, des_card);
					strcat(tmp_fnam, TMPFNAM);
					if (delete(tmp_fnam) == 0) {
#ifdef DEBUG
						printf("delete error.\n");
#endif /* DEBUG */
					}
					return(-1);
				}
				rwflag = 1;
			} else {
writedata:
			/*
		 	 * コピー先のファイルに 128 byte 書き込む
		 	 */
			/* :Write 128 bytes to duplicate file*/
				if (write(dfd, cbuf, TRBYTE) != TRBYTE) {
#ifdef DEBUG
					printf("write error.\n");
#endif /* DEBUG */
					if (++retry < MAXRETRY)
						goto writedata;
					close(dfd);
					close(sfd);
					sfd = 0;
					dfd = 0;
					strcpy(tmp_fnam, des_card);
					strcat(tmp_fnam, TMPFNAM);
					if (delete(tmp_fnam) == 0) {
#ifdef DEBUG
						printf("delete error.\n");
#endif /* DEBUG */
					}
					return(-1);
				}
				rwflag = 0;
				trans += TRBYTE;
			/*
		 	 * コピーが終了したなら rename する
		 	 */
			/* :Rename after copying */
				if (trans >= CardInf2.BlockEntry * 8192) {
					close(dfd);
					close(sfd);
					sfd = 0;
					dfd = 0;
					strcpy(tmp_fnam, des_card);
					strcat(tmp_fnam, TMPFNAM);
					i = rename(tmp_fnam, des_fnam);
#ifdef DEBUG
printf("rename: ret=%d, tmp_fnam=%s, des_fnam=%s\n", i, tmp_fnam, des_fnam);
#endif /* DEBUG */
					return(CardInf2.BlockEntry);
				}
				if ((trans % 8192) == 0) {
#ifdef DEBUG
printf("CardCopy: one block copy. trans=%d\n", trans);
#endif /* DEBUG */
					bkcpflag = -1;
				}
			}
		} else {
			return(0);
		}
		return(0);
	}
}

/***********************************************************
	カードの抜き差しをチェックする
	:Card check operations
***********************************************************/
static int CheckCard(int side)
{
	long    chan;
	int     id;
	int     ret;
	int     i;
	RECT    rect;

	if (side == 0) {
		chan = 0x00;
		id = _ID_ICON_A;
	} else {
		chan = 0x10;
		id = _ID_ICON_B;
	}

        ret = OSD_test_media(chan);
#ifdef DEBUG2
printf("CheckCard(%d): ret=%d\n", chan, ret);
#endif /* DEBUG2 */
	/*
 	 * 直前のイベントが TIMEOUT で今回が IOE であるなら
	 *	    NEWCARD である
 	 */
	/* :If previous event  is TIMEOUT and current event is IOE,
	 * :then it is a NEW CARD. */
	if ((ret == 0) && (lastevent[side] == -1)) {
		/* NEW CARD */
		ret = 1;
		lastevent[side] = 0;
	} else {
		lastevent[side] = ret;
	}
	/*
 	 * TIMEOUT
 	 */
        if (ret == -1) {
		/* カードがつながっていない場合の処理を行なう。 */
		/* :Perform processing when cards are not attached*/
	    if (FileEntry[side]) {
		SetSpritePriority(MENUgetSprtxPtr (_CONF_ID_YES), HIDE );
		SetSpritePriority(MENUgetSprtxPtr (_CONF_ID_NO),  HIDE );
		SetSpritePriority(MENUgetSprtxPtr (_CONF_SPRT_BALL_YES),HIDE );
		SetSpritePriority(MENUgetSprtxPtr (_CONF_SPRT_BALL_NO),	HIDE );
		SetSpritePriority(MENUgetSprtxPtr (_CONF_ID_COPY),HIDE );
		SetSpritePriority(MENUgetSprtxPtr (_CONF_ID_DELETE),HIDE );
		SetSpritePriority(MENUgetSprtxPtr (_CONF_ID_MESSAGE),HIDE );
#ifdef ENGLISH
		SetSpritePriority(MENUgetSprtxPtr (_CONF_ID_FILE),   HIDE );
		SetSpritePriority(MENUgetSprtxPtr (_CONF_ID_CARD),   HIDE );
#endif /* ENGLISH */
		SetSpritePriority(&maxstat.bgsprt[fot][_CONF_BG_PLATE],	HIDE );
		SetSpritePriority( MENUgetSprtxPtr (_ID_COPY), 4 );
		SetSpritePriority( MENUgetSprtxPtr (_ID_ACOPY), 5 );
		IconRemove2(id);
		while(MENUpullCurrentItem() != -1);
		MENUsetCurrentItem(EventHistory);
#ifdef EMOUSE
		EMouseSetCardHome();
#endif /* EMOUSE */
		Target = 0;
		SetStringUVclr(0);
		FileEntry[side] = 0;
		blocknum[side] = 0;
		funcokflag = 0;
	    }
	    return(-1);
        }
	/*
 	 * NEWCARD
 	 */
	if (ret == 1) {
		/* NEWCARD の場合の処理を行なう。 */
		/* :Perform NEW CARD processing*/
#ifdef DEBUG
		printf("new card!\n");
#endif /* DEBUG */
		Target = 0;
		CloseCard();
		ClearOTagR(cdb->ot, OTSIZE);
		SetUpPageItem(MEMORY_CARD);	
		StringTimInitFlag = 1;
		return(-2);
	}
	return(0);
}

/***********************************************************
	_card_info でカードの状態をチェックする
	:Use _card_info to check card status
***********************************************************/
static
int OSD_test_media( chan )
long chan;
{
#ifdef DEBUG2
	printf("OSD_test_media:chan=%d\n", chan);
#endif /* DEBUG2 */
	_new_card();
	if(_card_info( chan )==0)
		return -1;
	return OSD_TESTVAL[ OSD_card_event_1() ];
}

/***********************************************************
	イベント待ちルーチン
	:Event wait routine
***********************************************************/
static
int OSD_card_event_1()
{
#ifdef DEBUG2
	printf("OSD_test_event_1:\n");
#endif /* DEBUG2 */
	while(1) {
		if(TestEvent(ev0)==1) {  /* IOE */
			OSD_undeliv();
			return 0;
		}
		if(TestEvent(ev1)==1) {  /* ERROR */
			OSD_undeliv();
			return 1;
		}
		if(TestEvent(ev2)==1) {  /* TIMEOUT */
			OSD_undeliv();
			return 2;
		}
		if(TestEvent(ev3)==1) {  /* UNKNOWN */
			OSD_undeliv();
			return 3;
		}
	}
}

/***********************************************************
	イベントを undeliver する
	:Undeliver event
***********************************************************/
static
void OSD_undeliv()
{
    UnDeliverEvent(SwCARD, EvSpIOE);
    UnDeliverEvent(SwCARD, EvSpERROR);
    UnDeliverEvent(SwCARD, EvSpTIMOUT);
    UnDeliverEvent(SwCARD, EvSpUNKNOWN);
}

/***********************************************************
	カード関係のイベントをクローズする
	:Close card-related event
***********************************************************/
static void CloseCard()
{
	EnterCriticalSection();
	CloseEvent( ev0 );
	CloseEvent( ev1 );
	CloseEvent( ev2 );
	CloseEvent( ev3 );
	ExitCriticalSection();
}

/***********************************************************

	カード関係の処理ルーチンここまで
	Card-related processing routine goes to hear

***********************************************************/


/*/////////////////////////
// メインループ
// main loop
/////////////////////////*/
/* max.c の main loop から call される */
void funcMemCard( long pad )
{
    int	i, j;
    int	x, y;
  
    int	side;
    int	rside;
    int	id;
    int	tid;
    int	rtid;
    int	item;
    char	fnam[64];
    int   cnt;
    int   k;
    int   tnum;

    /*
      * カードの抜き差しをポーリングでチェックする
    */
    /* :Use polling to check card operations   */
    CardPoll();

    /* 最初の１回のみ初期化を行う */
    if (openingflag == 0) {
  	if (openingcnt == 0) {
	    funcInitCard(0);
	    funcInitCard(1);
	    StringTimInitFlag = 1;
#ifdef EMOUSE
	    EMouseSetCardHome();
#endif /* EMOUSE */
  	}
  	if (++openingcnt < 3)
	  return;
	openingflag = 1;
    }

    /* ICON のスプライトパケットを登録する */
    for ( i = 0; i < spriteCount; i++ ) {
	if( Sprites[fot][i].priority == HIDE ) continue;
      	AddSprite2( cdb, &Sprites[fot][i] );
    }
    
    if( Event==_ID_EXIT ) funcExit( pad );

    if( Event==_ID_MOVE ) {
        if (funcMove(0) == -1) {
	    return;
	}
        funcMove(1);
        return;
    }
    
    /* 操作と ICON 共に指定されている場合 */
    if( Event && Target ) {
#ifdef DEBUG
	printf( "%s ( Event No.%d) Item %d \n", EventMessage[Event], Event, Target );
#endif /* DEBUG */

	if( Confirm ) {
#ifdef EMOUSE
	    EMouseSetCardConfirm();
#endif /* EMOUSE */
	    SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_YES),     (u_long)2 );
	    SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_NO),      (u_long)2 );
	    SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_MESSAGE), (u_long)3 );
	    SetSpritePriority( MENUgetSprtxPtr (_CONF_SPRT_BALL_YES), (u_long)3 );
	    SetSpritePriority( MENUgetSprtxPtr (_CONF_SPRT_BALL_NO), (u_long)3 );
	    SetSpritePriority( &maxstat.bgsprt[fot][_CONF_BG_PLATE],   4 );
	    SetSpritePriority( MENUgetSprtxPtr (_ID_COPY), HIDE );
	    SetSpritePriority( MENUgetSprtxPtr (_ID_ACOPY), HIDE );
	    MENUpushCurrentItem ();
      
	    switch ( Event ) {
	      case _ID_COPY :	/* Copy */

		SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_COPY), 3);
#ifdef ENGLISH
		SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_FILE), 3);
#endif /* ENGLISH */
		MENUsetCurrentItem ( _CONF_ID_YES );
		break;
	      case _ID_ACOPY :	/* Copy All */
		SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_COPY), 3);
#ifdef ENGLISH
		SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_CARD), 3);
#endif /* ENGLISH */
		MENUsetCurrentItem ( _CONF_ID_YES );
		break;
	      case _ID_DELETE :	/* Delete */
		SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_DELETE), 3);
#ifdef ENGLISH
		SetSpritePriority( MENUgetSprtxPtr (_CONF_ID_FILE), 3);
#endif /* ENGLISH */
		MENUsetCurrentItem ( _CONF_ID_NO );
		break;
		default :
		break;
	    }
	    EventHistory = Event;
	    Event = 0;
	    Confirm = 0;
	    return;
	}
	
	MENUclrQueBuffer();
	MENUsetCurrentItem( Event );
	EventHistory = Event;
	Event = 0;
	Target = 0;
    }
}

/***********************************************************
  コールバック関数の記述
  Callback function description

***********************************************************/
/*//////////////////////////
//
//////////////////////////*/
/* Memory Card からの EXIT */
void	funcExit( long pad )
{

    if (funcokflag) {
	return;
    } else {
	if (pad & OKKey) {
	    CloseCard();
	    funcMemExit(pad);
	} else
	  if (pad & (_PADLup|_PADLdown)){
	      MENUidol2(pad);
	  }
    }
}
/*//////////////////////////
//
//////////////////////////*/
/*
  Memory Card からファイル名を読み出す。 FileList[side][].name
  Block 数を格納する                     FileEntry[side]
  File 数を格納する                      FileEntry[side]
*/
static void	funcInitCard(int side)
{
    struct	DIRENTRY d;
    char	flist[8];
    int	n = 0;
    
    if (side == 0)
      strcpy(flist, "bu00:");
    else
      strcpy(flist, "bu10:");
    
    if ((struct DIRENTRY *)firstfile(flist, &d) == &d) {
	do {
	    strcpy(FileList[side][n+1].name, d.name);
	    FileList[side][n+1].bnum = d.size/8192;   /* New Card Block Size */
#ifdef DEBUG
	    printf("name=%s, attr=%08x, size=%d, next=%d, head=%08x, sys=%08x\n",
		   d.name, d.attr,d.size,*d.next,(long)d.system);
#endif
	    n++;
	} while((struct DIRENTRY *)nextfile(&d) == &d);
    } else {
#ifdef DEBUG
	printf("Can't get file list from %s.\n", flist);
#endif /* DEBUG */
    }
    FileEntry[side] = n;
}
/*//////////////////////////
//
//////////////////////////*/
/* ICON を選択中の場合の処理 */
static int	funcMove(int side)
{
    char	flist[64];
    int	id;
    int	n;
    int	cnt;
    int	i;
    int	ii;
    
    if (Event == 0)
      return;

    if (cardmv[side].flag < 1) {
	cardmv[side].flag++;
	return;
    }

    if (CheckCard(side) == -2) {
	return(-1);
    }

    cardmv[side].flag = 0;
    /* ICON イメージの(再)初期化 */
    if ((cardmv[0].ent == -1) && (cardmv[1].ent == -1)) {
	funcStringTim(0);
	funcStringTim(1);
	Event = 0;
	return;
    }

    /* 一つもない場合*/
    if (cardmv[side].ent == -1) {
	return;
    }

    /* 存在する ICON の場合 */
    if (cardmv[side].ent <= FileEntry[side]) {
	if (side == 0) {
	    strcpy(flist, "bu00:");
	    id = _ID_ICON_A;
	} else {
	    strcpy(flist, "bu10:");
	    id = _ID_ICON_B;
	}
	n = cardmv[side].cnt - 1;
	strcat(flist, FileList[side][cardmv[side].ent].name);

	if (GetCardInf(flist, &CardInf[side][n])) {
	    /* Header 情報が読み出せなかった場合 */
	    for (i = cardmv[side].ent; i < FileEntry[side]; i++) {
		strcpy(FileList[side][i].name, FileList[side][i+1].name);
	    }
	    strcpy(FileList[side][i].name, "");
	    FileEntry[side]--;
	    return;
	}

	if ((CardInf[side][n].Type < 0x11) ||
	    (CardInf[side][n].Type > 0x13)) {
		/* Icon のタイプが不正な場合 */
#ifdef DEBUG
		printf("GetCardInf: bad type error.[%x]\n",CardInf[side][n].Type);
#endif /* DEBUG */
		for (i = cardmv[side].ent; i < FileEntry[side]; i++) {
		    strcpy(FileList[side][i].name, FileList[side][i+1].name);
		}
		strcpy(FileList[side][i].name, "");
		FileEntry[side]--;
		return;
	}

#ifdef DEBUG2
	printf("block size[%d][%d]=%d, CardInf=%d\n",
	       side, n, FileList[side][cardmv[side].ent].bnum,
	       CardInf[side][n].BlockEntry);
#endif /* DEBUG2 */

	/* New Card Block Size */
	CardInf[side][n].BlockEntry = FileList[side][cardmv[side].ent].bnum;

	if ((CardInf[side][n].BlockEntry <= 0) || 
	    (CardInf[side][n].BlockEntry > 15)) {
		/* Block 数が不正な場合 */
#ifdef DEBUG
		printf("GetCardInf: bad blockentry error.\n");
#endif /* DEBUG */
		for (i = cardmv[side].ent; i < FileEntry[side]; i++) {
		    strcpy(FileList[side][i].name, FileList[side][i+1].name);
		}
		strcpy(FileList[side][i].name, "");
		FileEntry[side]--;
		return;
	    }
#ifdef DEBUG1
	printf( "'%s' type[%x]  %d blocks\n", CardInf[side][n].Title, 
	       CardInf[side][n].Type, CardInf[side][n].BlockEntry );
#endif /* DEBUG1 */
	
	/* Block 数の読み出し FileList[side][].bnum */
	FileList[side][cardmv[side].ent].bnum = CardInf[side][n].BlockEntry;
	blocknum[side] += CardInf[side][n].BlockEntry;
	
	cnt =  CardInf[side][n].Type - 0x10;
	ii = cardmv[side].sprt;
	for( i=0; i<cnt; i++ ) {
	    PSXtimMake( (u_long*)(_ICONTEXT), 4, 16, 0,
		       CardInf[side][n].Icon[i], CardInf[side][n].Clut );
	    MakeSpriteData(&IconSprites[side][ii], 
			   (u_long*)(_ICONTEXT), 1);
	    SetSpriteSize( &IconSprites[side][ii], 32, 32 );
	    SetSpritePriority(&IconSprites[side][ii], OTSIZE/2+3 );
	    ii++;
	}
	IconMapping2(side, id, n);
	cardmv[side].sprt += cnt;
	cardmv[side].cnt++;
	cardmv[side].ent++;
    } else {
	IconCount[side] = cardmv[side].sprt;
	cardmv[side].ent = -1;
    }
}
/*//////////////////////////
//
//////////////////////////*/
/* 文書名の sprite を初期化する */
static void funcStringTim(side)
{
    	u_char	buf1[32*16+1], buf2[32*16+1];
	int	r1, r2;
	int	len;
	int 	i;

	bzero((char *)buf1, sizeof(buf1));
    	bzero((char *)buf2, sizeof(buf2));
    	if (side == 0) {
    		strcpy(buf1, NULLSTRING1);
    		strcpy(buf2, NULLSTRING2);
    	} else {
    		strcpy(buf1, NULLSTRING3);
    		strcpy(buf2, NULLSTRING4);
    	}
	for (i = 0; i < cardmv[side].cnt-1; i++) {
		if ((len = strlen(CardInf[side][i].Title)) < 32) {
			strncat(buf1, CardInf[side][i].Title, len);
			strcat(buf1, "\n");
			strcat(buf2, "\n");
	    	} else {
			strncat(buf1, CardInf[side][i].Title, 32);
			strncat(buf2, CardInf[side][i].Title+32,
				(len > 64) ? 32 : len-32);
			if( len < 64 )
	    			strcat(buf2, "\n");
		}
	}

	for (; i < 15; i++) {
		strcat(buf1, "\n");
		strcat(buf2, "\n");
	}
	/* 文書名の polygon の u,v を取り出す
	  FileList[side][].sprtnum, addr, u, v */
    	r1 = SetStringTim((u_long *)(TIMADDR + TIMSIZE*(side*2)),   buf1, 6);
    	r2 = SetStringTim((u_long *)(TIMADDR + TIMSIZE*(side*2+1)), buf2, 7);
    	for (i = 0; i < 16; i++) {
	    FileList[side][i].sprtnum = r1 << 8 | r2;
	    FileList[side][i].addr = (u_long *)(TIMADDR+r1*TIMSIZE);
	    FileList[side][i].u[0] = StrSprites[fot][0].poly.u0;
	    FileList[side][i].u[1] = StrSprites[fot][1].poly.u0;
	    FileList[side][i].v[0] = StrSprites[fot][0].poly.v0 + i * 16;
	    FileList[side][i].v[1] = StrSprites[fot][1].poly.v0 + i * 16;
	 }

	if (side == 0) {
	    /* 文書名の polygon を Sprites[fot][] にコピーする */
	    spriteCount += 2;	
	    bcopy((char *)&StrSprites[fot][0], 
		(char *)&Sprites[fot][spriteCount-2], sizeof(SPRTX));
	    bcopy((char *)&StrSprites[fot][1], 
		(char *)&Sprites[fot][spriteCount-1], sizeof(SPRTX));
	    setUVWH(&Sprites[fot][spriteCount-2].poly, 
	      FileList[side][0].u[0], FileList[side][0].v[0], 255, 15);
	    setUVWH(&Sprites[fot][spriteCount-1].poly, 
	      FileList[side][0].u[1], FileList[side][0].v[1], 255, 15);

	}
	funcokflag = 0;
}
/*//////////////////////////
//
//////////////////////////*/
/* Memory Card のボタンの callback */
void	funcMenu( long pad )
{
  int	id;
  int     i;
  
#ifdef DEBUG1
  printf("funcMenu() : pad = %08x", pad);
#endif

  /* Now copying or deleting */
  if (funcokflag) {
	return;
  }
  /*////////////////////
  // 確定
  ////////////////////*/

    if( pad & OKKey) { 

#ifdef DEBUG1
	printf("\tSELECT\n");
#endif

#ifdef EMOUSE
      	Event = MENUgetCurrentItem()%10;
#else
      	Event = MENUgetCurrentItem();
#endif /* EMOUSE */

      	id = MENUpullCurrentItem();
      	if( id<0 ) {
	    MENUpushCurrentItem();

	    if (MENUsetCurrentItem( _ID_ICON_A ) != -1) {
		SetStringUV(_ID_ICON_A);
#ifdef EMOUSE
		EMouseSetCardIconA();
#endif
	    } else if (MENUsetCurrentItem( _ID_ICON_B ) != -1) {
		SetStringUV(_ID_ICON_B);
#ifdef EMOUSE
		EMouseSetCardIconB();
#endif
#ifdef EMOUSE
	    } else {
		while(MENUpullCurrentItem() != -1);
#endif
	    }
	} else {
	    SetStringUVclr(0);
#ifdef EMOUSE
	    EMouseSetCardHome();
	    while(MENUpullCurrentItem() != -1);
#endif
	}
    }
    else if (pad & (_PADLup|_PADLdown)) {
	MENUidol2(pad);	
    }
    else if ((pad & _PADl) && (pad & _PADm) && (pad & _PADn) && (pad & _PADo)) {
#ifdef DEBUG
	printf("undelete %s ", deletefnam);
#endif /* DEBUG */
	if ((strncmp(deletefnam, "bu00:", 5) == 0) || 
	    (strncmp(deletefnam, "bu10:", 5) == 0)) {
		if (undelete(deletefnam) == 0) {
#ifdef DEBUG
			printf("error.\n");
#endif /* DEBUG */
			return;	
		}
#ifdef DEBUG
		printf("success.\n");
#endif /* DEBUG */
		CloseCard();
		SetUpPageItem(MEMORY_CARD);
	}
    }
}

void InitCardIcons ( int dev )
{
        int side;
	int i;

	fot = 0;

	if( dev == 0 ) {
	    Confirm = 0;
	    funcokflag = 1;
	    opflag = -1;
	    cpokflag = -1;
	    bkcpflag = -1;
	    vcount = 0;
	    spriteCount = 0;
	    strsprtcnt = 0;
	    straddr = 0;
	    openingflag = 0;
	    openingcnt = 0;
            bzero((char *)FileList, sizeof(FileList));
	    bzero(deletefnam, sizeof(deletefnam));
            for (i = 0; i < 2; i++) {
                cardmv[i].pos = 0;
                cardmv[i].cnt = 1;
                cardmv[i].sprt = 0;
                cardmv[i].flag = 0;
                cardmv[i].ent = 1;
		blocknum[i] = 0;
		lastevent[i] = 0;
            }

	    /*
	     * カード関係のイベントをオープンする
	     */
	    /* :Open card-related event */
	    CardOpenEvent();
	}

        if (dev)
                Event = _ID_MOVE;
        else
                Event = _ID_NOTHING;
	EventHistory = _ID_EXIT;

	/* get VRAM area for kanji strings */
	if (FindFreeArea(64, 256, 16, 1, &FreeRect[dev], &FreeClut[dev])) {
		UseTexMap(&FreeRect[dev]);
		UseClutMap(&FreeClut[dev]);
	}
	if (dev)
		delete("bu10:__tmp_file");
	else
		delete("bu00:__tmp_file");
}

/*//////////////////////////
//
//////////////////////////*/
/* ICON エリアでの操作 */
#ifdef EMOUSE
	void funcIcon( long pad )
#else
static	void funcIcon( long pad )
#endif /* EMOUSE */
{
    int	id;
    int	current;

    current = MENUgetCurrentItem();

#ifdef DEBUG1
    printf("funcIcon() : pad = %08x\tcurrent = %d\n", pad, current );
#endif

    if( pad & _PADLdown ) {
	do {
	    current--;
	    if (current == _ID_ICON_A - 1) {
		current++;
		break;
	    }
	    if (current == _ID_ICON_B -1) {
		current++;
		break;
	    }
	} while (MENUsetCurrentItem(current) < 0);
	SetStringUVclr(1);
	SetStringUV(current);
    }

    else if( pad & _PADLup ) {
	if(MENUsetCurrentItem(++current) < 0)
	  current--;
	SetStringUVclr(1);
	SetStringUV(current);
    }    

    else if( pad & (_PADk|_PADh)) {
	id = MENUpullCurrentItem();
	if( id<0 ) id = _ID_COPY;
#ifdef EMOUSE
	MENUsetCurrentItem( id % 10);
#else
	MENUsetCurrentItem( id );
#endif /* EMOUSE */
#ifdef EMOUSE
	while(MENUpullCurrentItem() != -1);
#endif /* EMOUSE */
	Event = 0;
	SetStringUVclr(0);
#ifdef EMOUSE
	EMouseSetCardHome();
#endif /* EMOUSE */
    }

    else if( pad & OKKey ) {
	Target = MENUgetCurrentItem();
	id = MENUpullCurrentItem();
	if( id<0 ) {
	    MENUpushCurrentItem();
	    MENUsetCurrentItem( EventHistory );
	}
	else
	  {
	      int i, ii;

	      if (id == _ID_ACOPY) {
		  for (i = 0; i < 15; i++) {
		      if (Target < 60)
			ii = _ID_ICON_A;
		      else
			ii = _ID_ICON_B;
		      MENUsetCurrentItem(ii+i);
		      MENUpushCurrentItem();
		  }
	      }
	      Confirm = 1;
#ifdef EMOUSE
	      EMouseSetCardConfirm();
#endif /* EMOUSE */
	  }
    }

    else if( pad & _PADLleft ) {
	if( current>=_ID_ICON_B ) {
	    current = _ID_ICON_A;
	    if (MENUsetCurrentItem(current) < 0);
	    else {
		SetStringUVclr(1);
		SetStringUV(current);
#ifdef EMOUSE
		EMouseSetCardIconA();
#endif /* EMOUSE */
	    }
	}
    }

    else if( pad & _PADLright ) {
	if( current<_ID_ICON_B ) {
	    current = _ID_ICON_B;
	    if (MENUsetCurrentItem(current) < 0);
	    else {
		SetStringUVclr(1);
		SetStringUV(current);
#ifdef EMOUSE
		EMouseSetCardIconB();
#endif /* EMOUSE */
	    }
	}
    }
}
/***********************************************************
  バックアップカード読み込み

***********************************************************/
/*//////////////////////////
//
//////////////////////////*/
static	void	IconMapping2( int side, int id, int n)
{
    _RMENU	RMI;
    int	ptr;
    int	cnt;
    int	i, j;
    int ii;
    
    RMI.group = 0;
    RMI.id = 0;
    RMI.tim = NULL;
    RMI.x = 320;
    RMI.y = 320;
#ifdef EMOUSE
    /* Enlarge Sencing Area of Memory Card ICONs */
    RMI.mw = DELTA+2;  
    RMI.mh = DELTA+2;
#endif /* EMOUSE */
    RMI.left = 0;
    RMI.right = 0;
    RMI.up = 0;
    RMI.down = 0;
    RMI.effect = 2;
    RMI.drawFunc = NULL;
    RMI.execFunc = funcIcon;
    RMI.pad = _PADLleft|_PADLright|_PADLup|_PADLdown|_PADk|_PADh|OKKey;
    
    RMI.group = side + 3;

    RMI.id = id + n;
    RMI.drawFunc = NULL;
    cnt = CardInf[side][n].Type - 0x10;
    ii = cardmv[side].sprt;
    for( i=0; i<CardInf[side][n].BlockEntry; i++, cardmv[side].pos++ ) {
	SetSpritePosition( &IconSprites[side][ii],
			  IconPos[side][cardmv[side].pos].x+25, 
			  IconPos[side][cardmv[side].pos].y+25 );
	ptr = MENUsetItem3( &RMI, &IconSprites[side][ii] );
#ifdef DEBUG1
printf("IconMapping() : MENU[%d] = %d\n", ptr, cardmv[side].pos);
#endif
	for( j=1; j<cnt; j++ ) {
	    MENUsetAnimationItem2( ptr, &IconSprites[side][ii+j] );
	}
    }
}
/*//////////////////////////
//
//////////////////////////*/
static	void	IconRemove( int side, int id )
{
	int	n;
	for( n=0; n<FileEntry[side]; n++ ) {
		MENUremoveItem( id + n );
	}
}
/*//////////////////////////
//
//////////////////////////*/
static	void	IconRemove2(int id)
{
	int	n;
	for( n=0; n<15; n++ ) {
		MENUremoveItem( id + n );
	}
}
/*//////////////////////////
//
//////////////////////////*/
static	void	IconRemove3( int side, int id )
{
	int	n;
	int	idmax;

	if (side == 0)
		idmax = _ID_ICON_A + 15;
	else
		idmax = _ID_ICON_B + 15;
	for( n=id; n<idmax; n++ ) {
		MENUremoveItem( n );
	}
}

static int DeleteFileList(int side)
{
  int i, j;

  for (i = 1; i < 16; i++) {
    if (strcmp(copy_fnam, FileList[side][i].name) == 0) {
	/* Delete した FileList[side][] をずらし NULL にする */
	for (j = i; j < 15; j++)
		bcopy((char *)&FileList[side][j+1], (char *)&FileList[side][j],
		      sizeof(_FFF));
	bzero((char *)&FileList[side][15], sizeof(_FFF));
	break;
    }
  }
}

static int CheckFileList(char *fnam, int side)
{
  int i;


  for (i = 1; i < 16; i++) {
      /* fname でFileList[side][].name を探す OK : -1, NG : 0 */
#ifdef DEBUG1
  printf("CheckFileList(%s) : FileList[%d][%d] sprtnum=%d, name=%s\n",
	 fnam, side, i, FileList[side][i].sprtnum, FileList[side][i].name );
#endif
    if (strcmp(fnam, FileList[side][i].name) == 0)
      return(-1);
  }
  return(0);
}

/*//////////////////////////
//
//////////////////////////*/
int SetStringTim(u_long *addr, u_char *str, int i)
{
    int	timsize;

    bzero((char *)addr, 8208*4);
    Krom2Tim(str, addr, 0, 0, 0, 0, 0xffff, 0x0000);
    if( strsprtcnt < 2 ) {
	UnuseTexMap(&FreeRect[strsprtcnt]);
	UnuseTexMap(&FreeClut[strsprtcnt]);
	MakeSpriteData(&StrSprites[fot][strsprtcnt], addr, 1);
	SetSpritePosition(&StrSprites[fot][strsprtcnt], STRX+256*(i-6), STRY);
	SetSpriteSize(&StrSprites[fot][strsprtcnt], 256, 32);
	SetSpriteShadeTex(&StrSprites[fot][strsprtcnt], 1);
	SetSpritePriority(&StrSprites[fot][strsprtcnt], 2);
    }

    return(strsprtcnt++);
}
/*//////////////////////////
//
//////////////////////////*/
/* 文書名の sprite をクリアする */
void SetStringUVclr(int flag)
{
    if( flag == 0 )
      SetStringUV(flag);
}
/*//////////////////////////
//
//////////////////////////*/
/* 文書名の sprite を置き換える */
void SetStringUV(int current)
{		
    int	side;
    int	rside;
    int	n;
    int	s1, s2;
    int	u[2], v[2];
    u_long *addr;
    RECT rect;
    int i, j;
    int tmpv[2];

    if (current == 0) {
	side = 0;
	rside = 1;
	n = 0;
	s1 = 0;
	s2 = 1;
    } else
    if (current == 1) {
	side = 1;
	rside = 0;
	n = 0;
	s1 = 2;
	s2 = 3;
    } else {
	if (current < _ID_ICON_B) {
	    side = 0;
	    rside = 1;
	    n = current - _ID_ICON_A + 1;
	} else {
	    side = 1;
	    rside = 0;
	    n = current - _ID_ICON_B + 1;
	}
	/* 現在の ICON の番号を取り出す FileList[side][].sprtnum */
	s1 = FileList[side][n].sprtnum >> 8 & 0xff;
	s2 = FileList[side][n].sprtnum & 0xff;
	if ((s1 == 0) && (s2 == 0)) {
	    s1 = 0;
	    s2 = 1;
	}
    }

    /* sprite の先頭アドレスが side と一致してなかったら再生成 */
    /* Card が抜かれた場合は再生成 */
#ifdef EEMOUSE
    if ( StringTimInitFlag ) {
	StringTimInitFlag = 0;
	for( j=0 ; j<2 ; j++ ) {
	    rect.x = Sprites[fot][spriteCount-(4-j*2)].px;
	    rect.y = Sprites[fot][spriteCount-(4-j*2)].py;
	    rect.w = Sprites[fot][spriteCount-(4-j*2)].pw;
	    rect.h = Sprites[fot][spriteCount-(4-j*2)].ph;
	    UnuseTexMap(&rect);
#ifdef DEBUG2
printf("unuseTex: (x,y,w,h)=(%d,%d,%d,%d)\n", rect.x, rect.y, rect.w, rect.h);
#endif /* DEBUG2 */
	    rect.x = Sprites[fot][spriteCount-(4-j*2)].cx;
	    rect.y = Sprites[fot][spriteCount-(4-j*2)].cy;
	    rect.w = Sprites[fot][spriteCount-(4-j*2)].cw;
	    rect.h = Sprites[fot][spriteCount-(4-j*2)].ch;
	    UnuseClutMap(&rect);
	    addr = (u_long *)(TIMADDR + TIMSIZE*j*2);
	    MakeSpriteData((SPRTX *)&StrSprites[fot][j*2], addr, 1);
	    SetSpritePosition((SPRTX *)&StrSprites[fot][j*2], STRX, STRY);
	    SetSpriteSize((SPRTX *)&StrSprites[fot][j*2], 256, 32);
	    SetSpriteShadeTex((SPRTX *)&StrSprites[fot][j*2], 1);
	
	    rect.x = Sprites[fot][spriteCount-(4-(j*2+1))].px;
	    rect.y = Sprites[fot][spriteCount-(4-(j*2+1))].py;
	    rect.w = Sprites[fot][spriteCount-(4-(j*2+1))].pw;
	    rect.h = Sprites[fot][spriteCount-(4-(j*2+1))].ph;
	    UnuseTexMap(&rect);
	    rect.x = Sprites[fot][spriteCount-(4-(j*2+1))].cx;
	    rect.y = Sprites[fot][spriteCount-(4-(j*2+1))].cy;
	    rect.w = Sprites[fot][spriteCount-(4-(j*2+1))].cw;
	    rect.h = Sprites[fot][spriteCount-(4-(j*2+1))].ch;
	    UnuseClutMap(&rect);
	    addr = (u_long *)(TIMADDR + TIMSIZE*(j*2+1));
	    MakeSpriteData((SPRTX *)&StrSprites[fot][(j*2+1)], addr, 1);
	    SetSpritePosition((SPRTX *)&StrSprites[fot][(j*2+1)], STRX+256, STRY);
	    SetSpriteSize((SPRTX *)&StrSprites[fot][(j*2+1)], 256, 32);
	    SetSpriteShadeTex((SPRTX *)&StrSprites[fot][(j*2+1)], 1);
	    
	    if (straddr == FileList[side][0].addr) {
		tmpv[0] = FileList[side][0].v[0];
		tmpv[1] = FileList[side][0].v[1];
	    } else {
		tmpv[0] = FileList[rside][0].v[0];
		tmpv[1] = FileList[rside][0].v[1];
	    }
	    for (i = 0; i < 16; i++) {
	    /* 反対側の ICON の場合、FileList[side][]u, v の再設定 */
		v[0] = FileList[side][i].v[0] - tmpv[0];
		v[1] = FileList[side][i].v[1] - tmpv[1];
		FileList[side][i].u[0] = StrSprites[fot][j*2].poly.u0;
		FileList[side][i].u[1] = StrSprites[fot][(j*2+1)].poly.u0;
		FileList[side][i].v[0] = StrSprites[fot][j*2].poly.v0 + v[0];
		FileList[side][i].v[1] = StrSprites[fot][(j*2+1)].poly.v0 + v[1];
#ifdef DEBUG2
	printf("%d:%d: u0=%d, u1=%d,  v0=%d, v1=%d\n", n, i, FileList[side][i].u[0], FileList[side][i].u[1], FileList[side][i].v[0], FileList[side][i].v[1]);
#endif /* DEBUG2 */
	    }
        }
	straddr = FileList[side][n].addr;
    }

#ifdef DEBUG2
    printf("\rstraddr=%08x, FileList[%d][%d].addr=%08x  s1=%d, s2=%d, spritCount=%d ",
	   straddr, side, n, FileList[side][n].addr, s1, s2, spriteCount);
#endif /* DEBUG2 */

    /* 現在の文書名を StrSprites[fot][] にコピーする */
    mmemcpy((char *)&StrSprites[fot][s1], (char *)&Sprites[fot][spriteCount-(4-s1)], sizeof(SPRTX));	
    mmemcpy((char *)&StrSprites[fot][s2], (char *)&Sprites[fot][spriteCount-(4-s2)], sizeof(SPRTX));	
    u[0] = FileList[side][n].u[0];
    u[1] = FileList[side][n].u[1];
    v[0] = FileList[side][n].v[0];
    v[1] = FileList[side][n].v[1];
    setUVWH(&Sprites[fot][spriteCount-(4-s1)].poly, u[0], v[0], 255, 15);
    setUVWH(&Sprites[fot][spriteCount-(4-s2)].poly, u[1], v[1], 255, 15);

    straddr = FileList[side][n].addr;

#else /* EMOUSE */

    if (straddr != FileList[side][n].addr) {
	rect.x = Sprites[fot][spriteCount-OFFSET].px;
	rect.y = Sprites[fot][spriteCount-OFFSET].py;
	rect.w = Sprites[fot][spriteCount-OFFSET].pw;
	rect.h = Sprites[fot][spriteCount-OFFSET].ph;
	UnuseTexMap(&rect);
#ifdef DEBUG2
printf("unuseTex: (x,y,w,h)=(%d,%d,%d,%d)\n", rect.x, rect.y, rect.w, rect.h);
#endif /* DEBUG2 */
	rect.x = Sprites[fot][spriteCount-OFFSET].cx;
	rect.y = Sprites[fot][spriteCount-OFFSET].cy;
	rect.w = Sprites[fot][spriteCount-OFFSET].cw;
	rect.h = Sprites[fot][spriteCount-OFFSET].ch;
	UnuseClutMap(&rect);
	addr = (u_long *)(TIMADDR + TIMSIZE*s1);
	MakeSpriteData((SPRTX *)&StrSprites[fot][s1], addr, 1);
	SetSpritePosition((SPRTX *)&StrSprites[fot][s1], STRX, STRY);
	SetSpriteSize((SPRTX *)&StrSprites[fot][s1], 256, 32);
	SetSpriteShadeTex((SPRTX *)&StrSprites[fot][s1], 1);
	
	rect.x = Sprites[fot][spriteCount-OFFSET+1].px;
	rect.y = Sprites[fot][spriteCount-OFFSET+1].py;
	rect.w = Sprites[fot][spriteCount-OFFSET+1].pw;
	rect.h = Sprites[fot][spriteCount-OFFSET+1].ph;
	UnuseTexMap(&rect);
	rect.x = Sprites[fot][spriteCount-OFFSET+1].cx;
	rect.y = Sprites[fot][spriteCount-OFFSET+1].cy;
	rect.w = Sprites[fot][spriteCount-OFFSET+1].cw;
	rect.h = Sprites[fot][spriteCount-OFFSET+1].ch;
	UnuseClutMap(&rect);
	addr = (u_long *)(TIMADDR + TIMSIZE*s2);
	MakeSpriteData((SPRTX *)&StrSprites[fot][s2], addr, 1);
	SetSpritePosition((SPRTX *)&StrSprites[fot][s2], STRX+256, STRY);
	SetSpriteSize((SPRTX *)&StrSprites[fot][s2], 256, 32);
	SetSpriteShadeTex((SPRTX *)&StrSprites[fot][s2], 1);
	
	if (straddr == FileList[side][0].addr) {
	    tmpv[0] = FileList[side][0].v[0];
	    tmpv[1] = FileList[side][0].v[1];
	} else {
	    tmpv[0] = FileList[rside][0].v[0];
	    tmpv[1] = FileList[rside][0].v[1];
	}
	for (i = 0; i < 16; i++) {
	    if (FileList[side][i].addr != straddr) {
		v[0] = FileList[side][i].v[0] - tmpv[0];
		v[1] = FileList[side][i].v[1] - tmpv[1];
		FileList[side][i].u[0] = StrSprites[fot][s1].poly.u0;
		FileList[side][i].u[1] = StrSprites[fot][s2].poly.u0;
		FileList[side][i].v[0] = StrSprites[fot][s1].poly.v0 + v[0];
		FileList[side][i].v[1] = StrSprites[fot][s2].poly.v0 + v[1];
#ifdef DEBUG2
	printf("%d:%d: u0=%d, u1=%d,  v0=%d, v1=%d\n", n, i, FileList[side][i].u[0], FileList[side][i].u[1], FileList[side][i].v[0], FileList[side][i].v[1]);
#endif /* DEBUG2 */
	    }
	}
	
	straddr = FileList[side][n].addr;
    }

#ifdef DEBUG2
    printf("\rstraddr=%08x, FileList[%d][%d].addr=%08x  s1=%d, s2=%d, spritCount=%d ",
	   straddr, side, n, FileList[side][n].addr, s1, s2, spriteCount);
#endif /* DEBUG2 */

    mmemcpy((char *)&StrSprites[fot][s1], (char *)&Sprites[fot][spriteCount-OFFSET], sizeof(SPRTX));	
    mmemcpy((char *)&StrSprites[fot][s2], (char *)&Sprites[fot][spriteCount-OFFSET+1], sizeof(SPRTX));	
    u[0] = FileList[side][n].u[0];
    u[1] = FileList[side][n].u[1];
    v[0] = FileList[side][n].v[0];
    v[1] = FileList[side][n].v[1];
    setUVWH(&Sprites[fot][spriteCount-OFFSET].poly, u[0], v[0], 255, 15);
    setUVWH(&Sprites[fot][spriteCount-OFFSET+1].poly, u[1], v[1], 255, 15);
#endif /* EMOUSE */
}

static int vsyf;

/* max.c の main loop から call される */
void CopyMemCard()
{
    int ret;
    int side;

    if (opflag != -1) {
        if (bkcpflag != -1) {
	    vsyf++;
	    ret = CardCopy2(0, 0, 0);
	 }
        else
	  return;
	if (opflag < _ID_ICON_B)
	    side = 0;
	else
	    side = 1;
	if (ret == -1) {
	    IconRemove3(side, opflag);
	    DeleteFileList(side);
	    opflag = -1;
	    cpokflag = -1;
	    funcokflag = 0;
	    bkcpflag = -1;
	    SetStringUVclr(0);
#ifdef DEBUG
printf("copyerr: blocknum[0]=%d, blocknum[1]=%d\n", blocknum[0], blocknum[1]);
#endif /* DEBUG */
	} else
	if (ret > 0) {
	    blocknum[side] += ret;
#ifdef DEBUG
printf("copyend: blocknum[0]=%d, blocknum[1]=%d\n", blocknum[0], blocknum[1]);
#endif /* DEBUG */
	    FileEntry[side]++;
	    opflag = -1;
	    bkcpflag = -1;
	    if (cpokflag == -1) {
		funcokflag = 0;
	    	SetStringUVclr(0);
	    }
	}
    }
}

void MemCardDelete()
{
  int	i;
  int	side;
  int	tid;
  char	fnam[64];

      /* DELETE */
      if (Target < _ID_ICON_B) {
	side = 0;
	tid = Target-_ID_ICON_A;
	strcpy(fnam, "bu00:");
	strcat(fnam, FileList[side][tid+1].name);
      } else {
	side = 1;
	tid = Target-_ID_ICON_B;
	strcpy(fnam, "bu10:");
	strcat(fnam, FileList[side][tid+1].name);
      }
      if (delete(fnam) == -1) {
#ifdef DEBUG
	printf("delete error.\n");
#endif /* DEBUG */
	return;
      }
      strcpy(deletefnam, fnam);

      /* Delete の後処理 */
      blocknum[side] -= FileList[side][tid+1].bnum;
#ifdef DEBUG
	printf("delete: blocknum[%d]=%d\n", side, blocknum[side]);
#endif /* DEBUG */

      /* FileList[side][] をずらす */
      for (i = tid; i < 14; i++)
	bcopy((char *)&FileList[side][i+2], (char *)&FileList[side][i+1], sizeof(_FFF));
      /* FileList[side][15] は NULL */
      bzero((char *)&FileList[side][15], sizeof(_FFF));

      MENUsetItemEffect(Target, 0x10);
      Target = 0;
      SetStringUVclr(0);
      FileEntry[side]--;
      funcokflag = 1;
}

void MemCardCopy()
{
  int	i, j, k;
  int	side;
  int	rside;
  int	id;
  int	tid;
  int	item;
  char	fnam[64];
  int   cnt;
  int   tnum;
  int	cpwidth;

      bzero(deletefnam, sizeof(deletefnam));
      /* COPY */
      if (Target < _ID_ICON_B) {
	side = 0;
	rside = 1;
	tnum = Target - _ID_ICON_A;
	if (CheckFileList(FileList[side][tnum+1].name, 1) == -1)
	  return;
	strcpy(fnam, FileList[side][tnum+1].name);
	tid = _ID_ICON_B;
      } else {
	side = 1;
	rside = 0;
	tnum = Target - _ID_ICON_B;
	if (CheckFileList(FileList[side][tnum+1].name, 0) == -1)
	  return;
	strcpy(fnam, FileList[side][tnum+1].name);
	tid = _ID_ICON_A;
      }
      if ((blocknum[rside] + FileList[side][tnum+1].bnum) > 15) {
#ifdef DEBUG
	printf("No space left on side(%d), block:%d\n", rside, blocknum[rside]);
#endif /* DEBUG */
	return;
      }
      for (i = 0, cnt = 0 ; i < 15; i++) {
	if ((k = GetItemFromID3(tid+i)) == 0)
	  break;
	cnt += k;
      }
      if (i == 15)
	return;

      /* FileList[side][] を反対側に(rside) にコピーする */
      bcopy((char *)&FileList[side][tnum+1], (char *)&FileList[rside][i+1], sizeof(_FFF));

      id = CopyTarget(Target, i);

      for(j = 0; j < GetItemNumFromID(id); j++) {
	  item = GetItemFromID2(id, j);
	  xxx[item].x = IconPos[rside][cnt+j].x + WD/2;
	  xxx[item].y = IconPos[rside][cnt+j].y + WD/2;
/*
	  CopySprite(item, xxx[item].x, xxx[item].y);
*/
	  cpwidth = (abs(xxx[item].x - xxx[item].sx) + 
		abs(xxx[item].y - xxx[item].sy)) / 128 + 1;
	  if (xxx[item].x > xxx[item].sx)
	    xxx[item].dx = cpwidth;
	  else
	    xxx[item].dx = -cpwidth;
	  if (xxx[item].y > xxx[item].sy)
	    xxx[item].dy = cpwidth;
	  else
	    xxx[item].dy = -cpwidth;
      }
      MENUsetItemEffect(id, 0x20);
      Target = 0;
      funcokflag = 1;
}

void MemCardACopy()
{
  int	i, j, k;
  int	side;
  int	rside;
  int	id;
  int	tid;
  int	rtid;
  int	item;
  char	fnam[64];
  int   cnt;
  int   tnum;
  int	tmpblock = 0;
  int	cpwidth;

      bzero(deletefnam, sizeof(deletefnam));
      /* COPY ALL */
      if (Target < _ID_ICON_B) {
	side = 0;
	rside = 1;
	tid = _ID_ICON_A;
	rtid = _ID_ICON_B;
      } else {
	side = 1;
	rside = 0;
	tid = _ID_ICON_B;
	rtid = _ID_ICON_A;
      }
#ifdef DEBUG
   printf("acopy: blocknum[0]=%d, blocknum[1]=%d\n", blocknum[0], blocknum[1]);
#endif /* DEBUG */
      for (i = 0; i < 15; i++) {
	if (GetItemFromID(tid+i) == -1)
	  continue;
	if (CheckFileList(FileList[side][i+1].name, rside) == -1)
	  continue;
	for (j = 0, cnt = 0 ; j < 15; j++) {
	  if ((k = GetItemFromID3(rtid+j)) == 0)
	    break;
	  cnt += k;
        }
	if (j == 15)
	  break;
        tmpblock += FileList[side][i+1].bnum;
      }
      if ((blocknum[rside] + tmpblock) > 15) {
#ifdef DEBUG
	printf("No space left on side(%d), tmpblock:%d\n", rside, tmpblock);
#endif /* DEBUG */
	return;
      }
      for (i = 0; i < 15; i++) {
	if (GetItemFromID(tid+i) == -1)
	  continue;
	if (CheckFileList(FileList[side][i+1].name, rside) == -1)
	  continue;
	for (j = 0, cnt = 0 ; j < 15; j++) {
	  if ((k = GetItemFromID3(rtid+j)) == 0)
	    break;
	  cnt += k;
        }
	if (j == 15)
	  break;

        /* FileList[side][] を反対側に(rside) にコピーする */
	bcopy((char *)&FileList[side][i+1], (char *)&FileList[rside][j+1], sizeof(_FFF));

	id = CopyTarget(tid+i, j);

	if (id == -1)
	  continue;

	for(k = 0; k < GetItemNumFromID(id); k++) {
	    item = GetItemFromID2(id, k);
	    xxx[item].x = IconPos[rside][cnt+k].x + WD/2;
	    xxx[item].y = IconPos[rside][cnt+k].y + WD/2;
/*
	    CopySprite(item, xxx[item].x, xxx[item].y);
*/
	    cpwidth = (abs(xxx[item].x - xxx[item].sx) + 
		abs(xxx[item].y - xxx[item].sy)) / 128 + 1;
	    if (xxx[item].x > xxx[item].sx)
		xxx[item].dx = cpwidth;
	    else
		xxx[item].dx = -cpwidth;
	    if (xxx[item].y > xxx[item].sy)
	    	xxx[item].dy = cpwidth;
	    else
	   	 xxx[item].dy = -cpwidth;
	}
	MENUsetItemEffect(id, 0x20);
        funcokflag = 1;
      }
      Target = 0;
}

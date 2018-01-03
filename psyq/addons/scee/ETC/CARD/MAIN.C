
#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>
#include <sys/types.h>
#include <stdio.h>
#include <strings.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libmath.h>
#include <ctype.h>

#define K_MSG_X 40
#define K_MSG_Y 40
#define K_MSG_W 550
#define K_MSG_H 300

#define MAX_CHARS 16*40

#define BUF_SIZE   200
#define MEM_DEV0   "bu00:"
#define MAX_CARD_NAME 40

#define FAIL 0
#define OK   1

extern unsigned short ascii2sjis(unsigned char ascii_code);
extern struct DIRENTRY *firstfile(), *nextfile();

typedef struct {
	char	Magic[2];
	char	Type;
	char	BlockEntry;
#define TITLE_LEN 64
	char	Title[TITLE_LEN];
	char	reserve[28];
	char	Clut[32];
	char	Icon[3][128];
} card_header ;

void list_card(), check_card();
void asc2kanji(char *asc_str, char *kan_str) ;
int gametitle(char *fname, char *title, int *ascflag);
int isasc(char *title);
long dir_file(char *drv, struct DIRENTRY *d);

unsigned long ev0,ev1,ev2,ev3;
unsigned long ev10,ev11,ev12,ev13;
int k_sid;				/* Kanji Stream ID */

main()
{
	RECT area;
	DRAWENV	draw;			/* drawing environment */
	DISPENV	disp;			/* display environment */

	ResetCallback();
	ResetGraph(0);	
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */

    ev0 = OpenEvent(SwCARD, EvSpIOE, EvMdNOINTR, NULL);
    ev1 = OpenEvent(SwCARD, EvSpERROR, EvMdNOINTR, NULL);
    ev2 = OpenEvent(SwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
    ev3 = OpenEvent(SwCARD, EvSpNEW, EvMdNOINTR, NULL);
    ev10 = OpenEvent(HwCARD, EvSpIOE, EvMdNOINTR, NULL);
    ev11 = OpenEvent(HwCARD, EvSpERROR, EvMdNOINTR, NULL);
    ev12 = OpenEvent(HwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
    ev13 = OpenEvent(HwCARD, EvSpNEW, EvMdNOINTR, NULL);

	InitCARD(1);
	StartCARD();
	_bu_init();

	SetDefDrawEnv(&draw, 0,   0, 640, 480);
	SetDefDispEnv(&disp, 0,   0, 640, 480);

	draw.dfe = 1;
	PutDrawEnv(&draw);
	PutDispEnv(&disp);

    setRECT(&area,0,0,640,480);     /* Clear The whole Screen */
	ClearImage(&area,0,0,50);

	k_sid = KanjiFntOpen(K_MSG_X, K_MSG_Y, K_MSG_W, K_MSG_H, 640, 0, 0, 480, 0, MAX_CHARS);
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	setRECT(&area,K_MSG_X,K_MSG_Y,K_MSG_W,K_MSG_H);  /* Box for drawing */

	DrawSync(0);
    VSync(0);
	
    EnableEvent(ev0);
    EnableEvent(ev1);
    EnableEvent(ev2);
    EnableEvent(ev3);
    EnableEvent(ev10);
    EnableEvent(ev11);
    EnableEvent(ev12);
    EnableEvent(ev13);

	KanjiFntFlush(k_sid);

	DrawSync(0);

    check_card();

	return(0);				
}

void
check_card()
{
    long ret;
	int  i;

    while (1) {
	   _card_info(0x00);
	   ret = _card_event();
		switch(ret) {
			case 0:
				KanjiFntPrint("FORMATTED\n");
				list_card();
				break;
			case 2:
				KanjiFntPrint("NO CARD\n");
				break;
			case 3:
				KanjiFntPrint("UNFORMATTED\n");
				break;
			case 1:
			default:
				KanjiFntPrint("ERROR\n");
				break;
		}
	    KanjiFntFlush(-1);

		for (i = 0; i < 200 ; i++)
			VSync(0);
	}
}

_card_event()
{
        while(1) {
		if(TestEvent(ev0)==1) {         /* IOE */
                        return 0;
                }
                if(TestEvent(ev1)==1) {         /* ERROR */
                        return 1;
                }
                if(TestEvent(ev2)==1) {         /* TIMEOUT */
                        return 2;
                }
                if(TestEvent(ev3)==1) {         /* NEW CARD */
                        return 3;
                }
        }
}

long
dir_file(char *drv, struct DIRENTRY *d)
{
    long i;
    char key[128];


	strcpy(key,drv);
	strcat(key,"*");

    i = 0;
	if(firstfile(key, d)==d)
	{
        do {
            i++;
			d++;
        } while(nextfile(d)==d);
    }
	return i;
}

void
list_card()
{
	int  i;
	int ascflag;
	struct DIRENTRY d[15];
    long ret;
	char buf[BUF_SIZE];

	if ((ret = dir_file(MEM_DEV0,&d[0])) == 0)
	{
		KanjiFntPrint("\n  NO FILE\n");
	}
	else
	{
	    if (ret == 1)
		    KanjiFntPrint(k_sid,"1 File\n");
	    else
		    KanjiFntPrint(k_sid,"%d Files\n",ret);

		for(i=0;i<ret;i++) {

			if (gametitle(d[i].name,buf, &ascflag) == OK)
			{
		        if (ascflag == OK)
		            KanjiFntPrint(k_sid,"A%2d:%s\n",i+1,buf);
		        else
		            KanjiFntPrint(k_sid,"K%2d:%s\n",i+1,buf);
            }
			else
		        KanjiFntPrint(k_sid,"K%2d:?%s?\n",i+1,d[i].name);
		        
		    printf("%2d:%s (%s)\n",i+1,d[i].name, buf);
        }
	}
}	

# define ALNUM_THRESHOLD 4
/*
** Attempt to guess if the string is ASCII - i.e. all printable, no SHIFT-JIS characters.
*/
int
isasc(char *title)
{
	int len, alnumcount = 0;
	char *c;

	/*
	** Make sure we don't get carried away if the title is a full Shift-JIS name !
	** (ie no NULL terminator).
	*/
    len = strlen(title);

    if (len > TITLE_LEN)
		len = TITLE_LEN;

    for (c = title ; c < (title + len) ; c++)
	    printf("x%x ", *c);

    for (c = title ; c < (title + len) ; c++)
	{

        if (isprint(*c) == 0)
		    return(FAIL);
        else
		{
		    if ((*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <= 'z')
		    		|| (*c >= '0' && *c <= '9'))
			    alnumcount++;
			/*
			** Extra Negative Test to filter out Shift-JIS
			*/
            if (*c == 0x82 || *c == 0x60)
			    alnumcount--;
            }
    }

	/*
	** Secondary Check - check for a minimum number of alphanumerics
	*/

    if (alnumcount >= ALNUM_THRESHOLD)
        return(OK);
    else
        return(FAIL);
}

void
asc2kanji(char *asc_str, char *kan_str)
{
    
    char *end;
	unsigned short *kan;

    kan = (unsigned short *)kan_str;
    end = asc_str + strlen(asc_str) ; 

    while(asc_str < end)
	{
	    *kan = ascii2sjis(*asc_str);
		kan++;
		asc_str++;
    }
}	

/*
** Function: gametitle
**
** Returns the Memory Card Title, of the requested filename
**
** Returns OK/FAIL
*/
int
gametitle(char *fname, char *title, int *ascflag)
{
    int fd;
	char card_filename[MAX_CARD_NAME];
	card_header card_info;

	sprintf(card_filename,"%s%s",MEM_DEV0,fname);
	
    if ((fd=open(card_filename,O_RDONLY))>=0)
	{     
	    read(fd,(char *)&card_info,sizeof(card_info));
	    close(fd);
	}
    else
	{
	    return(FAIL);
	}
    bzero(title,TITLE_LEN);

    if ((*ascflag = isasc(card_info.Title)) == OK)
	    memcpy(title,card_info.Title,16);
	else
	    memcpy(title,card_info.Title,62);

	return(OK);
}


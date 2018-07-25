/*
** WRITCRD.C
**
** Program to save a game, using SHIFT-JIS titles.
**
** 28-feb-1996 (dave)
**	First Written
** 29-feb-1996 (pholman)
**	Restructured
*/

/********** includes *******************************************************/
#include <kernel.h>
#include <sys/file.h>
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include <libcd.h>

/*
** External Functions
*/
extern unsigned short ascii2sjis(unsigned char ascii_code);


/********** defines *******************************************************/
#define port0 0x00
#define port1 0x10	 

#define CARD_READY 0
#define CARD_FAIL  1
#define CARD_NONE  2
#define CARD_NEW   3

#define CARD_TITLE "WIZZ BANG - LEVEL 1"
#define SAVE_NAME  "bu00:TESTD"

/********** types **********************************************************/
typedef struct 
{
	char					Magic[2];
	char					Type;
	char					BlockEntry;
#define TITLE_LEN 64
	unsigned char		Title[TITLE_LEN];
	char					reserve[28];
#define CLUT_SIZE 32
	char					Clut[CLUT_SIZE];
#define ICON_SIZE 128
	char					Icon[3][ICON_SIZE];
}  _CARD;



unsigned short clut[] = { 0xff, 0xe0, 0x0f, 0xF4,
								  0xff, 0xe0, 0x0f, 0xF4,
								  0xff, 0xe0, 0x0f, 0xF4,
								  0xff, 0xe0, 0x0f, 0xF4  };	 

/*
** simple image of an exclamation mark ( 16*16 4 bit 128 bytes ) 
**/
unsigned long image[] = {0x88888888,0x88888888,0xA8888888,0x8888888A,
								 0xAA888888,0x888888AA,0xAA888888,0x888888AA,	 
								 0xAA888888,0x888888AA,0xAA888888,0x888888AA,
								 0xAA888888,0x888888AA,0xA8888888,0x8888888A,
								 0xA8888888,0x8888888A,0x88888888,0x88888888,
								 0xA8888888,0x8888888A,0xA8888888,0x8888888A,
								 0x88888888,0x88888888,0x88888888,0x88888888,
		 						 0x88888888,0x88888888,0x88888888,0x88888888,
								 };

/*
** Globals
*/
unsigned long 		ev0,ev1,ev2,ev3,ev10,ev11,ev12,ev13;
_CARD hed;

void
main()
{
    ResetCallback();                     /* Reset all of the callbacks.      */
    ResetGraph(0);                       /* Reset the GPU.                   */
    
    
    printf("Memory Card : Kanji demo\n");
    
    EnterCriticalSection();
    ev0 = OpenEvent(SwCARD,EvSpIOE,EvMdNOINTR,NULL);
    ev1 = OpenEvent(SwCARD,EvSpERROR,EvMdNOINTR,NULL);
    ev2 = OpenEvent(SwCARD,EvSpTIMOUT,EvMdNOINTR,NULL);
    ev3 = OpenEvent(SwCARD,EvSpNEW,EvMdNOINTR,NULL);
    
    ev10 = OpenEvent(SwCARD,EvSpIOE,EvMdNOINTR,NULL);
    ev11 = OpenEvent(SwCARD,EvSpERROR,EvMdNOINTR,NULL);
    ev12 = OpenEvent(SwCARD,EvSpTIMOUT,EvMdNOINTR,NULL);
    ev13 = OpenEvent(SwCARD,EvSpNEW,EvMdNOINTR,NULL);
    ExitCriticalSection();
    
    InitCARD(1);
    StartCARD();
    _bu_init();
    
    EnableEvent(ev0);
    EnableEvent(ev1);
    EnableEvent(ev2);
    EnableEvent(ev3);
    
    EnableEvent(ev10);
    EnableEvent(ev11);
    EnableEvent(ev12);
    EnableEvent(ev13);
    
    savegame();
}
    
    
/*****************************************************************************/
int
savegame()
{
    long i, fd;
	char *ptr;
    
    unsigned char ascii_title[TITLE_LEN/2];  /* Since title is double-byte */
    char load_buff[20000];

    union
    {
	    unsigned short kanji;   
		struct 
		{
			char low;
		    char high;
        } k_word;
    } kanji;
    
    /*
	** Set up header information
	*/
    hed.Magic[0] = 'S';
    hed.Magic[1] = 'C';
    hed.Type = 0x11;
    hed.BlockEntry = 1;
    
    memcpy( hed.Clut, clut, CLUT_SIZE );
    memcpy( hed.Icon[0], image, ICON_SIZE);  /* Only One Icon */
    
    
    for (i = 0 ; i<ICON_SIZE; i++)
    	hed.Title[i] = 0x00;
    
	/*
	** Now convert title from ASCII to SHIFT-JIS - remembering that the PS
	** is little endian (ie reverse the bytes).
	*/
    
    strcpy(ascii_title,CARD_TITLE);
    printf("Save Game with Title \"%s\"\n",ascii_title);

    for (i = 0, ptr = hed.Title ; ascii_title[i] != NULL ; i++)
    {
	    kanji.kanji =  ascii2sjis(ascii_title[i]);

        *ptr++ = kanji.k_word.high;
        *ptr++ = kanji.k_word.low;
    }
    
    if((fd=open(SAVE_NAME,O_WRONLY))>=0)
    {
    	printf("ERROR: that file already exists \n");
    	return 0;
    }
     
    if((fd=open(SAVE_NAME,O_CREAT|(hed.BlockEntry<<16)))==-1)
    {	
    	printf("ERROR: could not create file \n");
    	return 0;
    }
    else
    {
    	close(fd);
    	printf("\n");
    }
    
    printf("** SAVE TO MEM CARD **\n");
    
    
    if ((fd=open(SAVE_NAME,O_WRONLY))>=0)
    {
     	memcpy(load_buff,&hed,sizeof(_CARD));
    	if ((i = write(fd,&load_buff[0],sizeof(_CARD))) != (sizeof(_CARD))) 
    	{
    		printf("i === %d   actual === %d \n",i,sizeof(_CARD));
    		printf("Error failed whilst writing header\n"); 
    		close(fd);
    		return 0;
    	}
    }
    close(fd);
    printf("OK, my work here is done !");
    
    return 1;
}
/****************************************************************************/

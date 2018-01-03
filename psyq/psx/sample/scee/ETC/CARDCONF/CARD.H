
/*****************************************************************************/
/*****************************************************************************/
/** card.h 																						 **/
/*****************************************************************************/
/*****************************************************************************/


#define PORT0 0x00	/*this is for pad1*/
#define PORT1 0x10	/*this is for pad2*/ 


#define IOE 		0 	/** card is initialised and ready 			**/
#define ERROR 		1 	/** some sort of error            			**/
#define TIMEOUT	2 	/** no card or comms error        			**/
#define NEWCARD 	3 	/** card id uninitialies or is unformated **/


typedef struct 
	{
	char				Magic[2];			/* always "SC "*/
	char				Type;					/* 0x11 1 icon 0x12 2icons 0x13 3icons*/
	char				BlockEntry;			/* number of 8k slots used*/
	unsigned short	Title[32];			/* title 32 chars SJIS*/
	char				reserve[28];		/* don't use this bit*/
	unsigned short	Clut[16];			/* 4bit clut */
	char				Icon[3][16][8];	/* 3*16*16*4bit icon bitmaps*/
	}_CARD;



/************ data handling buffers *******************************************/

typedef union 
		{
		char 	text[sizeof(_CARD)];
		_CARD header;
		}BUFFER;

BUFFER buff;


typedef struct
{
_CARD header;
char  body [((15*8192)-sizeof(_CARD))];
}CARD_DATA;

typedef union
{
CARD_DATA   card_data;
char        buff[(15*8192)];
}CARD_BUFFER;
/******************************************************************************/

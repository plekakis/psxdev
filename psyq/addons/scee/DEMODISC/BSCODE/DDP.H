#ifndef __DDP_H
#define __DDP_H

#define BOOT               0

#define INTERACTIVE        0
#define ATTRACT            1
#define ERROR              2

#define MAX_ARGS           4

#define NUM_DDP_MENUS      8
#define MAX_DD_OPTIONS     36

typedef struct
 {              
	 DRAWENV draw;            /* drawing environment */
	 DISPENV disp;            /* display environment */
  }
DB;

#define PAL_MAGIC       256

#define REBOOT_FLAG     (unsigned char *)0x8000df00
#define APPDATA_POS     (DDPData *)0x8000df08

#define BOOT_STRING     "PSDEMO"

#define BOOT_CODE       0x89

#define TYPE_SCEI       0x01
#define TYPE_SCEE       0x02

#define CD_DEV_NAME     "cdrom:"

typedef struct
  {
    char state, selected;
    char menus[NUM_DDP_MENUS];
    char mode, boottype;
    char loadGame, cdb;
    CdlLOC pos;
    int args[MAX_ARGS];
    char* name;
    char booted;
    char randpos;
  }
DDPData;

#define PIRATE_TRACK    0
#define PIRATE_MINUTE   0
#define PIRATE_SECOND   3
#define PIRATE_SECTOR  	102

#define FSM_TRACK    0
#define FSM_MINUTE   0
#define FSM_SECOND   2
#define FSM_SECTOR   35  /* was 105 as second file */

#define SYNC_RATE    50     /* We are PAL - hurrah. */


#define INPUT_DELAY  10

#define TRANS_DELAY  30    /* At least 15 vblanks between transitions */

#define IDLE_TIMEOUT 9000   /* 3 minute idle to attract mode */

#define REBOOT       600    

#define BOOTTIMEOUT  500    /* BS boot timeout */
#define SYNCDELAY    10     /* VSyncs to add to CD sync. */  

#define CREDITS_PASSES 10


#define FORM1_SIZE               2048 /* Size of an XA mode 1 form 1 sector. */

#define Sectors(x) ((x+FORM1_SIZE-1)/FORM1_SIZE)

#endif

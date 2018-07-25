/******************************************************************************/
/******************************************************************************/
/* main.h  ** for new memory card project thingy **                           */
/******************************************************************************/
/******************************************************************************/


/**-------------------- file names -----------------------------------------**/

/* structure used to "cache" filestructs so don't have to keep using         */
/* cdsearch file.  reduce seeks, reduce loading times                        */

typedef struct
{
char cd_filename[50];
CdlFILE fp;       	
}FILEDATA;

#define NUM_FILES 34  /* remember to update this */         

#define POS_MAINBACKFILENAME  0  
#define POS_DWARNINGFILENAME  1
#define POS_FIRSTBCKFILENAME  2
#define POS_BACKFILENAME 	   3
#define POS_FONTFILENAME 	   4 
#define POS_GAICFILENAME 	   5
#define POS_VARIFILENAME 	   6
#define POS_SAVEFILENAME 	   7
#define POS_MSICFILENAME 	   8
#define POS_WAITFILENAME 	   9
#define POS_ALERT1FILENAME   10	 
#define POS_ALERT2FILENAME   11	 
#define POS_ALERT3FILENAME   12	 
#define POS_ALERT4FILENAME   13	 
#define POS_POWERLINE1 	     14
#define POS_COPYRGHT 	     15
#define POS_POWERLINE2 	     16
#define POS_DDERBY 	        17
#define POS_FIRSTELMFILENAME 18
#define POS_SONYPRESENTS     19 
#define POS_START            20 
#define POS_ACOMBAT          21 
#define POS_KRAZY            22 
#define POS_LEMM3D           23 
#define POS_TEKKEN           24 
#define POS_WIPEOUT          25 
#define POS_VH               26 
#define POS_VB               27 
#define POS_CREDIT           28 
#define POS_POWERLINE3 	     29
#define POS_RRACER           30 
#define POS_DWORLD           31 
#define POS_KBLOOD           32 
#define POS_JFLASH           33 




/** have you updated num_files ?? **/


/*
#define MAINBACKFILENAME 	"\\TIM\\MAINBACK.TIM;1"  

#define DWARNINGFILENAME 	"\\TIM\\DWARNING.TIM;1"  

#define FIRSTBCKFILENAME 	"\\TIM\\FIRSTBCK.TIM;1"  

#define BACKFILENAME 	   "\\TIM\\BACKDROP.TIM;1"  
#define FONTFILENAME 	   "\\TIM\\ALPHA.TIM;1"
#define GAICFILENAME 	   "\\TIM\\GAMEICON.TIM;1"  
#define VARIFILENAME 	   "\\TIM\\VARIOUS.TIM;1"  
#define SAVEFILENAME 	   "\\TIM\\SAVE.TIM;1"  
#define MSICFILENAME 	   "\\TIM\\MAINSPRT.TIM;1"  
#define WAITFILENAME 	   "\\TIM\\WAITSPRT.TIM;1"  


#define ALERT1FILENAME 	   "\\ALERT\\ALERT1.TIM;1"  
#define ALERT2FILENAME 	   "\\ALERT\\ALERT2.TIM;1"  
#define ALERT3FILENAME 	   "\\ALERT\\ALERT3.TIM;1"  
#define ALERT4FILENAME 	   "\\ALERT\\ALERT4.TIM;1"  



#define POWERLINE1 	      "\\TIM\\TEST1.TIM;1"  
#define POWERLINE2 	      "\\TIM\\TEST2.TIM;1"  
#define POWERLINE3 	      "\\TIM\\TEST3.TIM;1"  

*/



#define FALSE 0
#define TRUE 1
#define ERROR_RECOVER 2
#define FORMAT 3




/**-------------------- cd ------------------------------------------------**/
#define FORM1_SIZE     2048 			  
#define Sectors(x) ((x+FORM1_SIZE-1)/FORM1_SIZE)




/**-------------------- display -------------------------------------------**/
#define	FRAME_X		320		
#define	FRAME_Y		256

#define SCREEN_X     1
#define SCREEN_Y     18
#define PAL_MAGIC    256

#define OTSIZE 50

typedef struct 
	{     
	DRAWENV     draw;                      /* drawing environment     */
	DISPENV     disp;                      /* display environment     */
	POLY_F4		screen_fade_mask;				/* game icons          		*/
	DR_MODE		screen_fade_mask_mode; 

//	POLY_F4		power_fade_mask;				/* this is shit but I don't	*/
//	DR_MODE		power_fade_mask_mode;      /* care. The whole project is shit*/


	POLY_FT4		first_icon;				   /* first screen arrow         */
	POLY_FT4		first_name;				   /* first screen text    		*/

	POLY_FT4		game_icon[10];				   /* game icons          		*/
	POLY_F4		game_icon_mask[10];		   /* game mask          		*/
	DR_MODE		game_icon_mask_mode[10];	/* game mask          		*/

	POLY_FT4		main_sprt;		         /* game mask          		*/
	POLY_FT4		main_new_sprt;		      /* game mask          		*/


/* for the card manager*/

	POLY_FT4		patch;				         /* covers del/quit/format  */
	POLY_FT4		card_1_slot[15];				/* slot icons          		*/
	POLY_FT4		slot_border[15];				/* slot boxes          		*/
	POLY_FT4		wait_sprite;				

	POLY_FT4		yes_button;		
	POLY_FT4		yes_text;		

	POLY_FT4		no_button;		
	POLY_FT4		no_text;		

	POLY_FT4		message_text;		

	POLY_FT4		confirm_button;		


	POLY_FT4		text[32];		            /* this is for new_text routine */

//--------- new card manager

	POLY_FT4		option_message_delete;		
	POLY_FT4		option_message_quit;		
	POLY_FT4		option_message_format;		












/* for the save screen */

	POLY_FT4		save[5];		               /* these are the sliders with save on them*/
	POLY_FT4		savebar[2];		            /* this is the bit that holds the bar */
	POLY_FT4		savebar_body[3];		      /* this is the save bar itself, made of beginning, middle and end */
	POLY_FT4		save_message;		         /* this is the save message               */
   POLY_FT4    alert_button;              /* button flare            */


	u_long      ot[OTSIZE];                /* ordering table          */
	} DB;
/**-------------------------------------------------------------------------**/


/**--------------------- tim loader -----------------------------------**/
typedef struct
	{
	u_long		id;
	u_long		flag;
	u_long		bnum;	 /** pixel section (nu,ber of bytes including header )**/
	u_short		dy;	 /** ypos in vram   **/
	u_short		dx;	 /** xpos in vram   **/
	u_short		h;		 /** height in vram **/
	u_short		w;		 /** width in vram  **/
	u_short		pixel[256][256];
	} _TIM16;

typedef union 
		{
		_TIM16 tim_struct;
		char   tim_buffer[sizeof(_TIM16)];
		}TIM;
	
/**-------------------------------------------------------------------------**/




#define DELETE 1
#define QUIT   2
#define FORMAT 3

#define DELETE    1
#define OP_FAILED 2
#define SURE      3


#define YES_BUTTON 1
#define NO_BUTTON  2

#define DELETE_BUTTON  3
#define QUIT_BUTTON    4
#define FORMAT_BUTTON  5

#define AL1A_OK   6
#define AL1B_OK   7
#define AL2A_OK   8
#define AL2B_OK   9
#define AL3A_OK  10
#define AL3B_OK  11
#define AL1B_NO  12
#define AL2A_NO  13
#define AL2B_NO  14
#define AL3A_NO  15
#define AL3B_NO  16
#define AL4A_NO  17
#define AL4B_NO  18

#define SAVE_OK           11
#define CARD_FULL         12
#define NO_FORMAT         21
#define FORMAT_SURE       22
#define OVERWRITE         31
#define OVERWRITE_SURE    32
#define NO_CARD           41
#define FAILED            42

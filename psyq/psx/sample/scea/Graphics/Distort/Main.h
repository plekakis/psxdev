/***********************************************************************/
/***********************************************************************/
/* main.h */
#ifndef MAIN_HEADER
#define MAIN_HEADER


/** graphics overheads **/

#define	FRAME_X		320		
#define	FRAME_Y		240

#define OTSIZE		4096
#define OT_FRONT	1
#define OT_BACK		OTSIZE-1



typedef struct 
	{     
	DRAWENV     draw;                      /* drawing environment     */
	DISPENV     disp;                      /* display environment     */
	u_long      ot[OTSIZE];                /* ordering table          */
	} DB;


/** constants **/
#define TRUE  1
#define FALSE 0




#endif
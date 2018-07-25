

/** graphics overheads **/

#define	FRAME_X		320		
#define	FRAME_Y		240

#define OT_LENGTH       13          
#define OTSIZE		(1<<OT_LENGTH)	

typedef struct 
	{     
	DRAWENV     draw;                      /* drawing environment     */
	DISPENV     disp;                      /* display environment     */
	POLY_G3		windscreen[10];
	u_long      ot[OTSIZE];                /* ordering table          */
	} DB;


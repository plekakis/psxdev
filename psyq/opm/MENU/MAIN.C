/*-------------------------------------------------**
**                                                 **
**  *		Load Exec - Sample Menu                **
**  *		Copyright (C) 1993 by Sony Corporation **
**  *			 All rights Reserved               **
**  *                                              **
** 	**** 	July.27.1997  B.Dawson                 **
**                                                 **
**                                                 **
**-------------------------------------------------*/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*#define DEBUG */

 /* Double-buffering structure*/
typedef struct {
	DRAWENV		draw;		/* drawing environment*/
	DISPENV		disp;		/* display environment */
} DB;

/* parse controller */
static int  pad_read(int n,int* CurrentApp);	

/* callback for VSync*/
static void  cbvsync(void);	

#ifdef LINKED_STARTUP
int main(int argc, int *argv)
#else
int main()
#endif
{
int* CurrentApp, ExecMode, NumApps, FirstTime;

	/* double buffer */
	DB	db[2];		
	
	/* current double buffer */
	DB	*cdb;		

	/* selected child app*/
	int	app = -1;	
	
	int	i,cnt;	/* work */

   #ifdef LINKED_STARTUP
     CurrentApp = argv[0];
     ExecMode   = argv[1];
     NumApps    = argv[2];
     FirstTime  = argv[3];
   #endif

    /*Halt interrupts and Flush Cache to avoid executing
    the old code*/
    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();

	/*ReEnable Callbacks */
    ResetCallback();	
	/* reset graphics system (0:cold,1:warm); */
	ResetGraph(0);		
	/* reset PAD */
    PadInit(0);	

    /* set debug mode (0:off,1:monitor,2:dump) */
	SetGraphDebug(0);	
	
    /*Let the world know we made it in!!!*/
    printf("\nIn Menu...");
  	
    /* set callback*/
//	VSyncCallback(cbvsync); 

	/* inititlalize environment for double buffer*/
   	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

     /*set clear buff prior to drawing*/
    db[0].draw.isbg = db[1].draw.isbg = 1; 
   
   	/*Load font data into frame buffer*/
    FntLoad(960, 256);	
	
	/**/
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));	

	/* nable to display */
	SetDispMask(1);		/* 0:inhibit,1:enable:*/

	while ((app = pad_read(app,CurrentApp)) < 0) {
		/* swap double buffer ID */
		cdb  = (cdb==db)? db+1: db;	

		/* wait for any GPU commands to operations*/
        DrawSync(0);		
		
		/* cnt = VSync(1);	/* check for count */
		/* cnt = VSync(2);	/* wait for V-BLNK (1/30) */
		cnt = VSync(0);		/* wait for V-BLNK (1/60) */

	    /* update display environment*/
		PutDispEnv(&cdb->disp); 
		
		/* update drawing environment*/
		PutDrawEnv(&cdb->draw); 
	  	
        /*print menu text to screen */
      	FntPrint("\n************ M E N U ***********\n");  
	  	FntPrint("\nPress X to Launch App 1");
	  	FntPrint("\n\nPress TRIANGLE to Launch App 2");
	  	FntPrint("\n\nPress CIRCLE to Launch App 3");
        FntFlush(-1);
	}
   	
    printf("\nBye-Bye...");
   	
    /*Stop pad interrupts*/
    PadStop();

    /*Hal triggering of registerd  callbacks*/		
	StopCallback();	

	return(0);
}


/*  Read controll-pad */
static int pad_read(int n,int* CurrentApp)		
{
	/* Get pad button status */
    u_long	padd = PadRead(1);		

	 /*if the X button*/
    if(padd & PADRdown)   
    {
    	*CurrentApp=1;	/* set child to be launched */
		return 0;
    }
    
    /*if the triangle button */
    if(padd & PADRup)  	
    {
    	*CurrentApp=2;	/* set child to be launched */
		return 1;
    }
	
    if(padd & PADRright)  	
    {
    	*CurrentApp=3;	/* set child to be launched */
		return 1;
    }
	
    return(-1);
}

/* callback */
static void cbvsync(void)
{
	/* print absolute VSync count */
	FntPrint("V-BLNK(%d)\n", VSync(-1));	
}


/******************END OF MAIN.C*************************/		   

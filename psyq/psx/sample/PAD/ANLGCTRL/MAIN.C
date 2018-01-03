/* $PSLibId: Run-time Library Release 4.4$ */
/*			  extap.c
 *
 *	Actuator accessing for expanded-protocol controllers
 *
 *	Copyright (C) 1997 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00        August 1,1997	Honda
 *	1.01        October 1,1997	Honda	added gun test
 *	1.10        January 13,1998	Honda	tab fix
 *
 *
 * --------------------------- key bindings -------------------------------
 *	L1:actuator 0 level -1
 *	L2:actuator 0 level +1
 *	R1:actuator 1 level -10
 *	R2:actuator 1 level +10
 *	square : suspend/resume communication with target port 
 *	circle: terminal type alternation switch lock/unlock
 *	left:  set terminal type to 16-button
 *	right: set terminal type to analog-controller
 *
 * ------------------------- display on screen ----------------------------
 *  [MULTI TAP]		controllers connected directly to port : [DIRECT]
 *			connected through multi tap : [MULTI TAP]
 *  1A [EX]=7		current terminal type
 *  G=0300,156		Gun target position (only with gun controller:ID=3)
 *  B=FFFF		button state (for each bit  1:released 0:pushed)
 *  A=80,80,80,80	analog level (4 channels)
 *  ID(4,7)		IDs of supported terminal types
 *  AC(1,2,0,10)=0	actuator primitives:
 *			act 0: continuous-rotary-vibrator, fast-rotation,
 *				current supply: 100mA, data length: 1bit,
 *				current value: 0
 *    (1,1,1,20)=90	act 1: continuous-rotary-vibrator, slow-rotation,
 *				current supply: 200mA, data length: 1byte,
 *				current value: 90 
 *  CM(0,1)		Actuators that can be controlled simultaneously are 
 *			act0 and act1. 
 *  SW(UNLOCK)		terminal type alternation switch is available
 *
 **/

#include <r3000.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "libpad.h"
#include <kernel.h>


typedef struct
{
	DRAWENV		draw;		/* drawing environment */
	DISPENV		disp;		/* display environment */
} DB;

#define MultiTap	1
#define CtrlTypeMtap	8
#define PortPerMtap	4

#define BtnM0		1
#define BtnM1		2
#define BtnMode		4
#define BtnStart	8
#define BtnEnable	0x10


typedef struct
{
	/* button state at last V-Sync*/
	unsigned char Button;

	/* lock/unlock mode for setting terminal type alternation button */
	unsigned char Lock;

	/* value for setting actuators*/
	unsigned char Motor0,Motor1;

	/* flag for "already called PadSetActAlign()" */
	unsigned char Send;

	/* target coordinate of gun (which use interrupt) */
	int X,Y;
} HISTORY;


static HISTORY history[2][4];
static unsigned char padbuff[2][34];
int padEnable = 3;

typedef struct
{
	short Vert;
	short Horz;
} POINT;

typedef struct
{
	unsigned char Port;
	unsigned char Size;
	POINT Pos[10];
} GUNDATA;

GUNDATA gunPos;


void dispPad(int, unsigned char *);
int setPad(int, unsigned char *);
void sprintf();
void *bzero(unsigned char *, int);
int PadChkMtap(int);


/*#main--- main routine */
int main()
{
	DB db[2],*cdb=0;

	bzero((unsigned char *)history, sizeof(history));
	bzero((unsigned char *)padbuff, sizeof(padbuff));

	ResetCallback();

#if MultiTap
	PadInitMtap(padbuff[0],padbuff[1]);
#else
	PadInitDirect(padbuff[0],padbuff[1]);
#endif
	PadInitGun((unsigned char *)&gunPos, 10);
	PadStartCom();

	ResetGraph(0);		
	SetGraphDebug(0);	

	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(4, 20, 312, 240, 0, 1024));	

	db[0].draw.isbg = 1;
	setRGB0(&db[0].draw, 60, 120, 120);
	db[1].draw.isbg = 1;
	setRGB0(&db[1].draw, 60, 120, 120);
	SetDispMask(1);

	VSync(0);
	while (TRUE)
	{
		cdb  = (cdb==db)? db+1: db;	

		if (setPad(0,padbuff[0]) == 1){
			break;
		};
		setPad(0x10,padbuff[1]);

		/* display controller state on port 1 */
		dispPad(0,padbuff[0]);

		/* display controller state on port 2 */
		dispPad(0x10,padbuff[1]);

		VSync(0);		/* wait for V-BLNK (1/60) */

		/* alternate double buffers */
		PutDispEnv(&cdb->disp);
		PutDrawEnv(&cdb->draw); 
		FntFlush(-1);
	}
	PadRemoveGun();
        PadStopCom();
        ResetGraph(3);
        StopCallback();
        return 0;
}


/*#setPad : analyze button state to act expanded-protocol controller functions */
int setPad(int port, unsigned char *rxbuf)
{
	HISTORY *hist;
	int button,count;

	if(rxbuf[1]>>4 == CtrlTypeMtap)
	{
		for(count=0;count<PortPerMtap;++count)
		{
			if (setPad(port+count, rxbuf + 2 +count*8) == 1){
				VSync(2);
				return 1;
			}
		}
		return 0;
	}

	/* ignore received data when communication failed */
	if(*rxbuf)
	{
		return 0;
	}

	button = ~((rxbuf[2]<<8) | rxbuf[3]);
	hist = &history[port>>4][port & 3];
	if (button & PADselect ){
		if (PadInfoMode(port,2,0) != 0){
                	PadSetMainMode(port,0,2);
                	VSync(2);
                	while (PadGetState(port) != 6){
                	}
		}
                return 1;
        }

#if 0
	/* suspend/resume opposite port of communication */
	if(!(hist->Button & BtnEnable) && button & PADRleft)
	{
		padEnable ^= (1 << (!(port>>4)));
		PadEnableCom(padEnable);
	}
#endif

	/* Confirmation for reloading controller information by calling 
         PadStopCom(), PadStartCom() */
	if(!(hist->Button & BtnStart) && button & PADstart)
	{
		PadStopCom();
		PadStartCom();
	}

	if(PadInfoMode(port,InfoModeCurExID,0))
	{
		/* set actuator 0 level of expanded-protocol controller */
		if(!(hist->Button & BtnM0))
		{
			if(button & PADL1 && hist->Motor0)
			{
				hist->Motor0 -= 1;
			}
			else if(button & PADL2 && hist->Motor0 < 255)
			{
				hist->Motor0 += 1;
			}
		}

		/* set actuator 1 level of expanded-protocol controller */
		if(!(hist->Button & BtnM1))
		{
			if(button & PADR1 && hist->Motor1)
			{
				hist->Motor1 -= 10;
			}
			else if(button & PADR2 && hist->Motor1 < 246)
			{
				hist->Motor1 += 10;
			}
		}
		/* alternate terminal type and lock/unlock switch state */
		if(!(hist->Button & BtnMode))
		{
			if(button & PADLleft)
			{
				PadSetMainMode(port,0,hist->Lock);
			}
			else if(button & PADLright)
			{
				PadSetMainMode(port,1,hist->Lock);
			}
			else if(button & PADRright)
			{
				switch(hist->Lock)
				{
					case 0:
					case 2:
						hist->Lock = 3;
						break;
					case 3:
						hist->Lock = 2;
						break;
				}
				PadSetMainMode(port,
					PadInfoMode(port,InfoModeCurExOffs,0),
					hist->Lock);
			}
		}
	}
	else
	{
		/* set actuator level of SCPH-1150 */
		if(!(hist->Button & BtnM0))
		{
			if(button & PADL1 && hist->Motor0)
			{
				hist->Motor0 = 0x40;
				hist->Motor1 -= 1;
			}
			else if(button & PADL2 && hist->Motor0 < 255)
			{
				hist->Motor0 = 0x40;
				hist->Motor1 += 1;
			}
		}
	}

	/*store button state of this V-Sync period*/
	if(button & (PADLright|PADLleft|PADRright))
		hist->Button |= BtnMode; else hist->Button &= ~BtnMode;
	if(button & (PADL1 | PADL2))
		hist->Button |= BtnM0; else hist->Button &= ~BtnM0;
	if(button & (PADR1 | PADR2))
		hist->Button |= BtnM1; else hist->Button &= ~BtnM1;
	if(button & PADRleft)
		hist->Button |= BtnEnable; else hist->Button &= ~BtnEnable;
	if(button & PADstart)
		hist->Button |= BtnStart; else hist->Button &= ~BtnStart;
	return 0;
}


/*#dispPad : display controller state */
void dispPad(int port, unsigned char *rxbuf)
{
	unsigned char buff[20];
	int count,pos,initlevel,maxnum;
	HISTORY *hist;

	/* Specify actuator alignment parameter (it should be decided  
          after checking the actuator primitives, but for simplicity 
          pre-defined data is used) */
	static unsigned char align[6]={0,1,0xFF,0xFF,0xFF,0xFF};

	if(rxbuf[1]>>4 == CtrlTypeMtap)
	{
		for(count=0;count<PortPerMtap;++count)
		{
			dispPad(port+count, rxbuf + 2 +count*8);
		}
		return;
	}

	hist = &history[port>>4][port & 3];
	initlevel = PadGetState(port);

	/* ------------------------------------------------------------ */
	/*              display multitap/direct connection  */
	/* ------------------------------------------------------------ */

	if(!(port & 0xF))
	{
		if(PadChkMtap(port))
		{
			FntPrint("[~c686multi tap~c888]");
		}
		else
		{
			FntPrint("[~c686direct~c888]");
		}
		if( ( !(port & 0x10) && !(padEnable & 1) ) ||
		    (  (port & 0x10) && !(padEnable & 2) ) )
		{
			FntPrint("~c866 suspend~c888");
		}
	}

	FntPrint("\n");

	/* ------------------------------------------------------------ */
	/*             display port number */
	/* ------------------------------------------------------------ */

	if(PadChkMtap(port))
	{
		FntPrint("%d%c",(port>>4)+1,'A'+ (port & 0xF));
	}
	else
	{
		FntPrint("%d",(port>>4)+1);
	}

	/* ------------------------------------------------------------ */
	/* display controller type and ID */
	/* ------------------------------------------------------------ */

	/* no controller connected */
	if(initlevel==PadStateDiscon)
	{
		FntPrint("[~c866none~c888]\n\n\n");
		return;
	}

	if(PadInfoMode(port,InfoModeCurExID,0))
	{
		/* expanded-protocol controller connected */
		FntPrint("[~c668ex~c888]=~c686%x~c888 ",
			PadInfoMode(port,InfoModeCurExID,0));
	}
	else
	{
		/* controller (not expanded-protocol) connected */
		FntPrint("[~c668ctrl~c888]=~c686%x~c888 ",
			PadInfoMode(port,InfoModeCurID,0));
	}

	/* ------------------------------------------------------------ */
	/* To the controller, set buffer pointer for setting actuator level,
	   and set data alignment of actuator level(only for 
           expanded-protocol) */
	/* ------------------------------------------------------------ */

	/* clear flag "already called PadSetActAlign" when reloading 
	   actuator information due to terminal type switch or
	   connected controller change */
	if(initlevel == PadStateFindPad)
	{
		hist->Send = 0;
	}

	/* Set buffer pointer for setting actuator level and data alignment
	   of actuator level, upon completion of actuator information 
           loading  */
	if(!hist->Send)
	{
		PadSetAct(port,&hist->Motor0,2);

		/* for non-expanded protocol */
		if(initlevel == PadStateFindCTP1)
		{
			hist->Send = 1;
		}
		/* for expanded controller (wait until finish loading 
		   actuator information) */
		else if(initlevel == PadStateStable)
		{
			/* set flag when accepted */
			if(PadSetActAlign(port,align))
			{
				hist->Send = 1;
			}
		}
	}

	/* ------------------------------------------------------------ */
	/*            display coordinate of gun target */
	/* ------------------------------------------------------------ */

	if(PadInfoMode(port, InfoModeCurID, 0) == 3)
	{
		if(gunPos.Port == port && gunPos.Size)
		{
			hist->X = hist->Y = 0;
			for(count=0; count<gunPos.Size; ++count)
			{
				hist->X += gunPos.Pos[count].Horz;
				hist->Y += gunPos.Pos[count].Vert;
			}
			hist->X /= gunPos.Size;
			hist->Y /= gunPos.Size;
		}
		sprintf(buff,"%04d,%03d",hist->X,hist->Y);
		FntPrint("g=~c686%s~c888 ",buff);
	}

	/* ------------------------------------------------------------ */
	/*    display button state and level of analog channels */
	/* ------------------------------------------------------------ */

	FntPrint("b=~c686%x~c888 ", rxbuf[2] << 8 | rxbuf[3]);
	switch(PadInfoMode(port,InfoModeCurID,0))
	{
		case 1:
			sprintf(buff,"%02x,%02x",rxbuf[4],rxbuf[5]);
			FntPrint("m=~c686%s~c888\n",buff);
			break;
		case 2:
			sprintf(buff,"%02x,%02x,%02x,%02x",
				rxbuf[4],rxbuf[5],rxbuf[6],rxbuf[7]);
			FntPrint("a=~c686%s~c888\n",buff);
			break;
		case 6:
			sprintf(buff,"%3d,%3d",
				rxbuf[4]+rxbuf[5]*256,rxbuf[6]+rxbuf[7]*256);
			FntPrint("g=~c686%s~c888\n",buff);
			break;
		case 5:
		case 7:
			sprintf(buff,"%02x,%02x,%02x,%02x",
				rxbuf[6],rxbuf[7],rxbuf[4],rxbuf[5]);
			FntPrint("a=~c686%s~c888\n",buff);
			break;
		default:
			FntPrint("\n");
	}

	/* ------------------------------------------------------------ */
	/*   display actuator information */
	/* ------------------------------------------------------------ */

	/* access actuator information after load completion */
	if(initlevel < PadStateStable)
	{
		FntPrint("\n\n");
		return;
	}

	/* display actuator information */
	FntPrint(" ac");
	maxnum = PadInfoAct(port,-1,0);
	for(count=0;count<maxnum;++count)
	{
		FntPrint("(~c866%d,%d,%d,%d~c888)=~c686%d~c888 ",
			PadInfoAct(port,count,InfoActFunc),
			PadInfoAct(port,count,InfoActSub),
			PadInfoAct(port,count,InfoActSize),
			PadInfoAct(port,count,InfoActCurr),
			*(&hist->Motor0+count));
	}

	/* display terminal type IDs supported by the controller */
	FntPrint("\n ID(~c866");
	maxnum = PadInfoMode(port,InfoModeIdTable,-1);
	for(count=0;count<maxnum;++count)
	{
		FntPrint("%x",PadInfoMode(port,InfoModeIdTable,count));
		if(count != maxnum-1)
		{
			FntPrint(",");
		}
	}

	/* display the list of actuator combinations which can 
	   concurrently operate */
	FntPrint("~c888) cm");
	maxnum = PadInfoComb(port,-1,0);
	for(count=0;count<maxnum;++count)
	{
		FntPrint("(~c866");
		for(pos=0;pos<PadInfoComb(port,count,-1);++pos)
		{
			FntPrint("%x",PadInfoComb(port,count,pos));
			if(pos != PadInfoComb(port,count,-1)-1)
			{
				FntPrint(",");
			}
		}
		FntPrint("~c888) ");
	}

	/* display lock/unlock mode of terminal type alternation switch */
	FntPrint("sw(~c686");
	switch(hist->Lock)
	{
		case 0:
		case 2:
			FntPrint("unlock");
			break;
		case 3:			
			FntPrint(" lock ");
			break;
	}
	FntPrint("~c888)\n");
}


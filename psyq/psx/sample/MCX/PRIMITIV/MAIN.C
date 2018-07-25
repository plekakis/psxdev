/* $PSLibId: Run-time Library Release 4.4.1$ */
/*				mcx
 *
 *		PDAの各種情報を取得、設定する
 *
 *		Copyright (C) 1998 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	0.00		 6 Aug.1998	T.Honda
 *	1.00		24 Sep.1998	T.Honda
 */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpad.h>
#include <libmcrd.h>
#include "libmcx.h"



#define DeviceParamLen		32
#define DeviceDataLen		128
#define DeviceParamWidth	16

static union
{
	struct
	{
		unsigned char Dev;
		unsigned char Param[DeviceParamLen];
		unsigned char Data[DeviceDataLen];
	} Struc;
	unsigned char Plane[1 + DeviceParamLen + DeviceDataLen];
} arg;


typedef struct
{
	DRAWENV		draw;		/* drawing environment : 描画環境 */
	DISPENV		disp;		/* display environment : 表示環境 */
} DB;



#define MaxFunc		18
#define MaxComDisp	23

#define PortNo1		0


static char dispMsg[50];
static int saveSync,saveCmd,saveRes;
static unsigned char *cmdcol[MaxFunc];
static int curFuncNo=0,oldFuncNo=ERROR;
static unsigned char pada[34],padb[34];
static int exitflag=0;
static unsigned char *week[]={"sun","mon","tue","wed","thr","fri","sat"};
static unsigned char *enable[]={"enable ","disable"};
static unsigned char *mark[]={"~c558", "~c588"};


static void setPacket();
static void dispPacket();
static void dispMenu();
static int manageParam(int,int);
static void dispNoData();
static int interval(int);

static int paramAllInfo(int);
static int paramGetApl(int);
static int paramGetTime(int);
static int paramSetTime(int, int);
static int paramHideTrans(int);
static int paramGetSerial(int);
static int paramGetMem(int, int);
static int paramExecFlag(int, int);
static int paramExecApl(int, int);
static int paramCurrCtrl(int, int);
static int paramFlashAcs(int, int);
static int paramSetMem(int, int);
static int paramShowTrans(int, int);
static int paramSetLED(int, int);
static int paramWriteDevice(int, int);
static int paramReadDevice(int, int);
static int paramGetUIFS(int);
static int paramSetUIFS(int, int);

extern int sprintf();




/*#main--- main routine */
int main() {
	DB db[2],*cdb;
	unsigned char *data;

	arg.Struc.Dev = 1;
	data = arg.Struc.Param;
	*data++ = 4;
	*data++ = 1;
	*data++ = 0;
	*data++ = 0xD;
	*data++ = 0x80;

	cdb = 0;
	ResetCallback();
	MemCardInit(0);
	MemCardStart();
	McxStartCom();
	PadInitDirect(pada,padb);
	PadStartCom();

	ResetGraph(0);
	SetGraphDebug(0);

	SetDefDrawEnv(&db[0].draw, 0,   0, 640, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 640, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 640, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 640, 240);

	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(4, 20, 624, 240, 0, 1024));	

	db[0].draw.isbg = 1;
	db[1].draw.isbg = 1;
	setRGB0(&db[0].draw, 60, 120, 120);
	setRGB0(&db[1].draw, 60, 120, 120);
	SetDispMask(1);		/* 0:inhibit,1:enable: ０：不可  １：可 */

	VSync(0);
	while (!exitflag)
	{
		cdb  = (cdb==db)? db+1: db;	

		setPacket(pada);
		dispPacket();
		
		FntFlush(-1);
		DrawSync(0);		
		VSync(0);

		PutDispEnv(&cdb->disp);
		PutDrawEnv(&cdb->draw); 
	}

	ResetGraph(3);
	StopCallback();
	PadStopCom();
	McxStopCom();
	MemCardStop();
	MemCardEnd();
	return 0;
}


/*#setPacket --- */
static void setPacket(unsigned char *pad)
{
	unsigned button;
	static int oldcmd,oldst;

	if(pad[0])
	{
		return;
	}
	button = (~((pad[2]<<8) | pad[3])) & 0xFFFF;

	manageParam(0, button);		/* make parameter */

	if(!oldcmd)
	{
		if(button & PADL2)
		{
			curFuncNo = (curFuncNo + MaxFunc - 1) % MaxFunc;
		}
		else if(button & PADL1)
		{
			curFuncNo = (curFuncNo + 1) % MaxFunc;
		}
	}
	if(!oldst)
	{
		if(button & PADstart)
		{
			if(manageParam(2,0))
			{	/* call mcx function */
				oldFuncNo = curFuncNo;
			}
			else
			{
				oldFuncNo = ERROR;
				strcpy(dispMsg,"~c855command can't reserve(busy)");
			}
		}
	}
	if(button & PADselect)
	{
		exitflag = TRUE;
	}

	oldcmd = button & (PADL1 | PADL2);
	oldst = button & PADstart;

	if(oldcmd)
	{
		*dispMsg = NULL;
	}
}


/*#dispPacket : */
static void dispPacket()
{
	int count, sync;
	long cmd, result;

	for(count=0;count<MaxFunc;++count)
	{
		cmdcol[count]="~c888";
	}
	cmdcol[curFuncNo]="~c585";

	dispMenu();

	FntFlush(-1);
	DrawSync(0);
	FntPrint("\n\n\n\n\n\n\n");

	switch(sync=McxSync(1, &cmd, &result))
	{
	case 0:
		saveSync = sync;
		saveCmd = cmd;
		saveRes = result;
		strcpy(dispMsg, "command executing");
		break;
	case 1:
		saveSync = sync;
		saveCmd = cmd;
		saveRes = result;
		switch(result)
		{
		case 0:
			strcpy(dispMsg, "command succeeded");
			break;
		case 1:
			strcpy(dispMsg, "~c855card not connected");
			break;
		case 2:
			strcpy(dispMsg, "~c855command failed");
			break;
		case 3:
			if(cmd == McxFuncExecFlag)
			{
				strcpy(dispMsg,"~c855command failed (card changed)");
			}
			else
			{
				strcpy(dispMsg, "command succeeded (card changed)");
			}
			break;
		}
		break;
	}

	manageParam(1,0);		/* display parameter */
	FntPrint("\n");

	if(curFuncNo==oldFuncNo)
	{
		manageParam(3,0);		/* display result */
	}
	else
	{
		FntPrint("    press start to exec command \n");
	}

	if(*dispMsg)
	{
		FntPrint("\n    ~c585%s ~c888",dispMsg);
		FntPrint(" sync %d  cmd %d  result %d \n",saveSync,saveCmd,saveRes);
	}
}


/*#dispMenu --- */
static void dispMenu()
{

	FntPrint("  %s1: all info        ~c888 %s 7: set time        ~c888 %s13: show trans    ~c888\n",cmdcol[0],cmdcol[6],cmdcol[12]);
	FntPrint("  %s2: current control ~c888 %s 8: get time        ~c888 %s14: hide trans    ~c888\n",cmdcol[1],cmdcol[7],cmdcol[13]);
	FntPrint("  %s3: flash access    ~c888 %s 9: set led         ~c888 %s15: set memory    ~c888\n",cmdcol[2],cmdcol[8],cmdcol[14]);
	FntPrint("  %s4: set exec flag   ~c888 %s10: get serial no   ~c888 %s16: get memory    ~c888\n",cmdcol[3],cmdcol[9],cmdcol[15]);
	FntPrint("  %s5: exec appli      ~c888 %s11: write device    ~c888 %s17: set user ifs  ~c888\n",cmdcol[4],cmdcol[10],cmdcol[16]);
	FntPrint("  %s6: get appli       ~c888 %s12: read device     ~c888 %s18: get user ifs  ~c888\n",cmdcol[5],cmdcol[11],cmdcol[17]);
	FntPrint("\n");
}


/*#manageParam --- */
static int manageParam(int mode, int button)
{

	switch(curFuncNo)
	{
	case 1:
		return paramCurrCtrl(mode, button);
	case 2:
		return paramFlashAcs(mode, button);
	case 3:
		return paramExecFlag(mode, button);
	case 4:
		return paramExecApl(mode, button);
	case 6:
		return paramSetTime(mode, button);
	case 8:
		return paramSetLED(mode, button);
	case 10:
		return paramWriteDevice(mode, button);
	case 11:
		return paramReadDevice(mode, button);
	case 12:
		return paramShowTrans(mode, button);
	case 14:
		return paramSetMem(mode, button);
	case 15:
		return paramGetMem(mode, button);
	case 16:
		return paramSetUIFS(mode, button);
	default:
		if(mode==1)
		{
			FntPrint("    no parameter needed\n");
		}
		else if(mode==2 || mode==3)
		{
			switch(curFuncNo)
			{
			case 0:
				return paramAllInfo(mode);
			case 5:
				return paramGetApl(mode);
			case 7:
				return paramGetTime(mode);
			case 9:
				return paramGetSerial(mode);
			case 13:
				return paramHideTrans(mode);
			case 17:
				return paramGetUIFS(mode);
			}
		}
	}
	return 0;
}


/*#dispNoData --- */
static void dispNoData()
{

	FntPrint("    no data disp \n");
}


/*#paramAllInfo --- */
static int paramAllInfo(int mode)
{
	static unsigned char state[18];
	unsigned char *time,spbuff[10];

	if(mode==2)
	{
		return McxAllInfo(PortNo1, state);
	}

	FntPrint("    current application : %d \n",state[0]+(state[1]<<8));

	FntPrint("    PDA appli flash access :%s \n",
		state[16] ? "INFERIOR":"SUPERIOR");

	FntPrint("    current control  sound:%s  infred tx:%s  LED:%s \n",
		enable[state[2]],enable[state[3]],enable[state[17]]);

	FntPrint("    serial No. : %x \n",*((unsigned *)(state+4)));

	time = state + 8;
	sprintf(spbuff,"%2x%02x",time[0],time[1]);
	FntPrint("    date: %s/",spbuff);
	FntPrint("%x/%x %s ",time[2],time[3],week[time[4]]);
	sprintf(spbuff,"%2x:%02x:%02x",time[5],time[6],time[7]);
	FntPrint("%s \n",spbuff);

	return 0;
}


/*#paramGetApl --- */
static int paramGetApl(int mode)
{
	static long aplno;

	if(mode==2)
	{
		return McxGetApl(PortNo1, &aplno);
	}

	FntPrint("    current application : %d \n",aplno);
	return 0;
}


/*#paramGetTime --- */
static int paramGetTime(int mode)
{
	static unsigned char time[8];
	unsigned char spbuff[10];

	if(mode==2)
	{
		return McxGetTime(PortNo1, time);
	}

	sprintf(spbuff,"%2x%02x",time[0],time[1]);
	FntPrint("    date: %s/",spbuff);
	FntPrint("%x/%x %s ",time[2],time[3],week[time[4]]);
	sprintf(spbuff,"%2x:%02x:%02x",time[5],time[6],time[7]);
	FntPrint("%s \n",spbuff);
	return 0;
}


/*#paramSetTime --- */
static int paramSetTime(int mode, int button)
{
	static unsigned char time[8]={ 0x19,0x98,9,0x24,2,9,0,0 };
	static unsigned char last[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	static unsigned char start[12]={0,3,2,5,0,3,5,1,4,6,2,4};
	static int parpos,oldpos;
	static int month=9,day=24,hour=9;
	static int inch=0, dech=0, incl=0, decl=0;
	unsigned char spbuff[10];
	int high,low,year,more=0;

	switch(mode)
	{
	case 0:
		if(!oldpos)
		{
			if(button & PADLright)
			{
				if(parpos < 7)
				{
					++parpos;
				}
				if(parpos == 4)
				{
					++parpos;
				}
			}
			else if(button & PADLleft)
			{
				if(parpos > 0)
				{
					--parpos;
				}
				if(parpos == 4)
				{
					--parpos;
				}
			}
		}
		oldpos = button & (PADLright | PADLleft);

		high = time[parpos] & 0xF0;
		low = time[parpos] & 0xF;
		if(!parpos || parpos==1 || parpos==6 || parpos==7)
		{
			if(button & PADRup)
			{
				++inch;
				if(interval(inch))
				{
					time[parpos]=((high + 0x10)% 0xA0)+low;
				}
			}
			else
			{
				inch = 0;
				if(button & PADRleft)
				{
					++dech;
					if(interval(dech))
					{
						time[parpos]=((high + 0x90)%
							 0xA0)+low;
					}
				}
				else
				{
					dech = 0;
				}
			}
			if(parpos==6 || parpos==7)
			{
				if(time[parpos] >= 0x90)
				{
					time[parpos]= 0x50 + low;
				}
				else if(time[parpos] >= 0x60)
				{
					time[parpos] = low;
				}
			}
		}

		high = time[parpos] & 0xF0;
		low = time[parpos] & 0xF;
		year = (time[0]>>4)*1000+(time[0]&0xF)*100+
			(time[1]>>4)*10+(time[1]&0xF);
		more = (!(year%4) && (time[1])) || !(year%400);
		if(button & PADRright)
		{
			++incl;
			if(interval(incl))
			{
				if(!parpos ||parpos==1 ||parpos==6 ||parpos==7)
				{
					time[parpos] = high + ((low +1) % 10);
				}
				else
				{
					switch(parpos)
					{
					case 2:
						++month;
						if(month > 12)
						{
							month = 1;
						}
						break;
					case 3:
						++day;
						if(month==2 && more)
						{
							if(day>29)
							{
								day=1;
							}
						}
						else
						{
							if(day>last[month-1])
							{
								day=1;
							}
						}
						break;
					case 5:
						hour = (hour + 1) % 24;
						break;
					}
				}
			}
		}
		else
		{
			incl = 0;
			if(button & PADRdown)
			{
				++decl;
				if(interval(decl))
				{
					if(!parpos ||parpos==1 ||parpos==6 ||parpos==7)
					{
						time[parpos]=high+((low+9)%10);
					}
					else
					{
						switch(parpos)
						{
						case 2:
							--month;
							if(month < 1)
							{
								month = 12;
							}
							break;
						case 3:
							--day;
							if(day)
							{
								break;
							}
							if(month==2 && more)
							{
								day=29;
							}
							else
							{
								day=last[month-1];
							}
							break;
						case 5:
							hour = (hour + 23)% 24;
							break;
						}
					}
				}
			}
			else
			{
				decl = 0;
			}
		}

		year = (time[0]>>4)*1000+(time[0]&0xF)*100+
			(time[1]>>4)*10+(time[1]&0xF);
		/* 年だけ／月だけ変えて、日数がオーバーしたとき */
		if(month==2 && ((!(year%4) && (time[1])) || !(year%400)))
		{
			if(day>29)
			{
				day=29;
			}
		}
		else
		{
			if(day>last[month-1])
			{
				day = last[month-1];
			}
		}
				

		time[2]= (month/10)*16 + (month%10);
		time[3]= (day/10)*16 + (day%10);
		time[5]= (hour/10)*16 + (hour%10);

		year -= (month<3);
		time[4]=(year+year/4-year/100+year/400+start[month-1]+day)%7;
		break;
	case 1:
		FntPrint("    right:  pos right     left:  pos left \n");
		FntPrint("    triang: inc upper     squar: dec upper \n");
		FntPrint("    circle: inc lower     cross: dec lower \n\n");

		sprintf(spbuff,"%2x",time[0]);
		FntPrint("    date: %s%s~c888",mark[!parpos],spbuff);
		sprintf(spbuff,"%02x",time[1]);
		FntPrint("%s%s~c888",mark[parpos==1],spbuff);
		sprintf(spbuff,"%2x",time[2]);
		FntPrint("/%s%s~c888",mark[parpos==2],spbuff);
		sprintf(spbuff,"%2x",time[3]);
		FntPrint("/%s%s~c888 %s",mark[parpos==3],spbuff,week[time[4]]);
		sprintf(spbuff,"%2x",time[5]);
		FntPrint(" %s%s~c888",mark[parpos==5],spbuff);
		sprintf(spbuff,"%02x",time[6]);
		FntPrint(":%s%s~c888",mark[parpos==6],spbuff);
		sprintf(spbuff,"%02x",time[7]);
		FntPrint(":%s%s~c888\n",mark[parpos==7],spbuff);

		break;
	case 2:
		return McxSetTime(PortNo1, time);
	case 3:
		dispNoData();
		break;
		}

	return NULL;
}


/*#paramHideTrans --- */
static int paramHideTrans(int mode)
{

	if(mode==2)
	{
		return McxHideTrans(PortNo1);
	}
	dispNoData();

	return NULL;
}


/*#paramGetSerial --- */
static int paramGetSerial(int mode)
{
	static unsigned long serial;

	if(mode==2)
	{
		return McxGetSerial(PortNo1, &serial);
	}
	FntPrint("    serial No. : %x \n",serial);

	return NULL;
}


/*#paramGetMem --- */
static int paramGetMem(int mode, int button)
{
	static unsigned char data[128];
	static int addrpos,oldpos;
	static unsigned addr[5]={ 0xD,0,1,4,0x80 };
	static int inch=0, dech=0, incl=0, decl=0;
	int line,count;
	unsigned char spbuff[10];

	switch(mode)
	{
	case 0:
		if(!oldpos)
		{
			if(button & PADLright)
			{
				if(addrpos < 4)
				{
					++addrpos;
				}
			}
			else if((button & PADLleft) && addrpos > 0)
			{
				--addrpos;
			}
		}
		oldpos = button & (PADLright | PADLleft);

		if(button & PADRup)
		{
			++inch;
			if(interval(inch))
			{
				addr[addrpos] = (addr[addrpos] + 0x10) & 0xFF;
			}
		}
		else
		{
			inch = 0;
			if(button & PADRleft)
			{
				++dech;
				if(interval(dech))
				{
					addr[addrpos] = (addr[addrpos] + 0xF0) & 0xFF;
				}
			}
			else
			{
				dech = 0;
			}
		}
		if(addr[4] > 0x80)
		{
			if(addr[4] == 0x90)
			{
				addr[4]=0;
			}
			else if(addr[4] == 0xF0)
			{
				addr[4]=0x80;
			}
			else
			{
				addr[4] &= 0x7F;
			}
		}

		if(button & PADRright)
		{
			++incl;
			if(interval(incl))
			{
				addr[addrpos] = (addr[addrpos] & 0xF0) +
				(((addr[addrpos] & 0xF) +1) & 0xF);
			}
		}
		else
		{
			incl = 0;
			if(button & PADRdown)
			{
				++decl;
				if(interval(decl))
				{
					addr[addrpos] = (addr[addrpos] & 0xF0)+
					(((addr[addrpos] & 0xF) + 0xF) & 0xF);
				}
			}
			else
			{
				decl = 0;
			}
		}
		if(addr[4] > 0x80)
		{
			addr[4] -= 0x10;
		}
		break;
	case 1:
		FntPrint("    right:  pos right     left:  pos left \n");
		FntPrint("    triang: inc upper     squar: dec upper \n");
		FntPrint("    circle: inc lower     cross: dec lower \n\n");
		FntPrint("    start address :~c558");
		for(count=0;count<4;++count)
		{
			sprintf(spbuff,"%02x",addr[count]);
			if(count==addrpos)
			{
				FntPrint("~c588%s~c558", spbuff);
			}
			else
			{
				FntPrint(spbuff);
			}
		}
		sprintf(spbuff,"%02x",addr[4]);
		FntPrint("  ~c888len %s%s~c888 \n",mark[addrpos==4],spbuff);
		break;
	case 2:
		return McxGetMem(PortNo1, data,
		(addr[0]<<24)|(addr[1]<<16)|(addr[2]<<8)|addr[3], addr[4]);
	case 3:
		for(line=0; line<8; ++line)
		{
			FntPrint("    ");
			for(count=0;count<16;++count)
			{
				if(line*16+count >= addr[4])
				{
					break;
				}
				sprintf(spbuff,"%02x",data[line*16+count]);
				FntPrint("%s ",spbuff);
			}
			FntPrint("\n");
			if(line*16+count > addr[4])
			{
				break;
			}
		}
		break;
	}

	return NULL;
}


/*#paramExecFlag --- */
static int paramExecFlag(int mode, int button)
{
	static int block=1,exec=0;
	static int inc=0, dec=0;
	long cmd, result;

	switch(mode)
	{
	case 0:
		if(button & PADRright)
		{
			exec = TRUE;
		}
		else if(button & PADRdown)
		{
			exec = FALSE;
		}

		if(button & PADRup)
		{
			++inc;
			if(interval(inc) && block < 15)
			{
				++block;
			}
		}
		else
		{
			inc = 0;
			if(button & PADRleft)
			{
				++dec;
				if(interval(dec) && block > 1)
				{
					--block;
				}
			}
			else
			{
				dec = 0;
			}
		}
		break;
	case 1:
		FntPrint("    triang: inc appli no.     squar: dec appli no. \n");
		FntPrint("    circle: exec flag on      cross: exec flag off \n\n");
		FntPrint("    appli(block) number :~c558 %d ~c888\n",block);
		FntPrint("    exec flag :~c558 %s ~c888\n",exec ? "on":"off");
		break;
	case 2:
		/* 未確認フラグをクリアするために MemCardAccept()を呼ぶ。
		  (未確認フラグをクリアしないとMcxErrNewCard で実行できない) */
		MemCardAccept(PortNo1);
		MemCardSync(0, &cmd, &result);
		if(result == 1 || result == 2)
		{
			strcpy(dispMsg,"~c855command failed (no card)");
			saveCmd = McxFuncExecFlag;
			saveRes = 1;
			return ERROR;
		}
		return McxExecFlag(PortNo1, block, exec);
	case 3:
		dispNoData();
		break;
	}

	return NULL;
}


/*#paramExecApl --- */
static int paramExecApl(int mode, int button)
{
	static int parpos=0, oldpos;
	static int inch=0, dech=0, incl=0, decl=0;
	static short data[5];
	int count;
	unsigned char spbuff[10];

	switch(mode)
	{
	case 0:
		if(!oldpos)
		{
			if(button & PADLright)
			{
				if(parpos < 4)
				{
					++parpos;
				}
			}
			else if((button & PADLleft) && parpos > 0)
			{
				--parpos;
			}
			if(button & PADLup)
			{
				parpos = 0;
			}
			else if((button & PADLdown) && !parpos)
			{
				parpos = 1;
			}
		}
		oldpos = button & (PADLright | PADLleft | PADLup | PADLdown);

		if(button & PADRup)
		{
			++inch;
			if(interval(inch) && parpos)
			{
				data[parpos] = (data[parpos] + 0x10) & 0xFF;
			}
		}
		else
		{
			inch = 0;
			if(button & PADRleft)
			{
				++dech;
				if(interval(dech) && parpos)
				{
					data[parpos]=(data[parpos]+0xF0)& 0xFF;
				}
			}
			else
			{
				dech = 0;
			}
		}

		if(button & PADRright)
		{
			++incl;
			if(interval(incl))
			{
				if(parpos)
				{
					data[parpos] = (data[parpos] & 0xF0)+
					(((data[parpos] & 0xF) +1) & 0xF);
				}
				else
				{
					if(data[0]==15)
					{
						data[0]=-1;
					}
					else
					{
						++data[0];
					}
				}
			}
		}
		else
		{
			incl = 0;
			if(button & PADRdown)
			{
				++decl;
				if(interval(decl))
				{
					if(parpos)
					{
						data[parpos]=(data[parpos] &
						0xF0)+(((data[parpos] & 0xF)
						+0xF) & 0xF);
					}
					else
					{
						if(data[0]==-1)
						{
							data[0]=15;
						}
						else
						{
							--data[0];
						}
					}
				}
			}
			else
			{
				decl = 0;
			}
		}
		break;
	case 1:
		FntPrint("    triang: inc appli no.     squar: dec appli no. \n\n");
		FntPrint("    application number : %s%d \n",
			mark[!parpos],data[0]);
		FntPrint("    ~c888arg for appli :~c558 ");
		for(count=0;count<4;++count)
		{
			sprintf(spbuff,"%02x",data[count+1]);
			if((count+1)==parpos)
			{
				FntPrint("~c588%s~c558", spbuff);
			}
			else
			{
				FntPrint(spbuff);
			}
		}
		FntPrint("~c888\n");
		break;
	case 2:
		return McxExecApl(PortNo1, data[0], 
			(data[1]<<24)|(data[2]<<16)|(data[3]<<8)|data[4]);
	case 3:
		dispNoData();
		break;
	}

	return 0;
}


/*#paramCurrCtrl --- */
static int paramCurrCtrl(int mode, int button)
{
	static int sound,infred,led;

	switch(mode)
	{
	case 0:
		if(button & PADR2)
		{
			sound = FALSE;
		}
		else if(button & PADR1)
		{
			sound = TRUE;
		}
		if(button & PADRup)
		{
			infred = FALSE;
		}
		else if(button & PADRleft)
		{
			infred = TRUE;
		}

		if(button & PADRright)
		{
			led = FALSE;
		}
		else if(button & PADRdown)
		{
			led = TRUE;
		}
		break;
	case 1:
		FntPrint("    R2:     sound enable       R1:    sound disable \n");
		FntPrint("    triang: infred tx enable   squar: infred tx disable \n");
		FntPrint("    circle: led enable         cross: LED disable \n\n");
		FntPrint("    function allowance  sound: ~c558%s~c888  infred: ~c558%s~c888  LED: ~c558%s~c888 \n",
			enable[sound],enable[infred],enable[led]);
		break;
	case 2:
		return McxCurrCtrl(PortNo1, sound, infred, led);
	case 3:
		dispNoData();
		break;
	}

	return 0;
}


/*#paramFlashAcs --- */
static int paramFlashAcs(int mode, int button)
{
	static int superior;

	switch(mode)
	{
	case 0:
		if(button & PADRright)
		{
			superior = FALSE;
		}
		else if(button & PADRdown)
		{
			superior = TRUE;
		}
		break;
	case 1:
		FntPrint("    circle: flash write superior to spi \n");
		FntPrint("    cross:  spi superior to flash write \n\n");

		FntPrint("    PDA appli flash access : ~c558%s~c888 \n",
			superior ? "INFERIOR":"SUPERIOR");
		break;
	case 2:
		return McxFlashAcs(PortNo1, superior);
	case 3:
		dispNoData();
		break;
	}

	return 0;
}


/*#paramSetMem --- */
static int paramSetMem(int mode, int button)
{
	static unsigned char data[128];
	static int entpos=-5,oldpos;
	static unsigned addr[5]={ 0xD,0,1,4,0x80 };
	static int inch=0, dech=0, incl=0, decl=0;
	int line,count,addrpos;
	unsigned char spbuff[10];

	addrpos = 0;
	switch(mode)
	{
	case 0:
		if(!oldpos)
		{
			if(button & PADLright)
			{
				if(entpos < ((int)addr[4]-1))
				{
					++entpos;
				}
			}
			else if((button & PADLleft) && entpos > -5)
			{
				--entpos;
			}

			if(button & PADLup)
			{
				if(0 <= entpos && entpos <= 5)
				{
					entpos = -5;
				}
				else if(entpos==6)
				{
					entpos = -4;
				}
				else if(entpos==7 || entpos==8)
				{
					entpos = -2;
				}
				else if(9 <= entpos && entpos <=15)
				{
					entpos = -1;
				}
				else if(entpos >= 16)
				{
					entpos -= 0x10;
				}
			}
			else if(button & PADLdown)
			{
				switch(entpos)
				{
				case -5:
					if(addr[4] > 5)
					{
						entpos = 5;
					}
					break;
				case -4:
				case -3:
					if(addr[4] > 6)
					{
						entpos = 6;
					}
					break;
				case -2:
					if(addr[4] > 7)
					{
						entpos = 7;
					}
					break;
				case -1:
					if(addr[4] > 10)
					{
						entpos = 10;
					}
					break;
				default:
					if((int)addr[4] > (entpos + 16))
					{
						entpos += 16;
					}
				}
				if((addr[4]) && (entpos < 0))
				{
					entpos = (int)addr[4] - 1;
				}
			}
		}
		oldpos = button & (PADLright | PADLleft | PADLup | PADLdown);

		if(entpos < 0)
		{
			addrpos = 5 + entpos;
		}
		else
		{
			addrpos = ERROR;
		}

		/* 0x10 の桁増減 */
		if(button & PADRup)
		{
			++inch;
			if(interval(inch))
			{
				if(entpos < 0)
				{
					addr[addrpos] =
					(addr[addrpos] + 0x10) & 0xFF;
				}
				else
				{
					data[entpos]=(data[entpos]+0x10)&0xFF;
				}
			}
		}
		else
		{
			inch = 0;
			if(button & PADRleft)
			{
				++dech;
				if(interval(dech))
				{
					if(entpos < 0)
					{
						addr[addrpos]=
						(addr[addrpos]+0xF0)&0xFF;
					}
					else
					{
						data[entpos]=
						(data[entpos]+0xF0)&0xFF;
					}
				}
			}
			else
			{
				dech = 0;
			}
		}
		if(addrpos >= 0 && addr[4] > 0x80)
		{
			if(addr[4] == 0x90)
			{
				addr[4]=0;
			}
			else if(addr[4] == 0xF0)
			{
				addr[4]=0x80;
			}
			else
			{
				addr[4] &= 0x7F;
			}
		}

		/* 0x01 の桁増減 */
		if(button & PADRright)
		{
			++incl;
			if(interval(incl))
			{
				if(entpos < 0)
				{
					addr[addrpos]=(addr[addrpos] & 0xF0) +
					(((addr[addrpos] & 0xF) +1) & 0xF);
				}
				else
				{
					data[entpos] = (data[entpos] & 0xF0) +
					(((data[entpos] & 0xF) +1) & 0xF);
				}
			}
		}
		else
		{
			incl = 0;
			if(button & PADRdown)
			{
				++decl;
				if(interval(decl))
				{
					if(entpos < 0)
					{
						addr[addrpos] =
						(addr[addrpos] & 0xF0)+
						(((addr[addrpos]&0xF)+
						0xF)&0xF);
					}
					else
					{
						data[entpos] =
						(data[entpos] & 0xF0)+
						(((data[entpos]&0xF)+0xF)&0xF);
					}
				}
			}
			else
			{
				decl = 0;
			}
		}
		if(addr[4] > 0x80)
		{
			addr[4] -= 0x10;
		}
		break;
	case 1:
		if(entpos < 0)
		{
			addrpos = 5 + entpos;
		}
		else
		{
			addrpos = ERROR;
		}

		FntPrint("    righ: pos right   left: pos left ");
		FntPrint("    up:   pos up      down: pos down\n");
		FntPrint("    trng: inc upper   sqar: dec upper");
		FntPrint("    crcl: inc lower   cros: dec lower\n\n");
		FntPrint("    start address :~c558");
		for(count=0;count<4;++count)
		{
			sprintf(spbuff,"%02x",addr[count]);
			if(count==addrpos)
			{
				FntPrint("~c588%s~c558", spbuff);
			}
			else
			{
				FntPrint(spbuff);
			}
		}
		sprintf(spbuff,"%02x",addr[4]);
		FntPrint("  ~c888len %s%s~c888 \n",mark[addrpos==4],spbuff);
		FntPrint("\n~c558");

		for(line=0; line<8; ++line)
		{
			FntPrint("    ");
			for(count=0;count<16;++count)
			{
				if(line*16+count >= addr[4])
				{
					break;
				}
				sprintf(spbuff,"%02x ",data[line*16+count]);
				if((line*16+count)==entpos)
				{
					FntPrint("~c588%s~c558",spbuff);
				}
				else
				{
					FntPrint(spbuff);
				}
			}
			FntPrint("\n");
			if(line*16+count > addr[4])
			{
				break;
			}
		}
		break;
	case 2:
		return McxSetMem(PortNo1, data,
		(addr[0]<<24)|(addr[1]<<16)|(addr[2]<<8)|addr[3], addr[4]);
	case 3:
		dispNoData();
		break;
	}

	FntPrint("~c888");
	return NULL;
}


/*#paramShowTrans --- */
static int paramShowTrans(int mode, int button)
{
	static int dir=0, time=1;
	static int inc=0, dec=0;

	switch(mode)
	{
	case 0:
		if(button & PADRup)
		{
			dir = TRUE;
		}
		else if(button & PADRleft)
		{
			dir = FALSE;
		}
		if(button & PADRright)
		{
			++inc;
			if(interval(inc) && time < 255)
			{
				++time;
			}
		}
		else
		{
			inc = 0;
			if(button & PADRdown)
			{
				++dec;
				if(interval(dec) && time > 0)
				{
					--time;
				}
			}
			else
			{
				dec = 0;
			}
		}
		break;
	case 1:
		FntPrint("    triang: PS to PDA    squar: PDA to PS \n");
		FntPrint("    circle: inc time     cross: dec time \n\n");
		FntPrint("    dir :~c558 %s ~c888\n",
			 dir ? "PS to PDA":"PDA to PS");
		FntPrint("    time out :~c558 %d ~c888s \n",time);
		break;
	case 2:
		return McxShowTrans(PortNo1, dir, time);
	case 3:
		dispNoData();
		break;
	}

	return 0;
}


/*#paramSetLED --- */
static int paramSetLED(int mode, int button)
{
	static int led=0;

	switch(mode)
	{
	case 0:
		if(button & PADRright)
		{
			led = TRUE;
		}
		else if(button & PADRdown)
		{
			led = FALSE;
		}
		break;
	case 1:
		FntPrint("    circle: on   cross: off \n\n");
		FntPrint("    led :~c558 %s ~c888\n", led ? "on":"off");
		break;
	case 2:
		return McxSetLED(PortNo1, led);
	case 3:
		dispNoData();
		break;
	}

	return 0;
}


/*#paramReadDevice --- */
static int paramReadDevice(int mode, int button)
{
	static int parpos=-1, oldpos;
	static int inch=0, dech=0, incl=0, decl=0;
	int line,count;
	unsigned char spbuff[10];

	switch(mode)
	{
	case 0:
		if(!oldpos)
		{
			if(button & PADLright)
			{
				if(parpos < DeviceParamLen - 1)
				{
					++parpos;
				}
			}
			else if((button & PADLleft) && parpos >= 0)
			{
				--parpos;
			}
			if(button & PADLup)
			{
				parpos -= DeviceParamWidth;
				if(parpos < 0)
				{
					parpos = -1;
				}
			}
			else if(button & PADLdown)
			{
				if(parpos < 0)
				{
					parpos = 0;
				}
				else if((parpos+DeviceParamWidth) <
					DeviceParamLen)
				{
					parpos += DeviceParamWidth;
				}
			}
		}
		oldpos = button & (PADLright | PADLleft | PADLup | PADLdown);

		if(button & PADRup)
		{
			++inch;
			if(interval(inch))
			{
				arg.Plane[parpos+1] =
					(arg.Plane[parpos+1] + 0x10) & 0xFF;
			}
		}
		else
		{
			inch = 0;
			if(button & PADRleft)
			{
				++dech;
				if(interval(dech))
				{
					arg.Plane[parpos+1] =
					(arg.Plane[parpos+1] + 0xF0) & 0xFF;
				}
			}
			else
			{
				dech = 0;
			}
		}

		if(button & PADRright)
		{
			++incl;
			if(interval(incl))
			{
				arg.Plane[parpos+1] =
				(arg.Plane[parpos+1] & 0xF0) +
				(((arg.Plane[parpos+1] & 0xF) +1) & 0xF);
			}
		}
		else
		{
			incl = 0;
			if(button & PADRdown)
			{
				++decl;
				if(interval(decl))
				{
					arg.Plane[parpos+1] =
					(arg.Plane[parpos+1] & 0xF0)+
					(((arg.Plane[parpos+1]&0xF)+0xF)& 0xF);
				}
			}
			else
			{
				decl = 0;
			}
		}
		break;
	case 1:
		FntPrint("    up/down/right/left:  pos up/down/right/left \n");
		FntPrint("    triang/square: inc/dec upper     circle/cross: inc/dec lower \n\n");
                FntPrint("    device no : %s%x~c888 \n",
                        mark[parpos==-1],arg.Struc.Dev);
		for(line=0;line<2;++line)
		{
			FntPrint(line ? "          ":"    fix :~c558 ");
			for(count=0;count<16;++count)
			{
				sprintf(spbuff,"%02x ",arg.Struc.Param[count+
					line*DeviceParamWidth]);
				if((count+line*DeviceParamWidth)== parpos)
				{
					FntPrint("~c588%s~c558",spbuff);
				}
				else
				{
					FntPrint(spbuff);
				}
			}
			FntPrint("\n");
		}
		FntPrint("~c888");
		break;
	case 2:
		for(count=0;count<DeviceDataLen;++count)
		{
			arg.Struc.Data[count] = 0;
		}
		return McxReadDev(PortNo1, arg.Struc.Dev, arg.Struc.Param,
			arg.Struc.Data);
	case 3:
		for(line=0; line<8; ++line)
		{
			FntPrint(line ? "          ":"    read: ");
			for(count=0;count<DeviceParamWidth;++count)
			{
				sprintf(spbuff,"%02x",
					arg.Struc.Data[line*
					DeviceParamWidth+count]);
				FntPrint("%s ",spbuff);
			}
			FntPrint("\n");
		}
		break;
	}

	return NULL;
}


/*#paramWriteDevice --- */
static int paramWriteDevice(int mode, int button)
{
	static int parpos=-1, oldpos;
	static int inch=0, dech=0, incl=0, decl=0;
	int line,count;
	unsigned char spbuff[10];

	switch(mode)
	{
	case 0:
		if(!oldpos)
		{
			if(button & PADLright)
			{
				if(parpos <(DeviceParamLen + DeviceDataLen -1))
				{
					++parpos;
				}
			}
			else if((button & PADLleft) && parpos >= 0)
			{
				--parpos;
			}
			if(button & PADLup)
			{
				parpos -= DeviceParamWidth;
				if(parpos < 0)
				{
					parpos = -1;
				}
			}
			else if(button & PADLdown)
			{
				if(parpos < 0)
				{
					parpos = 0;
				}
				else if((parpos+DeviceParamWidth) <
				(DeviceParamLen + DeviceDataLen))
				{
					parpos += DeviceParamWidth;
				}
			}
		}
		oldpos = button & (PADLright | PADLleft | PADLup | PADLdown);

		if(button & PADRup)
		{
			++inch;
			if(interval(inch))
			{
				arg.Plane[parpos+1] =
					(arg.Plane[parpos+1] + 0x10) & 0xFF;
			}
		}
		else
		{
			inch = 0;
			if(button & PADRleft)
			{
				++dech;
				if(interval(dech))
				{
					arg.Plane[parpos+1] =
					(arg.Plane[parpos+1] + 0xF0) & 0xFF;
				}
			}
			else
			{
				dech = 0;
			}
		}

		if(button & PADRright)
		{
			++incl;
			if(interval(incl))
			{
				arg.Plane[parpos+1] =
				(arg.Plane[parpos+1] & 0xF0)+
				(((arg.Plane[parpos+1] & 0xF) +1) & 0xF);
			}
		}
		else
		{
			incl = 0;
			if(button & PADRdown)
			{
				++decl;
				if(interval(decl))
				{
					arg.Plane[parpos+1] =
					(arg.Plane[parpos+1] & 0xF0)+
					(((arg.Plane[parpos+1]&0xF)+0xF)& 0xF);
				}
			}
			else
			{
				decl = 0;
			}
		}
		break;
	case 1:
		FntPrint("    up/down/right/left:  pos up/down/right/left \n");
		FntPrint("    triang/square: inc/dec upper     circle/cross: inc/dec lower \n\n");
                FntPrint("    device no : %s%x~c888 \n",
                        mark[parpos==-1], arg.Struc.Dev);

		for(line=0;line<2;++line)
		{
			FntPrint(line ? "          ":"    fix :~c558 ");
			for(count=0;count<16;++count)
			{
				sprintf(spbuff,"%02x ",arg.Struc.Param[count+
					line*DeviceParamWidth]);
				if((count+line*DeviceParamWidth)== parpos)
				{
					FntPrint("~c588%s~c558",spbuff);
				}
				else
				{
					FntPrint(spbuff);
				}
			}
			FntPrint("\n");
		}

		for(line=0; line<8; ++line)
		{
			FntPrint(line ? "          ":"    ~c888var :~c558 ");
			for(count=0;count<DeviceParamWidth;++count)
			{
				sprintf(spbuff,"%02x ",arg.Struc.Data[line*
					DeviceParamWidth+count]);
				if((count+line*DeviceParamWidth)==
				(parpos - DeviceParamLen))
				{
					FntPrint("~c588%s~c558",spbuff);
				}
				else
				{
					FntPrint(spbuff);
				}
			}
			FntPrint("\n");
		}
		FntPrint("~c888");
		break;
	case 2:
		return McxWriteDev(PortNo1, arg.Struc.Dev,
			arg.Struc.Param, arg.Struc.Data);
	case 3:
		dispNoData();
		break;
	}

	return NULL;
}


/*#paramGetUIFS --- */
static int paramGetUIFS(int mode)
{
	static unsigned char data[8];
	unsigned char spbuff[10];

	if(mode==2)
	{
		return McxGetUIFS(PortNo1, data);
	}

	sprintf(spbuff,"%2x:%02x",data[1],data[0]);
	FntPrint("    alarm time: %s \n",spbuff);
	FntPrint("    alarm:        %s \n",(data[2]&1) ? "on":"off");
	FntPrint("    key lock:     %s \n",(data[2]&2) ? "on":"off");
	FntPrint("    speaker:      %s \n",
		(data[2]&8) ? "off":((data[2]&4) ?"small":"large"));
	FntPrint("    area code:    %x \n",(data[2]&0x70)>>4);
	FntPrint("    rtc set:      %x \n",data[2]>>7);
	FntPrint("    font addrss:  %x \n",*(short *)(data + 4));
	return 0;
}


/*#paramSetUIFS --- */
static int paramSetUIFS(int mode, int button)
{
	static unsigned char data[8];
	static int parpos, oldpos;
	static int inch=0, dech=0, incl=0, decl=0;
	static int alarm,key,spk,oldpush,low,high,hour;
	unsigned char spbuff[10];

	switch(mode)
	{
	case 0:
		if(!oldpos)
		{
			if(button & (PADLright | PADLdown))
			{
				if(parpos < 2)
				{
					++parpos;
				}
			}
			else if((button & (PADLleft | PADLup)) && parpos > 0)
			{
				--parpos;
			}
		}
		oldpos = button & (PADLright | PADLleft | PADLup | PADLdown);

		if(parpos==2)
		{
			if(button & PADR2)
			{
				alarm = TRUE;
			}
			else if(button & PADR1)
			{
				alarm = FALSE;
			}

			if(button & PADRup)
			{
				key = TRUE;
			}
			else if(button & PADRleft)
			{
				key = FALSE;
			}

			if(!oldpush)
			{
				if((button & PADRdown) && spk<2)
				{
					++spk;
				}
				else if((button & PADRright) && spk>0)
				{
					--spk;
				}
			}

			data[2]=alarm | (key<<1) | (spk<<2);
			oldpush = button & (PADRright | PADRdown);
			break;
			}

		if(button & PADRup)
		{
			++inch;
			if(interval(inch) && parpos)
			{
				high = (high + 1) % 6;
			}
		}
		else
		{
			inch = 0;
			if(button & PADRleft)
			{
				++dech;
				if(interval(dech) && parpos)
				{
					high = (high + 5) % 6;
				}
			}
			else
			{
				dech = 0;
			}
		}

		if(button & PADRright)
		{
			++incl;
			if(interval(incl))
			{
				if(parpos)
				{
					low = (low + 1) % 10;
				}
				else
				{
					hour = (hour + 1) % 24;
				}
			}
		}
		else
		{
			incl = 0;
			if(button & PADRdown)
			{
				++decl;
				if(interval(decl))
				{
					if(parpos)
					{
						low = (low + 9) % 10;
					}
					else
					{
						hour = (hour + 23) % 24;
					}
				}
			}
			else
			{
				decl = 0;
			}
		}

		data[0]= (high << 4)| low;
		data[1]= ((hour/10)<<4) | (hour % 10);
		break;
	case 1:
		FntPrint("    right/left:  pos right/left \n");
		FntPrint("    triang/square: inc/dec upper     circle/cross: inc/dec lower \n");
		FntPrint("    R2/R1: alarm    triang/square: key lock     circle/cross: speaker \n\n");

		sprintf(spbuff,"%2x",data[1]);
		FntPrint("    alarm time: %s%s~c888:",mark[!parpos],spbuff);
		sprintf(spbuff,"%02x",data[0]);
		FntPrint("%s%s~c888\n",mark[parpos==1],spbuff);

		FntPrint("    alarm:       %s%s~c888 \n",
			mark[parpos==2],alarm ? "on":"off");
		FntPrint("    key lock:    %s%s~c888 \n",
			mark[parpos==2],key ? "on":"off");
		FntPrint("    speaker:     %s%s~c888 \n",mark[parpos==2],
			spk ? (spk==2 ? "off":"small"):"large");

		break;
	case 2:
		data[2] |= 0x80;
		return McxSetUIFS(PortNo1, data);
	case 3:
		dispNoData();
		break;
	}

	return NULL;
}



#define StepLow		40
#define TimesLow	2
#define StepHigh	15


/*#interval --- */
static int interval(int val)
{

	return (val<=(StepLow * TimesLow) && (val % StepLow)==1) ||
		(val >(StepLow * TimesLow) &&
		((val - StepLow * TimesLow) % StepHigh) == 1);
}

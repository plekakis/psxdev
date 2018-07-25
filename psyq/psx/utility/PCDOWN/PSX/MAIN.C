/*
	Download & Run PSX files using the DTL-H3050 serial cable
	Written by izumi		Date:14/July/'97

	Notes:Support to wait hitting any key before running your program
	      Support to select the indication of transferd status infomation
	        complie option : -DSTS_DISP -Xo$801e0000 (display status info.)
	     		      		    -Xo$8001f0000 (display nothing)
	      Support INDY (#define INDY)
 */
#include <sys/types.h>
#include <sys/file.h>
#include <kernel.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsio.h>

#define	KEY_WAIT	1	/* 1:wait hitting any key
				   0:run without hitting any key after downloading target program */

/*#define	INDY			/* if you use INDY */

#define	BPS9600		9600
#define	BPS19200	19200
#define	BPS38400	38400
#define	BPS57600	57600
#define	BPS115200	115200
#ifndef INDY
#define	BPS		BPS57600	/* for PC */
#else
#define	BPS		BPS38400	/* for INDY */
#endif

#define SIO_SB_11	0xc0
#define SIO_CHLEN_8	0x0c

#define	PCKT_SUCCESS	1
#define	PCKT_ERROR	2

#define	OK		1
#define	FAIL		0
#define	RECEIVE_END	0x7e

#define	DAT_FILE	1
#define	EXE_FILE	2

#define HEAD_SIZE 2048		/* Header size of EXE file */
char head_buf[HEAD_SIZE];	/* Buffer of EXE Header */
struct XF_HDR *head;     	/* Pointer to EXE Header Structure */

#define STACKP	0x801ffff0	/* Stack Pointer when target program runs */

typedef struct {
	DRAWENV draw;
	DISPENV disp;
} DB;

DB db[2];
DB *cdb;

u_long	padd, oldpad;

int bufp = 0;
int chlen = 0;
char sum = 0;
char pc_sum = 0;
int step = 0;
int file_type = 0;
u_long	ld_adrs = 0;
char *loadp;
unsigned long fsize = 0;
unsigned long fcnt = 0;

char DAT_ID[] = {"DAT"};
char EXE_ID[] = {"EXE"};


void main(void);
void init_sio(void);
void get_sio_data(void);
char sio_read(void);
void sio_write(char c);
int  pad_read(void);
#ifdef STS_DISP
void init_prim(DB *db);
void _pre_draw(void);
void _draw(void);
#endif

void main(void)
{
	ResetCallback();

	PadInit(0);
	ResetGraph(0);

/*	AddSIO(BPS);
	DelSIO();*/
	init_sio();

#ifdef STS_DISP
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	FntLoad(960, 256);
	
	FntOpen(16, 16, 256, 200, 0, 256);

	init_prim(&db[0]);
	init_prim(&db[1]);

	SetDispMask(1);

	_draw();
	FntPrint("wait data...\n");
	_pre_draw();
	_draw();
#endif

	/* main loop */
	while (1) {

		get_sio_data();

#ifdef STS_DISP
		FntPrint("addr = %x size = %d\n", ld_adrs, fsize);
		FntPrint("pklen = %d pcsum = %x\n", chlen, pc_sum);
		FntPrint("cnt = %d\n", fcnt);
		_pre_draw();
		_draw();
#endif
	}

}

void init_sio(void)
{
	_sio_control(1,2,MR_SB_01|MR_CHLEN_8|0x02);
	_sio_control(1,3,BPS);

	_sio_control(1,1,CR_RXEN|CR_TXEN);	/* RTS:off DTR:off */
}

void get_sio_data(void)
{
	int i;
	char c;
	char sts;
	char id_buf[4];

	switch(step) {
	case 0:		/* check the file type */
		bzero(id_buf, 4);
		for(i=0; i<3; i++) {
			id_buf[i] = sio_read()&0xff;
		}
		if(!strncmp(id_buf, DAT_ID, 3)) {
			sio_write(OK);
			file_type = DAT_FILE;
			step++;
		}
		else if(!strncmp(id_buf, EXE_ID, 3)) {
			sio_write(OK);
			file_type = EXE_FILE;
			step++;
		}
		else sio_write(FAIL);
		break;
	case 1:		/* get the load address */
		ld_adrs = 0;
		for(i=0; i<4; i++) {
			ld_adrs |= (sio_read()&0xff)<<((3-i)*8);
		}
		sio_write(OK);
		if(file_type==EXE_FILE) loadp = head_buf;
		else loadp = (char *)ld_adrs;
		step++;
		break;
	case 2:		/* get the file size */
		fsize = 0;
		fcnt = 0;
		for(i=0; i<4; i++) {
			fsize |= (sio_read()&0xff)<<((3-i)*8);
		}
		sio_write(OK);
		step++;
		break;
	case 3:		/* get the packet length */
		chlen = sio_read()&0xff;
		step++;
		break;
	case 4:		/* get the check sum */
		pc_sum = sio_read()&0xff;
		step++;
		break;
	case 5:		/* get the data of packet size */
		if(bufp < chlen) {
			c = sio_read();
			if((file_type==EXE_FILE)&&(fcnt==HEAD_SIZE))
				loadp = (char *)ld_adrs;
			loadp[bufp] = c;
			sum += c;
			bufp++;
			fcnt++;
			break;
		}
		else if(bufp == chlen) {
			sum &= 0xff;
			if(pc_sum == sum) {
				sts = PCKT_SUCCESS;
				loadp += chlen;
			}
			else {
				sts = PCKT_ERROR;
				fcnt -= chlen;
			}
			sio_write(sts);
			bufp = 0;
			sum = 0;
			if(fcnt==fsize) step = 6;	/* end of receiving? */
			else step = 3;
			break;
		}
	case 6:		/* send the end code */
		sio_write((char)RECEIVE_END);
		if(file_type == EXE_FILE) step++;
		else step = 0;
		break;
	case 7:		/* if EXE file */
#if KEY_WAIT
		while(!pad_read()) {	/* wait hitting any key */
#ifdef STS_DISP
			FntPrint("complete!\n");
			FntPrint("change disc & press start key.\n");
			_pre_draw();
			_draw();
#endif
		}
#endif
		ResetGraph(3);
		PadStop();
		StopCallback();
		head = (struct XF_HDR *)head_buf;
		head->exec.s_addr = STACKP;
		head->exec.s_size = 0;
		EnterCriticalSection();
		Exec(&(head->exec),1,0);	/* run ! */
		break;
	}
}

char sio_read(void)
{
	char c;
	long sts;

	sts = _sio_control(0,1,0);
	_sio_control(1,1,sts|CR_RTS);		/* RTS:on */
	while(!(_sio_control(0,0,0)&SR_RXRDY));
	c = _sio_control(0,4,0)&0xff;
	sts = _sio_control(0,1,0);
	_sio_control(1,1,sts&(~CR_RTS));	/* RTS:off */
	return c;
}

void sio_write(char c)
{
	while((_sio_control(0,0,0)&(SR_TXU|SR_TXRDY))!=(SR_TXU|SR_TXRDY));
	_sio_control(1,4,c);
}

int pad_read(void)
{
	padd = PadRead(1);		/*: read PAD data */

	if (((padd & PADstart)>0)&&((oldpad & PADstart)==0)) {
	 	return(1);
	}

	oldpad = padd;
	return(0);
}

#ifdef STS_DISP
void init_prim(DB *db)
{
	/* set bg color */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);
}

void _pre_draw(void)
{
	DrawSync(0);
	FntFlush(-1);
}


void _draw(void)
{
	cdb = (cdb==db)?db+1:db;
	PutDrawEnv(&cdb->draw);
	PutDispEnv(&cdb->disp);
}
#endif


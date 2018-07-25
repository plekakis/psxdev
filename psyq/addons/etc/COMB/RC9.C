/* $PSLibId: Runtime Library Release 3.6$ */
/*
 * File:rc9.c
 * Remote controller driver
*/

#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <libcomb.h>

#define BR 2073600 	/* max:2073600 */
static long fr,fw;		/* file descriptor */
static unsigned long ev_r, ev_e;	/* read/error event descriptor */
static long re_flag;			/* rec error recovery */
static unsigned char rec[8];	/* recieive buffer */
static unsigned char send[8];	/* send buffer */
static unsigned long dog;		/* watch dog */

#define COUNT_TIMEOUT_R 60
static _rec(), _send();

/* local controller part */
static unsigned char _c_buf[4][34];

static
long
_error()
{
		printf("error\n");
		_comb_control(2,0,0);		/* reset driver */
		re_flag = 1;
		_c_buf[2][0] = _c_buf[3][0] = 0xff;
		dog = COUNT_TIMEOUT_R;
}


static
_send()
{
		send[0] = 'R';
		send[1] = 0x00;
		send[2] = _c_buf[0][2];
		send[3] = _c_buf[0][3];
		send[4] = _c_buf[1][2];
		send[5] = _c_buf[1][3];
		send[6] = 0x00;
		send[7] = 'C';

		FntPrint("state %x\n", _comb_control(0,0,0) ); 
		if( (_comb_control(0,0,0)&0x180) == 0x180 ) {
			write(fw,send,8);
		}
}


static 
_rec()
{
		if(rec[0]=='R' && rec[7]=='C') {
			_c_buf[2][2] = rec[2];
			_c_buf[2][3] = rec[3];
			_c_buf[3][2] = rec[4];
			_c_buf[3][3] = rec[5];
			_c_buf[2][0] = _c_buf[3][0] = 0x00;
			_c_buf[2][1] = _c_buf[3][1] = 0x41;
		}
		else {
			_c_buf[2][0] = _c_buf[3][0] = 0xff;
		}
		re_flag = 0;
		rec[0] = 0x00;
		if(read(fr,rec,8)==-1)
			printf("read()==-1\n");
		dog = COUNT_TIMEOUT_R;
}


_init_cont()
{
	/* open and enable an event to detect the end of read operation */
	ev_r = OpenEvent(HwSIO,EvSpIOER,EvMdNOINTR,NULL);

	/* open and enable an event to detect the error */
	ev_e = OpenEvent(HwSIO,EvSpERROR,EvMdINTR,_error);

	/* attacth the SIO driver to the kernel */
	AddCOMB();

	/* open a write path and set baud-rate */
	fw = open("sio:",O_WRONLY);

	/* open a read path and set baud-rate */
	fr = open("sio:",O_RDONLY|O_NOWAIT);

	/* normal set up */
	InitPAD((char *)&_c_buf[0][0],34,(char *)&_c_buf[1][0],34);
	StartPAD();
}

_start_remote()
{
	dog = 1;
	re_flag = 0;
	_c_buf[2][0] = _c_buf[3][0] = 0xff;
	EnableEvent(ev_e);
	EnableEvent(ev_r);
	_comb_control(1,1,3);	
	_comb_control(1,4,8);
	_comb_control(1,3,2073600);
	printf("baud rate = %d\n",_comb_control(0,3,0));
	printf("buf char = %d\n",_comb_control(0,4,0));
}

_finish_cont()
{
	close(fw);
	close(fr);
	CloseEvent(ev_r);
	CloseEvent(ev_e);
	/* detacth the SIO driver to the kernel */
	DelCOMB();
}

_get_cont(chan)
long chan;
{
	unsigned long w;
	/* local controller part */
	if(_c_buf[chan][0]==0xff)   return 0x8000ffff;
	w = (unsigned long)_c_buf[chan][2];
	return (long)( (w<<8) | (unsigned long)_c_buf[chan][3] );
}

_do_remote()
{
	_send();
	if(TestEvent(ev_r)==1 || --dog==0) {
		EnterCriticalSection();
		if(dog==0) {
			if(!re_flag)
				_comb_control(2,3,0);	/* cancel previous read() */
			printf("t/o in rec\n");
			_rec();
		}
		else if(rec[0]!='R' || rec[7]!='C') {
			_c_buf[2][0] = _c_buf[3][0] = 0xff;
			re_flag = 1;
			dog = COUNT_TIMEOUT_R;
			printf("shift in rec\n");
		} 
		else {
			_rec();
		}
		ExitCriticalSection();
	}
}


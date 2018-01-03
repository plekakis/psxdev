/* $PSLibId: Run-time Library Release 4.3$ */
/*
 * Remote controller driver
 */
#include <stdio.h>
#include <stdlib.h>
#include <libapi.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <strings.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcomb.h>

#define	BUFSIZE		(64)
/**************************************************************************/
static long fr,fw;			/* file descriptor */
static unsigned long ev_w, ev_r, ev_e;	/* write/read/error event descriptor */
static unsigned char recbuf[BUFSIZE];	/* recieive buffer */
static unsigned char senbuf[BUFSIZE];	/* send buffer */
static volatile long errcnt1, errcnt2;	/* error counter */
static long wr_flg, re_flg;		/* write recovery/read recovery */
/**************************************************************************/

long _error_remote(void)
{
	errcnt1++;
	re_flg = 0;
	return(0);
}

int _init_remote(void)
{
	/* open an event to detect the end of read operation */
	ev_r = OpenEvent(HwSIO,EvSpIOER,EvMdNOINTR,NULL);

	/* open an event to detect the end of write operation */
	ev_w = OpenEvent(HwSIO,EvSpIOEW,EvMdNOINTR,NULL);

	/* open an event to detect the error */
	ev_e = OpenEvent(HwSIO,EvSpERROR,EvMdINTR,_error_remote);

	/* attacth the SIO driver to the kernel */
	AddCOMB();

	/* open stream for writing */
	fw = open("sio:",O_WRONLY|O_NOWAIT);

	/* open stream for reading */
	fr = open("sio:",O_RDONLY|O_NOWAIT);

	re_flg = wr_flg = 0;
	return(0);
}

void _start_remote(long baud)
{
	CombSetBPS(baud);	/* set communication rate */
	CombSetPacketSize(1);	/* set the number of characters per transmission packet */

	/* enable an event to detect the error */
	EnableEvent(ev_e);

	/* enable an event to detect the end of read operation */
	EnableEvent(ev_r);

	/* enable an event to detect the end of write operation */
	EnableEvent(ev_w);
}

int _send_remote(void)
{
	int	i;
	unsigned char sum;

	if (TestEvent(ev_w)==1) {
		wr_flg = 0;
		return(1);
	}

	if (wr_flg==0 && CombCTS() != 0) {
		for( i=1, sum=0; i<BUFSIZE; i++ ) {
			sum += (unsigned char)i;
			senbuf[i] = i;
		}
		senbuf[0] = sum;
		write( fw, senbuf, BUFSIZE );
		wr_flg = 1;
	}
	return(0);
}

int _rec_remote(void)
{
	int	i;
	unsigned char sum;

	if (TestEvent(ev_r)==1 ) {
		for( i=1, sum=0; i<BUFSIZE; i++ ) {
			sum += recbuf[i];
		}
		if (recbuf[0] != sum ) {
			errcnt2++;
			re_flg = 0;
			return(0);
		}
		read( fr, recbuf, BUFSIZE );
		return(1);
	}
	if (re_flg==0 ) {
		CombReset();		/* reset driver */
		read( fr, recbuf, BUFSIZE );
		re_flg = 1;
	}
	return(0);
}

int _get_error_count1(void)
{
	return( errcnt1 );
}

int _get_error_count2(void)
{
	return( errcnt2 );
}


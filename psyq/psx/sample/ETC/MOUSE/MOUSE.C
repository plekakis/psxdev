/*
 * $PSLibId: Run-time Library Release 4.4$
 */
#include <sys/types.h>
static _Mouse(long);
static signed char *PAD_Rbuf[2];

static signed long MousePx[2];
static signed long MousePy[2];
static signed long MousePMinX,MousePMaxX;
static signed long MousePMinY,MousePMaxY;
static u_short MouseDx;
static u_short MouseDy;


MouseState(chan,buf)
long chan;
signed char buf[];
{
	long ret = 0;
	
	if( PAD_Rbuf[chan][0] != 0)
	{
		ret = -1;
		buf[0] = 0;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;
	}
	else
	{
		buf[0] = PAD_Rbuf[chan][4];
		buf[1] = PAD_Rbuf[chan][5];
		buf[2] = PAD_Rbuf[chan][3];
		buf[3] = PAD_Rbuf[chan][1];
	}
	return ret;
}

InitMouse( bufA, bufB)
signed char *bufA,*bufB;
{
        PAD_Rbuf[0] = bufA;
        PAD_Rbuf[1] = bufB;
}


RangeMouse(x0,x1,y0,y1)
long x0,x1,y0,y1;
{
	MousePMinX = x0*MouseDx;
	MousePMaxX = x1*MouseDx;
	MousePMinY = y0*MouseDy;
	MousePMaxY = y1*MouseDy;
}

SenseMouse(x,y)
u_long x,y;
{
	MouseDx = x;	
	MouseDy = y;
}

SetMouse(chan, x,y)
u_long chan, x, y;
{
	MousePx[chan] = x*MouseDx;
	MousePy[chan] = y*MouseDy;
}

MouseRead(chan, buf)
u_long chan,buf[];
{
	_Mouse(chan);
	buf[0] = MousePx[chan]/MouseDx;
	buf[1] = MousePy[chan]/MouseDy;
	buf[2] = ~0xff;
	if(PAD_Rbuf[chan][0]==0 && PAD_Rbuf[chan][1]==0x12)
		buf[2] = ~(PAD_Rbuf[chan][3])&0x0c;
}

static
_Mouse(chan)
long chan;
{
	if(PAD_Rbuf[chan][0]==0 && PAD_Rbuf[chan][1]==0x12)
	{
		MousePx[chan]+=PAD_Rbuf[chan][4];
		MousePy[chan]+=PAD_Rbuf[chan][5];

		if(MousePx[chan]>MousePMaxX)
			MousePx[chan]=MousePMaxX;
		else 
		if(MousePx[chan]<MousePMinX)
			MousePx[chan]=MousePMinX;
		if(MousePy[chan]>MousePMaxY)
			MousePy[chan]=MousePMaxY;
		else 
		if(MousePy[chan]<MousePMinY)
			MousePy[chan]=MousePMinY;
	}
}


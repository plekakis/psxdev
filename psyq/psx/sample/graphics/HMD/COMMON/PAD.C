/*
 * $PSLibId: Run-time Library Release 4.4$
 */
#include <sys/types.h>
#include <libetc.h>
#include <libpad.h>

#define P_MAX	0xc0
#define P_MIN	0x40

static u_char padbuf[2][34];

void common_PadInit(void)
{
	PadInitDirect(padbuf[0], padbuf[1]);
	PadStartCom();
}

void common_PadStop(void)
{
	PadStopCom();
}

u_long common_PadRead(void)
{
	u_long padd = 0;

	if (padbuf[0][0] != 0) {
		return(padd);
	}

	padd = ~((padbuf[0][2]<<8) | (padbuf[0][3]));

	if ((padbuf[0][1]>>4) == 7) {
		if (padbuf[0][4] > P_MAX)	padd |= (PADRright);
		else
		if (padbuf[0][4] < P_MIN)	padd |= (PADRleft);

		if (padbuf[0][5] > P_MAX)	padd |= (PADRdown);
		else
		if (padbuf[0][5] < P_MIN)	padd |= (PADRup);

		if (padbuf[0][6] > P_MAX)	padd |= (PADLright);
		else
		if (padbuf[0][6] < P_MIN)	padd |= (PADLleft);

		if (padbuf[0][7] > P_MAX)	padd |= (PADLdown);
		else
		if (padbuf[0][7] < P_MIN)	padd |= (PADLup);
	}

	return(padd);
}

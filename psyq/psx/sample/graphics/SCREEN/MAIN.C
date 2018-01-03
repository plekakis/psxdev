/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			screen: VRAM viewer
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Sep,30,1994	suzu
 *	1.10		May,30,1995	suzu
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

main()
{
	DISPENV	disp;
	u_long	padd, opadd = 0;
	int	i;
	
	StopCallback();
	PadInit(0);
	ResetGraph(0);
	SetDefDispEnv(&disp, 0, 0,  512,  240);
	setRECT(&disp.screen, 0, 0, 256, 256);
	
	PutDispEnv(&disp);
	SetDispMask(1);
	
	while (((padd = PadRead(1))&PADselect) == 0) {
		
		if (padd & PADRleft)	disp.screen.w--;
		if (padd & PADRright)	disp.screen.w++;
		if (padd & PADRup)	disp.screen.h--;
		if (padd & PADRdown)	disp.screen.h++;
		
		if (padd &PADL1) {
			if (padd & PADLleft)	disp.screen.x -= 2;
			if (padd & PADLright)	disp.screen.x += 2;
			if (padd & PADLup)	disp.screen.y -= 2;
			if (padd & PADLdown)	disp.screen.y += 2;
		}
		else {
			if (padd & PADL2) {
				if (padd & PADLleft)	disp.disp.x--;
				if (padd & PADLright)	disp.disp.x++;
				if (padd & PADLup)	disp.disp.y--;
				if (padd & PADLdown)	disp.disp.y++;
			}
			else {
				if (padd & PADLleft)	disp.disp.x -= 2;
				if (padd & PADLright)	disp.disp.x += 2;
				if (padd & PADLup)	disp.disp.y -= 2;
				if (padd & PADLdown)	disp.disp.y += 2;
			}
		}
		if ((opadd&PADR1) == 0 && (padd&PADR1))
			DumpDispEnv(&disp);
		opadd = padd;
		VSync(0);
		PutDispEnv(&disp);
		if (padd & PADselect) break;
	}
	PadStop();
	ResetGraph(3);
	StopCallback();

	return 0;
}


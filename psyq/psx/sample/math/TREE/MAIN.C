/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Floating-point calculation Sample Program
 *
 *      Copyright (C) 1995 by Sony Computer Entertainment Inc.
 *          All rights Reserved
 *
 *   Version    Date
 *  ------------------------------------------
 *  1.00        Mar,09,1995 yoshi
 *  1.01	Aug,15,1997 yoshi moved descriptions to readme files.
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libmath.h>

#define SEED_X	320		/* first X coordinate */
#define SEED_Y	450		/*  first Y coordinate */
#define ORDER 8			/*  times of branch off */
#define LENGTH 120.0    	/*  length of first branch */
#define ANGLE 0.0		/*  angle of first branch */
#define FACTOR 0.75		/*  rate of branch's variety */
#define TURN 0.4		/*  rate of branch's curve */

#define MSG_X 48
#define MSG_Y 400
#define MSG_W 256
#define MSG_H 48

int draw_tree(int, double, double, int, int, float);
int msg_print(float);

main()
{
	RECT area;
	DRAWENV	draw;			/* drawing environment */
	DISPENV	disp;			/* display environment */
	float turn_rad;
	unsigned long padd,pad,padr;

	ResetCallback();
	PadInit(0);

	ResetGraph(0);	
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */

	SetDefDrawEnv(&draw, 0,   0, 640, 480);
	SetDefDispEnv(&disp, 0,   0, 640, 480);
	draw.dfe = 1;
	PutDrawEnv(&draw);
	PutDispEnv(&disp);
	setRECT(&area,0,0,640,480);
	KanjiFntOpen(MSG_X, MSG_Y, MSG_W, MSG_H, 640, 0, 0, 480, 0, 32);
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	turn_rad = TURN;
	while(1){
		ClearImage(&area,0,0,50);
		DrawSync(0);

		msg_print(turn_rad);
		draw_tree(ORDER,LENGTH,ANGLE,SEED_X,SEED_Y,turn_rad);
		DrawSync(0);

		padd = PadRead(1);

		while(1){
			padr = PadRead(1);
			pad = padr & ~padd;
			padd = padr;
			if (pad & ( PADRdown | PADRright ))
				break;
			if (pad & PADLup){
				if( turn_rad < 0.95 )
					turn_rad += 0.1;
				msg_print(turn_rad);
			}
			if (pad & PADLdown){
				if( turn_rad > 0.05 )
					turn_rad -= 0.1;
				msg_print(turn_rad);
			}
			if (pad & PADselect) {
				PadStop();
				ResetGraph(3);
				StopCallback();
				return 0;
			}
			VSync(0);
		}
	}

	return(0);
}


int
msg_print(float rad)

{
	RECT area;
	char buf[128];

	setRECT(&area,MSG_X,MSG_Y,MSG_W,MSG_H);
	ClearImage(&area,0,0,50);
	
	/* transfer 'rad' to characters by sprintf2(), 
	   because KanjiFntPrint() can't accept '%f,e,E,g,G' format. */
	sprintf2(buf,"%4.2f",rad);
	KanjiFntPrint("‹È—¦%s\n",buf);
	KanjiFntFlush(-1);

	return(0);
}


int
draw_tree(int n, double length, double angle, int x0, int y0, float rad)

{
	double dx,dy;
	int x1,y1;
	LINE_F2 line;
	float turn;

	SetLineF2(&line);
	dx = length * sin(angle);
	dy = length * cos(angle);

	line.r0 = 200;
	line.g0 = 200;
	line.b0 = 200;
	line.x0 = x0;
	line.y0 = y0;
	x1 = x0 - (int)dx;
	y1 = y0 - (int)dy;
	line.x1 = x1;
	line.y1 = y1;
	DrawPrim(&line);

	if( n > 0 ){	/*  recursive call */
		draw_tree(n-1, length*FACTOR, angle+rad, x1, y1, rad); 
		draw_tree(n-1, length*FACTOR, angle-rad, x1, y1, rad); 
	}	

	return(0);
}


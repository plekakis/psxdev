/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto10: High-level interface
 *			
 *	    Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Apr.01,1996	suzu
 *		1.10		Apr,24,1996	suzu
*/	
/*	Following samples shows how to use high level functions.
 *	These fuctions supply convinent access to basic CD-ROM
 *	facilities. */

#include <sys/types.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

void init_graph(int x, int y, int isbg);
int main( void )
{
	static char *title = " CD-ROM TEST MENU  ";
	static char *menu[] = {
		"    Read Check    ",
		"   Exec Program   ",	
		"    CD  Player    ",
		"      Movie       ",
		0,
	};

	unsigned long padd;

	/* reset */
	ResetGraph(0);
	PadInit(0);		
	CdInit();
	
	/* initialize graphic environment */
	init_graph(70, 50, 1);

	/* select MENU */
	while (1) {
		switch (menuUpdate(title, menu, PadRead(1))) {
		    case 0:	Read(); 	init_graph(70, 50, 1);	break;
		    case 1:	Execute();	init_graph(70, 50, 1);	break;
		    case 2:	Play();		init_graph(70, 50, 1);	break;
		    case 3:	Movie();	init_graph(70, 50, 1);	break;
		}

		/* press START+SELECT to exit */
		padd = PadRead( 1 );
		if( padd & PADstart && padd & PADselect )
			break;

		VSync(0);
	}

	CdFlush();
	ResetGraph( 1 );
	PadStop();
	StopCallback();
	return 0;
}

void init_graph(int x, int y, int isbg)
{
	static DRAWENV	draw;
	static DISPENV	disp;
	static int	first = 1;

	if (first) {
		first = 0;
		SetDefDrawEnv(&draw, 0, 0, 320, 240);
		setRGB0(&draw, 0, 0, 200);
		SetDefDispEnv(&disp, 0, 0, 320, 240);
	}

	draw.isbg = isbg;
	PutDrawEnv(&draw);
	PutDispEnv(&disp);
	menuInit(0, x, y, 256);
	SetDispMask(1);
}


/***************************************************************************
 * 
 *			CD-ROM read test
 *
 ***************************************************************************/

static void read_test(char *file);
Read()
{
	static char *title = "   READ CHECK: SELECT FILE    ";
	static char *menu[] = {
		"\\XDATA\\EXE\\CDRCUBE.EXE", 
		"\\XDATA\\EXE\\CDANIM.EXE",
		"\\XDATA\\EXE\\CDBALLS.EXE", 
		0,
	};

	int	id;
	u_int	padd;
	
	/* initialize graphic environment */
	init_graph(40, 90, 0);

	/* open message window */
	SetDumpFnt(FntOpen(8, 180, 304, 16, 1, 256));	

	/* select MENU */
	while ((padd = PadRead(1)) != PADselect) {
		if ((id = menuUpdate(title, menu, padd)) >= 0) 
			read_test(menu[id]);
		VSync(0);
	}
}

#define MAXSECTOR	128
static void read_test(char *file)
{
	u_long	buf[2][MAXSECTOR*512];
	int	i, cnt;

	for (i = 0; i < MAXSECTOR*512; i++)
		buf[0][i] = buf[1][i] = 0;
	
	for (i = 0; i < 2; i++) {
		if (CdReadFile(file, buf[i], MAXSECTOR*2048) == 0) {
			FntPrint("%s: cannot open\n", file);
			FntFlush(-1);
			return;
		}
		while ((cnt =CdReadSync(1, 0)) > 0) {
			FntPrint("reading(%d) %s...\n", i, file);
			FntPrint("%d sectors left\n", cnt);
			FntFlush(-1);
			VSync(0);
		}
		if (cnt < 0) {
			FntPrint("%s: read failed\n", file);
			FntFlush(-1);
			return;
		}
	}
	for (i = 0; i < MAXSECTOR*512; i++)
		if (buf[0][i] != buf[1][i]) {
			FntPrint("%s: verify error\n",file);
			FntFlush(-1);
			return;
		}
	FntPrint("%s: verify complete\n", file);
	FntFlush(-1);

}
/***************************************************************************
 * 
 *		Execute SubProgram
 *
 ***************************************************************************/

static int exec_test(char *file);
Execute()
{
	static char *title = "   EXECUTE PROGRAM    ";
	static char *menu[] = {
		"\\XDATA\\EXE\\CDRCUBE.EXE", 
		"\\XDATA\\EXE\\CDANIM.EXE",
		"\\XDATA\\EXE\\CDBALLS.EXE", 
		0,
	};

	int	id;
	u_int	padd;
	
	/* initialize graphic environment */
	init_graph(40, 90, 0);

	/* open message window */
	SetDumpFnt(FntOpen(8, 180, 304, 16, 1, 256));	

	/* select MENU */
	while ((padd = PadRead(1)) != PADselect) {
		if ((id = menuUpdate(title, menu, padd)) >= 0) {
			exec_test(menu[id]);
			return;
		}
		VSync(0);
	}
}

static int exec_test(char *file)
{
	struct EXEC	*exec;
	DRAWENV	draw;
	DISPENV	disp;

	if ((exec = CdReadExec(file)) == 0)
		return(0);
	
	/* execute */
	GetDispEnv(&disp);		/* push DISPENV */
	GetDrawEnv(&draw);		/* push DRAWENV */
	SetDispMask(0);			/* black out */
	CdSync(0, 0);			/* terminate background command */
	CdFlush();			/* flush CD-ROM */
	PadStop();			/* stop PAD */
	ResetGraph(1);			/* flush GPU */
	StopCallback();			/* stop callback */
	Exec(exec,1,0);			/* execute */
	RestartCallback();		/* recover callback */
	PadInit(0);			/* recover PAD */
	PutDispEnv(&disp);		/* pop DISPENV */
	PutDrawEnv(&draw);		/* pop DRAWENV */
	SetDispMask(1);			/* recover dispmask */
	
	return(exec->t_addr);
}

/***************************************************************************
 * 
 *			CD Player
 *
 ***************************************************************************/

static void play_test(int command);
Play()
{
	static char *title = "  CD PLAYER   ";
	static char *menu[] = {
		"   Play   ",
		"   Stop   ",
		"Next Track",
		"Prev Track",
		0,
	};
	
	int	id;
	u_int	padd;
	
	/* initialize graphic environment */
	init_graph(95, 90, 0);

	/* open message window */
	SetDumpFnt(FntOpen(80, 180, 144, 16, 1, 256));	

	/* select MENU */
	while ((padd = PadRead(1)) != PADselect) {
		play_test(menuUpdate(title, menu, padd));
		VSync(0);
	}

	/* stop CD */
	CdPlay(0, 0, 0);
}


static void play_test(int command)
{
	static int	track[] = {7, 2, 6, 3, 5, 4, 5, 3, 0};
	int		track_id = 0, i;

	/* get current position */
	track_id = CdPlay(3, 0, 0);
	
	/* display current position */
	FntPrint("status: %s\n", track_id>=0? "play": "stop");
	FntPrint("track:  ");
	for (i = 0; track[i]; i++) 
		FntPrint("~c%s%d", i == track_id? "888":"444", track[i]);
	FntFlush(-1);

	switch (command) {
	    case 0:	/* play */
		CdPlay(2, track, 0); break;
		
	    case 1:	/* stop */
		CdPlay(0, 0, 0); break;
		
	    case 2:	/* next track */
		if (track[++track_id] == 0) track_id--;
		CdPlay(2, track, track_id);
		break;
		
	    case 3:	/* prev track */
		if (--track_id < 0) track_id = 0;
		CdPlay(2, track, track_id);
		break;
	}
}
/***************************************************************************
 * 
 *			Movie
 *
 ***************************************************************************/

Movie()
{
	int id;
	
	id = FntOpen(16, 180, 280, 16, 1, 256);	
	FntPrint(id, "Movie: Not Supported now. Sorry...\n");
	FntPrint(id, "     Hit Any Button to Return\n");
	FntFlush(id);

	while(PadRead(1));
	while(PadRead(1) == 0);
}

/*********************************************************/
/*                                                       */
/* OVERLAY DEMONSTRATION for SONY PLAYSTATION Dev system */
/*  Main program module - always resident                */
/*                                                       */
/*********************************************************/

#include <libsn.h>

int   _ramsize=0x00200000; /*  2MB RAM for production Playstation   */
int _stacksize=0x00004000; /* 16KB stack is more than enough        */

/* External routines in loadable levels */

extern void init_level_1 ( void );
extern void init_level_2 ( void );
extern void init_level_3 ( void );

extern void body_level_1 ( void );
extern void body_level_2 ( void );
extern void body_level_3 ( void );

/* External pointer to load area */

extern char *loadaddress;

/* Globals */

int game_over;		/* flag set when game over */
int level_over;		/* flag set when level finished */

/* common routine 1 */

void common1 ( void )
{
	int	i;

	for(i=0;i<4000;i++)
		;
}

/* common routine 2 */

void common2 ( void )
{
	int	i;

	for(i=0;i<12000;i++)
		;
}

void msg(char *ptr)
{
	PCwrite(-1,ptr,strlen(ptr));
}

void clearmsg(void)
{
	PCwrite(-1,"\f",1);
}

static load_level (int level)
{
	int h, l;

	static char filename[] = "lX.bin";

	filename[1] = level + '0';

	h = PCopen(filename, 0, 0);
	l = PClseek(h, 0, 2);
	PClseek(h, 0, 0);
	PCread(h, loadaddress, l);
	PCclose(h);
	FlushCache();

	/* Level specific initialisation */

	switch(level) {
		case 1:
			init_level_1 ();
			break;
		case 2:
			init_level_2 ();
			break;
		case 3:
			init_level_3 ();
			break;
	}
}

static int main_loop (int level)
{
	/*

            Do main loop stuff here

        */

	/* Now do level specific stuff */

	switch(level) {
		case 1:
			body_level_1 ();
			break;
		case 2:
			body_level_2 ();
			break;
		case 3:
			body_level_3 ();
			break;
	}
}

/* very basic game loop */

void main ( void )
{
	int level;

	int	reg3=1,reg4=2;
	
	register struct t{
		int	spango;
	} regstruc;

	struct s{
		int	gs1;
		int	gs2;
		short	gs3;
		char *ptr;
	} gamestruc;

	gamestruc.ptr="ABCD";
	regstruc.spango=23;

/*	msg(gamestruc.ptr); */

	gamestruc.gs1=0x23232323;
	gamestruc.gs2=0x88888888;
	gamestruc.gs2=0x12345A;

	for(reg3=0;reg3<10;reg3++){
		reg4--;
		regstruc.spango++;
	}


	clearmsg();

	while (1) {
		game_over = 0;
		for (level = 1; (! game_over) && (level <= 3); level++) {
			load_level(level);
			level_over = 0;

			while ((! game_over) && (! level_over)) {
				main_loop(level);
				pollhost();
			}
		}
	}
}


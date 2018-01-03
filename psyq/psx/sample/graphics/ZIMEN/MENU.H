/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*				menu
 *
 *			Simple Menu Database
 *
 *		Copyright (C) 1997 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	0.10		Jul,15,1997	suzu
 */

/*
 * program list*/
void  tuto0(void),tuto1(void),tuto2(void),tuto3(void),tuto4(void),tuto5(void);

/*
 * menu title*/
static char *_title = "Zimen - The Ground - ";
	
/*
 * program list*/
static char *_list[] = {
	" tuto0   demo help : primitive divide        ",
	" tuto1   demo help : area clip               ",
	" tuto2   demo help : limitted CELL map       ",
	" tuto3   demo help : unlimitted CELL map     ",
	" tuto4   demo help : with automatic divide   ",
	" tuto5   demo help : fog                     ",
	0,
};

/*
 * Cursor stop position; number indicates cursor width */
static char *_attr[] = {
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	0,
};

/*
 * Detail message; shown in the bottom line according to cursor position */
static char *_msg[] = {
	" automatically divided POLY_FT4 ",
	" Corn clip ",
	" The ground expressed with the limited map ", 
	" The ground expressed with the lap-rounded map", 
	" Automatic division to reduce texture distortion",
	" CLUT fog ",
	0,
};

/*
 * function name to be executed */
static void (*_func[])() = {
	tuto0,
	tuto1,
	tuto2,
	tuto3,
	tuto4,
	tuto5,
};

/*
 * help message; shown when HELP button is selected */
static char *_help[] = {
	"tuto0: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto1: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto2: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto3: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto4: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto5: Usage",
	"   Up       turn up      ",
	"   Down     turn down    ",
	"   Right    turn  right  ",
	"   Left     turn left    ",
	"   -------",	
	"   Circle   rotate right  ",
	"   Square   rotate left   ",
	"   -------",	
	"   R1       go forword     ",
	"   R2       go backword    ",
	"   -------",	
	"   SELECT   exit",
	0,
};

/* (for GUI debug) 
void tuto0(void) {printf("tuto0...\n");}
void tuto1(void) {printf("tuto1...\n");}
void tuto2(void) {printf("tuto2...\n");}
void tuto3(void) {printf("tuto3...\n");}
void tuto4(void) {printf("tuto4...\n");}
void tuto5(void) {printf("tuto5...\n");}
*/


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
void  tuto1(void), tuto2(void), tuto3(void),tuto4(void);
void  tuto5(void), tuto6(void), tuto7(void);

/*
 * menu title*/
static char *_title = "Movie Tutorial";
	
/*
 * program list*/
static char *_list[] = {
	" tuto1   demo help : VLC+MDEC                 ",	
	" tuto2   demo help : Piplined LoadImage       ",	
	" tuto3   demo help : On-memory movie          ",
	" tuto4   demo help : callback                 ",
	" tuto5   demo help : callback chain           ",
	" tuto6   demo help : piplined on-memory movie ",
	" tuto7   demo help : tune-up parametes        ",
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
	"         4    4 ",
	0,
};

/*
 * Detail message; shown in the bottom line according to cursor position */
static char *_msg[] = {
	"simple VLC decode and MDEC on memory decompression",
	"paralell execution of LoadImage() and DecDCTout()",
	"simple on-memory movie operation",
	"handshake using callback",
	"parallel execution using callback",
	"complete background on-memory movie decompression",
	"fine tune-up for parameters",
	0,
};

/*
 * function name to be executed */
static void (*_func[])() = {
	tuto1,
	tuto2,
	tuto3,
	tuto4,
	tuto5,
	tuto6,
	tuto7,
};

/*
 * help message; shown when HELP button is selected */
static char *_help[] = {
	
	"tuto1: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto2: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto3: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto4: Usage",
	"   SELECT   exit    ",
	0,

	"tuto5: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto6: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto7: Usage",
	"   Up        Incriment Y[0,0]      ",
	"   Down      Decriment Y[0,0]      ",
	"   Right     Incriment Cb/Cr[0,0]  ",
	"   Left      Decriment Cb/Cr[0,0]  ",
	"   -------",	
	"   SELECT   exit",
	0,
};

/* (for GUI debug) 
void tuto1(void) {printf("tuto1...\n");}
void tuto2(void) {printf("tuto2...\n");}
void tuto3(void) {printf("tuto3...\n");}
void tuto4(void) {printf("tuto4...\n");}
void tuto5(void) {printf("tuto5...\n");}
void tuto6(void) {printf("tuto6...\n");}
void tuto7(void) {printf("tuto7...\n");}
*/

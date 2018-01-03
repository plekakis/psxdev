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
void	tuto0(void),tuto1(void);

/*
 * menu title*/
static char *_title = "relation of GS and GPU";
	
/*
 * program list
 *=============================================================
 *	colum	contents
 *------------------------------------------------------------
 *	 0-8	program name
 *	 9-13	execututtion button name
 *	14-17	help button name
 *	18-	description
 *------------------------------------------------------------ */
static char *_list[] = {
	" tuto0   demo help : primitivis on libgs     ",
	" tuto1   demo help : objects on libgpu       ",
	0,
};

/*
 * Cursor stop position; number indicates cursor width */
static char *_attr[] = {
	"         4    4 ",
	"         4    4 ",
	0,
};

/*
 * Detail message; shown in the bottom line according to cursor position */
static char *_msg[] = {
	" append lowlevel primitives on the GsOT ",
	" append complex TMD objects on simple OT ",
	0,
};

/*
 * function name to be executed */
static void (*_func[])() = {
	tuto0,
	tuto1,
};

/*
 * help message; shown when HELP button is selected */
static char *_help[] = {
	"tuto0: Usage",
	"   Up       rotate balls -x ",
	"   Down     rotate balls +x ",
	"   Right    rotate balls -y ",
	"   Left     rotate balls +y ",
	"   -------",	
	"   L1       move balls far   ",
	"   L2       move balls near ",
	"   -------",	
	"   Triangle rotate TMD -x  ",
	"   Cross    rotate TMD +x  ",
	"   Circle   rotate TMD -y  ",
	"   Square   rotate TMD +y  ",
	"   -------",	
	"   R1       move TMD far   ",
	"   R2       move TMD near ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto1: Usage",
	"   Up       rotate balls -x ",
	"   Down     rotate balls +x ",
	"   Right    rotate balls -y ",
	"   Left     rotate balls +y ",
	"   -------",	
	"   L1       move balls far   ",
	"   L2       move balls near ",
	"   -------",	
	"   Triangle rotate TMD -x  ",
	"   Cross    rotate TMD +x  ",
	"   Circle   rotate TMD -y  ",
	"   Square   rotate TMD +y  ",
	"   -------",	
	"   R1       move TMD far   ",
	"   R2       move TMD near ",
	"   -------",	
	"   SELECT   exit",
	0,
};

/* (for GUI debug) 
void tuto0(void) {printf("tuto0...\n");}
void tuto1(void) {printf("tuto1...\n");}
*/


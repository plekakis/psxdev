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

void	Balls(void), Diffuse(void), Mat(void), Rgb24(void), Morph(void);

static char *_title = "Basic Graphics Demonstration";
	
static char *_list[] = {
	" balls   demo help : 2D sprite                 ", 
	" diffuse demo help : basic geometry operation  ", 
	" mat     demo help : 3D sprite                 ", 
	" rgb24   demo help : 24bit RGB display mode    ", 
	" morph   demo help : 2D Image manipulation     ", 
	0,
};

static char *_attr[] = {
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	0,
};

static char *_msg[] = {
	"Draw up to 2000 sprites (16x16)",
	"Draw 9x9x9 3D particle and balls",
	"3D located sprite animation",
	"24bit color high resolution mode",
	"Texture mapping onto free 3D curve surface",
};

static char *_help[] = {
	"Balls: Usage",
	"   Up       incriment number of balls   ",
	"   Down     decriment number of balls   ",
	"   Triangle incriment number of balls   ",
	"   Cross    decriment number of balls   ",
	"   SELECT   exit",
	0,
	
	"Diffuse: Usage",
	"   Up       Rotate +x angle     ",
	"   Down     Rotate -x angle     ",
	"   Right    Rotate -y angle     ",
	"   Left     Rotate +y angle     ",
	"   Circle   Toggle Object Type  ",
	"   Cross    Toggle Semitrans    ",
	"   SELECT   exit",
	0,
	
	"Mat: Usage",
	"   Up       Rotate +x angle",
	"   Down     Rotate -x angle",
	"   Right    Rotate -y angle",
	"   Left     Rotate +y angle",
	"   Triangle Incriment number of matchang   ",
	"   Cross    Decriment number of matchang   ",
	"   SELECT   exit",
	0,
	
	"Rgb24: Usage",
	"   SELECT   exit  ",
	0,
	
	"Morph: Usage",
	"   Up       Rotate +x angle",
	"   Down     Rotate -x angle",
	"   Right    Rotate -y angle",
	"   Left     Rotate +y angle",
	"   Triangle Twist  +y direction",
	"   Cross    Twist  -y direction",
	"   Circle   Twist  +x direction",
	"   Square   Twist  -x direction",
	"   -------",	
	"   L1       Pull up ",
	"   L2       Push down ",
	"   R1+Up    Move sticked point (up)",	
	"   R1+Down  Move sticked point (down)",	
	"   R1+Right Move sticked point (right)",	
	"   R1+Left  Move sticked point (left)",	
	"   -------",	
	"   START    Reset position",
	"   SELECT   Explosion",
	"   * Push SELECT twice to exit",
	0,
};
	
static void (*_func[])() = {
	Balls,
	Diffuse,
	Mat,
	Rgb24,
	Morph,
};

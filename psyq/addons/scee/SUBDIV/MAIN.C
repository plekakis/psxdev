/*--------------------------------------------------

here is a quick Sony Playstation 2D sub-divide demo (C) 1996 Derek Leigh-Gilchrist
EMAIL: del@nlights.demon.co.uk

Poly GT3 & GT4 Fix: Morten Ofstad / SCEE.

TAB SETTING = 2 (otherwise this will look UGLY, well actually it looks ugly anyway!)

this version updated to split polys into a max of 16 bits (instead of the libs version 64!!)

use a pad in port 1, here are the controls...

SELECT   = change poly type (ft3,gt3,ft4,gt4)
START    = pause rotation
LEFT     = reduce amount of subdivision (0=no subdivide, 1=split into 4 polys, 2=split into 16 polys)
RIGHT    = increase amount of subdivision (0=no subdivide, 1=split into 4 polys, 2=split into 16 polys)
UP       = ZOOM OUT
DOWN     = ZOOM IN
SQUARE   = rotate Y(-)
CIRCLE   = rotate Y(+)
CROSS    = rotate X(-)
TRIANGLE = rotate X(+)

if you hold the L1 button and move up,down,left,right you can move the poly`s origin...

--------------------------------------------------*/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsn.h>
#include <kernel.h>
#include "subdiv.h"

#define		FONT_ENABLE							/* libs debug window on/off */
#define		MAXOT 256						 		/* max entries in OTAG */

// taste some globals... (hoho)

void			*freeram_addr;					/* free ram start address */

u_long		*clut_pointer;					// pointer to clut data
u_long		*texture_pointer;				// pointer to texture data

u_long		*cur_ot; 				 				/* current ordering table */
u_long		*last_ot;				 				/* last ordering table */

u_long		*ot1;										// ordering table #1
u_long		*ot2;										// ordering table #2

u_long		*primlist1; 	 					/* primitive list1 */
u_long		*primlist2; 	 					/* primitive list2 */
u_long		*cur_primlist; 					/* current primitive list */

int				otcnt=0; 		 	 	 				/* ordering table counter */

int				framecnt;
int				frame;
u_int			padd;						 				/* joypad var */

u_short		tpageid;								/* texture page ID */
u_short		clutid;   	  	 	 			/* Clut ID */

#define		SCR_Z		512							/* distant to screen */
SVECTOR		x[4];							 			/* wall's position */
long			dmy, flg;								/* dummy */

SVECTOR		ang  = {0, 0, 512};
VECTOR		vec  = {0, 0, SCR_Z+256};
MATRIX		m;

int			debounce1=0;							// cack debounce var for left on joypad
int			debounce2=0;							// cack debounce var for right on joypad
int			debounce3=0;							// cack debounce var for select on joypad

int			subdivide_amount=0;				// number of sub-divides to do...
int			demo_type=0;							// which poly type to show (FT3,GT3,FT4,GT4)

int			origin_x=160;
int			origin_y=120;

#define NDIVMAX		2


/*** prototypes ***/
void		main(void);
static	int myvbl(void);
void		init_lists(void);

void 		do_user_input(void);
void		control_the_plate(void);

void		do_demo(void);
void 		demo_ft4(void);
void 		demo_gt4(void);
void 		demo_ft3(void);
void 		demo_gt3(void);

void		*get_mem(int allocsize);
int 		loadfile(char *filename, char *buffer);



/***** MAIN *****/

void	main(void)
{
	DRAWENV		draw1[2];    		 						 					/* drawing environment */
	DISPENV		disp[2];      	 									 		/* display environment */
	BLK_FILL	bg1;	 					 			 								/* CLS background1 */
	BLK_FILL	bg2;   	 	 		 												/* CLS background2 */

	ResetCallback();

	PadInit(0);						 						 			 				/* initialize PAD */
	ResetGraph(0);				 						 			 				/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);			 						 				 			/* set debug mode */
	VSyncCallback((void *)myvbl	);	 				 				/* setup my vbl callback */

	printf("SUBDIVISION CODE BY DEL / CORE DESIGN (del@nlights.demon.co.uk)\n");

	PCinit();									 	 										/* init PC file system */

	/* initialize plate position */
	setVector(&x[0], -128, -128, 0); setVector(&x[1],  128, -128, 0);
	setVector(&x[2], -128,  128, 0); setVector(&x[3],  128,  128, 0);
	InitGeom();											 								/* initialize geometry subsystem */
	SetGeomOffset(160, 120); 		 										/* set geometry origin as (160, 120) */
	SetGeomScreen(SCR_Z);	 													/* distance to viewing-screen */

	/* set draw & display environment for each buffer */
	SetDefDrawEnv(&draw1[0], 0,   0, 320, 240);
	SetDefDrawEnv(&draw1[1], 0, 240, 320, 240);
	SetDefDispEnv(&disp[0], 0, 240, 320, 240);
	SetDefDispEnv(&disp[1], 0,   0, 320, 240);

	#ifdef FONT_ENABLE
	FntLoad(960, 256);															/* load basic font pattern */
	SetDumpFnt(FntOpen(16, 200, 288, 32, 0, 1024));
	#endif

	/* initialize background cls primitive 1*/
	SetBlockFill(&bg1);   	 							 					/* set BlockFill primitive ID */
	setRGB0(&bg1, 80, 20, 40);	 	 				 	 					/* (R,G,B)=(0,0,0) */
	setXY0(&bg1,  0,   0); 	 	 				 	 	 		 			/* right-upper point is (0,0) */
	setWH(&bg1, 320, 240); 	 				 			 					/* size is (320x240) */

	/* initialize background cls primitive 2*/
	SetBlockFill(&bg2);    													/* set BlockFill primitive ID */
	setRGB0(&bg2, 80, 20, 40);	 							 		 		/* (R,G,B)=(0,0,0) */
	setXY0(&bg2,  0,   240); 												/* right-upper point is (0,240) */
	setWH(&bg2, 320, 240); 	 									 			/* size is (320x240) */

	// setup ram base address...
	freeram_addr = (void *)0x080000; 								// free ram starts at $80000

	// allocate ram for otags,primlists,textures & cluts
	ot1 = (u_long *)get_mem(MAXOT*4);		 						// space for otag1
	ot2 = (u_long *)get_mem(MAXOT*4);								// space for otag2
	primlist1 = (u_long *)get_mem(0x010000);				// 64k for primlist1
	primlist2 = (u_long *)get_mem(0x010000);				// 64k for primlist2
	texture_pointer = (u_long *)get_mem(0x010000);	// 64k for texture data
	clut_pointer = (u_long *)get_mem(512);					// 512 bytes for my clut...

	// load the texture & clut data
	loadfile( (char *)"data\\cd2.clt", (char *)clut_pointer);
	loadfile( (char *)"data\\cd2.raw", (char *)texture_pointer);

	// download the texture & clut data to VRAM
	clutid = LoadClut(clut_pointer, 0,500);
	tpageid = LoadTPage(texture_pointer, 1, 0, 320,  0, 256,256);

	DrawSync(0);																		// wait for end of drawing...!

	DrawPrim( (u_long *)&bg1 ); 			 		 					/* cls frame buffer 1 */
	DrawPrim( (u_long *)&bg2 );				 							/* cls frame buffer 2 */

	ClearOTag(ot1, MAXOT);	 												/* clear ordering table1 */
	ClearOTag(ot2, MAXOT);							 			 			/* clear ordering table2 */

	padd = 0;
	VSync(0);			 		     		   	 			 		 				/* wait for V-BLNK */
	framecnt=0;

	SetDispMask(1);		      												/* enable the display */

/*-----------------23/06/95 11:43-------------------
 this is the MAIN-LOOP... just remember that!
--------------------------------------------------*/

	while ( 1 )																 	 		/* loop forever */
	{

		padd = PadRead(1);		 												/* read the pad */

		/* swap double buffer */
		PutDrawEnv(frame%2? &draw1[1]:&draw1[0]);			/* init drawing enviroment (whole screen) */
		PutDispEnv(frame%2? &disp[1]:&disp[0]);				/* init display enviroment */

		init_lists();																	/* setup current prim lists etc. */
		DrawOTag(last_ot);				 				 			 			/* draw the last OT list */
		ClearOTag((u_long *)cur_ot, MAXOT);						/* clear the current ordering table */

		otcnt=0;

		// add cls prim to clear the screen
		AddPrim(&cur_ot[otcnt++], frame%2? ( (u_long *)&bg1 ):( (u_long *)&bg2 ) );

		/* add drawing enviroment prim */
		SetDrawEnv( (DR_ENV *)cur_primlist, frame%2? &draw1[0]:&draw1[1]);
		AddPrim(&cur_ot[otcnt++], (u_long *)cur_primlist);
		cur_primlist += 16;

		// this reacts to your joypad movements...
		do_user_input();
		// this spins the plate & changes angles with joypad...
		control_the_plate();

		SetGeomOffset(origin_x, origin_y); 		 		/* set geometry origin as (160, 120) */
		do_demo();

		DrawSync(0);															// wait for end of drawing...!
		VSync(0);			 		     		   	 			 			/* wait for V-BLNK */

		// shockingly poor ROM font stuff... (it works, but only just!)
		#ifdef FONT_ENABLE
		FntPrint("subdivide_amount=%d\n", subdivide_amount);
		FntPrint("ang.x=%d, ang.y=%d, ang.z=%d\n", ang.vx,ang.vy,ang.vz);
		FntFlush(-1);
		#endif

		framecnt=0;
		frame++;
	}

// we never get here...

}

/*-----------------12-07-96 02:40pm-----------------
  Vsync callback
--------------------------------------------------*/
static int myvbl(void)
{
	framecnt++;
}


/*-----------------12/07/96 14:55-------------------
 setup the current and last list pointers....
--------------------------------------------------*/

void		init_lists(void)
{

	last_ot = frame%2? ot2:ot1;		 		 				 									/* current ordering table */
	cur_ot = frame%2? ot1:ot2;		 		 				 									/* current ordering table */

	/** setup current primitve list **/
	cur_primlist = frame%2? primlist1:primlist2;								/* current primitive list */

	return;
};


//
// demo the poly...!
//

void	do_demo(void)
{
	// do which ever demo you selected...
	switch ( demo_type )
	{
		case (0):
		{
			demo_ft3();							// subdiv demo #0 (kewl)
			break;
		}
		case (1):
		{
			demo_gt3();							// subdiv demo #1 (kewl)
			break;
		}
		case (2):
		{
			demo_ft4();							// subdiv demo #2 (kewl)
			break;
		}
		case (3):
		{
			demo_gt4();							// subdiv demo #3 (kewl)
			break;
		}
	}
}



/*-----------------12-07-96 02:52pm-----------------
 react to users joypad movements.... (bleak)
--------------------------------------------------*/

void	do_user_input(void)
{

	if ( (padd&PADL1)==0 )
	{

		if (padd & PADLup)
 			vec.vz += 16;					// zoom out
		if (padd & PADLdown)
 		 	vec.vz -= 16; 				// zoom in

// change number of sub-divisions with left/right
		if ( debounce1==0 )
		{
			if ( (padd & PADLleft) && subdivide_amount > 0 )
 			{
				debounce1=1;
 				subdivide_amount--;
			}
		}
		else
		{
			if ( (padd & PADLleft)==0 )
			{
				debounce1=0;
			}
		}

		if ( debounce2==0 )
		{
			if ( (padd&PADLright) && subdivide_amount < (NDIVMAX) )
			{
				debounce2=1;
				subdivide_amount++;
			}
		}
		else
		{
			if ( (padd & PADLright)==0 )
			{
				debounce2=0;
			}
		}

	}
	else
	{
 		if (padd & PADLup)
 			origin_y -= 4;
 		if (padd & PADLdown)
 			origin_y += 4;
 		if (padd & PADLleft)
 			origin_x -= 4;
 		if (padd & PADLright)
			origin_x += 4;

	}

		// rotations
 		if (padd & PADRup)
 			ang.vx += 32;
 		if (padd & PADRdown)
 			ang.vx -= 32;
 		if (padd & PADRleft)
 			ang.vy += 32;
 		if (padd & PADRright)
			ang.vy -= 32;



	// spin the plate unless start is pressed
	if ( (padd&PADstart)==0 )
		ang.vz += 32;					// spin this slag (z)

	// change poly type with select (4 types)
	if ( debounce3==0 )
	{
 		if ( padd & PADk )
 		{
			debounce3 = 1;
 			demo_type++;
 			if ( demo_type == 4 )
 				demo_type = 0;
		}
	}
	else
	{
		if ( (padd & PADk)==0 )
		{
			debounce3=0;
		}
	}


	return;

}

/*-----------------12-07-96 03:08pm-----------------
 sort the gte stuff out....
--------------------------------------------------*/
void	control_the_plate(void)
{

	ang.vx &= 4095;
	ang.vy &= 4095;
	ang.vz &= 4095;				// angle range me !

// setup matrices..
	RotMatrix(&ang, &m);
	TransMatrix(&m, &vec);
	SetRotMatrix(&m);
	SetTransMatrix(&m);

}


/*-----------------12-07-96 04:14pm-----------------
 here is a quick demo of a POLY_FT4...
 it calls the routine DEL_subdivide_ft4() to do the subdivision (SWEET)
--------------------------------------------------*/
void	demo_ft4(void)
{

	POLY_FT4	 		*polyme;
	int						primskip;

	// setup a big poly (ready for subdivision)
	polyme=(POLY_FT4 *)cur_primlist;

	/** primitive length = 9 **/
	setlen(polyme,9);
	/** primitive code (polyFT4,not trans, no shadetex) */
	setcode(polyme,( ((0x02c)&~2)|1)  );

	/* rotate and shift each vertex of the wall */
	RotTransPers(&x[0], (long *)&polyme->x0, &dmy, &flg);
	RotTransPers(&x[1], (long *)&polyme->x1, &dmy, &flg);
	RotTransPers(&x[2], (long *)&polyme->x2, &dmy, &flg);
	RotTransPers(&x[3], (long *)&polyme->x3, &dmy, &flg);

	polyme->tpage = tpageid;
 	polyme->clut  = clutid;
	setUV4(polyme,0, 0, 0+255, 0, 0, 0+255, 0+255, 0+255);

	if ( subdivide_amount==0 )
	{
		// no subdivision, so just add to otag as normal...
		AddPrim(&cur_ot[otcnt++], (u_long *)polyme);
		cur_primlist+=10;
	}
	else
	{
		// subdivide the poly... :)
		primskip = DEL_subdivide_ft4_asm( (POLY_FT4 *)cur_primlist,(u_long *)&cur_ot[otcnt],subdivide_amount);
		cur_primlist += primskip;
		otcnt++;
	}

	return;
}

/*-----------------12-07-96 04:14pm-----------------
 here is a quick demo of a POLY_GT4...
 it calls the routine DEL_subdivide_gt4() to do the subdivision (SWEET)
--------------------------------------------------*/
void	demo_gt4(void)
{

	POLY_GT4	*polyme;
	int primskip;

	// setup a big poly (ready for subdivision)
	polyme=(POLY_GT4 *)cur_primlist;

	/** primitive length = 12 **/
	setlen(polyme,12);
	/** primitive code (polyGT4,not trans, shadetex) */
	setcode(polyme,( ((0x03c)|2)&~1)  );

	//setRGB0(polyme,255,255,255);
	//setRGB1(polyme,192,192,192);
	//setRGB2(polyme,128,128,128);
	//setRGB3(polyme,64,64,64);

	// Set up different RGBs. 
	setRGB0(polyme,128,128,128);
	setRGB1(polyme,32, 64, 128);
	setRGB2(polyme,128,128,128);
	setRGB3(polyme,128, 64, 32);

	/* rotate and shift each vertex of the wall */
	RotTransPers(&x[0], (long *)&polyme->x0, &dmy, &flg);
	RotTransPers(&x[1], (long *)&polyme->x1, &dmy, &flg);
	RotTransPers(&x[2], (long *)&polyme->x2, &dmy, &flg);
	RotTransPers(&x[3], (long *)&polyme->x3, &dmy, &flg);

	polyme->tpage = tpageid;
 	polyme->clut  = clutid;
	setUV4(polyme,0, 0, 0+255, 0, 0, 0+255, 0+255, 0+255);

	if ( subdivide_amount==0 )
	{
		// no subdivision, so just add to otag as normal...
		AddPrim(&cur_ot[otcnt++], (u_long *)polyme);
		cur_primlist+=13;
	}
	else
	{
		// subdivide the poly... :)
		primskip = DEL_subdivide_gt4_asm( (POLY_GT4 *)cur_primlist, (u_long *)&cur_ot[otcnt], subdivide_amount );
		cur_primlist += primskip;
		otcnt++;
	}

	return;
}

/*-----------------12-07-96 10:25am-----------------
 here is a quick demo of a POLY_FT3...
 it calls the routine DEL_subdivide_ft3() to do the subdivision (SWEET)
--------------------------------------------------*/
void 		demo_ft3(void)
{

	POLY_FT3	 		*polyme;
	int	primskip;

	// setup a big poly (ready for subdivision)
	polyme=(POLY_FT3 *)cur_primlist;

	/** primitive length = 7 **/
	setlen(polyme,7);
	/** primitive code (polyFT3,not trans, no shadetex) */
	setcode(polyme,( ((0x024)&~2)|1)  );

	/* rotate and shift each vertex of the wall */
	RotTransPers(&x[0], (long *)&polyme->x0, &dmy, &flg);
	RotTransPers(&x[1], (long *)&polyme->x1, &dmy, &flg);
	RotTransPers(&x[2], (long *)&polyme->x2, &dmy, &flg);

	polyme->tpage = tpageid;
 	polyme->clut  = clutid;
	setUV3(polyme,0, 0, 0+255, 0, 0, 0+255);

	if ( subdivide_amount==0 )
	{
		// no subdivision, so just add to otag as normal...
		AddPrim(&cur_ot[otcnt++], (u_long *)polyme);
		cur_primlist+=8;
	}
	else
	{
		// subdivide the poly... :)
		primskip = DEL_subdivide_ft3_asm( (POLY_FT3 *)cur_primlist, (u_long *)&cur_ot[otcnt], subdivide_amount );
		cur_primlist += primskip;
		otcnt++;
	}

	return;
}

/*-----------------12-07-96 10:25am-----------------
 here is a quick demo of a POLY_GT3...
 it calls the routine DEL_subdivide_gt3() to do the subdivision (SWEET)
--------------------------------------------------*/
void 		demo_gt3(void)
{

	POLY_GT3	 		*polyme;
	int primskip;

	// setup a big poly (ready for subdivision)
	polyme=(POLY_GT3 *)cur_primlist;

	/** primitive length = 9 **/
	setlen(polyme,9);
	/** primitive code (polyGT3,not trans, shadetex) */
	setcode(polyme,( ((0x034)&~2)&~1)  );

	/* rotate and shift each vertex of the wall */
	RotTransPers(&x[0], (long *)&polyme->x0, &dmy, &flg);
	RotTransPers(&x[1], (long *)&polyme->x1, &dmy, &flg);
	RotTransPers(&x[2], (long *)&polyme->x2, &dmy, &flg);

	//setRGB0(polyme,255,255,255);
	//setRGB1(polyme,192,192,192);
	//setRGB2(polyme,128,128,128);

	// Set up different RGBs. 
	setRGB0(polyme,128,128,128);
	setRGB1(polyme,32, 64, 128);
	setRGB2(polyme,128,128,128);

	polyme->tpage = tpageid;
 	polyme->clut  = clutid;
	setUV3(polyme,0, 0, 0+255, 0, 0, 0+255);

	if ( subdivide_amount==0 )
	{
		// no subdivision, so just add to otag as normal...
		AddPrim(&cur_ot[otcnt++], (u_long *)polyme);
		cur_primlist+=10;
	}
	else
	{
		// subdivide the poly... :)
 		primskip = DEL_subdivide_gt3_asm( (POLY_GT3 *)cur_primlist,(u_long *)&cur_ot[otcnt],subdivide_amount);
		cur_primlist += primskip;
		otcnt++;
	}

	return;
}



/*-----------------12/09/95 17:30-------------------
 dels handy get_mem(); (always returns a long alligned address...)
--------------------------------------------------*/

void	*get_mem(int allocsize)
{
void	*ram;

	ram = freeram_addr;

	if ( (allocsize&3)!=0 )
	{
		allocsize = ((allocsize+4)&0xfffffffc);
	}

	freeram_addr += allocsize;
	return(ram);
}

/*-----------------13/06/95 15:56-------------------
 del`s handy loadfile routine
--------------------------------------------------*/

int	loadfile(char *filename, char *buffer)
{

int	length;
int	bytesread;
int	filehandle;

	filehandle = PCopen(filename,0,0);
	length = PClseek( filehandle,0,2);
	PClseek( filehandle,0,0);
	bytesread = PCread(filehandle, (char *)buffer ,length);
	PCclose(filehandle);

	return(bytesread);

};





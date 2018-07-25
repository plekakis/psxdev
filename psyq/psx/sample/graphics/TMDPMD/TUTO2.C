/* $PSLibId: Run-time Library Release 4.4$ */
/*			    tuto2
 */			
/*		PMD viewer (using function table)		 */
/*		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	----------------------------------------------------	
 *	1.00		Jul, 7,1994	oka
 *	2.00		Jul,21,1994	suzu	
 *	2.01		Mar, 6,1997	sachiko	(added autopad)
  */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>	

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define TEX_ADDR	0x800f0000	/* texture address */
#define PMD_ADDR	0x800e0000	/* PMD object address */
#define OTLENGTH	8		/* OT depth */
#define OTSIZE		(1<<OTLENGTH)
#define SCR_Z		1024

static int pad_read(void);
static void RotPMD(u_long *ot, int id);
static void loadTIM(u_long *addr);

void tuto2(void)
{
	u_long	otbuf[2][1<<OTLENGTH]; 	/* OT */
	int     id;			/* double buffer ID */

	db_init(640, 480, SCR_Z, 60, 120, 120);	/* init */
	loadTIM((u_long *)TEX_ADDR);		/* load texture */
	SetDispMask(1);				/* start display */

	while (pad_read() == 0) {
		id = id? 0: 1;			/* swap */
		ClearOTagR(otbuf[id], OTSIZE);	/* clear OT */
		RotPMD(otbuf[id], id);		/* add PMD to OT */
		FntPrint("PMD viewer(1)\n");	/* message */
		db_swap(otbuf[id]+OTSIZE-1);	/* draw OT */
	}
	DrawSync(0);
	return;
}

/*
 * update world-screen matrix according to input of controller */
static int pad_read(void)
{
	static SVECTOR	ang   = {512, 512, 512};	/* rotate angle */
	static VECTOR	vec   = {0,     0, SCR_Z*5};	
	static MATRIX	m;				/* matrix */
	static int	scale = ONE;
	
	VECTOR	svec;
	int 	padd = PadRead(1);
	
	if (padd & PADselect)	return(-1);
	if (padd & PADRup)	ang.vx -= 32;
	if (padd & PADRdown)	ang.vx += 32;
	if (padd & PADRright)	ang.vy -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADR1) 	ang.vz += 32;
	if (padd & PADR2)	ang.vz -= 32;

	if (padd & PADL1)	vec.vz += 32;
	if (padd & PADL2) 	vec.vz -= 32;

	ang.vz += 4;
	ang.vy += 4;
	
	setVector(&svec, scale, scale, scale);
	RotMatrix(&ang, &m);	
	TransMatrix(&m, &vec);	
	ScaleMatrix(&m, &svec);
	db_set_matrix(&m);
	return(0);
}		

/*
 * Load TIM data from main memory to the frame buffer */	
static void loadTIM(u_long *addr)
{
	TIM_IMAGE	image;		/* TIM header */
	
	OpenTIM(addr);			/* open TIM */
	while (ReadTIM(&image)) {
		if (image.caddr)	/* load CLUT (if needed) */
			LoadImage(image.crect, image.caddr);
		if (image.paddr) 	/* load texture pattern */
			LoadImage(image.prect, image.paddr);
	}
}


/*
 * PMD functions
 *	There are 16 PMD functions accroding to its vertex format
 * */	
	
/* independent vertex type (8 type) */
static void (*PMD_func[])() = {
	RotSMD_FT3,	RotSMD_FT4,	RotSMD_GT3,	RotSMD_GT4,
	RotSMD_F3,	RotSMD_F4,	RotSMD_G3,	RotSMD_G4,
};

/* shared vertex type (8 type) */
static void (*PMD_SV_func[])() = {
	RotSMD_SV_FT3,	RotSMD_SV_FT4,	RotSMD_SV_GT3,	RotSMD_SV_GT4,
	RotSMD_SV_F3,	RotSMD_SV_F4,	RotSMD_SV_G3,	RotSMD_SV_G4,
};

/*
 * draw PMD  */	
static void RotPMD(u_long *ot, int id)
{
	int     i,j;
	long	*pmdtop;	/*PMD file PRIMITIVE Gp top address*/
	long	*pmdwc;		/*PMD file word counter*/
	long	pointer;	/*PMD file POINTER */
	long	*ptop;		/*PMD file PRIMITIVE Gp Block top address*/
	long	pid;		/*PMD file pid*/
	long	*primtop;	/*PMD file PRIMITIVE Gp top address*/
	long	nobj;		/*PMD file NOBJ*/
	long	nptr;		/*PMD file NPTR*/
	long	type_npacket;	/*PMD file PRIMITIVE Gp header*/
	short	type;		/*PMD file PRIMITIVE Gp type*/
	short	ltype;		/*PMD file PRIMITIVE Gp local type*/
	long	*svtop;		/*PMD file Vertex top address*/
	long	backc;		/*PMD file Back clip ON/OFF flag*/

	pmdwc = pmdtop = (long *)PMD_ADDR;

	pid   = *pmdwc;			pmdwc++;
	ptop  = pmdtop + ((*pmdwc)>>2);	pmdwc++;
	svtop = pmdtop + ((*pmdwc)>>2);	pmdwc++;
	nobj  = *pmdwc;			pmdwc++;

	for(i = 0;i < nobj; i++) {
		nptr = *pmdwc;
		pmdwc++;
		for(j = 0;j < nptr; j++){
			pointer      = *pmdwc;
			primtop      = pmdtop+(pointer>>2);
			type_npacket = *(primtop);	
			type         = type_npacket>>16;
			ltype        = type&0x000f;
			backc        = (type&0x0020)>>5;
			
			/* independent vertex type */
			if (ltype < 0x8)
				(*PMD_func[type])(primtop,
						     ot, OTLENGTH, id, 0,0,0,1);
				/*backc);*/	
			/* shared vertex type */
			else if (ltype < 0x10)
				(*PMD_SV_func[ltype&0x07])(primtop, svtop,
						     ot, OTLENGTH, id, 0,0,0,1);
				/*backc);*/
			pmdwc++;
		}
	}
}







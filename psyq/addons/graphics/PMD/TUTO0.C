/* $PSLibId: Runtime Library Release 3.6$ */
/*			    tuto0
 *			
 *		PMD viewer (using function table)		
 :	        PMD ビューアプロトタイプ (関数テーブル型)
 *
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	----------------------------------------------------	
 *	1.00		Jul, 7,1994	oka
 *	2.00		Jul,21,1994	suzu	
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>	

#define TEX_ADDR	0x801c0000	/* texture address */
#define PMD_ADDR	0x801a0000	/* PMD object address */
#define OTLENGTH	8		/* OT depth */
#define OTSIZE		(1<<OTLENGTH)
#define SCR_Z		1024

static int pad_read(void);
static void RotPMD(u_long *ot, int id);
static void loadTIM(u_long *addr);

main()
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
	PadStop();				/* quit */
	StopCallback();
	return(0);
}

/*
 * update world-screen matrix according to input of controller
 : GTE マトリクスを設定する
 */
static int pad_read(void)
{
	static SVECTOR	ang   = {512, 512, 512};	/* rotate angle */
	static VECTOR	vec   = {0,     0, SCR_Z*5};	
	static MATRIX	m;				/* matrix */
	static int	scale = ONE;
	
	VECTOR	svec;
	int 	padd = PadRead(1);
	
	if (padd & PADk)	return(-1);
	if (padd & PADRup)	ang.vx += 32;
	if (padd & PADRdown)	ang.vx -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;
	if (padd & PADL1)	vec.vz += 32;
	if (padd & PADL2) 	vec.vz -= 32;
	if (padd & PADR1)	scale  -= ONE/256;
	if (padd & PADR2)	scale  += ONE/256;

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
 * Load TIM data from main memory to the frame buffer
 : TIM をロードする
 */	
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
 *
 : PMD 描画関数
 *	プリミティブの型と共有頂点／独立頂点に応じて１６の関数が
 *	ある。
 */	
	
/* independent vertex type (8 type) */
static void (*PMD_func[])() = {
	RotPMD_FT3,	RotPMD_FT4,	RotPMD_GT3,	RotPMD_GT4,
	RotPMD_F3,	RotPMD_F4,	RotPMD_G3,	RotPMD_G4,
};

/* shared vertex type (8 type) */
static void (*PMD_SV_func[])() = {
	RotPMD_SV_FT3,	RotPMD_SV_FT4,	RotPMD_SV_GT3,	RotPMD_SV_GT4,
	RotPMD_SV_F3,	RotPMD_SV_F4,	RotPMD_SV_G3,	RotPMD_SV_G4,
};

/*
 * draw PMD 
 : PMD を描画する
 */	
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
						     ot, OTLENGTH, id, backc);
			/* shared vertex type */
			else if (ltype < 0x10)
				(*PMD_SV_func[ltype&0x0f])(primtop, svtop,
						     ot, OTLENGTH, id, backc);
			pmdwc++;
		}
	}
}







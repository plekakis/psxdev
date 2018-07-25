
#ifndef	__MAIN_H_
#define	__MAIN_H_

#define SCR_Z 					512
#define OTSIZE 					256
#define OBJ_MAX_POLY				100
#define TEXADDR					((u_long *)0x80100000)
#define TEXADDR2				((u_long *)0x80105000)
#define TEXADDR3				((u_long *)0x80110000)
#define SHIP_MODEL_ADDRESS			((u_long *)0x80130000)
#define DCACHE					0x1f800000
#define DCACHETOP				(DCACHE + 0x400)	/* 1k above base */



// Screen position and dimensions. 
#define	FRAME_X			512

#ifdef NTSC

#define	FRAME_Y			240
#define SCREEN_X		0
#define SCREEN_Y		0	

#else

#define	FRAME_Y			256
#define SCREEN_X		0
#define SCREEN_Y		18	

#endif // NTSC

typedef struct  {
    SVECTOR		poly_norm;
    SVECTOR		v0, v1, v2;	/* vertex */
    int			ni0, ni1, ni2;	/* index into norm array */
} VERT_F3;

typedef struct	{
	int 		n;
	SVECTOR		normal[OBJ_MAX_POLY];
	SVECTOR		rot_norm[OBJ_MAX_POLY];
} object_norms;

typedef struct  {
    int			n;
    VERT_F3		vert[OBJ_MAX_POLY];
    POLY_GT3		poly[OBJ_MAX_POLY];
    object_norms	ship_norms;
    int			x_offset, y_offset;
} OBJ_GT3;


typedef struct {
    short		change0, change1, change2, change3;
} RGB_STRUCT;

typedef struct {
    u_long 		*addr;		//	texture addr in mem
    int		texture_x;	//	x location in tpage
    int		texture_y;	//	y loaction in tpage
    int		clut_x;		//	clut x location
    int		clut_y;		//	clut y location
    int		tpage;		// 	tpage no. from 0-15, 16-31
    int		semi_trans;	//	0 = solid, 1 = semi_trans
} TEXTURE_INFO;

typedef struct  {
    u_long		world_ot[OTSIZE];
    u_long		ship2_ot[OTSIZE];
    u_long		ship3_ot[OTSIZE];
    OBJ_GT3		spaceship;
    OBJ_GT3		spaceship2;
    OBJ_GT3		spaceship3;
    POLY_G4		background;
    RGB_STRUCT		background_alt;
    DRAWENV		draw;
    DISPENV		disp;
} DB;

/* GLOBAL DEFINATIONS */

/* double buffer and pointer to current buffer */

extern	DB	db[2];
extern	DB	*cdb;
extern	unsigned long	_ramsize;
extern	unsigned long	_stacksize;

#endif

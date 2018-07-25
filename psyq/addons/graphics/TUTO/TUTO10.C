/* $PSLibId: Runtime Library Release 3.6$ */
/*		   tuto10: 3 dimentional cell type BG
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul,29,1994	suzu 
 *		2.00		May,22,1995	sachiko
 *
 *			3D cell-typed-BG
 :		      �R�c�Z���^�C�v�� BG �̎���
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* unit of BG cell is 32x32: BG �Z���̒P�ʂ� 32x32 */
#define BG_CELLX	32			
#define BG_CELLY	32

/* max width and heght of BG: BG �̍ő啝 */
#define BG_WIDTH	1024			
#define BG_HEIGHT	512			
	
/* number of cell: �Z���̌� */
#define BG_NX		(BG_WIDTH/BG_CELLX)	
#define BG_NY		(BG_HEIGHT/BG_CELLY)	

/* The depth of OT is4 : �n�s�̕���\�� 4 */
#define OTSIZE		4			

/* screen size is 640x240: �X�N���[���T�C�Y(640x240) */
#define SCR_W		640	
#define SCR_H		240

/* depth of screen: �X�N���[���̐[�� */
#define SCR_Z		512			

/*
 * Define structure to deal BG
 : BG �\���̂�V������`����
 */
typedef struct {
	SVECTOR		*ang;		/* rotation: ��]�p */
	VECTOR		*vec;		/* translation: �ړ��� */
	SVECTOR		*ofs;		/* offset on map: �}�b�v��I�t�Z�b�g */
	POLY_GT4	cell[BG_NY*BG_NX];	/* BG cells: BG �Z���z�� */
} BG;

/*
 * Define structure to deal BG
 : �p�P�b�g�_�u���o�b�t�@
 */
typedef struct {
	DRAWENV		draw;		/* drawing environment: �`��� */
	DISPENV		disp;		/* display environment:�\���� */
	u_long		ot[OTSIZE];	/* OT: �n�s */
	BG		bg0;		/* BG 0 */
} DB;

static void init_prim(DB *db,
		      int dr_x, int dr_y, int ds_x, int ds_y, int w, int h);
static void bg_init(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs);
static void bg_update(u_long *ot, BG *bg);
static int pad_read(BG *bg);

main()
{
	/* double buffer: �_�u���o�b�t�@ */
	DB		db[2];		
	
	/* current buffer: ���݂̃o�b�t�@�A�h���X */
	DB		*cdb;		

	/* initialize GTE: GTE �̏����� */
	init_system(SCR_W/2, SCR_H/2, SCR_Z);
	
	/* Set initial value of packet buffers.
	 * and make links of primitive list for BG.
	 : �p�P�b�g�o�b�t�@�̓��e�̏����l��ݒ� 			
	 * �����ŁABG �p�̃v���~�e�B�u���X�g�̃����N�܂Œ����Ă��܂�
	 */
	init_prim(&db[0], 0,     0, 0, SCR_H, SCR_W, SCR_H);
	init_prim(&db[1], 0, SCR_H, 0,     0, SCR_W, SCR_H);

	/* enable to screen: �\���J�n */
	SetDispMask(1);

	/* main loop: ���C�����[�v */
	cdb = db;
	while (pad_read(&cdb->bg0) == 0) {

		/* exchange primitive buffer: �p�P�b�g�o�b�t�@�̌��� */
		cdb = (cdb==db)? db+1: db;	
		
		/* clear OT:  OT �̃N���A */
		ClearOTag(cdb->ot, OTSIZE);	

		bg_update(cdb->ot, &cdb->bg0);

		DrawSync(0);	/* wait for end of drawing: �`��̏I����҂� */
		VSync(0);	/* wait for V-BLNK: ���������̔�����҂� */
		
		/* swap double buffer draw: �_�u���o�b�t�@���� */
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);
		DrawOTag(cdb->ot);	
	}
	/* quit: �I�� */
        PadStop();
	ResetGraph(1);
	StopCallback();
	return;
}

/*
 * initialize primitive double buffers 
 * Parameters which would not be changed any more must be set here.
 :
 * �p�P�b�g�_�u���o�b�t�@�̊e�����o�̏�����
 * ���C�����[�v���ŕύX����Ȃ����̂͂��ׂĂ����ł��炩���ߐݒ肵�Ă����B
 */
/* DB	*db,		primitive buffer: �p�P�b�g�o�b�t�@  */
/* int	dr_x, dr_y	drawing area location: �`����̍��� �wY */
/* int	ds_x, ds_y	display area location: �\�����̍��� �wY */
/* int	w,h		drawing/display  area: �`��E�\���̈�̕��ƍ��� */
static void init_prim(DB *db,
		      int dr_x, int dr_y, int ds_x, int ds_y, int w, int h)
{
	/* Buffer BG location 
	 * GTE treat angles like follows: 360��= 4096 (ONE) 
	 : BG �̈ʒu�o�b�t�@ 
	 * GTE �ł́A�p�x�� 360��= 4096 (ONE) �Ƃ��܂��B*/
	static SVECTOR	ang = {-ONE/5, 0,       0};
	static VECTOR	vec = {0,      SCR_H/2, SCR_Z/2};
	static SVECTOR	ofs = {0,      0,       0};

	/* set double buffer: �_�u���o�b�t�@�̐ݒ� */
	SetDefDrawEnv(&db->draw, dr_x, dr_y, w, h);
	SetDefDispEnv(&db->disp, ds_x, ds_y, w, h);

	/* set auto clear mode for background
	   : �o�b�N�O���E���h�����N���A�̐ݒ� */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 0, 0, 0);

	/* initialize for BG: BG �̏����� */
	bg_init(&db->bg0, &ang, &vec, &ofs);
}

/***************************************************************************
 * 
 *			BG management
 :			BG �n���h�����O
 *
 ***************************************************************************/
/*
 * BG cell structure: BG �Z���\����
 */
typedef struct {
	u_char	u, v;	/* cell texture UV: �Z���e�N�X�`���p�^�[�����W */
	u_short	clut;	/* cell texture CLUT: �Z���e�N�X�`���p�^�[�� CLUT */
	u_long	attr;	/* attribute: �A�g���r���[�g */
} CTYPE;

/*
 * BG mesh structure: BG Mesh �\���́i�����̃��[�N�p�j
 */
/* 2D array for cell type: �Z���^�C�v���L�q�����Q�����z�� */
#include "bgmap.h"	
/* BG texrure pattern/CLUT: BG �e�N�X�`���摜�p�^�[���E�e�N�X�`�� CLUT */
extern	u_long	bgtex[];

/* initialize BG */
/* BG		*bg,	BG data: BG �f�[�^ */
/* int		x,y	location on screen: �X�N���[����̕\���ʒu�i�w�j */
/* VECTOR	*vec	translation vector: ���s�ړ��x�N�g�� */
/* SVECTOR	*ofs	map offset: �I�t�Z�b�g */

static void bg_init(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs)
{
	POLY_GT4	*cell;
	u_short		tpage, clut;
	int		i, x, y, ix, iy;
	u_char		col;

	/* set location data: �ʒu�o�b�t�@��ݒ� */
	bg->ang = ang;
	bg->vec = vec;
	bg->ofs = ofs;

	/* load texture and CLUT: �e�N�X�`���E�e�N�X�`�� CLUT �����[�h */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 481);

	/* getnerate primitive list */
	for (cell = bg->cell, iy = 0; iy < BG_NY; iy++) {
		for (ix = 0; ix < BG_NX; ix++, cell++) {

			/* define POLY_GT4: POLY_GT4 �v���~�e�B�u */
			SetPolyGT4(cell);	

			/* change luminace according to Z value
			   : �����Â��A��O�𖾂邭���邽�߂ɐF��ݒ肷�� */
			/* far side: ���� */
			col = 224*iy/BG_NY+16;		
			setRGB0(cell, col, col, col);
			setRGB1(cell, col, col, col);

			/* near side: ��O�� */
			col = 224*(iy+1)/BG_NY+16;	
			setRGB2(cell, col, col, col);
			setRGB3(cell, col, col, col);

			/* set tpage: �e�N�X�`�� tpage ��ݒ� */
			cell->tpage = tpage;	
			
			/* set CLUT: �e�N�X�`�� CLUT ��ݒ� */
			cell->clut  = clut;	
		}
	}
	
}

/*
 * Update BG members
 : BG �̃����o���X�V����
 */	
/* u_long	*ot,	OT: �I�[�_�����O�e�[�u�� */
/* BG		*bg	BG buffer: BG �o�b�t�@ */
static void bg_update(u_long *ot, BG *bg)
{
	static SVECTOR	Mesh[BG_NY+1][BG_NX+1];

	MATRIX		m;
	POLY_GT4	*cell;
	CTYPE		*ctype;		/* cell type */
	SVECTOR		mp;
	
	/* current absolute position: ���݈ʒu�i���[���h���W�j */
	int		tx, ty;		
	
	/* current left upper corner of map: ���݈ʒu�i�}�b�v�̋��j */
	int		mx, my;		
	
	/* current relative position: ���݈ʒu�i�}�b�v�̋����j */
	int		dx, dy;		
	
	int		ix, iy;		/* work */
	int		xx, yy;		/* work */
	long		dmy, flg;	/* work */

	/* current postion of left upper corner of BG
	   : ���݈ʒu�i BG �̍���j */
	
	/* Lap-round at  BG_CELLX*BG_MAPX , BG_CELLY*BG_MAPY   
	   : ( BG_CELLX*BG_MAPX , BG_CELLY*BG_MAPY ) �Ń��b�v���E���h */
	tx = (bg->ofs->vx)&(BG_CELLX*BG_MAPX-1);
	ty = (bg->ofs->vy)&(BG_CELLY*BG_MAPY-1);

	/*: tx �� BG_CELLX �Ŋ������l���}�b�v�̈ʒu (mx)*/
	/*: tx �� BG_CELLX �Ŋ������]�肪�\���ړ��� (dx)*/
	mx =  tx/BG_CELLX;	my =  ty/BG_CELLY;
	dx = -(tx%BG_CELLX);	dy = -(ty%BG_CELLY);

	PushMatrix();

	/* calculate matrix: �}�g���N�X�̌v�Z */
	RotMatrix(bg->ang, &m);		/*: ��]�p�x */
	TransMatrix(&m, bg->vec);	/*: ���s�ړ��x�N�g�� */
	
	/* set matrix: �}�g���N�X�̐ݒ� */
	SetRotMatrix(&m);		/*: ��]�p�x */
	SetTransMatrix(&m);		/*: ���s�ړ��x�N�g�� */

	mp.vy = -BG_HEIGHT + dy;
	mp.vz = 0;

	/* generate mesh: ���b�V���𐶐� */
	for (iy = 0; iy < BG_NY+1; iy++, mp.vy += BG_CELLY) {
		mp.vx = -BG_WIDTH/2 + dx; 
		for (ix = 0; ix < BG_NX+1; ix++, mp.vx += BG_CELLX) 
			RotTransPers(&mp, (long *)&Mesh[iy][ix], &dmy, &flg);
	}

	/* Update (u0,v0), (x0,y0) members 
	   : �v���~�e�B�u���X�g�� (u0,v0), (x0,y0) �����o��ύX */
	for (cell = bg->cell, iy = 0; iy < BG_NY; iy++) {
		for (ix = 0; ix < BG_NX; ix++, cell++) {

			/* check if mesh is in display area or not 
			   : �\���̈�����ǂ����̃`�F�b�N */
			if (Mesh[iy  ][ix+1].vx <     0) continue;
			if (Mesh[iy  ][ix  ].vx > SCR_W) continue;
			if (Mesh[iy+1][ix  ].vy <     0) continue;
			if (Mesh[iy  ][ix  ].vy > SCR_H) continue;

			/* (BG_MAPX, BG_MAPY) �Ń��b�v���E���h */
			xx = (mx+ix)&(BG_MAPX-1);
			yy = (my+iy)&(BG_MAPY-1);

			/* get cell type from map database
			   : �}�b�v����Z���^�C�v���l�� */

			/* notice that Map[][] has a ASCII type ID code.
			   : Map[][] �̓L�����N�^�R�[�h�� ID �������Ă��� */
			ctype = &CType[(Map[yy])[xx]-'0'];

			/* updatea (u,v),(x,y): (u,v), (x, y) ���X�V */
			setUVWH(cell, ctype->u, ctype->v,
				BG_CELLX-1, BG_CELLY-1);

			setXY4(cell,
			       Mesh[iy  ][ix  ].vx, Mesh[iy  ][ix  ].vy,
			       Mesh[iy  ][ix+1].vx, Mesh[iy  ][ix+1].vy,
			       Mesh[iy+1][ix  ].vx, Mesh[iy+1][ix  ].vy,
			       Mesh[iy+1][ix+1].vx, Mesh[iy+1][ix+1].vy);

			/* add to OT: �n�s�ɓo�^ */
			AddPrim(ot, cell);
		}
	}
	/* pop matrix: �}�g���N�X�̕��A */
	PopMatrix();
}

/*
 * read controller: �R���g���[���p�b�h�̃f�[�^��ǂށB
 */
#define DT	8	/* speed */
static int pad_read(BG *bg)
{
	u_long	padd = PadRead(1);

	/* quit: �v���O�����̏I�� */
	if(padd & PADselect) 	return(-1);

	bg->ofs->vy -= 4;

	/* translate: ���s�ړ� */
	if(padd & PADLup)	bg->ofs->vy -= 2;
	if(padd & PADLdown)	bg->ofs->vy += 2;
	if(padd & PADLleft)	bg->ofs->vx -= 2;
	if(padd & PADLright)	bg->ofs->vx += 2;

	/* rotate: ��] */
	if (padd & PADRup)	bg->ang->vx += DT;
	if (padd & PADRdown)	bg->ang->vx -= DT;
	if (padd & PADRleft) 	bg->ang->vy += DT;
	if (padd & PADRright)	bg->ang->vy -= DT;

	return(0);
}

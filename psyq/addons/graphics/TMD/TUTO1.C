/* $PSLibId: Runtime Library Release 3.6$ */
/*			    tuto1
 *			
 *	   TMD viewer prototype (without shading, with texture)
 :	   TMD �r���[�A�v���g�^�C�v�i�����v�Z�Ȃ�  FT3 �^�j
 *
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Mar,15,1994	suzu
 *	1.10		Jan,22,1996	suzu	(English comment)
 */

#include <sys/types.h>	
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include "rtp.h"

/*
 * Vertex database of triangle primitive 
 : �R�p�`�|���S���̒��_���
 */
typedef struct {
	SVECTOR	n0;		/* nromal: �@�� */
	SVECTOR	v0, v1, v2;	/* vertex: ���_ */
} VERT_F3;

/*
 * Triangle primitive buffer. This buffer should be allocaed dynamically
 * according to total primitive size by malloc()
 : �R�p�`�|���S���p�P�b�g
 * ���_�E�p�P�b�g�o�b�t�@�́Amalloc() �œ��I�Ɋ���t����ׂ��ł��B	
 */
#define MAX_POLY	400			/* max polygon (per object) */

/* object definition : �I�u�W�F�N�g�̒�` */
typedef struct {
	MATRIX		m;	/* local-local matrix: ���[�J���}�g���N�X */
	MATRIX		lw;	/* local-world matrix: ���[�J���}�g���N�X */
	int		n;	/* number of polygons: �|���S���� */
	VERT_F3		vert[MAX_POLY];		/* vertex */
	POLY_FT3	prim[2][MAX_POLY];	/* primitives (2 set) */
} OBJ_FT3;

#define SCR_Z		1024	/* projection: ���e�ʂ܂ł̋��� */
#define OTSIZE		4096	/* depth of OT: �n�s�̒i���i�Q�ׂ̂���j */
#define MAX_OBJ		9	/* max object: �I�u�W�F�N�g�̌� */

#define MODELADDR	((u_long *)0x80120000)	/* TMD address */
#define TEXADDR		((u_long *)0x80140000)	/* TIM address */

static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m);
static void set_position(OBJ_FT3 *obj, int n);
static void add_OBJ_FT3(MATRIX *ws, u_long *ot, OBJ_FT3 *obj, int id);
static void loadTIM(u_long *addr);
static void setSTP(u_long *col, int n);
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj);

main()
{
	static SVECTOR	ang = {0,0,0};	/* self-rotation angle; ���]�p */
	MATRIX	rot;			/* self-rotation angle: ���] */
	MATRIX	ws;			/* word-screen matrix */
	OBJ_FT3	obj[MAX_OBJ];		/* object:�I�u�W�F�N�g */
	u_long	ot[2][OTSIZE];		/* OT: �n�s �o�b�t�@*/
	int	nobj = 1;		/* number of obj: �I�u�W�F�N�g�̌� */
	int	id   = 0;		/* buffer ID: �p�P�b�g ID */
	int	i, n; 			/* work */
	
	/* initialize frame double buffer: �_�u���o�b�t�@�̏����� */
	db_init(640, 480/*240*/, SCR_Z, 60, 120, 120);	
	
	/* load TIM to frame buffer: TIM ���t���[���o�b�t�@�ɓ]������ */
	loadTIM(TEXADDR);	

	/* read each TMD: TMD ���΂�΂�ɂ��ēǂݍ��� */
	for (i = 0; i < MAX_OBJ; i++) 
		loadTMD_FT3(MODELADDR, &obj[i]);	
	
	/* layout each object: �I�u�W�F�N�g�ʒu�����C�A�E�g */
	set_position(obj, 0);		

	/* start display;�\���J�n */
	SetDispMask(1);			
	
	/* main loop: ���C�����[�v */
	while ((nobj = pad_read(obj, nobj, &ws)) != -1) {
		
		/* swap primitive buffer ID:�p�P�b�g�o�b�t�@ ID���X���b�v */
		id = id? 0: 1;
		
		/* clear OT:�n�s���N���A */
		ClearOTagR(ot[id], OTSIZE);			

		/* rotate matrix of the earth: �n�������]������ */
		ang.vy += 32;
		RotMatrix(&ang, &rot);
		
		/* set primitive vertex: �v���~�e�B�u�̐ݒ� */
		for (n = i = 0; i < nobj; i++) {
			MulMatrix0(&obj[i].m, &rot, &obj[i].lw);
			add_OBJ_FT3(&ws, ot[id], &obj[i], id);
		}
		
		/* print debug information: �f�o�b�O�X�g�����O�̐ݒ� */
		FntPrint("objects=%d\n", nobj);
		FntPrint("total  =%d\n", obj[0].n*nobj);
		
		/* draw OT and swap frame double buffer
		 : �_�u���o�b�t�@�̃X���b�v�Ƃn�s�`��
		 */
		db_swap(&ot[id][OTSIZE-1]);
	}
	StopCallback();
	PadStop();
}


/*
 * Read controler and set world-screen matrix
 : �p�b�h�̓ǂݍ���
 * ��]�̒��S�ʒu�Ɖ�]�p�x��ǂݍ��݃��[���h�X�N���[���}�g���N�X��ݒ�
 * 	
 */	
static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m)
{
	static SVECTOR	ang   = {0, 0, 0};
	static VECTOR	vec   = {0, 0, SCR_Z*3};
	static int	scale = ONE;
	static int	opadd = 0;
	
	VECTOR	svec;
	int 	padd = PadRead(1);
	
	if (padd & PADk)	return(-1);
	if (padd & PADRup)	ang.vx += 8;
	if (padd & PADRdown)	ang.vx -= 8;
	if (padd & PADRleft) 	ang.vy += 8;
	if (padd & PADRright)	ang.vy -= 8;
	if (padd & PADL1)	vec.vz += 8;
	if (padd & PADL2) 	vec.vz -= 8;
	
	if ((opadd==0) && (padd & PADLup))	set_position(obj, nobj++);
	if ((opadd==0) && (padd & PADLdown))	nobj--;
	
	limitRange(nobj, 1, MAX_OBJ);
	opadd = padd;
	
	setVector(&svec, scale, scale, scale);
	RotMatrix(&ang, m);	
	
#ifdef ROTATE_YORSELF	/* rotate your camera: ��ώ��i����������) */
	{
		VECTOR	vec2;
		ApplyMatrixLV(m, &vec, &vec2);
		TransMatrix(m, &vec2);
		dumpVector("vec2=", &vec2);
	}
#else			/* rotate the world: �q�ώ��i���E������) */
	TransMatrix(m, &vec);	
#endif
	SetRotMatrix(m);
	SetTransMatrix(m);

	return(nobj);
}

/*
 * layout many object in the world-coordinate.
 * Since the position of each object is determined by rand(),
 * two objects may be located at the same position.
 : �I�u�W�F�N�g�����[���h���W�n�Ƀ��C�A�E�g
 * �����ł͗������g�p���ēK���ɔz�u���Ă���	
 * ���̂��߁A��̃I�u�W�F�N�g�������ʒu�ɂԂ��邱�Ƃ�����B	
 */	
#define UNIT	512		/* resolution: �ŏ��𑜓x */

static void set_position(OBJ_FT3 *obj, int n)
{
	SVECTOR	ang;

	static loc_tab[][3] = {
		 0, 0, 0,
		 1, 0, 0,	0, 1, 0,	 0, 0, 1,
		-1, 0, 0,	0,-1, 0,	 0, 0,-1,
		 1, 1, 0,	0, 1, 1,	 1, 0, 1,
		-1,-1, 0,	0,-1,-1,	-1, 0,-1,
		 1,-1, 0,	0,-1, 1,	-1, 0, 1,
		-1, 1, 0,	0, 1,-1,	 1, 0,-1,
	};
	
	/* set axis of each earth: �e�n���̒n����ݒ� */
	ang.vx = rand()%4096;
	ang.vy = rand()%4096;
	ang.vz = rand()%4096;
	RotMatrix(&ang, &obj[n].m);	
	
	/* set position of each earth: �e�n���̏ꏊ��ݒ� */
	obj[n].lw.t[0] = loc_tab[n][0]*UNIT;
	obj[n].lw.t[1] = loc_tab[n][1]*UNIT;
	obj[n].lw.t[2] = loc_tab[n][2]*UNIT;
}

/*
 * Add new objects to OT
 : �I�u�W�F�N�g�̓o�^
 */
static void add_OBJ_FT3(MATRIX *ws, u_long *ot, OBJ_FT3 *obj, int id)
{
	MATRIX		ls;		/* local-screen matrix */
	VERT_F3		*vp;		/* work */
	POLY_FT3	*pp;		/* work */
	int		i;
	long		flg;
	
	vp = obj->vert;
	pp = obj->prim[id];
		
	/* push current matrix: �J�����g�}�g���N�X��ޔ� */
	PushMatrix();				

	/* make local-screen coordinate
	 :  ���[�J���X�N���[���}�g���N�X�����
	 */
	CompMatrix(ws, &obj->lw, &ls);

	/* set matrix: ���[�J���X�N���[���}�g���N�X��ݒ� */
	SetRotMatrix(&ls);		/* set matrix */
	SetTransMatrix(&ls);		/* set vector */
	
	/* rotTransPers3 is macro. see rtp.h
	 : rotTransPers3 �̓}�N���Brtp.h ���Q�Ƃ̂���
	 */
	for (i = 0; i < obj->n; i++, vp++, pp++) {
		rotTransPers3(ot, OTSIZE, pp,
			      &vp->v0, &vp->v1, &vp->v2);
	}
	
	/* pop old matrix: �}�g���N�X�����ɂ��ǂ��ă��^�[�� */
	PopMatrix();
}


/*
 * Load TIM data from main memory to the frame buffer
 : TIM �����[�h����
 */	
static void loadTIM(u_long *addr)
{
	TIM_IMAGE	image;		/* TIM header */
	
	OpenTIM(addr);			/* open TIM */
	while (ReadTIM(&image)) {
		if (image.caddr) {	/* load CLUT (if needed) */
			setSTP(image.caddr, image.crect->w);
			LoadImage(image.crect, image.caddr);
		}
		if (image.paddr) 	/* load texture pattern */
			LoadImage(image.prect, image.paddr);
	}
}
	
/*
 * change STP on texture pattern to inhibit transparent color
 : �����F�������֎~���邽�߂� STP bit �� 1 �ɂ���
 */	
static void setSTP(u_long *col, int n)
{
	n /= 2;  
	while (n--) 
		*col++ |= 0x80008000;
}

/*
 * parse TMD: TMD �̉��
 */
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj)
{
	VERT_F3		*vert;
	POLY_FT3	*prim0, *prim1;
	TMD_PRIM	tmdprim;
	int		col, i, n_prim = 0;

	vert  = obj->vert;
	prim0 = obj->prim[0];
	prim1 = obj->prim[1];
	
	/* open TMD: TMD �̃I�[�v�� */
	if ((n_prim = OpenTMD(tmd, 0)) > MAX_POLY) 
		n_prim = MAX_POLY;
	
	/*
	 * Set unchanged member of primitive here to deliminate main
	 * memory write access
	 : ���������C�g�A�N�Z�X�����炷���߂Ƀv���~�e�B�u�o�b�t�@�̂����A
	 * ���������Ȃ����̂����炩���ߐݒ肵�Ă���
	 */	 
	for (i = 0; i < n_prim && ReadTMD(&tmdprim) != 0; i++) {

		/* initialize primitive: �v���~�e�B�u�̏����� */
		SetPolyFT3(prim0);

		/* copy normal and vertex: �@���ƒ��_�x�N�g�����R�s�[���� */
		copyVector(&vert->n0, &tmdprim.n0);
		copyVector(&vert->v0, &tmdprim.x0);
		copyVector(&vert->v1, &tmdprim.x1);
		copyVector(&vert->v2, &tmdprim.x2);
		
		/* lighting: �����v�Z�i�ŏ��̈��̂݁j*/
		col = (tmdprim.n0.vx+tmdprim.n0.vy)*128/ONE/2+128;
		setRGB0(prim0, col, col, col);
		
		/* copy texture point because this point never changes
		 * by rotation
		 : �e�N�X�`�����W�͕ς��Ȃ��̂ł����ŃR�s�[���Ă���
		 */
		setUV3(prim0,
		       tmdprim.u0, tmdprim.v0,
		       tmdprim.u1, tmdprim.v1,
		       tmdprim.u2, tmdprim.v2);
		
		/* copy tpage and clut
		 : �e�N�X�`���y�[�W�^�e�N�X�`�� CLUT ID ���R�s�[
		 */
		prim0->tpage = tmdprim.tpage;
		prim0->clut  = tmdprim.clut;

		/* duplicate primitive for primitive double buffering
		 : �_�u���o�b�t�@���g�p����̂Ńv���~�e�B�u�̕���������
		 * ��g�����Ă���
		 */  
		memcpy(prim1, prim0, sizeof(POLY_FT3));
		
		vert++, prim0++, prim1++;
	}
	return(obj->n = i);
}
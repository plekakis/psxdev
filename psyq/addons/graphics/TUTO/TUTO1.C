/* $PSLibId: Runtime Library Release 3.6$ */
/*				tuto1: OT
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *			  simple sample to use OT
 :			�n�s���g�p�����`��̃e�X�g
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Primitive double buffer.
 * Primitive buffer should be 2 buffers for CPU and GPU.
 : �v���~�e�B�u�_�u���o�b�t�@
 * �`��� CPU �����Ŏ��s�����邽�߂ɂ͂Q�g�̃o�b�t�@���K�v
 */	
typedef struct {		
	DRAWENV		draw;		/* Drawing Environment : �`��� */
	DISPENV		disp;		/* Display Environment	: �\���� */
	u_long		ot[9];		/* Ordering Table : �n�s */
	SPRT_16		ball[8][8][8];	/* sprite buffer : �X�v���C�g */
					/* Z-direction	: ���s������: �W�� */
					/* X-direction	: ������:     �W�� */
					/* Y-direction	: �c����:     �W�� */
} DB;

static void init_prim(DB *db);
static int pad_read(int *dx, int *dy);

main()
{
	DB	db[2];		/* primitive double buffer
				   : �v���~�e�B�u�_�u���o�b�t�@ */
	DB	*cdb;		/* current primitive buffer pointer
				   : �ݒ�p�P�b�g�o�b�t�@�փ|�C���^ */
	SPRT_16	*bp;		/* work */
	int	ox, oy;		/* work */
	int	dx = 0, dy = 0;	/* work */
	int	depth;		/* work */
	int	x, y;		/* work */
	int	ix, iy;		/* work */

	/* reset controller: �R���g���[���̃��Z�b�g */
	PadInit(0);		
	
	/* reset graphic subsystem: �`��E�\�����̃��Z�b�g */
	ResetGraph(0);		
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));
	
	/* define frame double buffer :  �_�u���o�b�t�@�̒�` */
	/*	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,240)-(320,480) (320x240)
	 */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* set primitive members of each buffer
	   : ���ꂼ��̃v���~�e�u�o�b�t�@�̓��e�̏����l��ݒ� */
	init_prim(&db[0]);	/* �p�P�b�g�o�b�t�@ # 0 */
	init_prim(&db[1]);	/* �p�P�b�g�o�b�t�@ # 1 */

	/* display enable : �\���J�n */
	SetDispMask(1);

	/* main loop : ���C�����[�v */
	while (pad_read(&dx, &dy) == 0) {

		/* swap db[0], db[1]: �_�u���o�b�t�@�|�C���^�̐؂�ւ� */
		cdb = (cdb==db)? db+1: db;

		/* clear ordering table (8 entry)
		   : �n�s�������� �i�G���g�������W�j*/
		ClearOTag(cdb->ot,8);

		/* calculate postion of sprites : �X�v���C�g�̈ʒu���v�Z */
		for (depth = 0; depth < 8; depth++) {
			/* Far sprites move slowly, and near sprites
			 * move fast.
			 : ���̃X�v���C�g�قǒx���A
			 * �߂��̃X�v���C�g�قǑ��������悤�ɐݒ�
			 */
			/* left upper corner (Y): ������W�l (Y) */
			oy =  56 + dy*(depth+1);	
			
			/* left upeer corner (X): ������W�l (X) */
			ox =  96 + dx*(depth+1);	
 			for (iy = 0, y = oy; iy < 8; iy++, y += 16) 
			for (ix = 0, x = ox; ix < 8; ix++, x += 16) {

				bp = &cdb->ball[depth][iy][ix];

				/* set upper-left corner of sprites
				   : �X�v���C�g�̉E��_���w�� */
				setXY0(bp, x, y);

				/* add primitives to OT
				   ot[depth+1] �ɓo�^ */
				AddPrim(&cdb->ot[depth], bp);
			}
		}
		/* wait for previous 'DrawOTag' 
		   : �o�b�N�O���E���h�ő����Ă���`��̏I����҂� */
		DrawSync(0);

		/* wait for next V-BLNK
		   : V-BLNK ������̂�҂� */
		VSync(0);

		/* swap frame double buffer:
		 *  set the drawing environment and display environment.
		 : �t���[���_�u���o�b�t�@����������
		 * �J�����g�p�P�b�g�o�b�t�@�̕`����E�\������ݒ肷��B
		 */
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);

		/* start Drawing
		   : �n�s��̃v���~�e�B�u���o�b�N�O���E���h�ŕ`�悷��B*/
		DrawOTag(cdb->ot);
		FntPrint("tuto1: OT\n");
		FntFlush(-1);
	}
        PadStop();
	ResetGraph(1);
	StopCallback();
	return;
}

/* Intitalize the members of primitives in each primitive-buffer.
 * All of unchanged parameters are set here.
 : �v���~�e�B�u�_�u���o�b�t�@�̊e�����o�̏�����
 * ���C�����[�v���ŕύX����Ȃ����̂͂��ׂĂ����ł��炩���ߐݒ肵�Ă����B
 */	
/* DB	*db;	�v���~�e�B�u�o�b�t�@ */
static void init_prim(DB *db)
{
	/* 16x16 ball texture pattern: 16x16 �e�N�X�`���p�^�[���i�{�[���j*/
	extern u_long	ball16x16[];	
	
	/* ball CLUT (16 colorsx32):  �{�[�� CLUT (16�Fx32) */
	extern u_long	ballclut[][8];	

	SPRT_16	*bp;
	u_short	clut;
	int	depth, x, y;

	/* Translate 4bit-texture pattern to (640, 0) on the frame-buffer
	 * and set the default texture page.
	 : 4bit ���[�h�̃{�[���̃e�N�X�`���p�^�[�����t���[���o�b�t�@��
	 * �]������B���̎��̃e�N�X�`���y�[�W���f�t�H���g�e�N�X�`���y�[�W��
	 * �ݒ肷��B	 
	 */	 
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);

	/* background clear enable : �����w�i�N���A���[�h�ɂ��� */
	/* �����w�i�N���A���[�h�ɂ��� */
	db->draw.isbg = 1;
	/* : �w�i�F�̐ݒ� */
	setRGB0(&db->draw, 60, 120, 120);	

	/* Initialize 8x8x8=256 sprites at this point.
	 * CLUT of each texture is changed accroding to the sprites' depth.
	 : 8x8x8=256 �̃X�v���C�g���܂Ƃ߂ď���������B
	 * CLUT �́A���s���ɉ����Đؑւ���B	 
	 */	 
	for (depth = 0; depth < 8; depth++) {
		/* load CLUT on frame buffer (notice the CLUT address)
		   : CLUT �����[�h CLUT ���[�h�A�h���X�ɒ��� */
		clut = LoadClut(ballclut[depth], 0, 480+depth);

		/* Unchanged members of primitives are set here 
		   : �v���~�e�B�u�̃����o�ŕω����Ȃ����̂������Őݒ肷��B*/
		for (y = 0; y < 8; y++) 
			for (x = 0; x < 8; x++) {
				bp = &db->ball[depth][y][x];
			
				/* SPRT_16 primitve: 16x16�̃X�v���C�g */
				SetSprt16(bp);		
			
				/* ShadeTex disable: �V�F�[�f�B���O�֎~ */
				SetShadeTex(bp, 1);	
			
				/* (u0,v0) = (0, 0) */
				setUV0(bp, 0, 0);	
			
				/* set texture CLUT ID: �e�N�X�`��CLUT ID  */
				bp->clut = clut;	
			}
	}
}

/*: �R���g���[���̉��
 */
/* int	*dx;	�X�v���C�g�̍��W�l�̃L�[���[�h�i�w�j */
/* int	*dy;	�X�v���C�g�̍��W�l�̃L�[���[�h�i�x�j */

static int pad_read(int *dx, int *dy)
{
	/* get controller value: �R���g���[������f�[�^��ǂݍ��� */
	u_long	padd = PadRead(1);	

	/* exit program: �v���O�����̏I�� */
	if (padd & PADselect)	return(-1);

	/* move sprite position: �X�v���C�g�̈ړ� */
	if (padd & PADLup)	(*dy)--;
	if (padd & PADLdown)	(*dy)++;
	if (padd & PADLleft)	(*dx)--;
	if (padd & PADLright)	(*dx)++;

	return(0);
}

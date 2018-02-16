/* $PSLibId: Runtime Library Release 3.6$ */
/*			lscr.c: line scroll sample
 *
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------------------	
 *	1.00		95/04/01	suzu
 *
 *			Pseudo line scroll
 *
 *	This is a pseudo line-scroll sample program. This driver does
 *	not use H-Sync interrupt, but uses recursive texture mapping
 *	technique. Sine the mapping source is read from previous line,
 *	the additional 3rd buffer for recursive mapping is not required.
 *
 *	To add this function to your application:
 *	 * Call LscrInit() to notify the double buffer position.
 *	 * Set scroll parameters to the external array 'LscrOffset[240]'
 *	   For example,
 *	
 *			LscrOffset[10] = 20;
 *	
 *	   means 10th line is scrolled 20 dot to right.
 *	
 *	* After calling DrawOTag(), call LscrFlush() to execute
 *	  scrolling.
 *
 *	To scroll only BG plane,
 *	
 *	* separete BG OTs and FG(Sprite) OTs, and call LscrFlush()
 *	  after BG drawing.
 *	
 * 		DrawOtag(bg_ot[id]);		// OT of BG
 * 		LscrFlush(id);
 * 		DrawOtag(fg_ot[id]);		// OT of FG
 *	
 *	  Each drawing request qued and executed in the background.
 *	  
 :			���C���X�N���[���T���v��
 * 
 *	�����ł̃��C���X�N���[���́A�����A���̊��荞�݂��g�p������
 *	���J�[�V�u�}�b�s���O�i�ċA�}�b�s���O�j���g�p���Ď������Ă�
 *	�܂��B�������ċA�}�b�v�p�̃T�[�h�o�b�t�@�͕K�v����܂���B
 *
 *	�����̃v���O�����Ƀ��C���X�N���[���@�\��ǉ�����ɂ́A
 *
 *	�E�ŏ��� LscrInit() ���Ă�Ń_�u���o�b�t�@�̈ʒu��ݒ肷��B
 *	
 *	�E�O���ϐ� int LscrOffset[] �Ƀ��C�����ƃX�N���[���ʂ�ݒ肷��B
 *	  �Ⴆ�΁ALscrOffset[10] = 20; �Ƃ���΁A10 ���C���ڂ� 20 �h�b�g
 *	  �E�ɃX�N���[�����邱�Ƃ��Ӗ����܂��B
 *
 *	�E DrawOTag()/GsDrawOt() �̂��ƂŁALscrFlush() ���ĂԁB���̂�
 *	   �� LscrOffset[] �̒l�ɉ��������C���X�N���[�����s�Ȃ��܂��B
 *
 *	���s�Ȃ��ĉ������B
 *
 *	�܂��ABG(�o�b�N�O���E���h�j �݂̂����C���X�N���[������ꍇ�́A
 *
 *		DrawOtag(bg_ot[id]);		// BG �� OT 
 *		LscrFlush(id);
 *		DrawOtag(fg_ot[id]);		// FG �� OT 
 *
 *	�� BG ��FG (�t�H�A�O���E���h) �� �n�s�𕪊����邱�ƂŎ����ł�
 *	�܂��B
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define MAXW		512	/* maximum size of drawing width
				 : �`��̈�̕��̍ő�l */
#define MAXH		240

int	LscrOffset[MAXH];

typedef struct {		/* Complex primitive: �����v���~�e�B�u */
	DR_MODE	mode;
	SPRT	sprt;
} TSPRT;

struct {
	TILE	bg[MAXH];
	TSPRT	l[MAXH], r[MAXW];
} Lscr[2];

static setBgLine(TILE *bg, int y, int r, int g, int b);
static setTSprt(TSPRT *tsprt, int w, int h, int x, int y, int tx, int ty);

static int	Width, Height;

LscrFlush(int drawbuf)
{
	int	y;
	for (y = 0; y < Height; y++) {
		Lscr[drawbuf].l[y].sprt.x0 = LscrOffset[y];
		Lscr[drawbuf].r[y].sprt.x0 = LscrOffset[y]+256;
	}
	DrawOTag((u_long *)&Lscr[drawbuf].bg[0]);
	
}

LscrInit(int w, int h, int ox0, int oy0, int ox1, int oy1)
{
	int y, i;
	
	if (w > MAXW) {
		printf("LscrInit: size overflow (%d,%d)\n", w, h);
		w = MAXW;
	}
	if (h > MAXH) {
		printf("LscrInit: size overflow (%d,%d)\n", w, h);
		h = MAXH;
	}
	Width  = w;
	Height = h;	
	
	for (y = 0; y < Height; y++) {
		
		/* clear scroll offset: �X�N���[���I�t�Z�b�g������ */
		LscrOffset[y] = 0;
		
		/* initialize background color to black(0,0,0)
		 *  �w�i�F����(0,0,0) �ɏ�����
		 */
		setBgLine(&Lscr[0].bg[y], y, 0, 0, 0);
		setBgLine(&Lscr[1].bg[y], y, 0, 0, 0);
		
		/* initialize scroll sprites
		 :  ���C���X�N���[���p�̃X�v���C�g��������
		 */
		setTSprt(&Lscr[0].l[y], 256,   1,   0, y, ox0,     oy0+y+1);
		setTSprt(&Lscr[0].r[y], w-256, 1, 256, y, ox0+256, oy0+y+1);

		setTSprt(&Lscr[1].l[y], 256,   1,   0, y, ox1,     oy1+y+1);
		setTSprt(&Lscr[1].r[y], w-256, 1, 256, y, ox1+256, oy1+y+1);

		/* make primitive links: �����N�𒣂� ���Ԃ� ���と�E�� */
		for (i = 0; i < 2; i++) {
			CatPrim(&Lscr[i].bg[y], &Lscr[i].l[y]);
			CatPrim(&Lscr[i].l[y], &Lscr[i].r[y]);
			if (y > 0)
				CatPrim(&Lscr[i].r[y-1], &Lscr[i].bg[y]);
			if (y == Height-1)
				TermPrim(&Lscr[i].r[y]);
		}
	}
}

/*
 * initialize background line (black line)
 : �w�i�̒�����������
 */
static setBgLine(TILE *bg, int y, int r, int g, int b)
{
	SetTile(bg);
	setRGB0(bg, r, g, b);
	setXY0(bg, 0, y);
	setWH(bg, Width, 1);
}

/*
 * initialize sprite with tpage using MargePrim()
 : tpage ���X�v���C�g�v���~�e�B�u���������B�����ɂ� MargePrim() ���g�p
 */
static setTSprt(TSPRT *tsprt, int w, int h, int x, int y, int tx, int ty)
{
	
	/* Set primitive mode.
	 * dfe = 1, dtd = 1, tp = 2(16bit), abr=0, tw=0
	 * u, v is used lower 8bit of (tx, ty)
	 * tpage is calculated by higher 8bit of (tx,ty)
	 *
	 : ���[�h��ݒ肷��					
	 * dfe = 1, dtd = 1, tp = 2(16bit), abr=0, tw=0
	 * u, v �́Atx, ty �̉��� 8bit ���g�p
	 * �e�N�X�`���y�[�W�́Atx, ty �� 9bit �ȏ���g�p
	 */
	SetDrawMode(&tsprt->mode, 1, 1,
		    GetTPage(2, 0, tx&~0xff, ty&~0xff), 0);
	
	/* initialize SPRT: �X�v���C�g��ݒ肷�� */
	SetSprt(&tsprt->sprt);
	setRGB0(&tsprt->sprt, 0x80, 0x80, 0x80);
	setUV0(&tsprt->sprt, tx&0xff, ty&0xff);
	setXY0(&tsprt->sprt, x, y);
	setWH(&tsprt->sprt, w, h);

	/* connect SPRT and DR_MODE primitive: �������� */
	if (MargePrim(&tsprt->mode, &tsprt->sprt) != 0) {
		printf("Marge failed\n");
		return(-1);
	}
}
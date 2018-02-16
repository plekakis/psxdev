/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *	rcube for overlay: PlayStation Demonstration program
 *
 *	"rcube.c" Main routine
 *
 *		Version 3.01			Jan, 28, 1994
 *		Version 3.01a	yoshi		Mar, 31, 1995
 *		Version 3.02			Jan, 9, 1995
 *		Version 3.02a	yoshi		Aug, 3, 1995
 *		Version 3.02b	yoshi		Mar, 4, 1996
 *
 *		Copyright (C) 1993,1994,1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *====================================================================
 * This was rewritten as a child process. Compile conditionally with OVERLAY.
 * �q�v���Z�X�p�ɏ����������BOVERLAY�ŏ����R���p�C������B
 *
 */
#include <sys/types.h>
#include <sys/file.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libcd.h>
#include "table.h"
#include "pos.h"

/*
 * texture data : �e�N�X�`�����
 */
#define TIM_ADDR 0x80108000		/* stored address of the TIM file to be used : 
                                   �g�p����TIM�t�@�C���̊i�[�A�h���X */
/*#define TIM_ADDR 0x80020000		/* stored address of the TIM file to be used :
                                       �g�p����TIM�t�@�C���̊i�[�A�h���X */
#define TIM_HEADER 0x00000010

/*
 * modeling data : ���f�����O�f�[�^���
 */
#define TMD_ADDR 0x80100000		/* stored address of the TMD file to be used : 
                                   �g�p����TMD�t�@�C���̊i�[�A�h���X */
/*#define TMD_ADDR 0x80010000		/* stored address of the TMD file to be used :
                                       �g�p����TMD�t�@�C���̊i�[�A�h���X */
u_long *TmdBase;			/* address of object within TMD : 
                               TMD�̂����A�I�u�W�F�N�g���̃A�h���X */
int CurrentTmd; 			/* the TMD number for the TMD being used : �g�p����TMD�ԍ� */

/*
 * ordering table (OT) : �I�[�_�����O�e�[�u�� (OT)
 */
#define OT_LENGTH  7			/* OT resolution (large) : OT�𑜓x�i�傫���j */
GsOT WorldOT[2];			/* OT data (double buffer) : OT���i�_�u���o�b�t�@�j */
GsOT_TAG OTTags[2][1<<OT_LENGTH];	/* OT tag area (double buffer) : 
                                       OT�̃^�O�̈�i�_�u���o�b�t�@�j */

/*
 * GPU packet creation area : GPU�p�P�b�g�����̈�
 */
#define PACKETMAX 1000			/* maximum packet number in 1 frame : 
                                   1�t���[���̍ő�p�P�b�g�� */
PACKET GpuPacketArea[2][PACKETMAX*64];	/* packet area (double buffer) : 
                                           �p�P�b�g�̈�i�_�u���o�b�t�@�j */

/*
 *  object (cube) variable : �I�u�W�F�N�g�i�L���[�u�j�ϐ�
 */
#define NCUBE 44
#define OBJMAX NCUBE
int nobj;				/* number of cubes : �L���[�u�� */
GsDOBJ2 object[OBJMAX];			/* 3D object variable : 3D�I�u�W�F�N�g�ϐ� */
GsCOORDINATE2 objcoord[OBJMAX];		/* local coordinate variable : ���[�J�����W�ϐ� */
SVECTOR Rot[OBJMAX];			/* rotation angle : ��]�p */
SVECTOR RotV[OBJMAX];			/* rotation speed (angular velocity) : 
                                   ��]�X�s�[�h�i�p���x�j */
VECTOR Trns[OBJMAX];			/* cube position (parallel displacement) : 
                                   �L���[�u�ʒu�i���s�ړ��ʁj */
VECTOR TrnsV[OBJMAX];			/* displacement speed : �ړ��X�s�[�h */

/*
 *  VIEW (viewpoint) : ���_�iVIEW�j
 */
GsRVIEW2  View;				/* ���_�ϐ� */
int ViewAngleXZ;			/* ���_�̍��� */
int ViewRadial;				/* ���_����̋��� */
#define DISTANCE 600			/* Radial�̏����l */

/*
 *  light source : ����
 */
GsF_LIGHT pslt[3];			/* light source data variable x 3 : �������ϐ��~3 */

/*
 *  other : ���̑�...
 */
int Bakuhatu;				/* explosion processing flag : ���������t���O */
u_long PadData;				/* controller pad data : �R���g���[���p�b�h�̏�� */
u_long oldpad;				/* controller pad data from previous frame : 
                               �P�t���[���O�̃p�b�h��� */
GsFOGPARAM dq;				/* parameter for depth queue (fog) : 
                               �f�v�X�L���[(�t�H�O)�p�p�����[�^ */
int dqf;				/* check if fog is ON : �t�H�O��ON���ǂ��� */
int back_r, back_g, back_b;		/* background color : �o�b�N�O���E���h�F */
#define FOG_R 160
#define FOG_G 170
#define FOG_B 180

/*
 *  function prototype declaration : �֐��̃v���g�^�C�v�錾
 */
void drawCubes();
int moveCubes();
void initModelingData();
void allocCube();
void initSystem();
void initAll();
void initTexture();
void initView();
void initLight();
void changeFog();
void changeTmd();
int	datafile_search();
int	datafile_read();

/*
 *  file information : �t�@�C�����
 */
typedef struct{
	char	*fname;
	void	*addr;
	CdlFILE finfo;
} FILE_INFO;

#define DFILENO 2

static FILE_INFO dfile[DFILENO] = {
	{ "\\DATA\\RCUBE.TMD;1",(void *)TMD_ADDR,0 },
	{ "\\DATA\\RCUBE.TIM;1",(void *)TIM_ADDR,0 } 
};

/*
 *  main routine : ���C�����[�`��
 */
#ifdef OVERLAY
child_rcube()
#else
main()
#endif
{
	RECT rct;

#ifdef OVERLAY
	/* some tricks are used to allow smooth screen transitions : 
	   ��ʐ؂�ւ����X���[�Y�ɍs���悤�ɏ����H�v���Ă��� */
	VSync(0);
	SetDispMask(0);
	ResetGraph(1);
	setRECT(&rct,0,0,1024,512);
	ClearImage(&rct,0,0,0);
	DrawSync(0);
#else
	ResetCallback();
	CdInit();
	PadInit(0);
#endif

	datafile_search(dfile,DFILENO);
	datafile_read(dfile,DFILENO);

	/* initialize system : �V�X�e���̏����� */
	initSystem();

	/* other intializations : ���̑��̏����� */
	Bakuhatu = 0;
	PadData = 0;
	CurrentTmd = 0;
	dqf = 0;
	back_r = back_g = back_b = 0;
	initView();
	initLight(0, 0xc0);
	initModelingData(TMD_ADDR);
	initTexture(TIM_ADDR);
	allocCube(NCUBE);
	
	/* main loop : ���C�����[�v */
	while(1) {
		if(moveCubes())
			break;
		GsSetRefView2(&View);
		drawCubes();
	}

	return(0);
}


/*
 *  draw 3D object : 3D�I�u�W�F�N�g�i�L���[�u�j�̕`��
 */
void drawCubes()
{
	int i;
	GsDOBJ2 *objp;
	int activeBuff;
	MATRIX LsMtx;

	/* which of the double buffers are active? : 
	   �_�u���o�b�t�@�̂����ǂ��炪�A�N�e�B�u���H */
	activeBuff = GsGetActiveBuff();

	/* set GPU packet creation address to the start of the area : 
	   GPU�p�P�b�g�����A�h���X���G���A�̐擪�ɐݒ� */
	GsSetWorkBase((PACKET*)GpuPacketArea[activeBuff]);

	/* clear OT contents : OT�̓��e���N���A */
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* enter 3D object (cube) into OT : 
	   3D�I�u�W�F�N�g�i�L���[�u�j��OT�ւ̓o�^ */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* rotation angle -> set in matrix : ��]�p->�}�g���N�X�ɃZ�b�g */
		RotMatrix(Rot+i, &(objp->coord2->coord));
		
		/* reset flag since matrix has been updated : 
		   �}�g���N�X���X�V�����̂Ńt���O�����Z�b�g */
                objp->coord2->flg = 0;

		/* translation capacity -> set in matrix : 
		   ���s�ړ���->�}�g���N�X�ɃZ�b�g */
                TransMatrix(&(objp->coord2->coord), &Trns[i]);
		
		/* calculate matrix for perspective transformation and set in GTE : 
		   �����ϊ��̂��߂̃}�g���N�X���v�Z���Ăf�s�d�ɃZ�b�g */
		GsGetLs(objp->coord2, &LsMtx);
		GsSetLsMatrix(&LsMtx);
		GsSetLightMatrix(&LsMtx);

		/* perform perspective transformation and enter in OT : 
		   �����ϊ�����OT�ɓo�^ */
		GsSortObject4(objp, &WorldOT[activeBuff], 14-OT_LENGTH,getScratchAddr(0));
		objp++;
	}

    /* include pad data in buffer : 
	   �p�b�h�̓��e���o�b�t�@�Ɏ�荞�� */
	oldpad = PadData;
	PadData = PadRead(1);
	
	/* wait for V-BLNK : 
	   V-BLNK��҂� */
	VSync(0);
	
	/* forcibly stop drawing operation for previous frame : 
	   �O�̃t���[���̕`���Ƃ������I�� */
	ResetGraph(1);

	/* swap double buffer : �_�u���o�b�t�@����ꊷ���� */
	GsSwapDispBuff();

	/* insert screen clear command at start of OT : 
	   OT�̐擪�ɉ�ʃN���A���߂�}�� */
	GsSortClear(back_r, back_g, back_b, &WorldOT[activeBuff]);

	/* begin drawing OT contents in background : 
	   OT�̓��e���o�b�N�O���E���h�ŕ`��J�n */
	GsDrawOt(&WorldOT[activeBuff]);
}

/*
 *  move cube : �L���[�u�̈ړ�
 */
int moveCubes()
{
	int i;
	GsDOBJ2   *objp;
	
	/* process according to pad value : �p�b�h�̒l�ɂ���ď��� */
	if((PadData & PADLleft)>0) {
		ViewAngleXZ++;
		if(ViewAngleXZ >= 72) {
			ViewAngleXZ = 0;
		}
	}
	if((PadData & PADLright)>0) {
		ViewAngleXZ--;
		if(ViewAngleXZ < 0) {
		  ViewAngleXZ = 71;
		}
	}
	if((PadData & PADLup)>0) View.vpy += 100;
	if((PadData & PADLdown)>0) View.vpy -= 100;
	if((PadData & PADRdown)>0) {
		ViewRadial-=3;
		if(ViewRadial < 8) {
			ViewRadial = 8;
		}
	}
	if((PadData & PADRright)>0) {
		ViewRadial+=3;
		if(ViewRadial > 450) {
			ViewRadial = 450;
		}
	}
	if((PadData & PADk)>0) return(-1);
	if(((PadData & PADRleft)>0)&&((oldpad&PADRleft) == 0)) changeFog();
	if(((PadData & PADRup)>0)&&((oldpad&PADRup) == 0)) changeTmd();
	if(((PadData & PADn)>0)&&((oldpad&PADn) == 0)) Bakuhatu = 1;
	if(((PadData & PADl)>0)&&((oldpad&PADl) == 0)) allocCube(NCUBE);

	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;

	/* update position data for cube : �L���[�u�̈ʒu���X�V */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* begin explosion : �����̊J�n */
		if(Bakuhatu == 1) {

			/* increase rotation speed : ���]���x up */
			RotV[i].vx *= 3;
			RotV[i].vy *= 3;
			RotV[i].vz *= 3;

			/* set direction of displacement and velocity : �ړ�����&���x�ݒ� */
			TrnsV[i].vx = objp->coord2->coord.t[0]/4+
						(rand()-16384)/200;	
			TrnsV[i].vy = objp->coord2->coord.t[1]/6+
						(rand()-16384)/200-200;
			TrnsV[i].vz = objp->coord2->coord.t[2]/4+
						(rand()-16384)/200;
		}
		/* processing for during explosion : �������̏��� */
		else if(Bakuhatu > 1) {
			if(Trns[i].vy > 3000) {
				Trns[i].vy = 3000-(Trns[i].vy-3000)/2;
				TrnsV[i].vy = -TrnsV[i].vy*6/10;
			}
			else {
				TrnsV[i].vy += 20*2;	/* free fall : ���R���� */
			}

			if((TrnsV[i].vy < 70)&&(TrnsV[i].vy > -70)&&
			   (Trns[i].vy > 2800)) {
				Trns[i].vy = 3000;
				TrnsV[i].vy = 0;

				RotV[i].vx *= 95/100;
				RotV[i].vy *= 95/100;
				RotV[i].vz *= 95/100;
			}


			TrnsV[i].vx = TrnsV[i].vx*97/100;
			TrnsV[i].vz = TrnsV[i].vz*97/100;
		}

		/* update rotation angle (rotation) : ��]�p(Rotation)�̍X�V */
 		Rot[i].vx += RotV[i].vx;
		Rot[i].vy += RotV[i].vy;
		Rot[i].vz += RotV[i].vz;

		/* update translation (Transfer) : ���s�ړ���(Transfer)�̍X�V */
 		Trns[i].vx += TrnsV[i].vx;
		Trns[i].vy += TrnsV[i].vy;
		Trns[i].vz += TrnsV[i].vz;

		objp++;
	}

	if(Bakuhatu == 1)
		Bakuhatu++;

	return(0);
}

/*
 *  set cube to initial position : �L���[�u�������ʒu�ɔz�u
 */
void allocCube(n)
int n;
{	
	int x, y, z;
	int i;
	int *posp;
	GsDOBJ2 *objp;
	GsCOORDINATE2 *coordp;

	posp = cube_def_pos;
	objp = object;
	coordp = objcoord;
	nobj = 0;
	for(i = 0; i < NCUBE; i++) {

		/* intialize object struture : �I�u�W�F�N�g�\���̂̏����� */
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		GsInitCoordinate2(WORLD, coordp);
		objp->coord2 = coordp;
		objp->attribute = 0;
		coordp++;
		objp++;

		/* set initial position (read from pos.h) : 
		   �����ʒu�̐ݒ�(pos.h����ǂ�) */
		Trns[i].vx = *posp++;
		Trns[i].vy = *posp++;
		Trns[i].vz = *posp++;
		Rot[i].vx = 0;
		Rot[i].vy = 0;
		Rot[i].vz = 0;

		/* initialize velocity : ���x�̏����� */
		TrnsV[i].vx = 0;
		TrnsV[i].vy = 0;
		TrnsV[i].vz = 0;
		RotV[i].vx = rand()/300;
		RotV[i].vy = rand()/300;
		RotV[i].vz = rand()/300;

		nobj++;
	}
	Bakuhatu = 0;
}

/*
 *  initialize function group : �C�j�V�����C�Y�֐��Q
 */
void initSystem()
{
	int i;

	/* initialize pad : �p�b�h�̏����� */
	PadInit(0);

	/* initialize graphics : �O���t�B�b�N�̏����� */
	GsInitGraph(640, 480, 2, 0, 0);
	GsDefDispBuff(0, 0, 0, 0);

	/* initialize OT : OT�̏����� */
	for(i = 0; i < 2; i++) {	
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}	

	/* initialize 3D system : 3D�V�X�e���̏����� */
	GsInit3D();
}

void initModelingData(tmdp)
u_long *tmdp;
{
	u_long size;
	int i;
	int tmdobjnum;
	
	/* skip header : �w�b�_���X�L�b�v */
	tmdp++;

	/* mapping to read address : ���A�h���X�փ}�b�s���O */
	GsMapModelingData(tmdp);

	tmdp++;
	tmdobjnum = *tmdp;
	tmdp++; 		/* point to object at beginning : 
	                   �擪�̃I�u�W�F�N�g���|�C���g */
	TmdBase = tmdp;
}

/*
 *  read in texture (transfer to VRAM) : 
 *  �e�N�X�`���̓ǂݍ��݁iVRAM�ւ̓]���j
 */
void initTexture(tex_addr)
u_long *tex_addr;
{
	RECT rect1;
	GsIMAGE tim1;
	int i;
	
	while(1) {
		if(*tex_addr != TIM_HEADER) {
			break;
		}
		tex_addr++;	/* skip header (1word) : �w�b�_�̃X�L�b�v(1word) */
		GsGetTimInfo(tex_addr, &tim1);
		tex_addr += tim1.pw*tim1.ph/2+3+1;	/* proceed up to next block : 
		                                       ���̃u���b�N�܂Ői�߂� */
		rect1.x=tim1.px;
		rect1.y=tim1.py;
		rect1.w=tim1.pw;
		rect1.h=tim1.ph;
		LoadImage(&rect1,tim1.pixel);
		if((tim1.pmode>>3)&0x01) {	/* if ther is a CLUT, then transfer : CLUT������Γ]�� */
			rect1.x=tim1.cx;
			rect1.y=tim1.cy;
			rect1.w=tim1.cw;
			rect1.h=tim1.ch;
			LoadImage(&rect1,tim1.clut);
			tex_addr += tim1.cw*tim1.ch/2+3;
		}
	}
}

/*
 *  intialize viewpoint : ���_�̏�����
 */
void initView()
{
	/* set viewpoint variable as initial position : �����ʒu�����_�ϐ��ɃZ�b�g */
	ViewAngleXZ = 54;
	ViewRadial = 75;
	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpy = -100;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;
	View.vrx = 0; View.vry = 0; View.vrz = 0;
	View.rz=0;

	/* parent coordinates of viewpoint : ���_�̐e���W */
	View.super = WORLD;

	/* set : �ݒ� */
	GsSetRefView2(&View);

	/* Projection */
	GsSetProjection(1000);

	/* mod = normal lighting : ���[�h = 'normal lighting' */
	GsSetLightMode(0);
}

/*
 *  initialize light source : �����̏�����
 */
void initLight(c_mode, factor)
int c_mode;	/* if 0, white light; if 1, cocktail lights : 
               �O�̂Ƃ����F���A�P�̂Ƃ��J�N�e�����C�g */
int factor;	/* brightness factor (0 - 255) : ���邳�̃t�@�N�^�[(0�`255) */
{
	if(c_mode == 0) {
		/* set white light : ���F���̃Z�b�g */
		pslt[0].vx = 200; pslt[0].vy= 200; pslt[0].vz= 300;
		pslt[0].r = factor; pslt[0].g = factor; pslt[0].b = factor;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -50; pslt[1].vy= -1000; pslt[1].vz= 0;
		pslt[1].r=0x20; pslt[1].g=0x20; pslt[1].b=0x20;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
		pslt[2].r=0x0; pslt[2].g=0x0; pslt[2].b=0x0;
		GsSetFlatLight(2,&pslt[2]);
	}
	else {
		/* cocktail lights (using Gouraud) : 
		   �J�N�e�����C�g�iGouraud�Ŏg�p�j */
		pslt[0].vx = 200; pslt[0].vy= 100; pslt[0].vz= 0;
		pslt[0].r = factor; pslt[0].g = 0; pslt[0].b = 0;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -200; pslt[1].vy= 100; pslt[1].vz= 0;
		pslt[1].r=0; pslt[1].g=0; pslt[1].b=factor;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = 0; pslt[2].vy= -200; pslt[2].vz= 0;
		pslt[2].r=0; pslt[2].g=factor; pslt[2].b=0;
		GsSetFlatLight(2,&pslt[2]);
	}	

	/* ambient light : �A���r�G���g�i���ӌ��j */
	GsSetAmbient(ONE/2,ONE/2,ONE/2);
}

/*
 * fog ON/OFF : �t�H�O��ON/OFF
 */
void changeFog()
{
	if(dqf) {
		/* reset fog : �t�H�O�̃��Z�b�g */
		GsSetLightMode(0);
		dqf = 0;
		back_r = 0;
		back_g = 0;
		back_b = 0;
	}
	else {
		/* set fog : �t�H�O�̐ݒ� */
		dq.dqa = -600;
		dq.dqb = 5120*4096;
		dq.rfc = FOG_R;
		dq.gfc = FOG_G;
		dq.bfc = FOG_B;
		GsSetFogParam(&dq);
		GsSetLightMode(1);
		dqf = 1;
		back_r = FOG_R;
		back_g = FOG_G;
		back_b = FOG_B;
	}
}

/*
 *  switch TMD data : TMD�f�[�^�̐؂�ւ�
 */
void changeTmd()
{
	u_long *tmdp;
	GsDOBJ2 *objp;
	int i;

	/* switch TMD : TMD��؂�ւ� */
	CurrentTmd++;
	if(CurrentTmd == 4) {
		CurrentTmd = 0;
	}
	objp = object;
	for(i = 0; i < nobj; i++) {
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		objp++;
	}

	/* switch color/brightness of light source according to TMD type : 
	   TMD�̎�ނɂ��킹�Č����̐F/���邳��؂�ւ� */
	switch(CurrentTmd) {
	    case 0:
                /* normal (flat) : �m�[�}�� (flat) */
		initLight(0, 0xc0);
		break;
	    case 1:
	        /* semi-transparent (flat) : ������ (flat) */
		initLight(0, 0xc0);
		break;
	    case 2:
	        /* Gouraud */
		initLight(1, 0xff);
		break;
	    case 3:
	        /* with texture : �e�N�X�`���t�� */
		initLight(0, 0xff);
		break;
	}
}



int
datafile_search(file,nf)

FILE_INFO *file;
int nf;
{
	short i,j;

	for (j = 0; j < nf; j++){
		for (i = 0; i < 10; i++) {	/* ten retries : ���g���C�� 10 �� */
			if (CdSearchFile( &(file[j].finfo), file[j].fname ) != 0) 
				break;
			else
				printf("%s not find.\n",file[j].fname);
		}
	}
}


int
datafile_read(file,nf)

FILE_INFO *file;
int nf;
{
	int	mode = CdlModeSpeed;	
	int	nsector;
	short i,j;
	long cnt;

	for (j = 0; j < nf; j++){
		for (i = 0; i < 10; i++) {	/* ten retries : ���g���C�� 10 �� */
			nsector = (file[j].finfo.size + 2047) / 2048;
		
			/* set target position : �^�[�Q�b�g�|�W�V������ݒ� */
			CdControl(CdlSetloc, (u_char *)&(file[j].finfo.pos), 0);

			/* begin read : ���[�h�J�n */
			CdRead(nsector, file[j].addr, mode);
	
			/* normal operations can be performed behind the read operation.*/
			/* sector count is monitored here until Read is finished*/
			/* ���[�h�̗��Œʏ�̏����͎��s�ł���B*/
			/* �����ł́ARead ���I������܂Ŏc��̃Z�N�^�����Ď����� */
			while ((cnt = CdReadSync(1, 0)) > 0 ) {
				VSync(0);
			}
		
			/* break if normal exit : ����I���Ȃ�΃u���[�N */
			if (cnt == 0)	break;
		}
	}
}

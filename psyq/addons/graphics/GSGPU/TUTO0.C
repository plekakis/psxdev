/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*
 *	lowlevel functions in high level program  
 :       GsOT �� �v���~�e�B�u�� AddPrim ����� (GS �x�[�X�j
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	2.00		Aug,31,1993	masa (original)
 *	2.10		Mar,25,1994	suzu (added addPrimitive())
 *      2.20            Dec,25,1994     yuta (chaned GsDOBJ4)
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

#define MODEL_ADDR	(u_long *)0x80040000	/* modeling data info. */
#define TEX_ADDR	(u_long *)0x80180000	/* texture info. */
	
#define SCR_Z		1000		/* projection */
#define OT_LENGTH	12		/* OT resolution */
#define OTSIZE		(1<<OT_LENGTH)	/* OT tag size */
#define PACKETMAX	4000		/* max number of packets per frame */
#define PACKETMAX2	(PACKETMAX*24)	/* average packet size is 24 */

PACKET	GpuPacketArea[2][PACKETMAX2];	/* packet area (double buffer) */
GsOT	WorldOT[2];			/* OT info */
SVECTOR	PWorld;			 	/* vector for making Coordinates */

GsOT_TAG	OTTags[2][OTSIZE];	/* OT tag */
GsDOBJ2		object;			/* object substance */
GsRVIEW2	view;			/* view point */
GsF_LIGHT	pslt[3];		/* lighting point */
u_long		PadData;		/* controller info. */
GsCOORDINATE2   DWorld;			/* Coordinate for GsDOBJ2 */

extern MATRIX GsIDMATRIX;

/*
 *  main: ���C�����[�`��
 */
main()
{
	
	/* Initialize: �C�j�V�����C�Y */
	initSystem();			/* grobal variables */
	initView();			/* position matrix */
	initLight();			/* light matrix */
	initModelingData(MODEL_ADDR);	/* load model data */
	initTexture(TEX_ADDR);		/* load texture pattern */
	initPrimitives();		/* GPU primitives */
	
	while(1) {
		if ( moveObject() ) break;
		drawObject();
	}
	PadStop();
	StopCallback();
	return 0;
}

/*
 *  3D object drawing procedure; 3D�I�u�W�F�N�g�`�揈��
 */
drawObject()
{
	int activeBuff;
	MATRIX tmpls;
	
	/* get active buffer ID: �_�u���o�b�t�@�̂����ǂ��炪�A�N�e�B�u���H */
	activeBuff = GsGetActiveBuff();
	
	/* Set GPU packet generation address to start of area
	 : GPU�p�P�b�g�����A�h���X���G���A�̐擪�ɐݒ�
	 */
	GsSetWorkBase((PACKET*)GpuPacketArea[activeBuff]);
	
	/* clear contents of OT : OT�̓��e���N���A */
	ClearOTagR((u_long *)WorldOT[activeBuff].org, OTSIZE);
	
	/* register 3D object to OT: 3D�I�u�W�F�N�g�i�L���[�u�j��OT�ւ̓o�^ */
	GsGetLw(object.coord2,&tmpls);		
	GsSetLightMatrix(&tmpls);
	GsGetLs(object.coord2,&tmpls);
	GsSetLsMatrix(&tmpls);
	GsSortObject4(&object,
		      &WorldOT[activeBuff],14-OT_LENGTH, getScratchAddr(0));
	
	/* add primitive: �v���~�e�B�u��ǉ� */
	addPrimitives(WorldOT[activeBuff].org);
	
	/* fetch pad contents: �p�b�h�̓��e���o�b�t�@�Ɏ�荞�� */
	PadData = PadRead(0);

	/* wait for V-BLNK: V-BLNK��҂� */
	VSync(0);

	/* forced termination of previous frame's drawing operation
	 : �O�̃t���[���̕`���Ƃ������I��
	 */
	ResetGraph(1);

	/* replace double buffer: �_�u���o�b�t�@����ꊷ���� */
	GsSwapDispBuff();

	/* insert screen clear command at start of OT
	 : OT�̐擪�ɉ�ʃN���A���߂�}��
	 */
	GsSortClear(0x0, 0x0, 0x0, &WorldOT[activeBuff]);

	/* Start drawing contents of OT as background
	 : OT�̓��e���o�b�N�O���E���h�ŕ`��J�n
	 */
	/*DumpOTag(WorldOT[activeBuff].org+OTSIZE-1);*/
	DrawOTag((u_long *) (WorldOT[activeBuff].org+OTSIZE-1));
}

/*
 *  translate object using control pad
 :�R���g���[���p�b�h�ɂ��I�u�W�F�N�g�ړ�
 */
moveObject()
{
	/* update local coordinate systems among object veriables
	 : �I�u�W�F�N�g�ϐ����̃��[�J�����W�n�̒l���X�V
	 */
	if(PadData & PADRleft) PWorld.vy -= 5*ONE/360;
	if(PadData & PADRright) PWorld.vy += 5*ONE/360;
	if(PadData & PADRup) PWorld.vx += 5*ONE/360;
	if(PadData & PADRdown) PWorld.vx -= 5*ONE/360;
	
	if(PadData & PADm) DWorld.coord.t[2] -= 200;
	if(PadData & PADo) DWorld.coord.t[2] += 200;
	
	/* Calculate Matrix from Object Parameter and Set Coordinate
	 :�I�u�W�F�N�g�̃p�����[�^����}�g���b�N�X���v�Z�����W�n�ɃZ�b�g
	 */
	set_coordinate(&PWorld,&DWorld);
	
	/* Clear flag of Coordinate for recalculation
	 : �Čv�Z�̂��߂̃t���O���N���A����
	 */
	DWorld.flg = 0;
	
	/* quit */
	if(PadData & PADselect) 
		return -1;		
	return 0;
}

/*
 *  initialization routines: ���������[�`���Q
 */
initSystem()
{
	int	i;

	/* control pad initialization: �R���g���[���p�b�h�̏����� */
	PadInit(0);
	PadData = 0;
	
	/* initialize environment: ���̏����� */
	GsInitGraph(320, 240, 0, 0, 0);
	GsDefDispBuff(0, 0, 0, 240);
	
	/* initialize OT: OT�̏����� */
	for (i = 0; i < 2; i++) {
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].point  = 0;
		WorldOT[i].offset = 0;
		WorldOT[i].org    = OTTags[i];
		WorldOT[i].tag    = OTTags[i] + OTSIZE - 1;
	}
	
	/* init 3D libs: 3D���C�u�����̏����� */
	GsInit3D();
}

/*
 *  set viewing position: ���_�ʒu�̐ݒ�
 */
initView()
{
	/* set projection: �v���W�F�N�V�����i����p�j�̐ݒ� */
	GsSetProjection(SCR_Z);

	/* set viewing point: ���_�ʒu�̐ݒ� */
	view.vpx = 0; view.vpy = 0; view.vpz = -1000;
	view.vrx = 0; view.vry = 0; view.vrz = 0;
	view.rz = 0;
	view.super = WORLD;
	GsSetRefView2(&view);

	/* set Zclip value: Z�N���b�v�l��ݒ� */
	GsSetNearClip(100);
}

/*
 *  set light source (lighting direction and color)
 : �����̐ݒ�i�Ǝ˕������F�j
 */
initLight()
{
	/* lighting in direction of light source #0(100,100,100)
	 : ����#0 (100,100,100)�̕����֏Ǝ�
	 */
	pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
	pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
	GsSetFlatLight(0,&pslt[0]);
	
	/* light source #1 (not used): ����#1�i�g�p�����j */
	pslt[1].vx = 100; pslt[1].vy= 100; pslt[1].vz= 100;
	pslt[1].r=0; pslt[1].g=0; pslt[1].b=0;
	GsSetFlatLight(1,&pslt[1]);
	
	/* light source #2 (not used): ����#1�i�g�p�����j */
	pslt[2].vx = 100; pslt[2].vy= 100; pslt[2].vz= 100;
	pslt[2].r=0; pslt[2].g=0; pslt[2].b=0;
	GsSetFlatLight(2,&pslt[2]);
	
	/* set ambient: �A���r�G���g�i���ӌ��j�̐ݒ� */
	GsSetAmbient(ONE/2,ONE/2,ONE/2);

	/* set light mode: �������[�h�̐ݒ�i�ʏ����/FOG�Ȃ��j */
	GsSetLightMode(0);
}

/*
 * read TMD data from memory and initialize object
 : ���������TMD�f�[�^�̓ǂݍ���&�I�u�W�F�N�g������
 *		(�擪�̂P�̂ݎg�p�j
 */
initModelingData(addr)
u_long *addr;
{
	u_long *tmdp;
	
	/* top address of TMD data: TMD�f�[�^�̐擪�A�h���X */
	tmdp = addr;			
	
	/* skip fie header: �t�@�C���w�b�_���X�L�b�v */
	tmdp++;				
	
	/* map to real address: ���A�h���X�փ}�b�v */
	GsMapModelingData(tmdp);	
	
	tmdp++;		/* skip flag: �t���O�ǂݔ�΂� */
	tmdp++;		/* skip number of objects:�I�u�W�F�N�g���ǂݔ�΂� */
	
	GsLinkObject4((u_long)tmdp,&object,0);
	
	/* Init work vector
         : �}�g���b�N�X�v�Z���[�N�̃��[�e�[�V�����x�N�^�[������
	 */
        PWorld.vx=PWorld.vy=PWorld.vz=0;
	GsInitCoordinate2(WORLD, &DWorld);
	
	/* initialize 3D objects: 3D�I�u�W�F�N�g������ */
	object.coord2 =  &DWorld;
	object.coord2->coord.t[2] = 4000;
	object.tmd = tmdp;		
	object.attribute = 0;
}

/*
 *  read texture data from memory: �i��������́j�e�N�X�`���f�[�^�̓ǂݍ���
 */
initTexture(addr)
u_long *addr;
{
	RECT rect1;
	GsIMAGE tim1;

	/* get TIM data info. : TIM�f�[�^�̏��𓾂� */	
	/* skip and pass file header: �t�@�C���w�b�_���΂��ēn�� */
	GsGetTimInfo(addr+1, &tim1);	

	/* transfer pixel data to VRAM: �s�N�Z���f�[�^��VRAM�֑��� */	
	rect1.x=tim1.px;
	rect1.y=tim1.py;
	rect1.w=tim1.pw;
	rect1.h=tim1.ph;
	LoadImage(&rect1,tim1.pixel);

	/* if CLUT exists, transfer it to VRAM: CLUT������ꍇ��VRAM�֑��� */
	if((tim1.pmode>>3)&0x01) {
		rect1.x=tim1.cx;
		rect1.y=tim1.cy;
		rect1.w=tim1.cw;
		rect1.h=tim1.ch;
		LoadImage(&rect1,tim1.clut);
	}
}

/* Set coordinte parameter from work vector
 :���[�e�V�����x�N�^����}�g���b�N�X���쐬�����W�n�ɃZ�b�g����
 */
set_coordinate(pos,coor)
SVECTOR *pos;			/* work vector : ���[�e�V�����x�N�^ */
GsCOORDINATE2 *coor;		/* Coordinate : ���W�n */
{
  MATRIX tmp1;
  SVECTOR v1;
  
  tmp1   = GsIDMATRIX;		/* start from unit matrix
				 :�P�ʍs�񂩂�o������ */
    
  /* Set translation : ���s�ړ����Z�b�g���� */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  v1 = *pos;
  
  /* Rotate Matrix
   : �}�g���b�N�X�Ƀ��[�e�[�V�����x�N�^����p������
   */
  RotMatrix(&v1,&tmp1);
  
  /* Set Matrix to Coordinate
   : ���߂��}�g���b�N�X�����W�n�ɃZ�b�g����
   */
  coor->coord = tmp1;
  
  /* Clear flag becase of changing parameter
   : �}�g���b�N�X�L���b�V�����t���b�V������
   */
  coor->flg = 0;
}

/*
 * initialize primitives: �v���~�e�B�u�̏�����
 */
#include "balltex.h"
/* number of balls: �{�[���̌� */
#define NBALL	256		

/* ball scattering range: �{�[�����U��΂��Ă���͈� */
#define DIST	SCR_Z/4		

/* primitive buffer: �v���~�e�B�u�o�b�t�@ */
POLY_FT4	ballprm[2][NBALL];	

/* position of ball: �{�[���̈ʒu */
SVECTOR		ballpos[NBALL];		
	
initPrimitives()
{

	int		i, j;
	u_short		tpage, clut[32];
	POLY_FT4	*bp;	
		
	/* load ball's texture page
	 : �{�[���̃e�N�X�`���y�[�W�����[�h����
	 */
	tpage = LoadTPage(ball16x16, 0, 0, 640, 256, 16, 16);

	/* load CLUT for balls: �{�[���p�� CLUT�i�R�Q�j�����[�h���� */
	for (i = 0; i < 32; i++)
		clut[i] = LoadClut(ballcolor[i], 256, 480+i);
	
	/* initialize primiteves: �v���~�e�B�u������ */
	for (i = 0; i < 2; i++)
		for (j = 0; j < NBALL; j++) {
			bp = &ballprm[i][j];
			SetPolyFT4(bp);
			SetShadeTex(bp, 1);
			bp->tpage = tpage;
			bp->clut = clut[j%32];
			setUV4(bp, 0, 0, 16, 0, 0, 16, 16, 16);
		}
	
	/* initialize positio: �ʒu�������� */
	for (i = 0; i < NBALL; i++) {
		ballpos[i].vx = (rand()%DIST)-DIST/2;
		ballpos[i].vy = (rand()%DIST)-DIST/2;
		ballpos[i].vz = (rand()%DIST)-DIST/2;
	}
}

/*
 * register primitives to OT: �v���~�e�B�u�̂n�s�ւ̓o�^
 */
addPrimitives(ot)
u_long	*ot;
{
	static int	id    = 0;		/* buffer ID */
	static VECTOR	trans = {0, 0, SCR_Z};	/* world-screen vector */
	static SVECTOR	angle = {0, 0, 0};	/* world-screen angle */
	static MATRIX	rottrans;		/* world-screen matrix */
	int		i, padd;
	long		dmy, flg, otz;
	POLY_FT4	*bp;
	SVECTOR		*sp;
	SVECTOR		dp;
	
	
	id = (id+1)&0x01;	/* swap ID: ID �� �X���b�v */
	
	/* push current GTE matrix: �J�����g�}�g���N�X��ޔ������� */
	PushMatrix();		
	
	/* read controler and update world-screen matrix
	 : �p�b�h�̓��e����}�g���N�X rottrans ���A�b�v�f�[�g
	 */
	padd = PadRead(1);
	if(padd & PADLup)    angle.vx += 10;
	if(padd & PADLdown)  angle.vx -= 10;
	if(padd & PADLleft)  angle.vy -= 10;
	if(padd & PADLright) angle.vy += 10;
	if(padd & PADl) trans.vz -= 50;
	if(padd & PADn) trans.vz += 50;
	
	RotMatrix(&angle, &rottrans);		/* rotate: ��] */
	TransMatrix(&rottrans, &trans);		/* translate: ���s�ړ� */
	
	/* copy world-screen matrix ('rottrans') to current matrix
	 : �}�g���N�X rottrans ���J�����g�}�g���N�X�ɐݒ�
	 */
	SetTransMatrix(&rottrans);	
	SetRotMatrix(&rottrans);
	
	/* update primitive members:�v���~�e�B�u���A�b�v�f�[�g */
	bp = ballprm[id];
	sp = ballpos;
	for (i = 0; i < NBALL; i++, bp++, sp++) {
		otz = RotTransPers(sp, (long *)&dp, &dmy, &flg);
		if (otz > 0 && otz < OTSIZE) {
			setXY4(bp, dp.vx, dp.vy,    dp.vx+16, dp.vy,
			           dp.vx, dp.vy+16, dp.vx+16, dp.vy+16);

			AddPrim(ot+otz, bp);
		}
	}

	/* recover old GTE matrix: �ޔ������Ă����}�g���N�X�������߂��B*/
	PopMatrix();
}
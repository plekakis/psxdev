/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/* 
 * 	janken: sample program using TOD data
 * 
 *	"main.c" main routine
 *
 * 		Version 2.00	Feb, 3, 1995
 * 
 * 		Copyright (C) 1994, 1995 by Sony Computer Entertainment
 * 		All rights Reserved
 */

/*****************/
/* Include files */
/*****************/
#include <sys/types.h>
#include <libetc.h>		/* PAD���g�����߂ɃC���N���[�h����K�v���� */
#include <libgte.h>		/* LIGS���g�����߂ɃC���N���[�h����K�v���� */
#include <libgpu.h>		/* LIGS���g�����߂ɃC���N���[�h����K�v���� */
#include <libgs.h>		/* �O���t�B�b�N���C�u���� ���g�����߂�
				   �\���̂Ȃǂ���`����Ă��� */
#include "tod.h"		/* TOD�����֐��̐錾�� */
#include "te.h"			/* TMD-ID ���X�g (rsdlink�ō쐬) */

/**********/
/* For OT */
/**********/
#define OT_LENGTH	12		 /* OT�𑜓x��12bit�i�傫���j */
GsOT		WorldOT[2];		 /* OT���i�_�u���o�b�t�@�j */
GsOT_TAG	OTTags[2][1<<OT_LENGTH]; /* OT�̃^�O�̈�i�_�u���o�b�t�@�j */

/***************/
/* For Objects */
/***************/
#define OBJMAX	50			/* �ő�I�u�W�F�N�g�� */
GsDOBJ2		obj_area[OBJMAX];	/* �I�u�W�F�N�g�ϐ��̈� */
GsCOORDINATE2	obj_coord[OBJMAX];	/* ���[�J�����W�ϐ��̈� */
GsCOORD2PARAM	obj_cparam[OBJMAX];	/* ���[�J�����W�ϐ��̈� */
TodOBJTABLE	objs;			/* �I�u�W�F�N�g�e�[�u�� */

/***************/
/* For Packets */
/***************/
#define PACKETMAX	6000		/* 1�t���[���̍ő�p�P�b�g�� */
#define PACKETMAX2	(PACKETMAX*24)	/* 1�p�P�b�g���σT�C�Y��24Byte�Ƃ��� */
PACKET	GpuPacketArea[2][PACKETMAX2];	/* �p�P�b�g�̈�i�_�u���o�b�t�@�j */

/***************/
/* For Viewing */
/***************/
#define VPX	-8000
#define VPY	-2000
#define VPZ	-5000
#define VRX	-900
#define VRY	-1500
#define VRZ	0
GsRVIEW2	view;			/* ���_��� */

/************************/
/* For Lights ( three ) */
/************************/
GsF_LIGHT	pslt[3];		/* �����~�R�� */

/*********************/
/* For Modeling data */
/*********************/
#define MODEL_ADDR	0x800c0000
u_long		*TmdP;			/* ���f�����O�f�[�^�|�C���^ */

/**********************/
/* For Animation data */
/**********************/
#define TOD_ADDR0	0x800e0000
#define TOD_ADDR1	0x80100000
#define TOD_ADDR2	0x80120000
#define TOD_ADDR3	0x80140000
#define TOD_ADDR4	0x80160000
#define TOD_ADDR5	0x80180000
#define TOD_ADDR6	0x801a0000
#define TOD_ADDR7	0x801c0000
#define TOD_ADDR8	0x801e0000
#define NUMTOD		9
u_long		*TodP[NUMTOD];		/* �A�j���[�V�����f�[�^�|�C���^�z�� */
int		NumFrame[NUMTOD];	/* �A�j���[�V�����f�[�^�t���[�����z�� */
int		StartFrameNo[NUMTOD];	/* �A�j���[�V�����X�^�[�g�t���[���ԍ��z�� */

/******************/
/* For Controller */
/******************/
u_long		padd;

/*******************/
/* For Janken type */
/*******************/
int		pose;

/*************************/
/* Prototype Definitions */
/*************************/
int	exitProgram();
void	drawObject();
void	initSystem();
void	initView();
void	initLight();
void	initModelingData();
void	initTod();
void	initPose();
int	obj_interactive();


/*
 * Main routine
/* ���C���E���[�`��
 *
 */
main()
{
    /* Initialize 
       /* �C�j�V�����C�Y */
    ResetCallback();
    initSystem();
    initView();
    initLight();
    initModelingData();
    initTod();
    initPose();
    
    FntLoad(960, 256);
    SetDumpFnt(FntOpen(32-320, 32-120, 256, 200, 0, 512));

    while( 1 ) {

	/* Read the pad data */
	padd = PadRead( 1 );
	/* play TOD data according to the pad data */
	if(obj_interactive()) break;
    }

    PadStop();
    ResetGraph(3);
    StopCallback();
    return 0;
}

/*
 * Draw 3D objects
/* 3D�I�u�W�F�N�g�`�揈��
 *
 */
void
drawObject()
{
    int		i;
    int		activeBuff;
    GsDOBJ2	*objp;
    MATRIX	LsMtx;
    int         load_count1,load_count2,load_count3,load_org;

    activeBuff = GsGetActiveBuff();

    /* Set the address at which drawing command will be put
       /* �`��R�}���h���i�[����A�h���X��ݒ肷�� */
    GsSetWorkBase( (PACKET*)GpuPacketArea[activeBuff] );

    /* Initialize OT 
       /* OT �̏����� */
    GsClearOt( 0, 0, &WorldOT[activeBuff] );

    /* Set the pointer to the object array 
       /* �I�u�W�F�N�g�z��ւ̃|�C���^���Z�b�g���� */
    objp = objs.top;

    load_count1=load_count2=load_count3=0;
    for( i = 0; i < objs.nobj; i++ ) { 

	/* flag whether the coord has changed or not
	   /* coord ������������ꂽ���ǂ����̃t���O */
	objp->coord2->flg = 0;

	if ( ( objp->id != TOD_OBJ_UNDEF ) && ( objp->tmd != 0 ) ) {

	    /* Calculate the local screen matrix 
	       /* ���[�J���X�N���[���}�g���b�N�X���v�Z */
	    load_org = VSync(1);
	    GsGetLs( objp->coord2, &LsMtx );
	    load_count1+= VSync(1)-load_org;
	    /* Set the local screen matrix to GTE 
	       /* ���[�J���X�N���[���}�g���b�N�X�� GTE �ɃZ�b�g���� */
	    GsSetLsMatrix( &LsMtx );
	    
	    /* Set the light matrix to GTE 
	       /* ���C�g�}�g���b�N�X�� GTE �ɃZ�b�g���� */
	    GsSetLightMatrix( &LsMtx );
	    
	    /* Transform the object perspectively and assign it to the OT
	       /* �I�u�W�F�N�g�𓧎��ϊ����I�[�_�����O�e�[�u���Ɋ���t���� */
	    load_org = VSync(1);
	    GsSortObject4( objp, 		/* Pointer to the object */
			   &WorldOT[activeBuff],/* Pointer to the OT */
			   14-OT_LENGTH,getScratchAddr(0) );	/* number of bits to be shifted*/
	    load_count2+= VSync(1)-load_org;
	}

	objp++;
    }
    FntPrint("%d %d\n",load_count1,load_count2);

    VSync( 0 );				/* Wait for V-BLNK */
    ResetGraph( 1 );			/* Cancel the current display */
    GsSwapDispBuff();			/* Switch the double buffer */

    /* Resister the clear command to OT 
       /* ��ʃN���A�R�}���h�� OT �ɓo�^ */
    GsSortClear( 0x0,				/* R of the background */
		 0x0,				/* G of the background */
		 0x0,				/* B of the background */
		 &WorldOT[activeBuff] );	/* Pointer to the OT */

    /* Do the drawing commands assigned to the OT 
       /* OT �Ɋ���t����ꂽ�`��R�}���h�̕`�� */
    GsDrawOt( &WorldOT[activeBuff] );
    FntFlush(-1);
}

/*
 * Initialize system
/* ���������[�`���Q
 *
 */
void
initSystem()
{
  /* Initialize the controll pad
     /* �R���g���[���p�b�h�̏����� */
  PadInit( 0 );
  padd = 0;
	
  /* Initialize the GPU 
     /* GPU�̏����� */
  GsInitGraph( 640, 240, GsOFSGPU|GsNONINTER, 1, 0 );
  GsDefDispBuff( 0, 0, 0, 240 );
	
  /* Initialize the OT 
     /* OT�̏����� */
  WorldOT[0].length = OT_LENGTH;
  WorldOT[0].org = OTTags[0];
  WorldOT[0].offset = 0;
  WorldOT[1].length = OT_LENGTH;
  WorldOT[1].org = OTTags[1];
  WorldOT[1].offset = 0;
	
  /* Initialize the 3D library 
     /* 3D���C�u�����̏����� */
  GsInit3D();
}

/*
 * Set the location of the view point
/* ���_�ʒu�̐ݒ�
 *
 */
void
initView()
{
    /* Set the view angle 
       /* �v���W�F�N�V�����i����p�j�̐ݒ� */
    GsSetProjection( 1000 );

    /* Set the view point and the reference point 
       /* ���_�ʒu�̐ݒ� */
    view.vpx = VPX; view.vpy = VPY; view.vpz = VPZ;
    view.vrx = VRX; view.vry = VRY; view.vrz = VRZ;
    view.rz = 0;
    view.super = WORLD;
    GsSetRefView2( &view );

    /* Set the value of Z clip 
       /* Z�N���b�v�l��ݒ� */
    GsSetNearClip( 100 );
}

/*
 * Set the light ( the direction and the color )
/* �����̐ݒ�i�Ǝ˕������F�j
 *
 */
void
initLight()
{
    /* Light #0 , direction (100,100,100) 
       /* ����#0 (100,100,100)�̕����֏Ǝ� */
    pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
    pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
    GsSetFlatLight( 0,&pslt[0] );
	
    /* Light #1 ( not used ) 
       /* ����#1�i�g�p�����j */
    pslt[1].vx = 100; pslt[1].vy= 100; pslt[1].vz= 100;
    pslt[1].r=0; pslt[1].g=0; pslt[1].b=0;
    GsSetFlatLight( 1,&pslt[1] );
	
    /* Light #2 ( not used ) 
       /* ����#2�i�g�p�����j */
    pslt[2].vx = 100; pslt[2].vy= 100; pslt[2].vz= 100;
    pslt[2].r=0; pslt[2].g=0; pslt[2].b=0;
    GsSetFlatLight( 2,&pslt[2] );
	
    /* Set the ambient light 
       /* �A���r�G���g�i���ӌ��j�̐ݒ� */
    GsSetAmbient( ONE/2,ONE/2,ONE/2 );

    /* Set the light mode 
       /* �������[�h�̐ݒ�i�ʏ����/FOG�Ȃ��j */
    GsSetLightMode( 0 );
}

/*
 * Get TMD data from the memory ( use only the top one )
/* ���������TMD�f�[�^�̓ǂݍ��� (�擪�̂P�̂ݎg�p�j
 *
 */
void
initModelingData()
{
    /* Top address of the TMD data 
       /* TMD�f�[�^�̐擪�A�h���X */
    TmdP = ( u_long * )MODEL_ADDR;

    /* Skip the file header 
       /* �t�@�C���w�b�_���X�L�b�v */
    TmdP++;

    /* Map to the real address 
       /* ���A�h���X�փ}�b�v */
    GsMapModelingData( TmdP );

    /* Initialize the object table 
       /* �I�u�W�F�N�g�e�[�u���̏����� */
    TodInitObjTable( &objs, obj_area, obj_coord, obj_cparam, OBJMAX );
}


/*
 * Set TOD data from the memory
/* ���������TOD�f�[�^�̓ǂݍ���
 *
 */
void
initTod()
{
	TodP[0] = ( u_long * )TOD_ADDR0;
	TodP[0]++;
	NumFrame[0] = *TodP[0]++;
	StartFrameNo[0] = *( TodP[0] + 1 );

	TodP[1] = ( u_long * )TOD_ADDR1;
	TodP[1]++;
	NumFrame[1] = *TodP[1]++;
	StartFrameNo[1] = *( TodP[1] + 1 );

	TodP[2] = ( u_long * )TOD_ADDR2;
	TodP[2]++;
	NumFrame[2] = *TodP[2]++;
	StartFrameNo[2] = *( TodP[2] + 1 );

	TodP[3] = ( u_long * )TOD_ADDR3;
	TodP[3]++;
	NumFrame[3] = *TodP[3]++;
	StartFrameNo[3] = *( TodP[3] + 1 );

	TodP[4] = ( u_long * )TOD_ADDR4;
	TodP[4]++;
	NumFrame[4] = *TodP[4]++;
	StartFrameNo[4] = *( TodP[4] + 1 );

	TodP[5] = ( u_long * )TOD_ADDR5;
	TodP[5]++;
	NumFrame[5] = *TodP[5]++;
	StartFrameNo[5] = *( TodP[5] + 1 );

	TodP[6] = ( u_long * )TOD_ADDR6;
	TodP[6]++;
	NumFrame[6] = *TodP[6]++;
	StartFrameNo[6] = *( TodP[6] + 1 );

	TodP[7] = ( u_long * )TOD_ADDR7;
	TodP[7]++;
	NumFrame[7] = *TodP[7]++;
	StartFrameNo[7] = *( TodP[7] + 1 );

	TodP[8] = ( u_long * )TOD_ADDR8;
	TodP[8]++;
	NumFrame[8] = *TodP[8]++;
	StartFrameNo[8] = *( TodP[8] + 1 );
}

/*
 * Draw the initial pose
/* �o�ꂩ�珉���|�[�Y���Ƃ�܂�
 *
 */
void
initPose()
{
    int		i;
    int		j;
    int		frameNo;
    int		oldFrameNo;

    /* �ŏ��̃t���[���� TOD�f�[�^�̏��� */
    TodP[0] = TodSetFrame( StartFrameNo[0], TodP[0], &objs, te_list, 
			   TmdP, TOD_CREATE );
    drawObject();
    oldFrameNo = StartFrameNo[0];

    /* ����ȍ~�̃t���[���� TOD�f�[�^�̏��� */
    for ( i = 1 ; i < NumFrame[0] ; i++ ) {

	frameNo = *( TodP[0] + 1 );

	for ( j = 0 ; j < frameNo - oldFrameNo - 1 ; j++ ) {
	    drawObject();
	}

	/* 1�t���[������ TOD�f�[�^�̏��� */
	TodP[0] = TodSetFrame( frameNo, TodP[0], &objs, te_list, 
			       TmdP, TOD_CREATE );
	drawObject();
	oldFrameNo = frameNo;
    }
    drawObject();

    pose = -1;
}

/*
 * play TOD data according to the pad data 
/* �p�b�h�ɂ��ATOD�f�[�^��\������
 *
 */
int obj_interactive()
{
    int		i;
    u_long	*TodPtmp;		/* pointer to TOD data  */
    int		step;
    
    /* �u���[�v( ) */
    if ( ( padd & PADRright ) > 0 ) {

	/* ���݂̃|�[�Y����U��n�߂̒�ʒu�ɖ߂� */
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* �u����`�񂯁`��v�ƈ��茝��U�� */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* ��ʒu����u���[�v���o�� */
	TodPtmp = TodP[2];
	for ( i = StartFrameNo[2] ; i < NumFrame[2] + StartFrameNo[2] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	pose = 2;
    }

    /* �u�ρ[�v */
    if ( ( padd & PADRleft ) > 0 ) {

	/* ���݂̃|�[�Y����U��n�߂̒�ʒu�ɖ߂� */
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* �u����`�񂯁`��v�ƈ��茝��U�� */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* ��ʒu����u�ρ[�v���o�� */
	TodPtmp = TodP[4];
	for ( i = StartFrameNo[4] ; i < NumFrame[4] + StartFrameNo[4] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	pose = 4;
    }

    /* �u���傫�v */
    if ( ( padd & PADRup ) > 0 ) {

	/* ���݂̃|�[�Y����U��n�߂̒�ʒu�ɖ߂� */
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* �u����`�񂯁`��v�ƈ��茝��U�� */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* ��ʒu����u���傫�v���o�� */
	TodPtmp = TodP[3];
	for ( i = StartFrameNo[3] ; i < NumFrame[3] + StartFrameNo[3] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	pose = 3;
    }

    /* �u���[�v�u���傫�v�u�ρ[�v�̂����ꂩ�������_���ɏo�� */
    if ( ( padd & PADRdown ) > 0 ) {

	/* ���݂̃|�[�Y����U��n�߂̒�ʒu�ɖ߂� */
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* �u����`�񂯁`��v�ƈ��茝��U�� */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* ��ʒu����u���[�v�u���傫�v�u�ρ[�v�̂����ꂩ�������_���ɏo�� */
	pose = rand()%3 + 2;
	TodPtmp = TodP[pose];
	for ( i = StartFrameNo[pose] 
	      ; i < NumFrame[pose] + StartFrameNo[pose] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();
    }

    /* ���_�̕ύX */
    if ( ( padd & PADn ) > 0 ) {
	view.vpz += 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADo ) > 0 ) {
	view.vpz -= 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLleft ) > 0 ) {
	view.vpx -= 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLright ) > 0 ) {
	view.vpx += 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLdown ) > 0 ) {
	view.vpy += 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLup ) > 0 ) {
	view.vpy -= 100;
	GsSetRefView2( &view );
	drawObject();
    }
    
    /* Move the view point to the initial place 
       /* ���_�����ɖ߂� */
    if ( ( padd & PADh ) > 0 ) {
	initView();
	drawObject();
    }

    /* Exit the program 
       /* �v���O�������I������ */
    if ( ( padd & PADk ) > 0 ) {

	/* �����|�[�Y�ɖ߂� */
	if ( pose > 0 ) {

	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* ���_�����̈ʒu�ɖ߂� */
	step = ( ( VPX - view.vpx ) * ( VPX - view.vpx )
		 + ( VPY - view.vpy ) * ( VPY - view.vpy )
		 + ( VPZ - view.vpz ) * ( VPZ - view.vpz ) ) / 500000;
	if ( step > 50 ) {
	    step = 50;
	}
	for ( i = 1 ; i <= step ; i++ ) {
	    view.vpx = view.vpx + ( i * ( VPX - view.vpx ) ) / step;
	    view.vpy = view.vpy + ( i * ( VPY - view.vpy ) ) / step;
	    view.vpz = view.vpz + ( i * ( VPZ - view.vpz ) ) / step;
	    GsSetRefView2( &view );
	    drawObject();
	}

	/* �ޏ� */
	TodPtmp = TodP[8];
	for ( i = StartFrameNo[8] ; i < NumFrame[8] + StartFrameNo[8] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* �v���O�����̏I�� */
	return -1;
    }

    return 0;
}
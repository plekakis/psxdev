/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *	spaceshuttle: sample program
 *
 *	"tuto0.c" main routine
 *
 *		Version 1.00	Mar,  31, 1994
 *
 *		Copyright (C) 1994  Sony Computer Entertainment
 *		All rights Reserved
 */

#include <sys/types.h>

#include <libetc.h>		/* for Control Pad :
				   PAD���g�����߂ɃC���N���[�h����K�v���� */
#include <libgte.h>		/* LIBGS uses libgte :
				   LIGS���g�����߂ɃC���N���[�h����K�v����*/
#include <libgpu.h>		/* LIBGS uses libgpu :
				   LIGS���g�����߂ɃC���N���[�h����K�v���� */
#include <libgs.h>		/* for LIBGS :
				   �O���t�B�b�N���C�u���� ���g�����߂�
				   �\���̂Ȃǂ���`����Ă��� */

#define OBJECTMAX 100		/* Max Objects :
                                   �RD�̃��f���͘_���I�ȃI�u�W�F�N�g��
                                   �������邱�̍ő吔 ���`���� */

#define TEX_ADDR   0x80010000	/* Top Address of texture data1 (TIM FORMAT) :
                                   �e�N�X�`���f�[�^�iTIM�t�H�[�}�b�g�j
				   ���������A�h���X */

#define MODEL_ADDR 0x80040000	/* Top Address of modeling data (TMD FORMAT) :
				   ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j
				   ���������A�h���X */


#define OT_LENGTH  10		/* Area of OT : �I�[�_�����O�e�[�u���̉𑜓x */


GsOT            Wot[2];		/* Handler of OT
                                   Uses 2 Hander for Dowble buffer :
				   �I�[�_�����O�e�[�u���n���h��
				   �_�u���o�b�t�@�̂��߂Q�K�v */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* Area of OT :
						�I�[�_�����O�e�[�u������ */

GsDOBJ5		object[OBJECTMAX]; /* Array of Object Handler :
				      �I�u�W�F�N�g�n���h��
				      �I�u�W�F�N�g�̐������K�v */

unsigned long   Objnum;		/* valibable of number of Objects :
				   ���f�����O�f�[�^�̃I�u�W�F�N�g�̐���
				   �ێ����� */


GsCOORDINATE2   DSpaceShattle,DSpaceHatchL,DSpaceHatchR,DSatt;
/* Coordinate for GsDOBJ2 : �I�u�W�F�N�g���Ƃ̍��W�n */

SVECTOR         PWorld,PSpaceShattle,PSpaceHatchL,PSpaceHatchR,PSatt;
/* work short vector for making Coordinate parmeter :
   �I�u�W�F�N�g���Ƃ̍��W�n����邽�߂̌��f�[�^ */

GsRVIEW2  view;			/* View Point Handler :
				   ���_��ݒ肷�邽�߂̍\���� */
GsF_LIGHT pslt[3];		/* Flat Lighting Handler :
				   ���s������ݒ肷�邽�߂̍\���� */
unsigned long padd;		/* Controler data :
				   �R���g���[���̃f�[�^��ێ����� */

u_long          preset_p[0x10000];

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ5 *op;			/* pointer of Object Handler :
				   �I�u�W�F�N�g�n���h���ւ̃|�C���^ */
  int     outbuf_idx;
  MATRIX  tmpls;
  
  ResetCallback();
  init_all();

  GsInitVcount();
  while(1)
    {
      if(obj_interactive()==0)
        return 0;	/* interactive parameter get ;
			   �p�b�h�f�[�^���瓮���̃p�����[�^������ */
      GsSetRefView2(&view);	/* Calculate World-Screen Matrix :
				   ���[���h�X�N���[���}�g���b�N�X�v�Z */
      outbuf_idx=GsGetActiveBuff();/* Get double buffer index :
				      �_�u���o�b�t�@�̂ǂ��炩�𓾂� */
      
      GsClearOt(0,0,&Wot[outbuf_idx]); /* Clear OT for using buffer :
					  �I�[�_�����O�e�[�u�����N���A���� */
      
      for(i=0,op=object;i<Objnum;i++)
	{
	  /* Calculate Local-World Matrix :
	     �X�N���[���^���[�J���}�g���b�N�X���v�Z���� */
	  GsGetLs(op->coord2,&tmpls);
	  /* Set LWMATRIX to GTE Lighting Registers :
	     ���C�g�}�g���b�N�X��GTE�ɃZ�b�g���� */
	  GsSetLightMatrix(&tmpls);
	  /* Set LSAMTRIX to GTE Registers :
	     �X�N���[���^���[�J���}�g���b�N�X��GTE�ɃZ�b�g���� */
	  GsSetLsMatrix(&tmpls);
	  /* Perspective Translate Object and Set OT :
	     �I�u�W�F�N�g�𓧎��ϊ����I�[�_�����O�e�[�u���ɓo�^���� */
	  GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,getScratchAddr(0));
  	  op++;
	}
      padd=PadRead(0);		/* Readint Control Pad data :
				   �p�b�h�̃f�[�^��ǂݍ��� */
      VSync(0);			/* Wait for VSYNC : V�u�����N��҂� */
      ResetGraph(1);		/* Reset GPU */
      GsSwapDispBuff();		/* Swap double buffer :
				   �_�u���o�b�t�@��ؑւ��� */
      /* Set SCREEN CLESR PACKET to top of OT :
	 ��ʂ̃N���A���I�[�_�����O�e�[�u���̍ŏ��ɓo�^���� */
      GsSortClear(0x0,0x0,0x0,&Wot[outbuf_idx]);
      /* Drawing OT :
	 �I�[�_�����O�e�[�u���ɓo�^����Ă���p�P�b�g�̕`����J�n���� */
      GsDrawOt(&Wot[outbuf_idx]);
    }
}

obj_interactive()
{
  SVECTOR v1;
  MATRIX  tmp1;
  
  /* rotate Y axis : �V���g����Y����]������ */
  if((padd & PADRleft)>0) PSpaceShattle.vy +=5*ONE/360;
  /* rotate Y axis : �V���g����Y����]������ */
  if((padd & PADRright)>0) PSpaceShattle.vy -=5*ONE/360;
  /* rotate X axis :�V���g����X����]������ */
  if((padd & PADRup)>0) PSpaceShattle.vx+=5*ONE/360;
  /* rotate X axis : �V���g����X����]������ */
  if((padd & PADRdown)>0) PSpaceShattle.vx-=5*ONE/360;
  
  /* transfer Z axis : �V���g����Z���ɂ����ē����� */
  if((padd & PADh)>0)      DSpaceShattle.coord.t[2]+=100;
  
  /* transfer Z axis : �V���g����Z���ɂ����ē����� */
  /* if((padd & PADk)>0)      DSpaceShattle.coord.t[2]-=100; */
  
  /* exit program :
     �v���O�������I�����ă��j�^�ɖ߂� */
  if((padd & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}
  
  
  /* transfer X axis : �V���g����X���ɂ����ē����� */
  if((padd & PADLleft)>0) DSpaceShattle.coord.t[0] -=10;
  /* transfer X axis : �V���g����X���ɂ����ē����� */
  if((padd & PADLright)>0) DSpaceShattle.coord.t[0] +=10;

  /* transfer Y axis : �V���g����Y���ɂ����ē����� */
  if((padd & PADLdown)>0) DSpaceShattle.coord.t[1]+=10;
  /* transfer Y axis : �V���g����Y���ɂ����ē����� */
  if((padd & PADLup)>0) DSpaceShattle.coord.t[1]-=10;
  
  if((padd & PADl)>0)
    {				/* open the hatch : �n�b�`���J�� */
      object[3].attribute &= 0x7fffffff;	/* exist the satellite :
						   �q���𑶍݂����� */
      /* set the rotate parameter of the right hatch along z axis :
	 �E�n�b�`��Z���ɉ�]�����邽�߂̃p�����[�^���Z�b�g���� */
      PSpaceHatchR.vz -= 2*ONE/360;
      /* set the rotate parameter of the left hatch along z axis :
	 ���n�b�`��Z���ɉ�]�����邽�߂̃p�����[�^���Z�b�g���� */      
      PSpaceHatchL.vz += 2*ONE/360;
      /* caliculate the matrix and set :
	 �E�n�b�`��Z���ɉ�]�����邽�߂Ƀp�����[�^����}�g���b�N�X���v�Z��
	 �E�n�b�`�̍��W�n�ɃZ�b�g���� */
      set_coordinate(&PSpaceHatchR,&DSpaceHatchR);
      /* caliculate the matrix and set :
	 ���n�b�`��Z���ɉ�]�����邽�߂Ƀp�����[�^����}�g���b�N�X���v�Z��
	 ���n�b�`�̍��W�n�ɃZ�b�g���� */
      set_coordinate(&PSpaceHatchL,&DSpaceHatchL);
    }
  if((padd & PADm)>0)
    {				/* close the hatch : �n�b�`���Ƃ��� */
      PSpaceHatchR.vz += 2*ONE/360;
      PSpaceHatchL.vz -= 2*ONE/360;
      set_coordinate(&PSpaceHatchR,&DSpaceHatchR);
      set_coordinate(&PSpaceHatchL,&DSpaceHatchL);
    }

  if((padd & PADn)>0 && (object[3].attribute & 0x80000000) == 0)
    {				/* transfer the satellite : �q���𑗏o���� */
      DSatt.coord.t[1] -= 10;	/* translation parameter set :
				   �q����X���ɉ����ē������p�����[�^�Z�b�g */
      /* rotation parameter set :
	 �q����Y���ɉ����ĉ�]�����邽�߂̃p�����[�^�Z�b�g */
      PSatt.vy += 2*ONE/360;
      /* set the matrix to the coordinate from parameters :
	 �p�����[�^����}�g���b�N�X���v�Z�����W�n�ɃZ�b�g */
      set_coordinate(&PSatt,&DSatt);
    }
  if((padd & PADo)>0 && (object[3].attribute & 0x80000000) == 0)
    {				/* transfer the satellite : �q����������� */
      DSatt.coord.t[1] += 10;
      PSatt.vy -= 2*ONE/360;
      set_coordinate(&PSatt,&DSatt);
    }
  /* set the matrix to the coordinate from the parameters of the shuttle :
     �V���g���̃p�����[�^����}�g���b�N�X���v�Z�����W�n�ɃZ�b�g */
  set_coordinate(&PSpaceShattle,&DSpaceShattle);
  return 1;
}


init_all()			/* Initialize routine : ���������[�`���Q */
{
  ResetGraph(0);		/* reset GPU */
  PadInit(0);			/* Reset Controller : �R���g���[�������� */
  padd=0;			/* controller value initialize :
				   �R���g���[���l������ */
  GsInitGraph(640,480,2,1,0);	/* rezolution set , non interrace mode :
				   �𑜓x�ݒ�i�C���^�[���[�X���[�h�j */
  GsDefDispBuff(0,0,0,0);	/* Double buffer setting :
				   �_�u���o�b�t�@�w�� */

/*  GsInitGraph(640,240,1,1,0);	/* rezolution set , non-interrace mode :
    �𑜓x�ݒ�i�m���C���^�[���[�X���[�h�j */
/*  GsDefDispBuff(0,0,0,240);	/* Double buffer setting :�_�u���o�b�t�@�w�� */

  GsInit3D();			/* Init 3D system : �RD�V�X�e�������� */
  
  Wot[0].length=OT_LENGTH;	/* Set bit length of OT handler :
				   �I�[�_�����O�e�[�u���n���h���ɉ𑜓x�ݒ� */
  Wot[0].org=zsorttable[0];	/* Set Top address of OT Area to OT handler :
				   �I�[�_�����O�e�[�u���n���h����
				   �I�[�_�����O�e�[�u���̎��̐ݒ� */
  /* same setting for anoter OT handler :
     �_�u���o�b�t�@�̂��߂�������ɂ������ݒ� */
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  coord_init();			/* Init coordinate : ���W��` */
  model_init();			/* Reading modeling data :
				   ���f�����O�f�[�^�ǂݍ��� */
  view_init();			/* Setting view point : ���_�ݒ� */
  light_init();			/* Setting Flat Light : ���s�����ݒ� */
  /*
  texture_init(TEX_ADDR);
  */
}

view_init()			/* Setting view point : ���_�ݒ� */
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/* Set projection : �v���W�F�N�V�����ݒ� */
  
  /* Setting view point location : ���_�p�����[�^�ݒ� */
  view.vpx = 0; view.vpy = 0; view.vpz = -2000;
  /* Setting focus point location : �����_�p�����[�^�ݒ� */
  view.vrx = 0; view.vry = 0; view.vrz = 0;
  /* Setting bank of SCREEN : ���_�̔P��p�����[�^�ݒ� */
  view.rz=0;
  /* Setting parent of viewing coordinate : ���_���W�p�����[�^�ݒ� */  
  view.super = WORLD;
  /* view.super = &DSatt; */
  
  /* Calculate World-Screen Matrix from viewing paramter :  
     ���_�p�����[�^���Q���王�_��ݒ肷��
     ���[���h�X�N���[���}�g���b�N�X���v�Z���� */
  GsSetRefView2(&view);
}


light_init()			/* init Flat light : ���s�����ݒ� */
{
  /* Setting Light ID 0 : ���C�gID�O �ݒ� */  
  /* Setting direction vector of Light0 :
     ���s���������p�����[�^�ݒ� */
  pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
  /* Setting color of Light0 : ���s�����F�p�����[�^�ݒ� */
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  /* Set Light0 from parameters : �����p�����[�^��������ݒ� */
  GsSetFlatLight(0,&pslt[0]);
  
  /* Setting Light ID 1 : ���C�gID�P �ݒ� */
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /* Setting Light ID 2 : ���C�gID�Q �ݒ� */
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
  pslt[2].r=0x60; pslt[2].g=0x60; pslt[2].b=0x60;
  GsSetFlatLight(2,&pslt[2]);
  
  /* Setting Ambient : �A���r�G���g�ݒ� */
  GsSetAmbient(0,0,0);
  
  /* Setting default light mode : �����v�Z�̃f�t�H���g�̕����ݒ� */  
  GsSetLightMode(0);
}

coord_init()			/* Setting coordinate : ���W�n�ݒ� */
{
  /* Setting hierarchy of Coordinate : ���W�̒�` */  
  /* SpaceShuttle's coordinate connect the WORLD coordinate directly :
     SpaceShattle���W�n�̓��[���h�ɒ��ɂԂ牺���� */
  GsInitCoordinate2(WORLD,&DSpaceShattle);
  /* the right hatch's coordinate connects the shuttle's :
     �n�b�`�E�̍��W�n�̓X�y�[�X�V���g���ɂԂ牺���� */
  GsInitCoordinate2(&DSpaceShattle,&DSpaceHatchL);
  /* the left hatch's coordinate connects the shuttle's :
     �n�b�`���̍��W�n�̓X�y�[�X�V���g���ɂԂ牺���� */
  GsInitCoordinate2(&DSpaceShattle,&DSpaceHatchR);
  /* the satellite's coordinate connects the shuttle's :
     �q���̍��W�n�̓X�y�[�X�V���g���ɂԂ牺���� */
  GsInitCoordinate2(&DSpaceShattle,&DSatt);
  
  /* offset the hatch's orign point to the edge of the shuttle :
     �n�b�`�̌��_���X�y�[�X�V���g���̂ւ�Ɏ����Ă��� */
  DSpaceHatchL.coord.t[0] =  356;
  DSpaceHatchR.coord.t[0] = -356;
  DSpaceHatchL.coord.t[1] = 34;
  DSpaceHatchR.coord.t[1] = 34;
  
  /* offset the satellite's orign point 20 from the orign point of the shuttle:
     �q���̌��_���X�y�[�X�V���g���̌��_����Y���ɂQ�O���炷 */
  DSatt.coord.t[1] = 20;
  
  /* Init work vector : �}�g���b�N�X�v�Z���[�N�̃��[�e�[�V�����x�N�^�[������ */
  PWorld.vx=PWorld.vy=PWorld.vz=0;
  PSatt = PSpaceHatchR = PSpaceHatchL = PSpaceShattle = PWorld;
  /* the org point of DWold is set to Z = 4000 :
     �X�y�[�X�V���g���̌��_�����[���h��Z = 4000�ɐݒ� */
  DSpaceShattle.coord.t[2] = 4000;
}

/* Set coordinte parameter from work vector :
   �}�g���b�N�X�v�Z���[�N����}�g���b�N�X���쐬�����W�n�ɃZ�b�g���� */
set_coordinate(pos,coor)
SVECTOR *pos;			/* work vector : ���[�e�V�����x�N�^ */
GsCOORDINATE2 *coor;		/* Coordinate : ���W�n */
{
  MATRIX tmp1;
  
  /* Set translation : ���s�ړ����Z�b�g���� */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  
  /* Rotate Matrix :
     �}�g���b�N�X�Ƀ��[�e�[�V�����x�N�^����p������ */
  RotMatrix(pos,&tmp1);
  
  /* Set Matrix to Coordinate :
     ���߂��}�g���b�N�X�����W�n�ɃZ�b�g���� */
  coor->coord = tmp1;

  /* Clear flag becase of changing parameter :
     �}�g���b�N�X�L���b�V�����t���b�V������ */
  coor->flg = 0;
}


/* Load texture to VRAM : �e�N�X�`���f�[�^��VRAM�Ƀ��[�h���� */
texture_init(addr)
unsigned long addr;
{
  RECT rect1;
  GsIMAGE tim1;
  
  /* Get texture information of TIM FORMAT :
     TIM�f�[�^�̃w�b�_����e�N�X�`���̃f�[�^�^�C�v�̏��𓾂� */  
  GsGetTimInfo((unsigned long *)(addr+4),&tim1);
  rect1.x=tim1.px;		/* X point of image data on VRAM :
				   �e�N�X�`�������VRAM�ł�X���W */
  rect1.y=tim1.py;		/* Y point of image data on VRAM :
				   �e�N�X�`�������VRAM�ł�Y���W */
  rect1.w=tim1.pw;		/* Width of image : �e�N�X�`���� */
  rect1.h=tim1.ph;		/* Height of image : �e�N�X�`������ */
  
  /* Load texture to VRAM : VRAM�Ƀe�N�X�`�������[�h���� */
  LoadImage(&rect1,tim1.pixel);
  
  /* Exist Color Lookup Table : �J���[���b�N�A�b�v�e�[�u�������݂��� */  
  if((tim1.pmode>>3)&0x01)
    {
      rect1.x=tim1.cx;		/* X point of CLUT data on VRAM :
				   �N���b�g�����VRAM�ł�X���W */
      rect1.y=tim1.cy;		/* Y point of CLUT data on VRAM :
				   �N���b�g�����VRAM�ł�Y���W */
      rect1.w=tim1.cw;		/* Width of CLUT : �N���b�g�̕� */
      rect1.h=tim1.ch;		/* Height of CLUT : �N���b�g�̍��� */

      /* Load CULT data to VRAM : VRAM�ɃN���b�g�����[�h���� */
      LoadImage(&rect1,tim1.clut);
    }
}

/* Read modeling data (TMD FORMAT) : ���f�����O�f�[�^�̓ǂݍ��� */
model_init()
{
  unsigned long *dop;
  GsDOBJ5 *objp;		/* handler of object :
				   ���f�����O�f�[�^�n���h�� */
  int i;
  u_long *oppp;
  
  dop=(u_long *)MODEL_ADDR;	/* Top Address of MODELING DATA(TMD FORMAT) :
				   ���f�����O�f�[�^���i�[����Ă���A�h���X */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* Mappipng real address :
				   ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j��
				   ���A�h���X�Ƀ}�b�v���� */
  dop++;
  Objnum = *dop;		/* Get number of Objects :
				   �I�u�W�F�N�g����TMD�̃w�b�_���瓾�� */
  dop++;			/* Adjusting for GsLinkObject4 :
				   GsLinkObject2�Ń����N���邽�߂�TMD��
				   �I�u�W�F�N�g�̐擪�ɂ����Ă��� */
  /* Link ObjectHandler and TMD FORMAT MODELING DATA :
     TMD�f�[�^�ƃI�u�W�F�N�g�n���h����ڑ����� */
  for(i=0;i<Objnum;i++)		
    GsLinkObject5((unsigned long)dop,&object[i],i);
  
  oppp = preset_p;
  for(i=0,objp=object;i<Objnum;i++)
    {
      /* Set Coordinate of Object Handler :
	 �f�t�H���g�̃I�u�W�F�N�g�̍��W�n�̐ݒ� */
      objp->coord2 =  &DSpaceShattle;
				/* default object attribute (not display):
				   �f�t�H���g�̃I�u�W�F�N�g�̃A�g���r���[�g
				   �̐ݒ�i�\�����Ȃ��j */
      objp->attribute = GsDOFF;
      oppp = GsPresetObject(objp,oppp);
      objp++;
    }
  
  object[0].attribute &= ~GsDOFF;
  
  object[0].attribute &= ~GsDOFF;	/* display on : �\���I�� */
  object[0].coord2    = &DSpaceShattle;	/* set the shuttle coordinate :
					   �X�y�[�X�V���g���̍��W�ɐݒ� */
  object[1].attribute &= ~GsDOFF;	/* display on : �\���I�� */
  object[1].attribute |= GsALON;	/* semi-transparent : ������ */
  object[1].coord2    = &DSpaceHatchR;  /* set the right hatch coordinate :
					   �n�b�`�E�̍��W�ɐݒ� */
  object[2].attribute &= ~GsDOFF;	/* display on :�\���I�� */
  object[2].attribute |= GsALON;	/* semi-transparent : ������ */
  object[2].coord2    = &DSpaceHatchL;  /* set the left hatch coordiante :
					   �n�b�`���̍��W�ɐݒ� */
  
  object[3].coord2    = &DSatt;	/* set the satellite coordinate :
				   �q���̍��W�ɐݒ� */
}
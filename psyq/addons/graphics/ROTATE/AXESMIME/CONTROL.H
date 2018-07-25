/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *		
 *		"control.h"
*/

#define CTLMAX 	10	/* Maximum number of control waveform channels :
			   ����g�`�̍ő�`�����l���� */

#define CTLTIME 32	/* Maximum length of transfer function waveform :
			   �`�B�֐��g�`�̍ő咷�� */

/* Control waveform-generating program structure :
   ����g�`�����v���O�����p�\���� */
typedef struct {
	int  *cnv;	/* Transfer function waveform data pointer :
			   �`�B�֐��g�`�f�[�^�̃|�C���^ */

	int num;	/* Transfer function waveform data length :
			   �`�B�֐��g�`�f�[�^�̒��� */

	int in;		/* Input : ���� */
	int out;	/* Output : �o�� */

} CTLFUNC;

extern CTLFUNC ctlfc[];
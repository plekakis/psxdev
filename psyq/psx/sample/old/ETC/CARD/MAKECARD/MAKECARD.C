/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*****************************************************************
 *
 * file: makecard.c
 *
 * 	Copyright (C) 1994 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include "../max/cardio.h"
#include <sys/types.h>
#include <asm.h>
#include <libapi.h>
#include <sys/file.h>

#include "hand1.h"
#include "hand2.h"
#include "hand3.h"

typedef	struct {
    long	id;
    long	flag;
    long	cbnum;
    short	cx;
    short	cy;
    short	cw;
    short	ch;
    char	clut[32];
    long	pbnum;
    short	px;
    short	py;
    short	pw;
    short	ph;
    char	image[2];
} _TIM4;

_CARD	HEAD;

unsigned char	buf[122880];

void MakeBackupCard( char* fname, int type, char* name, u_long* clut,
		    u_long* image1, u_long* image2, u_long* image3,
		    long length );

void	main( void )
{
    ResetCallback();

    InitCARD(1);
    StartCARD();
    _bu_init();
    _card_auto(1);
    
    MakeBackupCard( "bu10:hand", 0x13, "じゃんけん　ぽん　ぐー、ちょき、ぱー",
		   (u_long *)(((_TIM4*)hand1)->clut),
		   (u_long *)(((_TIM4*)hand1)->image),
		   (u_long *)(((_TIM4*)hand2)->image),
		   (u_long *)(((_TIM4*)hand3)->image), (long)8192 );

    StopCallback();
}

void MakeBackupCard(char* fnam, int type, char* name, u_long* clut,
		    u_long* image1, u_long* image2, u_long* image3,
		    long length )
{
    long  fd;
    long  totalbytes;
    long  totalblock;
    _CARD cardinf;

    int i, j;
    unsigned char *cp;

    for (i = 0; i < 256; i++)
      for (j = 0; j < 480; j++)
	buf[i*480+j] = i;

    totalbytes = ((length/8192) * 8192) + (length%8192 ? 8192:0 );

    HEAD.Magic[0] = 'S';
    HEAD.Magic[1] = 'C';
    HEAD.Type = type;
    HEAD.BlockEntry = ( totalbytes/8192  )+( totalbytes%8192 ? 1:0 );
    strcpy( HEAD.Title, name );
    memcpy( HEAD.Clut, clut, 32 );
    if (type == 0x13) {
	memcpy( HEAD.Icon[0], image1, 128);
	memcpy( HEAD.Icon[1], image2, 128);
	memcpy( HEAD.Icon[2], image3, 128);
    } else {
	memcpy( HEAD.Icon[0], image1, 128 );
	memcpy( HEAD.Icon[1], image1, 128 );
	memcpy( HEAD.Icon[2], image1, 128 );
    }
    
    memcpy( buf, &HEAD, sizeof( HEAD ));
    
    if ((fd = open( fnam,  O_CREAT | HEAD.BlockEntry << 16 )) == -1)
      printf("%s open error.\n", fnam);
    printf( "Discript: %d\n", fd );
    close( fd );
    
    if ((fd = open( fnam,  O_WRONLY )) == -1)
      printf("%s open error.\n", fnam);
    if ((i = write( fd, buf, totalbytes )) != totalbytes)
      printf("write error. %d, %d\n", i, totalbytes);
    close( fd );
    
    if ((fd = open( fnam,  O_RDONLY )) == -1)
      printf("%s open error.\n", fnam);
    if ((i = read( fd, (char*)&cardinf, sizeof( _CARD ))) != sizeof(_CARD))
      printf("read error.\n");
    close( fd );
    
}


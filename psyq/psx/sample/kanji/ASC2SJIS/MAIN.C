/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*****************************************************************
 *
 * file: main.c
 *
 *      Copyright (C) 1994 by Sony Computer Entertainment Inc.
 *                                        All Rights Reserved.
 *
 *      Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include <stdio.h>

main()
{
    int i;
    unsigned char ascii_code;
    unsigned short sjis_code;
    char *strings;

    for( i=0x20; i<0x7f ; i++ ) {
    	ascii_code = i;

	sjis_code = ascii2sjis(ascii_code);

	if(sjis_code)
	    printf("ASCII Character : %c -> Shift-JIS code : 0x%x\n",
	    	ascii_code, sjis_code);
    }

    return;
}

/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _MAXPROTO_H_
#define _MAXPROTO_H_

/*****************************************************************
 *
 * file: maxproto.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

/* pad.c */
int  pad_read( void );
void ShellOpenCheck( void );

/* maxinit.c */
void max_init( DB *db );
void init_prim( DB *db );

/* max.c */
void max( void );
int  ps_logo( int fadeflag );
void LED( unsigned char x );
int  bootVersion( void );
void mmemcpy( char *dst, char *src, int size );

/* drawmax */
void do_max_cube( DB *db, DB **cdb, int nonstop );
void DrawDB( DB *cdb );

/* menuset.c */
void SetUpPageItem( int sel );

#endif /* _MAXPROTO_H_ */
/* DON'T ADD STUFF AFTER THIS */

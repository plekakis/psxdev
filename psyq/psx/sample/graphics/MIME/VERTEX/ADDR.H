/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	mime
 *
 *	"addr.h" memory allocation
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
 **/

/* Top Address of modeling data (TMD format) */
#define MODEL_ADDR (u_long *)0x800d0000

/* Top Address of reserved vertex data  */
#define ORGVTXBUF (u_long *)0x80050000	

/* Top Address of reserved normal data */	
#define ORGNRMBUF (u_long *)0x80060000	

/* Top Address of vertex differencial data (VDF format) */
#define MDFDATAVTX (u_long *)0x80090000	

/* Top Address of normal differencial data (NDF format) */
#define MDFDATANRM (u_long *)0x800b0000	

/* Top Address of packet data */
#define PACKETBUF  (u_long *)0x80100000

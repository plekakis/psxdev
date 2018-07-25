/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	paddef.h
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#define A_PADo		PADo
#define A_PADm		PADm
#define A_PADn		PADn
#define A_PADl		PADl
#define A_PADRup	PADRup
#define A_PADRright	PADRright
#define A_PADRdown	PADRdown
#define A_PADRleft	PADRleft
#define A_PADk		PADk
#define A_PADi		PADi
#define A_PADj		PADj
#define A_PADh		PADh
#define A_PADLup	PADLup
#define A_PADLright	PADLright
#define A_PADLdown	PADLdown
#define A_PADLleft	PADLleft
#define A_PADL2 	A_PADo
#define A_PADR2 	A_PADm
#define A_PADL1 	A_PADn 
#define A_PADR1 	A_PADl
#define A_PADselect 	A_PADk
#define A_PADstart 	A_PADh
#define A_PADRcross	(A_PADRup|A_PADRright|A_PADRdown|A_PADRleft)
#define A_PADLcross	(A_PADLup|A_PADLright|A_PADLdown|A_PADLleft)
#define A_PADL12	(A_PADL1|A_PADL2)
#define A_PADR12	(A_PADR1|A_PADR2)
#define A_PADLR		(A_PADL1|A_PADL2|A_PADR1|A_PADR2)

#define B_PADo		(A_PADo<<16)
#define B_PADm		(A_PADm<<16)
#define B_PADn		(A_PADn<<16)
#define B_PADl		(A_PADl<<16)
#define B_PADRup	(A_PADRup<<16)
#define B_PADRright	(A_PADRright<<16)
#define B_PADRdown	(A_PADRdown<<16)
#define B_PADRleft	(A_PADRleft<<16)
#define B_PADk		(A_PADk<<16)
#define B_PADi		(A_PADi<<16)
#define B_PADj		(A_PADj<<16)
#define B_PADh		(A_PADh<<16)
#define B_PADLup	(A_PADLup<<16)
#define B_PADLright	(A_PADLright<<16)
#define B_PADLdown	(A_PADLdown<<16)
#define B_PADLleft	(A_PADLleft<<16)
#define B_PADL2		B_PADo
#define B_PADR2		B_PADm
#define B_PADL1		B_PADn 
#define B_PADR1		B_PADl
#define B_PADselect	B_PADk
#define B_PADstart	B_PADh
#define B_PADRcross	(B_PADRup|B_PADRright|B_PADRdown|B_PADRleft)
#define B_PADLcross	(B_PADLup|B_PADLright|B_PADLdown|B_PADLleft)
#define B_PADL12	(B_PADL1|B_PADL2)
#define B_PADR12	(B_PADR1|B_PADR2)
#define B_PADLR		(B_PADL1|B_PADL2|B_PADR1|B_PADR2)

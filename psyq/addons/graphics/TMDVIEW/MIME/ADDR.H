/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *	mime
 *
 *	"addr.h" メモリ割り当て
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
 */

/* Top Address of modeling data (TMD format) :
   モデリングデータ（TMDフォーマット） がおかれるアドレス */
#define MODEL_ADDR (u_long *)0x800d0000

/* Top Address of reserved vertex data  :
   オリジナルの頂点データが退避されるアドレス */
#define ORGVTXBUF (u_long *)0x80050000	

/* Top Address of reserved normal data :
   オリジナルの法線データが退避されるアドレス */	
#define ORGNRMBUF (u_long *)0x80060000	

/* Top Address of vertex differencial data (VDF format) :
   頂点差分データ（VDFフォーマット） がおかれるアドレス */
#define MDFDATAVTX (u_long *)0x80090000	

/* Top Address of normal differencial data (NDF format) :
   法線差分データ（NDFフォーマット） がおかれるアドレス */
#define MDFDATANRM (u_long *)0x800b0000	

/* Top Address of packet data :
   パケットデータを置くアドレス */
#define PACKETBUF  (u_long *)0x80100000

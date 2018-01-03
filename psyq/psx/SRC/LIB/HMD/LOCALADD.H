/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#define gte_ldlvl_0( r0 ) __asm__ volatile (			\
	"mtc2	%0, $8"					\
	:							\
	: "r"( r0 ) )

#define gte_ldlvl_1( r0 ) __asm__ volatile (			\
	"mtc2	%0, $9"					\
	:							\
	: "r"( r0 ) )

#define gte_ldlvl_2( r0 ) __asm__ volatile (			\
	"mtc2	%0, $10"					\
	:							\
	: "r"( r0 ) )
#define gte_ldlvl_3( r0 ) __asm__ volatile (			\
	"mtc2	%0, $11"					\
	:							\
	: "r"( r0 ) )

#define gte_stmac_1() ({ u_long __value;\
	__asm__ volatile (			\
	".set noreorder;"			\
	"mfc2	%0, $25;"					\
	"nop;"					\
	".set reorder"				\
	: "=r"( __value ) :);\
	__value;})

#define gte_stmac_2() ({ u_long __value;\
	__asm__ volatile (			\
	".set noreorder;"			\
	"mfc2	%0, $26;"					\
	"nop;"					\
	".set reorder"				\
	: "=r"( __value ) :);\
	__value;})

#define gte_stmac_3() ({ u_long __value;\
	__asm__ volatile (			\
	".set noreorder;"			\
	"mfc2	%0, $27;"					\
	"nop;"					\
	".set reorder"				\
	: "=r"( __value ) :);\
	__value;})


#define gte_stlvl_1() ({ u_long __value;\
	__asm__ volatile (			\
	".set noreorder;"			\
	"mfc2	%0, $9;"					\
	"nop;"					\
	".set reorder"				\
	: "=r"( __value ) :);\
	__value;})

#define gte_stlvl_2() ({ u_long __value;\
	__asm__ volatile (			\
	".set noreorder;"			\
	"mfc2	%0, $10;"					\
	"nop;"					\
	".set reorder"				\
	: "=r"( __value ) :);\
	__value;})

#define gte_stlvl_3() ({ u_long __value;\
	__asm__ volatile (			\
	".set noreorder;"			\
	"mfc2	%0, $11;"					\
	"nop;"					\
	".set reorder"				\
	: "=r"( __value ) :);\
	__value;})


#define gte_lzc_nom( r0 ) ({ u_long __value;\
	__asm__ volatile (			\
	".set noreorder;"			\
	"mtc2	%1, $30;"			\
	"nop;"					\
	"nop;"					\
	"mfc2	%0, $31;"			\
	"nop;"					\
	".set reorder"				\
	: "=r"( __value ) \
	: "r"( r0 ) ); \
	__value;})

#define gte_square12_nom(r1,r2)					\
				{	gte_ldlvl(r1);		\
					gte_sqr12();		\
					gte_stlvnl(r2);	}	

typedef struct {
	long	vx;
	long	vy;
	long	vz;
} LVECTOR;

typedef struct {
    u_long *primtop;
    short size, primnum;
    u_long *mimediff[10000];
} MIME_PRIM;

typedef struct {
    short size, primnum;
    u_long *mimediff[10000];
} MIME_PRIM_ARG;

typedef struct {
    u_long *primtop;
    short size, primnum;
    u_long *mimediff[10000];
} RST_MIME_PRIM;

typedef struct {
    short size, primnum;
    u_long *mimediff[10000];
} RST_MIME_PRIM_ARG;

typedef struct {
    u_long headlen;
    long *mimepr;
    u_long mimenum;
    unsigned long mimeid;
    u_long *mimedifftop;
    u_long *orgtop;
    u_long *vtxtop;
    u_long *nrmtop;
} GsMIMeVNHeader;

typedef struct {
    u_long headlen;
    unsigned long mimeid;
    u_long *mimedifftop;
    u_long *orgtop;
    u_long *vtxtop;
    u_long *nrmtop;
} GsMIMeRstVNHeader;

typedef struct {
    u_long headlen;
    u_long coordtop;
    long *mimepr;
    u_long mimenum;
    unsigned long mimeid;
    u_long *mimedifftop;
} GsMIMeJntHeader;

typedef struct {
    u_long headlen;
    u_long coordtop;
    unsigned long mimeid;
    u_long *mimedifftop;
} GsMIMeRstJntHeader;

typedef struct {
    SVECTOR dv;
    LVECTOR dt;
} DIFFS_A0;

typedef struct {
    SVECTOR dv;
    LVECTOR dt;
} DIFFS_A1;

typedef struct {
    short m[3][3];
    short pad;
    LVECTOR dt;
} DIFFS_A2;

typedef struct {
    SVECTOR dv;
    LVECTOR dt;
} DIFFS_AR0;

typedef struct {
    short m[3][3];
    short pad;
    LVECTOR dt;
} DIFFS_AR1;

typedef struct {
    short m[3][3];
    short pad;
    LVECTOR dt;
} DIFFS_AR2;

typedef struct _MBLOCK_A0 {
    unsigned short coordid, diffnum;
    u_long dflags;
    DIFFS_A0 diffs[32];
} MBLOCK_A0;

typedef struct _MBLOCK_A1 {
    unsigned short coordid, diffnum;
    u_long dflags;
    DIFFS_A1 diffs[32];
} MBLOCK_A1;

typedef struct _MBLOCK_A2 {
    unsigned short coordid, diffnum;
    u_long dflags;
    DIFFS_A2 diffs[32];
} MBLOCK_A2;

typedef struct {
    SVECTOR *vstart;
    u_short pad, vnum;
    SVECTOR dv[32];
} DIFFS_B;

typedef struct _MBLOCK_B{
    u_short onum, diffnum;
    u_long dflags;
    DIFFS_B *diff_top[32]; /* diff_top[diffnum], changed[cnum] */
} MBLOCK_B;

typedef struct {
    SVECTOR *vstart;
    SVECTOR *ostart;
    u_short changed, vnum;
} DIFFS_BR;

typedef struct _MBLOCK_BR {
    unsigned short coordid, diffnum;
    DIFFS_BR diffs[10000];
} MBLOCK_BR;


#define add_offset(p,o) ((p)=(void *)((u_long)(o)+(4*(long)(p))))

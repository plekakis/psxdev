/*
** Main.h
   mike acton
*/


//#define PC_OK 

#define  schar    signed char
#define  sshort   signed short
#define  slong    signed long
#define  sint     signed int

#define  uchar    unsigned char
#define  ushort   unsigned short
#define  ulong    unsigned long
#define  uint     unsigned int

#define SCREEN_MAX_X 512
#define SCREEN_MAX_Y 240

extern uchar     major_version;
extern uchar     minor_version;

extern DRAWENV   draw[2];    
extern DISPENV   disp[2];
extern uchar     pageid;
extern ushort    pad[2];
extern ushort    dpad[2];
extern ushort    apad[2];
extern ushort    pad_count[2][32];
extern ulong     sys_ot[2];

void vswap(void);
void get_pad(void);
void clear_vram(void);
void draw_vram(void);

#define  qsin(a) q_sin(((a)&0x0fff))
#define  qcos(a) q_sin((((a)+0x0400)&0x0fff))

#define q_sin(a) \
( \
	(a>=0&&a<=1023)    ?(rsin_tbl[a]) \
	:(a>=1024&&a<=2047)?(rsin_tbl[2047-a]) \
	:(a>=2048&&a<=3071)?(-rsin_tbl[a-2048]) \
	:(-rsin_tbl[4095-a]) \
)

extern sshort rsin_tbl[]; /* from libgte.lib */

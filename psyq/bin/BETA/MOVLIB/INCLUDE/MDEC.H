//
//      Header file for MDEC compression library
//
//      (C) 1995, 1996, 1997 Sony Computer Entertainment Inc.
//      All Rights Reserved.
//

#ifndef _MDEC_H_
#define _MDEC_H_

typedef unsigned long u_long;
typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned int u_int;
typedef int				BOOL;

#ifndef max
#define max(a, b)	(((a) >= (b))? (a): (b))
#endif

#ifndef min
#define min(a, b)	(((a) <= (b))? (a): (b))
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// Structure for setting Q matrix and DCT
//
// contents of iq_y and iq_c should be zig-zag order
// e.g.
//	iq_y[i] = IQY[zscan[i]]
//	iq_c[i] = IQC[zscan[i]]
//	
//	IQY, IQC: Q matrix whose contents are pixel order
//	u_char zscan[64] = {
//		0 ,1 ,8 ,16,9 ,2 ,3 ,10,
//		17,24,32,25,18,11,4 ,5 ,
//		12,19,26,33,40,48,41,34,
//		27,20,13,6 ,7 ,14,21,28,
//		35,42,49,56,57,50,43,36,
//		29,22,15,23,30,37,44,51,
//		58,59,52,45,38,31,39,46,
//		53,60,61,54,47,55,62,63,
//	};
struct DECDCTENV {
	u_char	iq_y[64];	// IQ (Y): zig-zag order
	u_char	iq_c[64];	// IQ (Cb,Cr): zig-zag order
	short	dct[64];	// IDCT coef (currently not available)
};

/////////////////////////
// Format
/////////////////////////
enum Format {
	RGBformat,
	YUVformat
};

/////////////////////////
// functions
/////////////////////////

BOOL MdecBegin();
	// Initializes library
	// return value: TRUE if succeeded

BOOL MdecEnd();
	// Terminates library
	// return value: TRUE if succeeded

void MdecPutEnv(DECDCTENV *env);
	// Changes Q matrix and DCT matrix
	// DCT matrix cannot be changed with this version

void MdecPutEnvDefault();
	// Sets default Q matrix

void MdecGetEnv(DECDCTENV *env);
	// Get Q matrix

u_long MdecEncode(
	u_char *pix,			// image to be compressed (RGB or YUV)
	Format format,			// image format (RGBformat or YUVformat)
	u_short width,			// width of the image
	u_short height,			// height of the image
	u_long *bs,				// buffer for bs data
	u_long bsBuffSize,		// buffer size for bs data (unit: long word)
	u_long bsLimitSize,		// maximum size for bs data (unit: byte)
	double fullness,		// fullness (0.0-1.0) ... see below
	u_short initQuant,		// initial quant value (1 - 63)
	u_short *finalQuant		// final quant value retured
);
	// Encodes one image
	// with specifing size of encoded data(bs data).
	// return value: size of encoded data (byte)
	// This function try to encode image data into the size shown below.
	//		fullness * bsLimitSize <= size <= bsLimitSize

u_long MdecEncode(
	u_char *pix,			// image to be compressed (RGB=8:8:8)
	Format format,			// image format
	u_short width,			// width of the image
	u_short height,			// height of the image
	u_long *bs,				// buffer for bs data
	u_long bsBuffSize,		// buffer size for bs data (unit: long word)
	u_short quant			// initial quant value (1 - 63)
);
	// Encodes one image
	// with specifing quant value.
	// return value: size of encoded data (byte)

#endif /* _MDEC_H_ */

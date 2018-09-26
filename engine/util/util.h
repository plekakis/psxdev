#ifndef UTIL_H_DEF
#define UTIL_H_DEF

#include "../engine.h"

// Color macros & functions
#define PACK_RGB(r, g, b) ( ( (r) << 16) | ( (g) << 8) | (b) )
#define UNPACK_RGB(v, r, g, b) { *(r) = ( (v) >> 16) & 0xff; *(g) = ( (v) >> 8) & 0xff; *(b) = (v) & 0xff; }

// Array macros & functions
#define ARRAY_SIZE(x) sizeof( (x) ) / sizeof( (x)[0] )

// Pointer macros & functions
#define ALIGN_PTR(ptr, alignment) ((void*)( ((uint32)(ptr) + ((alignment) - 1)) & ~((alignment) - 1)));

// Bit macros & functions
uint8 Util_CountBits8(uint8 i_bitmask);
uint8 Util_CountBits16(uint16 i_bitmask);
uint8 Util_CountBits32(uint32 i_bitmask);

// Some macros & functions potentially missing from GTE lib
#define setColor(c, _r, _g, _b) \
	(c)->r = (_r), (c)->g = (_g), (c)->b = (_b)

#endif // UTIL_H_DEF

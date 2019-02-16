#ifndef UTIL_H_DEF
#define UTIL_H_DEF

#include "../engine.h"

// Color macros & functions
#define PACK_RGB(r, g, b) ( ( (r) << 16) | ( (g) << 8) | (b) )
#define UNPACK_RGB(v, r, g, b) { *(r) = ( (v) >> 16) & 0xff; *(g) = ( (v) >> 8) & 0xff; *(b) = (v) & 0xff; }

// Array macros & functions
#define ARRAY_SIZE(x) sizeof( (x) ) / sizeof( (x)[0] )

// Pointer macros & functions
uint8* Util_AlignPtr(uint8* i_ptr, uint32 i_alignment);

void Util_MemZero(void* i_ptr, uint32 i_size);

// Bit macros & functions
uint8 Util_CountBits8(uint8 i_bitmask);
uint8 Util_CountBits16(uint16 i_bitmask);
uint8 Util_CountBits32(uint32 i_bitmask);

// Random number functions
#define RAND_MAX (0x7fff)

#define RAND_RANGE(type, min, max) (rand() % ((max) + 1 - (min)) + (min))

// Some macros & functions potentially missing from GTE lib
#define setColor(c, _r, _g, _b) \
	(c)->r = (_r), (c)->g = (_g), (c)->b = (_b)

#endif // UTIL_H_DEF

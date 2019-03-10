#ifndef UTIL_H_DEF
#define UTIL_H_DEF

#include "../engine.h"

//-----------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(a, b, v) (MAX((a), MIN((v), (b))) )

//-----------------------------------------------------------------------
#define PACK_RGB(r, g, b) ( ( (r) << 16) | ( (g) << 8) | (b) )
#define UNPACK_RGB(v, r, g, b) { *(r) = ( (v) >> 16) & 0xff; *(g) = ( (v) >> 8) & 0xff; *(b) = (v) & 0xff; }

//-----------------------------------------------------------------------
#define ARRAY_SIZE(x) sizeof( (x) ) / sizeof( (x)[0] )
#define IS_POW2(x) ( (((x) & ~((x)-1))==(x))? (x) : 0)

//-----------------------------------------------------------------------
#define sgn(x) ((x) > 0) ? 1 : ( ((x) < 0) ? -1 : 0)

//-----------------------------------------------------------------------
#define RAND_MAX (0x7fff)
#define RAND_RANGE(type, min, max) (rand() % ((max) + 1 - (min)) + (min))

//-----------------------------------------------------------------------
#define setColor(c, _r, _g, _b) \
	(c)->r = (_r), (c)->g = (_g), (c)->b = (_b)

#define mulVector(v0, v1) \
	(v0)->vx *= (v1)->vx,	\
	(v0)->vy *= (v1)->vy,	\
	(v0)->vz *= (v1)->vz	

#define vectorDot(v0, v1) ((v0)->vx * (v1)->vx + (v0)->vy * (v1)->vy + (v0)->vz * (v1)->vz)
#define vectorLength(v0) vectorDot(v0, v0)

//-----------------------------------------------------------------------
#define OFFSET_OF(s,m) (uint32)&(((s *)0)->m)

//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
uint8* Util_AlignPtr(uint8* i_ptr, uint32 i_alignment);
uint32 Util_AlignUp(uint32 i_value, uint32 i_alignment);
void Util_MemZero(void* i_ptr, uint32 i_size);

//-----------------------------------------------------------------------
uint8 Util_CountBits8(uint8 i_bitmask);
uint8 Util_CountBits16(uint16 i_bitmask);
uint8 Util_CountBits32(uint32 i_bitmask);

//-----------------------------------------------------------------------
StringId Util_HashString(const char* i_string);
#define ID(x) Util_HashString((x))

#endif // UTIL_H_DEF

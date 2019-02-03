#include "util.h"

///////////////////////////////////////////////////
uint8* Util_AlignPtr(uint8* i_ptr, uint32 i_alignment)
{
	uint32 alignmentMinus1 = i_alignment - 1;
	return (uint8*)( ((uint32)i_ptr + alignmentMinus1) & ~alignmentMinus1);
}

///////////////////////////////////////////////////
#define COUNT_BITS(type, length, bitmask) \
	type pos = 0u;\
	uint8 count = 0u;\
	for (pos = 0; pos < length; ++pos)\
	{\
		count = count + (uint8)((bitmask) & 1);\
		(bitmask) = (bitmask) >> 1u;\
	}\
	return count;

///////////////////////////////////////////////////
uint8 Util_CountBits8(uint8 i_bitmask)
{
	COUNT_BITS(uint32, 8, i_bitmask)
}

///////////////////////////////////////////////////
uint8 Util_CountBits16(uint16 i_bitmask)
{
	COUNT_BITS(uint16, 16, i_bitmask)
}

///////////////////////////////////////////////////
uint8 Util_CountBits32(uint32 i_bitmask)
{
	COUNT_BITS(uint32, 32, i_bitmask)
}
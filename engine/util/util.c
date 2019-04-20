#include "util.h"

///////////////////////////////////////////////////
uint32 Util_AlignUp(uint32 i_value, uint32 i_alignment)
{
	{
		VERIFY_ASSERT(IS_POW2(i_alignment), "Util_AlignUp: Alignment must be a power of 2! Specified: %u", i_alignment);
	}
	{
		uint32 alignmentMinus1 = i_alignment - 1;
		return (i_value + alignmentMinus1) & ~alignmentMinus1;
	}
}

///////////////////////////////////////////////////
uint8* Util_AlignPtr(uint8* i_ptr, uint32 i_alignment)
{
	return (uint8*)Util_AlignUp((uint32)i_ptr, i_alignment);
}

///////////////////////////////////////////////////
uint32 Util_AlignPtrAdjustment(uint8* i_ptr, uint32 i_alignment)
{
	return Util_AlignUpAdjustment((uint32)i_ptr, i_alignment);
}

///////////////////////////////////////////////////
uint32 Util_AlignUpAdjustment(uint32 i_value, uint32 i_alignment)
{	
	{
		VERIFY_ASSERT(IS_POW2(i_alignment), "Util_AlignUpAdjustment: Alignment must be a power of 2! Specified: %u", i_alignment);
	}

	{
		uint32 adjustment = i_alignment - i_value & (i_alignment - 1);
		return (adjustment == i_alignment) ? 0u : adjustment;
	}
}

///////////////////////////////////////////////////
uint32 Util_AlignPtrAdjustmentHeader(uint8* i_ptr, uint32 i_alignment, uint32 i_headerSize)
{
	return Util_AlignUpAdjustmentHeader((uint32)i_ptr, i_alignment, i_headerSize);
}

///////////////////////////////////////////////////
uint32 Util_AlignUpAdjustmentHeader(uint32 i_value, uint32 i_alignment, uint32 i_headerSize)
{
	uint32 adjustment = Util_AlignUpAdjustment(i_value, i_alignment);
	uint32 neededSpace = i_headerSize;

	if (adjustment < neededSpace)
	{
		neededSpace -= adjustment;

		//Increase adjustment to fit header 
		adjustment += i_alignment * (neededSpace / i_alignment);

		if (neededSpace % i_alignment > 0)
		{
			adjustment += i_alignment;
		}
	}

	return adjustment;
}

///////////////////////////////////////////////////
void Util_MemZero(void* i_ptr, uint32 i_size)
{
	VERIFY_ASSERT(i_ptr, "Util_MemZero: Address cannot be null!");
	VERIFY_ASSERT(i_size, "util_MemZero: Size cannot be 0!");

	memset(i_ptr, 0, i_size);
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

///////////////////////////////////////////////////
StringId Util_HashString(const char* i_string)
{
	// djb2 implementation of string hashing
	// http://www.cse.yorku.ca/~oz/hash.html
	// Adopted for 16bit hashing

	uint32 hash = 5381u;
	char c;
	while (c = *i_string++)
	{
		hash = ((hash << 5u) + hash) + c;
	}
		
	return (hash ^ (hash >> 16)) & 0xffff;
}
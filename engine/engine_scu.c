#include "core/core_scu.c"
#include "gfx/gfx_scu.c"
#include "system/system_scu.c"
#include "input/input_scu.c"

// Utility functions

///////////////////////////////////////////////////
void* AlignPtr(void* i_ptr, uint32 i_alignment)
{
	return (void*)( ((uint32)i_ptr + (i_alignment - 1)) & ~(i_alignment - 1));
}

///////////////////////////////////////////////////
uint32 CountBits(uint32 i_bitmask)
{
	uint32 pos = 0u;
	uint32 count = 0u;
	for (pos=0; pos<32; ++pos)
	{
		if (i_bitmask & 1)
			count++;

		i_bitmask = i_bitmask >> 1u;
	}

	return count;
}
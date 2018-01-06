#include "core/core_scu.c"
#include "gfx/gfx_scu.c"
#include "system/system_scu.c"

// Utility functions
void* AlignPtr(void* i_ptr, uint32 i_alignment)
{
	return (void*)( ((uint32)i_ptr + (i_alignment - 1)) & ~(i_alignment - 1));
}
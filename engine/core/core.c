#include "core.h"

uint32 g_ram_start	= 0x80100000;
uint32 g_ram_size = 2 * 1024 * 1024;

///////////////////////////////////////////////////
int16 Core_Initialize(uint32 i_stackSizeInBytes, uint32 i_scratchSizeInBytes)
{
	int16 result = E_OK;

	EnterCriticalSection();
	InitHeap3((void*)g_ram_start, g_ram_size);
	ExitCriticalSection();
	
	result = Core_InitStack(CORE_STACKALLOC, i_stackSizeInBytes, 4);
	VERIFY_ASSERT(SUCCESS(result), "Core_Initialize: Unable to allocate memory for core stack allocator (requested: %u bytes)", i_stackSizeInBytes);

	result = Core_InitScratch(CORE_SCRATCHALLOC, i_scratchSizeInBytes, 4);
	VERIFY_ASSERT(SUCCESS(result), "Core_Initialize: Unable to allocate memory for core scratch allocator (requested: %u bytes)", i_scratchSizeInBytes);

	return E_OK;
}

///////////////////////////////////////////////////
int16 Core_Shutdown()
{
	Core_FreeScratch(&g_coreScratchAlloc);
	Core_FreeStack(&g_coreStackAlloc);
	return E_OK;
}
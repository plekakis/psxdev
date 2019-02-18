#include "core.h"

uint32 g_ram_start	= 0x80100000;
uint32 g_ram_size = 2 * 1024 * 1024;

///////////////////////////////////////////////////
int16 Core_Initialize(uint32 i_stackSizeInBytes, uint32 i_scratchSizeInBytes)
{
	EnterCriticalSection();
	InitHeap3((void*)g_ram_start, g_ram_size);
	ExitCriticalSection();

	Core_InitStack(&g_coreStackAlloc, i_stackSizeInBytes, 4);
	Core_InitScratch(&g_coreScratchAlloc, i_scratchSizeInBytes, 4);

	return E_OK;
}

///////////////////////////////////////////////////
int16 Core_Shutdown()
{
	Core_FreeScratch(&g_coreScratchAlloc);
	Core_FreeStack(&g_coreStackAlloc);
	return E_OK;
}
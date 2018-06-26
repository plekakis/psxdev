#include "core.h"

uint32 __ramsize    = 0x00200000; // force 2MB of RAM
uint32 __stacksize  = 0x00004000; // force 16KB of stack

uint32 g_ram_start	= 0x8001d800;
uint32 g_ram_end	= 0x801f8000;

///////////////////////////////////////////////////
int16 Core_Initialize()
{
	InitHeap3((void*)g_ram_start, (g_ram_end - g_ram_start));

	return E_OK;
}

///////////////////////////////////////////////////
int16 Core_Shutdown()
{
	return E_OK;
}
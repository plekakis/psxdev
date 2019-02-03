#include "core.h"

uint32 g_ram_start	= 0x80100000;
uint32 g_ram_size = 2 * 1024 * 1024;

///////////////////////////////////////////////////
int16 Core_Initialize()
{
	EnterCriticalSection();
	InitHeap3((void*)g_ram_start, g_ram_size);
	ExitCriticalSection();

	return E_OK;
}

///////////////////////////////////////////////////
int16 Core_Shutdown()
{
	return E_OK;
}
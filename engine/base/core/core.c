#include "core.h"

extern unsigned long __heapbase;
extern unsigned long __heapsize;
extern unsigned long __bss;
extern unsigned long __bsslen;
extern unsigned long __data;
extern unsigned long __datalen;
extern unsigned long __text;
extern unsigned long __textlen;

uint32 g_sysRamSize;

///////////////////////////////////////////////////
int16 Core_Initialize(uint32 i_sysStackSizeInBytes, uint32 i_stackSizeInBytes, uint32 i_scratchSizeInBytes)
{
	int16 result = E_OK;

	uint32 stackSize = i_sysStackSizeInBytes;
	uint32 ramStart = __bss + __bsslen;
	g_sysRamSize = 0x80200000 - ramStart - stackSize;

#if 0
	REPORT("ram start: %x\n", ramStart);
	REPORT("ram size: %x\n", ramSize);
	REPORT("heap base: %x len %d.\n", __heapbase, __heapsize);
	REPORT("text base: %x len %d.\n", __text, __textlen);
	REPORT("bss base: %x len %d.\n", __bss, __bsslen);
	REPORT("data base: %x len %d.\n", __data, __datalen);
#endif 

	EnterCriticalSection();
	InitHeap3((void*)ramStart, g_sysRamSize);
	ExitCriticalSection();
	
	REPORT("Available RAM size in bytes: %u", g_sysRamSize);
	REPORT("Available stack size in bytes: %u", stackSize);

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
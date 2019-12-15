#include "core.h"

extern unsigned long __heapbase;
extern unsigned long __heapsize;
extern unsigned long __bss;
extern unsigned long __bsslen;
extern unsigned long __data;
extern unsigned long __datalen;
extern unsigned long __text;
extern unsigned long __textlen;

uint32 g_sysHeapStart;
uint32 g_sysHeapEnd;
uint32 g_sysHeapSize;
uint32 g_sysStackSize;

///////////////////////////////////////////////////
int16 Core_Initialize(uint32 i_sysStackSizeInBytes, uint32 i_stackSizeInBytes, uint32 i_scratchSizeInBytes)
{
	int16 result = E_OK;

	g_sysStackSize = i_sysStackSizeInBytes;
	g_sysHeapStart = __heapbase;
	g_sysHeapEnd = 0x80200000 - g_sysStackSize;
	g_sysHeapSize = g_sysHeapEnd - g_sysHeapStart;

#if 1
	REPORT("heap start: %x\n", g_sysHeapStart);
	REPORT("heap end: %x\n", g_sysHeapEnd);
	REPORT("text base: %x len %d.\n", __text, __textlen);
	REPORT("data base: %x len %d.\n", __data, __datalen);
	REPORT("bss base: %x len %d.\n", __bss, __bsslen);	
	REPORT("heap base: %x len %d.\n", __heapbase, __heapsize);
	REPORT("heap size: %i\n", g_sysHeapSize);
	
#endif

	REPORT("Available RAM size in bytes: %u (%.2f Kb)", g_sysHeapSize, (float)g_sysHeapSize / 1024.0f);
	REPORT("Available stack size in bytes: %u (%.2f Kb)", g_sysStackSize, (float)g_sysStackSize / 1024.0f);

	result = Core_InitHeap(g_sysHeapStart, g_sysHeapEnd);
	VERIFY_ASSERT(SUCCESS(result), "Core_Initialize: Unable to initialise CoreHeap");

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
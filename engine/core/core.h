#ifndef CORE_H_INC
#define CORE_H_INC

#include "core_allocators.h"

StackBuffer g_coreStackAlloc;
ScratchBuffer g_coreScratchAlloc;

#define CORE_STACKALLOC (&g_coreStackAlloc)
#define CORE_SCRATCHALLOC (&g_coreScratchAlloc)

// Initialize core memory and internal allocators
int16 Core_Initialize(uint32 i_stackSizeInBytes, uint32 i_scratchSizeInBytes);

// Shutdown
int16 Core_Shutdown();

#endif // CORE_H_INC
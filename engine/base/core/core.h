#ifndef CORE_H_INC
#define CORE_H_INC

#include "core_allocators.h"

StackBuffer g_coreStackAlloc;
ScratchBuffer g_coreScratchAlloc;

#define CORE_STACKALLOC (&g_coreStackAlloc)
#define CORE_SCRATCHALLOC (&g_coreScratchAlloc)

#define ALLOC_VERBOSE (1u)

// Initialize core memory and internal allocators
int16 Core_Initialize(uint32 i_sysStackSizeInBytes, uint32 i_stackSizeInBytes, uint32 i_scratchSizeInBytes);

// Allocate memory
void* Core_Malloc(uint32 i_sizeInBytes, uint8 i_alignment);

// Free memory
void Core_Free(void* i_address);

// Returns free memory in bytes
uint32 Core_GetFreeMemory();

// Returns used memory in bytes
uint32 Core_GetUsedMemory();

// Returns total memory in bytes
uint32 Core_GetTotalMemory();

// Shutdown
int16 Core_Shutdown();

#endif // CORE_H_INC
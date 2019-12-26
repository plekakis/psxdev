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

// Initialise heap
uint16 Core_InitHeap(uint32 i_start, uint32 i_end);

// Allocate memory
void* Core_Malloc(uint32 i_sizeInBytes);

// Allocate memory and initialise to 0
void* Core_CAlloc(uint32 i_elementCount, uint32 i_sizePerElement);

// Reallocate memory, preserving contents
void* Core_Realloc(void* i_ptr, uint32 i_sizeInBytes);

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
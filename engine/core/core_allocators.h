#ifndef CORE_ALLOCATORS_H_INC
#define CORE_ALLOCATORS_H_INC

#include "../engine.h"

///////////////////////////////////////////////////
// SCRATCH ALLOCATOR
///////////////////////////////////////////////////
typedef struct
{
	uint8 *m_start;
	uint8 *m_end;
	uint8 *m_next;
	uint8  m_alignment;
}ScratchBuffer;

// Initialise a scratch buffer
int16 Core_InitScratch(ScratchBuffer* o_buffer, uint32 i_scratchSizeInBytes, uint8 i_alignment);

// Free the scratch buffer allocation
int16 Core_FreeScratch(ScratchBuffer* i_buffer);

// Reset the scratch current pointer
int16 Core_ResetScratch(ScratchBuffer* i_buffer);

// Returns total scratch memory
uint32 Core_GetTotalScratch(ScratchBuffer* i_buffer);

// Returns free scratch memory
uint32 Core_GetFreeScratch(ScratchBuffer* i_buffer);

// Returns used scratch memory
uint32 Core_GetUsedScratch(ScratchBuffer* i_buffer);

// Allocates memory, returns NULL if out of memory
void* Core_AllocScratch(ScratchBuffer* i_buffer, uint32 i_bytes, uint8 i_alignment);

// Checks if specified address is within scratch allocation buffer
bool Core_IsFromScratch(ScratchBuffer* i_buffer, void* i_address);

///////////////////////////////////////////////////
// STACK ALLOCATOR
///////////////////////////////////////////////////

#endif // CORE_ALLOCATORS_H_INC

#ifndef GFX_SCRATCH_H_INC
#define GFX_SCRATCH_H_INC

// Initialise a scratch buffer per framebuffer
int16 Gfx_InitScratch(uint32 i_gfxScratchSizeInBytes);

// Free the scratch buffers
int16 Gfx_FreeScratch();

// Reset the scratch buffer
int16 Gfx_ResetScratch();

// Get the total scratch memory
uint32 Gfx_GetTotalScratch();

// Returns the free memory
uint32 Gfx_GetFreeScratch();

// Returns the used memory
uint32 Gfx_GetUsedScratch();

// Allocates memory from the per-framebuffer scratch buffer
// Returns NULL if out of memory
void* Gfx_Alloc(uint32 i_bytes, uint32 i_alignment);

#endif // GFX_SCRATCH_H_INC
#ifndef GFX_SCRATCH_H_INC
#define GFX_SCRATCH_H_INC

// Initialise a scratch buffer per framebuffer
int16 Gfx_InitScratch(uint8 i_framebufferIndex);

// Free the scratch buffers
int16 Gfx_FreeScratch(uint8 i_frameBufferIndex);

// Reset the scratch buffer
int16 Gfx_ResetScratch(uint8 i_frameBufferIndex);

// Allocates memory from the per-framebuffer scratch buffer
// Returns NULL if out of memory
void* Gfx_Alloc(uint32 i_bytes, uint32 i_alignment);

#endif // GFX_SCRATCH_H_INC
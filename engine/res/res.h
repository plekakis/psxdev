#ifndef RES_H_INC
#define RES_H_INC

#include "../engine.h"

// Get the vram image size using bitplane info (because vram position is in 16bits mode only)
#define	ImageToVRamSize(size, mode)			((size) / (1 << (2 - ((mode) & 3))))
#define	VRamToImageSize(size, mode)			((size) * (1 << (2 - ((mode) & 3))))

// Type of texture resource
typedef enum
{
	ResTextureType_4BitCLUT,
	ResTextureType_8BitCLUT,
	ResTextureType_16Bit,
	ResTextureType_24Bit
}ResTextureType;

// Texture resource
typedef struct
{
	uint16			m_clut;
	uint16			m_tpage;
	uint8			m_width;
	uint8			m_height;
	ResTextureType	m_type;
}ResTexture;

// Initializes the resource system
int16 Res_Initialize();

// Shutdown the resource system
int16 Res_Shutdown();

// Update the resource system
int16 Res_Update();

// Load a new TIM into video ram, reading it from cd first. This is a blocking function.
int16 Res_ReadLoadTIM(StringId i_filename, ResTexture* o_texture);

// Load a new TIM into video ram. The source address must be already initialized, pointing to valid data.
int16 Res_LoadTIM(void* i_srcAddress, ResTexture* o_texture);

#endif // RES_H_INC

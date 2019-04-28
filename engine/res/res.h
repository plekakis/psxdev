#ifndef RES_H_INC
#define RES_H_INC

#include "../gfx/gfx.h"

// Get the vram image size using bitplane info (because vram position is in 16bits mode only)
#define	ImageToVRamSize(size, mode)			((size) / (1 << (2 - ((mode) & 3))))
#define	VRamToImageSize(size, mode)			((size) * (1 << (2 - ((mode) & 3))))

// Texture resource
typedef struct
{
	TextureMode		m_type;
	uint16			m_clut;
	uint16			m_tpage;
	uint16			m_x;
	uint16			m_y;
	uint8			m_width;
	uint8			m_height;	
}ResTexture;

typedef struct
{
	union
	{
		PRIM_F3		*m_f3;
		PRIM_G3		*m_g3;
		PRIM_FT3	*m_ft3;
		PRIM_GT3	*m_gt3;
	}m_data;
		
	uint16		m_polyCount;
	PRIM_TYPE	m_primType;
}ResModel;

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

// Load a new TMD into system ram, reading it from cd first. This is a blocking function.
int16 Res_ReadLoadTMD(StringId i_filename, PRIM_TYPE i_primType, ResModel* o_model);

// Load a new TMD into system ram. The source address must be already initialized, pointing to valid data.
int16 Res_LoadTMD(void* i_srcAddress, PRIM_TYPE i_primType, ResModel* o_model);

// Free a previously loaded TMD.
int16 Res_FreeTMD(ResModel* io_model);

#endif // RES_H_INC

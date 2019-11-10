#ifndef RES_H_INC
#define RES_H_INC

#include <base/gfx/gfx.h>

#define RES_VERBOSE_MATERIAL_LOADING (1u)
#define RES_VERBOSE_MODEL_LOADING (1u)
#define RES_VERBOSE_MODEL_LOADING_DATA (RES_VERBOSE_MODEL_LOADING && 0)
#define RES_VERBOSE_TEXTURE_LOADING (1u)

// Get the vram image size using bitplane info (because vram position is in 16bits mode only)
#define	ImageToVRamSize(size, mode)			((size) / (1 << (2 - ((mode) & 3))))
#define	VRamToImageSize(size, mode)			((size) * (1 << (2 - ((mode) & 3))))

// Texture resource
typedef struct
{
	TextureMode		m_type;
	StringId		m_filename;
	uint16			m_clut;
	uint16			m_tpage;
	uint16			m_x;
	uint16			m_y;
	uint16			m_width;
	uint16			m_height;
}ResTexture;

typedef struct
{
	uint16		m_polyCount;
	PRIM_TYPE	m_primType;
	void*		m_data;
}ResModelSubMesh;

typedef struct
{
	uint8			 m_submeshCount;
	StringId		 m_filename;
	ResModelSubMesh* m_submeshes;
}ResModel2;

typedef struct
{
	StringId	m_name;
	StringId	m_texture;
	uint8		m_type;
	uint8		m_red, m_green, m_blue, m_flags;
	uint8		m_pad[3];
}ResMaterial;

// Initializes the resource system
int16 Res_Initialize();

// Shutdown the resource system
int16 Res_Shutdown();

// Update the resource system
int16 Res_Update();

// Return a pointer to the specified material (NULL if material is not found).
ResMaterial* const Res_GetMaterial(StringId i_materialName);

// Return a pointer to the material for the specified mesh filename and submesh index (NULL if material is not found).
ResMaterial* const Res_GetMaterialLink(StringId i_meshFilename, uint8 i_submesh);

// Return the material's hashed name for that index.
StringId Res_GetMaterialName(uint16 i_index);

// Load a new TIM into video ram, reading it from cd first. This is a blocking function.
int16 Res_ReadLoadTIM(StringId i_filename, ResTexture** o_texture);

// Load a new TIM into video ram. The source address must be already initialized, pointing to valid data.
int16 Res_LoadTIM(void* i_srcAddress, StringId i_filename, ResTexture** o_texture);

// Load a new PSM into system ram, reading it from cd first. This is a blocking function.
int16 Res_ReadLoadPSM(StringId i_filename, ResModel2** o_model);

// Load a new PSM into system ram. The source address must be already initialized, pointing to valid data.
int16 Res_LoadPSM(void* i_srcAddress, StringId i_filename, ResModel2** o_model);

// Free a previously loaded PSM.
int16 Res_FreePSM(ResModel2** io_model);

// Free a previously loaded TIM.
int16 Res_FreeTIM(ResTexture** io_texture);

#endif // RES_H_INC

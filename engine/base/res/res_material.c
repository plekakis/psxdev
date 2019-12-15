#include "res.h"
#include <base/stream/stream.h>
#include <base/core/core.h>
#include <base/util/util.h>

///////////////////////////////////////////////////
// Overview:
// ---------
// Materials and their links are stored in the /ROOT/MATLIB.MAT file.
// The material descriptions are loaded at engine start-up and their links to models' submeshes are tracked.
// At any point, the runtime can query for material availability for a specific model / submesh index combination.
//
// Example:
// --------
// ResMaterial* submeshIndex0Mat = Res_GetMaterialLink(ID("ROOT\\PSM\\TEST.PSM", 0);
// /* ... use submeshIndex0Mat ... */
//
// The returned ResMaterial pointer will include information about the texture, vertex type, base color, etc.
//
// Authoring:
// ----------
// Such links are created in the BuildTool and stored in a Materials.xml at the root of the data_source/ directory of each project.
// This xml is then processed by the MaterialCompiler and writes the final data/ROOT/MATLIB.MAT file.

typedef struct
{
	uint32 m_magic;
	uint16 m_materialCount;
	uint16 m_meshCount;
}MAT_HEADER;

typedef struct
{
	uint16 m_name;
	uint16 m_texture;
	uint8  m_type;
	uint8  m_red, m_green, m_blue, m_flags;
	uint8  m_pad[3];
}MAT_INSTANCE;

typedef struct
{
	uint16 m_materialIndex;
	uint16 m_meshFilename;
	uint8  m_submeshIndex;
	uint8  m_pad[3];
}MAT_LINK;

ResMaterial* g_matlib = NULL;
MAT_LINK* g_matlibLinks = NULL;
uint16 g_matlibSize = 0u;
uint16 g_matlibLinkSize = 0u;

///////////////////////////////////////////////////
int16 Res_LoadMATLIB()
{
	// Load the MATLIB.MAT from disk and initialize the material cache.
	int16 res = E_FILE_IO;
	uint8* ptr = NULL;

#if RES_VERBOSE_MATERIAL_LOADING
	REPORT("Begin loading MATLIB.MAT");
#endif // RES_VERBOSE_MODEL_LOADING

	// Read the file off disc
	Stream_BeginRead(ID("ROOT\\MATLIB.MAT"), (void*)&ptr);	
	if (ptr)
	{
		Stream_ReadFileBlocking();

		{
			MAT_HEADER* header = (MAT_HEADER*)ptr; ptr += sizeof(MAT_HEADER);
			g_matlibSize = header->m_materialCount;
			g_matlibLinkSize = header->m_meshCount;
			g_matlib = (ResMaterial*)Core_Malloc(sizeof(ResMaterial) * g_matlibSize);
			g_matlibLinks = (MAT_LINK*)Core_Malloc(sizeof(MAT_LINK) * g_matlibLinkSize);

			REPORT("Res_LoadMATLIB: Found %u materials and %u meshes", header->m_materialCount, header->m_meshCount);

			// Load materials.
			{
				uint16 matIndex;
				for (matIndex = 0; matIndex < g_matlibSize; ++matIndex)
				{
					ResMaterial* material = &g_matlib[matIndex];
					MAT_INSTANCE* instance = (MAT_INSTANCE*)ptr; ptr += sizeof(MAT_INSTANCE);

					material->m_name = instance->m_name;
					material->m_texture = instance->m_texture;
					material->m_type = instance->m_type;
					material->m_red = instance->m_red;
					material->m_green = instance->m_green;
					material->m_blue = instance->m_blue;
					material->m_flags = instance->m_flags;

					REPORT("Res_LoadMATLIB: Loaded material %u with texture: %u, color[%u, %u, %u], flags: %x", material->m_name, material->m_texture, material->m_red, material->m_green, material->m_blue, material->m_flags);
				}
			}

			// The remaining block is the mesh references, copy it
			memcpy(g_matlibLinks, ptr, sizeof(MAT_LINK) * g_matlibLinkSize);
#if RES_VERBOSE_MATERIAL_LOADING
			{
				uint16 linkIndex;
				for (linkIndex = 0; linkIndex < g_matlibLinkSize; ++linkIndex)
				{
					MAT_LINK* link = &g_matlibLinks[linkIndex];
					StringId materialName = Res_GetMaterialName(link->m_materialIndex);
					REPORT("Res_LoadMATLIB: Loaded link for mesh: %u, submesh: %u, material index: %u (%u)", link->m_meshFilename, link->m_submeshIndex, link->m_materialIndex, materialName);
				}
			}
#endif // RES_VERBOSE_MATERIAL_LOADING
		}

		res = E_OK;
	}
	Stream_EndRead();

#if RES_VERBOSE_MATERIAL_LOADING
	REPORT("End loading MATLIB.MAT (success: %s)", (res == E_OK) ? "TRUE" : "FALSE");
#endif // RES_VERBOSE_MODEL_LOADING

	return res;
}

///////////////////////////////////////////////////
int16 Res_FreeMATLIB()
{
	if (g_matlib)
	{
		Core_Free(g_matlib);
	}

	if (g_matlibLinks)
	{
		Core_Free(g_matlibLinks);
	}
	return E_OK;
}

///////////////////////////////////////////////////
StringId Res_GetMaterialName(uint16 i_index)
{
	VERIFY_ASSERT(i_index < g_matlibLinkSize, "Res_GetMaterialName: Index out of range");
	return g_matlib[i_index].m_name;
}

///////////////////////////////////////////////////
ResMaterial* const Res_GetMaterial(StringId i_materialName)
{
	uint16 i;
	for (i = 0; i < g_matlibSize; ++i)
	{
		ResMaterial* material = &g_matlib[i];

		if (material->m_name == i_materialName)
		{
			return material;
		}
	}
	return NULL;
}

///////////////////////////////////////////////////
ResMaterial* const Res_GetMaterialLink(StringId i_meshFilename, uint8 i_submesh)
{
	uint16 i;
	for (i = 0; i < g_matlibLinkSize; ++i)
	{
		MAT_LINK* link = &g_matlibLinks[i];
		if ((link->m_meshFilename == i_meshFilename) && (link->m_submeshIndex == i_submesh))
		{
			uint16 matIndex = link->m_materialIndex;
			VERIFY_ASSERT(matIndex < g_matlibSize, "Res_GetMaterialLink: material index is out of range");

			return &g_matlib[matIndex];
		}
	}
	return NULL;
}
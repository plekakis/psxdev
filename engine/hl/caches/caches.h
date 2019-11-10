#ifndef HL_CACHES_H_DEF
#define HL_CACHES_H_DEF

#include "obj_cache.h"

///////////////////////////////////////////////////
// MODEL CACHE
///////////////////////////////////////////////////
struct ResModel2;

// A cache for models, encapsulates an ObjCache and provides integrated callbacks for loading and freeing models.
typedef struct
{
	ObjCache m_cache;
}HL_ModelCache;

// Create a model cache.
int16 HL_NewModelCache(HL_ModelCache* o_cache, uint32 i_entryCount);

// Destroy a model cache.
int16 HL_FreeModelCache(HL_ModelCache* io_cache);

// Load a model using the cache.
int16 HL_LoadModel(HL_ModelCache* i_cache, StringId i_filename, ResModel2** o_model);

// Find a model in the cache.
ResModel2* const HL_FindModel(HL_ModelCache* i_cache, StringId i_filename);

///////////////////////////////////////////////////
// TEXTURE CACHE
///////////////////////////////////////////////////
struct ResTexture;

// A cache for textures, encapsulates an ObjCache and provides integrated callbacks for loading and freeing textures.
typedef struct
{
	ObjCache m_cache;
}HL_TextureCache;

// Create a texture cache.
int16 HL_NewTextureCache(HL_TextureCache* o_cache, uint32 i_entryCount);

// Destroy a texture cache.
int16 HL_FreeTextureCache(HL_TextureCache* io_cache);

// Load a texture using the cache.
int16 HL_LoadTexture(HL_TextureCache* i_cache, StringId i_filename, ResTexture** o_texture);

// Find a texture in the cache.
ResTexture* const HL_FindTexture(HL_TextureCache* i_cache, StringId i_filename);

#endif // HL_CACHES_H_DEF
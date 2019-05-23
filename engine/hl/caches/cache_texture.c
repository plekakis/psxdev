#include "caches.h"
#include <base/res/res.h>

///////////////////////////////////////////////////
void HL_Internal_FreeTexture(void** io_psm)
{
	Res_FreeTIM((ResTexture**)io_psm);
}

///////////////////////////////////////////////////
int16 HL_NewTextureCache(HL_TextureCache* o_cache, uint32 i_entryCount)
{
	return ObjCache_Create(&o_cache->m_cache, i_entryCount, HL_Internal_FreeTexture);
}

///////////////////////////////////////////////////
int16 HL_FreeTextureCache(HL_TextureCache* io_cache)
{
	return ObjCache_Free(&io_cache->m_cache);
}

///////////////////////////////////////////////////
int16 HL_LoadTexture(HL_TextureCache* i_cache, StringId i_filename, ResTexture** o_texture)
{
	bool isAdded;
	*o_texture = ObjCache_Insert(&i_cache->m_cache, i_filename, &isAdded);
	if (isAdded)
	{
		Res_ReadLoadTIM(i_filename, o_texture);
		ObjCache_Update(&i_cache->m_cache, i_filename, *o_texture);
	}
}

///////////////////////////////////////////////////
ResTexture* const HL_FindTexture(HL_TextureCache* i_cache, StringId i_filename)
{
	void* const ptr = ObjCache_Find(&i_cache->m_cache, i_filename);
	if (ptr)
	{
		return (ResTexture*)ptr;
	}
	return NULL;
}
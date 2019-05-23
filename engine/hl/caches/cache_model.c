#include "caches.h"
#include <base/res/res.h>

///////////////////////////////////////////////////
void HL_Internal_FreeModel(void** io_psm)
{
	Res_FreePSM((ResModel2**)io_psm);
}

///////////////////////////////////////////////////
int16 HL_NewModelCache(HL_ModelCache* o_cache, uint32 i_entryCount)
{
	return ObjCache_Create(&o_cache->m_cache, i_entryCount, HL_Internal_FreeModel);
}

///////////////////////////////////////////////////
int16 HL_FreeModelCache(HL_ModelCache* io_cache)
{
	return ObjCache_Free(&io_cache->m_cache);
}

///////////////////////////////////////////////////
int16 HL_LoadModel(HL_ModelCache* i_cache, StringId i_filename, ResModel2** o_model)
{
	bool isAdded;
	*o_model = ObjCache_Insert(&i_cache->m_cache, i_filename, &isAdded);
	if (isAdded)
	{
		Res_ReadLoadPSM(i_filename, o_model);
		ObjCache_Update(&i_cache->m_cache, i_filename, *o_model);
	}
}

///////////////////////////////////////////////////
ResModel2* const HL_FindModel(HL_ModelCache* i_cache, StringId i_filename)
{
	void* const ptr = ObjCache_Find(&i_cache->m_cache, i_filename);
	if (ptr)
	{
		return (ResModel2*)ptr;
	}
	return NULL;
}
#include "obj_cache.h"
#include <base/core/core.h>
#include <base/core/core_allocators.h>

///////////////////////////////////////////////////
int16 ObjCache_Create(ObjCache* o_cache, uint32 i_entryCount, ObjCacheFreeFunc i_entryFreeCallback)
{
	VERIFY_ASSERT(o_cache, "ObjCache_Create: Input pointer is null");
	VERIFY_ASSERT(i_entryCount > 0, "ObjCache_Create: entryCount is 0 (cache must have at least space for 1 object)");

	o_cache->m_entries = (ObjCacheEntry*)Core_PushStack(CORE_STACKALLOC, sizeof(ObjCacheEntry) * i_entryCount, 4);
	o_cache->m_entryCount = i_entryCount;
	o_cache->m_currentIndex = 0;
	o_cache->m_freeCallback = i_entryFreeCallback;

	memset(o_cache->m_entries, 0, sizeof(ObjCacheEntry) * i_entryCount);
}

///////////////////////////////////////////////////
int16 ObjCache_Free(ObjCache* io_cache)
{
	VERIFY_ASSERT(io_cache, "ObjCache_Free: Input pointer is null");

	if (io_cache->m_freeCallback)
	{
		uint32 i;
		for (i = 0; i < io_cache->m_currentIndex; ++i)
		{
			io_cache->m_freeCallback(&io_cache->m_entries[i].m_dataPtr);
		}
	}
	Core_PopStack(CORE_STACKALLOC);
	return E_OK;
}

///////////////////////////////////////////////////
void* const ObjCache_Find(ObjCache* const i_cache, uint16 i_key)
{
	VERIFY_ASSERT(i_cache, "ObjCache_Find: Input pointer is null");

	// find and return the data
	{
		uint32 i;
		for (i = 0; i < i_cache->m_currentIndex; ++i)
		{
			ObjCacheEntry* const entry = &i_cache->m_entries[i];
			if (entry->m_key == i_key)
			{
				return entry->m_dataPtr;
			}
		}
	}
	
	return NULL;
}

///////////////////////////////////////////////////
int16 ObjCache_Update(ObjCache* const i_cache, uint16 i_key, void* const i_data)
{
	VERIFY_ASSERT(i_cache, "ObjCache_Update: Input pointer is null");
	
	{
		uint32 i;
		for (i = 0; i < i_cache->m_currentIndex; ++i)
		{
			ObjCacheEntry* entry = &i_cache->m_entries[i];
			if (entry->m_key == i_key)
			{
				// Only be able to update when the entry has just been added, don't allow further modifications.
				VERIFY_ASSERT(entry->m_dataPtr == NULL, "ObjCache_Update: trying to update an entry with already valid data, this is not allowed.");
				entry->m_dataPtr = i_data;
				return E_OK;
			}
		}
	}

	return E_FAILURE;
}

///////////////////////////////////////////////////
void* const ObjCache_Insert(ObjCache* const i_cache, uint16 i_key, bool* o_isAdded)
{
	VERIFY_ASSERT(i_cache, "ObjCache_Find: Input pointer is null");

	{
		void* const data = ObjCache_Find(i_cache, i_key);

		*o_isAdded = (data == NULL);
		if (*o_isAdded)
		{
			return ObjCache_Add(i_cache, i_key, NULL);
		}
		return data;
	}
}

///////////////////////////////////////////////////
void* const ObjCache_Add(ObjCache* const i_cache, uint16 i_key, void* const i_data)
{
	VERIFY_ASSERT(i_cache, "ObjCache_Add: Input pointer is null");

	{
		ObjCacheEntry* nextEntry = &i_cache->m_entries[i_cache->m_currentIndex++];
		VERIFY_ASSERT(nextEntry->m_dataPtr == NULL, "ObjCache_Add: Data already exists for slot index %u and key %u. This shouldn't happen.", i_cache->m_currentIndex, i_key);

		nextEntry->m_key = i_key;
		nextEntry->m_dataPtr = i_data;

		return nextEntry->m_dataPtr;
	}
}
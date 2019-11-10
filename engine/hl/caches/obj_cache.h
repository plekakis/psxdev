#ifndef OBJ_CACHE_H_INC
#define OBJ_CACHE_H_INC

#include <engine.h>

/*
Object Cache
------------

This is a generic object cache, keeping its entries on the core stack. As a result, they can be thrown away at level-unload easily.
Optionally, a callback to free allocated entry data can be specified. This is assuming that data is allocated previously using Core_Malloc.

Combining the above, memory can be freed in one go with minimal tracking required.

Usage:
------

ObjCache g_cache;

...

ObjCache_Create(&g_cache, 64, FreeCallbackFunc); // create space for 64 entries

bool isAdded;
ptr = ObjCache_Insert(&g_cache, key, &isAdded);
if (isAdded)
{
	// new entry added, do stuff with "ptr" (eg. load data from disc)
	// ...
	//
	ObjCache_Update(&g_cache, key, ptr);
}

...

ObjCache_Free(&g_cache);
*/

// Callback for freeing entries in the cache.
typedef void(*ObjCacheFreeFunc)(void**);

typedef struct
{
	uint16	m_key;
	void*	m_dataPtr;
}ObjCacheEntry;

typedef struct
{
	ObjCacheEntry*		m_entries;
	uint32				m_entryCount;
	uint32				m_currentIndex;
	ObjCacheFreeFunc	m_freeCallback;
}ObjCache;

// Create a new object cache and allocate enough space using the built-in stack allocator.
int16 ObjCache_Create(ObjCache* o_cache, uint32 i_entryCount, ObjCacheFreeFunc i_entryFreeCallback);

// Destroy an object cache by freeing memory previously allocated by the stack allocator.
int16 ObjCache_Free(ObjCache* io_cache);

// Do a linear search in the cache for the specified key. If found, return the data associated with it, NULL othewise.
void* const ObjCache_Find(ObjCache* const i_cache, uint16 i_key);

// Tries to insert a new object in the cache.
void* const ObjCache_Insert(ObjCache* const i_cache, uint16 i_key, bool* o_isAdded);

// Updates the entry for specified key.
int16 ObjCache_Update(ObjCache* const i_cache, uint16 i_key, void* const i_data);

// Add a key/data pair to the next available slot in the cache. Returns the added data pointer.
void* const ObjCache_Add(ObjCache* const i_cache, uint16 i_key, void* const i_data);

#endif // OBJ_CACHE_H_INC
#include "stream.h"

#define STREAM_CATALOG_RETRIES (10u)
#define STREAM_CATALOG "\\ROOT\\CATALOG.CAT;1"

///////////////////////////////////////////////////
void Stream_EnumerateCatalog()
{
	// Load the catalog file and search for all the files in the cd, store their information to be used later.
	uint32 i;
	bool found = FALSE;
	CdlFILE catalog;
	for (i = 0; i < STREAM_CATALOG_RETRIES; ++i)
	{
		if (CdSearchFile(&catalog, STREAM_CATALOG) != 0)
		{
			found = TRUE;
			break;
		}
	}

	VERIFY_ASSERT(found, "%s was not found on CD! This is required.", STREAM_CATALOG);
	REPORT("Catalog file loaded, size: %i bytes. Searching for files...", catalog.size);
}

///////////////////////////////////////////////////
int16 Stream_Initialize()
{
	int32 err = CdInit();
	VERIFY_ASSERT(err == 1, "Stream_Initialize: CdInit failure");

#if ASSERT_ENABLED
	CdSetDebug(2);
#endif // ASSERT_ENABLED

	Stream_EnumerateCatalog();
	return E_OK;
}

///////////////////////////////////////////////////
int16 Stream_Shutdown()
{
	return E_OK;
}

///////////////////////////////////////////////////
int16 Stream_Update()
{
	return E_OK;
}
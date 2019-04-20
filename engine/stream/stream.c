#include "stream.h"
#include "../core/core.h"
#include "../util/util.h"

#define CD_READ_RETRIES (10u)
#define STREAM_CATALOG "\\ROOT\\CATALOG.CAT;1"

CdFileEntry* g_cdEntries = NULL; // This is populated by the catalog enumeration, it's an array of all files present on cd
uint32 g_numCdEntries = 0u;

CdFileEntry* g_currentCdEntry = NULL;	// Set by BeginRead, invalidated by EndRead
void* g_currentCdBuffer;				// Allocated by BeginRead, freed by EndRead

///////////////////////////////////////////////////
int16 Stream_ReadFileBlockImpl(const char* i_filename, void* o_buffer, uint32 i_sizeInBytes)
{
	uint32 i;	
	for (i = 0; i < CD_READ_RETRIES; ++i)
	{
		int32 remainingSectors;
		CdReadFile((char*)i_filename, (u_long*)o_buffer, i_sizeInBytes);
		while ((remainingSectors = CdReadSync(1, 0)) > 0)
		{
			VSync(0);
		}

		// All done
		if (remainingSectors == 0)
		{
			break;
		}
	}
	return (i == CD_READ_RETRIES) ? E_FILE_IO : E_OK;
}

///////////////////////////////////////////////////
void Stream_EnumerateCatalog()
{	
	uint32 i;
	CdlFILE catalog;

	// Load the catalog file and search for all the files in the cd, store their information to be used later.
	{
		bool found = FALSE;	
		for (i = 0; i < CD_READ_RETRIES; ++i)
		{
			if (CdSearchFile(&catalog, STREAM_CATALOG) != 0)
			{
				found = TRUE;
				break;
			}
		}

		VERIFY_ASSERT(found, "Stream_EnumerateCatalog: %s was not found on CD! This is required.", STREAM_CATALOG);
		REPORT("Catalog file found, size: %i bytes. Searching for files...", catalog.size);
	}
	
	// Read the catalog and extract filenames
	{
		void* catalogBuffer = Core_PushStack(CORE_STACKALLOC, catalog.size + Stream_CdSectorSize(), 4);
		
		int16 err = Stream_ReadFileBlockImpl(STREAM_CATALOG, catalogBuffer, catalog.size);
		VERIFY_ASSERT(SUCCESS(err), "Stream_EnumerateCatalog: Failed reading the catalog file contents!");

		// Count files, there is one file per line.
		{
			char* contents = catalogBuffer;
			while (*contents++ != '\0')
			{
				if (*contents == '\n')
				{
					++g_numCdEntries;
				}
			}
		}

		REPORT("Catalog file loaded, %u files found.", g_numCdEntries);

		if (g_numCdEntries > 0)
		{
			// Go over each line (each filename)
			char* contents = (char*)strtok(catalogBuffer, "\n");

			g_cdEntries = Core_Malloc(sizeof(CdFileEntry) * g_numCdEntries, 4);

			while (contents != NULL)
			{
				uint32 searchAttempt;
				bool found = FALSE;
				CdFileEntry* entry = &g_cdEntries[i];
				
				// Search for the file and cache the result.
				for (searchAttempt = 0; searchAttempt < CD_READ_RETRIES; ++searchAttempt)
				{
					char filename[64];
					sprintf(filename, "\\%s;1", contents);

					if (CdSearchFile(&entry->m_file, filename) != 0)
					{
						found = TRUE;
						break;
					}
				}

				VERIFY_ASSERT(found, "%s which was present on the catalog, was not found on the CD!", contents);

				entry->m_filename = ID(contents);
				
				REPORT("Found %s (hash: %u), size: %u bytes", contents, entry->m_filename, entry->m_file.size);

				// Grab next token
				contents = (char*)strtok(NULL, "\n");
				++i;
			}
		}
		
		Core_PopStack(CORE_STACKALLOC);
	}
}

///////////////////////////////////////////////////
int16 Stream_Initialize()
{
	int32 err = CdInit();
	VERIFY_ASSERT(err == 1, "Stream_Initialize: CdInit failure");

#if ASSERT_ENABLED
	#if STREAM_VERBOSE
	CdSetDebug(2);
	#else
	CdSetDebug(1);
	#endif // STREAM_VERBOSE
#endif // ASSERT_ENABLED

	Stream_EnumerateCatalog();
	return E_OK;
}

///////////////////////////////////////////////////
int16 Stream_Shutdown()
{
	Core_Free(g_cdEntries);
	return E_OK;
}

///////////////////////////////////////////////////
CdFileEntry* Stream_GetFileInfo(StringId i_filename)
{
	uint32 i;
	for (i = 0; i < g_numCdEntries; ++i)
	{
		if (g_cdEntries[i].m_filename == i_filename)
		{			
			return &g_cdEntries[i];
		}
	}
	return NULL;
}

///////////////////////////////////////////////////
uint32 Stream_GetFileSize(StringId i_filename)
{
	CdFileEntry* entry = Stream_GetFileInfo(i_filename);
	VERIFY_ASSERT(entry, "File id %u not found!", i_filename);

	return entry->m_file.size;
}

///////////////////////////////////////////////////
int16 Stream_BeginRead(StringId i_filename, void** o_ptr)
{
	VERIFY_ASSERT(g_currentCdEntry == NULL, "Stream_BeginRead called twice! (current cd entry not invalidated?)");
	VERIFY_ASSERT(g_currentCdBuffer == NULL, "Stream_BeginRead called twice! (allocation not freed?)");

	// Find the file info in the cached array
	{
		g_currentCdEntry = Stream_GetFileInfo(i_filename);
		if (!g_currentCdEntry)
			return E_FILE_IO;
	}

	// Allocate enough space to read from cd, aligned to sector size
	{
		uint32 size = Util_AlignUp(g_currentCdEntry->m_file.size, Stream_CdSectorSize());
		g_currentCdBuffer = (uint8*)Core_PushStack(CORE_STACKALLOC, size, 4);
		if (!g_currentCdBuffer)
			return E_OUT_OF_MEMORY;

		*o_ptr = g_currentCdBuffer;
	}
	
	return E_OK;
}

///////////////////////////////////////////////////
int16 Stream_EndRead()
{
	VERIFY_ASSERT(g_currentCdEntry != NULL, "Stream_EndRead called twice! (current cd entry not found?)");
	VERIFY_ASSERT(g_currentCdBuffer != NULL, "Stream_EndRead called twice! (allocation unsuccessful?)");

	Core_PopStack(CORE_STACKALLOC);
	g_currentCdBuffer = NULL;
	g_currentCdEntry = NULL;
}

///////////////////////////////////////////////////
int16 Stream_ReadFileBlocking()
{
	uint32 i;
	bool read = FALSE;
	
	// Try enough times, until the file is read.
	for (i = 0; i < CD_READ_RETRIES; ++i)
	{
		int32 remainingSectors;
		int32 mode = CdlModeSpeed;
		int32 nsector = (g_currentCdEntry->m_file.size + Stream_CdSectorSize()-1) / Stream_CdSectorSize();
		
		// When changing the speed, we need to wait for 3 VSyncs. I am not sure why, however this means that chaining file loads using this function
		// will be slower than expected.
		CdControlF(CdlSetloc, (uint8*)&g_currentCdEntry->m_file.pos);
		CdControlB(CdlSetmode, (uint8*)&mode, 0);
		VSync(3);

		CdRead(nsector, g_currentCdBuffer, mode);
		
		// Wait until the file has been read.
		while ((remainingSectors = CdReadSync(1, 0)) > 0)
		{
			VSync(0);
		}
		
		// All done
		if (remainingSectors == 0)
		{
			read = TRUE;
			break;
		}
	}
	return read ? E_OK : E_FILE_IO;
}

///////////////////////////////////////////////////
int16 Stream_Update()
{
	return E_OK;
}
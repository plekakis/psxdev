#ifndef STREAM_H_INC
#define STREAM_H_INC

#include "../engine.h"

#ifndef STREAM_VERBOSE
#define STREAM_VERBOSE (0u)
#endif // STREAM_VERBOSE

// A helper structure to locate files on cd.
typedef struct
{
	CdlFILE	 m_file;
	StringId m_filename;
}CdFileEntry;

// The cd sector size, allocations that are used to read from cd need to have their size padded by this.
static uint32 Stream_CdSectorSize() { return 2048; }

// Initializes the stream system
int16 Stream_Initialize();

// Shutdown the stream system
int16 Stream_Shutdown();

// Update the stream system
int16 Stream_Update();

// Get cached file info
CdFileEntry* Stream_GetFileInfo(StringId i_filename);

// Read a file, blocking for completion
int16 Stream_ReadFileBlocking(StringId i_filename, void* o_buffer);

// Read a file, using callback for completion
int16 Stream_ReadFile(StringId i_filename, void* o_buffer);

// Get a file's size in bytes
uint32 Stream_GetFileSize(StringId i_filename);

#endif // STREAM_H_INC

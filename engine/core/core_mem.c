#include "core.h"

///////////////////////////////////////////////////
void* Core_Malloc(uint32 i_sizeInBytes, uint32 i_alignment)
{
	return (void*)malloc3(i_sizeInBytes);
}

///////////////////////////////////////////////////
void Core_Free(void* i_address)
{
	free3(i_address);
}
#include "res.h"
#include "../stream/stream.h"
#include "../core/core.h"
#include "../util/util.h"

///////////////////////////////////////////////////
int16 Res_LoadTIM(void* i_srcAddress, ResTexture* o_texture)
{
	TIM_IMAGE tim;
	Util_MemZero(&tim, sizeof(tim));

	VERIFY_ASSERT(i_srcAddress, "Source address is null");
	VERIFY_ASSERT(o_texture, "Destination texture is null");

	if (OpenTIM((uint32*)i_srcAddress) != 0)
		return E_INVALIDARGS;

	if (ReadTIM(&tim) == 0)
		return E_FAILURE;
	
	o_texture->m_type = (ResTextureType)tim.mode & 3;
	o_texture->m_width = VRamToImageSize(tim.prect->w, tim.mode & 3);
	o_texture->m_height = VRamToImageSize(tim.prect->h, tim.mode & 3);

	o_texture->m_tpage = LoadTPage(tim.paddr, tim.mode & 3, 0, tim.prect->x, tim.prect->y, o_texture->m_width, o_texture->m_height);	

	if (o_texture->m_type < ResTextureType_16Bit)
	{
		o_texture->m_clut = LoadClut(tim.caddr, tim.crect->x, tim.crect->y);
	}

	return E_OK;
}

///////////////////////////////////////////////////
int16 Res_ReadLoadTIM(StringId i_filename, ResTexture* o_texture)
{
	int16 res = E_OK;

	// Get file information
	uint32 size = Stream_GetFileSize(i_filename);

	// Allocate enough space using the built-in stack allocator
	uint8* ptr = (uint8*)Core_PushStack(CORE_STACKALLOC, size, 4);
	
	// Read the file off disc
	Stream_ReadFileBlocking(i_filename, ptr);

	// Load the TIM
	res = Res_LoadTIM(ptr, o_texture);

	// Pop stack
	Core_PopStack(CORE_STACKALLOC);

	return res;
}
#include "res.h"
#include <base/stream/stream.h>
#include <base/core/core.h>
#include <base/util/util.h>

///////////////////////////////////////////////////
int16 Res_LoadTIM(void* i_srcAddress, ResTexture** o_texture)
{
	TIM_IMAGE tim;
	Util_MemZero(&tim, sizeof(tim));

	VERIFY_ASSERT(i_srcAddress, "Source address is null");
	VERIFY_ASSERT(o_texture, "Destination texture is null");

	if (OpenTIM((uint32*)i_srcAddress) != 0)
		return E_INVALIDARGS;

	if (ReadTIM(&tim) == 0)
		return E_FAILURE;

	*o_texture = (ResTexture*)Core_Malloc(sizeof(ResTexture), 4);

	Util_MemZero(*o_texture, sizeof(ResTexture));

	(*o_texture)->m_type = (TextureMode)tim.mode & 3;
	(*o_texture)->m_x = tim.prect->x;
	(*o_texture)->m_y = tim.prect->y;
	(*o_texture)->m_width = VRamToImageSize(tim.prect->w, tim.mode);
	(*o_texture)->m_height = tim.prect->h;

	REPORT("texture type %i: %i, %i", (*o_texture)->m_type, (*o_texture)->m_width, (*o_texture)->m_height);

	// Width & height are actual pixels, not area covered in framebuffer (which can be compressed in 4/8 bit CLUT modes)
	(*o_texture)->m_tpage = LoadTPage(tim.paddr, (*o_texture)->m_type, 0, tim.prect->x, tim.prect->y, (*o_texture)->m_width, (*o_texture)->m_height);

	if ((*o_texture)->m_type < TEXTURE_MODE_16BIT)
	{
		// clut x must be 16pixel aligned.
		VERIFY_ASSERT( (tim.crect->x & 15)==0, "CLUT x is not 16pixel aligned!");
		(*o_texture)->m_clut = LoadClut(tim.caddr, tim.crect->x, tim.crect->y);
	}

	return E_OK;
}

///////////////////////////////////////////////////
int16 Res_ReadLoadTIM(StringId i_filename, ResTexture** o_texture)
{
	int16 res = E_OK;
	void* ptr;

	// Read the file off disc
	Stream_BeginRead(i_filename, &ptr);

	Stream_ReadFileBlocking();

	// Load the TIM
	res = Res_LoadTIM(ptr, o_texture);

	Stream_EndRead();
	return res;
}

///////////////////////////////////////////////////
int16 Res_FreeTIM(ResTexture** io_texture)
{
	VERIFY_ASSERT(io_texture && *io_texture, "Input texture is null");
	Core_Free(*io_texture);
}
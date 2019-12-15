#include "res.h"
#include <base/stream/stream.h>
#include <base/core/core.h>
#include <base/util/util.h>

///////////////////////////////////////////////////
int16 Res_LoadTIM(void* i_srcAddress, StringId i_filename, ResTexture** o_texture)
{
	TIM_IMAGE tim;
	Util_MemZero(&tim, sizeof(tim));

	VERIFY_ASSERT(i_srcAddress, "Source address is null");
	VERIFY_ASSERT(o_texture, "Destination texture is null");

	if (OpenTIM((uint32*)i_srcAddress) != 0)
		return E_INVALIDARGS;

	if (ReadTIM(&tim) == 0)
		return E_FAILURE;

	*o_texture = (ResTexture*)Core_Malloc(sizeof(ResTexture));

	Util_MemZero(*o_texture, sizeof(ResTexture));

	(*o_texture)->m_filename = i_filename;
	(*o_texture)->m_type = (TextureMode)tim.mode & 3;
	(*o_texture)->m_x = tim.prect->x;
	(*o_texture)->m_y = tim.prect->y;
	(*o_texture)->m_width = VRamToImageSize(tim.prect->w, tim.mode);
	(*o_texture)->m_height = tim.prect->h;

#if RES_VERBOSE_TEXTURE_LOADING
	REPORT("Texture type %i: x: %i, y: %i, width: %i, height: %i", (*o_texture)->m_type, (*o_texture)->m_x, (*o_texture)->m_y, (*o_texture)->m_width, (*o_texture)->m_height);
#endif // RES_VERBOSE_TEXTURE_LOADING

	// Width & height are actual pixels, not area covered in framebuffer (which can be compressed in 4/8 bit CLUT modes)
	(*o_texture)->m_tpage = LoadTPage(tim.paddr, (*o_texture)->m_type, 0, tim.prect->x, tim.prect->y, (*o_texture)->m_width, (*o_texture)->m_height);

	if ((*o_texture)->m_type < TEXTURE_MODE_16BIT)
	{
		// clut x must be 16pixel aligned.
		VERIFY_ASSERT( (tim.crect->x & 15)==0, "CLUT x is not 16pixel aligned!");
		(*o_texture)->m_clut = LoadClut(tim.caddr, tim.crect->x, tim.crect->y);

#if RES_VERBOSE_TEXTURE_LOADING
		REPORT("CLUT x: %i, y: %i, width: %i, height: %i", tim.crect->x, tim.crect->y, tim.crect->w, tim.crect->h);
#endif // RES_VERBOSE_TEXTURE_LOADING
	}

	return E_OK;
}

///////////////////////////////////////////////////
int16 Res_ReadLoadTIM(StringId i_filename, ResTexture** o_texture)
{
	int16 res = E_FILE_IO;
	void* ptr = NULL;

#if RES_VERBOSE_TEXTURE_LOADING
	REPORT("Begin loading TIM %i", i_filename);
#endif // RES_VERBOSE_TEXTURE_LOADIN

	// Read the file off disc
	Stream_BeginRead(i_filename, &ptr);
	if (ptr)
	{
		Stream_ReadFileBlocking();

		// Load the TIM
		res = Res_LoadTIM(ptr, i_filename, o_texture);
	}
	Stream_EndRead();

#if RES_VERBOSE_TEXTURE_LOADING
	REPORT("End loading TIM %i (success: %s)", i_filename, (res == E_OK) ? "TRUE" : "FALSE");
#endif // RES_VERBOSE_TEXTURE_LOADING
	return res;
}

///////////////////////////////////////////////////
int16 Res_FreeTIM(ResTexture** io_texture)
{
	VERIFY_ASSERT(io_texture && *io_texture, "Input texture is null");

#if RES_VERBOSE_TEXTURE_LOADING
	REPORT("Freeing TIM %u", (*io_texture)->m_filename);
#endif // RES_VERBOSE_TEXTURE_LOADING
	Core_Free(*io_texture);
}
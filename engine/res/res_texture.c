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

	Util_MemZero(o_texture, sizeof(ResTexture));

	o_texture->m_type = (TextureMode)tim.mode & 3;
	o_texture->m_x = tim.prect->x;
	o_texture->m_y = tim.prect->y;
	o_texture->m_width = VRamToImageSize(tim.prect->w, o_texture->m_type);
	o_texture->m_height = VRamToImageSize(tim.prect->h, o_texture->m_type);

	o_texture->m_tpage = LoadTPage(tim.paddr, o_texture->m_type, 0, o_texture->m_x, o_texture->m_y, o_texture->m_width, o_texture->m_height);

	if (o_texture->m_type < TEXTURE_MODE_16BIT)
	{
		o_texture->m_clut = LoadClut(tim.caddr, tim.crect->x, tim.crect->y);
	}

	return E_OK;
}

///////////////////////////////////////////////////
int16 Res_ReadLoadTIM(StringId i_filename, ResTexture* o_texture)
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
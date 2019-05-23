#if !CONFIG_FINAL
#include "embedded/debug/gfx_debugfont.c"

static uint16 s_debugfontClut;
static uint16 s_debugFontTPage;

#endif // CONFIG_FINAL

#define CHAR2D SPRT_8
#define BATCH2D_VERIFY(batch, x, type) \
	{ \
		uint8* current = (uint8*)(x) + sizeof(type); \
		uint8* end = (uint8*)(batch)->m_baseAddress + (batch)->m_sizeInBytes; \
		VERIFY_ASSERT( current <= end, "Batch2D (ptr: %p) scratch exhausted while trying to allocate a " #type "! (%u bytes over)", batch, (current - end)) \
	}

///////////////////////////////////////////////////
static void Gfx_Batch2D_Increment(Batch2D* io_batch, uint32 i_sizeInBytes)
{
	// Move to the next primitive, incrementing the current address and linking previous & current primitives together
	void* const current = io_batch->m_currentAddress;
	void* const previous = (uint8*)current - io_batch->m_prevPrimSize;

	catPrim(previous, current);

	io_batch->m_prevPrimSize = i_sizeInBytes;
	io_batch->m_currentAddress = (uint8*)io_batch->m_currentAddress + io_batch->m_prevPrimSize;
}

///////////////////////////////////////////////////
static void Gfx_Batch2D_UpdateFlags(void* i_prim, PrimFlags i_flags)
{
	setSemiTrans(i_prim, (i_flags & PRIM_FLAG_SEMI_TRANS) != 0);
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddMode(Batch2D* io_batch, BlendRate i_blendRate, ModeFlags i_flags, RECT* i_textureWindow, TPageAddress* i_tpageAddress)
{
	const uint16 tpage = getTPage(i_tpageAddress ? i_tpageAddress->m_mode : 0, i_blendRate, i_tpageAddress ? i_tpageAddress->m_tpageX : 0, i_tpageAddress ? i_tpageAddress->m_tpageY : 0);
	return Gfx_Batch2D_AddModeDirect(io_batch, i_flags, i_textureWindow, tpage);
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddModeDirect(Batch2D* io_batch, ModeFlags i_flags, RECT* i_textureWindow, uint16 i_tpage)
{
	DR_MODE* mode = (DR_MODE*)io_batch->m_currentAddress;
	BATCH2D_VERIFY(io_batch, mode, DR_MODE);

	VERIFY_ASSERT( (i_textureWindow->w < 256) && (i_textureWindow->h < 256), "Texture window w & h must be less than 256. Passed in: %ux%u", i_textureWindow->w, i_textureWindow->h);
	
	setDrawMode
	(
		mode,
		(i_flags & MODE_FLAG_DRAW_IN_DISP_AREA) != 0,
		(i_flags & MODE_FLAG_DITHERING) != 0,
		i_tpage,
		i_textureWindow
	);

	Gfx_Batch2D_Increment(io_batch, sizeof(DR_MODE));
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddTPage(Batch2D* io_batch, BlendRate i_blendRate, ModeFlags i_flags, TPageAddress* i_tpageAddress)
{
	const uint16 tpage = getTPage(i_tpageAddress ? i_tpageAddress->m_mode : 0, i_blendRate, i_tpageAddress ? i_tpageAddress->m_tpageX : 0, i_tpageAddress ? i_tpageAddress->m_tpageY : 0);
	return Gfx_Batch2D_AddTPageDirect(io_batch, i_flags, tpage);
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddTPageDirect(Batch2D* io_batch, ModeFlags i_flags, uint16 i_tpage)
{
	DR_TPAGE* tpage = (DR_TPAGE*)io_batch->m_currentAddress;
	BATCH2D_VERIFY(io_batch, tpage, DR_TPAGE);

	setDrawTPage
	(
		tpage,
		(i_flags & MODE_FLAG_DRAW_IN_DISP_AREA) != 0,
		(i_flags & MODE_FLAG_DITHERING) != 0,
		i_tpage
	);

	Gfx_Batch2D_Increment(io_batch, sizeof(DR_TPAGE));
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddTile(Batch2D* io_batch, DVECTOR* const i_position, DVECTOR* const i_size, CVECTOR* const i_color, PrimFlags i_flags)
{
	TILE* tile = (TILE*)io_batch->m_currentAddress;
	BATCH2D_VERIFY(io_batch, tile, TILE);

	VERIFY_ASSERT((i_size->vx & 1) == 0, "Tile width must be even number! Passed in: %u", i_size->vx);

	setTile(tile);
	setXY0(tile, i_position->vx, i_position->vy);
	setWH(tile, i_size->vx, i_size->vy);
	setRGB0(tile, i_color->r, i_color->g, i_color->b);
	
	Gfx_Batch2D_Increment(io_batch, sizeof(TILE));
	Gfx_Batch2D_UpdateFlags(tile, i_flags);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddSprite(Batch2D* io_batch, DVECTOR* const i_position, DVECTOR* const i_size, TVECTOR* const i_uv, CVECTOR* const i_color, ClutAddress* i_clutAddress, PrimFlags i_flags)
{
	VERIFY_ASSERT(i_clutAddress, "Expected a valid CLUT address!");
	return Gfx_Batch2D_AddSpriteDirect(io_batch, i_position, i_size, i_uv, i_color, getClut(i_clutAddress->m_clutX, i_clutAddress->m_clutY), i_flags);
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddSpriteDirect(Batch2D* io_batch, DVECTOR* const i_position, DVECTOR* const i_size, TVECTOR* const i_uv, CVECTOR* const i_color, uint16 i_clut, PrimFlags i_flags)
{
	SPRT* sprt = (SPRT*)io_batch->m_currentAddress;
	BATCH2D_VERIFY(io_batch, sprt, SPRT);

	VERIFY_ASSERT((i_size->vx & 1) == 0, "Sprite width must be even number! Passed in: %u", i_size->vx);
	VERIFY_ASSERT( (i_uv->u & 1) == 0, "Sprite texture u coordinate must be even number! Passed in: %u", i_uv->u);

	setSprt(sprt);
	setXY0(sprt, i_position->vx, i_position->vy);
	setWH(sprt, i_size->vx, i_size->vy);
	setRGB0(sprt, i_color->r >> 1, i_color->g >> 1, i_color->b >> 1);
	setUV0(sprt, i_uv ? i_uv->u : MIN(i_size->vx, 256), i_uv ? i_uv->v : MIN(i_size->vy, 256));
	sprt->clut = i_clut;

	Gfx_Batch2D_Increment(io_batch, sizeof(SPRT));
	Gfx_Batch2D_UpdateFlags(sprt, i_flags);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddLineF(Batch2D* io_batch, DVECTOR* const i_start, DVECTOR* const i_end, CVECTOR* const i_color, PrimFlags i_flags)
{
	LINE_F2* line = (LINE_F2*)io_batch->m_currentAddress;
	BATCH2D_VERIFY(io_batch, line, LINE_F2);

	setLineF2(line);
	setXY2(line, i_start->vx, i_start->vy, i_end->vx, i_end->vy);
	setRGB0(line, i_color->r, i_color->g, i_color->b);
	
	Gfx_Batch2D_UpdateFlags(line, i_flags);
	Gfx_Batch2D_Increment(io_batch, sizeof(LINE_F2));
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddLineG(Batch2D* io_batch, DVECTOR* const i_start, DVECTOR* const i_end, CVECTOR* const i_startColor, CVECTOR* const i_endColor, PrimFlags i_flags)
{
	LINE_G2* line = (LINE_G2*)io_batch->m_currentAddress;
	BATCH2D_VERIFY(io_batch, line, LINE_G2);

	setLineG2(line);
	setXY2(line, i_start->vx, i_start->vy, i_end->vx, i_end->vy);
	setRGB0(line, i_startColor->r, i_startColor->g, i_startColor->b);
	setRGB1(line, i_endColor->r, i_endColor->g, i_endColor->b);

	Gfx_Batch2D_UpdateFlags(line, i_flags);
	Gfx_Batch2D_Increment(io_batch, sizeof(LINE_G2));
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddString(Batch2D* io_batch, const char* i_string, DVECTOR* const i_position, DVECTOR* const i_maxSize, CVECTOR* i_color, uint16 i_clut, PrimFlags i_flags)
{
	uint16 startX = i_position->vx;
	uint16 startY = i_position->vy;
	uint16 x = 0;
	uint16 y = 0;
	uint8 r = i_color->r >> 1;
	uint8 g = i_color->g >> 1;
	uint8 b = i_color->b >> 1;
	int32	code, isret;
	DVECTOR ms = { i_maxSize ? i_maxSize->vx : INT16_MAX, i_maxSize ? i_maxSize->vy : INT16_MAX };
	char* s = (char*)i_string;

	while (*s)
	{
		isret = 0;
		switch (*s)
		{
		case '~':
			switch (*++s)
			{
			case 'c':
				r = (*++s - '0') * 16;
				g = (*++s - '0') * 16;
				b = (*++s - '0') * 16;
				break;
			}
			break;

		case '\n':
			isret = 1;
			break;

		case '\t':
			if ((x += 8 * 4) >= ms.vx) isret = 1;
			break;

		case ' ':
			if ((x += 8) >= ms.vx) isret = 1;
			break;

		default:
			if (*s >= 'a' && *s <= 'z')
				code = *s - 'a' + 33;
			else
				code = *s - '!' + 1;

			{
				const uint8 u = (code % 16) << 3;
				const uint8 v = (code / 16) << 3;
				CHAR2D* sp = (CHAR2D*)io_batch->m_currentAddress;
				BATCH2D_VERIFY(io_batch, sp, CHAR2D);

				SetSprt8(sp);
				setUV0(sp, u, v);
				setXY0(sp, x + startX, y + startY);
				setRGB0(sp, r, g, b);
				sp->clut = i_clut;

				Gfx_Batch2D_UpdateFlags(sp, i_flags);
				Gfx_Batch2D_Increment(io_batch, sizeof(CHAR2D));
			}

			if ((x += 8) >= ms.vx) isret = 1;
			break;
		}
		if (isret)
		{
			x = 0;
			if ((y += 8) >= ms.vy)
				break;
		}
		s++;
	}
	
	return E_OK;
}
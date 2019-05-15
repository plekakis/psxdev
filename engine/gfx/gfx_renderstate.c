#include "gfx.h"

// The render state
typedef struct
{
	uint32			m_flags;				// corresponds to a bitmask of RENDERSTATE
	uint32			m_texture;				// packed tpage, clut
	uint32			m_textureScaleOffset;	// packed texture scale and offset
	uint32			m_fog;					// packed near, far
	uint32			m_fogColor;				// packed rgb color
	uint32			m_polyBaseColor;		// packed polygon base color (vertex/face color can be multiplied by this)
	uint32			m_backColor;			// packed rgb back color
	uint32			m_clearColor;			// packed rgb clear color
	MATRIX			m_lightColors;			// #0, #1 and #2 light colors
	MATRIX			m_lightVectors;			// #0, #1 and #2 light vectors
	DivisionParams*	m_divisionParams;		// A pointer to the current division parameters
}RenderState;

RenderState g_rs;

///////////////////////////////////////////////////
void Gfx_InitState()
{
	Util_MemZero(&g_rs, sizeof(g_rs));

	Gfx_SetTextureScaleOffset(1, 1, 0, 0);
	Gfx_SetBackColor(32, 32, 32);
	Gfx_SetRenderState(RS_PERSP);
}

///////////////////////////////////////////////////
uint32 Gfx_GetRenderState()
{
	return g_rs.m_flags;
}

///////////////////////////////////////////////////
uint32 Gfx_InvalidateRenderState(uint32 i_state)
{
	g_rs.m_flags &= ~i_state;
	return g_rs.m_flags;
}

///////////////////////////////////////////////////
uint32 Gfx_SetRenderState(uint32 i_state)
{
	g_rs.m_flags |= i_state;
	return g_rs.m_flags;
}

///////////////////////////////////////////////////
void Gfx_SetPolyBaseColor(uint8 i_red, uint8 i_green, uint8 i_blue)
{
	g_rs.m_polyBaseColor = PACK_RGB(i_red, i_green, i_blue);
}

///////////////////////////////////////////////////
void Gfx_GetPolyBaseColor(uint8* o_red, uint8* o_green, uint8* o_blue)
{
	VERIFY_ASSERT(o_red && o_green && o_blue, "Gfx_GetPolyBaseColor: Null output pointers");
	UNPACK_RGB(g_rs.m_polyBaseColor, o_red, o_green, o_blue);
}

///////////////////////////////////////////////////
void Gfx_SetLightVector(uint8 i_index, uint16 i_x, uint16 i_y, uint16 i_z)
{
	g_rs.m_lightVectors.m[i_index][0] = i_x;
	g_rs.m_lightVectors.m[i_index][1] = i_y;
	g_rs.m_lightVectors.m[i_index][2] = i_z;

	DF_SET(DF_LIGHTS);
}

///////////////////////////////////////////////////
void Gfx_SetLightColor(uint8 i_index, uint32 i_red, uint32 i_green, uint32 i_blue)
{
	float r = (float)i_red / 255.0f;
	float g = (float)i_green / 255.0f;
	float b = (float)i_blue / 255.0f;

	g_rs.m_lightColors.m[0][i_index] = (int16)(r * ONE);
	g_rs.m_lightColors.m[1][i_index] = (int16)(g * ONE);
	g_rs.m_lightColors.m[2][i_index] = (int16)(b * ONE);

	DF_SET(DF_LIGHTS);
}

///////////////////////////////////////////////////
void Gfx_SetFogNearFar(uint32 i_near, uint32 i_far)
{
	g_rs.m_fog = i_near | (i_far << 16);
	SetFogNearFar(i_near, i_far, Gfx_GetDisplayWidth());
}

///////////////////////////////////////////////////
void Gfx_GetFogNearFar(uint32* o_near, uint32* o_far)
{
	VERIFY_ASSERT(o_near && o_far, "Gfx_GetFogNearFar: Null output pointers");
	*o_near = g_rs.m_fog & 0xffff;
	*o_far = g_rs.m_fog >> 16;
}

///////////////////////////////////////////////////
void Gfx_SetTexture(TextureMode i_textureMode, BlendRate i_blendRate, TPageAddress* i_tpageAddress, ClutAddress* i_clutAddress)
{
	VERIFY_ASSERT(i_tpageAddress, "TPageAddress must not be null!");

	{
		uint16 tpage = getTPage(i_textureMode, i_blendRate, i_tpageAddress->m_tpageX, i_tpageAddress->m_tpageY);
		uint16 clut = i_clutAddress ? getClut(i_clutAddress->m_clutX, i_clutAddress->m_clutY) : 0;
		Gfx_SetTextureDirect(tpage, clut);
	}
}

///////////////////////////////////////////////////
void Gfx_SetTextureScaleOffset(uint8 i_scaleU, uint8 i_scaleV, uint8 i_offsetU, uint8 i_offsetV)
{
	g_rs.m_textureScaleOffset = i_scaleU | (i_scaleV << 8) | (i_offsetU << 16) | (i_offsetV << 24);
}

///////////////////////////////////////////////////
void Gfx_GetTextureScaleOffset(uint8* o_scaleU, uint8* o_scaleV, uint8* o_offsetU, uint8* o_offsetV)
{
	VERIFY_ASSERT(o_scaleU && o_scaleV && o_offsetU && o_offsetV, "Gfx_GetTextureScaleOffset: Null output pointers");
	*o_scaleU =  g_rs.m_textureScaleOffset & 0xff;
	*o_scaleV =  (g_rs.m_textureScaleOffset >> 8) & 0xff;
	*o_offsetU = (g_rs.m_textureScaleOffset >> 16) & 0xff;
	*o_offsetV = (g_rs.m_textureScaleOffset >> 24) & 0xff;
}

///////////////////////////////////////////////////
void Gfx_SetTextureDirect(uint16 i_page, uint16 i_clut)
{
	g_rs.m_texture = i_page | (i_clut << 16);
}

///////////////////////////////////////////////////
void Gfx_GetTexture(uint16* o_page, uint16* o_clut)
{
	VERIFY_ASSERT(o_page && o_clut, "Gfx_GetTexture: Null output pointers");
	*o_page = g_rs.m_texture & 0xffff;
	*o_clut = g_rs.m_texture >> 16;
}

///////////////////////////////////////////////////
void Gfx_SetFogColor(uint8 i_red, uint8 i_green, uint8 i_blue)
{
	g_rs.m_fogColor = PACK_RGB(i_red, i_green, i_blue);
	gte_SetFarColor(i_red, i_green, i_blue);
}

///////////////////////////////////////////////////
void Gfx_GetFogColor(uint8* o_red, uint8* o_green, uint8* o_blue)
{
	VERIFY_ASSERT(o_red && o_green && o_blue, "Gfx_GetFogColor: Null output pointers");
	UNPACK_RGB(g_rs.m_fogColor, o_red, o_green, o_blue);
}

///////////////////////////////////////////////////
void Gfx_SetBackColor(uint8 i_red, uint8 i_green, uint8 i_blue)
{
	g_rs.m_backColor = PACK_RGB(i_red, i_green, i_blue);
	gte_SetBackColor(i_red, i_green, i_blue);
}

///////////////////////////////////////////////////
void Gfx_GetBackColor(uint8* o_red, uint8* o_green, uint8* o_blue)
{
	VERIFY_ASSERT(o_red && o_green && o_blue, "Gfx_GetBackColor: Null output pointers");
	UNPACK_RGB(g_rs.m_backColor, o_red, o_green, o_blue);
}

///////////////////////////////////////////////////
void Gfx_SetClearColor(uint8 i_red, uint8 i_green, uint8 i_blue)
{
	g_rs.m_clearColor = PACK_RGB(i_red, i_green, i_blue);
}

///////////////////////////////////////////////////
void Gfx_GetClearColor(uint8* o_red, uint8* o_green, uint8* o_blue)
{
	VERIFY_ASSERT(o_red && o_green && o_blue, "Gfx_GetClearColor: Null output pointers");
	UNPACK_RGB(g_rs.m_clearColor, o_red, o_green, o_blue);
}

///////////////////////////////////////////////////
void Gfx_SetDivisionParams(DivisionParams* i_params)
{
	g_rs.m_divisionParams = i_params;
}

///////////////////////////////////////////////////
void Gfx_GetDivisionParams(DivisionParams** o_params)
{
	VERIFY_ASSERT(o_params, "Gfx_GetDivisionParams: Null output pointers");
	*o_params = g_rs.m_divisionParams;
}
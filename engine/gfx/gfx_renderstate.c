// The render state
typedef struct
{
	uint32		m_flags;		// corresponds to a bitmask of RENDERSTATE
	uint32		m_fog;			// packed near, far
	uint32		m_fogColor;		// packed rgb color
	uint32		m_backColor;	// packed rgb back color
	uint32		m_clearColor;	// packed rgb clear color
	MATRIX		m_lightColors;	// #0, #1 and #2 light colors
	MATRIX		m_lightVectors;	// #0, #1 and #2 light vectors
}RenderState;

RenderState g_rs;

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
	*o_near = g_rs.m_fog & 0xffff;
	*o_far = g_rs.m_fog >> 16;
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
	UNPACK_RGB(g_rs.m_clearColor, o_red, o_green, o_blue);
}
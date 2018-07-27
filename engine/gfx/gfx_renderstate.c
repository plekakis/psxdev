// The render state
typedef struct
{
	uint32		m_flags;		// corresponds to a bitmask of RENDERSTATE
	uint32		m_fog;			// packed near, far
	uint32		m_fogColor;		// packed rgb color
}RenderState;

static RenderState g_rs;

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
void Gfx_SetFogColor(uint32 i_red, uint32 i_green, uint32 i_blue)
{
	g_rs.m_fogColor = PACK_RGB(i_red, i_green, i_blue);
	SetFarColor(i_red, i_green, i_blue);
}

///////////////////////////////////////////////////
void Gfx_GetFogColor(uint32* o_red, uint32* o_green, uint32* o_blue)
{
	UNPACK_RGB(g_rs.m_fogColor, o_red, o_green, o_blue);
}
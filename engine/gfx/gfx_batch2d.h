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
int16 Gfx_Batch2D_AddMode(Batch2D* io_batch, BlendRate i_blendRate)
{
	DR_MODE* mode = (DR_MODE*)io_batch->m_currentAddress;

	SetDrawMode(mode, 0, 0, getTPage(0, i_blendRate, 0, 0), 0);

	Gfx_Batch2D_Increment(io_batch, sizeof(DR_MODE));
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddTile(Batch2D* io_batch, DVECTOR* const i_position, DVECTOR* const i_size, CVECTOR* const i_color, PrimFlags i_flags)
{
	TILE* tile = (TILE*)io_batch->m_currentAddress;
	
	setTile(tile);
	setXY0(tile, i_position->vx, i_position->vy);
	setWH(tile, i_size->vx, i_size->vy);
	setRGB0(tile, i_color->r, i_color->g, i_color->b);
		
	Gfx_Batch2D_Increment(io_batch, sizeof(TILE));
	Gfx_Batch2D_UpdateFlags(tile, i_flags);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Batch2D_AddLineF(Batch2D* io_batch, DVECTOR* const i_start, DVECTOR* const i_end, CVECTOR* const i_color, PrimFlags i_flags)
{
	LINE_F2* line = (LINE_F2*)io_batch->m_currentAddress;

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

	setLineG2(line);
	setXY2(line, i_start->vx, i_start->vy, i_end->vx, i_end->vy);
	setRGB0(line, i_startColor->r, i_startColor->g, i_startColor->b);
	setRGB1(line, i_endColor->r, i_endColor->g, i_endColor->b);

	Gfx_Batch2D_UpdateFlags(line, i_flags);
	Gfx_Batch2D_Increment(io_batch, sizeof(LINE_G2));
	return E_OK;
}
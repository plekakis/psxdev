///////////////////////////////////////////////////
int16 Gfx_BeginBatch2D(Batch2D* o_batch, uint32 i_batchSizeInBytes)
{
	o_batch->m_baseAddress = Gfx_Alloc(i_batchSizeInBytes, 4);
	o_batch->m_currentAddress = o_batch->m_baseAddress;
	o_batch->m_sizeInBytes = i_batchSizeInBytes;
	o_batch->m_prevPrimSize = 0u;
	return E_OK;
}


///////////////////////////////////////////////////
int16 Gfx_EndBatch2D(Batch2D* i_batch)
{
	void* const start = i_batch->m_baseAddress;
	void* const end = i_batch->m_currentAddress - i_batch->m_prevPrimSize;
	
	// Terminate the primitive linkage and submit it to the OT
	termPrim(end);
	addPrims(Gfx_GetCurrentOT(), start, end);
	return E_OK;
}
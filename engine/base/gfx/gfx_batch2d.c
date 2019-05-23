#include "gfx.h"

///////////////////////////////////////////////////
int16 Gfx_BeginBatch2D(Batch2D* o_batch, uint32 i_batchSizeInBytes)
{
	o_batch->m_baseAddress = Gfx_Alloc(i_batchSizeInBytes + sizeof(DR_MODE), 4);
	o_batch->m_currentAddress = o_batch->m_baseAddress;
	o_batch->m_sizeInBytes = i_batchSizeInBytes;
	o_batch->m_prevPrimSize = 0u;
	return E_OK;
}


///////////////////////////////////////////////////
int16 Gfx_EndBatch2D(Batch2D* i_batch)
{
	// Add the reset DR_MODE
	{		
		DR_MODE* mode = (DR_MODE*)i_batch->m_currentAddress;
		BATCH2D_VERIFY(i_batch, mode, DR_MODE);

		Gfx_GetDefaultDrawMode(mode);
		Gfx_Batch2D_Increment(i_batch, sizeof(DR_MODE));
	}

	// End the batch and submit
	{
		void* const start = i_batch->m_baseAddress;
		void* const end = i_batch->m_currentAddress - i_batch->m_prevPrimSize;

		VERIFY_ASSERT((uint8*)end <= ((uint8*)start + i_batch->m_sizeInBytes), "Gfx_EndBatch2D: End pointer out of scratch memory range!");

		// Terminate the primitive linkage and submit it to the OT
		termPrim(end);

		if (start != end)
		{
			addPrims(Gfx_GetCurrentOT(), start, end);
		}
	}
	return E_OK;
}
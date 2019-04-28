#include "res.h"
#include "../gfx/gfx.h"
#include "../stream/stream.h"
#include "../core/core.h"
#include "../util/util.h"

///////////////////////////////////////////////////
void ReadF3(TMD_PRIM* i_prim, ResModel* io_model, uint16 i_currentPoly)
{
	// Allocation
	if (!io_model->m_data.m_f3)
	{
		io_model->m_data.m_f3 = Core_Malloc(sizeof(PRIM_F3) * io_model->m_polyCount, 4);
	}

	// Read
	{
		PRIM_F3* poly = io_model->m_data.m_f3 + i_currentPoly;
		SVECTOR normal;
		int32 winding;

		Util_FaceNormal(&i_prim->n0, &i_prim->n1, &i_prim->n2, &normal);
		winding = Util_GetTriangleWinding(&i_prim->x0, &i_prim->x1, &i_prim->x2, &normal);

		// Copy vertex
		copyVector(&poly->v0, &i_prim->x0);
		copyVector(&poly->v1, (winding > 0) ? &i_prim->x1 : &i_prim->x2);
		copyVector(&poly->v2, (winding > 0) ? &i_prim->x2 : &i_prim->x1);

		// Copy color from v0
		setColor(&poly->c0, i_prim->r0, i_prim->g0, i_prim->b0);
		
		// Copy normal from v0
		copyVector(&poly->n0, &i_prim->n0);
	}
}

///////////////////////////////////////////////////
void ReadFT3(TMD_PRIM* i_prim, ResModel* io_model, uint16 i_currentPoly)
{
	// Allocation
	if (!io_model->m_data.m_ft3)
	{
		io_model->m_data.m_ft3 = Core_Malloc(sizeof(PRIM_FT3) * io_model->m_polyCount, 4);
	}

	// Read
	{
		PRIM_FT3* poly = io_model->m_data.m_ft3 + i_currentPoly;
		SVECTOR normal;
		int32 winding;

		Util_FaceNormal(&i_prim->n0, &i_prim->n1, &i_prim->n2, &normal);
		winding = Util_GetTriangleWinding(&i_prim->x0, &i_prim->x1, &i_prim->x2, &normal);

		// Copy vertex
		copyVector(&poly->v0, &i_prim->x0);
		copyVector(&poly->v1, (winding > 0) ? &i_prim->x1 : &i_prim->x2);
		copyVector(&poly->v2, (winding > 0) ? &i_prim->x2 : &i_prim->x1);

		// Copy color from v0
		setColor(&poly->c0, i_prim->r0, i_prim->g0, i_prim->b0);

		// Copy normal from v0
		copyVector(&poly->n0, &i_prim->n0);

		// Copy UV
		poly->uv0.u = i_prim->u0;
		poly->uv0.v = i_prim->v0;

		poly->uv1.u = (winding > 0) ? i_prim->u1 : i_prim->u2;
		poly->uv1.v = (winding > 0) ? i_prim->v1 : i_prim->v2;

		poly->uv2.u = (winding > 0) ? i_prim->u2 : i_prim->u1;
		poly->uv2.v = (winding > 0) ? i_prim->v2 : i_prim->v1;
	}
}

///////////////////////////////////////////////////
void ReadG3(TMD_PRIM* i_prim, ResModel* io_model, uint16 i_currentPoly)
{
	// Allocation
	if (!io_model->m_data.m_g3)
	{
		io_model->m_data.m_g3 = Core_Malloc(sizeof(PRIM_G3) * io_model->m_polyCount, 4);
	}

	// Read
	{
		PRIM_G3* poly = io_model->m_data.m_g3 + i_currentPoly;
		SVECTOR normal;
		int32 winding;

		Util_FaceNormal(&i_prim->n0, &i_prim->n1, &i_prim->n2, &normal);
		winding = Util_GetTriangleWinding(&i_prim->x0, &i_prim->x1, &i_prim->x2, &normal);

		// Copy vertex		
		copyVector(&poly->v0, &i_prim->x0);
		copyVector(&poly->v1, (winding > 0) ? &i_prim->x1 : &i_prim->x2);
		copyVector(&poly->v2, (winding > 0) ? &i_prim->x2 : &i_prim->x1);

		// Copy color
		setColor(&poly->c0, i_prim->r0, i_prim->g0, i_prim->b0);
		if (winding > 0)
		{			
			setColor(&poly->c1, i_prim->r1, i_prim->g1, i_prim->b1);
			setColor(&poly->c2, i_prim->r2, i_prim->g2, i_prim->b2);
		}
		else
		{
			setColor(&poly->c1, i_prim->r2, i_prim->g2, i_prim->b2);
			setColor(&poly->c2, i_prim->r1, i_prim->g1, i_prim->b1);
		}

		// Copy normal
		copyVector(&poly->n0, &i_prim->n0);
		copyVector(&poly->n1, (winding > 0) ? &i_prim->n1 : &i_prim->n2);
		copyVector(&poly->n2, (winding > 0) ? &i_prim->n2 : &i_prim->n1);
	}
}

///////////////////////////////////////////////////
void ReadGT3(TMD_PRIM* i_prim, ResModel* io_model, uint16 i_currentPoly)
{
	// Allocation
	if (!io_model->m_data.m_gt3)
	{
		io_model->m_data.m_gt3 = Core_Malloc(sizeof(PRIM_GT3) * io_model->m_polyCount, 4);
	}

	// Read
	{
		PRIM_GT3* poly = io_model->m_data.m_gt3 + i_currentPoly;
		SVECTOR normal;
		int32 winding;

		Util_FaceNormal(&i_prim->n0, &i_prim->n1, &i_prim->n2, &normal);
		winding = Util_GetTriangleWinding(&i_prim->x0, &i_prim->x1, &i_prim->x2, &normal);

		// Copy vertex
		copyVector(&poly->v0, &i_prim->x0);
		copyVector(&poly->v1, (winding > 0) ? &i_prim->x1 : &i_prim->x2);
		copyVector(&poly->v2, (winding > 0) ? &i_prim->x2 : &i_prim->x1);

		// Copy color
		setColor(&poly->c0, i_prim->r0, i_prim->g0, i_prim->b0);
		if (winding > 0)
		{
			setColor(&poly->c1, i_prim->r1, i_prim->g1, i_prim->b1);
			setColor(&poly->c2, i_prim->r2, i_prim->g2, i_prim->b2);
		}
		else
		{
			setColor(&poly->c1, i_prim->r2, i_prim->g2, i_prim->b2);
			setColor(&poly->c2, i_prim->r1, i_prim->g1, i_prim->b1);
		}

		// Copy normal
		copyVector(&poly->n0, &i_prim->n0);
		copyVector(&poly->n1, &i_prim->n1);
		copyVector(&poly->n2, &i_prim->n2);

		// Copy UV
		poly->uv0.u = i_prim->u0;
		poly->uv0.v = i_prim->v0;

		poly->uv1.u = (winding > 0) ? i_prim->u1 : i_prim->u2;
		poly->uv1.v = (winding > 0) ? i_prim->v1 : i_prim->v2;

		poly->uv2.u = (winding > 0) ? i_prim->u2 : i_prim->u1;
		poly->uv2.v = (winding > 0) ? i_prim->v2 : i_prim->v1;
	}
}

///////////////////////////////////////////////////
int16 Res_LoadTMD(void* i_srcAddress, PRIM_TYPE i_primType, ResModel* o_model)
{
	uint16 polyCount = 0u;
	uint16 currentPoly = 0u;
	TMD_PRIM prim;
	
	VERIFY_ASSERT(i_srcAddress, "Source address is null");
	VERIFY_ASSERT(o_model, "Destination model is null");

	// Open the TMD and get the poly count.
	if ( (polyCount = OpenTMD((uint32*)i_srcAddress, 0)) == 0)
		return E_INVALIDARGS;
	
	REPORT("Res_LoadTMD: Loading model with %u polygons...", polyCount);

	Util_MemZero(o_model, sizeof(ResModel));
	
	// Assuming triangle topology, read all the data, depending on the primitive type.
	o_model->m_polyCount = polyCount;
	o_model->m_primType = i_primType;

	while (ReadTMD(&prim) != NULL)
	{
		switch (i_primType)
		{
		case PRIM_TYPE_POLY_F3:
			ReadF3(&prim, o_model, currentPoly);
			break;

		case PRIM_TYPE_POLY_FT3:
			ReadFT3(&prim, o_model, currentPoly);
			break;

		case PRIM_TYPE_POLY_G3:
			ReadG3(&prim, o_model, currentPoly);
			break;

		case PRIM_TYPE_POLY_GT3:
			ReadGT3(&prim, o_model, currentPoly);
			break;
		default:
			VERIFY_ASSERT(FALSE, "Invalid PRIM_TYPE");
			break;
		}

		++currentPoly;
	}
	return E_OK;
}

///////////////////////////////////////////////////
int16 Res_ReadLoadTMD(StringId i_filename, PRIM_TYPE i_primType, ResModel* o_model)
{
	int16 res = E_OK;
	void* ptr;

	// Read the file off disc
	Stream_BeginRead(i_filename, &ptr);

	Stream_ReadFileBlocking();

	// Load the TIM
	res = Res_LoadTMD(ptr, i_primType, o_model);

	Stream_EndRead();
	return res;
}

///////////////////////////////////////////////////
int16 Res_FreeTMD(ResModel* io_model)
{
	VERIFY_ASSERT(io_model, "Input model is null");

	// Free correct memory
	switch (io_model->m_primType)
	{
	case PRIM_TYPE_POLY_F3:
		Core_Free(io_model->m_data.m_f3);
		break;

	case PRIM_TYPE_POLY_FT3:
		Core_Free(io_model->m_data.m_ft3);
		break;

	case PRIM_TYPE_POLY_G3:
		Core_Free(io_model->m_data.m_g3);
		break;

	case PRIM_TYPE_POLY_GT3:
		Core_Free(io_model->m_data.m_gt3);
		break;
	default:
		break;
	}
}
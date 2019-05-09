#include "res.h"
#include "../gfx/gfx.h"
#include "../stream/stream.h"
#include "../core/core.h"
#include "../util/util.h"

//
// Helper loading macros
//

#define TMD_DECL(prim, type) \
	SVECTOR normal; \
	int32 winding; \
	type* poly = NULL; \
	Util_FaceNormal(&(prim)->n0, &(prim)->n1, &(prim)->n2, &normal); \
	winding = Util_GetTriangleWinding(&(prim)->x0, &(prim)->x1, &(prim)->x2, &normal); \
	if (!io_model->m_data) \
	{ \
		io_model->m_data = Core_Malloc(sizeof(type) * io_model->m_polyCount, 4); \
	} \
	poly = (type*)io_model->m_data + i_currentPoly;

#define TMD_COPY_POS(poly, prim, index) \
	copyVector(&(poly)->v ## index, &(prim)->x ## index)

#define TMD_COPY_POS3(poly, prim) \
	copyVector(&(poly)->v0, &(prim)->x0); \
	copyVector(&(poly)->v1, (winding > 0) ? &(prim)->x1 : &(prim)->x2); \
	copyVector(&(poly)->v2, (winding > 0) ? &(prim)->x2 : &(prim)->x1);

#define TMD_COPY_COL(poly, prim, index) \
	setColor(&(poly)->c ## index, (prim)->r ## index, (prim)->g ## index, (prim)->b ## index);

#define TMD_COPY_COL3(poly, prim) \
	setColor(&(poly)->c0, (prim)->r0, (prim)->g0, (prim)->b0); \
	setColor(&(poly)->c1, (winding > 0) ? (prim)->r1 : (prim)->r2, (winding > 0) ? (prim)->g1 : (prim)->g2, (winding > 0) ? (prim)->b1 : (prim)->b2); \
	setColor(&(poly)->c2, (winding > 0) ? (prim)->r2 : (prim)->r1, (winding > 0) ? (prim)->g2 : (prim)->g1, (winding > 0) ? (prim)->b2 : (prim)->b1);

#define TMD_COPY_NORM(poly, prim, index) \
	copyVector(&(poly)->n ## index, &(prim)->n ## index);

#define TMD_COPY_NORM3(poly, prim) \
	copyVector(&(poly)->n0, &(prim)->n0); \
	copyVector(&(poly)->n1, (winding > 0) ? &(prim)->n1 : &(prim)->n2); \
	copyVector(&(poly)->n2, (winding > 0) ? &(prim)->n2 : &(prim)->n1);

#define TMD_COPY_UV(poly, prim, index) \
	(poly)->uv ## index.u = (prim)->u ## index; \
	(poly)->uv ## index.v = (prim)->v ## index; \

#define TMD_COPY_UV3(poly, prim) \
	(poly)->uv0.u = (prim)->u0; \
	(poly)->uv0.v = (prim)->v0; \
	(poly)->uv1.u = (winding > 0) ? (prim)->u1 : (prim)->u2; \
	(poly)->uv1.v = (winding > 0) ? (prim)->v1 : (prim)->v2; \
	(poly)->uv2.u = (winding > 0) ? (prim)->u2 : (prim)->u1; \
	(poly)->uv2.v = (winding > 0) ? (prim)->v2 : (prim)->v1;

///////////////////////////////////////////////////
void ReadF3(TMD_PRIM* i_prim, ResModel* io_model, uint16 i_currentPoly)
{
	// Alloc and prepare poly
	TMD_DECL(i_prim, PRIM_F3);

	// Copy vertex
	TMD_COPY_POS3(poly, i_prim);

	// Copy color from v0
	TMD_COPY_COL(poly, i_prim, 0);
		
	// Copy normal from v0
	TMD_COPY_NORM(poly, i_prim, 0);
}

///////////////////////////////////////////////////
void ReadFT3(TMD_PRIM* i_prim, ResModel* io_model, uint16 i_currentPoly)
{	
	// Alloc and prepare poly
	TMD_DECL(i_prim, PRIM_FT3);

	// Copy position
	TMD_COPY_POS3(poly, i_prim);

	// Copy color from v0
	TMD_COPY_COL(poly, i_prim, 0);

	// Copy normal from v0
	TMD_COPY_NORM(poly, i_prim, 0);

	// Copy UV
	TMD_COPY_UV3(poly, i_prim);
}

///////////////////////////////////////////////////
void ReadG3(TMD_PRIM* i_prim, ResModel* io_model, uint16 i_currentPoly)
{
	// Alloc and prepare poly
	TMD_DECL(i_prim, PRIM_G3);

	// Copy vertex		
	TMD_COPY_POS3(poly, i_prim);

	// Copy color
	TMD_COPY_COL3(poly, i_prim);

	// Copy normal
	TMD_COPY_NORM3(poly, i_prim);
}

///////////////////////////////////////////////////
void ReadGT3(TMD_PRIM* i_prim, ResModel* io_model, uint16 i_currentPoly)
{
	// Alloc and prepare poly
	TMD_DECL(i_prim, PRIM_GT3);

	// Copy vertex
	TMD_COPY_POS3(poly, i_prim);

	// Copy color
	TMD_COPY_COL3(poly, i_prim);

	// Copy normal
	TMD_COPY_NORM3(poly, i_prim);

	// Copy UV
	TMD_COPY_UV3(poly, i_prim);
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

	// Load the TMD
	res = Res_LoadTMD(ptr, i_primType, o_model);

	Stream_EndRead();
	return res;
}

///////////////////////////////////////////////////
int16 Res_FreeTMD(ResModel* io_model)
{
	VERIFY_ASSERT(io_model, "Input model is null");
	Core_Free(io_model->m_data);
}
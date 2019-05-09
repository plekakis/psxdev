#include "res.h"
#include "../gfx/gfx.h"
#include "../stream/stream.h"
#include "../core/core.h"
#include "../util/util.h"

typedef struct
{
	uint32 magic;
	uint8  submeshCount;
	uint8  pad[3];
}PSM_HEADER;

typedef struct
{
	uint16 triangleCount;
	uint8  type;
	uint8  pad[1];
}PSM_SUBMESH;

typedef struct
{
	uint8	r0, g0, b0, p0;
	SVECTOR	x0, x1, x2, n0;
}PSM_F3;

typedef struct
{
	uint8	r0, g0, b0, p0;
	uint8	u0, v0, u1, v1;
	uint8	u2, v2, uvp0, uvp1;
	SVECTOR	x0, x1, x2, n0;
}PSM_FT3;

typedef struct
{
	uint8	r0, g0, b0, p0;
	uint8	r1, g1, b1, p1;
	uint8	r2, g2, b2, p2;

	SVECTOR	x0, x1, x2, xpad;
	SVECTOR	n0, n1, n2, npad;
}PSM_G3;

typedef struct
{
	uint8	r0, g0, b0, p0;
	uint8	r1, g1, b1, p1;
	uint8	r2, g2, b2, p2;
	uint8	u0, v0, u1, v1;
	uint8	u2, v2, uvp0, uvp1;

	SVECTOR	x0, x1, x2, xpad;
	SVECTOR	n0, n1, n2, npad;
}PSM_GT3;

#define PSM_COPY_POS3(poly, prim) \
	copyVector(&(poly)->v0, &(prim)->x0); \
	copyVector(&(poly)->v1, &(prim)->x1); \
	copyVector(&(poly)->v2, &(prim)->x2);

#define PSM_COPY_COL(poly, prim, index) \
	setColor(&(poly)->c ## index, (prim)->r ## index, (prim)->g ## index, (prim)->b ## index);

#define PSM_COPY_COL3(poly, prim) \
	setColor(&(poly)->c0, (prim)->r0, (prim)->g0, (prim)->b0); \
	setColor(&(poly)->c1, (prim)->r1, (prim)->g1, (prim)->b1); \
	setColor(&(poly)->c2, (prim)->r2, (prim)->g2, (prim)->b2);

#define PSM_COPY_NORM(poly, prim, index) \
	copyVector(&(poly)->n ## index, &(prim)->n ## index);

#define PSM_COPY_NORM3(poly, prim) \
	copyVector(&(poly)->n0, &(prim)->n0); \
	copyVector(&(poly)->n1, &(prim)->n1); \
	copyVector(&(poly)->n2, &(prim)->n2);

#define PSM_COPY_UV(poly, prim, index) \
	(poly)->uv ## index.u = (prim)->u ## index; \
	(poly)->uv ## index.v = (prim)->v ## index; \

#define PSM_COPY_UV3(poly, prim) \
	(poly)->uv0.u = (prim)->u0; \
	(poly)->uv0.v = (prim)->v0; \
	(poly)->uv1.u = (prim)->u1; \
	(poly)->uv1.v = (prim)->v1; \
	(poly)->uv2.u = (prim)->u2; \
	(poly)->uv2.v = (prim)->v2;

///////////////////////////////////////////////////
void PSM_ReportF3(uint16 i_triIndex, uint8* i_vertexData)
{
	PRIM_F3* f3 = (PRIM_F3*)i_vertexData;

	REPORT("[F3 TRI %u]", i_triIndex);
	REPORT("r: %u, g: %u, b: %u", f3->c0.r, f3->c0.g, f3->c0.b);
	REPORT("x0: %i, y0: %i, z0: %i", f3->v0.vx, f3->v0.vy, f3->v0.vz);
	REPORT("x1: %i, y1: %i, z1: %i", f3->v1.vx, f3->v1.vy, f3->v1.vz);
	REPORT("x2: %i, y2: %i, z2: %i", f3->v2.vx, f3->v2.vy, f3->v2.vz);
	REPORT("n: %i, n: %i, n: %i", f3->n0.vx, f3->n0.vy, f3->n0.vz);
}

///////////////////////////////////////////////////
void PSM_ReportFT3(uint16 i_triIndex, uint8* i_vertexData)
{
	PRIM_FT3* ft3 = (PRIM_FT3*)i_vertexData;

	REPORT("[FT3 TRI %u]", i_triIndex);
	REPORT("r: %u, g: %u, b: %u", ft3->c0.r, ft3->c0.g, ft3->c0.b);
	REPORT("x0: %i, y0: %i, z0: %i", ft3->v0.vx, ft3->v0.vy, ft3->v0.vz);
	REPORT("x1: %i, y1: %i, z1: %i", ft3->v1.vx, ft3->v1.vy, ft3->v1.vz);
	REPORT("x2: %i, y2: %i, z2: %i", ft3->v2.vx, ft3->v2.vy, ft3->v2.vz);
	REPORT("u0: %i, v0: %i", ft3->uv0.u, ft3->uv0.v);
	REPORT("u1: %i, v1: %i", ft3->uv1.u, ft3->uv1.v);
	REPORT("u2: %i, v2: %i", ft3->uv2.u, ft3->uv2.v);
	REPORT("n: %i, n: %i, n: %i", ft3->n0.vx, ft3->n0.vy, ft3->n0.vz);
}

///////////////////////////////////////////////////
void PSM_ReportG3(uint16 i_triIndex, uint8* i_vertexData)
{
	PRIM_G3* g3 = (PRIM_G3*)i_vertexData;

	REPORT("[G3 TRI %u]", i_triIndex);
	REPORT("r0: %u, g0: %u, b0: %u", g3->c0.r, g3->c0.g, g3->c0.b);
	REPORT("r1: %u, g1: %u, b1: %u", g3->c1.r, g3->c1.g, g3->c1.b);
	REPORT("r2: %u, g2: %u, b2: %u", g3->c2.r, g3->c2.g, g3->c2.b);
	REPORT("x0: %i, y0: %i, z0: %i", g3->v0.vx, g3->v0.vy, g3->v0.vz);
	REPORT("x1: %i, y1: %i, z1: %i", g3->v1.vx, g3->v1.vy, g3->v1.vz);
	REPORT("x2: %i, y2: %i, z2: %i", g3->v2.vx, g3->v2.vy, g3->v2.vz);
	REPORT("n0: %i, n0: %i, n0: %i", g3->n0.vx, g3->n0.vy, g3->n0.vz);
	REPORT("n1: %i, n1: %i, n1: %i", g3->n1.vx, g3->n1.vy, g3->n1.vz);
	REPORT("n2: %i, n2: %i, n2: %i", g3->n2.vx, g3->n2.vy, g3->n2.vz);
}

///////////////////////////////////////////////////
void PSM_ReportGT3(uint16 i_triIndex, uint8* i_vertexData)
{
	PRIM_GT3* gt3 = (PRIM_GT3*)i_vertexData;

	REPORT("[GT3 TRI %u]", i_triIndex);
	REPORT("r0: %u, g0: %u, b0: %u", gt3->c0.r, gt3->c0.g, gt3->c0.b);
	REPORT("r1: %u, g1: %u, b1: %u", gt3->c1.r, gt3->c1.g, gt3->c1.b);
	REPORT("r2: %u, g2: %u, b2: %u", gt3->c2.r, gt3->c2.g, gt3->c2.b);
	REPORT("x0: %i, y0: %i, z0: %i", gt3->v0.vx, gt3->v0.vy, gt3->v0.vz);
	REPORT("x1: %i, y1: %i, z1: %i", gt3->v1.vx, gt3->v1.vy, gt3->v1.vz);
	REPORT("x2: %i, y2: %i, z2: %i", gt3->v2.vx, gt3->v2.vy, gt3->v2.vz);
	REPORT("u0: %i, v0: %i", gt3->uv0.u, gt3->uv0.v);
	REPORT("u1: %i, v1: %i", gt3->uv1.u, gt3->uv1.v);
	REPORT("u2: %i, v2: %i", gt3->uv2.u, gt3->uv2.v);
	REPORT("n0: %i, n0: %i, n0: %i", gt3->n0.vx, gt3->n0.vy, gt3->n0.vz);
	REPORT("n1: %i, n1: %i, n1: %i", gt3->n1.vx, gt3->n1.vy, gt3->n1.vz);
	REPORT("n2: %i, n2: %i, n2: %i", gt3->n2.vx, gt3->n2.vy, gt3->n2.vz);
}

///////////////////////////////////////////////////
void PSM_ReadF3(uint8* io_srcData, uint8* io_vertexData)
{
	PRIM_F3* dst = (PRIM_F3*)io_vertexData;
	PSM_F3* src = (PSM_F3*)io_srcData;

	// Copy position
	PSM_COPY_POS3(dst, src);

	// Copy color from v0
	PSM_COPY_COL(dst, src, 0);

	// Copy normal from v0
	PSM_COPY_NORM(dst, src, 0);

	dst++; src++;
}

///////////////////////////////////////////////////
void PSM_ReadFT3(uint8* io_srcData, uint8* io_vertexData)
{
	PRIM_FT3* dst = (PRIM_FT3*)io_vertexData;
	PSM_FT3* src = (PSM_FT3*)io_srcData;

	// Copy position
	PSM_COPY_POS3(dst, src);

	// Copy color from v0
	PSM_COPY_COL(dst, src, 0);

	// Copy normal from v0
	PSM_COPY_NORM(dst, src, 0);

	// Copy UV
	PSM_COPY_UV3(dst, src);

	dst++; src++;
}

///////////////////////////////////////////////////
void PSM_ReadG3(uint8* io_srcData, uint8* io_vertexData)
{
	PRIM_G3* dst = (PRIM_G3*)io_vertexData;
	PSM_G3* src = (PSM_G3*)io_srcData;

	// Copy position		
	PSM_COPY_POS3(dst, src);

	// Copy color
	PSM_COPY_COL3(dst, src);

	// Copy normal
	PSM_COPY_NORM3(dst, src);

	dst++; src++;
}

///////////////////////////////////////////////////
void PSM_ReadGT3(uint8* io_srcData, uint8* io_vertexData)
{
	PRIM_GT3* dst = (PRIM_GT3*)io_vertexData;
	PSM_GT3* src = (PSM_GT3*)io_srcData;

	// Copy position
	PSM_COPY_POS3(dst, src);

	// Copy color
	PSM_COPY_COL3(dst, src);

	// Copy normal
	PSM_COPY_NORM3(dst, src);

	// Copy UV
	PSM_COPY_UV3(dst, src);

	dst++; src++;
}

///////////////////////////////////////////////////
int16 Res_LoadPSM(void* i_srcAddress, ResModel2* o_model)
{
	uint32 submeshIndex = 0u;
	uint8* ptr = (uint8*)i_srcAddress;

	PSM_HEADER* header = (PSM_HEADER*)ptr; ptr += sizeof(PSM_HEADER);
	
	REPORT("magic: %x, subMeshCount: %u", header->magic, header->submeshCount);

	o_model->m_submeshCount = header->submeshCount;
	o_model->m_submeshes = Core_Malloc(sizeof(ResModel) * o_model->m_submeshCount, 4);

	for (submeshIndex=0; submeshIndex < o_model->m_submeshCount; ++submeshIndex)
	{
		uint32 triIndex = 0u;
		ResModel* mdl = &o_model->m_submeshes[submeshIndex];
		PSM_SUBMESH* submesh = (PSM_SUBMESH*)ptr; ptr += sizeof(PSM_SUBMESH);
		
		mdl->m_polyCount = submesh->triangleCount;
		mdl->m_primType = submesh->type;

		mdl->m_data = Core_Malloc(sizeof(PRIM_F3) * mdl->m_polyCount, 4);
		REPORT("[SUBMESH %u] poly count: %u, prim type: %u", submeshIndex, mdl->m_polyCount, mdl->m_primType);

		for (triIndex = 0; triIndex < mdl->m_polyCount; ++triIndex)
		{
			uint8* data = (uint8*)mdl->m_data;

			switch (mdl->m_primType)
			{
			case PRIM_TYPE_POLY_F3:
				PSM_ReadF3(ptr, data);
				PSM_ReportF3(triIndex, data);
				break;

			case PRIM_TYPE_POLY_FT3:
				PSM_ReadFT3(ptr, data);
				PSM_ReportFT3(triIndex, data);
				break;

			case PRIM_TYPE_POLY_G3:
				PSM_ReadG3(ptr, data);
				PSM_ReportG3(triIndex, data);
				break;

			case PRIM_TYPE_POLY_GT3:
				PSM_ReadGT3(ptr, data);
				PSM_ReportGT3(triIndex, data);
				break;

			default:
				VERIFY_ASSERT(FALSE, "Invalid PRIM_TYPE");
				break;
			}			
		}
	}
	return E_OK;
}

///////////////////////////////////////////////////
int16 Res_ReadLoadPSM(StringId i_filename, ResModel2* o_model)
{
	int16 res = E_OK;
	void* ptr;

	// Read the file off disc
	Stream_BeginRead(i_filename, &ptr);

	Stream_ReadFileBlocking();

	// Load the PSM
	res = Res_LoadPSM(ptr, o_model);

	Stream_EndRead();
	return res;
}

///////////////////////////////////////////////////
int16 Res_FreePSM(ResModel2* io_model)
{
	VERIFY_ASSERT(io_model, "Input model is null");
	
	{
		uint32 submeshIndex = 0;
		for (submeshIndex = 0; submeshIndex < io_model->m_submeshCount; ++submeshIndex)
		{
			Core_Free(io_model->m_submeshes[submeshIndex].m_data);
			Core_Free(io_model->m_submeshes);
		}
	}
}
#include "res.h"
#include <base/gfx/gfx.h>
#include <base/stream/stream.h>
#include <base/core/core.h>
#include <base/util/util.h>

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
	uint8	r1, g1, b1, p1;
	uint8	r2, g2, b2, p2;
	uint8	u0, v0, u1, v1;
	uint8	u2, v2, uvp0, uvp1;

	SVECTOR	x0, x1, x2, xpad;
	SVECTOR	n0, n1, n2, npad;
}PSM_VERTEX;

static uint32 s_engineVertexSizes[] = { sizeof(PRIM_F3), sizeof(PRIM_FT3), sizeof(PRIM_G3), sizeof(PRIM_GT3) };

#define PSM_DECLARE(type) \
	PRIM_ ## type* dst = (PRIM_ ## type*)io_vertexData; \
	PSM_VERTEX* src = (PSM_VERTEX*)io_srcData;

#define PSM_COPY_POS3 \
	copyVector(&dst->v0, &src->x0); \
	copyVector(&dst->v1, &src->x1); \
	copyVector(&dst->v2, &src->x2);

#define PSM_COPY_COL(index) \
	setColor(&dst->c ## index, src->r ## index, src->g ## index, src->b ## index);

#define PSM_COPY_COL3 \
	setColor(&dst->c0, src->r0, src->g0, src->b0); \
	setColor(&dst->c1, src->r1, src->g1, src->b1); \
	setColor(&dst->c2, src->r2, src->g2, src->b2);

#define PSM_COPY_NORM(index) \
	copyVector(&dst->n ## index, &src->n ## index);

#define PSM_COPY_NORM3 \
	copyVector(&dst->n0, &src->n0); \
	copyVector(&dst->n1, &src->n1); \
	copyVector(&dst->n2, &src->n2);

#define PSM_COPY_UV(poly, prim, index) \
	dst->uv ## index.u = src->u ## index; \
	dst->uv ## index.v = src->v ## index; \

#define PSM_COPY_UV3 \
	dst->uv0.u = src->u0; \
	dst->uv0.v = src->v0; \
	dst->uv1.u = src->u1; \
	dst->uv1.v = src->v1; \
	dst->uv2.u = src->u2; \
	dst->uv2.v = src->v2;

///////////////////////////////////////////////////
void PSM_ReportF3(uint16 i_triIndex, uint8* i_vertexData)
{
#if RES_VERBOSE_MODEL_LOADING_DATA
	PRIM_F3* f3 = (PRIM_F3*)i_vertexData;

	REPORT("[F3 TRI %u]", i_triIndex);
	REPORT("[r: %u, g: %u, b: %u]", f3->c0.r, f3->c0.g, f3->c0.b);
	REPORT("[x0: %i, y0: %i, z0: %i],[x1: %i, y1: %i, z1: %i],[x2: %i, y2: %i, z2: %i]", f3->v0.vx, f3->v0.vy, f3->v0.vz, f3->v1.vx, f3->v1.vy, f3->v1.vz, f3->v2.vx, f3->v2.vy, f3->v2.vz);
	REPORT("[nx: %i, ny: %i, nz: %i]", f3->n0.vx, f3->n0.vy, f3->n0.vz);
#endif // RES_VERBOSE_MODEL_LOADING_DATA
}

///////////////////////////////////////////////////
void PSM_ReportFT3(uint16 i_triIndex, uint8* i_vertexData)
{
#if RES_VERBOSE_MODEL_LOADING_DATA
	PRIM_FT3* ft3 = (PRIM_FT3*)i_vertexData;

	REPORT("[FT3 TRI %u]", i_triIndex);
	REPORT("[r: %u, g: %u, b: %u]", ft3->c0.r, ft3->c0.g, ft3->c0.b);
	REPORT("[x0: %i, y0: %i, z0: %i],[x1: %i, y1: %i, z1: %i],[x2: %i, y2: %i, z2: %i]", ft3->v0.vx, ft3->v0.vy, ft3->v0.vz, ft3->v1.vx, ft3->v1.vy, ft3->v1.vz, ft3->v2.vx, ft3->v2.vy, ft3->v2.vz);
	REPORT("[u0: %i, v0: %i],[u1: %i, v1: %i],[u2: %i, v2: %i]", ft3->uv0.u, ft3->uv0.v, ft3->uv1.u, ft3->uv1.v, ft3->uv2.u, ft3->uv2.v);
	REPORT("[nx: %i, ny: %i, nz: %i]", ft3->n0.vx, ft3->n0.vy, ft3->n0.vz);
#endif // RES_VERBOSE_MODEL_LOADING_DATA
}

///////////////////////////////////////////////////
void PSM_ReportG3(uint16 i_triIndex, uint8* i_vertexData)
{
#if RES_VERBOSE_MODEL_LOADING_DATA
	PRIM_G3* g3 = (PRIM_G3*)i_vertexData;

	REPORT("[G3 TRI %u]", i_triIndex);
	REPORT("[r0: %u, g0: %u, b0: %u],[r1: %u, g1: %u, b1: %u],[r2: %u, g2: %u, b2: %u]", g3->c0.r, g3->c0.g, g3->c0.b, g3->c1.r, g3->c1.g, g3->c1.b, g3->c2.r, g3->c2.g, g3->c2.b);
	REPORT("[x0: %i, y0: %i, z0: %i],[x1: %i, y1: %i, z1: %i],[x2: %i, y2: %i, z2: %i]", g3->v0.vx, g3->v0.vy, g3->v0.vz, g3->v1.vx, g3->v1.vy, g3->v1.vz, g3->v2.vx, g3->v2.vy, g3->v2.vz);
	REPORT("[nx0: %i, ny0: %i, nz0: %i],[nx1: %i, ny1: %i, nz1: %i],[nx2: %i, ny2: %i, nz2: %i]", g3->n0.vx, g3->n0.vy, g3->n0.vz, g3->n1.vx, g3->n1.vy, g3->n1.vz, g3->n2.vx, g3->n2.vy, g3->n2.vz);
#endif // RES_VERBOSE_MODEL_LOADING_DATA
}

///////////////////////////////////////////////////
void PSM_ReportGT3(uint16 i_triIndex, uint8* i_vertexData)
{
#if RES_VERBOSE_MODEL_LOADING_DATA
	PRIM_GT3* gt3 = (PRIM_GT3*)i_vertexData;

	REPORT("[GT3 TRI %u]", i_triIndex);
	REPORT("[r0: %u, g0: %u, b0: %u],[r1: %u, g1: %u, b1: %u],[r2: %u, g2: %u, b2: %u]", gt3->c0.r, gt3->c0.g, gt3->c0.b, gt3->c1.r, gt3->c1.g, gt3->c1.b, gt3->c2.r, gt3->c2.g, gt3->c2.b);
	REPORT("[x0: %i, y0: %i, z0: %i],[x1: %i, y1: %i, z1: %i],[x2: %i, y2: %i, z2: %i]", gt3->v0.vx, gt3->v0.vy, gt3->v0.vz, gt3->v1.vx, gt3->v1.vy, gt3->v1.vz, gt3->v2.vx, gt3->v2.vy, gt3->v2.vz);
	REPORT("[u0: %i, v0: %i],[u1: %i, v1: %i],[u2: %i, v2: %i]", gt3->uv0.u, gt3->uv0.v, gt3->uv1.u, gt3->uv1.v, gt3->uv2.u, gt3->uv2.v);
	REPORT("[nx0: %i, ny0: %i, nz0: %i],[n1: %i, n1: %i, n1: %i],[n2: %i, n2: %i, n2: %i]", gt3->n0.vx, gt3->n0.vy, gt3->n0.vz, gt3->n1.vx, gt3->n1.vy, gt3->n1.vz, gt3->n2.vx, gt3->n2.vy, gt3->n2.vz);
#endif // RES_VERBOSE_MODEL_LOADING_DATA
}

///////////////////////////////////////////////////
void PSM_ReadF3(uint8* io_srcData, uint8* io_vertexData)
{
	// Declaration
	PSM_DECLARE(F3);

	// Copy position
	PSM_COPY_POS3;

	// Copy color from v0
	PSM_COPY_COL(0);

	// Copy normal from v0
	PSM_COPY_NORM(0);
}

///////////////////////////////////////////////////
void PSM_ReadFT3(uint8* io_srcData, uint8* io_vertexData)
{
	// Declaration
	PSM_DECLARE(FT3);

	// Copy position
	PSM_COPY_POS3;

	// Copy color from v0
	PSM_COPY_COL(0);

	// Copy normal from v0
	PSM_COPY_NORM(0);

	// Copy UV
	PSM_COPY_UV3;
}

///////////////////////////////////////////////////
void PSM_ReadG3(uint8* io_srcData, uint8* io_vertexData)
{
	// Declaration
	PSM_DECLARE(G3);

	// Copy position		
	PSM_COPY_POS3;

	// Copy color
	PSM_COPY_COL3;

	// Copy normal
	PSM_COPY_NORM3;
}

///////////////////////////////////////////////////
void PSM_ReadGT3(uint8* io_srcData, uint8* io_vertexData)
{
	// Declaration
	PSM_DECLARE(GT3);

	// Copy position
	PSM_COPY_POS3;

	// Copy color
	PSM_COPY_COL3;

	// Copy normal
	PSM_COPY_NORM3;

	// Copy UV
	PSM_COPY_UV3;
}

///////////////////////////////////////////////////
int16 Res_LoadPSM(void* i_srcAddress, StringId i_filename, ResModel2** o_model)
{
	uint32 submeshIndex = 0u;
	uint8* ptr = (uint8*)i_srcAddress;

	PSM_HEADER* header = (PSM_HEADER*)ptr; ptr += sizeof(PSM_HEADER);
	
	*o_model = Core_Malloc(sizeof(ResModel2));
	
#if RES_VERBOSE_MODEL_LOADING
	REPORT("SubMesh count: %u", header->submeshCount);
#endif // RES_VERBOSE_MODEL_LOADING
	VERIFY_ASSERT(header->magic == 0x1234beef, "Invalid PSM header!");

	(*o_model)->m_filename = i_filename;
	(*o_model)->m_submeshCount = header->submeshCount;
	(*o_model)->m_submeshes = Core_Malloc(sizeof(ResModelSubMesh) * (*o_model)->m_submeshCount);

	for (submeshIndex=0; submeshIndex < (*o_model)->m_submeshCount; ++submeshIndex)
	{
		uint32 triIndex = 0u;
		ResModelSubMesh* mdl = &(*o_model)->m_submeshes[submeshIndex];
		// See if there is a material defined for this model's submesh. If so, take the vertex format from it, otherwise default to F3.		
		ResMaterial* submeshMaterial = Res_GetMaterialLink(i_filename, submeshIndex);

		PSM_SUBMESH* submesh = (PSM_SUBMESH*)ptr; ptr += sizeof(PSM_SUBMESH);
		
		mdl->m_polyCount = submesh->triangleCount;
		mdl->m_primType = submeshMaterial ? submeshMaterial->m_type : PRIM_TYPE_POLY_F3;
		mdl->m_data = Core_Malloc(s_engineVertexSizes[mdl->m_primType] * mdl->m_polyCount);

#if RES_VERBOSE_MODEL_LOADING
		REPORT("[SUBMESH %u] poly count: %u, prim type: %u", submeshIndex, mdl->m_polyCount, mdl->m_primType);
#endif // RES_VERBOSE_MODEL_LOADING

		{
			uint8* dstData = (uint8*)mdl->m_data;
			uint8* srcData = ptr;

			for (triIndex = 0; triIndex < mdl->m_polyCount; ++triIndex)
			{
				switch (mdl->m_primType)
				{
				case PRIM_TYPE_POLY_F3:
					PSM_ReadF3(srcData, dstData);
					PSM_ReportF3(triIndex, dstData);
					break;

				case PRIM_TYPE_POLY_FT3:
					PSM_ReadFT3(srcData, dstData);
					PSM_ReportFT3(triIndex, dstData);
					break;

				case PRIM_TYPE_POLY_G3:
					PSM_ReadG3(srcData, dstData);
					PSM_ReportG3(triIndex, dstData);
					break;

				case PRIM_TYPE_POLY_GT3:
					PSM_ReadGT3(srcData, dstData);
					PSM_ReportGT3(triIndex, dstData);
					break;

				default:
					VERIFY_ASSERT(FALSE, "Invalid PRIM_TYPE");
					break;
				}

				srcData += sizeof(PSM_VERTEX);
				dstData += s_engineVertexSizes[mdl->m_primType];
			}

			ptr += mdl->m_polyCount * sizeof(PSM_VERTEX);
		}
	}
	return E_OK;
}

///////////////////////////////////////////////////
int16 Res_ReadLoadPSM(StringId i_filename, ResModel2** o_model)
{
	int16 res = E_FILE_IO;
	void* ptr = NULL;

#if RES_VERBOSE_MODEL_LOADING
	REPORT("Begin loading PSM %u", i_filename);
#endif // RES_VERBOSE_MODEL_LOADING

	// Read the file off disc
	Stream_BeginRead(i_filename, &ptr);
	if (ptr)
	{
		Stream_ReadFileBlocking();

		// Load the PSM
		res = Res_LoadPSM(ptr, i_filename, o_model);
	}
	Stream_EndRead();

#if RES_VERBOSE_MODEL_LOADING
	REPORT("End loading PSM %u (success: %s)", i_filename, (res == E_OK) ? "TRUE" : "FALSE");
#endif // RES_VERBOSE_MODEL_LOADING
	return res;
}

///////////////////////////////////////////////////
int16 Res_FreePSM(ResModel2** io_model)
{
	VERIFY_ASSERT(io_model && *io_model, "Input model is null");

#if RES_VERBOSE_MODEL_LOADING
	REPORT("Freeing PSM %u", (*io_model)->m_filename);
#endif // RES_VERBOSE_MODEL_LOADING
	{
		uint32 submeshIndex = 0;
		for (submeshIndex = 0; submeshIndex < (*io_model)->m_submeshCount; ++submeshIndex)
		{
#if RES_VERBOSE_MODEL_LOADING
			REPORT("Freeing submesh %u", submeshIndex);
#endif // RES_VERBOSE_MODEL_LOADING
			Core_Free((*io_model)->m_submeshes[submeshIndex].m_data);
			Core_Free((*io_model)->m_submeshes);
		}

		Core_Free(*io_model);
	}	
#if RES_VERBOSE_MODEL_LOADING
	REPORT("Freed PSM %u", (*io_model)->m_filename);
#endif // RES_VERBOSE_MODEL_LOADING
}
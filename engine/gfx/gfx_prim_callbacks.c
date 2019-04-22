#include "gfx.h"

#if !CONFIG_FINAL
GfxPrimCounts g_primCounts;
///////////////////////////////////////////////////
void Gfx_Debug_GetPrimCounts(GfxPrimCounts* o_counts)
{
	memcpy(o_counts, &g_primCounts, sizeof(g_primCounts));
}

// Writes count data to the GfxPrimCounts structure
#define REGISTER_PRIM(type) \
	{ \
		uint32 offset = OFFSET_OF(GfxPrimCounts, m_prim## type)/sizeof(uint16); \
		uint16* counts = (uint16*)&g_primCounts; \
		*(counts + offset++) += primCount; \
		*(counts + offset++) += primDivCount-1; \
		*(counts + offset++) += litBit ? primCount : 0; \
		*(counts + offset++) += fogBit ? primCount : 0; \
	}

#else
#define REGISTER_PRIM(type)
#endif // !CONFIG_FINAL

///////////////////////////////////////////////////
// BASIC PRIMITIVE TYPES
///////////////////////////////////////////////////

// Callbacks for primitive submission, one per type
void* (*fncAddPrim[PRIM_TYPE_MAX])(void*, int32*);
void InitAddPrimCallbacks();

void* (*fncAddPointSpr[PRIM_TYPE_MAX])(POINT_SPRITE* const, int32*);
void InitAddPointSprCallbacks();

DIVPOLYGON3 dd;

/*
This is a heavily macro'ed implementation for pushing primitives to the OT. Supports:
 - Transformation, backface culling & clipping
 - Lighting
 - Depth cueing
 - Polygon division

 Primitives are allocated off the Gfx scratch allocator (exception is the transform & backface cull step, this can early out and uses a POLY_XX member on the stack).
*/

#define PRIMVALID(otz, v) ( (otz) > 0 && (otz) < OT_ENTRIES) && ((v) > 0)

#define DECL_PRIM_AND_TRANSFORM(type) \
	int32	p, flg, otz, valid=1, hasprim=FALSE, submit=FALSE, nclip; \
	uint32 state = Gfx_GetRenderState(); \
	bool transformBit = (state & RS_PERSP) != 0; \
	bool litBit = (state & RS_LIGHTING) != 0; \
	bool fogBit = (state & RS_FOG) != 0; \
	bool backfaceCullBit = (state & RS_BACKFACE_CULL) != 0; \
	DivisionParams* divparams; \
	uint16 primCount = 1; \
	uint16 primDivCount = 0; \
	PRIM_## type* prim = (PRIM_## type*)i_prim; \
	DIVPOLYGON3* divp = &dd; \
	POLY_## type* poly; \
	POLY_## type temp; \
	Gfx_GetDivisionParams(&divparams); \
	if (transformBit) \
	{ \
		/* Do a pre-transformation to do backface culling and clipped triangle checking */ \
		/* This is important to save allocations and further processing */ \
		uint16 displayWidth = Gfx_GetDisplayWidth(); \
		uint16 displayHeight = Gfx_GetDisplayHeight(); \
		SetPoly## type(&temp); \
		/* Load the vertex into GTE registers and do a perspective transformation */ \
		gte_ldv3(&prim->v0, &prim->v1, &prim->v2); \
		gte_rtpt(); \
		/* Calculate the normal clip factor, used for backface culling */ \
		gte_nclip(); \
		/*
		// Clipping will not work with subdivided primitives; what to do?		
		gte_stflg(&flg); \
		if (flg < 0) return NULL; \
		*/ \
		/* Store the screenspace coordinates into the POLY structure */ \
		gte_stsxy3((int32*)&temp.x0, (int32*)&temp.x1, (int32*)&temp.x2); \
		/* Clip to viewing area. There has to be a way to do this in a better way... */ \
		if ( (temp.x0 < 0) && (temp.x1 < 0) && (temp.x2 < 0) ) return NULL; \
		if ( (temp.x0 > displayWidth) && (temp.x1 > displayWidth) && (temp.x2 > displayWidth) ) return NULL; \
		if ( (temp.y0 < 0) && (temp.y1 < 0) && (temp.y2 < 0) ) return NULL; \
		if ( (temp.y0 > displayHeight) && (temp.y1 > displayHeight) && (temp.y2 > displayHeight) ) return NULL; \
		/* Retrieve the normal clip factor and test for backfaces */ \
		gte_stopz(&nclip); \
		if ((nclip <= 0) && backfaceCullBit) return NULL; \
		/* Average the z values and retrieve the otz */ \
		gte_avsz3(); \
		gte_stotz(&otz); \
		/* At this stage, if the otz is within OT bounds and the primitive is valid, carry on. */ \
		if ( !PRIMVALID(otz, valid)) return NULL; \
		/* Configure the primitive division */ \
		if ((state & RS_DIVISION) && divparams) \
		{ \
			int8 idx; \
			int32 otc = otz >> (14 - OT_LENGTH); \
			int32 otcTest = otc; \
			divp->ndiv = 0; \
			/* Find where in the distance array the otc value is. The index is our division count. */ \
			for (idx = 0; idx < DIVMODE_COUNT-1; ++idx) \
			{ \
				uint8 next = divparams->m_distances[idx + 1]; \
				uint8 curr = divparams->m_distances[idx]; \
				if (otcTest < curr) \
				{ \
					otcTest += (next - curr); \
					continue; \
				} \
 				if ( ((otc < next) && (otc >= curr))) \
				{ \
					divp->ndiv = DIVMODE_COUNT-idx; \
					break; \
				} \
			} \
			divp->pih = displayWidth; \
			divp->piv = displayHeight; \
		} \
		else \
		{ \
			divp->ndiv = 0; \
		} \
		primCount = ((1 << divp->ndiv) << divp->ndiv); \
		primDivCount = primCount; \
		/* We can allocate memory for enough POLY_XX structures now. */ \
		poly = (POLY_## type*)Gfx_Alloc(sizeof(POLY_## type) * MAX(primDivCount, primCount), 4); \
		memcpy(poly, &temp, sizeof(temp)); \
	} \
	else \
	{ \
		/* it's a screenspace primitive, we do not support division for them. Allocate one POLY_XX and fill it. */ \
		poly = (POLY_## type*)Gfx_Alloc(sizeof(POLY_## type), 4); \
		SetPoly## type(poly); \
		setXY3(poly, prim->v0.vx, prim->v0.vy, \
			prim->v1.vx, prim->v1.vy, \
			prim->v2.vx, prim->v2.vy \
		); \
		divp->ndiv = 0; \
	}

#define DO_FINAL_COLOR_IMPL(index) \
	gte_ldrgb(&poly->r## index); \
	if (litBit & transformBit) { \
		gte_ldv0(&(prim->n## index).vx); gte_nccs(); gte_strgb(&poly->r## index); \
	} \
	if (fogBit & transformBit) { \
		gte_stdp(&p); gte_dpcs(); gte_strgb(&poly->r## index); \
	}

#define DO_FINAL_COLOR(index) \
	(prim->c## index).cd = poly->code; \
	poly->r## index = (prim->c## index).r; \
	poly->g## index = (prim->c## index).g; \
	poly->b## index = (prim->c## index).b; \
	DO_FINAL_COLOR_IMPL(index)

#define DO_FINAL_COLOR_TEX(index) \
	(prim->c## index).cd = poly->code; \
	poly->r## index = ((prim->c## index).r >> 1); \
	poly->g## index = ((prim->c## index).g >> 1); \
	poly->b## index = ((prim->c## index).b >> 1); \
	DO_FINAL_COLOR_IMPL(index)

#define UPDATE_TEXTURE \
	{ \
		/* Set the uv on the poly and update its texture page and clut */ \
		uint8 tscaleU, tscaleV, toffsetU, toffsetV; \
		Gfx_GetTextureScaleOffset(&tscaleU, &tscaleV, &toffsetU, &toffsetV); \
		setUV3 \
		( \
			poly, \
			prim->uv0.u * tscaleU + toffsetU, prim->uv0.v * tscaleV + toffsetV, \
			prim->uv1.u * tscaleU + toffsetU, prim->uv1.v * tscaleV + toffsetV, \
			prim->uv2.u * tscaleU + toffsetU, prim->uv2.v * tscaleV + toffsetV \
		); \
		VERIFY_ASSERT( (prim->uv0.u < 256) && (prim->uv0.v < 256), "UV0 greater than 255! Passed in: %u, %u", prim->uv0.u, prim->uv0.v); \
		VERIFY_ASSERT((prim->uv1.u < 256) && (prim->uv1.v < 256), "UV1 greater than 255! Passed in: %u, %u", prim->uv1.u, prim->uv1.v); \
		VERIFY_ASSERT((prim->uv2.u < 256) && (prim->uv2.v < 256), "UV2 greater than 255! Passed in: %u, %u", prim->uv2.u, prim->uv2.v); \
		Gfx_GetTexture(&poly->tpage, &poly->clut); \
	}

/*
Begin division:
- If it's needed, passthrough the primitive. 
- Otherwise, submission will be required to the OT. By this point, the primitive's otz will be valid (has been checked during transformation)
*/
#define BEGIN_PRIM_PREP \
	{ \
		if (divp->ndiv == 0) \
		{ \
			*o_otz = otz; \
			hasprim = TRUE; \
			submit = TRUE; \
		} \
		else \
		{ \
			hasprim = TRUE; \
		} \
	}

/*
Division macros: Assuming poly is pointing to a memory location with a large enough allocation for the divison (check above), do the division on the GTE.
This operation submits the primitive to the OT.
*/
#define PRIMDIV_G3 \
	if (divp->ndiv != 0) \
	{ \
		poly = (POLY_G3*)DivideG3(&prim->v0, &prim->v1, &prim->v2, &poly->r0, &poly->r1, &poly->r2, poly, Gfx_GetCurrentOT() + otz, divp); \
	}

#define PRIMDIV_GT3 \
	if (divp->ndiv != 0) \
	{ \
		poly = (POLY_GT3*)DivideGT3(&prim->v0, &prim->v1, &prim->v2, &poly->u0, &poly->u1, &poly->u2, &poly->r0, &poly->r1, &poly->r2, poly, Gfx_GetCurrentOT() + otz, divp); \
	}

#define PRIMDIV_F3 \
	if (divp->ndiv != 0) \
	{ \
		poly = (POLY_F3*)DivideF3(&prim->v0, &prim->v1, &prim->v2, &poly->r0, poly, Gfx_GetCurrentOT() + otz, divp); \
	}

#define PRIMDIV_FT3 \
	if (divp->ndiv != 0) \
	{ \
		poly = (POLY_FT3*)DivideFT3(&prim->v0, &prim->v1, &prim->v2, &poly->u0, &poly->u1, &poly->u2, &poly->r0, poly, Gfx_GetCurrentOT() + otz, divp); \
	}

/* Primitive submission is finished by returning either the poly address or NULL (if the primitive didn't pass various checks or it was divided). */
#define END_PRIM_PREP \
	return submit ? poly : NULL;

#define BEGIN_PRIM_WORK \
	if (hasprim) \
	{

#define END_PRIM_WORK \
	}

///////////////////////////////////////////////////
void* AddPrim_POLY_F3(void* i_prim, int32* o_otz)
{
	DECL_PRIM_AND_TRANSFORM(F3);
	
	BEGIN_PRIM_PREP;
		BEGIN_PRIM_WORK;
			DO_FINAL_COLOR(0);
		END_PRIM_WORK;
	PRIMDIV_F3;
	REGISTER_PRIM(F3);
	END_PRIM_PREP;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_FT3(void* i_prim, int32* o_otz)
{
	DECL_PRIM_AND_TRANSFORM(FT3);
	
	BEGIN_PRIM_PREP;
		BEGIN_PRIM_WORK;
			UPDATE_TEXTURE;
			DO_FINAL_COLOR_TEX(0);
		END_PRIM_WORK;
	PRIMDIV_FT3;
	REGISTER_PRIM(FT3);
	END_PRIM_PREP;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_G3(void* i_prim, int32* o_otz)
{
	DECL_PRIM_AND_TRANSFORM(G3);
	
	BEGIN_PRIM_PREP;
		BEGIN_PRIM_WORK;
			DO_FINAL_COLOR(0);
			DO_FINAL_COLOR(1);
			DO_FINAL_COLOR(2);
		END_PRIM_WORK;
	PRIMDIV_G3;
	REGISTER_PRIM(G3);
	END_PRIM_PREP;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_GT3(void* i_prim, int32* o_otz)
{
	DECL_PRIM_AND_TRANSFORM(GT3);

	BEGIN_PRIM_PREP;
		BEGIN_PRIM_WORK;
			UPDATE_TEXTURE;
			DO_FINAL_COLOR_TEX(0);
			DO_FINAL_COLOR_TEX(1);
			DO_FINAL_COLOR_TEX(2);
		END_PRIM_WORK;
	PRIMDIV_GT3;
	REGISTER_PRIM(GT3);
	END_PRIM_PREP;
}

///////////////////////////////////////////////////
void* AddPointSpr_POLY_F(POINT_SPRITE* const i_prim, int32* o_otz)
{
	int32	p, flg, otz, valid;
	
	// Expand the point sprite from the centre outwards across width and height
	int16 const halfWidth = i_prim->width >> 1;
	int16 const halfHeight = i_prim->height >> 1;

	SVECTOR v0 = { i_prim->p.vx - halfWidth, i_prim->p.vy - halfHeight, i_prim->p.vz };
	SVECTOR v1 = { i_prim->p.vx + halfWidth, i_prim->p.vy - halfHeight, i_prim->p.vz };
	SVECTOR v2 = { i_prim->p.vx - halfWidth, i_prim->p.vy + halfHeight, i_prim->p.vz };
	SVECTOR v3 = { i_prim->p.vx + halfWidth, i_prim->p.vy + halfHeight, i_prim->p.vz };

	POLY_F4* poly = (POLY_F4*)Gfx_Alloc(sizeof(POLY_F4), 4);
	SetPolyF4(poly);

	valid = RotAverageNclip4
	(
		&v0, &v1, &v2, &v3,
		(int32*)&poly->x0, (int32*)&poly->x1, (int32*)&poly->x2, (int32*)&poly->x3,
		&p,
		&otz,
		&flg
	);

	if (PRIMVALID(otz, valid))
	{
		CVECTOR* c = &i_prim->c;
		if (Gfx_GetRenderState() & RS_FOG)
		{
			DpqColor(c, p, c);
		}

		setRGB0(poly, i_prim->c.r, i_prim->c.g, i_prim->c.b);

		*o_otz = otz;
		return poly;		
	}
}

///////////////////////////////////////////////////
void* AddPointSpr_POLY_FT(POINT_SPRITE* const i_prim, int32* o_otz)
{
	
	return NULL;
}

///////////////////////////////////////////////////
void InitAddPointSprCallbacks()
{
	// POLY
	fncAddPointSpr[PRIM_TYPE_POLY_F3] = &AddPointSpr_POLY_F;
	fncAddPointSpr[PRIM_TYPE_POLY_FT3] = &AddPointSpr_POLY_FT;
}

///////////////////////////////////////////////////
void InitAddPrimCallbacks()
{
	// POLY
	fncAddPrim[PRIM_TYPE_POLY_F3] = &AddPrim_POLY_F3;
	fncAddPrim[PRIM_TYPE_POLY_FT3] = &AddPrim_POLY_FT3;
	fncAddPrim[PRIM_TYPE_POLY_G3] = &AddPrim_POLY_G3;
	fncAddPrim[PRIM_TYPE_POLY_GT3] = &AddPrim_POLY_GT3;
}

///////////////////////////////////////////////////
// HIGHER LEVEL PRIMITIVE TYPES
///////////////////////////////////////////////////

// Callbacks for higher level primitive submission, one per type
void (*fncAddCube[PRIM_TYPE_MAX])(void*, uint32, uint8);
void InitAddCubeCallbacks();

void (*fncAddPlane[PRIM_TYPE_MAX])(void*, uint32, uint32, uint8);
void InitAddPlaneCallbacks();

///////////////////////////////////////////////////
void AddCube_POLY_F3(void* i_data, uint32 i_size, uint8 i_uvSize)
{
	const CVECTOR *i_color = (CVECTOR*)i_data;
	const PRIM_F3 primitives[12] = 
	{
		// Front
		{ {-i_size, -i_size, -i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},	{0,0,-ONE},	i_color[0] },
		{ {i_size, i_size, -i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},	{0,0,-ONE},	i_color[0] },
		// Right
		{ {i_size, -i_size, -i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},		{ONE,0,0},	i_color[1] },
		{ {i_size, i_size, i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},		{ONE,0,0},	i_color[1] },
		// Back
		{ {i_size, -i_size, i_size}, {-i_size, i_size, i_size},	{-i_size, -i_size, i_size},		{0,0,ONE},	i_color[2] },
		{ {-i_size, i_size, i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},		{0,0,ONE},	i_color[2] },
		// Left
		{ {-i_size, -i_size, i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},	{-ONE,0,0},	i_color[3] },
		{ {-i_size, i_size, -i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},	{-ONE,0,0},	i_color[3] },
		// Top
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {i_size, i_size, i_size},		{0,ONE,0},	i_color[4] },
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {-i_size, i_size, -i_size},		{0,ONE,0},	i_color[4] },
		// Bottom
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, -i_size, i_size},	{0,-ONE,0},	i_color[5] },
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size},	{0,-ONE,0},	i_color[5] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_F3, (void* const)primitives, ARRAY_SIZE(primitives));
}

///////////////////////////////////////////////////
void AddCube_POLY_FT3(void* i_data, uint32 i_size, uint8 i_uvSize)
{
	const CVECTOR *i_color = (CVECTOR*)i_data;
	const PRIM_FT3 primitives[12] =
	{
		// Front
		{ {-i_size, -i_size, -i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},	{0,0},{i_uvSize,0},{i_uvSize,i_uvSize},  {0,0,-ONE},i_color[0] },
		{ {i_size, i_size, -i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},	{i_uvSize,i_uvSize},{0,i_uvSize},{0,0},  {0,0,-ONE},i_color[0] },
		// Right	  
		{ {i_size, -i_size, -i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},		{0,0},{i_uvSize,0},{i_uvSize,i_uvSize},  {ONE,0,0},i_color[1] },
		{ {i_size, i_size, i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},		{i_uvSize,i_uvSize},{0,i_uvSize},{0,0},  {ONE,0,0},i_color[1] },
		// Back	  
		{ {i_size, -i_size, i_size}, {-i_size, i_size, i_size},	{-i_size, -i_size, i_size},		{0,0},{i_uvSize,0},{i_uvSize,i_uvSize},  {0,0,ONE},i_color[2] },
		{ {-i_size, i_size, i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},		{i_uvSize,i_uvSize},{0,i_uvSize},{0,0},  {0,0,ONE},i_color[2] },
		// Left	  
		{ {-i_size, -i_size, i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},	{0,0},{i_uvSize,0},{i_uvSize,i_uvSize},  {-ONE,0,0},i_color[3] },
		{ {-i_size, i_size, -i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},	{i_uvSize,i_uvSize},{0,i_uvSize},{0,0},  {-ONE,0,0},i_color[3] },
		// Top	  
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {i_size, i_size, i_size},		{0,0},{i_uvSize,0},{i_uvSize,i_uvSize},  {0,ONE,0},	i_color[4] },
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {-i_size, i_size, -i_size},		{i_uvSize,i_uvSize},{0,i_uvSize},{0,0},  {0,ONE,0},	i_color[4] },
		// Bottom	  
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, -i_size, i_size},	{0,0},{i_uvSize,0},{i_uvSize,i_uvSize},  {0,-ONE,0},i_color[5] },
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size},	{i_uvSize,i_uvSize},{0,i_uvSize},{0,0},  {0,-ONE,0},i_color[5] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_FT3, (void* const)primitives, ARRAY_SIZE(primitives));
}

///////////////////////////////////////////////////
void AddCube_POLY_G3(void* i_data, uint32 i_size, uint8 i_uvSize)
{
	const CVECTOR *i_color = (CVECTOR*)i_data;
	const PRIM_G3 primitives[12] = 
	{
		// Front
		{ {-i_size, -i_size, -i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},	{0,0,-ONE},	{0,0,-ONE}, {0,0,-ONE}, i_color[0], i_color[2], i_color[1] },
		{ {i_size, i_size, -i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},	{0,0,-ONE},	{0,0,-ONE}, {0,0,-ONE}, i_color[2], i_color[0], i_color[3] },
		// Right					    
		{ {i_size, -i_size, -i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},		{ONE,0,0},	{ONE,0,0},	{ONE,0,0},	i_color[1], i_color[6], i_color[5] },
		{ {i_size, i_size, i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},		{ONE,0,0},	{ONE,0,0},	{ONE,0,0},	i_color[6], i_color[1], i_color[2] },
		// Back					    
		{ {i_size, -i_size, i_size}, {-i_size, i_size, i_size},	{-i_size, -i_size, i_size},		{0,0,ONE},	{0,0,ONE},	{0,0,ONE},	i_color[5], i_color[7], i_color[4] },
		{ {-i_size, i_size, i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},		{0,0,ONE},	{0,0,ONE},	{0,0,ONE},	i_color[7], i_color[5], i_color[6] },
		// Left					    
		{ {-i_size, -i_size, i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},	{-ONE,0,0},	{-ONE,0,0}, {-ONE,0,0}, i_color[4], i_color[3], i_color[0] },
		{ {-i_size, i_size, -i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},	{-ONE,0,0},	{-ONE,0,0}, {-ONE,0,0}, i_color[3], i_color[4], i_color[7] },
		// Top					    
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {i_size, i_size, i_size},		{0,ONE,0},	{0,ONE,0},	{0,ONE,0},	i_color[0], i_color[4], i_color[5] },
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {-i_size, i_size, -i_size},		{0,ONE,0},	{0,ONE,0},	{0,ONE,0},	i_color[5], i_color[1], i_color[0] },
		// Bottom					    
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, -i_size, i_size},	{0,-ONE,0},	{0,-ONE,0}, {0,-ONE,0}, i_color[3], i_color[2], i_color[6] },
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size},	{0,-ONE,0},	{0,-ONE,0}, {0,-ONE,0}, i_color[6], i_color[7], i_color[3] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_G3, (void* const)primitives, ARRAY_SIZE(primitives));
}

///////////////////////////////////////////////////
void AddCube_POLY_GT3(void* i_data, uint32 i_size, uint8 i_uvSize)
{
	const CVECTOR *i_color = (CVECTOR*)i_data;
	const PRIM_GT3 primitives[12] =
	{
		// Front
		{ {-i_size, -i_size, -i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},	{0,0,-ONE},	{0,0,-ONE}, {0,0,-ONE}, i_color[0], i_color[2], i_color[1]	,{0,0},{i_uvSize,0},{i_uvSize,i_uvSize}},
		{ {i_size, i_size, -i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},	{0,0,-ONE},	{0,0,-ONE}, {0,0,-ONE}, i_color[2], i_color[0], i_color[3]	,{i_uvSize,i_uvSize},{0,i_uvSize},{0,0}},
		// Right																			  					    		  
		{ {i_size, -i_size, -i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},		{ONE,0,0},	{ONE,0,0},	{ONE,0,0},	i_color[1], i_color[6], i_color[5]	,{0,0},{i_uvSize,0},{i_uvSize,i_uvSize}},
		{ {i_size, i_size, i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},		{ONE,0,0},	{ONE,0,0},	{ONE,0,0},	i_color[6], i_color[1], i_color[2]	,{i_uvSize,i_uvSize},{0,i_uvSize},{0,0}},
		// Back																			  					    		  
		{ {i_size, -i_size, i_size}, {-i_size, i_size, i_size},	{-i_size, -i_size, i_size},		{0,0,ONE},	{0,0,ONE},	{0,0,ONE},	i_color[5], i_color[7], i_color[4]	,{0,0},{i_uvSize,0},{i_uvSize,i_uvSize}},
		{ {-i_size, i_size, i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},		{0,0,ONE},	{0,0,ONE},	{0,0,ONE},	i_color[7], i_color[5], i_color[6]	,{i_uvSize,i_uvSize},{0,i_uvSize},{0,0}},
		// Left																			  					    		  
		{ {-i_size, -i_size, i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},	{-ONE,0,0},	{-ONE,0,0}, {-ONE,0,0}, i_color[4], i_color[3], i_color[0]	,{0,0},{i_uvSize,0},{i_uvSize,i_uvSize}},
		{ {-i_size, i_size, -i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},	{-ONE,0,0},	{-ONE,0,0}, {-ONE,0,0}, i_color[3], i_color[4], i_color[7]	,{i_uvSize,i_uvSize},{0,i_uvSize},{0,0}},
		// Top																			  					    		  
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {i_size, i_size, i_size},		{0,ONE,0},	{0,ONE,0},	{0,ONE,0},	i_color[0], i_color[4], i_color[5]	,{0,0},{i_uvSize,0},{i_uvSize,i_uvSize}},
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {-i_size, i_size, -i_size},		{0,ONE,0},	{0,ONE,0},	{0,ONE,0},	i_color[5], i_color[1], i_color[0]	,{i_uvSize,i_uvSize},{0,i_uvSize},{0,0}},
		// Bottom																			  					    		  
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, -i_size, i_size},	{0,-ONE,0},	{0,-ONE,0}, {0,-ONE,0}, i_color[3], i_color[2], i_color[6]	,{0,0},{i_uvSize,0},{i_uvSize,i_uvSize}},
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size},	{0,-ONE,0},	{0,-ONE,0}, {0,-ONE,0}, i_color[6], i_color[7], i_color[3]	,{i_uvSize,i_uvSize},{0,i_uvSize},{0,0}}
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_GT3, (void* const)primitives, ARRAY_SIZE(primitives));
}

///////////////////////////////////////////////////
void InitAddCubeCallbacks()
{
	fncAddCube[PRIM_TYPE_POLY_F3] = &AddCube_POLY_F3;
	fncAddCube[PRIM_TYPE_POLY_FT3] = &AddCube_POLY_FT3;
	fncAddCube[PRIM_TYPE_POLY_G3] = &AddCube_POLY_G3;
	fncAddCube[PRIM_TYPE_POLY_GT3] = &AddCube_POLY_GT3;
}

#define DECL_PLANE_DATA \
	const CVECTOR *i_color = (CVECTOR*)i_data; \
	const uint32 halfWidth = i_width >> 1; \
	const uint32 halfHeight = i_height >> 1

///////////////////////////////////////////////////
void AddPlane_POLY_F3(void* i_data, uint32 i_width, uint32 i_height, uint8 i_uvSize)
{
	DECL_PLANE_DATA;

	const PRIM_F3 primitives[2] = 
	{
		// Front
		{ {-halfWidth, 0, -halfHeight}, {-halfWidth, 0, halfHeight}, {halfWidth, 0, halfHeight},	{0,ONE,0},	i_color[0] },
		{ {halfWidth, 0, halfHeight}, {halfWidth, 0, -halfHeight}, {-halfWidth, 0, -halfHeight},	{0,ONE,0},	i_color[0] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_F3, (void* const)primitives, ARRAY_SIZE(primitives));
}

///////////////////////////////////////////////////
void AddPlane_POLY_FT3(void* i_data, uint32 i_width, uint32 i_height, uint8 i_uvSize)
{
	DECL_PLANE_DATA;

	const PRIM_FT3 primitives[2] =
	{
		// Front
		{ {-halfWidth, 0, -halfHeight}, {-halfWidth, 0, halfHeight}, {halfWidth, 0, halfHeight},	{0,0},{0,32},{32,32}, {0,ONE,0},	i_color[0] },
		{ {-halfWidth, 0, -halfHeight}, {halfWidth, 0, halfHeight}, {halfWidth, 0, -halfHeight},	{0,0},{32,32},{32,0}, {0,ONE,0},i_color[0] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_FT3, (void* const)primitives, ARRAY_SIZE(primitives));
}

///////////////////////////////////////////////////
void AddPlane_POLY_G3(void* i_data, uint32 i_width, uint32 i_height, uint8 i_uvSize)
{
	DECL_PLANE_DATA;

	const PRIM_G3 primitives[2] = 
	{
		// Front
		{ {-halfWidth, 0, -halfHeight}, {-halfWidth, 0, halfHeight}, {halfWidth, 0, halfHeight}, { 0,ONE,0 },{ 0,ONE,0 },{ 0,ONE,0 }, i_color[0], i_color[1], i_color[2] },
		{ {halfWidth, 0, halfHeight}, {halfWidth, 0, -halfHeight}, {-halfWidth, 0, -halfHeight}, { 0,ONE,0 },{ 0,ONE,0 },{ 0,ONE,0 }, i_color[2], i_color[3], i_color[0] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_G3, (void* const)primitives, ARRAY_SIZE(primitives));
}

///////////////////////////////////////////////////
void AddPlane_POLY_GT3(void* i_data, uint32 i_width, uint32 i_height, uint8 i_uvSize)
{

}

///////////////////////////////////////////////////
void InitAddPlaneCallbacks()
{
	fncAddPlane[PRIM_TYPE_POLY_F3] = &AddPlane_POLY_F3;
	fncAddPlane[PRIM_TYPE_POLY_FT3] = &AddPlane_POLY_FT3;
	fncAddPlane[PRIM_TYPE_POLY_G3] = &AddPlane_POLY_G3;
	fncAddPlane[PRIM_TYPE_POLY_GT3] = &AddPlane_POLY_GT3;
}

///////////////////////////////////////////////////
void InitPrimCallbacks()
{
	InitAddPrimCallbacks();
	InitAddPointSprCallbacks();
	InitAddCubeCallbacks();
	InitAddPlaneCallbacks();
}

///////////////////////////////////////////////////
void BeginPrimSubmission()
{
#if !CONFIG_FINAL
	Util_MemZero(&g_primCounts, sizeof(g_primCounts));
#endif // !CONFIG_FINAL
}
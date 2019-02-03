///////////////////////////////////////////////////
// BASIC PRIMITIVE TYPES
///////////////////////////////////////////////////

DIVPOLYGON3 divp;

// Callbacks for primitive submission, one per type
void* (*fncAddPrim[PRIM_TYPE_MAX])(void*, int32*);
void InitAddPrimCallbacks();

void* (*fncAddPointSpr[PRIM_TYPE_MAX])(POINT_SPRITE* const, int32*);
void InitAddPointSprCallbacks();

#define PRIMVALID(otz, v) ( (otz) > 0 && (otz) < MAX_OT_LENGTH) && ((v) > 0)

#define DECL_PRIM_DATA(type) \
	int32	p, flg, otz, valid, hasprim=FALSE, submit=FALSE; \
	PRIM_## type* prim = (PRIM_## type*)i_prim; \
	POLY_## type* poly = (POLY_## type*)Gfx_Alloc(sizeof(POLY_## type) * ((1 << divp.ndiv) << divp.ndiv), 4); \
	SetPoly## type(poly);

#define TRANSFORM_PRIM \
	if (Gfx_GetRenderState() & RS_PERSP) \
	{ \
		valid = RotAverageNclip3(&prim->v0, &prim->v1, &prim->v2, \
			(int32*)&poly->x0, (int32*)&poly->x1, (int32*)&poly->x2, \
			&p, \
			&otz, \
			&flg); \
	} \
	else \
	{ \
		setXY3(poly, prim->v0.vx, prim->v0.vy, \
			prim->v1.vx, prim->v1.vy, \
			prim->v2.vx, prim->v2.vy \
		); \
	}

#define DECL_PRIM_COL(index) CVECTOR* c## index = &prim->c## index

#define INIT_POLY_COL(index) \
	(c## index)->cd = poly->code; \
	poly->r## index = (c## index)->r; \
	poly->g## index = (c## index)->g; \
	poly->b## index = (c## index)->b;

#define BEGIN_LIGHTING if (Gfx_GetRenderState() & RS_LIGHTING) {
#define DO_LIGHTING(index) NormalColorCol(&prim->n## index, c## index, (CVECTOR*)&poly->r## index)
#define END_LIGHTING }

#define BEGIN_DQ if (Gfx_GetRenderState() & RS_FOG) {
#define DO_DQ(index) DpqColor((CVECTOR*)&poly->r## index, p, (CVECTOR*)&poly->r## index)
#define END_DQ }

#define BEGIN_PRIM_PREP \
	if (divp.ndiv == 0) \
	{ \
		if (PRIMVALID(otz, valid)) \
		{ \
			*o_otz = otz; \
			hasprim = TRUE; \
			submit = TRUE; \
		} \
	} \
	else \
	{ \
		hasprim = TRUE; \
	}

#define PRIMDIV_G3 \
	if (divp.ndiv != 0) \
	{ \
		poly = (POLY_G3*)DivideG3(&prim->v0, &prim->v1, &prim->v2, &poly->r0, &poly->r1, &poly->r2, poly, Gfx_GetCurrentOT() + otz, &divp); \
	}

#define PRIMDIV_F3 \
	if (divp.ndiv != 0) \
	{ \
		poly = (POLY_F3*)DivideF3(&prim->v0, &prim->v1, &prim->v2, &poly->r0, poly, Gfx_GetCurrentOT() + otz, &divp); \
	}

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
	DECL_PRIM_DATA(F3);
	
	TRANSFORM_PRIM;
	
	BEGIN_PRIM_PREP	
		BEGIN_PRIM_WORK
			DECL_PRIM_COL(0);
			INIT_POLY_COL(0);
		
			BEGIN_LIGHTING;
				DO_LIGHTING(0);
			END_LIGHTING;
	
			BEGIN_DQ;
				DO_DQ(0);
			END_DQ;
		END_PRIM_WORK
	PRIMDIV_F3
	END_PRIM_PREP
}

///////////////////////////////////////////////////
void* AddPrim_POLY_FT3(void* i_prim, int32* o_otz)
{
	DECL_PRIM_DATA(FT3);

	*o_otz = 0;
	return poly;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_G3(void* i_prim, int32* o_otz)
{
	DECL_PRIM_DATA(G3);
	
	TRANSFORM_PRIM;

	BEGIN_PRIM_PREP	
		BEGIN_PRIM_WORK
			DECL_PRIM_COL(0);
			DECL_PRIM_COL(1);
			DECL_PRIM_COL(2);

			INIT_POLY_COL(0);
			INIT_POLY_COL(1);
			INIT_POLY_COL(2);

			// Lighting first
			BEGIN_LIGHTING;
				DO_LIGHTING(0);
				DO_LIGHTING(1);
				DO_LIGHTING(2);
			END_LIGHTING;

			// Then depth queue
			BEGIN_DQ;
				DO_DQ(0);
				DO_DQ(1);
				DO_DQ(2);
			END_DQ;
		END_PRIM_WORK
	PRIMDIV_G3
	END_PRIM_PREP
}

///////////////////////////////////////////////////
void* AddPrim_POLY_GT3(void* i_prim, int32* o_otz)
{
	//DECL_PRIM_DATA(GT3);

	return NULL;
}

///////////////////////////////////////////////////
void* AddPointSpr_POLY_F(POINT_SPRITE* const i_prim, int32* o_otz)
{
	int32	p, flg, otz, valid;
	
	// Expand the point sprite from the centre outwards across width and height
	int16 const halfWidth = i_prim->width / 2;
	int16 const halfHeight = i_prim->height / 2;

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
void (*fncAddCube[PRIM_TYPE_MAX])(void*, uint32);
void InitAddCubeCallbacks();

void (*fncAddPlane[PRIM_TYPE_MAX])(void*, uint32, uint32);
void InitAddPlaneCallbacks();

///////////////////////////////////////////////////
void AddCube_POLY_F3(void* i_data, uint32 i_size)
{
	const CVECTOR *i_color = (CVECTOR*)i_data;
	const PRIM_F3 primitives[12] = 
	{
		// Front
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},	{0,0,ONE},	i_color[0] },	
		{ {i_size, i_size, -i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},	{0,0,ONE},	i_color[0] },
		// Right
		{ {i_size, -i_size, -i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},		{-ONE,0,0},	i_color[1] },
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},		{-ONE,0,0},	i_color[1] },
		// Back
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},		{0,0,-ONE},	i_color[2] },
		{ {-i_size, i_size, i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},		{0,0,-ONE},	i_color[2] },
		// Left
		{ {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},	{ONE,0,0},	i_color[3] },
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {-i_size, -i_size, i_size},	{ONE,0,0},	i_color[3] },
		// Top
		{ {-i_size, -i_size, -i_size}, {-i_size, -i_size, i_size}, {i_size, -i_size, i_size},	{0,ONE,0},	i_color[4] },
		{ {i_size, -i_size, i_size}, {i_size, -i_size, -i_size}, {-i_size, -i_size, -i_size},	{0,ONE,0},	i_color[4] },
		// Bottom
		{ {-i_size, i_size, -i_size}, {i_size, i_size, -i_size}, {i_size, i_size, i_size},		{0,-ONE,0},	i_color[5] },
		{ {i_size, i_size, i_size}, {-i_size, i_size, i_size}, {-i_size, i_size, -i_size},		{0,-ONE,0},	i_color[5] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_F3, (void* const)primitives, ARRAY_SIZE(primitives));
}

///////////////////////////////////////////////////
void AddCube_POLY_FT3(void* i_data, uint32 i_size)
{

}

///////////////////////////////////////////////////
void AddCube_POLY_G3(void* i_data, uint32 i_size)
{
	const CVECTOR *i_color = (CVECTOR*)i_data;
	const PRIM_G3 primitives[12] = 
	{
		// Front
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},	{0,0,ONE},	{0,0,ONE}, {0,0,ONE}, i_color[0], i_color[1], i_color[2] },
		{ {i_size, i_size, -i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},	{0,0,ONE},	{0,0,ONE}, {0,0,ONE}, i_color[2], i_color[3], i_color[0] },
		// Right																									    
		{ {i_size, -i_size, -i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},		{-ONE,0,0},	{-ONE,0,0},	{-ONE,0,0},	i_color[1], i_color[5], i_color[6] },
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},		{-ONE,0,0},	{-ONE,0,0},	{-ONE,0,0},	i_color[6], i_color[2], i_color[1] },
		// Back																										    
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},		{0,0,-ONE},	{0,0,-ONE},	{0,0,-ONE},	i_color[5], i_color[4], i_color[7] },
		{ {-i_size, i_size, i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},		{0,0,-ONE},	{0,0,-ONE},	{0,0,-ONE},	i_color[7], i_color[6], i_color[5] },
		// Left																										    
		{ {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},	{ONE,0,0},	{ONE,0,0}, {ONE,0,0}, i_color[4], i_color[0], i_color[3] },
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {-i_size, -i_size, i_size},	{ONE,0,0},	{ONE,0,0}, {ONE,0,0}, i_color[3], i_color[7], i_color[4] },
		// Top																										    
		{ {-i_size, -i_size, -i_size}, {-i_size, -i_size, i_size}, {i_size, -i_size, i_size},	{0,ONE,0},	{0,ONE,0},	{0,ONE,0},	i_color[0], i_color[4], i_color[5] },
		{ {i_size, -i_size, i_size}, {i_size, -i_size, -i_size}, {-i_size, -i_size, -i_size},	{0,ONE,0},	{0,ONE,0},	{0,ONE,0},	i_color[5], i_color[1], i_color[0] },
		// Bottom																									    
		{ {-i_size, i_size, -i_size}, {i_size, i_size, -i_size}, {i_size, i_size, i_size},		{0,-ONE,0},	{0,-ONE,0}, {0,-ONE,0}, i_color[3], i_color[2], i_color[6] },
		{ {i_size, i_size, i_size}, {-i_size, i_size, i_size}, {-i_size, i_size, -i_size},		{0,-ONE,0},	{0,-ONE,0}, {0,-ONE,0}, i_color[6], i_color[7], i_color[3] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_G3, (void* const)primitives, ARRAY_SIZE(primitives));
}

///////////////////////////////////////////////////
void AddCube_POLY_GT3(void* i_data, uint32 i_size)
{

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
void AddPlane_POLY_F3(void* i_data, uint32 i_width, uint32 i_height)
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
void AddPlane_POLY_FT3(void* i_data, uint32 i_width, uint32 i_height)
{

}

///////////////////////////////////////////////////
void AddPlane_POLY_G3(void* i_data, uint32 i_width, uint32 i_height)
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
void AddPlane_POLY_GT3(void* i_data, uint32 i_width, uint32 i_height)
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

	divp.ndiv = 2;			/* divide depth */
	divp.pih = Gfx_GetDisplayWidth();			/* horizontal resolution */
	divp.piv = Gfx_GetDisplayHeight();			/* vertical resolution */
}
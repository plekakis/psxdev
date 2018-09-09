///////////////////////////////////////////////////
// BASIC PRIMITIVE TYPES
///////////////////////////////////////////////////

// Callbacks for primitive submission, one per type
void* (*fncAddPrim[PRIM_TYPE_MAX])(void*, int32*);
void InitAddPrimCallbacks();

void* (*fncAddPointSpr[PRIM_TYPE_MAX])(POINT_SPRITE* const, int32*);
void InitAddPointSprCallbacks();

#define PRIMVALID(otz, v) ( (otz) > 0 && (otz) < MAX_OT_LENGTH) && ((v) > 0)

///////////////////////////////////////////////////
void* AddPrim_POLY_F3(void* i_prim, int32* o_otz)
{
	int32	p, flg, otz, valid=0;
	PRIM_F3* prim = (PRIM_F3*)i_prim;
	POLY_F3* poly = (POLY_F3*)Gfx_Alloc(sizeof(POLY_F3), 4);
	
	SetPolyF3(poly);

	if (Gfx_GetRenderState() & RS_PERSP)
	{
		valid = RotAverageNclip3(&prim->v0, &prim->v1, &prim->v2,
				(int32*)&poly->x0, (int32*)&poly->x1, (int32*)&poly->x2,
				&p, 
				&otz,
				&flg);		
	}
	else
	{
		setXY3(poly, prim->v0.vx, prim->v0.vy,
				prim->v1.vx, prim->v1.vy,
				prim->v2.vx, prim->v2.vy
				);
	}

	if (PRIMVALID(otz, valid))
	{	
		CVECTOR* c = &prim->c;
		c->cd = poly->code;

		poly->r0 = c->r;
		poly->g0 = c->g;
		poly->b0 = c->b;

		// Lighting first
		if (Gfx_GetRenderState() & RS_LIGHTING)
		{			
			NormalColorCol(&prim->n0, c, (CVECTOR*)&poly->r0);
		}

		// Then depth queue
		p = (Gfx_GetRenderState() & RS_FOG) ? p : 0;
		DpqColor((CVECTOR*)&poly->r0, p, (CVECTOR*)&poly->r0);

		*o_otz = otz;
		return poly;
	}
    return NULL;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_FT3(void* i_prim, int32* o_otz)
{
	POLY_FT3* poly = (POLY_FT3*)i_prim;
	SetPolyFT3(poly);

	*o_otz = 0;
	return poly;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_G3(void* i_prim, int32* o_otz)
{
	int32	p, flg, otz, valid;
	PRIM_G3* prim = (PRIM_G3*)i_prim;
	POLY_G3* poly = (POLY_G3*)Gfx_Alloc(sizeof(POLY_G3), 4);
			
	SetPolyG3(poly);
	
	if (Gfx_GetRenderState() & RS_PERSP)
	{
		valid = RotAverageNclip3(&prim->v0, &prim->v1, &prim->v2,
				(int32*)&poly->x0, (int32*)&poly->x1, (int32*)&poly->x2,
				&p,
				&otz,
				&flg);
	}
	else
	{
		setXY3(poly, prim->v0.vx, prim->v0.vy,
				prim->v1.vx, prim->v1.vy,
				prim->v2.vx, prim->v2.vy
				);
	}

	if (PRIMVALID(otz, valid))
	{
		CVECTOR* c0 = &prim->c0;
		CVECTOR* c1 = &prim->c1;
		CVECTOR* c2 = &prim->c2;

		c0->cd = poly->code;
		c1->cd = poly->code;
		c2->cd = poly->code;

		poly->r0 = c0->r;
		poly->g0 = c0->g;
		poly->b0 = c0->b;
		
		poly->r1 = c1->r;
		poly->g1 = c1->g;
		poly->b1 = c1->b;
		
		poly->r2 = c2->r;
		poly->g2 = c2->g;
		poly->b2 = c2->b;

		// Lighting first
		// There is something wrong with this...
		// Currently havingt he depth queue override the lighting results. As a result, gouraud triangles won't get lit :(
		if (Gfx_GetRenderState() & RS_LIGHTING)
		{
			NormalColorCol3(&prim->n0, &prim->n1, &prim->n2, c0, (CVECTOR*)&poly->r0, (CVECTOR*)&poly->r1, (CVECTOR*)&poly->r2);
		}

		// Then depth queue
		p = (Gfx_GetRenderState() & RS_FOG) ? p : 0;
		//DpqColor3((CVECTOR*)&poly->r0, (CVECTOR*)&poly->r1, (CVECTOR*)&poly->r2,
		DpqColor3(c0, c1, c2,
				p,
				 (CVECTOR*)&poly->r0, (CVECTOR*)&poly->r1, (CVECTOR*)&poly->r2);

		*o_otz = otz;
		return poly;
	}
    return NULL;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_GT3(void* i_prim, int32* o_otz)
{
	
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
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},	{0,0,-ONE},	i_color[0] },	
		{ {i_size, i_size, -i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},	{0,0,-ONE},	i_color[0] },
		// Right
		{ {i_size, -i_size, -i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},		{ONE,0,0},	i_color[1] },
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},		{ONE,0,0},	i_color[1] },
		// Back
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},		{0,0,ONE},	i_color[2] },
		{ {-i_size, i_size, i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},		{0,0,ONE},	i_color[2] },
		// Left
		{ {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},	{-ONE,0,0},	i_color[3] },
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {-i_size, -i_size, i_size},	{-ONE,0,0},	i_color[3] },
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
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},	{0,0,-ONE},	{0,0,-ONE}, {0,0,-ONE}, i_color[0], i_color[1], i_color[2] },
		{ {i_size, i_size, -i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},	{0,0,-ONE},	{0,0,-ONE}, {0,0,-ONE}, i_color[2], i_color[3], i_color[0] },
		// Right																									    
		{ {i_size, -i_size, -i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},		{ONE,0,0},	{ONE,0,0},	{ONE,0,0},	i_color[1], i_color[5], i_color[6] },
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},		{ONE,0,0},	{ONE,0,0},	{ONE,0,0},	i_color[6], i_color[2], i_color[1] },
		// Back																										    
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},		{0,0,ONE},	{0,0,ONE},	{0,0,ONE},	i_color[5], i_color[4], i_color[7] },
		{ {-i_size, i_size, i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},		{0,0,ONE},	{0,0,ONE},	{0,0,ONE},	i_color[7], i_color[6], i_color[5] },
		// Left																										    
		{ {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},	{-ONE,0,0},	{-ONE,0,0}, {-ONE,0,0}, i_color[4], i_color[0], i_color[3] },
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {-i_size, -i_size, i_size},	{-ONE,0,0},	{-ONE,0,0}, {-ONE,0,0}, i_color[3], i_color[7], i_color[4] },
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

///////////////////////////////////////////////////
void AddPlane_POLY_F3(void* i_data, uint32 i_width, uint32 i_height)
{
	const CVECTOR *i_color = (CVECTOR*)i_data;
	const uint32 halfWidth = i_width >> 1;
	const uint32 halfHeight = i_height >> 1;

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
	const CVECTOR *i_color = (CVECTOR*)i_data;
	const uint32 halfWidth = i_width >> 1;
	const uint32 halfHeight = i_height >> 1;

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
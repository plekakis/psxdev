///////////////////////////////////////////////////
// BASIC PRIMITIVE TYPES
///////////////////////////////////////////////////

// Callbacks for primitive submission, one per type
void* (*fncAddPrim[PRIM_TYPE_MAX])(void*, int32*, uint8);
void InitAddPrimCallbacks();

///////////////////////////////////////////////////
void* AddPrim_POLY_F3(void* i_prim, int32* o_otz, uint8 i_flags)
{
	int32	p, flg, otz;
	int32	isomote = INT_MAX;
	uint8   isPerspective = i_flags & PRIM_FLAG_PERSP;
	uint8	isFogged = i_flags & PRIM_FLAG_FOG;
	PRIM_F3* prim = (PRIM_F3*)i_prim;
	POLY_F3* poly = (POLY_F3*)Gfx_Alloc(sizeof(POLY_F3), 4);
	
	SetPolyF3(poly);

	if (isPerspective)
	{
		isomote = RotAverageNclip3(&prim->v0, &prim->v1, &prim->v2,
				(int32*)&poly->x0, (int32*)&poly->x1, (int32*)&poly->x2,
				(int32*)&p, (int32*)&otz, (int32*)&flg);
	}
	else
	{
		setXY3(poly, prim->v0.vx, prim->v0.vy,
				prim->v1.vx, prim->v1.vy,
				prim->v2.vx, prim->v2.vy
				);
	}

	

	if (isomote > 0)
	{
		
		//CVECTOR c;
		//NormalColorDpq(&prim->n0, &prim->c, p, &poly->r0);

		setRGB0(poly, prim->c.r, prim->c.g, prim->c.b);

		*o_otz = otz;
		return poly;
	}
    return NULL;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_FT3(void* i_prim, int32* o_otz, uint8 i_flags)
{
	POLY_FT3* poly = (POLY_FT3*)i_prim;
	SetPolyFT3(poly);

	*o_otz = 0;
	return poly;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_G3(void* i_prim, int32* o_otz, uint8 i_flags)
{
	int32	p, flg, otz;
	int32	isomote = INT_MAX;
	uint8   isPerspective = i_flags & PRIM_FLAG_PERSP;
	PRIM_G3* prim = (PRIM_G3*)i_prim;	
	POLY_G3* poly = (POLY_G3*)Gfx_Alloc(sizeof(POLY_G3), 4);
			
	SetPolyG3(poly);
	
	if (isPerspective)
	{
		isomote = RotAverageNclip3(&prim->v0, &prim->v1, &prim->v2,
				(int32*)&poly->x0, (int32*)&poly->x1, (int32*)&poly->x2,
				&p, &otz, &flg);
	}
	else
	{
		setXY3(poly, prim->v0.vx, prim->v0.vy,
				prim->v1.vx, prim->v1.vy,
				prim->v2.vx, prim->v2.vy
				);
	}

	if (isomote > 0)
	{
		setRGB0(poly, prim->c0.r, prim->c0.g, prim->c0.b);
		setRGB1(poly, prim->c1.r, prim->c1.g, prim->c1.b);
		setRGB2(poly, prim->c2.r, prim->c2.g, prim->c2.b);

		*o_otz = otz;
		return poly;
	}
    return NULL;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_GT3(void* i_prim, int32* o_otz, uint8 i_flags)
{
	POLY_GT3* poly = (POLY_GT3*)i_prim;
	SetPolyGT3(poly);

	*o_otz = 0;
	return poly;
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

// Callbacks for pre-made orbject submission, one per type
void (*fncAddCube[PRIM_TYPE_MAX])(void*, uint32);
void InitAddCubeCallbacks();

///////////////////////////////////////////////////
void AddCube_POLY_F3(void* i_data, uint32 i_size)
{
	CVECTOR *i_color = (CVECTOR*)i_data;
	PRIM_F3 primitives[12] = 
	{
		// Front
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},	{0,0,-1},	i_color[0] },	
		{ {i_size, i_size, -i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},	{0,0,-1},	i_color[0] },
		// Right
		{ {i_size, -i_size, -i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},		{1,0,0},	i_color[1] },
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},		{1,0,0},	i_color[1] },
		// Back
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},		{0,0,1},	i_color[2] },
		{ {-i_size, i_size, i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},		{0,0,1},	i_color[2] },
		// Left
		{ {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},	{-1,0,0},	i_color[3] },
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {-i_size, -i_size, i_size},	{-1,0,0},	i_color[3] },
		// Top
		{ {-i_size, -i_size, -i_size}, {-i_size, -i_size, i_size}, {i_size, -i_size, i_size},	{0,1,0},	i_color[4] },
		{ {i_size, -i_size, i_size}, {i_size, -i_size, -i_size}, {-i_size, -i_size, -i_size},	{0,1,0},	i_color[4] },
		// Bottom
		{ {-i_size, i_size, -i_size}, {i_size, i_size, -i_size}, {i_size, i_size, i_size},		{0,-1,0},	i_color[5] },
		{ {i_size, i_size, i_size}, {-i_size, i_size, i_size}, {-i_size, i_size, -i_size},		{0,-1,0},	i_color[5] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_F3, primitives, ARRAY_SIZE(primitives), PRIM_FLAG_PERSP | PRIM_FLAG_FOG);
}

///////////////////////////////////////////////////
void AddCube_POLY_FT3(void* i_data)
{

}

///////////////////////////////////////////////////
void AddCube_POLY_G3(void* i_data, uint32 i_size)
{
	CVECTOR *i_color = (CVECTOR*)i_data;
	PRIM_G3 primitives[12] = 
	{
		// Front
		{ {-i_size, -i_size, -i_size}, {i_size, -i_size, -i_size}, {i_size, i_size, -i_size},		i_color[0], i_color[1], i_color[2] },
		{ {i_size, i_size, -i_size}, {-i_size, i_size, -i_size}, {-i_size, -i_size, -i_size},		i_color[2], i_color[3], i_color[0] },
		// Right
		{ {i_size, -i_size, -i_size}, {i_size, -i_size, i_size}, {i_size, i_size, i_size},			i_color[1], i_color[5], i_color[6] },
		{ {i_size, i_size, i_size}, {i_size, i_size, -i_size}, {i_size, -i_size, -i_size},			i_color[6], i_color[2], i_color[1] },
		// Back
		{ {i_size, -i_size, i_size}, {-i_size, -i_size, i_size}, {-i_size, i_size, i_size},			i_color[5], i_color[4], i_color[7] },
		{ {-i_size, i_size, i_size}, {i_size, i_size, i_size}, {i_size, -i_size, i_size},			i_color[7], i_color[6], i_color[5] },
		// Left
		{ {-i_size, -i_size, i_size}, {-i_size, -i_size, -i_size}, {-i_size, i_size, -i_size},		i_color[4], i_color[0], i_color[3] },
		{ {-i_size, i_size, -i_size}, {-i_size, i_size, i_size}, {-i_size, -i_size, i_size},		i_color[3], i_color[7], i_color[4] },
		// Top
		{ {-i_size, -i_size, -i_size}, {-i_size, -i_size, i_size}, {i_size, -i_size, i_size},		i_color[0], i_color[4], i_color[5] },
		{ {i_size, -i_size, i_size}, {i_size, -i_size, -i_size}, {-i_size, -i_size, -i_size},		i_color[5], i_color[1], i_color[0] },
		// Bottom
		{ {-i_size, i_size, -i_size}, {i_size, i_size, -i_size}, {i_size, i_size, i_size},			i_color[3], i_color[2], i_color[6] },
		{ {i_size, i_size, i_size}, {-i_size, i_size, i_size}, {-i_size, i_size, -i_size},			i_color[6], i_color[7], i_color[3] }
	};

	Gfx_AddPrims(PRIM_TYPE_POLY_G3, primitives, ARRAY_SIZE(primitives), PRIM_FLAG_PERSP);
}

///////////////////////////////////////////////////
void AddCube_POLY_GT3(void* i_data)
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
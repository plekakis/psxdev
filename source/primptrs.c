enum PRIMIDX
{
    PRIMIDX_G3,
    PRIMIDX_F3,
    PRIMIDX_GT3,
    PRIMIDX_FT3,
    PRIMIDX_MAX = PRIMIDX_FT3
};
// sizes and function pointers for primitive permutations
// Initialisation
void (*fncInitPrimitive[PRIMIDX_MAX])(void* inMem, void** outX);

void init_prim_g3(void* inMem, void** outX)
{
    POLY_G3* asG3 = (POLY_G3*)inMem;
    outX[0] = &asG3->x0;
    outX[1] = &asG3->x1;
    outX[2] = &asG3->x2;

    SetPolyG3(asG3);
}

void init_prim_f3(void* inMem, void** outX)
{
    POLY_F3* asF3 = (POLY_F3*)inMem;
    outX[0] = &asF3->x0;
    outX[1] = &asF3->x1;
    outX[2] = &asF3->x2;

    SetPolyF3(asF3);
}

void init_prim_gt3(void* inMem, void** outX)
{
    POLY_GT3* asGT3 = (POLY_GT3*)inMem;
    outX[0] = &asGT3->x0;
    outX[1] = &asGT3->x1;
    outX[2] = &asGT3->x2;

    SetPolyGT3(asGT3);
}

void init_prim_ft3(void* inMem, void** outX)
{
    POLY_FT3* asFT3 = (POLY_FT3*)inMem;
    outX[0] = &asFT3->x0;
    outX[1] = &asFT3->x1;
    outX[2] = &asFT3->x2;

    SetPolyFT3(asFT3);
}

// SetRGB
void (*fncPrimSetColors[PRIMIDX_MAX])(void* inMem, CVECTOR** inColors);

void prim_set_colors_g3(void* inMem, CVECTOR** inColors)
{
    POLY_G3* asG3 = (POLY_G3*)inMem;
    setRGB0(asG3, inColors[0]->r, inColors[0]->g, inColors[0]->b);
    setRGB1(asG3, inColors[1]->r, inColors[1]->g, inColors[1]->b);
    setRGB2(asG3, inColors[2]->r, inColors[2]->g, inColors[2]->b);
}

void prim_set_colors_f3(void* inMem, CVECTOR** inColors)
{
    POLY_F3* asF3 = (POLY_F3*)inMem;
    setRGB0(asF3, inColors[0]->r, inColors[0]->g, inColors[0]->b);
}

void prim_set_colors_gt3(void* inMem, CVECTOR** inColors)
{
    POLY_GT3* asGT3 = (POLY_GT3*)inMem;
    setRGB0(asGT3, inColors[0]->r, inColors[0]->g, inColors[0]->b);
    setRGB1(asGT3, inColors[1]->r, inColors[1]->g, inColors[1]->b);
    setRGB2(asGT3, inColors[2]->r, inColors[2]->g, inColors[2]->b);
}

void prim_set_colors_ft3(void* inMem, CVECTOR** inColors)
{
    POLY_FT3* asFT3 = (POLY_FT3*)inMem;
    setRGB0(asFT3, inColors[0]->r, inColors[0]->g, inColors[0]->b);
}

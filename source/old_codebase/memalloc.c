SCRATCH scratch_mem;
DB_SCRATCH db_scratch_mem;

u_long mem_allocated = 0;

u_long align_size(u_long inSize, u_short inAlignment)
{
    return (inSize + (inAlignment - 1)) & ~(inAlignment - 1);
}

void init_scratch(u_long inSize, SCRATCH* outScratchMem)
{
    outScratchMem->start = (u_char*)realloc3(outScratchMem->start, inSize);
	outScratchMem->end = ((u_char*)outScratchMem->start) + inSize;
	outScratchMem->next = outScratchMem->start;
}

u_char* alloc_scratch(SCRATCH* inScratchMem, u_long inSize, u_short inAlignment)
{
    u_char *m = 0;
    inSize = align_size(inSize, inAlignment);

    m = inScratchMem->next + inSize;

    if (m > inScratchMem->end) return 0;

    inScratchMem->next = m;
    return m;
}

void shutdown_scratch(SCRATCH* inScratchMem)
{
    free3(inScratchMem->start);

    inScratchMem->start = inScratchMem->end = inScratchMem->next = 0;
}

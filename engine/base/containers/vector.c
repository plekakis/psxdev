#include "vector.h"

///////////////////////////////////////////////////
vector* Vector_Init(uint32 i_elementSize, uint32 i_reservationCount, uint32 i_flags)
{
    VERIFY_ASSERT(i_elementSize < UINT16_MAX, "Vector_Init: Element size of %u bytes is too large (UINT16_MAX is the largest supported", i_elementSize);

    {
        vector* v = (vector*)Core_Malloc(sizeof(vector));

        v->m_usedElementCount = 0u;
        v->m_flags = i_flags;
        v->m_elementSize = i_elementSize;
        v->m_sizeInBytes = i_elementSize * i_reservationCount;
        v->m_data = Core_Malloc( v->m_sizeInBytes);

#if VECTOR_VERBOSE
        REPORT("Vector_Init: Created vector %x with %u reserved slots of %u bytes each", v, i_reservationCount, i_elementSize);   
#endif // VECTOR_VERBOSE

        return v;
    }
}

///////////////////////////////////////////////////
void Vector_Free(vector* io_vector)
{
    VERIFY_ASSERT(io_vector, "Vector_Free: NULL vector specified!");

    // First, free data.
    Core_Free(io_vector->m_data);
    io_vector->m_data = NULL;

    // Finally, free vector itself.
    Core_Free(io_vector);

#if VECTOR_VERBOSE
    REPORT("Vector_Free: Freed vector %x and its contents", io_vector);
#endif // VECTOR_VERBOSE
}

///////////////////////////////////////////////////
void Vector_Clear(vector* io_vector)
{
    VERIFY_ASSERT(io_vector, "Vector_Clear: NULL vector specified!");

    // Optionally free memory
    if ( (io_vector->m_flags & CF_NOSHRINK) == 0)
    {
        Core_Free(io_vector->m_data);
        io_vector->m_data = NULL;

        io_vector->m_sizeInBytes = 0u;
    }

    io_vector->m_usedElementCount = 0u;

#if VECTOR_VERBOSE
    REPORT("Vector_Clear: Cleared vector %x", io_vector);
#endif // VECTOR_VERBOSE
}

///////////////////////////////////////////////////
uint32 Vector_GetCount(vector* i_vector)
{
    VERIFY_ASSERT(i_vector, "Vector_GetCount: NULL vector specified!");
    return i_vector->m_usedElementCount;
}

///////////////////////////////////////////////////
uint32 Vector_GetSize(vector* i_vector)
{
    VERIFY_ASSERT(i_vector, "Vector_GetSize: NULL vector specified!");
    return i_vector->m_sizeInBytes / i_vector->m_elementSize;
}

///////////////////////////////////////////////////
void Vector_Resize(vector* io_vector, uint32 i_reservationCount)
{
    VERIFY_ASSERT(io_vector, "Vector_Resize: NULL vector specified!");

    {
        const uint32 newSize = i_reservationCount * io_vector->m_elementSize;
        const uint32 oldSize = io_vector->m_sizeInBytes;
        const bool mustRealloc = (io_vector->m_flags & CF_NOSHRINK) ? (newSize > oldSize) : (newSize != oldSize);

        void* newElements = mustRealloc ? Core_Realloc(io_vector->m_data, newSize) : io_vector->m_data;
        VERIFY_ASSERT(newElements, "Vector_Resize: Failed to allocate memory for vector resize.");
        if (newElements)
        {
            uint32 prevUsedElementCount = io_vector->m_usedElementCount;
            
            io_vector->m_sizeInBytes = mustRealloc ? newSize : MAX(newSize, oldSize);
            io_vector->m_usedElementCount = MIN(prevUsedElementCount, newSize / io_vector->m_elementSize);
            io_vector->m_data = newElements;

            #if VECTOR_VERBOSE
            if (newSize != oldSize)
            {
                REPORT("Vector_Resize: Resized vector %x. New used element count: %u, previous used element count: %u", io_vector, io_vector->m_usedElementCount, prevUsedElementCount);
            }
            #endif // VECTOR_VERBOSE
        }
    } 
}
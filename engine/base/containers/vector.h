#ifndef VECTOR_H_INC
#define VECTOR_H_INC

typedef struct
{
    uint32 m_elementSize;
    uint32 m_usedElementCount;
    uint32 m_sizeInBytes;
    uint32 m_flags;
    void*  m_data;
}vector;

void Vector_Init(vector** io_vector, uint32 i_elementSize, uint32 i_reservationCount, uint32 i_flags);
void Vector_Free(vector* io_vector);
void Vector_Resize(vector* io_vector, uint32 i_reservationCount);
uint32 Vector_GetCount(vector* i_vector);
uint32 Vector_GetSize(vector* i_vector);
void Vector_Clear(vector* io_vector);

#define Vector_PushBack(v, t, e) \
    { \
        VERIFY_ASSERT((v), "Vector_PushBack: NULL vector specified!"); \
        { \
            uint32 size = Vector_GetSize((v)); \
            uint32 count = Vector_GetCount((v)); \
            if (count == size) \
            { \
                REPORT("Vector_PushBack: Resizing from %u to %u elements of size %u", size, size+1, (v)->m_elementSize); \
                Vector_Resize((v), size + 1); \
            } \
            { \
                t* values = (t*)(v)->m_data; \
                values[count] = e; \
                (v)->m_usedElementCount++; \
            } \
        } \
    }

#define Vector_Get(v, t, i, o) \
    { \
        VERIFY_ASSERT( (v), "Vector_Get: NULL vector specified!"); \
        VERIFY_ASSERT( (o), "Vector_Get: NULL output specified!"); \
        VERIFY_ASSERT( (i) < Vector_GetCount((v)), "Vector_Get: Out of bounds (index: %u, count: %u)", (i), Vector_GetCount((v))); \
        { \
            t* values = (t*)(v)->m_data; \
            *o = values[(i)]; \
        } \
    }

#endif // VECTOR_H_INC
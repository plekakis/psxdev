#include <base/containers/containers.h>

typedef struct
{
    uint32 index;
    uint32 value;
}Entity;

Entity g_entity = {5, 100};

///////////////////////////////////////////////////
void start()
{
    {
        vector* v = Vector_Init(sizeof(Entity), 5, CF_NOSHRINK);

        uint32 i=0;
        Entity a2 = {1, 10001};
        for (i=0; i<10; ++i)
        {
            Entity a = {5 * i, 10 * (i+1)};
            Vector_PushBack(v, Entity, a);
        }
                
        //Vector_Insert(v, Entity, a2, 10);
        Vector_Remove(v, Entity, 5);

        for (i=0; i<Vector_GetCount(v); ++i)
        {
            Entity c;
            Vector_Get(v, Entity, i, &c);
            REPORT("Value at %i: %i, %i", i, c.index, c.value);
        }

        Vector_Free(v);
    }

    // Pointer test.
    {
        Entity* g_test;
        vector* v = Vector_Init(sizeof(Entity*), 0, CF_NOSHRINK); // no reservation, PushBack will allocate.
        
        Vector_PushBack(v, Entity*, &g_entity);

        Vector_Get(v, Entity*, 0, &g_test);
        REPORT("%u, %u", g_test->index, g_test->value);

        Vector_Free(v);
    }
}

///////////////////////////////////////////////////
void update()
{

}

///////////////////////////////////////////////////
void render()
{

}
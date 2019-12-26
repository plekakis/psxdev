#include <base/containers/containers.h>

typedef struct
{
    uint32 t;
    uint32 g;
}test;
vector* v;

///////////////////////////////////////////////////
void start()
{
	Vector_Init(&v, sizeof(test), 5, CF_NOSHRINK);

    {
        uint32 i=0;
        
        for (i=0; i<6; ++i)
        {
            test a = {5 * i, 10 * (i+1)};
            Vector_PushBack(v, test, a);
        }
        
        for (i=0; i<Vector_GetCount(v); ++i)
        {
            test c;
            Vector_Get(v, test, i, &c);
            REPORT("Value at %i: %i, %i", i, c.t, c.g);
        }
    }

    Vector_Free(v);
}

///////////////////////////////////////////////////
void update()
{

}

///////////////////////////////////////////////////
void render()
{

}
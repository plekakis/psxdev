#ifndef CONTAINERS_H_INC
#define CONTAINERS_H_INC

#define VECTOR_VERBOSE (1u)

typedef enum
{
    CF_DEFAULT = 0,
    CF_NOSHRINK = 1 << 0
}ContainerFlags;

#include <base/core/core.h>
#include "vector.h"

#endif // CONTAINERS_H_INC
#include "Face.h"
#include <algorithm>

uint32_t packFace(uint32_t direction, uint32_t texture)
{
    return (direction & 7u)
         | ((texture & 2047u) << 3);
}


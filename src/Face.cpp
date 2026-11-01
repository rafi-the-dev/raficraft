#include "Face.h"
#include <algorithm>

uint32_t packFace(uint32_t direction, uint32_t texture)
{
    return (direction & 7u)
         | ((texture & 2047u) << 3);
}

uint32_t packTorchFace(uint32_t direction) {
    return packFace(direction, 4) | FACE_TORCH_FLAG;
}

#include "Face.h"
#include <algorithm>

uint32_t packFace(uint32_t direction, uint32_t texture) {
    return (direction & 7u) | ((texture & 2047u) << 3u);
}

void packLight(uint32_t& face, uint32_t blockLight, uint32_t skyLight) {
    face |= (blockLight & 15u) << 14u;
    face |= (skyLight & 15u) << 18u;
}

uint32_t packTorchFace(uint32_t direction) {
    return packFace(direction, 4) | FACE_TORCH_FLAG;
}

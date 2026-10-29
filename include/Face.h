#pragma once
#include <cstdint>

struct Face {
    int x, y, z;
    uint32_t data;
};

enum FaceDirection {
    FRONT  = 0,
    BACK   = 1,
    LEFT   = 2,
    RIGHT  = 3,
    TOP    = 4,
    BOTTOM = 5
};

uint32_t packFace(uint32_t direction, uint32_t texture)
{
    return (direction & 7u)
         | ((texture & 2047u) << 3);
}

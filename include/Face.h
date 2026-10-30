#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>

struct Face {
    int x = 0;
    int y = 0;
    int z = 0;
    uint32_t data = 0;
};

enum FaceDirection {
    FRONT  = 0,
    BACK   = 1,
    LEFT   = 2,
    RIGHT  = 3,
    TOP    = 4,
    BOTTOM = 5
};

uint32_t packFace(uint32_t direction, uint32_t texture);

struct FaceBatch
{
    uint32_t textureIndex = 0;
    std::size_t firstFace = 0;
    std::size_t faceCount = 0;
};

std::vector<FaceBatch> buildFaceBatches(std::vector<Face>& faces);

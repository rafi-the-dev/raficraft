#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

struct Face {
    int x = 0;
    int y = 0;
    int z = 0;
    uint32_t data = 0;
};

enum FaceDirection { FRONT = 0, BACK = 1, LEFT = 2, RIGHT = 3, TOP = 4, BOTTOM = 5 };

uint32_t packFace(uint32_t direction, uint32_t texture);

void packLight(uint32_t& face, uint32_t blockLight, uint32_t skyLight);

constexpr uint32_t FACE_TORCH_FLAG = 1u << 31;

uint32_t packTorchFace(uint32_t direction);



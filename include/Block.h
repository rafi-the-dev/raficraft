#pragma once
#include <cstdint>

struct Block {
    enum {
        air = 0,
        dirt,
        stone,
        grass,
        torch,
        blockCount,
    };
    int type = 0;
};

uint32_t getBlockTexture(Block block, int faceDirection);

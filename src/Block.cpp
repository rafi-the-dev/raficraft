#include "Block.h"
#include <cstdint>

uint32_t getBlockTexture(Block block, int faceDirection) {
    if(block.type == Block::grass) {
        if(faceDirection == 4) {
            return 2; // Grass top
        }

        if(faceDirection == 5) {
            return 0; // Dirt bottom
        }

        return 3; // Grass sides
    }

    if(block.type == Block::dirt) {
        return 0;
    }

    if(block.type == Block::stone) {
        return 1;
    }

    return 0;
}

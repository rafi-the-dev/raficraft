#include "World.h"

Block world::getBlock(int x, int y, int z) {
    if(y < 0 || y >= CHUNK_SIZE_Y) {
        return Block{Block::air};
    }

    int chunkX = x / CHUNK_SIZE_X;
    int chunkZ = z / CHUNK_SIZE_Z;

    int localX = x % CHUNK_SIZE_X;
    int localZ = z % CHUNK_SIZE_Z;

    // Handle negative coordinates
    if(localX < 0) {
        localX += CHUNK_SIZE_X;
        chunkX--;
    }

    if(localZ < 0) {
        localZ += CHUNK_SIZE_Z;
        chunkZ--;
    }

    auto it = chunks.find(glm::ivec2(chunkX, chunkZ));

    if(it == chunks.end()) {
        return Block{Block::air};
    }

    return it->second.blocks[localX][y][localZ];
}

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

void world::markChunkDirty(int x, int z) {
    auto it = chunks.find(glm::ivec2(x, z));

    if(it != chunks.end()) {
        it->second.dirty = true;
    }
}

void world::setBlock(int x, int y, int z, Block type) {
    if(y < 0 || y >= CHUNK_SIZE_Y) {
        return;
    }

    int chunkX = static_cast<int>(std::floor(static_cast<float>(x) / CHUNK_SIZE_X));
    int chunkZ = static_cast<int>(std::floor(static_cast<float>(z) / CHUNK_SIZE_Z));

    int localX = x - chunkX * CHUNK_SIZE_X;
    int localZ = z - chunkZ * CHUNK_SIZE_Z;

    auto it = chunks.find(glm::ivec2(chunkX, chunkZ));

    if(it == chunks.end()) {
        return;
    }

    it->second.dirty = true;

    it->second.blocks[localX][y][localZ].type = type.type;

    markChunkDirty(chunkX, chunkZ);

    if(localX == 0) {
        markChunkDirty(chunkX - 1, chunkZ);
    }

    if(localX == CHUNK_SIZE_X - 1) {
        markChunkDirty(chunkX + 1, chunkZ);
    }

    if(localZ == 0) {
        markChunkDirty(chunkX, chunkZ - 1);
    }

    if(localZ == CHUNK_SIZE_Z - 1) {
        markChunkDirty(chunkX, chunkZ + 1);
    }
}

void world::markNeighborsDirty(int x, int z) {
    markChunkDirty(x - 1, z);
    markChunkDirty(x + 1, z);
    markChunkDirty(x, z - 1);
    markChunkDirty(x, z + 1);
}

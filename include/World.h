#pragma once
#include <algorithm>
#include "Player.h"
#include "glm/glm.hpp"

struct Chunk;
struct ChunkEntry;

struct world {
    std::vector<ChunkEntry> chunks;
    Block getBlock(int x, int y, int z);
    void setBlock(int x, int y, int z, Block type);
    void markChunkDirty(int x, int z);
    void markNeighborsDirty(int x, int z);
    ChunkEntry newChunk(int x, int z);
    void sortChunks(const glm::vec3& playerPos);
    void update(Player& p);
};

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

    Chunk* chunk = nullptr;

    for(const ChunkEntry& entry : chunks) {
        if(entry.pos.x == chunkX && entry.pos.y == chunkZ) {
            if(entry.chunk == nullptr) {
                return Block{Block::air};
            }

            return entry.chunk->blocks[localX][y][localZ];
        }
    }

    return Block{Block::air};
}

void world::markChunkDirty(int x, int z) {

    for(int i = 0; i < chunks.size(); i++) {

        if(chunks[i].chunk == nullptr) {
            continue;
        }
        if(chunks[i].pos.x == x && chunks[i].pos.y == z) {

            chunks[i].chunk->dirty = true;
        }
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

    Chunk* chunk = nullptr;

    for(int i = 0; i < chunks.size(); i++) {
        if(chunks[i].pos.x == chunkX && chunks[i].pos.y == chunkZ) {
            chunk = chunks[i].chunk;
        }
    }

    if(chunk == nullptr) {
        return;
    }

    chunk->dirty = true;

    chunk->blocks[localX][y][localZ].type = type.type;

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

ChunkEntry world::newChunk(int x, int z) {
    Chunk* chunk = new Chunk{};
    chunk->UploadGPU();
    chunks.push_back(ChunkEntry{.chunk = chunk});
    return chunks.back();
}

void world::sortChunks(const glm::vec3& playerPos) {
    const auto distanceSquared = [&](const ChunkEntry& entry) {
        glm::vec2 center(entry.pos.x * CHUNK_SIZE_X + CHUNK_SIZE_X * 0.5f,
                         entry.pos.y * CHUNK_SIZE_Z + CHUNK_SIZE_Z * 0.5f);

        glm::vec2 difference = center - glm::vec2(playerPos.x, playerPos.z);

        return glm::dot(difference, difference);
    };

    std::sort(chunks.begin(), chunks.end(), [&](const ChunkEntry& a, const ChunkEntry& b) {
        return distanceSquared(a) < distanceSquared(b);
    });
}
void world::update(Player& p) {
    sortChunks(p.pos);
}

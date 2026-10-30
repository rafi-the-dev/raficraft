#include "Worldgen.h"

#include <algorithm>
#include <cmath>

void generateChunk(Chunk& chunk, int chunkX, int chunkZ, FastNoiseSIMD* noise) {
    float* heights = noise->GetNoiseSet(chunkX * CHUNK_SIZE_X, 0, chunkZ * CHUNK_SIZE_Z,
                                        CHUNK_SIZE_X, 1, CHUNK_SIZE_Z);

    for(int x = 0; x < CHUNK_SIZE_X; x++) {
        for(int z = 0; z < CHUNK_SIZE_Z; z++) {
            float value = heights[x * CHUNK_SIZE_Z + z];
            int height = 64 + static_cast<int>(value * 50.0f);

            for(int y = 0; y < CHUNK_SIZE_Y; y++) {
                if(y > height) {
                    chunk.blocks[x][y][z].type = Block::air;
                } else if(y == height) {
                    chunk.blocks[x][y][z].type = Block::grass;
                } else if(y >= height - 4) {
                    chunk.blocks[x][y][z].type = Block::dirt;
                } else {
                    chunk.blocks[x][y][z].type = Block::stone;
                }
            }
        }
    }

    FastNoiseSIMD::FreeNoiseSet(heights);
}

void UpdateChunks(world& w, const glm::vec3& playerPos, FastNoiseSIMD* noise) {
    constexpr int MAX_GENERATIONS = 2;
    constexpr int MAX_MESH_UPDATES = 2;

    int centerX = static_cast<int>(std::floor(playerPos.x / CHUNK_SIZE_X));
    int centerZ = static_cast<int>(std::floor(playerPos.z / CHUNK_SIZE_Z));

    for(auto it = w.chunks.begin(); it != w.chunks.end();) {
        int dx = std::abs(it->pos.x - centerX);
        int dz = std::abs(it->pos.y - centerZ);

        int distance = std::max(dx, dz);

        if(distance > KEEP_DISTANCE) {
            int x = it->pos.x;
            int z = it->pos.y;

            if(it->chunk != nullptr) {
                it->chunk->UnloadGPU();
                delete it->chunk;
                it->chunk = nullptr;
            }

            it = w.chunks.erase(it);
            w.markNeighborsDirty(x, z);
            continue;
        }

        if(distance > RENDER_DISTANCE && it->chunk != nullptr) {
            if(it->chunk->vao != 0) {
                it->chunk->UnloadGPU();
            }
        }

        ++it;
    }

    for(int x = centerX - KEEP_DISTANCE; x <= centerX + KEEP_DISTANCE; x++) {
        for(int z = centerZ - KEEP_DISTANCE; z <= centerZ + KEEP_DISTANCE; z++) {
            glm::ivec2 pos(x, z);

            auto it = std::find_if(w.chunks.begin(), w.chunks.end(),
                                   [&](const ChunkEntry& entry) { return entry.pos == pos; });

            if(it == w.chunks.end()) {
                ChunkEntry entry{};
                entry.pos = pos;
                w.chunks.push_back(entry);
            }
        }
    }

    w.sortChunks(playerPos);

    int generated = 0;

    for(ChunkEntry& entry : w.chunks) {
        if(generated >= MAX_GENERATIONS) {
            break;
        }

        if(entry.chunk != nullptr) {
            continue;
        }

        entry.chunk = new Chunk{};

        generateChunk(*entry.chunk, entry.pos.x, entry.pos.y, noise);

        w.markNeighborsDirty(entry.pos.x, entry.pos.y);

        generated++;
    }

    int meshed = 0;

    for(ChunkEntry& entry : w.chunks) {
        if(meshed >= MAX_MESH_UPDATES) {
            break;
        }

        if(entry.chunk == nullptr) {
            continue;
        }

        int dx = std::abs(entry.pos.x - centerX);
        int dz = std::abs(entry.pos.y - centerZ);

        if(std::max(dx, dz) > RENDER_DISTANCE) {
            continue;
        }

        Chunk& chunk = *entry.chunk;

        if(!chunk.dirty && chunk.vao != 0) {
            continue;
        }

        chunk.BuildFaces(w, entry.pos.x, entry.pos.y);

        if(chunk.vao == 0) {
            chunk.UploadGPU();
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);

            glBufferData(GL_ARRAY_BUFFER, chunk.faces.size() * sizeof(Face), chunk.faces.data(),
                         GL_DYNAMIC_DRAW);
        }

        meshed++;
    }
}

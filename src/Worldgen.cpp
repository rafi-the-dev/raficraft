#include "Worldgen.h"

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

void GenWorld(world& w, int sizeX, int sizeZ) {
    FastNoiseSIMD* noise = FastNoiseSIMD::NewFastNoiseSIMD(1337);

    noise->SetNoiseType(FastNoiseSIMD::SimplexFractal);
    noise->SetFrequency(0.005f);
    noise->SetFractalOctaves(4);

    // Remove chunks outside boundaries
    for(auto it = w.chunks.begin(); it != w.chunks.end();) {
        const auto& pos = it->first;

        if(pos.x < 0 || pos.x >= sizeX || pos.y < 0 || pos.y >= sizeZ) {
            it = w.chunks.erase(it);
        } else {
            ++it;
        }
    }

    // Generate missing chunks
    for(int x = 0; x < sizeX; x++) {
        for(int z = 0; z < sizeZ; z++) {
            glm::ivec2 pos(x, z);

            if(w.chunks.find(pos) == w.chunks.end()) {
                Chunk chunk{};
                generateChunk(chunk, x, z, noise);

                w.chunks.emplace(pos, std::move(chunk));
            }
        }
    }

    delete noise;
}


void UpdateChunks(world& w, int centerX, int centerZ) {
    FastNoiseSIMD* noise = FastNoiseSIMD::NewFastNoiseSIMD(1337);

    noise->SetNoiseType(FastNoiseSIMD::SimplexFractal);
    noise->SetFrequency(0.005f);
    noise->SetFractalOctaves(4);

    for(auto it = w.chunks.begin(); it != w.chunks.end();) {
        int dx = std::abs(it->first.x - centerX);
        int dz = std::abs(it->first.y - centerZ);

        int distance = std::max(dx, dz);

        Chunk& chunk = it->second;

        if(distance > KEEP_DISTANCE) {
            chunk.UnloadGPU();
            it = w.chunks.erase(it);
            continue;
        }

        if(distance > RENDER_DISTANCE) {
            chunk.UnloadGPU();
        }

        ++it;
    }

    for(int x = centerX - KEEP_DISTANCE; x <= centerX + KEEP_DISTANCE; x++) {
        for(int z = centerZ - KEEP_DISTANCE; z <= centerZ + KEEP_DISTANCE; z++) {
            glm::ivec2 pos(x, z);

            if(w.chunks.find(pos) != w.chunks.end()) {
                continue;
            }

            auto [it, inserted] = w.chunks.try_emplace(pos);

            if(inserted) {
                generateChunk(it->second, x, z, noise);
                w.markNeighborsDirty(x, z);
            }
        }
    }

    delete noise;

    for(int x = centerX - RENDER_DISTANCE; x <= centerX + RENDER_DISTANCE; x++) {
        for(int z = centerZ - RENDER_DISTANCE; z <= centerZ + RENDER_DISTANCE; z++) {
            auto it = w.chunks.find(glm::ivec2(x, z));

            if(it == w.chunks.end()) {
                continue;
            }

            Chunk& chunk = it->second;

            if(chunk.vao == 0) {
                chunk.dirty = true;
            }
        }
    }
}

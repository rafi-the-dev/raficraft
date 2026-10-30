#include "Chunk.h"

Block Chunk::getBlock(int x, int y, int z) {
    if(x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) {
        return Block{};
    }

    return blocks[x][y][z];
}

void Chunk::BuildFaces(world& w, int chunkX, int chunkZ) {
    faces.clear();

    for(int x = 0; x < CHUNK_SIZE_X; x++) {
        for(int y = 0; y < CHUNK_SIZE_Y; y++) {
            for(int z = 0; z < CHUNK_SIZE_Z; z++) {
                Block block = blocks[x][y][z];

                if(block.type == Block::air) {
                    continue;
                }

                uint32_t texture = static_cast<uint32_t>(block.type) - 1;

                if(block.type == Block::grass) {
                    texture = 3;
                }

                int worldX = chunkX * CHUNK_SIZE_X + x;
                int worldZ = chunkZ * CHUNK_SIZE_Z + z;

                if(w.getBlock(worldX, y, worldZ + 1).type == Block::air) {
                    faces.push_back({x, y, z, packFace(0, texture)});
                }

                if(w.getBlock(worldX, y, worldZ - 1).type == Block::air) {
                    faces.push_back({x, y, z, packFace(1, texture)});
                }

                if(w.getBlock(worldX - 1, y, worldZ).type == Block::air) {
                    faces.push_back({x, y, z, packFace(2, texture)});
                }

                if(w.getBlock(worldX + 1, y, worldZ).type == Block::air) {
                    faces.push_back({x, y, z, packFace(3, texture)});
                }

                if(w.getBlock(worldX, y + 1, worldZ).type == Block::air) {
                    uint32_t topTexture = texture;

                    if(block.type == Block::grass) {
                        topTexture = 2;
                    }

                    faces.push_back({x, y, z, packFace(4, topTexture)});
                }

                if(w.getBlock(worldX, y - 1, worldZ).type == Block::air) {
                    faces.push_back({x, y, z, packFace(5, texture)});
                }
            }
        }
    }
}

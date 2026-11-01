#include "Chunk.h"
#include "World.h"

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

                // Torches use special geometry in the vertex shader.
                if(block.type == Block::torch) {
                    for(uint32_t direction = 0; direction < 6; direction++) {
                        faces.push_back({x, y, z, packTorchFace(direction)});
                    }

                    continue;
                }

                int worldX = chunkX * CHUNK_SIZE_X + x;
                int worldZ = chunkZ * CHUNK_SIZE_Z + z;

                // +Z (Direction 0)
                Block neighborZPositive;

                if(z + 1 < CHUNK_SIZE_Z) {
                    neighborZPositive = blocks[x][y][z + 1];
                } else {
                    neighborZPositive = w.getBlock(worldX, y, worldZ + 1);
                }

                if(neighborZPositive.type == Block::air || neighborZPositive.type == Block::torch) {
                    faces.push_back({x, y, z, packFace(0, getBlockTexture(block, 0))});
                }

                // -Z (Direction 1)
                Block neighborZNegative;

                if(z > 0) {
                    neighborZNegative = blocks[x][y][z - 1];
                } else {
                    neighborZNegative = w.getBlock(worldX, y, worldZ - 1);
                }

                if(neighborZNegative.type == Block::air || neighborZNegative.type == Block::torch) {
                    faces.push_back({x, y, z, packFace(1, getBlockTexture(block, 1))});
                }

                // -X (Direction 2)
                Block neighborXNegative;

                if(x > 0) {
                    neighborXNegative = blocks[x - 1][y][z];
                } else {
                    neighborXNegative = w.getBlock(worldX - 1, y, worldZ);
                }

                if(neighborXNegative.type == Block::air || neighborXNegative.type == Block::torch) {
                    faces.push_back({x, y, z, packFace(2, getBlockTexture(block, 2))});
                }

                // +X (Direction 3)
                Block neighborXPositive;

                if(x + 1 < CHUNK_SIZE_X) {
                    neighborXPositive = blocks[x + 1][y][z];
                } else {
                    neighborXPositive = w.getBlock(worldX + 1, y, worldZ);
                }

                if(neighborXPositive.type == Block::air || neighborXPositive.type == Block::torch) {
                    faces.push_back({x, y, z, packFace(3, getBlockTexture(block, 3))});
                }

                // +Y (Direction 4, top)
                bool topVisible = y + 1 >= CHUNK_SIZE_Y || blocks[x][y + 1][z].type == Block::air ||
                                  blocks[x][y + 1][z].type == Block::torch;

                if(topVisible) {
                    faces.push_back({x, y, z, packFace(4, getBlockTexture(block, 4))});
                }

                // -Y (Direction 5, bottom)
                bool bottomVisible = y == 0 || blocks[x][y - 1][z].type == Block::air ||
                                     blocks[x][y - 1][z].type == Block::torch;

                if(bottomVisible) {
                    faces.push_back({x, y, z, packFace(5, getBlockTexture(block, 5))});
                }
            }
        }
    }

    dirty = false;
}

void Chunk::UploadGPU() {
    if(vao != 0) {
        return;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, faces.size() * sizeof(Face), faces.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(0, 3, GL_INT, sizeof(Face), (void*)offsetof(Face, x));
    glVertexAttribDivisor(0, 1);

    glEnableVertexAttribArray(1);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Face), (void*)offsetof(Face, data));
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);
}

void Chunk::UnloadGPU() {
    if(vao != 0) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if(vbo != 0) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    faces.clear();
}

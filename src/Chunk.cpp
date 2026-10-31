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

    const auto neighborBlock = [&](int x, int y, int z) {
        if(x >= 0 && x < CHUNK_SIZE_X && z >= 0 && z < CHUNK_SIZE_Z) {
            return getBlock(x, y, z);
        }

        return w.getBlock(chunkX * CHUNK_SIZE_X + x, y, chunkZ * CHUNK_SIZE_Z + z);
    };

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

                if(z + 1 < CHUNK_SIZE_Z) {
                    if(blocks[x][y][z + 1].type == Block::air) {
                        faces.push_back({x, y, z, packFace(0, texture)});
                    }
                } else {
                    if(w.getBlock(worldX, y, worldZ + 1).type == Block::air) {
                        faces.push_back({x, y, z, packFace(0, texture)});
                    }
                }

                if(z > 0) {
                    if(blocks[x][y][z - 1].type == Block::air) {
                        faces.push_back({x, y, z, packFace(1, texture)});
                    }
                } else {
                    if(w.getBlock(worldX, y, worldZ - 1).type == Block::air) {
                        faces.push_back({x, y, z, packFace(1, texture)});
                    }
                }

                if(x > 0) {
                    if(blocks[x - 1][y][z].type == Block::air) {
                        faces.push_back({x, y, z, packFace(2, texture)});
                    }
                } else {
                    if(w.getBlock(worldX - 1, y, worldZ).type == Block::air) {
                        faces.push_back({x, y, z, packFace(2, texture)});
                    }
                }

                if(x + 1 < CHUNK_SIZE_X) {
                    if(blocks[x + 1][y][z].type == Block::air) {
                        faces.push_back({x, y, z, packFace(3, texture)});
                    }
                } else {
                    if(w.getBlock(worldX + 1, y, worldZ).type == Block::air) {
                        faces.push_back({x, y, z, packFace(3, texture)});
                    }
                }

                bool topVisible = y + 1 >= CHUNK_SIZE_Y || blocks[x][y + 1][z].type == Block::air;

                if(topVisible) {
                    uint32_t topTexture = texture;

                    if(block.type == Block::grass) {
                        topTexture = 2;
                    }

                    faces.push_back({x, y, z, packFace(4, topTexture)});
                }

                bool bottomVisible = y == 0 || blocks[x][y - 1][z].type == Block::air;

                if(bottomVisible) {
                    uint32_t bottomTexture = texture;

                    if(block.type == Block::grass) {
                        bottomTexture = static_cast<uint32_t>(Block::dirt) - 1;
                    }

                    faces.push_back({x, y, z, packFace(5, bottomTexture)});
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

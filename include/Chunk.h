#pragma once
#include "Block.h"
#include "Face.h"
#include "World.h"
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "vector"
#include <cstdint>
#include <iostream>

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

const glm::vec3 directions[6] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                                 {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};

struct world;

struct Chunk {
    Block blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];
    GLuint vao = 0;
    GLuint vbo = 0;
    bool dirty = true;

    std::vector<Face> faces;

    void BuildFaces(world& w, int chunkX, int chunkZ);

    Block getBlock(int x, int y, int z);
};

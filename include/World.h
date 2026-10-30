#pragma once
#include "Chunk.h"
#include "glm/glm.hpp"
#include <unordered_map>

struct Chunk;

struct ChunkHash {
    std::size_t operator()(const glm::ivec2& pos) const {
        std::size_t h1 = std::hash<int>{}(pos.x);
        std::size_t h2 = std::hash<int>{}(pos.y);

        return h1 ^ (h2 << 1);
    }
};

struct world {
    std::unordered_map<glm::vec2, Chunk, ChunkHash> chunks;
    Block getBlock(int x, int y, int z);
};

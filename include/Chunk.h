#pragma once
#include "Block.h"

constexpr int CHUNK_SIZE = 16;

struct Chunk {
    Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
};


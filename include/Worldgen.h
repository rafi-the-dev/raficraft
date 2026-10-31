#pragma once

#include "World.h"
#include <FastNoiseSIMD.h>

inline constexpr int RENDER_DISTANCE = 32;
inline constexpr int KEEP_DISTANCE = 34;

void generateChunk(Chunk& chunk, int chunkX, int chunkZ, FastNoiseSIMD* noise);

void UpdateChunks(world& w, const glm::vec3& playerPos, FastNoiseSIMD* noise);

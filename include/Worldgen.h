#pragma once
#include "World.h"
#include <Chunk.h>
#include <FastNoiseSIMD.h>

inline constexpr int RENDER_DISTANCE = 3;
inline constexpr int KEEP_DISTANCE = 5;

void generateChunk(Chunk& chunk, int chunkX, int chunkZ, FastNoiseSIMD* noise);
void GenWorld(world& w, int sizeX, int sizeZ);
void UpdateChunks(world& w, int centerX, int centerZ);

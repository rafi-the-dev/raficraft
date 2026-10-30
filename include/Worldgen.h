#pragma once
#include "World.h"
#include <Chunk.h>
#include <FastNoiseSIMD.h>

void generateChunk(Chunk& chunk, int chunkX, int chunkZ, FastNoiseSIMD* noise);
void GenWorld(world& w, int sizeX, int sizeZ);

#pragma once
#include <unordered_map>
#include "glm/glm.hpp"
#include "Chunk.h"

struct world {
    std::unordered_map<glm::vec2, Chunk>;
};

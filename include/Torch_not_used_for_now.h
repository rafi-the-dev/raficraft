#pragma once
#include <cstdint>
#include <glm/glm.hpp>

enum class TorchFacing { Up, North, South, East, West };

struct TorchEntry {
    glm::ivec3 pos;
    TorchFacing facing;
};

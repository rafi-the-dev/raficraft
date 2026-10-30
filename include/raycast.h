#pragma once
#include <glm/glm.hpp>

struct world;

struct RaycastHit {
    bool hit = false;
    glm::ivec3 block = {0, 0, 0};
    glm::ivec3 normal = {0, 0, 0};
};

RaycastHit raycast(world& w, glm::vec3 origin, glm::vec3 direction, float maxDistance);RaycastHit raycast(world& w, glm::vec3 origin, glm::vec3 direction, float maxDistance);

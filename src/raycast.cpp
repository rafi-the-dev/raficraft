#include "raycast.h"
#include "World.h"

#include <cmath>
#include <limits>

RaycastHit raycast(world& w, glm::vec3 origin, glm::vec3 direction, float maxDistance) {
    RaycastHit result;

    if(glm::dot(direction, direction) == 0.0f) {
        return result;
    }

    direction = glm::normalize(direction);

    glm::ivec3 block = glm::floor(origin);

    glm::ivec3 step = {
        direction.x > 0.0f ? 1 : (direction.x < 0.0f ? -1 : 0),
        direction.y > 0.0f ? 1 : (direction.y < 0.0f ? -1 : 0),
        direction.z > 0.0f ? 1 : (direction.z < 0.0f ? -1 : 0)
    };

    const float infinity = std::numeric_limits<float>::infinity();

    glm::vec3 delta = {
        step.x != 0 ? std::abs(1.0f / direction.x) : infinity,
        step.y != 0 ? std::abs(1.0f / direction.y) : infinity,
        step.z != 0 ? std::abs(1.0f / direction.z) : infinity
    };

    glm::vec3 distance = {
        step.x > 0 ? (block.x + 1.0f - origin.x) * delta.x
                   : (step.x < 0 ? (origin.x - block.x) * delta.x : infinity),

        step.y > 0 ? (block.y + 1.0f - origin.y) * delta.y
                   : (step.y < 0 ? (origin.y - block.y) * delta.y : infinity),

        step.z > 0 ? (block.z + 1.0f - origin.z) * delta.z
                   : (step.z < 0 ? (origin.z - block.z) * delta.z : infinity)
    };

    glm::ivec3 normal(0);
    float traveled = 0.0f;

    while(traveled <= maxDistance) {
        if(w.getBlock(block.x, block.y, block.z).type != Block::air) {
            result.hit = true;
            result.block = block;
            result.normal = normal;
            return result;
        }

        if(distance.x <= distance.y && distance.x <= distance.z) {
            traveled = distance.x;
            block.x += step.x;
            distance.x += delta.x;
            normal = {-step.x, 0, 0};
        } else if(distance.y <= distance.z) {
            traveled = distance.y;
            block.y += step.y;
            distance.y += delta.y;
            normal = {0, -step.y, 0};
        } else {
            traveled = distance.z;
            block.z += step.z;
            distance.z += delta.z;
            normal = {0, 0, -step.z};
        }
    }

    return result;
}

#pragma once

#include <glm/glm.hpp>

struct Frustum {
    glm::vec4 planes[6];

    void Update(const glm::mat4& viewProjection) {
        glm::vec4 row0(viewProjection[0][0], viewProjection[1][0], viewProjection[2][0],
                       viewProjection[3][0]);

        glm::vec4 row1(viewProjection[0][1], viewProjection[1][1], viewProjection[2][1],
                       viewProjection[3][1]);

        glm::vec4 row2(viewProjection[0][2], viewProjection[1][2], viewProjection[2][2],
                       viewProjection[3][2]);

        glm::vec4 row3(viewProjection[0][3], viewProjection[1][3], viewProjection[2][3],
                       viewProjection[3][3]);

        planes[0] = row3 + row0;
        planes[1] = row3 - row0;
        planes[2] = row3 + row1;
        planes[3] = row3 - row1;
        planes[4] = row3 + row2;
        planes[5] = row3 - row2;

        for(glm::vec4& plane : planes) {
            float length = glm::length(glm::vec3(plane));

            if(length > 0.0f) {
                plane /= length;
            }
        }
    }

    bool IsVisible(const glm::vec3& min, const glm::vec3& max) const {
        for(const glm::vec4& plane : planes) {
            glm::vec3 positive(plane.x >= 0.0f ? max.x : min.x, plane.y >= 0.0f ? max.y : min.y,
                               plane.z >= 0.0f ? max.z : min.z);

            float distance = glm::dot(glm::vec3(plane), positive) + plane.w;

            if(distance < 0.0f) {
                return false;
            }
        }

        return true;
    }
};

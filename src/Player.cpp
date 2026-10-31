#include "Player.h"
#include "raycast.h"

void Player::applyForces(float deltaTime) {
    velocity.y += GRAVITY * deltaTime;
}

bool Player::Collide(world& w) {
    constexpr float EPSILON = 0.0001f;

    glm::vec3 min = {pos.x - width * 0.5f, pos.y, pos.z - width * 0.5f};

    glm::vec3 max = {pos.x + width * 0.5f, pos.y + height, pos.z + width * 0.5f};

    int minX = static_cast<int>(std::floor(min.x + EPSILON));
    int minY = static_cast<int>(std::floor(min.y + EPSILON));
    int minZ = static_cast<int>(std::floor(min.z + EPSILON));

    int maxX = static_cast<int>(std::floor(max.x - EPSILON));
    int maxY = static_cast<int>(std::floor(max.y - EPSILON));
    int maxZ = static_cast<int>(std::floor(max.z - EPSILON));

    for(int x = minX; x <= maxX; x++) {
        for(int y = minY; y <= maxY; y++) {
            for(int z = minZ; z <= maxZ; z++) {
                if(w.getBlock(x, y, z).type != Block::air) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Player::moveAxis(world& w, int axis, float distance) {
    constexpr float STEP = 0.05f;

    float remaining = std::abs(distance);
    float direction = distance > 0.0f ? 1.0f : -1.0f;

    while(remaining > 0.0f) {
        float step = std::min(STEP, remaining) * direction;

        pos[axis] += step;

        if(Collide(w)) {
            pos[axis] -= step;
            velocity[axis] = 0.0f;

            if(axis == 1 && direction < 0.0f) {
                onGround = true;
            }

            return;
        }

        remaining -= std::abs(step);
    }
}

void Player::Update(float deltaTime, GLFWwindow* window, world& w, glm::vec3 cameraFront) {
    Move(deltaTime, window);

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround) {
        velocity.y = 5.0f;
        onGround = false;
    }

    applyForces(deltaTime);

    onGround = false;

    moveAxis(w, 0, velocity.x * deltaTime);
    moveAxis(w, 2, velocity.z * deltaTime);
    moveAxis(w, 1, velocity.y * deltaTime);

    static bool wasLeftPressed = false;

    bool leftPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    if(leftPressed && !wasLeftPressed) {
        glm::vec3 eyePosition = pos + glm::vec3(0.0f, 1.62f, 0.0f);

        RaycastHit hit = raycast(w, eyePosition, cameraFront, 5.0f);

        if(hit.hit) {
            w.setBlock(hit.block.x, hit.block.y, hit.block.z, Block{.type = Block::air});

            int chunkX =
                static_cast<int>(std::floor(static_cast<float>(hit.block.x) / CHUNK_SIZE_X));

            int chunkZ =
                static_cast<int>(std::floor(static_cast<float>(hit.block.z) / CHUNK_SIZE_Z));
        }
    }

    static bool wasRightPressed = false;

    bool rightPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if(rightPressed && !wasRightPressed) {
        glm::vec3 eyePos = pos + glm::vec3(0.0f, 1.62f, 0.0f);

        RaycastHit hit = raycast(w, eyePos, cameraFront, 5.0f);

        if(hit.hit) {

            glm::ivec3 placePos = hit.block + hit.normal;

            glm::vec3 playerMin = pos - glm::vec3(0.3, 0.0f, 0.3);
            glm::vec3 playerMax = pos + glm::vec3(0.3, 1.62f, 0.3);

            glm::vec3 blockMin = glm::vec3(placePos);
            glm::vec3 blockMax = blockMin + glm::vec3(1.0f);

            bool overlaps = playerMin.x < blockMax.x && playerMax.x > blockMin.x &&
                            playerMin.y < blockMax.y && playerMax.y > blockMin.y &&
                            playerMin.z < blockMax.z && playerMax.z > blockMin.z;

            if(w.getBlock(placePos.x, placePos.y, placePos.z).type == Block::air && !overlaps) {
                w.setBlock(placePos.x, placePos.y, placePos.z, Block{.type = Block::dirt});
            }
        }
    }

    wasLeftPressed = leftPressed;
    wasRightPressed = rightPressed;
}

void Player::Move(float deltaTime, GLFWwindow* window) {
    glm::vec3 forward = {cos(glm::radians(yaw)), 0.0f, sin(glm::radians(yaw))};

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 movement(0.0f);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        movement += forward;
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        movement -= forward;
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        movement -= right;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        movement += right;
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        moveSpeed = 10.f;
    } else {
        moveSpeed = 5.f;
    }

    if(glm::length(movement) > 0.0f) {
        movement = glm::normalize(movement);
    }

    velocity.x = movement.x * moveSpeed;
    velocity.z = movement.z * moveSpeed;
}

void Player::updatePlayerPhysics(float dt) {
    constexpr float GRAVITY = -9.81f;

    velocity.y += GRAVITY * dt;
    pos += velocity * dt;
}

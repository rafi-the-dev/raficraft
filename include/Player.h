#pragma once
#include "Chunk.h"
#include "input.h"
#include <GLFW/glfw3.h>

const float GRAVITY = -9.81f;

struct Player {
    glm::vec3 pos = {4.0f, 100.f, 4.f};
    glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

    float yaw = -90.0f;
    float pitch = 0.0f;

    float moveSpeed = 5.0f;

    float mouseSensitivity = 0.1f;

    float width = 0.6f;
    float height = 1.8f;

    bool onGround = false;

    void applyForces(float deltaTime);
    void Move(float deltaTime, GLFWwindow* window);
    bool Collide(world& w);
    void moveAxis(world& w, int axis, float distance);
    void Update(float deltaTime, GLFWwindow* window, world& w, glm::vec3 cameraFront);
    void updatePlayerPhysics(float dt);
};

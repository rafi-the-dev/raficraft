#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "demoShaderLoader.h"
#include <glm/gtc/type_ptr.hpp>

struct BlockOutline {
    GLuint vao = 0;
    GLuint vbo = 0;
    Shader shader;
    GLuint modelLocation = 0;
    GLuint viewLocation = 0;
    GLuint projectionLocation = 0;

    void Init();
    void Draw(const glm::ivec3& blockPos, glm::mat4x4 view, glm::mat4x4 projection);
};



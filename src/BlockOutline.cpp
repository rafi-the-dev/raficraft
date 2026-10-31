#include "BlockOutline.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void BlockOutline::Init() {
    const float vertices[] = {
        // Bottom square
        0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0,

        // Top square
        0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0,

        // Vertical edges
        0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1};
    shader.loadShaderProgramFromFile(RESOURCES_PATH "BlockOutline.vert",
                                     RESOURCES_PATH "BlockOutline.frag");

    modelLocation = glGetUniformLocation(shader.id, "u_model");
    viewLocation = glGetUniformLocation(shader.id, "u_view");
    projectionLocation = glGetUniformLocation(shader.id, "u_projection");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
}

void BlockOutline::Draw(const glm::ivec3& blockPos, glm::mat4x4 view, glm::mat4x4 projection) {
    if(vao == 0) {
        std::cout << "forgot to call Init on BlockOutline";
    }
    shader.bind();

    glm::mat4x4 model = glm::translate(glm::mat4x4(1.0f), glm::vec3(blockPos) - glm::vec3(0.002f));

    model = glm::scale(model, glm::vec3(1.004f));

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(vao);
    glLineWidth(4.0f);
    glDrawArrays(GL_LINES, 0, 24);
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

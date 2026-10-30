#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Face.h"
#include <Block.h>
#include <demoShaderLoader.h>
#include <input.h>
#include <openglDebug.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "World.h"
#include <Worldgen.h>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

world w;

// Texture paths
constexpr const char* DIRT_TEXTURE = RESOURCES_PATH "assets/blocks/dirt.png";
constexpr const char* STONE_TEXTURE = RESOURCES_PATH "assets/blocks/stone.png";
constexpr const char* GRASS_TOP_TEXTURE = RESOURCES_PATH "assets/blocks/grass_top.png";
constexpr const char* GRASS_SIDE_TEXTURE = RESOURCES_PATH "assets/blocks/grass_side.png";

constexpr const char* TEXTURE_PATHS[] = {
    DIRT_TEXTURE,        // 0
    STONE_TEXTURE,       // 1
    GRASS_TOP_TEXTURE,   // 2
    GRASS_SIDE_TEXTURE   // 3
};

// Camera
double lastX = 0.0;
double lastY = 0.0;

float yaw = -90.0f;
float pitch = 0.0f;

bool firstMouse = true;

glm::vec3 cameraPos(0.0f, 66.0f, 4.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    double deltaX = xpos - lastX;
    double deltaY = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    constexpr float sensitivity = 0.1f;

    yaw += static_cast<float>(deltaX) * sensitivity;
    pitch += static_cast<float>(deltaY) * sensitivity;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);
}

// Load a file and create an OpenGL texture
GLuint loadTexture(const char* path) {
    int width, height, channels;

    // OpenGL UVs assume the bottom of the image is V=0
    stbi_set_flip_vertically_on_load(true);

    unsigned char* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);

    if(!pixels) {
        std::cerr << "Failed to load texture: " << path << '\n';
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(pixels);

    return texture;
}

int main() {
    if(!glfwInit()) {
        return -1;
    }

    // OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Minecraft Renderer", nullptr, nullptr);

    if(!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    if(!glfwExtensionSupported("GL_ARB_bindless_texture")) {
        std::cerr << "Required GL_ARB_bindless_texture extension is unavailable\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    enableReportGlErrors();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glfwSwapInterval(1);

    Shader s;
    if(!s.loadShaderProgramFromFile(RESOURCES_PATH "vertex.vert", RESOURCES_PATH "fragment.frag")) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    s.bind();

    std::vector<GLuint> textures;

    for(const char* path : TEXTURE_PATHS) {
        GLuint texture = loadTexture(path);

        if(texture == 0) {
            for(GLuint loaded : textures) {
                glDeleteTextures(1, &loaded);
            }
            glfwDestroyWindow(window);
            glfwTerminate();
            return -1;
        }

        textures.push_back(texture);
    }

    std::vector<GLuint64> handles;

    for(GLuint texture : textures) {
        GLuint64 handle = glGetTextureHandleARB(texture);

        glMakeTextureHandleResidentARB(handle);
        handles.push_back(handle);
    }

    GLuint handleSSBO;
    glGenBuffers(1, &handleSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, handleSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, handles.size() * sizeof(GLuint64), handles.data(),
                 GL_DYNAMIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, handleSSBO);

    GenWorld(w, 2, 2);

    for(auto& [pos, chunk] : w.chunks) {
        chunk.BuildFaces(w, pos.x, pos.y);
        glGenVertexArrays(1, &chunk.vao);
        glGenBuffers(1, &chunk.vbo);

        glBindVertexArray(chunk.vao);
        glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);

        glBufferData(GL_ARRAY_BUFFER, chunk.faces.size() * sizeof(Face), chunk.faces.data(),
                     GL_STATIC_DRAW);

        // Face position
        glEnableVertexAttribArray(0);
        glVertexAttribIPointer(0, 3, GL_INT, sizeof(Face), (void*)offsetof(Face, x));
        glVertexAttribDivisor(0, 1);

        // Packed direction and texture ID
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Face), (void*)offsetof(Face, data));
        glVertexAttribDivisor(1, 1);
    }

    // Uniform locations
    GLint uView = glGetUniformLocation(s.id, "u_view");
    GLint uModel = glGetUniformLocation(s.id, "u_model");
    GLint uProjection = glGetUniformLocation(s.id, "u_projection");
    GLint uTextureIndex = glGetUniformLocation(s.id, "textureIndex");

    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    glm::mat4 proj(1.0f);

    float lastFrame = static_cast<float>(glfwGetTime());

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        if(width == 0 || height == 0) {
            continue;
        }

        // Mouse direction
        glm::vec3 direction;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFront = glm::normalize(direction);

        // Camera movement
        float cameraSpeed = 15.0f * deltaTime;

        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPos += cameraSpeed * cameraFront;
        }

        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraFront;
        }

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraRight;
        }

        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPos += cameraSpeed * cameraRight;
        }

        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            cameraPos += cameraSpeed * cameraUp;
        }

        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            cameraPos -= cameraSpeed * cameraUp;
        }

        // Matrices
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        proj =
            glm::perspective(glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        s.bind();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, handleSSBO);

        glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));

        for(auto& [pos, chunk] : w.chunks) {
            glm::mat4 chunkModel = glm::translate(
                glm::mat4(1.0f), glm::vec3(pos.x * CHUNK_SIZE_X, 0.0f, pos.y * CHUNK_SIZE_Z));
            glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(chunkModel));
            glBindVertexArray(chunk.vao);

            // ARB bindless samplers must be dynamically uniform within each draw.
            for(const FaceBatch& batch : chunk.faceBatches) {
                glUniform1ui(uTextureIndex, batch.textureIndex);
                glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6,
                    static_cast<GLsizei>(batch.faceCount), static_cast<GLuint>(batch.firstFace));
            }
        }

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

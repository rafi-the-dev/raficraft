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
#undef STB_IMAGE_IMPLEMENTATION

#include "BlockOutline.h"
#include "Chunk.h"
#include "Frustum.h"
#include "Player.h"
#include "World.h"
#include "raycast.h"
#include <Worldgen.h>
#include <array>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glui/glui.h>
#include <iostream>
#include <string.h>
#include <vector>

world w;
Player player;

// Texture paths
constexpr const char* DIRT_TEXTURE = RESOURCES_PATH "assets/blocks/dirt.png";
constexpr const char* STONE_TEXTURE = RESOURCES_PATH "assets/blocks/stone.png";
constexpr const char* GRASS_TOP_TEXTURE = RESOURCES_PATH "assets/blocks/grass_top.png";
constexpr const char* GRASS_SIDE_TEXTURE = RESOURCES_PATH "assets/blocks/grass_side.png";

constexpr const char* TEXTURE_PATHS[] = {
    DIRT_TEXTURE,      // 0
    STONE_TEXTURE,     // 1
    GRASS_TOP_TEXTURE, // 2
    GRASS_SIDE_TEXTURE // 3
};

enum states {
    menu,
    playing,
};

states state = states::menu;

// Camera
double lastX = 0.0;
double lastY = 0.0;

bool firstMouse = true;

glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

float crosshairVertices[] = {-10.0f, -1.0f,  10.0f, -1.0f,  10.0f,  1.0f,

                             -10.0f, -1.0f,  10.0f, 1.0f,   -10.0f, 1.0f,

                             -1.0f,  -10.0f, 1.0f,  -10.0f, 1.0f,   10.0f,

                             -1.0f,  -10.0f, 1.0f,  10.0f,  -1.0f,  10.0f};

BlockOutline blockoutline;
Frustum frustum;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if(state != states::playing) {
        return;
    }
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

    player.yaw += static_cast<float>(deltaX) * sensitivity;
    player.pitch += static_cast<float>(deltaY) * sensitivity;

    player.pitch = glm::clamp(player.pitch, -89.0f, 89.0f);
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
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

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

    blockoutline.Init();

    gl2d::init();

    gl2d::Renderer2D uiRenderer;
    uiRenderer.create();

    gl2d::Font uiFont;
    uiFont.createFromFile(RESOURCES_PATH "font/ANDYB.TTF");

    glui::RendererUi ui;
    bool previousMouseHeld = false;

    FastNoiseSIMD* noise = FastNoiseSIMD::NewFastNoiseSIMD(1337);

    noise->SetNoiseType(FastNoiseSIMD::SimplexFractal);
    noise->SetFrequency(0.005f);
    noise->SetFractalOctaves(4);

    int previousChunkX = static_cast<int>(std::floor(player.pos.x / CHUNK_SIZE_X));
    int previousChunkZ = static_cast<int>(std::floor(player.pos.z / CHUNK_SIZE_Z));

    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, characterCallback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if(glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

#ifndef NDEBUG
    enableReportGlErrors();
#endif

    GLuint crosshairVAO, crosshairVBO;

    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);

    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

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

    // Uniform locations
    GLint uView = glGetUniformLocation(s.id, "u_view");
    GLint uModel = glGetUniformLocation(s.id, "u_model");
    GLint uProjection = glGetUniformLocation(s.id, "u_projection");
    GLint uTextureIndex = glGetUniformLocation(s.id, "textureIndex");

    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    glm::mat4 proj(1.0f);

    float lastFrame = static_cast<float>(glfwGetTime());
    Shader crosshairShader;

    crosshairShader.loadShaderProgramFromFile(RESOURCES_PATH "crosshair.vert",
                                              RESOURCES_PATH "crosshair.frag");

    GLint uScreenSize = glGetUniformLocation(crosshairShader.id, "screenSize");

    while(!glfwWindowShouldClose(window)) {
        typedInput.clear();
        glfwPollEvents();

        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        if(width == 0 || height == 0) {
            continue;
        }

        if(state == states::playing) {
            UpdateChunks(w, player.pos, noise);
            player.Update(deltaTime, window, w, cameraFront);
        }

        int playerChunkX = static_cast<int>(std::floor(player.pos.x / CHUNK_SIZE_X));
        int playerChunkZ = static_cast<int>(std::floor(player.pos.z / CHUNK_SIZE_Z));

        cameraFront = {cos(glm::radians(player.yaw)) * cos(glm::radians(player.pitch)),
                       sin(glm::radians(player.pitch)),
                       sin(glm::radians(player.yaw)) * cos(glm::radians(player.pitch))};

        // Camera movement
        float cameraSpeed = 15.0f * deltaTime;

        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

        glm::vec3 eyePos = player.pos + glm::vec3(0.0f, 1.62f, 0.0f);

        view = glm::lookAt(eyePos, eyePos + cameraFront, cameraUp);

        float farPlane = RENDER_DISTANCE * CHUNK_SIZE_X * 1.5f;

        proj = glm::perspective(glm::radians(90.0f), static_cast<float>(width) / height, 0.1f,
                                farPlane);

        frustum.Update(proj * view);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        s.bind();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, handleSSBO);

        glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));

        for(int i = 0; i < w.chunks.size(); i++) {
            glm::vec2 pos = w.chunks[i].pos;
            if(w.chunks[i].chunk == nullptr) {
                continue;
            }
            Chunk* chunk = w.chunks[i].chunk;
            int dx = std::abs(pos.x - playerChunkX);
            int dz = std::abs(pos.y - playerChunkZ);

            if(std::max(dx, dz) > RENDER_DISTANCE) {
                continue;
            }

            glm::vec3 chunkMin(pos.x * CHUNK_SIZE_X, 0.0f, pos.y * CHUNK_SIZE_Z);

            glm::vec3 chunkMax = chunkMin + glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);

            if(!frustum.IsVisible(chunkMin, chunkMax)) {
                continue;
            }

            if(chunk->vao == 0) {
                continue;
            }

            glm::mat4 chunkModel = glm::translate(
                glm::mat4(1.0f), glm::vec3(pos.x * CHUNK_SIZE_X, 0.0f, pos.y * CHUNK_SIZE_Z));

            glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(chunkModel));
            glBindVertexArray(chunk->vao);

            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(chunk->faces.size()));
        }

        if(state == states::playing) {

            RaycastHit hit = raycast(w, eyePos, cameraFront, 5.0f);

            if(hit.hit) {
                blockoutline.Draw(hit.block, view, proj);
            }

            s.bind();

            glDisable(GL_DEPTH_TEST);

            crosshairShader.bind();

            glUniform2f(uScreenSize, static_cast<float>(width), static_cast<float>(height));

            glBindVertexArray(crosshairVAO);
            glDrawArrays(GL_TRIANGLES, 0, 12);

            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);
        }

        if(state == states::menu) {
            static char seedText[12] = "1337";
            static bool invalidSeed = false;

            uiRenderer.updateWindowMetrics(width, height);

            ui.Begin(1);

            ui.Text("raficraft", {1, 1, 1, 1});

            ui.BeginMenu("Play", {1, 1, 1, 1}, {});

            ui.Text("World seed", {1, 1, 1, 1});
            ui.InputText("Seed: ", seedText, sizeof(seedText), {1, 1, 1, 1});

            if(invalidSeed) {
                ui.Text("Enter a valid whole number", {1, 0.3f, 0.3f, 1});
            }

            if(ui.Button("Create World", {1, 1, 1, 1})) {
                int seed = 0;
                const char* end = seedText + std::strlen(seedText);
                auto result = std::from_chars(seedText, end, seed);

                invalidSeed = result.ec != std::errc{} || result.ptr != end;

                if(!invalidSeed) {
                    noise->SetSeed(seed);
                    state = states::playing;
                    firstMouse = true;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
            }

            ui.EndMenu();

            if(ui.Button("Quit", {1, 1, 1, 1})) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            ui.End();

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int windowWidth, windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);

            // Convert window coordinates to framebuffer coordinates for HiDPI.
            glm::ivec2 mousePosition = {static_cast<int>(mouseX * width / windowWidth),
                                        static_cast<int>(mouseY * height / windowHeight)};

            bool mouseHeld = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

            ui.renderFrame(uiRenderer, uiFont, mousePosition, mouseHeld && !previousMouseHeld,
                           mouseHeld, !mouseHeld && previousMouseHeld,
                           false, // Escape released, unused here
                           typedInput, deltaTime);

            previousMouseHeld = mouseHeld;

            glDisable(GL_CULL_FACE);
            uiRenderer.flush();
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
        }

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

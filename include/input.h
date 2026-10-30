#pragma once

#include <GLFW/glfw3.h>
#include <string>

#define USE_GPU_ENGINE 0

extern std::string typedInput;

void characterCallback(GLFWwindow* window, unsigned int codepoint);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

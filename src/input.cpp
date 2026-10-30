#include "input.h"

#ifdef _WIN32
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}
#endif

std::string typedInput;

void characterCallback(GLFWwindow*, unsigned int codepoint)
{
    // Seed input only needs digits and a minus sign.
    if((codepoint >= '0' && codepoint <= '9') || codepoint == '-')
    {
        typedInput.push_back(static_cast<char>(codepoint));
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_BACKSPACE &&
       (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        typedInput.push_back('\b');
    }

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

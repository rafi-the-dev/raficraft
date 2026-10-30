#include "input.h"
#include <cstdlib>
#include <iostream>

void require(bool condition)
{
    if(!condition)
    {
        std::cerr << "Input regression failed\n";
        std::exit(1);
    }
}

bool closeRequested = false;
extern "C" void glfwSetWindowShouldClose(GLFWwindow*, int value)
{
    closeRequested = value == GLFW_TRUE;
}

int main()
{
    typedInput.clear();
    for(unsigned int character : {'-', '1', '3', '3', '7'})
    {
        characterCallback(nullptr, character);
    }
    characterCallback(nullptr, 'a');
    characterCallback(nullptr, 0x1F600);
    require(typedInput == "-1337");

    key_callback(nullptr, GLFW_KEY_BACKSPACE, 0, GLFW_PRESS, 0);
    key_callback(nullptr, GLFW_KEY_BACKSPACE, 0, GLFW_REPEAT, 0);
    key_callback(nullptr, GLFW_KEY_BACKSPACE, 0, GLFW_RELEASE, 0);
    require(typedInput == "-1337\b\b");

    typedInput.clear();
    characterCallback(nullptr, '2');
    require(typedInput == "2");

    key_callback(nullptr, GLFW_KEY_ESCAPE, 0, GLFW_RELEASE, 0);
    require(!closeRequested);
    key_callback(nullptr, GLFW_KEY_ESCAPE, 0, GLFW_PRESS, 0);
    require(closeRequested);
}

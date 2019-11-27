#pragma once

#include <GLFW/glfw3.h>

namespace opengl_demo
{
#if 0
    extern Camera camera;
#endif

    constexpr int SCR_WIDTH = 1280;
    constexpr int SCR_HEIGHT = 720;

    void process_input(GLFWwindow *window, float deltaTime);
    GLFWwindow* setup_window(const char* title);
}

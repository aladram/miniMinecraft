#pragma once

#include <GLFW/glfw3.h>

#include <opengl-demo/camera.hh>

namespace opengl_demo
{
    extern camera camera;

    constexpr int SCR_WIDTH = 1280;
    constexpr int SCR_HEIGHT = 720;

    void process_input(GLFWwindow *window, float deltaTime);
    GLFWwindow* setup_window(const char* title);
}

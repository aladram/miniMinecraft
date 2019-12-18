#include <opengl-demo/window.hh>

extern "C" {
#include <err.h>
}
#include <cfloat>
#include <cstdlib>
#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <opengl-demo/camera.hh>

using namespace opengl_demo;

typename opengl_demo::camera opengl_demo::camera{
    glm::vec3(-3.f, 0.f, -3.f),
    glm::vec3(1.f, 0.f, 0.f),
    glm::vec3(0.f, 1.f, 0.f)
};

void opengl_demo::process_input(GLFWwindow *window, float dt)
{
    constexpr float lambda = 2.f;

    // Exit on ESC press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float lambda_t = lambda * dt;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.position += lambda_t * camera.forward();
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.position -= lambda_t * camera.forward();
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.position -= lambda_t * camera.right();
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.position += lambda_t * camera.right();
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;

    glViewport(0, 0, width, height);
}

static void mouse_callback(GLFWwindow* window, double x, double y)
{
    (void)window;

    constexpr float max_yaw = M_PI / 2.f - 0.5f;
    constexpr float min_yaw = - max_yaw;
    constexpr float lambda = 0.001f;

    static float x_prev = x, y_prev = y;
    static float yaw = M_PI / 4, pitch = 0;

    float dx = x - x_prev;
    float dy = - (y - y_prev);

    x_prev = x;
    y_prev = y;
    
    yaw += lambda * dx;
    pitch = std::max(min_yaw, std::min(max_yaw, pitch + lambda * dy));

    camera.forward_ = glm::normalize(glm::vec3(
            std::cos(yaw) * std::cos(pitch),
            std::sin(pitch),
            std::sin(yaw) * std::cos(pitch)
        ));
}

void glfw_error_callback(int, const char *description)
{
    const char *error_description;
    glfwGetError(&error_description);
    warnx("An error occured with GLFW: %s", error_description);
}

GLFWwindow* opengl_demo::setup_window(const char* title)
{
    if (!glfwInit())
        glfwTerminate();

    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /*
     * MacOS compatibility
     * c.f. https://www.khronos.org/opengl/wiki/OpenGL_Context#Forward_compatibility
     */
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

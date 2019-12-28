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
#include <opengl-demo/world/world.hh>

using namespace opengl_demo;

camera_t opengl_demo::camera{
    glm::vec3(0.f),
    glm::vec3(std::sqrt(2)/2.f, 0.f, std::sqrt(2)/2.f),
    glm::vec3(0.f, 1.f, 0.f)
};

static world_t* world_ptr;
static bool left_click;

void opengl_demo::process_input(GLFWwindow* window, world& world, float dt)
{
    if (!world_ptr)
        world_ptr = &world;

    constexpr float lambda = 4.f;
    constexpr float jumpDelay = 0.1f;
    constexpr float breakDelay = 0.25f;

    static float lastJump = 2 * jumpDelay;
    static float lastBreak = 2 * breakDelay;

    // Exit on ESC press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    vector3 forward = camera.forward();
    forward[1] = 0;
    vector3 right = camera.right();
    right[1] = 0;

    // Move on arrow keys press
    float lambda_t = lambda * dt;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        world.player.position += lambda_t * forward;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        world.player.position -= lambda_t * forward;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        world.player.position -= lambda_t * right;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        world.player.position += lambda_t * right;

    // Jump on space press
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && lastJump > jumpDelay)
        world.player.velocity.y = 2.f * lambda;

    // Check last jump time
    if (std::abs(world.player.velocity.y) < FLT_EPSILON)
        lastJump += dt;
    else
        lastJump = 0;

    // Check left click
    if (left_click && lastBreak > breakDelay)
    {
        lastBreak = 0;
        auto block = camera.target_block(world);
        if (block)
        {
            // Set to air
            block->texture_ids = { 0, 0, 0 };
            world.set_block(block->position, *block);
        }
    }
    else
        lastBreak += dt;

    world.player.update(world, 0);
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    (void)window;
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT)
        left_click = (action == GLFW_PRESS);
}

void glfw_error_callback(int, const char *description)
{
    warnx("An error occured with GLFW: %s", description);
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

extern "C" {
#include <err.h>
}
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <map>
#include <numeric>
#include <stdexcept>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opengl-demo/my-shaders.hh>
#include <opengl-demo/program.hh>
#include <opengl-demo/renderer.hh>
#include <opengl-demo/window.hh>
#include <opengl-demo/world/block.hh>
#include <opengl-demo/world/entity.hh>
#include <opengl-demo/world/generator.hh>
#include <opengl-demo/world/world.hh>
#include <opengl-demo/utils/opengl_utils.hh>

using namespace opengl_demo;

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    (void)source, (void)id, (void)length, (void)userParam;

    if (type != GL_DEBUG_TYPE_ERROR)
        return;
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

void display_fps(float dt)
{
    static float t = 0;

    t += dt;
    if (t < 1)
        return;

    t = 0;
    std::cout << "FPS: " << (int)std::floor(1/dt) << std::endl;
}

int main()
{
    auto window = setup_window("OpenGL demo");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        errx(1, "Failed to initialize GLAD");

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef NDEBUG
    // Enable debug output
    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(MessageCallback, 0);
#endif

    program program{compile_my_shaders()};
    program.use();

    world world = generate_world();
    renderer renderer{world};

    float t_prev = glfwGetTime();

#ifndef NDEBUG
    // Disable vsync
    glfwSwapInterval(0);
#endif

    while (!glfwWindowShouldClose(window))
    {
        float t = glfwGetTime();
        float dt = t - t_prev;

        display_fps(dt);

        process_input(window, world, dt);

        world.player.update(world, dt);
        camera.position = world.player.eyes_position();

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        glm::mat4 projection = glm::perspective(
                // FOV
                glm::radians(70.f),
                // Aspect ratio
                (float)width / (float)height,
                // Near & far
                0.1f, 100.0f
            );
        glm::mat4 view = camera.look_at();

        program.put("view_proj", projection * view);
        program.use();

        // Blue sky background
        glClearColor(58.8 / 100., 83.5 / 100., 100. / 100., 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        t_prev = t;
    }

    glfwTerminate();
    return 0;
}

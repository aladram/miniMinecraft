extern "C" {
#include <err.h>
}
#include <cmath>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <opengl-demo/window.hh>
#include <opengl-demo/math.hh>
#include <opengl-demo/rendering/renderer.hh>
#include <opengl-demo/world/generator.hh>
#include <opengl-demo/world/world.hh>

using namespace opengl_demo;

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

        renderer.render(width, height, camera);

        glfwSwapBuffers(window);
        glfwPollEvents();

        t_prev = t;
    }

    glfwTerminate();
    return 0;
}

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opengl-demo/my-shaders.hh>
#include <opengl-demo/program.hh>
#include <opengl-demo/window.hh>

#include <array>
#include <map>
#include <numeric>
#include <stdexcept>
#include <vector>

using namespace opengl_demo;

float lastFrame = 0.0f;

static std::string get_path(const std::string& path) {
	return "../resources/" + path;
}

int main()
{
    auto window = setup_window("Transparency (OpenGL)");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    // Enable opengl_demo
    glEnable(GL_DEPTH_TEST);

    program program{compile_my_shaders()};

    program.use();
    program.put("my_texture", 0);

    glm::mat4 projection = glm::perspective(
            glm::radians(45.f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            100.0f
        );
    glm::mat4 view = glm::lookAt(
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 0.f, -1.f),
            glm::vec3(0.f, 1.f, 0.f)
        );

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_input(window, deltaTime);

        // Blue sky background
        glClearColor(135.f / 255.f, 206.f / 255.f, 235.f / 255.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();
#if 0
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
#endif

        program.put("proj_view_mat", projection * view);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


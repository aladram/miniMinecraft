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
    auto window = setup_window("OpenGL demo");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    glEnable(GL_DEPTH_TEST);

    program program{compile_my_shaders()};
    program.use();
#if 0
    program.put("my_texture", 0);
#endif

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    GLuint my_vbo;
    glGenBuffers(1, &my_vbo);

    GLuint my_vao;
    glGenVertexArrays(1, &my_vao);

    glBindVertexArray(my_vao);
    glBindBuffer(GL_ARRAY_BUFFER, my_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
        glBindVertexArray(my_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
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


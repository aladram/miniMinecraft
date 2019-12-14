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

static GLuint generate_vao(float *vertices, size_t vertices_size, unsigned *indices, size_t indices_size)
{
    GLuint my_vao, my_vbo, my_ebo;
    glGenVertexArrays(1, &my_vao);
    glGenBuffers(1, &my_vbo);
    glGenBuffers(1, &my_ebo);

    glBindVertexArray(my_vao);
      glBindBuffer(GL_ARRAY_BUFFER, my_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      // Must not unbind EBO after that: will stay bind for this VAO
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    return my_vao;
}

int main()
{
    auto window = setup_window("OpenGL demo");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    glEnable(GL_DEPTH_TEST);

    program program{compile_my_shaders()};
    program.use();

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    GLuint my_vao = generate_vao(vertices, sizeof(vertices), indices, sizeof(indices));

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

    program.put("proj_view_mat", projection * view);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_input(window, deltaTime);

        // Blue sky background
        glClearColor(135.f / 255.f, 206.f / 255.f, 235.f / 255.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float t = glfwGetTime();

        program.use();
        program.put("color", glm::vec4(0.f, (sin(t) / 2.0f) + 0.5f, 0.f, 0.f));
        glBindVertexArray(my_vao);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(*indices), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

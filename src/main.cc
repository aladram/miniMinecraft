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
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include <opengl-demo/my-shaders.hh>
#include <opengl-demo/program.hh>
#include <opengl-demo/window.hh>
#include <opengl-demo/primitives/cube.hh>
#include <opengl-demo/world/block.hh>
#include <opengl-demo/world/entity.hh>
#include <opengl-demo/world/world.hh>
#include <opengl-demo/utils/opengl_utils.hh>

using namespace opengl_demo;

extern unsigned char terrain_png_buf[0];
extern unsigned int terrain_png_buf_len;

static void generate_texture(void *buf, unsigned buf_size)
{
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load_from_memory((const stbi_uc *)buf, (int)buf_size, &width, &height, &nrChannels, 0);
    if (!data)
        errx(1, "Texture load failed");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

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

int main()
{
    auto window = setup_window("OpenGL demo");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    program program{compile_my_shaders()};
    program.use();

    world world = generate_world();
    auto positions = world.blocks;

    GLuint my_vao = generate_cube_vao();

    GLuint positions_vbo;
    glGenBuffers(1, &positions_vbo);

    glBindVertexArray(my_vao);
      glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(block) * positions.size(), positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(block), (void*)offsetof(block, position));
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(block), (void*)offsetof(block, texture_ids));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(3);
      glEnableVertexAttribArray(4);
      glVertexAttribDivisor(3, 1);
      glVertexAttribDivisor(4, 1);
    glBindVertexArray(0);

    GLuint my_texture;
    glGenTextures(1, &my_texture);

    glBindTexture(GL_TEXTURE_2D, my_texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      generate_texture(terrain_png_buf, terrain_png_buf_len);
    glBindTexture(GL_TEXTURE_2D, 0);

    float t_prev = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float t = glfwGetTime();
        float dt = t - t_prev;

        process_input(window, world, dt);

        world.player.update(world, dt);
        camera.position = world.player.position + vector3{0.3, 1.7, 0.3};

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        glm::mat4 projection = glm::perspective(
                glm::radians(70.f),
                (float)width / (float)height,
                0.1f,
                100.0f
            );

        glm::mat4 view = camera.look_at();

        program.put("view_proj", projection * view);


        // Blue sky background
        glClearColor(135.f / 255.f, 206.f / 255.f, 235.f / 255.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();
        program.put("color", glm::vec4(0.f, (sin(t) / 2.0f) + 0.5f, 0.f, 0.f));
        glBindTexture(GL_TEXTURE_2D, my_texture);
        glBindVertexArray(my_vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 12 * 3, positions.size());

        glfwSwapBuffers(window);
        glfwPollEvents();

        t_prev = t;
    }

    glfwTerminate();
    return 0;
}

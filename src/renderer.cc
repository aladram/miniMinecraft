#include <opengl-demo/renderer.hh>

extern "C" {
#include <err.h>
}

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <opengl-demo/primitives/cube.hh>
#include <opengl-demo/world/block.hh>
#include <opengl-demo/world/world.hh>

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

renderer::renderer(const typename opengl_demo::world& _world)
    : world{_world}
{
    // Generate a single block VAO
    world_vao = generate_cube_vao();
    // Generate block positions VBO for this VAO
    glGenBuffers(1, &positions_vbo);
    glBindVertexArray(world_vao);
      glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(block), (void*)offsetof(block, position));
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(block), (void*)offsetof(block, texture_ids));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(3);
      glEnableVertexAttribArray(4);
      glVertexAttribDivisor(3, 1);
      glVertexAttribDivisor(4, 1);
    glBindVertexArray(0);

    // Texture loading
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      generate_texture(terrain_png_buf, terrain_png_buf_len);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderer::render() const
{
    std::vector<block> blocks;
    for (const auto& chunk: world.chunks)
        for (const auto& block: chunk.second.blocks)
            // Check if not air
            if (block.texture_ids[0])
                blocks.push_back(block);

    glBindVertexArray(world_vao);
      glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(block) * blocks.size(), blocks.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(world_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 12 * 3, blocks.size());
}

#include <opengl-demo/renderer.hh>

extern "C" {
#include <err.h>
}
#include <cassert>
#include <cstdio>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <opengl-demo/my-shaders.hh>
#include <opengl-demo/program.hh>
#include <opengl-demo/math.hh>
#include <opengl-demo/camera.hh>
#include <opengl-demo/window.hh>
#include <opengl-demo/vertex_attributes.hh>
#include <opengl-demo/utils/opengl_utils.hh>
#include <opengl-demo/primitives/cube.hh>
#include <opengl-demo/world/block.hh>
#include <opengl-demo/world/chunk.hh>
#include <opengl-demo/world/world.hh>

using namespace opengl_demo;

extern unsigned char terrain_png_buf[0];
extern unsigned int terrain_png_buf_len;

static void game_texture(void *buf, unsigned buf_size)
{
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load_from_memory((const stbi_uc *)buf, (int)buf_size, &width, &height, &nrChannels, 0);
    if (!data)
        errx(1, "Texture load failed");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

// From https://www.khronos.org/opengl/wiki/Debug_Output
void GLAPIENTRY
message_callback( GLenum source,
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

renderer::renderer(const typename opengl_demo::world& _world)
    : world{_world}
{
#if !defined(NDEBUG) && OPENGL_VERSION_MAJOR >= 4 && OPENGL_VERSION_MINOR >= 3
    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, 0);
#endif

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunci(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program = program_t{compile_my_shaders()};
    program_2 = program_t{compile_my_shaders_2()};
    program.use();

    // Generate single block VAOs
    world_vao = generate_cube_vao();
    leaves_vao = generate_cube_mirror_vao();
    water_vao = generate_quad_vao();
    screen_vao = generate_screen_vao();

    // Generate block positions VBO for world VAO
    glGenBuffers(1, &positions_vbo);
    glBindVertexArray(world_vao);
      glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
        glVertexAttribPointer(ATTRIB_BLOCKS_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(gl_block), (void*)offsetof(gl_block, position));
        glVertexAttribPointer(ATTRIB_BLOCKS_TEXTURES_IDS, 3, GL_FLOAT, GL_FALSE, sizeof(gl_block), (void*)offsetof(gl_block, texture_ids));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(ATTRIB_BLOCKS_POSITIONS);
      glEnableVertexAttribArray(ATTRIB_BLOCKS_TEXTURES_IDS);
      glVertexAttribDivisor(ATTRIB_BLOCKS_POSITIONS, 1);
      glVertexAttribDivisor(ATTRIB_BLOCKS_TEXTURES_IDS, 1);
    glBindVertexArray(0);

    // Generate block positions VBO for leaves VAO
    glGenBuffers(1, &leaves_positions_vbo);
    glBindVertexArray(leaves_vao);
      glBindBuffer(GL_ARRAY_BUFFER, leaves_positions_vbo);
        glVertexAttribPointer(ATTRIB_BLOCKS_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(gl_block), (void*)offsetof(gl_block, position));
        glVertexAttribPointer(ATTRIB_BLOCKS_TEXTURES_IDS, 3, GL_FLOAT, GL_FALSE, sizeof(gl_block), (void*)offsetof(gl_block, texture_ids));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(ATTRIB_BLOCKS_POSITIONS);
      glEnableVertexAttribArray(ATTRIB_BLOCKS_TEXTURES_IDS);
      glVertexAttribDivisor(ATTRIB_BLOCKS_POSITIONS, 1);
      glVertexAttribDivisor(ATTRIB_BLOCKS_TEXTURES_IDS, 1);
    glBindVertexArray(0);

    // Generate block positions VBO for water VAO
    glGenBuffers(1, &water_positions_vbo);
    glBindVertexArray(water_vao);
      glBindBuffer(GL_ARRAY_BUFFER, water_positions_vbo);
        glVertexAttribPointer(ATTRIB_BLOCKS_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(gl_block), (void*)offsetof(gl_block, position));
        glVertexAttribPointer(ATTRIB_BLOCKS_TEXTURES_IDS, 3, GL_FLOAT, GL_FALSE, sizeof(gl_block), (void*)offsetof(gl_block, texture_ids));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(ATTRIB_BLOCKS_POSITIONS);
      glEnableVertexAttribArray(ATTRIB_BLOCKS_TEXTURES_IDS);
      glVertexAttribDivisor(ATTRIB_BLOCKS_POSITIONS, 1);
      glVertexAttribDivisor(ATTRIB_BLOCKS_TEXTURES_IDS, 1);
    glBindVertexArray(0);

    // Texture loading
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      game_texture(terrain_png_buf, terrain_png_buf_len);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderer::render(int width, int height, camera_t& camera)
{
    assert(width >= 0);
    assert(height >= 0);

    // Regenerate framebuffer if width or height has changed
    if (width != width_ || height != height_)
    {
        // Delete previous buffers if existing
        if (width_ != -1)
        {
            glDeleteFramebuffers(1, &fbo);
            glDeleteRenderbuffers(1, &depth_rbo);
            GLuint texs[] = { colors_tex, positions_tex, normals_tex };
            glDeleteTextures(sizeof(texs) / sizeof(*texs), texs);
        }

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
          glGenRenderbuffers(1, &depth_rbo);
          glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
          glBindRenderbuffer(GL_RENDERBUFFER, 0);

          // Generate colors texture
          glGenTextures(1, &colors_tex);
          glBindTexture(GL_TEXTURE_2D, colors_tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colors_tex, 0);
          glBindTexture(GL_TEXTURE_2D, 0);

          // Generate positions texture
          glGenTextures(1, &positions_tex);
          glBindTexture(GL_TEXTURE_2D, positions_tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, positions_tex, 0);
          glBindTexture(GL_TEXTURE_2D, 0);

          // Generate normals texture
          glGenTextures(1, &normals_tex);
          glBindTexture(GL_TEXTURE_2D, normals_tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, normals_tex, 0);
          glBindTexture(GL_TEXTURE_2D, 0);

          GLuint bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
          glDrawBuffers(sizeof(bufs) / sizeof(*bufs), bufs);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        width_ = width;
        height_ = height;
   }

    glm::mat4 projection = glm::perspective(
            // FOV
            glm::radians(70.f),
            // Aspect ratio
            (float)width / (float)height,
            // Near & far
            0.1f, 100.0f
        );
    glm::mat4 view = camera.look_at();

    program.use();
    program.put("view_proj", projection * view);
    program.put("ambient_light", vector3(0.7));
    program.put("dir_light", - vector3(1.f/2.f, std::sqrt(3.f)/2.f, 0));
    program.put("dir_light_color", vector3(0.7));
    program.put("tex", 0);

    // Binding our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClear(GL_DEPTH_BUFFER_BIT);

    // Blue sky background
    static constexpr float sky_color[] = { 58.8 / 100., 83.5 / 100., 100. / 100., 1.0f };
    static constexpr float inf_pos[] = { INFINITY, INFINITY, INFINITY };
    glClearBufferfv(GL_COLOR, 0, sky_color);
    glClearBufferfv(GL_COLOR, 1, inf_pos);
    glClearBufferfv(GL_COLOR, 2, inf_pos);

    static constexpr float max_render_distance = 100.f + std::sqrt(2.) * chunk_t::N;
    std::vector<gl_block> blocks;
    std::vector<gl_block> leaves;
    std::vector<gl_block> water;
    for (const auto& chunk: world.chunks)
    {
        // Skip chunk if too far
        vector3 loc = chunk.first * (int)chunk_t::N;
        if (glm::distance(loc, world.player.eyes_position()) > max_render_distance)
            continue;

        // Add blocks to VBO
        for (const auto& block: chunk.second.blocks)
        {
            if (block.type == block_type::WATER)
            {
                // Render only if air on top
                if (world.get_block(block.position + vector3(0, 1, 0)).type == block_type::AIR)
                    water.push_back(block.to_opengl());
            }
            else if (block.type == block_type::LEAVES)
                leaves.push_back(block.to_opengl());
            else if (block.visible)
                blocks.push_back(block.to_opengl());
        }
    }

    // Texture binding
    glBindTexture(GL_TEXTURE_2D, texture);

    // Blocks rendering
    glBindVertexArray(world_vao);
      glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(gl_block) * blocks.size(), blocks.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 12 * 3, blocks.size());
    glBindVertexArray(0);

    // Water rendering
    glBindVertexArray(water_vao);
      glBindBuffer(GL_ARRAY_BUFFER, water_positions_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(gl_block) * water.size(), water.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 12 * 3, water.size());
    glBindVertexArray(0);

    // Leaves rendering
    program.put("dir_light_color", vector3(0));
    glBindVertexArray(leaves_vao);
      glBindBuffer(GL_ARRAY_BUFFER, leaves_positions_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(gl_block) * leaves.size(), leaves.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 12 * 3, leaves.size());
    glBindVertexArray(0);

    glReadBuffer(GL_COLOR_ATTACHMENT1);
    vector3 loc;
    glReadPixels(width / 2, height / 2, 1, 1, GL_RGB, GL_FLOAT, (GLvoid*)&loc);
    if (loc.x != INFINITY && loc.y != INFINITY && loc.z != INFINITY)
        camera.target_loc = loc;
    else
        camera.target_loc = std::nullopt;

    glReadBuffer(GL_COLOR_ATTACHMENT2);
    vector3 normal;
    glReadPixels(width / 2, height / 2, 1, 1, GL_RGB, GL_FLOAT, (GLvoid*)&normal);
    if (normal.x != INFINITY && normal.y != INFINITY && normal.z != INFINITY)
        camera.target_normal = normal;
    else
        camera.target_loc = std::nullopt;

    // Binding default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colors_tex);

    program_2.use();
    program_2.put("tex", 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, colors_tex);

    glBindVertexArray(screen_vao);
      glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
    glBindVertexArray(0);
}

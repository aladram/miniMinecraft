#include <opengl-demo/rendering/renderer.hh>

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

renderer::renderer(world_t& _world)
    : world{_world}
{
    gl_blocks.reserve(256 * 256 * 128);
    gl_leaves.reserve(256 * 256 * 128);
    gl_water.reserve(256 * 256 * 128);

#if !defined(NDEBUG) && OPENGL_VERSION_MAJOR >= 4 && OPENGL_VERSION_MINOR >= 3
    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, 0);
#endif

    glEnable(GL_DEPTH_TEST);

    glEnablei(GL_BLEND, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program = program_t{compile_my_shaders()};
    program_2 = program_t{compile_my_shaders_2()};
    program_3 = program_t{compile_my_shaders_3()};
    program.use();

    world_renderer = block_renderer_t(generate_cube_vao());
    leaves_renderer = block_renderer_t(generate_cube_mirror_vao());
    water_renderer = block_renderer_t(generate_quad_vao());
    log_renderer = block_renderer_t(generate_cube_vao());
    screen_vao = generate_screen_vao();

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

void renderer::resize_framebuffer(bool regenerate)
{
    // Delete previous buffers if existing
    if (regenerate)
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
}

void renderer::render(int new_width, int new_height, camera_t& camera)
{
    assert(new_width >= 0);
    assert(new_height >= 0);

    // Regenerate framebuffer if width or height has changed
    if (new_width != width || new_height != height)
    {
        bool regenerate = width != -1;
        width = new_width;
        height = new_height;

        resize_framebuffer(regenerate);
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
    const bool cached = world.cached;
    if (!cached)
    {
        gl_blocks.clear();
        gl_leaves.clear();
        gl_water.clear();
        gl_log.clear();
        for (auto& chunk_pair: world.chunks)
        {
            // Skip chunk if too far
            vector3 loc = chunk_pair.first * (int)chunk_t::N;
            if (glm::distance(loc, world.player.eyes_position()) > max_render_distance)
                continue;

            auto& chunk = chunk_pair.second;

            // Regenerate chunk GL blocks cache if empty
            if (!chunk.is_cached())
                for (const auto& block: chunk.blocks)
                {
                    if (block.type == block_type::WATER)
                    {
                        // Render only if air on top
                        if (world.get_block(block.position + vector3(0, 1, 0)).type == block_type::AIR)
                            chunk.gl_cache[2].push_back(block.to_opengl());
                    }
                    else if (block.type == block_type::LEAVES)
                        chunk.gl_cache[1].push_back(block.to_opengl());
                    else if (block.type == block_type::LOG)
                        chunk.gl_cache[3].push_back(block.to_opengl());
                    else if (block.visible)
                        chunk.gl_cache[0].push_back(block.to_opengl());
                }

            // Add blocks to VBO
            gl_blocks.insert(gl_blocks.end(), chunk.gl_cache[0].begin(), chunk.gl_cache[0].end());
            gl_leaves.insert(gl_leaves.end(), chunk.gl_cache[1].begin(), chunk.gl_cache[1].end());
            gl_water.insert(gl_water.end(), chunk.gl_cache[2].begin(), chunk.gl_cache[2].end());
            gl_log.insert(gl_log.end(), chunk.gl_cache[3].begin(), chunk.gl_cache[3].end());
        }

        world.cached = true;
    }

    // Texture binding
    glBindTexture(GL_TEXTURE_2D, texture);

    world_renderer.render(gl_blocks, cached);
    leaves_renderer.render(gl_leaves, cached);
    water_renderer.render(gl_water, cached);

    program_3.use();
    program_3.put("view_proj", projection * view);
    program_3.put("ambient_light", vector3(0.7));
    program_3.put("dir_light", - vector3(1.f/2.f, std::sqrt(3.f)/2.f, 0));
    program_3.put("dir_light_color", vector3(0.7));
    program_3.put("tex", 0);
    log_renderer.render(gl_log, cached);

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

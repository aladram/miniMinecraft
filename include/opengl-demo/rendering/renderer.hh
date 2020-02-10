#pragma once

#include <glad/glad.h>

#include <opengl-demo/program.hh>
#include <opengl-demo/camera.hh>
#include <opengl-demo/rendering/block-renderer.hh>
#include <opengl-demo/world/world.hh>

namespace opengl_demo {
    class renderer
    {
public:
        renderer(world_t& _world);
        void resize_framebuffer(bool regenerate);
        void render(int new_width, int new_height, camera_t& camera);

private:
        // Classic class members
        world_t& world;
        int width = -1;
        int height = -1;

        // OpenGL class members
        std::vector<gl_block> gl_blocks;
        std::vector<gl_block> gl_leaves;
        std::vector<gl_block> gl_water;
        std::vector<gl_block> gl_log;
        vector3 last_cached_pos = vector3(0);
        program_t program;
        program_t program_2;
        program_t program_3;
        block_renderer_t world_renderer;
        block_renderer_t leaves_renderer;
        block_renderer_t water_renderer;
        block_renderer_t log_renderer;
        GLuint screen_vao;
        GLuint texture;
        GLuint fbo;
        GLuint depth_rbo;
        GLuint colors_tex;
        GLuint positions_tex;
        GLuint normals_tex;
    };
}

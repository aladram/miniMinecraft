#pragma once

#include <glad/glad.h>

#include <opengl-demo/program.hh>
#include <opengl-demo/world/world.hh>

namespace opengl_demo {
    class renderer
    {
public:
        renderer(const world& _world);
        void render(int width, int height);

private:
        const world_t& world;
        int width_ = -1;
        int height_ = -1;
        program_t program;
        program_t program_2;
        GLuint world_vao;
        GLuint leaves_vao;
        GLuint water_vao;
        GLuint screen_vao;
        GLuint positions_vbo;
        GLuint leaves_positions_vbo;
        GLuint water_positions_vbo;
        GLuint texture;
        GLuint fbo;
        GLuint depth_rbo;
        GLuint colors_tex;
        GLuint positions_tex;
        GLuint normals_tex;
    };
}

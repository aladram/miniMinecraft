#pragma once

#include <glad/glad.h>

#include <opengl-demo/program.hh>
#include <opengl-demo/world/world.hh>

namespace opengl_demo {
    struct renderer
    {
        renderer(const world& _world);
        void render(int width, int height) const;

        const world_t& world;
        program_t program;
        GLuint world_vao;
        GLuint leaves_vao;
        GLuint water_vao;
        GLuint positions_vbo;
        GLuint leaves_positions_vbo;
        GLuint water_positions_vbo;
        GLuint texture;
    };
}

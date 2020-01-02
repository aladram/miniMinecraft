#pragma once

#include <glad/glad.h>

#include <opengl-demo/world/world.hh>

namespace opengl_demo {
    struct renderer
    {
        renderer(const world& _world);
        void render() const;

        const world_t& world;
        GLuint world_vao;
        GLuint leaves_vao;
        GLuint water_vao;
        GLuint positions_vbo;
        GLuint leaves_positions_vbo;
        GLuint water_positions_vbo;
        GLuint texture;
    };
}

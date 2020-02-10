#pragma once

#include <glad/glad.h>

#include <opengl-demo/world/block.hh>

namespace opengl_demo {
    struct block_renderer
    {
        block_renderer() = default;
        explicit block_renderer(GLuint _vao);
        void render(const std::vector<gl_block>& blocks, bool cached) const;

        GLuint vao;
        GLuint vbo;
    };
    using block_renderer_t = block_renderer;
}

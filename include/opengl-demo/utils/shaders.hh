#pragma once

#include <glad/glad.h>

namespace opengl_demo
{
    GLuint compile_shaders(
            const char* vertex_shader_src,
            const char* geometry_shader_src,
            const char* fragment_shader_src
        );
}

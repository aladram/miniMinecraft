#pragma once

#include <glad/glad.h>

#include <opengl-demo/math.hh>
#include <opengl-demo/utils/opengl_utils.hh>

namespace opengl_demo
{
    struct program {
        auto loc(const char* name) const
        {
            auto l = glGetUniformLocation(p_id, name);
            TEST_OPENGL_ERROR();

            return l;
        }

        void use() const
        {
            glUseProgram(p_id);
            TEST_OPENGL_ERROR();
        }

        template <typename T>
        void put(const char* name, const T& val) const;

        GLuint p_id;
    };
    using program_t = program;
}

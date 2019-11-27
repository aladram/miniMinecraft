#pragma once

#include <glad/glad.h>

#include <opengl-demo/utils/opengl_utils.hh>

namespace opengl_demo
{
    struct program {
        auto loc(const char* name)
        {
            auto l = glGetUniformLocation(p_id, name);
            TEST_OPENGL_ERROR();

            return l;
        }

        void use()
        {
            glUseProgram(p_id);
            TEST_OPENGL_ERROR();
        }

        template <typename T>
        void put(const char* name, const T& val);

        GLuint p_id;
    };

    template <>
    void program::put<glm::mat4>(const char* name, const glm::mat4& val)
    {
        glUniformMatrix4fv(loc(name), 1, GL_FALSE, &val[0][0]);
        TEST_OPENGL_ERROR();
    }

    template<>
    void program::put<GLint>(const char* name, const GLint& val)
    {
        glUniform1i(loc(name), val);
        TEST_OPENGL_ERROR();
    }
}

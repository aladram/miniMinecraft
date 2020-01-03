#include <opengl-demo/program.hh>

#include <glad/glad.h>

#include <opengl-demo/math.hh>
#include <opengl-demo/utils/opengl_utils.hh>

using namespace opengl_demo;

template <>
void program::put<glm::vec4>(const char* name, const glm::vec4& val) const
{
    glUniform4fv(loc(name), 1, &val[0]);
    TEST_OPENGL_ERROR();
}

template <>
void program::put<glm::mat4>(const char* name, const glm::mat4& val) const
{
    glUniformMatrix4fv(loc(name), 1, GL_FALSE, &val[0][0]);
    TEST_OPENGL_ERROR();
}

template<>
void program::put<GLint>(const char* name, const GLint& val) const
{
    glUniform1i(loc(name), val);
    TEST_OPENGL_ERROR();
}
